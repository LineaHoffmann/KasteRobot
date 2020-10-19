#ifndef QDATABASE_H
#define QDATABASE_H

#include <mysql/mysql.h>


class qDatabase
{
    bool connectDB();
    bool disconnectDB();
//    void addEntry(qEntry);
//    void addQLinker(qLinker*);

public:
    qDatabase(); // Default constructor
    qDatabase(char *server, char *user, char *password, char *database); //Constructor overloading

    ~qDatabase(); // Destructor

    char *getServer() const;
    void setServer(char *value);

    char *getUser() const;
    void setUser(char *value);

    char *getPassword() const;
    void setPassword(char *value);

    char *getDatabase() const;
    void setDatabase(char *value);

private:
    //    std::thread *mThread = nullptr;
    //    qLinker *mQLinker;


    MYSQL *mMysql = nullptr;

//Pointers becuase long inputs, and can check if empty.
// Able to use for GUI input.
    // TODO Why char ?? Can we use something else ?
    char *mServer = nullptr;
    char *mUser = nullptr;
    char *mPassword = nullptr;
    char *mDatabase = nullptr;

    bool isConnected; //flag to check

};

#endif // QDATABASE_H
