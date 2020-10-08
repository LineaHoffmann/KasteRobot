#include "qdatabase.h"
#include <mysql/mysql.h>

 MYSQL *qDatabase::connectDB()
{
     //Connection details
     // IDEA: Make in qDatabase constructor ??
     char *sever = "localhost";
     char *user = "root";
     char *password = "vrg77pkh";
     char *database = "testDB";

     MYSQL *connection = mysql_init(NULL);

     // TODO Should be changed from GUI Input.
     if(!mysql_real_connect(connection,"localhost","root", "vrg77pkh", "testDB", 0, NULL, 0))
     {
         printf("Connection to database failed: %s\n", mysql_error(connection));
         exit(1);
     }

     connection = mysql_real_connect(connection, sever,user, password,
                                     database,0,NULL,0);


     //Test:


}

//bool qDatabase::disconnectDB()
//{

//}

//void qDatabase::addEntry(qEntry)
//{

//}

//void qDatabase::addQLinker(qLinker *)
//{

//}

//qDatabase::qDatabase()
//{

//}
