#include "xcontroller.hpp"

xController::xController()
{
    // Bool defaults
    mWithBall.exchange(false);

    // Camera
    mCamera = std::make_shared<xBaslerCam>("../resources/pylonimgs/*.bmp", 5000, 30);

    //Imagehandler
    mImagehandler = std::make_shared<ximageHandler>(cv::imread("../resources/testImg.png"));
    mImagehandler->ballColor(25, 15); //set what color ball we are looking for
    mImagehandler->setMinMaxRadius(1.7, 2.3); //i cm
    mImagehandler->setRobotBase(42.5, 9); //i cm
    mImagehandler->showResult = true;
    //mImagehandler->loadImage(cv::imread("../resources/testImg.png"));
    //mImagehandler->cutOutTable();




    //mImagehandler->setRobotBase(42.5, 9.5); //i cm
    //mImagehandler->setRobotBase(42.5, 14.5); //i cm





    // Robot
    std::vector<std::vector<double>> q{{0,-1.5,0,-1.5,-1,0}};
    //double spd{0.5}, acc{0.5};

    try {
        mRobot = std::make_shared<xUrControl>();
        //mRobot->setConnect("127.0.0.1");
    } catch (x_err::error& e) {
        std::string s = "[ROBOT] ";
        s.append(e.what());
        logerr(s.c_str());
    }

    // Gripper
    mGripper = std::make_shared<xGripperClient>();

    // Database
    mDatabase = std::make_shared<qDatabaseHandler>();

    //robworks
    mCollisionDetector = std::make_shared<xCollisionDetector>();
    mCollisionDetector->loadWorkcell("../resources/XML_files/CollisionV2.wc.xml");
    //    std::vector<double> currentPose {0.1098, -0.33075, 0.89921, 3.14149, 0.000383972, -2.07111};
    //    mCollisionDetector->checkQ(currentPose);


    //starting camera
    mCamera->start();

    // Sharing the database pointer so classes that need to can write to the log
    mRobot->addDatabasePointer(mDatabase);
    mImagehandler->addDatabasePointer(mDatabase);
    //mGripper->addDatabasePointer(mDatabase);

}

xController::~xController() {};
bool xController::hasNewImage()
{
    if (mCamera) {
        return mCamera->hasNewImage();
    }
    else return false;
}
cv::Mat xController::getImage()
{
    if (mCamera) {
        if(mWithBall.load()) {
            return std::get<1>(mImagehandler->findBallAndPosition(mCamera->getImage()));
        } else {
            return mCamera->getImage();
        }
    } else return cv::Mat();
}

void xController::fillInfo(treeInfo &info)
{
    // TODO: Fill the info struct
    if(mRobot && mRobot->getIsConnected()){
        try{
            RobotData robot(mRobot->getURStruct());

            if (robot.robotState == 7){
                info.robotState = ROBOT_RUNNING;
            } else {
                info.robotState = ROBOT_NOT_RUNNING;
            }

            info.robotIP = mRobot->getIP();
            info.robotJointPosition = robot.robotJointPosition;
            info.robotTcpPosition = robot.robotTcpPosition;
            info.robotPollingRate = robot.robotPollingRate;
        } catch (const std::exception &e) {
            std::cout << "error handled: " << e.what() << std::endl;
        }
    } else{
        info.robotState = ROBOT_NOT_CONNECTED;
    }

    if (mGripper && mGripper->isConnected()) {
        info.gripperState = GRIPPER_RUNNING;

        gripperData gripper(mGripper->getData());

        info.gripperTemp = gripper.temp;
        info.gripperForce = gripper.force;
        info.gripperPosition = gripper.pos;
        info.gripperSpeed = gripper.speed;
    }
    else {info.gripperState = GRIPPER_NOT_CONNECTED;}
}

std::vector<qDatabaseEntry*> xController::getDatabaseEntries() {
    // Get the data from the database object, if any.
    // If not, return the empty constructor
    return mDatabase ? mDatabase->retriveData() : std::vector<qDatabaseEntry*>();
}

//template <typename T>
//void xController::guiButtonPressed(BINDING_ID id, T data)


