#include "qdatabasehandler.hpp"

qDatabaseHandler::qDatabaseHandler()
{
}


qDatabaseHandler::~qDatabaseHandler()
{
}

Session *qDatabaseHandler::connect()
{
    try {
        // Testing purpose.
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

void qDatabaseHandler::disconnect()
{
    if (mSession != nullptr) {
        std::cout << "qDatabaseHandler: Closing session ..." << std::endl;
        mSession->close();
    }
    else {
        std::cout << "qDatabaseHandler: No active connection ..." << std::endl;
    }
}

void qDatabaseHandler::setDatabaseCredentials(const std::string& user,
                                              const std::string& password,
                                              const std::string& hostname,
                                              const std::string& schema,
                                              uint32_t port)
{
    mUser = user;
    mPassword = password;
    mHost = hostname;
    mDatabase = schema;
    mPort = port;
    connect();
}

std::vector<Row> *qDatabaseHandler::retriveData()
{
    // Det nyeste af alle tabeller (row).

    mSchema = new Schema(mSession->getSchema(mDatabase));
    // Accessing an exsisting table
    // TODO Make table variable, so can be changed from input
    mTable = new Table(mSchema->getTable("throw"));
    if(mTable->isView())
    {
        std::cout << "qDatabasehandler: Something is wrong, isVeiw() " << std::endl;
    }

    // Sql call, find rows.
    RowResult qResult = mTable->select("*").execute();
    disconnect();

    mRes = new std::vector<Row>(qResult.fetchAll());
    // Testing Purpose (Show in terminal)
    for(Row row : *mRes)
            {
                for(uint32_t i = 0; i < row.colCount(); i++)
                {
                    std::cout << row[i] << " | ";
                }
                std::cout << std::endl;
            }

    // Hvilket objekt vil jeg have ? -> Oprette objekt
    // Noget med entryType...Skal gøres noget med getTable() For at hente korrekt data.
    return mRes;
}

