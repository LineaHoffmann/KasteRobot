#ifndef QDATABASEHANDLER_H
#define QDATABASEHANDLER_H

#include "mysql-cppconn-8/mysqlx/xdevapi.h"
#include <iostream>
#include <vector>

using namespace mysqlx;

class qDatabaseHandler
{
public:
    qDatabaseHandler();
    qDatabaseHandler(std::string user, std::string password, std::string host, std::string database, uint32_t port);
    ~qDatabaseHandler();

    Session *connect();
    bool disconnect();

    std::vector<Row>* showTables();
    bool qInsert();

private:
    std::string mUser;
    std::string mPassword;
    std::string mHost;
    std::string mDatabase;
    uint32_t mPort;
    SSLMode mSsl_mode;

    Session *mSession = nullptr;
    Schema *mSchema = nullptr;
    Table *mTable = nullptr;
    std::vector<Row> *mRes;
};

#endif // QDATABASEHANDLER_H
