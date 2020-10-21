#ifndef QDATABASEHANDLER_H
#define QDATABASEHANDLER_H

#include "../includeheader.h"

class qDatabaseHandler
{

public:
    qDatabaseHandler();
   ~qDatabaseHandler();

private:
    std::thread *mThread = nullptr;
    qLinker *mQLinker;

    MYSQL *mMysql = nullptr;

    char *mServer;
    char *mUser;
    char *password;
    char *mDatabase;
    int *mPort;

};

#endif // QDATABASEHANDLER_H
