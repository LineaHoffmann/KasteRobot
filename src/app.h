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

#include "gui/cmain.h"
#include "logic/xcontroller.h"

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

    cMain* guiMain = nullptr;

    std::thread* thread = nullptr;
    void threadFunc();
    bool mJoinThread = false;
};

#endif // APP_H
