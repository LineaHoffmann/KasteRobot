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
#include <atomic>

#include "pylon/PylonIncludes.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "wx/log.h"

#include "xexceptions.hpp"

class xBaslerCam
{
public:
    xBaslerCam(const std::string& calibrationPath = "../imgs/*.bmp",
               const double exposure = 12500.0,
               const uint64_t framerate = 60);
    ~xBaslerCam();
    static void liveCalibration(std::shared_ptr<xBaslerCam> liveCamera, std::string path);

    void updateMapping(std::pair<cv::Mat, cv::Mat> newMapping);
    std::pair<cv::Mat, cv::Mat> getMapping();

    bool isConnected(); //returns true if we have a connection to the camera.
    bool start(); //returns true if the camera was started correctly.
    void shutdown();

    void setPath(const std::string& calibrationPath) {mPath = calibrationPath;}
    void calibrate(); //run calibration on pictures in path

    bool hasNewImage(); // Checks if a new picture is available - THREADSAFE
    const cv::Mat getImage(); // get newest cv:Mat image (remapped) - THREADSAFE

    std::thread *baslerCamThread; //skal muligvis senere flyttes til private.

    void setMyExposure(double value);

    void setFrameRate(uint64_t value);

private:
    void GrabPictures(); //polling pictures from cam to memory

    // Create a PylonImage that will be used to create OpenCV images later.
    Pylon::CPylonImage pylonImage;
    cv::Mat openCvImage;    // Create an OpenCV image.
    std::string mPath;     // Path of the folder containing checkerboard images

    // Atomics for thread safety
    std::atomic<bool> mIsRunning;
    std::atomic<bool> mExit;
    std::atomic<bool> mHasNewImage;

    double mExposure;
    uint64_t mFramerate;
    uint64_t mFrame = 1;
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
