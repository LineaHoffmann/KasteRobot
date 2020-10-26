#ifndef CLINKER_H
#define CLINKER_H
#pragma once

#include <utility>

#include "opencv2/opencv.hpp"
//#include "opencv2/core.hpp"

#include "../logic/xlinker.h"
#include "../logic/xurcontrol.h"

class xLinker;
class cLinker
{
public:
    cLinker();
    void addLogicLinker(std::shared_ptr<xLinker> link);

    // Public functions for checking pointer integrity
    bool cIsOk() const; // Calls from GUI classes
    bool xIsOk() const; // Calls from xLinker

    // Controller connections called from cMain
    void getCtrlState();
    void setCtrlStart();
    void setCtrlClose();

    // Robot connections called from cMain
    void getRobotState();
    std::vector<double> getRobotTCP();
    std::vector<double> getRobotJointValues();
    void setRobotConnect(std::string ip);
    void setRobotDisconnect();
    void setRobotNewTCP();
    void setRobotNewJointValues();
    UR_STRUCT getRobotStruct();

    // Camera connections called from cMain
    bool isCameraConnected();
    const cv::Mat& getCameraFrame();
    void setCameraConnect();
    void setCameraDisconnect();

    // Gripper connections called from cMain
    void getGripperState();
    void getGripperPosition();
    void getGripperForce();
    void setGripperConnect();
    void setGripperDisconnect();
    void setGripperPosition();
    void setGripperForce();

    // Database connections called from cMain
    void getDbState();
    void getDbEntries();
    void setDbEntry();
    void setDbConnect();
    void setDbDisconnect();

private:
    std::shared_ptr<xLinker> xLink;
    std::mutex mMtx;
};

#endif // CLINKER_H
