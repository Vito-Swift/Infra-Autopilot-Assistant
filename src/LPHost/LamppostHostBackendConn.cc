//
// Created by Vito Wu on 2021/7/8.
//

#include "LPHost/LamppostHostBackendConn.hh"


// todo: implement thread to connect with the database and upload alive lamppost and detected roadblocks
void LamppostHostBackendThread(void *vargs) {
    auto args = (LamppostHostBackendArgs_t *) vargs;
    auto prog = args->prog;

    sql::Driver *driver;
    sql::Connection *conn;
    sql::Statement *statement;
    sql::ResultSet *res;

    while (!test_cancel(&prog->term_mutex, &prog->term_flag)) {
        time_t now = time(nullptr);

        // todo: examine the outdated roadblocks
        std::unique_lock<std::mutex> lock(prog->crb_mutex);
        for (int i = 0; i < prog->CollectedRBCoordinates.size(); i++) {
            if (now - prog->LamppostAliveList[i].second >=)
        }
        lock.unlock();

        // examine the outdated alive lampposts
        pthread_mutex_lock(&prog->lal_modify_mutex);
        for (int i = 0; i < prog->LamppostAliveList.size(); i++) {
            if (now - prog->LamppostAliveList[i].second >= BACKBONE_ALIVE_INTERVAL) {
                PRINTF_THREAD_STAMP("Node %d has been disconnected for %d seconds, remove it from alive list\n",
                                    prog->LamppostAliveList[i].first, BACKBONE_ALIVE_INTERVAL);
                prog->LamppostAliveList.erase(prog->LamppostAliveList.begin() + i);
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

        // todo: insert the data into the SQL server

        usleep(500);
    }
}