#ifndef CCAMWORKER_H
#define CCAMWORKER_H

#include "cMain.h"

#include "wx/thread.h"
#include "wx/wxprec.h"
#include "wx/wx.h"

#include <chrono>

class cMain;

class cCamWorker : public wxThread
{
public:
    cCamWorker(cMain *pFrame);
    ~cCamWorker();

    // Worker function
    virtual void* Entry();
    // wxWidgets calls this on exit
    virtual void OnExit();
    // Simply returns thread life in seconds
    double GetTime(void);

    wxFrame *mpFrame;
    //cCamera *mpCamera;

    unsigned char mLife;

private:
    std::chrono::steady_clock::time_point *mCreation;
    wxMutex *mMutex;
};

#endif // CCAMWORKER_H
