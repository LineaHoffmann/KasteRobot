#include "xlinker.h"

xLinker::xLinker()
{
}
void xLinker::addCamera(std::shared_ptr<xBaslerCam> camera) {
    mCamera = camera;
}

void xLinker::addRobot(std::shared_ptr<xUR_Control> robot)
{
    mRobot = robot;
}
bool xLinker::cIsOk() {
    // TODO: When ready, also add the robot pointer to this
    if (mCamera) return true;
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
