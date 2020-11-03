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
        std::cout << "Session accepted ..." << std::endl;
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
        std::cerr << "Something didn't go well! " << e.what() << std::endl;
    }
    return mSession;
}

bool qDatabaseHandler::disconnect()
{
    // TODO make a check to see if session is there.
    std::cout << "Closing session ..." << std::endl;
    mSession->close();
    return true;
}

bool qDatabaseHandler::showTables()
{
    // I assume there's already loaded the workshop database, called WORKSHOP
    std::cout <<"Session accepted, creating collection..." <<std::endl;
    Schema schema = mSession->getSchema(mDatabase);
    std::vector<std::string> mSchVec = schema.getTableNames();

    std::cout << "Listing found tables: " << std::endl;
    for (uint32_t i = 0; i < mSchVec.size(); ++i) {
        std::cout << mSchVec.at(i) << " | ";
    } std::cout << std::endl;


    std::cout << "Show data in tabel: " << std::endl;
    return true;

}
