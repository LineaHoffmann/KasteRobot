#ifndef XLINKER_H
#define XLINKER_H

#include "wx/wx.h"


class xCamera;
class xRobot;

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
