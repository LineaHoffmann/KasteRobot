#ifndef CAPP_H
#define CAPP_H
#pragma once

#include "../includeheader.h"
#include "cmain.h"

class cApp : public wxApp
{
public:
    cApp();
    ~cApp();

public:
    virtual bool OnInit();
//    virtual int MainLoop();
private:
    cMain* mFrame = nullptr;
    
    
    std::thread* thread = nullptr;
    [[noreturn]] void threadFunc();
};

#endif // CAPP_H
