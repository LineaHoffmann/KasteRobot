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

//template <typename T>
//void xController::guiButtonPressed(BINDING_ID id, T data)



void xController::testDetectAndPickUp()
{
    //flyt robotten til hjem position
    //mRobot->move()
    //mRobot->HOME();  Jacob fix :D
    logstd("Robot Homed");


    std::tuple<bool, cv::Mat, cv::Point2f, float> ballResult = mImagehandler->findBallAndPosition(mCamera->getImage());
    if (std::get<0>(ballResult)){
        logstd("Ball found, moving robot to pre pickup position");

        //flyt robotten til det der prepickup position
        //mRobot->PickUp
        mGripper->home();

        cv::Point3d pickupPosition = xMath::ball_position_to_robotframe(ballResult);

        logstd("moving robot to pickup object");

        //flyt robot til positionen i variablen pickupPosition
        //mRobot->move()


        logstd("grip object...");
        mGripper->grip();

        logstd("moving robot to throwing position");
        //flyt robotten til hjem position eller evt en prepickup position
        //mRobot->move()
        //mRobot->HOME();  Jacob fix :D

        logstd("Sequenze succesfull");
        return;
    }
    logstd("Stopping detection and pickup sequenze");
}
