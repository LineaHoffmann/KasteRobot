#include <iostream>
#include <mysql/mysql.h>
#include "qdatabase.h"

int main(int argc, char** argv)
{
    std::string *server = new std::string("localhost");
    std::string *user = new std::string("root");
    std::string *pass = new std::string("vrg77pkh");
    std::string *db = new std::string("testDB");


    qDatabase dbT(server,user,pass,db);
    dbT.connectDB();
    std::cout << dbT.isConnected << std::endl;


    return 0;
}

