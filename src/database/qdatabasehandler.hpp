#ifndef QDATABASEHANDLER_H
#define QDATABASEHANDLER_H

#include "mysql-cppconn-8/mysqlx/xdevapi.h"
#include <iostream>

using namespace mysqlx;

/**
 * @brief The qDatabaseHandler class
 *  // NOTE TEST: app.cpp
    qDatabaseHandler dbTest("lineah", "admin", "localhost","workshop", 33060);
    dbTest.connect();
    dbTest.showTables();
    dbTest.disconnect();
 */

class qDatabaseHandler
{
public:
    qDatabaseHandler();
    qDatabaseHandler(std::string user, std::string password, std::string host, std::string database, uint32_t port);
    ~qDatabaseHandler();

    Session *connect();
    bool disconnect();

    bool showTables();

private:
    std::string mUser;
    std::string mPassword;
    std::string mHost;
    std::string mDatabase;
    uint32_t mPort;
    SSLMode mSsl_mode;

    Session *mSession = nullptr;
//    Schema *mSchema = nullptr;
};

#endif // QDATABASEHANDLER_H
