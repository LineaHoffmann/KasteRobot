#include "qdatabasehandler.hpp"



qDatabaseHandler::qDatabaseHandler()
{

}



qDatabaseHandler::qDatabaseHandler(std::string user, std::string password, std::string host, std::string database, uint32_t port) : qDatabaseHandler()
{
    mUser = user; mPassword = password; mHost = host; mDatabase = database; mPort = port;
    mSsl_mode = SSLMode::DISABLED;

}

qDatabaseHandler::~qDatabaseHandler()
{

}

Session *qDatabaseHandler::connect()
{
    try {
        std::cout << "Connecting to MySQL server with:" << std::endl;
        std::cout << mUser << ":" << mPassword << "@" << mHost << ":" << mPort << std::endl;;
        std::cout << "on database: " << mDatabase << std::endl;
        if (mSsl_mode == SSLMode::DISABLED) std::cout << "SSL is disabled." << std::endl;
        mSession = new Session(
                    SessionOption::USER, mUser,
                    SessionOption::PWD, mPassword,
                    SessionOption::HOST, mHost,
                    SessionOption::PORT, mPort,
                    SessionOption::DB, mDatabase,
                    SessionOption::SSL_MODE, mSsl_mode
                    );
        std::cout << "qDatabaseHandler: Session accepted ..." << std::endl;
        //NOTE Lad det blive i kode, da tjek for xDevAPI
        RowResult res = mSession->sql("show variables like 'version'").execute();
        std::stringstream version;
        version << res.fetchOne().get(1).get<std::string>();
        int major_version;
        version >> major_version;
        if (major_version < 8) {
          std::cout << "Server is not high enough version! Must be 8 or above!" << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << "qDatabaseHandler: Something didn't go well! " << e.what() << std::endl;
    }
    return mSession;
}

bool qDatabaseHandler::disconnect()
{
    // TODO make a check to see if session is there.
    std::cout << "qDatabaseHandler: Closing session ..." << std::endl;
    mSession->close();
    return true;
}


// TODO find better name
std::vector<Row> *qDatabaseHandler::showTables()
{
    // I assume there's already loaded the database
    std::cout <<"qDatabaseHandler: Session accepted, creating collection..." <<std::endl;
    Schema schema = mSession->getSchema(mDatabase);
    std::vector<std::string> mSchVec = schema.getTableNames();

    std::cout << "qDatabaseHandler:Listing found tables: " << std::endl;
    for (uint32_t i = 0; i < mSchVec.size(); ++i) {
        std::cout << mSchVec.at(i) << " | ";
    } std::cout << std::endl;

    SqlResult qSql = mSession->sql("SELECT * FROM workshop.bil").execute();
    if(qSql.hasData() ? !1 : 1)
    {
        std::cout << "qDatabasehandler.cpp : No data recieved: " << qSql.hasData() << std::endl;
    }
    disconnect();

    mRes = new std::vector<Row>(qSql.fetchAll());
    // Prints the output to the terminal.
    // TODO Save the data correct
    for(Row row : *mRes)
    {
        for(uint32_t i = 0; i < row.colCount(); i++)
        {
            std::cout << row[i] << " | ";
        }
        std::cout << std::endl;
    }
    return mRes;

}
