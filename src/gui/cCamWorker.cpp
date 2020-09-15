// Main header include
#include "cCamWorker.h"

// OpenCV includes
#include <opencv2/opencv.hpp>

// wxWidget includes
#include "wx/wx.h"

// System headers
#include <iostream>

cCamWorker::cCamWorker(cMain *pFrame) : wxThread(wxTHREAD_DETACHED)
{
    mpFrame = pFrame;
    //mpCamera = pCamera;
    mCreation = new std::chrono::steady_clock::time_point;
    mMutex = new wxMutex();

    return;
}
cCamWorker::~cCamWorker()
{
    mpFrame = nullptr;
    //mpCamera = nullptr;
    mCreation = nullptr;
    mMutex = nullptr;
}

void* cCamWorker::Entry()
{
    *mCreation = std::chrono::steady_clock::now();
    mLife = 1;

    // Start the camera

    while(mLife) {
        // While camera is running, process a frame

        // Sleep for 20ms (just to lower CPU time a bit)
        this->Sleep(20);
    }

}
void cCamWorker::OnExit()
{
    delete(mCreation);
    delete(mMutex);
}

double cCamWorker::GetTime(void)
{
    using namespace std::chrono; // Just to shorten things a bit
    return (double) (duration_cast<milliseconds>(steady_clock::now() - *mCreation).count() / 1000.0f);
}
