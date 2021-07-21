//
// Created by Vito Wu on 2021/7/13.
//

#include "Control/CtrlGeneral.hh"

namespace Control {
    std::pair<int, int> world_coord_to_map(double x, double y, mapinfo_t mapinfo) {
        return {(x - mapinfo.lt_point.first) / mapinfo.grid_size,
                (y - mapinfo.lt_point.second) / mapinfo.grid_size};
    }

    std::pair<double, double> map_coord_to_world(int x, int y, mapinfo_t mapinfo) {
        return {((x + mapinfo.lt_point.first) * mapinfo.grid_size),
                ((y + mapinfo.lt_point.second) * mapinfo.grid_size)};
    }

    void generate_roadblock_map(ControlManager_t *cm) {
        cout << "Analyzing environments and generating road map" << endl;

        // calculate map size and allocate memory
        cm->map_row_count = floor(cm->mapinfo.height / cm->mapinfo.grid_size);
        cm->map_col_count = floor(cm->mapinfo.width / cm->mapinfo.grid_size);
        cm->map = (int **) malloc(cm->map_row_count * sizeof(int *));
        for (int i = 0; i < cm->map_row_count; i++)
            cm->map[i] = SMALLOC(int, cm->map_col_count);

        // reset map to all 1
        for (int i = 0; i < cm->map_row_count; i++)
            for (int j = 0; j < cm->map_col_count; j++)
                cm->map[i][j] = 1;

        // todo: filter out outdated roadblocks
        // connect to sql database and get the coordinates of detected roadblocks
        cm->conn->setSchema(cm->db);
        sql::ResultSet *resultSet;
        sql::PreparedStatement *pstmt = cm->conn->prepareStatement(
                "SELECT x,y,last_seen FROM " + detected_road_blocks_table + " INNER JOIN " +
                "(SELECT MAX(row_id) as row_id FROM " + detected_road_blocks_table + " GROUP BY ref_id) last_updates" +
                " ON last_updates.row_id = " + detected_road_blocks_table + ".row_id");
        resultSet = pstmt->executeQuery();
        while (resultSet->next()) {
            auto x = resultSet->getDouble("x");
            auto y = resultSet->getDouble("y");
            auto last_seen = std::istringstream(resultSet->getString("last_seen"));
            std::tm t{};
            last_seen >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
            if (last_seen.fail()) {
                throw std::runtime_error{"failed to parse time string"};
            }
            time_t row_time = mktime(&t);
            double diff_time = difftime(time(0), row_time);
            if (diff_time <= RB_ALIVE_INTERVAL)
                cm->roadblocks.emplace_back(std::pair<double, double>(x, y));
        }
        delete pstmt;

        // fill in the map where the roadblock are placed in
        for (auto &roadblock: cm->roadblocks) {
            cout << "roadblock at: " << roadblock.first << ", " << roadblock.second << endl;
            // iterate over all grids and check whether the grid is within the
            // region of the roadblock
            for (int i = 0; i < cm->map_row_count; i++)
                for (int j = 0; j < cm->map_col_count; j++) {
                    auto world_coord = map_coord_to_world(i, j, cm->mapinfo);
                    if (calculateDistance(world_coord, roadblock) <= RB_SAFE_BOUNDING) {
                        cm->map[i][j] = 0;
                    }
                }
        }
    }

