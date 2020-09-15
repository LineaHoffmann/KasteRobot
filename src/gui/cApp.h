#ifndef CAPP_H
#define CAPP_H

#pragma once

#include "wx/wx.h"
#include "cMain.h"


class cApp : public wxApp
{
public:
    cApp();
    ~cApp();

public:
    virtual bool OnInit();

private:
    cMain* mFrame = nullptr;
};

#endif // CAPP_H