void xController::testDetectAndPickUp(std::shared_ptr<ximageHandler> mImagehandler,
                                      std::shared_ptr<xBaslerCam> mCamera,
                                      std::shared_ptr<xUrControl> mRobot,
                                      std::shared_ptr<xGripperClient> mGripper,
                                      std::shared_ptr<xCollisionDetector> mCollisionDetector)
{
    //flyt robotten til hjem position
    mRobot->setMove(ROBOT_MOVE_TYPE::HOME);
    logstd("Robot Homing");
    while(mRobot->getIsBusy()){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    mGripper->home();
    std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(mCamera->getImage());
    //std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(cv::imread("../resources/ballimgs/remappedBall1.png"));

    if (std::get<0>(ballResult)){
        logstd("Ball found, moving robot to pre pickup position");

        //flyt robotten til det der prepickup position
        mRobot->setMove(ROBOT_MOVE_TYPE::PICKUP);
        while(mRobot->getIsBusy()){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }


        std::vector<std::vector<double>> pickupPosition;
        pickupPosition.push_back(xMath::ball_position_to_robotframe(ballResult));
        std::stringstream s;
        s << "x: " << pickupPosition[0][0] << " || y: " << pickupPosition[0][1] << " || z: " << pickupPosition[0][2];
        logstd("moving robot to pickup object");
        //return;
        std::vector<std::vector<double>> moveTo = pickupPosition;
        moveTo[0][2] = 0.1;


        //flyt robot til positionen i variablen pickupPosition uden z højde
        mRobot->setMove(ROBOT_MOVE_TYPE::MOVE_L, moveTo);
        while(mRobot->getIsBusy()){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        //flyt robot til positionen i variablen pickupPosition
        mRobot->setMove(ROBOT_MOVE_TYPE::MOVE_L, pickupPosition);
        while(mRobot->getIsBusy()){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }


        logstd("grip object...");
        mGripper->grip();

        std::this_thread::sleep_for(std::chrono::milliseconds(2500));

        //flyt robotten til prepickup position
        try{
            mRobot->setMove(ROBOT_MOVE_TYPE::PICKUP);
            logstd("Robot moving to pre pickup position");
            while(mRobot->getIsBusy()){
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        } catch (const x_err::error &e){
            logerr("homing failed");
        }



        logstd("Sequenze succesfull");

    }else{
        logstd("Stopping detection and pickup sequenze");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}


void xController::testDetectAndPickUp2()
{
    //flyt robotten til hjem position
    try{
        mRobot->setMove(ROBOT_MOVE_TYPE::HOME);
        logstd("Robot Homing");
        while(mRobot->getIsBusy()){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } catch (const x_err::error &e){
        logerr("homing failed");
    }

    //std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(mCamera->getImage());
    std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(cv::imread("../resources/ballimgs/remappedBall3.png"));

    if (std::get<0>(ballResult)){
        logstd("Ball found, moving robot to pre pickup position");

        //flyt robotten til det der prepickup position

        //        std::vector<double> pickupPosition; //= {std::get<2>(ballResult).x/100, -(std::get<2>(ballResult).y/100), -0.025, 1.778, 2.577, -0.1};
        //        pickupPosition.push_back(xMath::ball_position_to_robotframe(ballResult));
        std::cout << std::get<2>(ballResult).x/100 << -(std::get<2>(ballResult).y/100) << std::endl;
        std::vector<std::vector<double>> pickupPosition;
        pickupPosition.push_back(xMath::ball_position_to_robotframe(ballResult));

        logstd("moving robot to pickup object");

        try {
            //            std::vector<std::vector<double>> q;
            //            q.push_back(pickupPosition);
            //            //flyt robot til positionen i variablen pickupPosition
            //            mRobot->setMove(ROBOT_MOVE_TYPE::MOVE_L, q);
            std::vector<std::vector<double>> pickupXYZ;
            //pickupXYZ.push_back(pickupPosition);
            createPath(pickupPosition);
            while(mRobot->getIsBusy()){
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        } catch (const x_err::error &e){
            logerr("failed");
        }

        logstd("grip object...");
        mGripper->grip();

        std::this_thread::sleep_for(std::chrono::milliseconds(2500));

        //flyt robotten til prepickup position
        try{
            mRobot->setMove(ROBOT_MOVE_TYPE::PICKUP);
            logstd("Robot moving to pre pickup position");
            while(mRobot->getIsBusy()){
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        } catch (const x_err::error &e){
            logerr("homing failed");
        }

        logstd("Sequenze succesfull");

    }else{
        logstd("Stopping detection and pickup sequenze");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

void xController::testThrowSpeedJ(double angle)
{
    logstd("Starting ball detection and pick-up sequence ..");
    std::thread findball(testDetectAndPickUp, mImagehandler, mCamera, mRobot, mGripper, mCollisionDetector);
    findball.join();


    time_t timeNow = time(0);
    std::stringstream timestamp;
    timestamp << std::put_time(localtime(&timeNow), "%d%m%y_%H%M%S");
    std::string file = "../resources/logcsv/kast_" + timestamp.str() + ".csv";
    std::ofstream log;
    log.open(file);
    log << "TCP speed;;;;;;Joint Angles;;;;;;TCP position;;;;;;Release;\n";
    log << "x;y;z;rx;ry;rz;q0;q1;q2;q3;q4;q5;x;y;z;rx;ry;rz;\n";
    log.close();

    int highPoll = 125;
    RobotData robotData(mRobot->getURStruct());
    int prevPollingRate = mRobot->getPollingRate();
    double relAngle = -2.05668	-1.62165;

    //start q[2] kan afgøre hvor langt der kastes ved brug af en af disse ligninger
    //angle = -0.0106*længde + 0.2075
    double x = 160;
    double shoulderAngle = -1.60;
    double baseAngle = -1.15192;
    std::vector<std::vector<double> > startq{{baseAngle, shoulderAngle, -2.2689, -1.8325 ,1.57,1.57}};

    logstd("Moving to throwing position");
    mRobot->setMove(ROBOT_MOVE_TYPE::MOVE_JLIN,startq, 1, 1);
    while(mRobot->getIsBusy()){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    mRobot->setPollingRate(highPoll);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    logstd("throwing...");
    std::cout << "start: " << robotData.robotJointPosition[2] << " | " << angle << std::endl;

    mRobot->setMove(ROBOT_MOVE_TYPE::SPEEDJ);

    bool released = false;
    log.open(file, std::ios::app); //appends data to already created file
    while (mRobot->getIsBusy()){
        robotData = mRobot->getURStruct();

        for (double d : robotData.robotTcpSpeed){
            log << d << ";";
        }
        for (double d : robotData.robotJointPosition){
            log << d << ";";
        }
        for (double d : robotData.robotTcpPosition){
            log << d << ";";
        }
        for (double d : robotData.robotJointSpeed){
            log << d << ";";
        }
        std::cout << std::endl;

        //if (!released) std::cout << robotData.robotJointPosition[2] << " | " << angle << std::endl;
        //release vinkel = startq[2] + startq[3]
        if (robotData.robotJointPosition[2] + robotData.robotJointPosition[3] >= relAngle && !released){
            mGripper->release();
            std::cout << "Gripper released" << std::endl;
            log << "release send;";
            released = true;
            //            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            //            mRobot->speedJStop();
        }
        log << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/highPoll));
    }
    log.close();
    std::cout << "slut: " << robotData.robotJointPosition[2] << " | " << angle << std::endl;

    mRobot->setPollingRate(prevPollingRate);

    logstd("Speedj test throw completed");
}

void xController::createPath(std::vector<std::vector<double>> q){
    std::array<float, 6> currentPoseArr;
    currentPoseArr = mRobot->getURStruct().robotJointPosition;
    std::vector<double> currentPose;
    for (size_t i = 0; i < currentPoseArr.size(); ++i) {
        currentPose.push_back(currentPoseArr[i]);
    }
    std::vector<std::vector<double>> path;
    path = mCollisionDetector->moveFromTo(currentPose, q[0]);
    if(mRobot){
        mRobot->setMove(ROBOT_MOVE_TYPE::MOVE_JPATH,path);
    }
}

void xController::throwBall(double x, double y)
{
    // NOTE srp: The database logging for throw is done here
    // Default log entry for a complete failure
    // Last param, deviation, should get from user input, but we don't ask
    point6D<double> releasePoint{-1,-1,-1,-1,-1,-1};
    qDatabaseThrowEntry<double> throwEntry(false, releasePoint, 2.6, 0.0, -1);

    //local scope fuction variables
    std::vector<std::vector<double>> q;

    try{
        mRobot->setMove(ROBOT_MOVE_TYPE::HOME);
        logstd("Robot Homing");
        while(mRobot->getIsBusy()){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } catch (const x_err::error &e){
        logerr("homing failed");
        mDatabase->pushLogEntry(throwEntry);
        return;
    }

    std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(mCamera->getImage());
    //std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(cv::imread("../resources/ballimgs/remappedBall2.png"));
    if (!std::get<0>(ballResult)){
        logstd("Ball not found, moving robot to pre pickup position, aborting");
        mDatabase->pushLogEntry(throwEntry);
        return;
    }
    std::vector<double> pickupPosition = xMath::ball_position_to_robotframe(ballResult);

    logstd("moving robot to pickup object");

    try {
        q.push_back(pickupPosition);
        mRobot->setMove(ROBOT_MOVE_TYPE::MOVE_L, q);
        while(mRobot->getIsBusy()){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } catch (const x_err::error &e){
        logerr("failed");
        mDatabase->pushLogEntry(throwEntry);
        return;
    }

    double radius = std::get<3>(ballResult);
    q.clear();
    q.push_back(pickupPosition);
    q[0][2] += (double) (radius/100) * 3;


    //flyt robot til positionen i variablen pickupPosition uden z højde
    mRobot->setMove(ROBOT_MOVE_TYPE::MOVE_L, q);
    while(mRobot->getIsBusy()){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    q.clear();
    q.push_back(pickupPosition);

    //flyt robot til positionen i variablen pickupPosition
    mRobot->setMove(ROBOT_MOVE_TYPE::MOVE_L, q);
    while(mRobot->getIsBusy()){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    mGripper->grip();
    while (mGripper->isReady()){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    //check if ball of right diameter is gripped
    if (mGripper->getPos() < (radius * 2)-2 || mGripper->getPos() > (radius * 2)+2 ){
        logerr("ball not detected in gripper, Aborting");
        mDatabase->pushLogEntry(throwEntry);
        return;
    }

    //moving to pre-pickup position
    try{
        mRobot->setMove(ROBOT_MOVE_TYPE::PICKUP);
        logstd("Robot moving to pre pickup position");
        while(mRobot->getIsBusy()){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } catch (const x_err::error &e){
        logerr("homing failed");
        mDatabase->pushLogEntry(throwEntry);
        return;
    }

    //preparing throw
    int highPoll = 125;
    RobotData robotData(mRobot->getURStruct());
    int prevPollingRate = mRobot->getPollingRate();
    double relAngle = -2.05668	-1.62165;
    q.clear();
    q.push_back(std::vector<double>{-1.15192, -1.57, -2.2689, -1.8325 ,1.57,1.57});

    xMath::calcThrow(q[0],x,y);

    for ( double d : q[0]){
        std::cout << d << " | ";
    }
    std::cout << std::endl;

    logstd("Moving to throwing position");
    try{
        mRobot->setMove(ROBOT_MOVE_TYPE::MOVE_JLIN,q,2,2);
        logstd("Robot moving to pre pickup position");
        while(mRobot->getIsBusy()){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } catch (const x_err::error &e){
        logerr("homing failed");
        mDatabase->pushLogEntry(throwEntry);
        return;
    }

    mRobot->setPollingRate(highPoll);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    logstd("throwing...");
    mRobot->setMove(ROBOT_MOVE_TYPE::SPEEDJ);

    bool released = false;
    while (mRobot->getIsBusy()){
        robotData = mRobot->getURStruct();

        if (robotData.robotJointPosition[2] + robotData.robotJointPosition[3] >= relAngle && !released){
            mGripper->release();
            std::cout << "Gripper released" << std::endl;
            released = true;
            releasePoint.x = robotData.robotTcpPosition[0];
            releasePoint.y = robotData.robotTcpPosition[1];
            releasePoint.z = robotData.robotTcpPosition[2];
            releasePoint.rx = robotData.robotTcpPosition[3];
            releasePoint.ry = robotData.robotTcpPosition[4];
            releasePoint.rz = robotData.robotTcpPosition[5];
            throwEntry.releaseVelocityActual = sqrt(
                        robotData.robotTcpSpeed[0] * robotData.robotTcpSpeed[0] +
                    robotData.robotTcpSpeed[1] * robotData.robotTcpSpeed[1] +
                    robotData.robotTcpSpeed[2] * robotData.robotTcpSpeed[2]);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/(highPoll*2)));
    }


    mRobot->setPollingRate(prevPollingRate);
    mIsAvailable.exchange(true);

    logstd("[THROW] throw completed");
    throwEntry.successful = true;
    mDatabase->pushLogEntry(throwEntry);
}
