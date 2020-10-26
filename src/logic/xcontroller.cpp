#include "xcontroller.hpp"

xController::xController()
{
    // Camera
    mCamera = std::make_shared<xBaslerCam>("../resources/pylonimgs/*.bmp", 12500);
    mCamera->start();

    // Robot
    mRobot = std::make_shared<xUrControl>();
    try {
        mRobot->connect("127.0.0.1");
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
    if (mCamera) return mCamera->hasNewImage();
    else return false;
}
cv::Mat xController::getImage()
{
    if (mCamera) return mCamera->getImage();
    else return cv::Mat();
}
