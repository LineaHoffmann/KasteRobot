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
    Table* table = new Table(mSchema->getTable("log"));
    RowResult logRes = table->select("*").orderBy("created_at desc").limit(100).execute();
    mRes = new std::vector<Row>(logRes.fetchAll());// saves the data in mRes, from log table

    for(Row row : *mRes)
    {
        // Timestamp - common for all types
        Row tempDateRow = session->sql("SELECT date_format(created_at, '%T - %d/%m/%Y') "
                                        "FROM kasteRobot.log "
                                        "WHERE log_ID = ?").bind(row[0]).execute().fetchOne();

        if(std::string(row[2]) == "throw") {
            Table *tempTableThrow = new Table(mSchema->getTable("log_throw"));
            Row tempThrowRow = tempTableThrow->select("*").where("log_ID = :param").bind("param", row[0]).execute().fetchOne();

            // Position
            tempTableThrow = new Table(mSchema->getTable("position"));
            Row tempPosRow = tempTableThrow->select("*").where("position_ID = :param").bind("param", tempThrowRow[4]).execute().fetchOne();
            point6D<double> pos(tempPosRow[2], tempPosRow[3], tempPosRow[4], tempPosRow[5], tempPosRow[6], tempPosRow[7]);

            qDatabaseThrowEntry<double> *tempThrowEntry = new qDatabaseThrowEntry<double>
                    (std::string(tempDateRow[0]),   // Timestamp
                    std::string(row[2]),            // Desc
                    bool(tempThrowRow[5]),          // Successful
                    pos,                            // Position
                    double(tempThrowRow[7]),        // release_vel_calc
                    double(tempThrowRow[8]),        // release_vel_act
                    double(tempThrowRow[6]));       // deviation
            result.push_back((qDatabaseEntry*) tempThrowEntry);
        } else if (std::string(row[2]) == "move") {
            Table *tempTableMove = new Table(mSchema->getTable("log_move"));
            Row tempMoveRow = tempTableMove->select("*").where("log_ID = :param").bind("param", row[0]).execute().fetchOne();
            ROBOT_MOVE_TYPE moveType = getRobotMoveTypeFromString(std::string(tempMoveRow[4]));

            // position Start
            tempTableMove = new Table(mSchema->getTable("position"));
            Row tempPosRowStart = tempTableMove->select("*").where("position_ID = :param").bind("param", tempMoveRow[5]).execute().fetchOne();
            point6D<double> posS(tempPosRowStart[2], tempPosRowStart[3], tempPosRowStart[4], tempPosRowStart[5], tempPosRowStart[6], tempPosRowStart[7]);

            // position End
            Row tempPosRowEnd = tempTableMove->select("*").where("position_ID = :param").bind("param", tempMoveRow[6]).execute().fetchOne();
            point6D<double> posE(tempPosRowEnd[2], tempPosRowEnd[3], tempPosRowEnd[4], tempPosRowEnd[5], tempPosRowEnd[6], tempPosRowEnd[7]);

            qDatabaseMoveEntry<double> *tempMoveEntry = new qDatabaseMoveEntry<double>
                    (std::string(tempDateRow[0]),
                    std::string(tempMoveRow[2]),
                    posS,
                    posE,
                    moveType);
            result.push_back((qDatabaseEntry*) tempMoveEntry);
        } else if (std::string(row[2]) == "gripper") {
            Table *tempTableGripper = new Table(mSchema->getTable("log_gripper"));
            Row tempResGripper = tempTableGripper->select("*").where("log_ID = :param").bind("param", row[0]).execute().fetchOne();

            try {
                // TODO [srp]: The indexing isn't guaranteed yet
                qDatabaseGripperEntry<double> *tempGripperEntry = new qDatabaseGripperEntry<double>
                        (std::string(tempDateRow[0]),                   // Timestamp
                        std::string(tempResGripper[2]),                 // Description
                        double(tempResGripper[3]) ? true : false,       // Succesful
                        double(tempResGripper[5]),                      // start_width
                        double(tempResGripper[6]));                     // end_width
                result.push_back((qDatabaseEntry*) tempGripperEntry);
            } catch (...) {
                logerr("Error while allocating Gripper Entry object for Database!");
            }

        } else if (std::string(row[2]) == "ball") {
            Table *tempTableBall = new Table(mSchema->getTable("log_ball"));
            Row tempBallRow = tempTableBall->select("*").where("log_ID = :param").bind("param", row[0]).execute().fetchOne();

            // Ball position
            tempTableBall = new Table(mSchema->getTable("position"));
            Row tempResBall = tempTableBall->select("*").where("position_ID = :param").bind("param", tempBallRow[5]).execute().fetchOne();
            point2D<double> posB(tempResBall[2],tempResBall[3]);

            qDatabaseBallEntry<double> *tempBallEntry = new qDatabaseBallEntry<double>
                    (std::string(tempDateRow[0]),   // timestamp
                    std::string(row[2]),    // descriptor
                    double(tempBallRow[4]), // ball diameter
                    posB);                  // ball position
            result.push_back((qDatabaseEntry*) tempBallEntry);

        } else {
            logstd("No matching description found");
        }
    }
    disconnect();
    return result;
}

