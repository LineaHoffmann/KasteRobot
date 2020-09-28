#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pylon/PylonIncludes.h>
#include <iostream>
#include "basler_cam.h"

int main(int argc, char* argv[])
{
    basler_cam camera1("../imgs/*.bmp", 12500);

    cv::imshow("Test Image", camera1.getImage());
    //cv::waitKey(0);
    camera1.baslerCamThread->join();



    return 1;
}
