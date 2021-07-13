//
// Created by Vito Wu on 2021/7/8.
//

#include "LPHost/LamppostHostBackendConn.hh"

void execute_sql(sql::Statement *statement, const std::string &str) {
    sql::ResultSet *res = statement->executeQuery(str);
    while (res->next()) {
        PRINTF_THREAD_STAMP("\t... MySQL replies:\n");
        PRINTF_THREAD_STAMP("%s\n", res->getString("_message").c_str());
    }
    delete res;
}

void initialize_database(LamppostHostProg *prog) {
    sql::Driver *driver;
    sql::Connection *conn;
    sql::Statement *statement;
    try {
        driver = get_driver_instance();
        conn = driver->connect(prog->options.DB_HOST, prog->options.DB_USER, prog->options.DB_PASSWD);
        conn->setSchema(prog->options.DB_DATABASE);
        statement = conn->createStatement();

        PRINTF_THREAD_STAMP("initialize RoadBlock table\n");
        execute_sql(statement,
                    "CREATE TABLE IF NOT EXISTS `" + detected_road_blocks_table +
                    "` (`x` DOUBLE NOT NULL,"
                    "`y` DOUBLE NOT NULL,"
                    "`ref_id` INT UNSIGNED NOT NULL,"
                    "`last_seen` DATETIME NOT NULL,"
                    "PRIMARY KEY (`ref_id`));");

        PRINTF_THREAD_STAMP("initialize Route table\n");
        execute_sql(statement,
                    "CREATE TABLE IF NOT EXISTS `" + planned_route_table +
                    "` (`x` DOUBLE NOT NULL,"
                    "`y` DOUBLE NOT NULL,"
                    "`pace_id` INT UNSIGNED AUTO_INCREMENT,"
                    "PRIMARY KEY (`pace_id`));");

        PRINTF_THREAD_STAMP("initialize Lamppost table\n");
        execute_sql(statement,
                    "CREATE TABLE IF NOT EXISTS `" + alive_lamppost_table +
                    "` (`addr` VARCHAR(100) NOT NULL,"
                    "`last_seen` DATETIME NOT NULL,"
                    "PRIMARY KEY (`addr`));");

        delete statement;
        delete conn;
    } catch (sql::SQLException &e) {
        PRINTF_THREAD_STAMP("Error: SQLException raised\n");
        PRINTF_THREAD_STAMP("\tWhat: %s\n", e.what());
        PRINTF_THREAD_STAMP("\tMySQL error code: %d\n", e.getErrorCode());
        PRINTF_THREAD_STAMP("\tSQLState: %s\n", e.getSQLState().c_str());
    }
}

void initialize_connection(sql::Driver *driver, sql::Connection *conn,
                           sql::Statement *statement,
                           LamppostHostProg *prog) {
    try {
        driver = get_driver_instance();
        conn = driver->connect(prog->options.DB_HOST, prog->options.DB_USER, prog->options.DB_PASSWD);
        statement = conn->createStatement();

    } catch (sql::SQLException &e) {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        pthread_exit(nullptr);
    }
}

// thread to connect with the database and upload alive lamppost and detected roadblocks
void LamppostHostBackendThread(void *vargs) {
    auto args = (LamppostHostBackendArgs_t *) vargs;
    auto prog = args->prog;

    sql::Driver *driver;
    sql::Connection *conn;
    sql::Statement *statement;

    PRINTF_THREAD_STAMP("Initializing database...\n");
    initialize_database(prog);
    initialize_connection(driver, conn, statement, prog);

    while (!test_cancel(&prog->term_mutex, &prog->term_flag)) {
        time_t now = time(nullptr);
        auto now_str = std::to_string(now);

        // examine the outdated roadblocks
        // each road block must be tagged with a reference id and we update the location of referenced roadblocks
        std::unique_lock<std::mutex> lock(prog->crb_mutex);
        for (int i = 0; i < prog->CollectedRBCoordinates.size(); i++) {
            auto x_str = std::to_string(prog->CollectedRBCoordinates[i].first.x);
            auto y_str = std::to_string(prog->CollectedRBCoordinates[i].first.y);
            auto ref_str = std::to_string(prog->CollectedRBCoordinates[i].first.marker_id);
            if (now - prog->CollectedRBCoordinates[i].second >= RB_ALIVE_INTERVAL) {
                PRINTF_THREAD_STAMP("Lost detection of road block %d for %d seconds, remove it from roadblock list\n",
                                    prog->CollectedRBCoordinates[i].first.marker_id, RB_ALIVE_INTERVAL);
                prog->CollectedRBCoordinates.erase(prog->CollectedRBCoordinates.begin() + i);
                // erase in the SQL database
                execute_sql(statement,
                            "DELETE FROM " + detected_road_blocks_table +
                            " WHERE ref_id=" + std::to_string(prog->CollectedRBCoordinates[i].first.marker_id));
            } else {
                // road block is still in the screen, update data in the SQL database
                execute_sql(statement,
                            "INSERT INTO " + detected_road_blocks_table +
                            " (" + x_str + ", " + y_str + ", " + ref_str + ", FROM_UNIXTIME(" + now_str + ")) " +
                            "ON DUPLICATE KEY UPDATE " +
                            " x=" + x_str + ", " + " y=" + y_str + ", " + " last_seen=FROM_UNIXTIME(" + now_str + ");");
            }
        }
        lock.unlock();

        // examine the outdated alive lampposts
        pthread_mutex_lock(&prog->lal_modify_mutex);
        for (int i = 0; i < prog->LamppostAliveList.size(); i++) {
            if (now - prog->LamppostAliveList[i].second >= BACKBONE_ALIVE_INTERVAL) {
                PRINTF_THREAD_STAMP("Node %s has been disconnected for %d seconds, remove it from alive list\n",
                                    bats_addr_to_str(prog->LamppostAliveList[i].first).c_str(),
                                    BACKBONE_ALIVE_INTERVAL);
                prog->LamppostAliveList.erase(prog->LamppostAliveList.begin() + i);
                // erase int the SQL database
                execute_sql(statement,
                            "DELETE FROM " + alive_lamppost_table +
                            " WHERE addr=" + bats_addr_to_str(prog->LamppostAliveList[i].first));
            } else {
                // lamppost is still alive, update data in the SQL database
                execute_sql(statement,
                            "INSERT INTO " + alive_lamppost_table +
                            " (" + bats_addr_to_str(prog->LamppostAliveList[i].first) + ", FROM_UNIX_TIME(" + now_str +
                            ")) ON DUPLICATE KEY UPDATE " +
                            " last_seen=FROM_UNIX_TIME(" + now_str + ");");
            }
        }
        if (!prog->LamppostAliveList.empty()) {
            PRINTF_STAMP("Alive lampposts: ");
            for (auto &i: prog->LamppostAliveList) {
                std::cout << i.first << " ";
            }
            std::cout << std::endl;
        }
        pthread_mutex_unlock(&prog->lal_modify_mutex);


        usleep(500);
    }

    delete conn;
    delete statement;
}