#ifndef XBASLERCAM_H
#define XBASLERCAM_H

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <atomic>

#include "pylon/PylonIncludes.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "wx/log.h"

#include "xexceptions.hpp"

class xBaslerCam
{
public:
    xBaslerCam();
    xBaslerCam(std::string calibrationPath);
    xBaslerCam(std::string calibrationPath, uint32_t exposure);
    xBaslerCam(std::string calibrationPath, uint32_t exposure, uint32_t maxFrameRate);

    ~xBaslerCam();

    bool isConnected(); //returns true if we have a connection to the camera.
    bool start(); //returns true if the camera was started correctly.
    void shutdown();

    void setPath(std::string calibrationPath) {path = calibrationPath; }
    void calibrate(); //run calibration on pictures in path

    bool hasNewImage(); // Checks if a new picture is available - THREADSAFE
    const cv::Mat getImage(); // get newest cv:Mat image (remapped) - THREADSAFE

    std::thread *baslerCamThread; //skal muligvis senere flyttes til private.

private:
    void GrabPictures(); //polling pictures from cam to memory

    // Create a PylonImage that will be used to create OpenCV images later.
    Pylon::CPylonImage pylonImage;
    cv::Mat openCvImage;    // Create an OpenCV image.
    std::string path = "../imgs/*.bmp";     // Path of the folder containing checkerboard images

    // Atomics for thread safety
    std::atomic<bool> mIsRunning;
    std::atomic<bool> mExit;
    std::atomic<bool> mHasNewImage;

    double myExposure = 12500;
    uint64_t frameRate  = 60;
    uint64_t frame = 1;
    bool isRectified = false;
    int32_t CHECKERBOARD[2]{9,6};

    std::vector<cv::Mat> caliPics;
    cv::Mat map1,map2;
    cv::Mat remapped_image;

    //calibration matrix
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    cv::Mat R;
    cv::Mat T;

    std::mutex mMtx;

};

#endif // XBASLERCAM_H
