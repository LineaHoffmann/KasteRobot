#ifndef XLINKER_H
#define XLINKER_H
#pragma once

#include "../includeheader.h"
#include "xbaslercam.h"

class xRobot;
class xController;
class xGripper;

class cLinker;
class dbLinker;

class xLinker
{
public:
    xLinker();
    void addCamera(xBaslerCam *cam);

    const cv::Mat& getCameraFrame();
    bool hasCameraFrame();

private:
    xBaslerCam *camera;
};

#endif // XLINKER_H
