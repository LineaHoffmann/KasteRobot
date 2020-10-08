#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <mutex>
#include <thread>
#include <opencv2/imgproc.hpp>

class imageHandler
{
public:
    imageHandler();
    imageHandler(cv::Mat cleanImg);

    void loadImage(cv::Mat image);
    bool cutOutTable();

    void dectectBall();
    void transformCoordinates();


    cv::Mat getInputImage() const;
    cv::Mat getTable();

private:
    bool debug = true;
    cv::Rect ROI;
    cv::Mat table;
    cv::Mat inputImage;



};

#endif // IMAGEHANDLER_H
