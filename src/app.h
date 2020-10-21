#ifndef APP_H
#define APP_H
#pragma once

#include "includeheader.h"

// NOTE: cMain and xBaslerCam should be encapsulated by cLinker and xLinker
#include "gui/cmain.h"
#include "logic/xbaslercam.h"
#include "logic/xlinker.h"
#include "gui/clinker.h"
#include "logic/xur_control.h"
#include "logic/xrobotexceptions.h"

class app : public wxApp
{
public:
    app();
    ~app();

public:
    virtual bool OnInit();

private:

    cLinker* cLink;
    xLinker* xLink;

    cMain* mFrame = nullptr;
    
    // Treat this thread as the main program loop for now ..
    std::thread* thread = nullptr;
    void threadFunc();
    bool mJoinThread = false;
};

#endif // APP_H
