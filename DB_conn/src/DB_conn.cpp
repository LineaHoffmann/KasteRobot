#include <iostream>
#include <mysql/mysql.h>
#include "qdatabase.h"

int main(int argc, char** argv)
{
    std::string server = 'localhost';
    char *user = 'root';
    char *pass = 'vrg77pkh';
    char *db = 'testDB';

    qDatabase(server,user,pass,db);
    qDatabase.connectDB();
    std::cout << qDatabase.isConnected << std::endl;


    return 0;
}

