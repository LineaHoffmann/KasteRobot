#include "clinker.h"

cLinker::cLinker() {}

void cLinker::addLogicLinker(xLinker *link) {
    xLink = link;
}

void cLinker::getCameraState() {}
const cv::Mat& cLinker::getCameraFrame() {
    return xLink->getCameraFrame();

}
bool cLinker::hasCameraFrame() {
    return xLink->hasCameraFrame();
}
