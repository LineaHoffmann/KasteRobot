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
    //mImagehandler->setRobotBase(42.5, 9.5); //i cm
    mImagehandler->setRobotBase(42.5, 14.5); //i cm

    mImagehandler->showResult = true;

    //robworks
    mCollisionDetector = std::make_shared<xCollisionDetector>("../resources/XML_files/Collision v1.wc.xml");


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
        UR_STRUCT robot(mRobot->getURStruct());

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
    mRobot->setMove(xUrControl::moveEnum::HOME);
    logstd("Robot Homing");
    while(mRobot->getIsBusy()){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }


    mGripper->home();
    //std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(mCamera->getImage());
    std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(cv::imread("../resources/ballimgs/remappedBall1.png"));

    if (std::get<0>(ballResult)){
        logstd("Ball found, moving robot to pre pickup position");

        //flyt robotten til det der prepickup position
            mRobot->setMove(xUrControl::moveEnum::PICKUP);
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
            mRobot->setMove(xUrControl::moveEnum::MOVE_L, moveTo);
            while(mRobot->getIsBusy()){
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            //flyt robot til positionen i variablen pickupPosition
            mRobot->setMove(xUrControl::moveEnum::MOVE_L, pickupPosition);
            while(mRobot->getIsBusy()){
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }


        logstd("grip object...");
        mGripper->grip();
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        logstd("moving robot to throwing position");
        mRobot->setMove(xUrControl::moveEnum::PICKUP);
        while(mRobot->getIsBusy()){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        std::vector<std::vector<double>> kast;
        kast.push_back(std::vector<double>{-1.15192, -1.39626,0.0, -1.5708,1.5708,0});

        //flyt robotten til hjem position eller evt en prepickup position
            mRobot->setMove(xUrControl::moveEnum::MOVE_JLIN, kast,3,3);
            int i = 0;
        while(mRobot->getIsBusy()){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            i++;
            if (i == 70){
                mGripper->release();
            }
        }


        logstd("Sequenze succesfull");

    }else{
    logstd("Stopping detection and pickup sequenze");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}
