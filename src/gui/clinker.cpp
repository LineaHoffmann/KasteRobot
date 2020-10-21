#include "clinker.h"

cLinker::cLinker() {}

void cLinker::addLogicLinker(xLinker *link) {
    xLink = link;
}

void cLinker::setRobotConnect(std::string ip)
{
    xLink->robotConnect(ip);
}

void cLinker::setRobotDisconnect()
{
    xLink->robotDisconnect();
}

UR_STRUCT *cLinker::getRobotStruct()
{
    return xLink->getRobotStruct();
}

void cLinker::getCameraState() {}
const cv::Mat& cLinker::getCameraFrame() {
    return xLink->getCameraFrame();

}
bool cLinker::hasCameraFrame() {
    return xLink->hasCameraFrame();
}
