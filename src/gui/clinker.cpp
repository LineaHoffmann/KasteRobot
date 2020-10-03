#include "clinker.h"

#ifndef logstd
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

cLinker::cLinker() {}


void cLinker::getCameraState() {}
wxImage* cLinker::getCameraFrame() {
    // This should only be run from the GUI thread! Anything else will not work
    try {
        return _linker->getCameraFrame();
    } catch (std::exception &e) {
        logerr("Loading image from camera threw exception!");
        logerr(e.what());
    }
    return nullptr;
}
bool cLinker::hasCameraFrame() {
    return _linker->hasCameraFrame();
}
