#ifndef XLINKER_H
#define XLINKER_H
#pragma once

#include <string>
#include <utility>
#include <mutex>

#include "../gui/clinker.h"

#include "xbaslercam.h"
#include "xurcontrol.h"

class cLinker;
class xLinker
{
public:
    xLinker();
    void addCamera(std::shared_ptr<xBaslerCam> cam);
    void addRobot(std::shared_ptr<xUrControl> robot);

    bool cIsOk();

    const cv::Mat& getCameraFrame();
    bool isCameraConnected();

    void robotConnect(std::string IP);
    void robotDisconnect();
    UR_STRUCT getRobotStruct();



private:
    std::shared_ptr<xBaslerCam> mCamera;
    std::shared_ptr<xUrControl> mRobot;

    std::shared_ptr<cLinker> cLink;

    std::mutex mMtx;

};

#endif // XLINKER_H
