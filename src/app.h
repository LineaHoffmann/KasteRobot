#ifndef APP_H
#define APP_H
#pragma once

#include "includeheader.h"
#include "gui/cmain.h"

class app : public wxApp
{
public:
    app();
    ~app();

public:
    virtual bool OnInit();
//    virtual int MainLoop();
private:
    cMain* mFrame = nullptr;
    
    
    std::thread* thread = nullptr;
    [[noreturn]] void threadFunc();
};

#endif // APP_H
