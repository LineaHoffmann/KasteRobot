#include "clinker.h"

cLinker::cLinker() {}
/**
 * @brief cLinker::cIsOk verifies the xLinker contents for the GUI
 * @return True if xLink is complete, false if not
 */
bool cLinker::cIsOk() const {
    if (xLink && xLink->cIsOk()) return true;
    else return false;
}

void cLinker::addLogicLinker(std::shared_ptr<xLinker> link) {
    xLink = link;
}

void cLinker::setRobotConnect(std::string ip)
{
    std::lock_guard<std::mutex> lock(mMtx);
    xLink->robotConnect(ip);
}

void cLinker::setRobotDisconnect()
{
    std::lock_guard<std::mutex> lock(mMtx);
    xLink->robotDisconnect();
}

UR_STRUCT cLinker::getRobotStruct()
{
    return xLink->getRobotStruct();
}

bool cLinker::isCameraConnected() {
    return xLink->isCameraConnected();
}
const cv::Mat& cLinker::getCameraFrame() {
    std::lock_guard<std::mutex> lock(mMtx);
    return xLink->getCameraFrame();
}
