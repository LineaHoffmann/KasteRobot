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
        return nullptr;
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
}

std::vector<qDatabaseEntry> qDatabaseHandler::retriveData()
{
    Session* session = connect();
    if (session == nullptr) return std::vector<qDatabaseEntry>(); // Bail out on failed connection
    mSchema = new Schema(session->getSchema(mDatabase));
    // Accessing an exsisting table
    // TODO Make table variable, so can be changed from input
    mTable = new Table(mSchema->getTable("log"));

    // Sql call, find rows.
    RowResult qResult = mTable->select("*").execute();
    std::vector<qDatabaseEntry> result;

    mRes = new std::vector<Row>(qResult.fetchAll());
    for(Row row : *mRes)
    {
        if(std::string(row[3]) == "throw")
        {
            Table *tempTable = new Table(mSchema->getTable("throw"));
            std::stringstream statement;
            statement << "log_ID = '" << std::string(row[1]) << "'";
//            RowResult tempRes = tempTable->select(statement.str().c_str()).execute();
            RowResult tempRes = tempTable->select("*").where(statement.str().c_str()).execute();
            Row tempRow = tempRes.fetchOne();

            // Posistion
            delete tempTable;
            tempTable = new Table(mSchema->getTable("position"));
            statement.clear();
            statement << "position_ID = '" << std::string(tempRow[3]) << "'";
            tempRes = tempTable->select("*").where(statement.str().c_str()).execute();
            Row tempPosRow = tempRes.fetchOne();
            point6D<double> pos(tempPosRow[2], tempPosRow[3], tempPosRow[4], tempPosRow[5], tempPosRow[6], tempPosRow[7]);


            qDatabaseThrowEntry tempEntry = qDatabaseThrowEntry(std::string(row[2]),
                    std::string(row[3]),
                    bool(tempRow[4]),
                    pos,
                    double(tempRow[8]),
                    double(tempRow[5]));

            result.push_back((qDatabaseEntry)tempEntry);
        }

        if(std::string(row[3]) == "move")
        {
            Table *tempTableMove = new Table(mSchema->getTable("newest_moveEntry"));
            RowResult tempResMove = tempTableMove->select("*").execute();
            Row tempMoveRow = tempResMove.fetchOne();

            // ROBOT ENUM TYPE.

            // position Start
            delete tempTableMove;
            tempTableMove = new Table(mSchema->getTable("position"));
            std::stringstream statement;
            statement << "position_ID = '" << std::string(tempMoveRow[3]) << "'";
            tempResMove = tempTableMove->select("*").where(statement.str().c_str()).execute();
            Row tempPosRowS = tempResMove.fetchOne();
            point6D<double> posS(tempPosRowS[2],tempPosRowS[3], tempPosRowS[4], tempPosRowS[5], tempPosRowS[6], tempPosRowS[7]);

            // position End
            delete tempTableMove;
            tempTableMove = new Table(mSchema->getTable("position"));
            statement.clear();
            statement << "position_ID = '" << std::string(tempMoveRow[4]) << "'";
            tempResMove = tempTableMove->select("*").where(statement.str().c_str()).execute();
            Row tempPosRowE = tempResMove.fetchOne();
            point6D<double> posE(tempPosRowE[2],tempPosRowE[3], tempPosRowE[4], tempPosRowE[5], tempPosRowE[6], tempPosRowE[7]);

//            qDatabaseMoveEntry tempMoveEntry = qDatabaseMoveEntry(std::string(row[2]),
//                    std::string(row[3]),
//                    posS,
//                    posE,
//                    std::string(tempMoveRow[2]));
        }

        if(std::string(row[3]) == "gripper")
        {

        }

        if(std::string(row[3]) == "ball"){}

        // Else ?? If none of the above?

    }
    disconnect();
    return result;
}

