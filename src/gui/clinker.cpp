#include "clinker.h"

cLinker::cLinker() {}
bool cLinker::cIsOk() const {
    if (xLink && xLink->cIsOk()) return true;
    else return false;
}

void cLinker::addLogicLinker(std::shared_ptr<xLinker> link) {
    xLink = link;
}

void cLinker::setRobotConnect(std::string ip)
{
    xLink->robotConnect(ip);
}

void cLinker::setRobotDisconnect()
{
    std::lock_guard<std::mutex> lock(mMtx);
    xLink->robotDisconnect();
}

UR_STRUCT *cLinker::getRobotStruct()
{
    return xLink->getRobotStruct();
}

int cLinker::getCameraState() {
    return xLink->getCameraState();
}
const cv::Mat& cLinker::getCameraFrame() {
    std::lock_guard<std::mutex> lock(mMtx);
    return xLink->getCameraFrame();
}
