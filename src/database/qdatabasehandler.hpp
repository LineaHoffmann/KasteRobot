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
    ~qDatabaseHandler();

    Session *connect();
    void disconnect();

    void setDatabaseCredentials(std::tuple<std::string, std::string, std::string, std::string, uint32_t> credentialsInput);


    std::vector<Row>* showTables();
    bool qInsert();

    std::vector<Row>* getDbData(std::string tableName);
    //bool writeSql(std::vector inputList); //WARNING must add type specifier for the vector, therefor commented.


private:
    std::string mUser;
    std::string mPassword;
    std::string mHost;
    std::string mDatabase;
    uint32_t mPort;
    SSLMode mSsl_mode = SSLMode::DISABLED;

    Session *mSession = nullptr;
    Schema *mSchema = nullptr;
    Table *mTable = nullptr;
    std::vector<Row> *mRes;
};

#endif // QDATABASEHANDLER_H
