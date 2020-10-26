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

#include "pylon/PylonIncludes.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "wx/log.h"

class xBaslerCam
{
public:
    xBaslerCam();
    xBaslerCam(std::string calibrationPath);
    xBaslerCam(std::string calibrationPath, int exposure);
    xBaslerCam(std::string calibrationPath, int exposure, int maxFrameRate);

    ~xBaslerCam();

    bool isConnected(); //returns true if we have a connection to the camera.
    bool start(); //returns true if the camera was started correctly.
    void shutdown();

    void setPath(std::string calibrationPath) {path = calibrationPath; }
    void calibrate(); //run calibration on pictures in path
    void updateCameraMatrix(cv::Mat NewCameraMatrix, cv::Mat NewCoeffs); //changing calibration manually use with care

    cv::Mat& getImage(); //get newest cv:Mat image (remapped)


    std::thread *baslerCamThread; //skal muligvis senere flyttes til private.

private:
    void GrabPictures(); //polling pictures from cam to memory

    // Create a PylonImage that will be used to create OpenCV images later.
    Pylon::CPylonImage pylonImage;
    cv::Mat openCvImage;    // Create an OpenCV image.
    std::string path = "../imgs/*.bmp";     // Path of the folder containing checkerboard images

    int myExposure = 12500;
    int frameRate  = 60;
    int frame = 1;
    bool exit = false;
    bool running = false;
    bool isRectified = false;
    int CHECKERBOARD[2]{9,6};

    std::vector<cv::Mat> caliPics;
    cv::Mat map1,map2;
    cv::Mat remapped_image;

    //calibration matrix
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    cv::Mat R;
    cv::Mat T;

    std::mutex PicsMtx;

};

#endif // XBASLERCAM_H
