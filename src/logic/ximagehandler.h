#ifndef XIMAGEHANDLER_H
#define XIMAGEHANDLER_H

#include "../includeheader.h"

class xImageHandler
{
public:
    xImageHandler();
    xImageHandler(cv::Mat cleanImg);

    void loadImage(cv::Mat image);
    bool cutOutTable();

    void dectectBall();
    void transformCoordinates();


    cv::Mat getInputImage() const;
    cv::Mat getTable();

private:
    // NOTE: This debug flag must be false for GUI run
    bool debug = false;
    cv::Rect ROI;
    cv::Mat table;
    cv::Mat inputImage;

};

#endif // XIMAGEHANDLER_H
