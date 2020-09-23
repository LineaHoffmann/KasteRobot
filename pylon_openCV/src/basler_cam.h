#ifndef BASLER_CAM_H
#define BASLER_CAM_H
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pylon/PylonIncludes.h>
#include <iostream>


class basler_cam
{
public:
    basler_cam(std::string calibrationPath);
    basler_cam(std::string calibrationPath, int exposure);
    void calibrate();
    void calibrate(std::vector<cv::Mat> imageList);



private:
    // The exit code of the sample application.
    int exitCode = 0;

    // Create a PylonImage that will be used to create OpenCV images later.
    Pylon::CPylonImage pylonImage;
    // Create an OpenCV image.
    cv::Mat openCvImage;
    // Path of the folder containing checkerboard images
    std::string path = "../imgs/*.bmp";

    int myExposure = 12500;
    int frame = 1;
    bool isCalibrated = false;
    int CHECKERBOARD[2]{9,6};

    std::vector<cv::Mat> caliPics;
    cv::Mat map1,map2;
    cv::Mat remapped_image;

    //calibration matrix
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    cv::Mat R;
    cv::Mat T;
};

#endif // BASLER_CAM_H
