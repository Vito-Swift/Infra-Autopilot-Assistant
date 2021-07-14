#ifndef LAMPPOSTAUTOCARDEMO_CTRLGENERAL_HH
#define LAMPPOSTAUTOCARDEMO_CTRLGENERAL_HH

#include "utils.hh"
#include "CtrlPathSearch.hh"
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

// The general flow of control module goes as following:
//      1. `lmpctl` receives demo command from user
//
//      2. `lmpctl` prepares mapinfo_t and SQL connection
//         to the ControlManager_t
//
//      3. generate a binary roadblock map with size
//              (ActualHeight / SizeOfGrid) * (ActualWidth / SizeOfGrid)
//         where the region of roadblocks are marked with `0`
//
//      4. generate a path by A* algorithm and refine the path by
//          gradient descent algorithm;
//
//      5. (python script) Launch the robomaster to move along the route.

namespace Control {

    typedef struct {
        std::pair<double, double> lt_point;        // top left coordinate of map (origin point)
        double width;                              // width of safe bound
        double height;                             // height of safe bound
        double grid_size;                          // grid size of the map (normally set to 0.1)
    } mapinfo_t;

    typedef struct {
        mapinfo_t mapinfo;
        std::vector<std::pair<double, double>> roadblocks;
        int **map;
        std::vector<std::pair<double, double>> plannedRoute;
        int map_col_count;
        int map_row_count;
        std::pair<double, double> init_point;
        std::pair<double, double> dest_point;
        sql::Connection *conn;
        std::string db;
    } ControlManager_t;

    void generate_roadblock_map(ControlManager_t *cm);

    bool generate_path(ControlManager_t *cm);

    void start_robomaster(ControlManager_t *cm);
}

#endif //LAMPPOSTAUTOCARDEMO_CTRLGENERAL_HH
