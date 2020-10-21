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
    void addRobot(xUR_Control *rob);

    const cv::Mat& getCameraFrame();
    bool hasCameraFrame();

    void robotConnect(std::string IP);
    void robotDisconnect();
    UR_STRUCT* getRobotStruct();



private:
    xBaslerCam *camera;
    xUR_Control *robot;

};

#endif // XLINKER_H
