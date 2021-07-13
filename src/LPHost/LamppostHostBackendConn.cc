//
// Created by Vito Wu on 2021/7/8.
//

#include "LPHost/LamppostHostBackendConn.hh"

void execute_sql_query(sql::Statement *statement, const std::string &str) {
    try {
        sql::ResultSet *res = statement->executeQuery(str);
        while (res->next()) {
            PRINTF_THREAD_STAMP("\t... MySQL replies:\n");
            PRINTF_THREAD_STAMP("%s\n", res->getString("_message").c_str());
        }
        delete res;
    } catch (sql::SQLException &e) {
        PRINTF_THREAD_STAMP("Error: SQLException raised\n");
        PRINTF_THREAD_STAMP("\tWhat: %s\n", e.what());
        PRINTF_THREAD_STAMP("\tMySQL error code: %d\n", e.getErrorCode());
        PRINTF_THREAD_STAMP("\tSQLState: %s\n", e.getSQLState().c_str());
    }
}

void initialize_database(LamppostHostProg *prog) {
    sql::Driver *driver;
    sql::Connection *conn;
    sql::Statement *statement;
    driver = get_driver_instance();
    conn = driver->connect(prog->options.DB_HOST, prog->options.DB_USER, prog->options.DB_PASSWD);
    conn->setSchema(prog->options.DB_DATABASE);
    statement = conn->createStatement();

    PRINTF_THREAD_STAMP("initialize RoadBlock table\n");
    statement->execute("DROP TABLE IF EXISTS " + detected_road_blocks_table);
    statement->execute("CREATE TABLE IF NOT EXISTS `" + detected_road_blocks_table +
                       "` (`x` DOUBLE NOT NULL, "
                       "`y` DOUBLE NOT NULL, "
                       "`ref_id` INT UNSIGNED NOT NULL, "
                       "`last_seen` DATETIME NOT NULL, "
                       "PRIMARY KEY (`ref_id`)) ENGINE=InnoDB DEFAULT CHARSET=utf8;");

    PRINTF_THREAD_STAMP("initialize Route table\n");
    statement->execute("DROP TABLE IF EXISTS " + planned_route_table);
    statement->execute("CREATE TABLE IF NOT EXISTS `" + planned_route_table +
                       "` (`x` DOUBLE NOT NULL, "
                       "`y` DOUBLE NOT NULL, "
                       "`pace_id` INT UNSIGNED AUTO_INCREMENT, "
                       "PRIMARY KEY (`pace_id`)) ENGINE=InnoDB DEFAULT CHARSET=utf8;");

    PRINTF_THREAD_STAMP("initialize Lamppost table\n");
    statement->execute("DROP TABLE IF EXISTS " + alive_lamppost_table);
    statement->execute("CREATE TABLE IF NOT EXISTS `" + alive_lamppost_table +
                       "` (`addr` VARCHAR(100) NOT NULL,"
                       "`last_seen` DATETIME NOT NULL,"
                       "PRIMARY KEY (`addr`)) ENGINE=InnoDB DEFAULT CHARSET=utf8;");

    conn->close();
    delete statement;
    delete conn;
}


// thread to connect with the database and upload alive lamppost and detected roadblocks
void *LamppostHostBackendThread(void *vargs) {
    auto args = (LamppostHostBackendArgs_t *) vargs;
    auto prog = args->prog;

    sql::Driver *driver;
    sql::Connection *conn;
    sql::Statement *statement;

    PRINTF_THREAD_STAMP("Initializing database...\n");
    initialize_database(prog);
    driver = get_driver_instance();
    conn = driver->connect(prog->options.DB_HOST, prog->options.DB_USER, prog->options.DB_PASSWD);
    conn->setSchema(prog->options.DB_DATABASE);
    statement = conn->createStatement();

    while (!test_cancel(&prog->term_mutex, &prog->term_flag)) {
        time_t now = time(nullptr);

        // examine the outdated roadblocks
        // each road block must be tagged with a reference id and we update the location of referenced roadblocks
        PRINTF_THREAD_STAMP("update roadblocks\n");
        {
            std::unique_lock<std::mutex> lock(prog->crb_mutex);
            for (int i = 0; i < prog->CollectedRBCoordinates.size(); i++) {
                auto x_str = std::to_string(prog->CollectedRBCoordinates[i].first.x);
                auto y_str = std::to_string(prog->CollectedRBCoordinates[i].first.y);
                auto ref_str = std::to_string(prog->CollectedRBCoordinates[i].first.marker_id);
                if (now - prog->CollectedRBCoordinates[i].second >= RB_ALIVE_INTERVAL) {
                    PRINTF_THREAD_STAMP(
                            "Lost detection of road block %d for %d seconds, remove it from roadblock list\n",
                            prog->CollectedRBCoordinates[i].first.marker_id, RB_ALIVE_INTERVAL);
                    prog->CollectedRBCoordinates.erase(prog->CollectedRBCoordinates.begin() + i);
                    // erase in the SQL database
                    statement->execute("DELETE FROM " + detected_road_blocks_table +
                                       " WHERE ref_id=" +
                                       std::to_string(prog->CollectedRBCoordinates[i].first.marker_id));
                } else {
                    // road block is still in the screen, update data in the SQL database
                    statement->execute("REPLACE INTO " + detected_road_blocks_table +
                                       " (x, y, ref_id, last_seen) " +
                                       "VALUES (" + x_str + ", " + y_str + ", " + ref_str + ", NOW());");
                }
            }
            lock.unlock();
        }

        {
            PRINTF_THREAD_STAMP("update lamppost\n");
            // examine the outdated alive lampposts
            std::unique_lock<std::mutex> lock(prog->lal_modify_mutex);
            for (int i = 0; i < prog->LamppostAliveList.size(); i++) {
                if (now - prog->LamppostAliveList[i].second >= BACKBONE_ALIVE_INTERVAL) {
                    PRINTF_THREAD_STAMP("Node %s has been disconnected for %d seconds, remove it from alive list\n",
                                        bats_addr_to_str(prog->LamppostAliveList[i].first).c_str(),
                                        BACKBONE_ALIVE_INTERVAL);
                    prog->LamppostAliveList.erase(prog->LamppostAliveList.begin() + i);
                    // erase int the SQL database
                    statement->execute("DELETE FROM " + alive_lamppost_table +
                                       " WHERE addr='" + bats_addr_to_str(prog->LamppostAliveList[i].first) + "';");
                } else {
                    // lamppost is still alive, update data in the SQL database
                    statement->execute("REPLACE INTO " + alive_lamppost_table + "(addr, last_seen) VALUES" +
                                       " (\"" + bats_addr_to_str(prog->LamppostAliveList[i].first) + "\", NOW());");
                }
            }
            if (!prog->LamppostAliveList.empty()) {
                PRINTF_STAMP("Alive lampposts: ");
                for (auto &i: prog->LamppostAliveList) {
                    std::cout << i.first << " ";
                }
                std::cout << std::endl;
            }
            lock.unlock();
        }

        sleep(1);
    }

    delete conn;
    delete statement;
}