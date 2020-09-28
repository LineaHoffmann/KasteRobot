#ifndef BASLER_CAM_H
#define BASLER_CAM_H
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pylon/PylonIncludes.h>
#include <iostream>
#include <mutex>
#include <thread>


class basler_cam
{
public:
    basler_cam(std::string calibrationPath);
    basler_cam(std::string calibrationPath, int exposure);

    ~basler_cam();

    void calibrate(); //run calibration on pictures in path
    void updateCameraMatrix(cv::Mat NewCameraMatrix, cv::Mat NewCoeffs); //changing calibration manually
    void GrabPictures(); //runs to get pictures from camera
    cv::Mat getImage(); //get newest cv:Mat image (remapped)


    std::thread *baslerCamThread;

private:

    // Create a PylonImage that will be used to create OpenCV images later.
    Pylon::CPylonImage pylonImage;
    cv::Mat openCvImage;    // Create an OpenCV image.
    std::string path = "../imgs/*.bmp";     // Path of the folder containing checkerboard images

    int myExposure = 12500;
    int frame = 1;
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

    std::mutex *PicsMtx = nullptr;

};

#endif // BASLER_CAM_H
