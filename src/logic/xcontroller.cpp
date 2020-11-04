#include "xcontroller.hpp"

xController::xController()
{
    // Bool defaults
    mWithBall.exchange(false);

    // Camera
    mCamera = std::make_shared<xBaslerCam>("../resources/pylonimgs/*.bmp", 5000, 30);
    mCamera->start();

    //Imagehandler
    mImagehandler = std::make_shared<ximageHandler>(cv::imread("../resources/testImg.png"));
    mImagehandler->ballColor(10, 20); //set what color ball we are looking for
    mImagehandler->setMinMaxRadius(1.7, 2.3); //i cm
    mImagehandler->setRobotBase(42.5, 9.5); //i cm
    mImagehandler->showResult = true;



    //robworks
    mCollisionDetector = std::make_shared<xCollisionDetector>("../resources/XML_files/Collision v1.wc.xml");


    // Robot
    std::vector<std::vector<double>> q{{0,-1.5,0,-1.5,-1,0}};
    //double spd{0.5}, acc{0.5};

    try {
        mRobot = std::make_shared<xUrControl>();
    } catch (x_err::error& e) {
        std::string s = "[ROBOT] ";
        s.append(e.what());
        logerr(s.c_str());
    }

    // Gripper
    mGripper = std::make_shared<xGripperClient>();
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
    try{
    mRobot->setMove(xUrControl::moveEnum::HOME);
    logstd("Robot Homing");
    while(mRobot->getIsBusy()){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    } catch (const x_err::error &e){
        logerr("homing failed");
    }

    //std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(mCamera->getImage());
    std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(cv::imread("../resources/ballimgs/remappedBall2.png"));

    if (std::get<0>(ballResult)){
        logstd("Ball found, moving robot to pre pickup position");

        //flyt robotten til det der prepickup position
        try {
        mRobot->setMove(xUrControl::moveEnum::PICKUP);
        } catch (const x_err::error &e){
            logerr("Pickup pos failed");
        }
        //mGripper->home();

        std::vector<double> pickupPosition = xMath::ball_position_to_robotframe(ballResult);

        logstd("moving robot to pickup object");

        try {
            std::vector<std::vector<double>> q;
            q.push_back(pickupPosition);
            //flyt robot til positionen i variablen pickupPosition
            mRobot->setMove(xUrControl::moveEnum::MOVE_L, q);
        } catch (const x_err::error &e){
            logerr("homing failed");
        }

        logstd("grip object...");
        //mGripper->grip();

        logstd("moving robot to throwing position");
        //flyt robotten til hjem position eller evt en prepickup position
        try{
        mRobot->setMove(xUrControl::moveEnum::HOME);
        logstd("Robot Homed");
        } catch (const x_err::error &e){
            logerr("homing failed");
        }

        logstd("Sequenze succesfull");
        return;
    }
    logstd("Stopping detection and pickup sequenze");
}
