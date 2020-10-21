#include "xlinker.h"

xLinker::xLinker()
{
}
void xLinker::addCamera(xBaslerCam *cam) {
    camera = cam;
}

void xLinker::addRobot(xUR_Control *rob)
{
    robot = rob;
}
const cv::Mat& xLinker::getCameraFrame() {
    return camera->getImage();
}
bool xLinker::hasCameraFrame() {
    // NOTE: This might not be required, we could just always go for the getCameraFrame
    return false;
}

void xLinker::robotConnect(std::string IP)
{
    robot->connect(IP);
}

void xLinker::robotDisconnect()
{
    robot->disconnect();
}

UR_STRUCT* xLinker::getRobotStruct()
{
    return robot->getURStruct();
}
