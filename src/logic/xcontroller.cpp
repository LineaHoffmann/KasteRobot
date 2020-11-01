#include "xcontroller.hpp"

xController::xController()
{
    // Camera
    mCamera = std::make_shared<xBaslerCam>("../resources/pylonimgs/*.bmp", 5000, 37);
    mCamera->start();

    //Imagehandler
    mImagehandler = std::make_shared<ximageHandler>(cv::imread("../resources/testImg.png"));
    mImagehandler->ballColor(10, 20); //set what color ball we are looking for
    mImagehandler->setMinMaxRadius(1.7, 2.3); //i cm
    mImagehandler->setRobotBase(42.2, 8.8); //i cm



    //robworks
    mCollisionDetector = std::make_shared<xCollisionDetector>("../resources/XML_files/Collision v1.wc.xml");


    // Robot
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
        if(withBall) {
            return mImagehandler->findBallAndPosition(mCamera->getImage()).first;
        } else {
            return mCamera->getImage();
        }
    } else return cv::Mat();
}

//template <typename T>
//void xController::guiButtonPressed(BINDING_ID id, T data)


