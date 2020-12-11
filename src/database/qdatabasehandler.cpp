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
        mSession = new Session(
                    SessionOption::USER, mUser,
                    SessionOption::PWD, mPassword,
                    SessionOption::HOST, mHost,
                    SessionOption::PORT, mPort,
                    SessionOption::DB, mDatabase,
                    SessionOption::SSL_MODE, mSsl_mode
                    );
        logstd("qDatabaseHandler: Session accepted ...");
        //Checks if xDevAPI is valid
        RowResult res = mSession->sql("show variables like 'version'").execute();
        std::stringstream version;
        version << res.fetchOne().get(1).get<std::string>();
        int major_version;
        version >> major_version;
        if (major_version < 8) {
            std::cout << "Server is not high enough version! Must be 8 or above!" << std::endl;
        }
        return mSession; // not good practice, but for now only solution.
    } catch (const std::exception &e) {
        std::cerr << "qDatabaseHandler: Something didn't go well! " << e.what() << std::endl;

        return nullptr;
    }
}

void qDatabaseHandler::disconnect()
{
    if (mSession != nullptr) {
        logstd("qDatabaseHandler: Closing session ...");
        mSession->close();
    }
    else {
        std::cout << "qDatabaseHandler: No active connection ..." << std::endl;
    }
    logstd("qDatabaseHandler: Session closed ...");
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

std::vector<qDatabaseEntry*> qDatabaseHandler::retriveData()
{
    Session* session = connect();
    std::vector<qDatabaseEntry*> result;
    if (session == nullptr) return result; // Bail out on failed connection
    mSchema = new Schema(session->getSchema(mDatabase)); // selects database to work with

    // Sql call, findest newest log_ID based on created_at timestamp.
    // TODO IF NO ENTRIES, handle that ?
    RowResult qResult = session->sql("SELECT * "
                                     "FROM kasteRobot.log "
                                     "WHERE created_at IN(SELECT MAX(created_at)FROM kasteRobot.log)").execute();

    mRes = new std::vector<Row>(qResult.fetchAll());// saves the data in mRes, from log table

    // Testing Purpose (Show in terminal)
//    for(Row row : *mRes)
//    {
//        std::cout << "SELECT * FROM log" << std::endl;
//        for(uint32_t i = 0; i < row.colCount(); i++)
//        {
//            std::cout << row[i] << " | ";
//        }
//        std::cout << std::endl;
//    }

    for(Row row : *mRes)
    {
        if(std::string(row[2]) == "throw")
        {
            Table *tempTableThrow = new Table(mSchema->getTable("log_throw"));
            std::stringstream statement;
            statement << "log_ID = '" << std::string(row[0]) << "'";
            RowResult tempResThrow = tempTableThrow->select("*").where(statement.str().c_str()).execute();
            Row tempThrowRow = tempResThrow.fetchOne();

            // Testing Purpose
            std::cout << "Throw SQL: " << std::endl;
            for(uint i = 0; i < tempThrowRow.colCount(); i++)
            {
                std::cout << tempThrowRow[i] << " | " << std::endl;
            }

            // Testing purpose
//            statement.str(" ");
//            std::cout << "statement: " <<statement.str() << std::endl;

            // Posistion
            delete tempTableThrow;
            tempTableThrow = new Table(mSchema->getTable("position"));

            // Testing purpose
            std::cout << "index 0: " << std::string(tempThrowRow[4]) << std::endl;
            statement.str(" ");
            statement << "position_ID = '" << std::string(tempThrowRow[4]) << "'";
            tempResThrow = tempTableThrow->select("*").where(statement.str().c_str()).execute();
            Row tempPosRow = tempResThrow.fetchOne();

            // Testing Purpose
//            std::cout << "Throw: Position: SQL: " << std::endl;
//            for(uint i = 0; i < tempPosRow.colCount(); i++)
//            {
//                std::cout << tempPosRow[i] << " | " << std::endl;
//            }

            point6D<double> pos(tempPosRow[2], tempPosRow[3], tempPosRow[4], tempPosRow[5], tempPosRow[6], tempPosRow[7]);

            qDatabaseThrowEntry<double> *tempThrowEntry = new qDatabaseThrowEntry<double>(std::string(row[1]),
                    std::string(row[2]), //time
                    bool(tempThrowRow[3]), // desc
                    pos,
                    double(tempThrowRow[5]), //v1 cal
                    double(tempThrowRow[6]), // v2 act
                    double(tempThrowRow[4])); // deviation
            result.push_back((qDatabaseEntry*) tempThrowEntry);
        }

        if(std::string(row[2]) == "move")
        {
            Table *tempTableMove = new Table(mSchema->getTable("log_move"));
            RowResult tempResMove = tempTableMove->select("*").execute();
            Row tempMoveRow = tempResMove.fetchOne();
            // Testing Purpose (Show in terminal)
            std::cout << "MOVE SQL: " << std::endl;
            for(uint i = 0; i < tempMoveRow.colCount(); i++)
            {
                std::cout << tempMoveRow[i] << " | ";
            }
            std::cout << std::endl;

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
            std::cout << statement.str() << std::endl;
            tempResMove = tempTableMove->select("*").where(statement.str().c_str()).execute();
            Row tempPosRowS = tempResMove.fetchOne();
            point6D<double> posS(tempPosRowS[2],tempPosRowS[3], tempPosRowS[4], tempPosRowS[5], tempPosRowS[6], tempPosRowS[7]);

            // position End
            statement.str(" ");
            statement << "position_ID = '" << std::string(tempMoveRow[6]) << "'";
            tempResMove = tempTableMove->select("*").where(statement.str().c_str()).execute();
            Row tempPosRowE = tempResMove.fetchOne();
            point6D<double> posE(tempPosRowE[2],tempPosRowE[3], tempPosRowE[4], tempPosRowE[5], tempPosRowE[6], tempPosRowE[7]);

            qDatabaseMoveEntry<double> *tempMoveEntry = new qDatabaseMoveEntry<double>(std::string(row[1]),
                    std::string(row[2]),
                    posS,
                    posE,
                    moveType);

            result.push_back((qDatabaseEntry*) tempMoveEntry);
        }

        if(std::string(row[2]) == "gripper")
        {
            Table *tempTableGripper = new Table(mSchema->getTable("log_gripper"));
            RowResult tempResGripper = tempTableGripper->select("*").execute();
            Row tempGripperRow = tempResGripper.fetchOne();

            // NOTE srp: Hack to get a formatted date from the server
            // Sadly it seems it'll have to be a sepperate call on the log_ID
            // Formatting directly in the server query is preferable
            Row tempDateRow = session->sql("SELECT date_format(created_at, '%T - %d/%m/%Y') "
                                            "FROM kasteRobot.log "
                                            "WHERE log_ID = ?").bind(tempGripperRow[0]).execute().fetchOne();

            try {
                // TODO [srp]: The indexing isn't guaranteed yet
                qDatabaseGripperEntry<double> *tempGripperEntry = new qDatabaseGripperEntry<double>
                        (std::string(tempDateRow[0]),                   // Timestamp
                        std::string(tempGripperRow[2]),                 // Description
                        false,       // Succesful
                        double(tempGripperRow[5]),                      // start_width
                        double(tempGripperRow[6]));                     // end_width
                result.push_back((qDatabaseEntry*) tempGripperEntry);
            } catch (...) {
                logerr("Error while allocating Gripper Entry object for Database!");
            }

        }

        if(std::string(row[2]) == "ball"){

            Table *tempTableBall = new Table(mSchema->getTable("log_gripperTest"));
            RowResult tempResBall = tempTableBall->select("*").execute();
            Row tempBallRow = tempResBall.fetchOne();

//          Testing Purpose
            std::cout << "Ball: SQL: " << std::endl;
            for(uint i = 0; i < tempBallRow.colCount(); i++)
            {
                std::cout << tempBallRow[i] << " | " << std::endl;
            }

            // Ball position
            delete tempTableBall;
            tempTableBall = new Table(mSchema->getTable("position"));
            std::stringstream statement;
            statement << "position_ID = '" << std::string(tempBallRow[5]) << "'";
            tempResBall = tempTableBall->select("*").where(statement.str().c_str()).execute();
            Row tempPosRowB = tempResBall.fetchOne();

            // Testing Purpose
            std::cout << "Ball: SQL: " << std::endl;
            for(uint i = 0; i < tempPosRowB.colCount(); i++)
            {
                std::cout << tempPosRowB[i] << " | " << std::endl;
            }

            point2D<double> posB(tempPosRowB[2],tempPosRowB[3]);

            qDatabaseBallEntry<double> *tempBallEntry = new qDatabaseBallEntry<double>(std::string(row[2]),
                    std::string(row[3]),
                    double(tempBallRow[3]), // diameter
                    posB);; // ballPosition

            result.push_back((qDatabaseEntry*) tempBallEntry);

        } else {
            logstd("No matching description found");
        }
    }
    disconnect();
    return result;
}

