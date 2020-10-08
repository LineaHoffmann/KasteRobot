#ifndef QDATABASE_H
#define QDATABASE_H

#include <mysql/mysql.h>


class qDatabase
{
//    isconnected(); //TODO Is this nessesary
    MYSQL *connectDB();
//    bool disconnectDB();
//    void addEntry(qEntry);
//    void addQLinker(qLinker*);

public:
    qDatabase();

private:
//    std::thread *mThread = nullptr;
//    qLinker *mQLinker;
};

#endif // QDATABASE_H
