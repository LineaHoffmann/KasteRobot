#pragma once
#ifndef APP_H
#define APP_H

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

//#include <utility>
//#include <thread>

#include "gui/cmain.hpp"
#include "logic/xcontroller.hpp"

//#include "wx/wx.h"

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
    std::atomic<bool> mJoinThread;
};

#endif // APP_H
