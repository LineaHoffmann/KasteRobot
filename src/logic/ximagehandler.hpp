#ifndef XIMAGEHANDLER_H
#define XIMAGEHANDLER_H
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <mutex>
#include <thread>
#include <opencv2/imgproc.hpp>
#include <tuple>

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif
#include "wx/log.h"



class ximageHandler
{
public:
    ximageHandler();
    ximageHandler(cv::Mat cleanImg);

    void loadImage(cv::Mat image);
    bool cutOutTable();



    //evt en funktion som samler alle loadimage, dectect, getcenter og transform i en funktion.
    std::tuple<bool, cv::Mat, cv::Point2f, float>  findBallAndPosition(cv::Mat image);

    std::pair<bool, cv::Mat> detectBall();
    void ballColor(int hue, int spread);

    cv::Mat getInputImage() const;
    cv::Mat getTable();
    cv::Point2f getPositionCM() const; //returnere i cm.

    void setMinMaxRadius(float minCM, float maxCM);
    void setRobotBase(float xcm, float ycm);

    float getRadiusCM() const; //returnere i cm.

    bool showResult = false;
private:

    bool debug = false;
    cv::Rect ROI;
    cv::Mat table;
    cv::Mat inputImage;
    std::vector<int> colorRange;
    float pixToCm;
    std::pair <float, float> minMaxRadius;
    float tableWidth = 80; //pixel to cm hackerway
    cv::Point2f centerPixel;
    float radius;
    cv::Point2f robotBase;



};

#endif // IMAGEHANDLER_H
