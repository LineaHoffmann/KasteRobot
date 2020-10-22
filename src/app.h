#ifndef APP_H
#define APP_H
#pragma once

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

#include <utility>
#include <thread>

#include "gui/clinker.h"
#include "gui/cmain.h"

#include "logic/xlinker.h"
//#include "logic/xbaslercam.h"
//#include "logic/xur_control.h"
//#include "logic/xrobotexceptions.h"

//#include "database/qlinker.h"
#include "database/qdatabasehandler.h"

#include "wx/wx.h"

#include <sys/time.h>
#include <sys/resource.h>

class app : public wxApp
{
public:
    app();
    ~app();

public:
    virtual bool OnInit();
private:
    std::shared_ptr<cLinker> cLink;
    std::shared_ptr<xLinker> xLink;

    cMain* guiMain = nullptr;

    std::thread* thread = nullptr;
    void threadFunc();
    bool mJoinThread = false;
};

#endif // APP_H
