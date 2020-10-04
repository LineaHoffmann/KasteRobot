#ifndef CLINKER_H
#define CLINKER_H
#pragma once

#include "../includeheader.h"
#include "../logic/xlinker.h"

class cLinker
{
public:
    cLinker();

    // Controller connections called from cMain
    void getCtrlState();
    void setCtrlStart();
    void setCtrlClose();

    // Robot connections called from cMain
    void getRobotState();
    std::vector<double> getRobotTCP();
    std::vector<double> getRobotJointValues();
    void setRobotConnect();
    void setRobotDisconnect();
    void setRobotNewTCP();
    void setRobotNewJointValues();

    // Camera connections called from cMain
    void getCameraState();
    wxImage* getCameraFrame();
    bool hasCameraFrame();
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

    std::mutex* getMtx();

private:

    xLinker *_linker;
    std::mutex mMtx;

};

#endif // CLINKER_H
