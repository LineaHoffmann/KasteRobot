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

        // Checks if mDatabse is the default schema, and sets it if not.
        std::string defaultSchema = mSession->getDefaultSchemaName();
        if(defaultSchema != mDatabase)
        {
            mSession->sql("USE " + mDatabase);
        }


    } catch (const std::exception &e) {
        std::cerr << "qDatabaseHandler: Something didn't go well! " << e.what() << std::endl;
    }
    return mSession;
}

bool qDatabaseHandler::disconnect()
{
    // TODO make a check to see if session is there.
    // Clode() returns void
    std::cout << "qDatabaseHandler: Closing session ..." << std::endl;
    mSession->close();
    return true;
}

void qDatabaseHandler::setDatabaseCredentials(std::tuple<std::string, std::string, std::string, std::string, uint32_t> credentialsInput)
{
    mUser = std::get<0>(credentialsInput);
    mPassword = std::get<1>(credentialsInput);
    mHost = std::get<2>(credentialsInput);
    mDatabase = std::get<3>(credentialsInput);
    mPort = std::get<4>(credentialsInput);

    //Test, checks variables
    std::cout << mUser << " : " << mPassword << " : " << mHost << " : "
              << mDatabase << " : " << mPort << std::endl;

    connect();

}

/**
 * @brief qDatabaseHandler::getDbData
 * @param tableName, the table you wish to collect data from
 * @return vector *mRes, contains data from sql query
 */
std::vector<Row> *qDatabaseHandler::getDbData(std::string tableName)
{
    mSchema = new Schema(mSession->getSchema(mDatabase));
    //Schema qSchema = mSession->getSchema(mDatabase);

    // Accessing an exsisting table
    // TODO Make table variable, so can be changed from input
    mTable = new Table(mSchema->getTable("throw!"));
    //Table qTable = mSchema->getTable("throw");
    if(mTable->isView())
    {
        std::cout << "qDatabasehandler: Something is wrong, isVeiw() " << std::endl;
    }

    // Sql call, find rows.
    RowResult qResult = mTable->select("*").execute();
    disconnect();

    mRes = new std::vector<Row>(qResult.fetchAll());
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

bool qDatabaseHandler::qInsert()
{
    // NOTE check with count, if new row added
    mTable->insert().execute();

    return true;

}
