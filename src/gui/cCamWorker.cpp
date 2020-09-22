// Main header include
#include "cCamWorker.h"

// OpenCV includes
#include <opencv2/opencv.hpp>

// wxWidget includes
#include "wx/wx.h"

// System headers
#include <iostream>

/**
 * @brief Worker thread class constructor for camera streaming
 * @param Parent frame
 */
cCamWorker::cCamWorker(cMain *pFrame) : wxThread(wxTHREAD_DETACHED)
{
    mpFrame = pFrame;
    //mpCamera = pCamera;
    mCreation = new std::chrono::steady_clock::time_point;
    mMutex = new wxMutex();

    return;
}
/**
 * @brief Destructor just resets pointers, wx handles rest
 */
cCamWorker::~cCamWorker()
{
    mpFrame = nullptr;
    //mpCamera = nullptr;
    mCreation = nullptr;
    mMutex = nullptr;
}
/**
 * @brief Entry point for thread creation
 * @return Absolutely nothing, as a pointer
 */
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
    return nullptr;
}
/**
 * @brief wxWidgets destructor (sorta, not really)
 */
void cCamWorker::OnExit()
{
    delete(mCreation);
    delete(mMutex);
}
/**
 * @brief Returns time passed since thread entry
 * @return Milliseconds with 3 digits as double
 */
double cCamWorker::GetTime(void)
{
    using namespace std::chrono; // Just to shorten things a bit. Don't worry, it's only local scope
    return (double) (duration_cast<milliseconds>(steady_clock::now() - *mCreation).count() / 1000.0f);
}
