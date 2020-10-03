#ifndef XLINKER_H
#define XLINKER_H
#pragma once

#include "../includeheader.h"

class xCamera;
class xRobot;
class xController;
class xGripper;

class cLinker;
class dbLinker;

class xLinker
{
public:
    xLinker();


    wxImage* getCameraFrame();
    bool hasCameraFrame();

private:
    xCamera *camera;
};

#endif // XLINKER_H
