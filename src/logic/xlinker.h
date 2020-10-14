#ifndef XLINKER_H
#define XLINKER_H
#pragma once

#include "../includeheader.h"
#include "xbaslercam.h"
#include "xur_control.h"

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
    void addRobot(xUR_Control *rob);

    const cv::Mat& getCameraFrame();
    bool hasCameraFrame();



private:
    xBaslerCam *camera;
    xUR_Control *robot;

};

#endif // XLINKER_H
