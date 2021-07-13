//
// Created by Vito Wu on 2021/7/8.
//

#ifndef LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTBACKENDCONN_HH
#define LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTBACKENDCONN_HH

#include "LamppostHostUtils.hh"
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

typedef struct {
    LamppostHostProg *prog;
} LamppostHostBackendArgs_t;

void* LamppostHostBackendThread(void *vargs);

#endif //LAMPPOSTAUTOCARDEMO_LAMPPOSTHOSTBACKENDCONN_HH
