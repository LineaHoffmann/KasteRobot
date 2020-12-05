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
    //mImagehandler->setRobotBase(42.5, 9.5); //i cm
    //mImagehandler->setRobotBase(42.5, 14.5); //i cm

    mImagehandler->showResult = true;



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


void xController::testDetectAndPickUp2(std::shared_ptr<ximageHandler> mImagehandler,
                                      std::shared_ptr<xBaslerCam> mCamera,
                                      std::shared_ptr<xUrControl> mRobot,
                                      std::shared_ptr<xGripperClient> mGripper,
                                      std::shared_ptr<xCollisionDetector> mCollisionDetector)
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

    std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(mCamera->getImage());
//    std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(cv::imread("../resources/ballimgs/remappedBall2.png"));

    if (std::get<0>(ballResult)){
        logstd("Ball found, moving robot to pre pickup position");

        //flyt robotten til det der prepickup position


        std::vector<double> pickupPosition = xMath::ball_position_to_robotframe(ballResult);

        logstd("moving robot to pickup object");

        try {
            std::vector<std::vector<double>> q;
            q.push_back(pickupPosition);
            //flyt robot til positionen i variablen pickupPosition
            mRobot->setMove(ROBOT_MOVE_TYPE::MOVE_L, q);
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
    time_t timeNow = time(0);
    std::stringstream timestamp;
    timestamp << std::put_time(localtime(&timeNow), "%d%m%y_%H%M%S");
    std::string file = "../resources/logcsv/kast_" + timestamp.str() + ".csv";
    std::ofstream log;
    log.open(file);
    log << "TCP speed;;;;;;Joint Angles;;;;;;\n";
    log << "x;y;z;rx;ry;rz;q0;q1;q2;q3;q4;q5;\n";
    log.close();

    int highPoll = 125;
    RobotData robotData(mRobot->getURStruct());
    int prevPollingRate = mRobot->getPollingRate();

    std::vector<std::vector<double> > startq{{-1.15192, -1.9198, -2.2689, -1.8325 ,1.57,1.57}};
    //{-1.15192, -1.39626,-0.39392081, -1.5708,1.5708,1.5708};

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
            std::cout << std::setw(12) << d <<", ";
            log << d << ";";
        }
        std::cout << "\t|\t";
        for (double d : robotData.robotJointPosition){
            std::cout << d << ", ";
            log << d << ";";
        }
        std::cout << std::endl;
        log << "\n";
        //if (!released) std::cout << robotData.robotJointPosition[2] << " | " << angle << std::endl;

        if (robotData.robotJointPosition[2] >= angle && !released){
            mGripper->release();
            std::cout << "Gripper released" << std::endl;
            released = true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/highPoll));
    }
    log.close();
    std::cout << "slut: " << robotData.robotJointPosition[2] << " | " << angle << std::endl;

    mRobot->setPollingRate(prevPollingRate);

    logstd("Speedj test throw completed");
}

void xController::testPathCreation(std::vector<std::vector<double>> q){
    std::vector<double> currentPose {3, -0.723, 0.283, -1.062, -1.188, -1.527}; //Joint configs
    std::vector<std::vector<double>> path;
    std::cout << "1 i test" << std::endl;
    path = mCollisionDetector->moveFromTo(currentPose, q[0]);
    std::cout << "2 i test" << std::endl;
    if(mRobot){
    mRobot->setMove(ROBOT_MOVE_TYPE::MOVE_JPATH,path);
    }
}
