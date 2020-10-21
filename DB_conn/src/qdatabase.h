#ifndef QDATABASE_H
#define QDATABASE_H

#include <mysql/mysql.h>


class qDatabase
{

    bool disconnectDB();
//    void addEntry(qEntry);
//    void addQLinker(qLinker*);

public:

    qDatabase(); // Default constructor
    qDatabase(std::string *server, std::string *user, std::string *password, std::string *database); //Constructor overloading

    ~qDatabase(); // Destructor

    bool connectDB();

    char *getServer() const;
    void setServer(char *value);

    char *getUser() const;
    void setUser(char *value);

    char *getPassword() const;
    void setPassword(char *value);

    char *getDatabase() const;
    void setDatabase(char *value);

    bool isConnected; //flag to check

private:
    //    std::thread *mThread = nullptr;
    //    qLinker *mQLinker;


    MYSQL *mMysql = nullptr;

//Pointers becuase long inputs, and can check if empty.
// Able to use for GUI input.
    // TODO Why char ?? Can we use something else ?
    std::string *mServer = nullptr;
    std::string *mUser = nullptr;
    std::string *mPassword = nullptr;
    std::string *mDatabase = nullptr;

};

#endif // QDATABASE_H
