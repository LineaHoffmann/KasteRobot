#include "qdatabasehandler.hpp"

qDatabaseHandler::qDatabaseHandler()
{
    // Start the worker thread
    mThreadLife.exchange(true);
    mThread = new std::thread(&qDatabaseHandler::mWorkerThreadLoop, this);
}


qDatabaseHandler::~qDatabaseHandler()
{
    mThreadLife.exchange(false);
    mThread->join();
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
void qDatabaseHandler::pushLogEntry(qDatabaseEntry *entry) {
    std::lock_guard<std::mutex> lock(mMtx);
    mWorkQueue.push(entry);
    return;
}
void qDatabaseHandler::mWorkerThreadLoop() {
    bool isQueueEmpty;
    while (mThreadLife.load()) {
        std::unique_lock<std::mutex> lock(mMtx);
        isQueueEmpty = mWorkQueue.empty();
        lock.unlock();
        while (!isQueueEmpty) {
            lock.lock();
            qDatabaseEntry* p = mWorkQueue.front();
            mWorkQueue.pop();
            isQueueEmpty = mWorkQueue.empty();
            lock.unlock();
            if (!mPushEntryToLog(p)) logerr("Worker failed to push a log entry!");
            delete p;
        }
        // Offloading
        lock.~unique_lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
bool qDatabaseHandler::mPushEntryToLog(qDatabaseEntry* entry) {
    {

            // Create a new session and get the main log table
            Session *session = connect();
            Schema schema = session->getSchema("kasteRobot");
            Table table_log = schema.getTable("log");
            Table table_position = schema.getTable("position");

            // Detect sub-type
            //  Build SQL statements and execute them
            //  Read the just added data from the database to check success
            if (auto t = dynamic_cast<qDatabaseBallEntry<double>*>(entry)) {
                auto _t = *t;
                table_log.insert("descriptor").values("ball").execute();
                Row logBallRow = table_log.select("log_ID").orderBy("created_at", "desc").execute().fetchOne();
                // Insert into position table
                std::string pos_values =
                        std::to_string(_t.ballPosition.x) + "," +
                        std::to_string(_t.ballPosition.y);
                table_position.insert("x_pos,y_pos").values(pos_values);
                Row posBallRow = table_position.select("position_ID").orderBy("created_at_position", "desc").execute().fetchOne();
                // Insert into ball table
                Table table_ball = schema.getTable("ball");
                std::string t_values =
                        "'" + std::string(logBallRow[0]) + "'," +
                        std::to_string(_t.ballDiameter) + ",'" +
                        std::string(posBallRow[0]) + "'";
                table_ball.insert("log_ID,diameter,ball_position").values(t_values).execute();
            } else if (auto t = dynamic_cast<qDatabaseMoveEntry<double>*>(entry)) {
                auto _t = *t;
                table_log.insert("descriptor").values("move").execute();
                Row logMoveRow = table_log.select("log_ID").orderBy("created_at", "desc").execute().fetchOne();
                // insert into position table
                std::string start_pos_values =
                        std::to_string(_t.start.x) + "," +
                        std::to_string(_t.start.y) + "," +
                        std::to_string(_t.start.z) + "," +
                        std::to_string(_t.start.rx) + "," +
                        std::to_string(_t.start.ry) + "," +
                        std::to_string(_t.start.rz) + ",";
                table_position.insert("x_pos,y_pos,z_pos,x_rotation,y_rotation,z_rotation").values(start_pos_values).execute();
                Row posMoveStartRow = table_position.select("position_ID").orderBy("created_at_position", "DESC").execute().fetchOne();
                std::string end_pos_values =
                        std::to_string(_t.end.x) + "," +
                        std::to_string(_t.end.y) + "," +
                        std::to_string(_t.end.z) + "," +
                        std::to_string(_t.end.rx) + "," +
                        std::to_string(_t.end.ry) + "," +
                        std::to_string(_t.end.rz) + ",";
                table_position.insert("x_pos,y_pos,z_pos,x_rotation,y_rotation,z_rotation").values(end_pos_values).execute();
                Row posMoveEndRow = table_position.select("position_ID").orderBy("created_at_position", "DESC").execute().fetchOne();
                // Insert into move table
                Table table_move = schema.getTable("move");

                std::string t_values =
                        "'" + std::string(logMoveRow[0]) + "'," +
                        getRobotMoveTypeAsString(_t.moveType) + ",'" +
                        std::string(posMoveStartRow[0]) + "','" +
                        std::string(posMoveEndRow[0]) + "'" ;
                table_move.insert("log_ID,moveType,start_positionID,end_positionID").values(t_values);
            } else if (auto t = dynamic_cast<qDatabaseThrowEntry<double>*>(entry)) {
                auto _t = *t;
                table_log.insert("descriptor").values("throw").execute();

                // Insert into position
                std::string position_values =
                        std::to_string(_t.releasePoint.x) + "," +
                        std::to_string(_t.releasePoint.y) + "," +
                        std::to_string(_t.releasePoint.z) + "," +
                        std::to_string(_t.releasePoint.rx) + "," +
                        std::to_string(_t.releasePoint.ry) + "," +
                        std::to_string(_t.releasePoint.rz) + "," ;
                table_position.insert("x_pos,y_pos,z_pos,x_rotation,y_rotation,z_rotation").values(position_values);
                Row logThrowRow = table_log.select("log_ID").orderBy("created_at","desc").execute().fetchOne();
                Row posThrowRow = table_position.select("position_ID").orderBy("created_at_position", "DESC").execute().fetchOne();

                // Insert into throw table
                Table table_throw = schema.getTable("throw");
                char success = _t.successful ? '1' : '0';
                std::string t_values =
                        "'" + std::string(logThrowRow[0]) + "','" +
                        std::string(posThrowRow[0]) + "'," +
                        success + "," +
                        std::to_string(_t.deviation) + "," +
                        std::to_string(_t.releaseVelocityCalced) + "," +
                        std::to_string(_t.releaseVelocityActual) + ",";
               table_throw.insert("log_ID,position_ID,successful,deviation,tcp_velocity_cal,tcp_velocity_act").values(t_values).execute();
            } else if (auto t = dynamic_cast<qDatabaseGripperEntry<double>*>(entry)) {
                auto _t = *t;
                table_log.insert("descriptor").values("gripper").execute();
                Row tempNewRow = table_log.select("log_ID").orderBy("created_at", "desc").execute().fetchOne();
                Table table_gripper = schema.getTable("gripper");
                char success = _t.successful ? '1' : '0';
                std::string t_values =
                        "'" + std::string(tempNewRow[0]) + "','" +
                        std::to_string(_t.start) + "','" +
                        std::to_string(_t.end) + "'," +
                        success;
                table_gripper.insert("log_ID, start_width, end_width, successful").values(t_values).execute();
            } else {
                logerr("Cannot push this type of data to the database!");
                disconnect();
                return false;
            }
            disconnect();
            return true;
        }
}
