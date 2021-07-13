//
// Created by Vito Wu on 2021/7/6.
//

#include <iostream>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>

#define DBHOST "tcp://10.26.1.16:3306"
#define USER "lamppost-autopilot"
#define PASSWORD "netcod99"
#define DATABASE "lamppost_runtime"

int main() {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *statement;
    sql::ResultSet *res;

    try {
        driver = get_driver_instance();
        con = driver->connect(DBHOST, USER, PASSWORD);
        con->setSchema("lamppost_runtime");
        statement = con->createStatement();
        res = statement->executeQuery("SELECT 'Hello World!' AS _message");
        while (res->next()) {
            std::cout << "\t... MySQL replies: ";
            std::cout << res->getString("_message") << std::endl;
            std::cout << "\t... MySQL says it again: ";
            /* Access column fata by numeric offset, 1 is the first column */
            std::cout << res->getString(1) << std::endl;
        }
        delete res;
        delete statement;
        delete con;
    } catch (sql::SQLException &e) {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }
    return 0;
}