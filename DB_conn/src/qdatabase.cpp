#include "qdatabase.h"
#include <stdio.h>
#include <mysql/mysql.h>


/**
  * @brief qDatabase::connectDB
  * @return MySQL connection, Pointer.
  */
 bool qDatabase::connectDB()
{
     mMysql = mysql_init(NULL); // initialisere mySQL pointer til null.

     if(!mysql_real_connect(mMysql,mServer,mUser, mPassword, mDatabase, 0, NULL, 0))
     {
         printf("Connection to database failed: %s\n", mysql_error(mMysql));
         return(1);
     }
     mMysql = mysql_real_connect(mMysql, mServer,mUser, mPassword,mDatabase,0,NULL,0);
     isConnected = true;
     return true;
 }

 /**
  * @brief qDatabase::disconnectDB
  * @return
  */
 bool qDatabase::disconnectDB()
{
     if(!isConnected)
     {
         printf("Database is already disconnected\n");
     }
     else
     {
         //Aware that we do not know it reacts if already disconnected
         mysql_close(mMysql);
         isConnected = false;
         return true;


     }
}

//void qDatabase::addEntry(qEntry)
//{

//}

//void qDatabase::addQLinker(qLinker *)
//{

//}

//qDatabase::qDatabase()
//{

//}


 /**
  * @brief qDatabase::qDatabase
  * @param server
  * @param user
  * @param password
  * @param database
  */
 qDatabase::qDatabase(std::string *server, std::string *user, std::string *password, std::string *database)
    /* :mServer{server}, mUser{user}, mPassword{password}, mDatabase{database}  */ //init list of DB details
 {
    mServer=new std::string(server);
    mUser = new std::string(user);
    mPassword = new std::string(password);
    mDatabase = new std::string(database);


 }

 qDatabase::~qDatabase()
 {
     if(!mServer)
         delete mServer;
     if(!mUser)
         delete mUser;
     if(!mPassword)
         delete mPassword;
     if(!mDatabase)
         delete mDatabase;
     if(!mMysql)
         delete mMysql;
 }

 char *qDatabase::getServer() const
 {
     return mServer;
 }

 void qDatabase::setServer(char *value)
 {
     mServer = value;
 }

 char *qDatabase::getUser() const
 {
     return mUser;
 }

 void qDatabase::setUser(char *value)
 {
     mUser = value;
 }

 char *qDatabase::getPassword() const
 {
     return mPassword;
 }

 void qDatabase::setPassword(char *value)
 {
     mPassword = value;
 }

 char *qDatabase::getDatabase() const
 {
     return mDatabase;
 }

 void qDatabase::setDatabase(char *value)
 {
     mDatabase = value;
 }