    void refine_path(std::vector<std::pair<double, double>> &plannedRoute) {
        // refine AStar path using gradient descent algorithm
        // reference: https://medium.com/@jaems33/understanding-robot-motion-path-smoothing-5970c8363bc4
        const float weight_data = 0.5;      // how much weight to update the data
        const float weight_smooth = 0.5;    // how much weight to smooth the path
        const double tolerance = 0.01; // how much change per iteration is necessary to keep iterating.

        // copy the planned route to the new one
        std::vector<std::pair<double, double>> newRoute(plannedRoute);
        int dims = 2;
        double change = tolerance;

        while (change >= tolerance) {
            change = 0.0;
            for (int i = 1; i < newRoute.size() - 1; i++) {
                // calculate dim1
                double x_i = plannedRoute[i].first;
                double y_i = newRoute[i].first;
                double y_prev = newRoute[i - 1].first;
                double y_next = newRoute[i + 1].first;
                double y_i_saved = y_i;
                y_i += weight_data * (x_i - y_i) + weight_smooth * (y_next + y_prev - (2 * y_i));
                newRoute[i].first = y_i;
                change += abs(y_i - y_i_saved);

                // calculate dim2
                x_i = plannedRoute[i].second;
                y_i = newRoute[i].second;
                y_prev = newRoute[i - 1].second;
                y_next = newRoute[i + 1].second;
                y_i_saved = y_i;
                y_i += weight_data * (x_i - y_i) + weight_smooth * (y_next + y_prev - (2 * y_i));
                newRoute[i].second = y_i;
                change += abs(y_i - y_i_saved);
            }
        }

        // copy back the refined route
        plannedRoute.assign(newRoute.begin(), newRoute.end());
    }

    bool generate_path(ControlManager_t *cm) {
        cout << "Generating Route" << endl;

        // get initialize point and dest point
        pair<double, double> init_point = {5, 0};
        auto map_coord_start = world_coord_to_map(init_point.first, init_point.second, cm->mapinfo);
        auto map_coord_dest = world_coord_to_map(cm->dest_point.first, cm->dest_point.second, cm->mapinfo);
        cout << "\t from: (" << init_point.first << ", " << init_point.second << ") to ("
             << cm->dest_point.first << ", " << cm->dest_point.second << ")" << endl;
        cout << "\t transformed map from: (" << map_coord_start.first << ", " << map_coord_start.second << ") to ("
             << map_coord_dest.first << ", " << map_coord_dest.second << ")" << endl;

        std::vector<std::pair<int, int>> plannedRoute;
        if (!aStarSearch(cm->map, cm->map_col_count, cm->map_row_count,
                         map_coord_start, map_coord_dest,
                         plannedRoute))
            return false;

        cm->plannedRoute.clear();
        for (auto &pace : plannedRoute) {
            cm->plannedRoute.push_back(map_coord_to_world(pace.first, pace.second, cm->mapinfo));
        }

        cout << "Optimizing generated route" << endl;
        refine_path(cm->plannedRoute);
        printf("\nOptimized route is:");
        for (auto &pace : cm->plannedRoute) {
            printf(" -> (%lf, %lf)", pace.first, pace.second);
        }
        printf("\n");

        cout << "Route is generated successfully" << endl;
        return true;
    }

    void store_path_to_server(ControlManager_t *cm) {
        cout << "Uploading planned path to the database" << endl;
        cm->conn->setSchema(cm->db);

        // reset SQL table
        sql::Statement *statement = cm->conn->createStatement();
        statement->execute("DROP TABLE IF EXISTS " + planned_route_table);
        statement->execute("CREATE TABLE IF NOT EXISTS `" + planned_route_table +
                           "` (`x` DOUBLE NOT NULL, "
                           "`y` DOUBLE NOT NULL, "
                           "`pace_id` INT UNSIGNED AUTO_INCREMENT, "
                           "PRIMARY KEY (`pace_id`)) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
        delete statement;

        // insert planned path into the sql database
        sql::PreparedStatement *preparedStatement =
                cm->conn->prepareStatement("INSERT INTO " + planned_route_table + " (x, y) " +
                                           "VALUES (?, ?)");
        for (int i = 0; i < cm->plannedRoute.size(); i++) {
            auto &pace = cm->plannedRoute[i];
            preparedStatement->setDouble(1, pace.first);
            preparedStatement->setDouble(2, pace.second);
            preparedStatement->execute();
        }
        delete preparedStatement;

        // free map memory
        for (int i = 0; i < cm->map_row_count; i++)
            SFREE(cm->map[i]);
        SFREE(cm->map);
    }

    void start_robomaster(ControlManager_t *cm) {

    }
}