#include <iostream>
#include <mysql/mysql.h>


/**
 * @brief The connection_details struct
 * Struct that contains host, username, password etc.
 */
struct connection_details
{
    char *server;
    char *user;
    char *password;
    char *database;
};

/**
 * @brief mysql_connection_setup
 * @param mysql_details
 * @return MYSQL pointer to the new connection
 */

MYSQL* mysql_connection_setup(struct connection_details mysql_details)
{
    MYSQL *connection = mysql_init(NULL);

    if(!mysql_real_connect(connection,mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, 0, NULL, 0))
    {
        printf("Connection error : %s\n", mysql_error(connection));
        exit(1);
    }
    return connection;
}


MYSQL_RES* mysql_peform_query(MYSQL *connection, char *sql_query)
{
    //send the query to the database
    if(mysql_query(connection, sql_query))
    {
        printf("MySQL qyery error : %s\n", mysql_error(connection));
        exit(1);
    }

    return mysql_use_result(connection);
}

int main(int argc, char** argv)
{
    std::cout << "Creating a conenction to DB" << std::endl;

    MYSQL *conn; //the connection
    MYSQL_RES *res; // the results
    MYSQL_ROW row; //the results row (line by line)

    struct connection_details mysqlID;
    mysqlID.server = "localhost";
    mysqlID.user = "root";
    mysqlID.password = "vrg77pkh";
    mysqlID.database = "testDB";

    // connect to mysql database
    conn = mysql_connection_setup(mysqlID);

    // assign the results return to the MYSQL_RES pointer
    res = mysql_peform_query(conn, "SHOW DATABASES"); // Make another class for queryies, ot txt file.

    printf("MySQL Tables in mysql database: \n");
    while ((row = mysql_fetch_row(res)) !=NULL)
        printf(("%s\n", row[0]));

    mysql_free_result(res);
    mysql_close(conn);

    return 0;
}

