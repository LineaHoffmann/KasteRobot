#include "xlinker.h"

xLinker::xLinker()
{
}
void xLinker::addCamera(std::shared_ptr<xBaslerCam> camera) {
    mCamera = camera;
}

void xLinker::addRobot(std::shared_ptr<xUrControl> robot)
{
    mRobot = robot;
}
/**
 * @brief xLinker::cIsOk verifies the pointers for the GUI (called from cLinker!)
 * @return True if xLinker is complete, false if not
 */
bool xLinker::cIsOk() {
    // TODO: Add gripper and qLinker->xIsOk() to this
    if (mCamera && mRobot) return true;
    else return false;
}
const cv::Mat& xLinker::getCameraFrame() {
    std::lock_guard<std::mutex> lock(mMtx);
    return mCamera->getImage();
}
bool xLinker::isCameraConnected() {
    std::lock_guard<std::mutex> lock(mMtx);
    return mCamera->isConnected();
}

void xLinker::robotConnect(std::string IP)
{
    std::lock_guard<std::mutex> lock(mMtx);
    mRobot->connect(IP);
}

void xLinker::robotDisconnect()
{
    std::lock_guard<std::mutex> lock(mMtx);
    mRobot->disconnect();
}

UR_STRUCT xLinker::getRobotStruct()
{
    std::lock_guard<std::mutex> lock(mMtx);
    // Is a pointer a good idea?
    // I'm guessing some other thread manages it
    return mRobot->getURStruct();
}
