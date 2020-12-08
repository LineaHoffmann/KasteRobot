#include "qdatabasehandler.hpp"

qDatabaseHandler::qDatabaseHandler()
{
}


qDatabaseHandler::~qDatabaseHandler()
{
}

Session* qDatabaseHandler::connect()
{
    // Testing purpose.
    std::cout << "Connecting to MySQL server with:" << std::endl;
    std::cout << mUser << ":" << mPassword << "@" << mHost << ":" << mPort << std::endl;;
    std::cout << "on database: " << mDatabase << std::endl;

    if (mSsl_mode == SSLMode::DISABLED) std::cout << "SSL is disabled." << std::endl;
    try {
            Session session(
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
    mTable = new Table(mSchema->getTable("kasteRobot.log"));// Accessing an exsisting table

    // Sql call, find newest row;
    std::stringstream statement;
    statement << "created_at IN(SELECT MAX(created_at) FROM log)";
    RowResult qResult = mTable->select("log_ID").execute();
    std::vector<qDatabaseEntry> result;    
    mRes = new std::vector<Row>(qResult.fetchAll());// saves the data in mRes, from log table

    for(Row row : *mRes)
    {
        if(std::string(row[3]) == "throw")
        {
            Table *tempTableThrow = new Table(mSchema->getTable("throw"));
            std::stringstream statement;
            statement << "log_ID = '" << std::string(row[1]) << "'";
            RowResult tempResThrow = tempTableThrow->select("*").where(statement.str().c_str()).execute();
            Row tempThrowRow = tempResThrow.fetchOne();

            // Posistion
            delete tempTableThrow;
            tempTableThrow = new Table(mSchema->getTable("position"));
            statement.clear();
            statement << "position_ID = '" << std::string(tempThrowRow[2]) << "'";
            tempResThrow = tempTableThrow->select("*").where(statement.str().c_str()).execute();
            Row tempPosRow = tempResThrow.fetchOne();
            point6D<double> pos(tempPosRow[1], tempPosRow[2], tempPosRow[3], tempPosRow[4], tempPosRow[5], tempPosRow[6]);

            qDatabaseThrowEntry tempThrowEntry = qDatabaseThrowEntry(std::string(row[1]),
                    std::string(row[2]), //time
                    bool(tempThrowRow[3]), // desc
                    pos,
                    double(tempThrowRow[5]), //v1 cal
                    double(tempThrowRow[6]), // v2 act
                    double(tempThrowRow[4])); // deviation

            result.push_back((qDatabaseEntry)tempThrowEntry);
        }

        if(std::string(row[3]) == "move")
        {
            Table *tempTableMove = new Table(mSchema->getTable("log_move"));
            RowResult tempResMove;
            if(tempTableMove->count() <= 1)
            {
                tempResMove = tempTableMove->select("*").execute();

            }
            else{
                std::stringstream statement;
                statement << "created_at IN(SELECT MAX(created_at) FROM log)";
                tempResMove = tempTableMove->select("*").where(statement.str().c_str()).execute();
            }
            Row tempMoveRow = tempResMove.fetchOne();

            // ROBOT ENUM TYPE.
            ROBOT_MOVE_TYPE moveType;
            if(std::string(tempMoveRow[4]) == "JLIN"){
               moveType = ROBOT_MOVE_TYPE::MOVE_JLIN;
            }else if(std::string(tempMoveRow[4]) == "JPATH"){
                moveType = ROBOT_MOVE_TYPE::MOVE_JPATH;
            }else if(std::string(tempMoveRow[4]) == "JIK"){
                moveType = ROBOT_MOVE_TYPE::MOVE_JIK;
            }else if(std::string(tempMoveRow[4]) == "LFK"){
                moveType = ROBOT_MOVE_TYPE::MOVE_LFK;
            }else if(std::string(tempMoveRow[4]) == "L"){
                moveType = ROBOT_MOVE_TYPE::MOVE_L;
            }else if(std::string(tempMoveRow[4])== "TLIN"){
                moveType = ROBOT_MOVE_TYPE::MOVE_TLIN;
            }else if(std::string(tempMoveRow[4]) == "SERVOJ"){
                moveType = ROBOT_MOVE_TYPE::SERVOJ;
            }else if(std::string(tempMoveRow[4]) == "HOME"){
                moveType = ROBOT_MOVE_TYPE::HOME;
            }else if(std::string(tempMoveRow[4]) == "PICKUP"){
                moveType = ROBOT_MOVE_TYPE::PICKUP;
            }else{
               logstd("No macthing moveType in move tabel");
               moveType = ROBOT_MOVE_TYPE::MOVE_DEFAULT;
            };

            // position Start
            delete tempTableMove;
            tempTableMove = new Table(mSchema->getTable("position"));
            std::stringstream statement;
            statement << "position_ID = '" << std::string(tempMoveRow[5]) << "'";
            tempResMove = tempTableMove->select("*").where(statement.str().c_str()).execute();
            Row tempPosRowS = tempResMove.fetchOne();
            point6D<double> posS(tempPosRowS[1],tempPosRowS[2], tempPosRowS[3], tempPosRowS[4], tempPosRowS[5], tempPosRowS[6]);

            // position End
            delete tempTableMove;
            tempTableMove = new Table(mSchema->getTable("position"));
            statement.clear();
            statement << "position_ID = '" << std::string(tempMoveRow[6]) << "'";
            tempResMove = tempTableMove->select("*").where(statement.str().c_str()).execute();
            Row tempPosRowE = tempResMove.fetchOne();
            point6D<double> posE(tempPosRowE[1],tempPosRowE[2], tempPosRowE[3], tempPosRowE[4], tempPosRowE[5], tempPosRowE[6]);

            qDatabaseMoveEntry tempMoveEntry = qDatabaseMoveEntry(std::string(row[2]),
                    std::string(row[3]),
                    posS,
                    posE,
                    moveType);

            result.push_back((qDatabaseEntry)tempMoveEntry);
        }

        if(std::string(row[3]) == "gripper")
        {
            Table *tempTableGripper = new Table(mSchema->getTable("log_gripper"));
            RowResult tempResGripper;
            if(tempTableGripper->count() <= 1)
            {
                tempResGripper = tempTableGripper->select("*").execute();
            }
            else{
                std::stringstream statement;
                statement << "created_at DESC";
                tempResGripper = tempTableGripper->select("*").orderBy(statement.str().c_str()).execute();
            }
            Row tempGripperRow = tempResGripper.fetchOne();

            qDatabaseGripperEntry tempGripperEntry = qDatabaseGripperEntry(std::string(row[2]),
                    std::string(row[3]),
                    double(tempGripperRow[4]), // start_width
                    double(tempGripperRow[5])); // end_width

            result.push_back((qDatabaseEntry)tempGripperEntry);
        }

        if(std::string(row[3]) == "ball"){

            Table *tempTableBall = new Table(mSchema->getTable("log_gripper"));
            RowResult tempResBall;
            if(tempTableBall->count() <= 1)
            {
                tempResBall = tempTableBall->select("*").execute();
            }
            else{
                std::stringstream statement;
                statement << "created_at DESC";
                tempResBall = tempTableBall->select("*").orderBy(statement.str().c_str()).execute();
            }
            Row tempGripperRow = tempResBall.fetchOne();

            // Ball position
            delete tempTableBall;
            tempTableBall = new Table(mSchema->getTable("position"));
            std::stringstream statement;
            statement << "position_ID = '" << std::string(tempGripperRow[5]) << "'";
            tempResBall = tempTableBall->select("*").where(statement.str().c_str()).execute();
            Row tempPosRowB = tempResBall.fetchOne();
            point2D<double> posB(tempPosRowB[1],tempPosRowB[2]);

            qDatabaseBallEntry tempBallEntry = qDatabaseBallEntry(std::string(row[2]),
                    std::string(row[3]),
                    double(tempGripperRow[3]), // diameter
                    posB); // ballPosition

            result.push_back((qDatabaseEntry)tempBallEntry);
        } else {
            logstd("No matching description found");
        }
    }
    disconnect();
    return result;
}

