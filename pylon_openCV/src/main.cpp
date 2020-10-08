#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pylon/PylonIncludes.h>
#include <iostream>
#include "basler_cam.h"
#include "imagehandler.h"

int main(int argc, char* argv[])
{

//    //example: given a path and exposure time
//    basler_cam camera1("../imgs/*.bmp", 12500);

//    if (camera1.start()){
//        std::cout << "camera started succesfully" << std::endl;
//    } else std::cout << "camera not connected, showing test image..." << std::endl;

//    //get latest image. if no connection is present. it will return a test pic or the latest picture from the camera.
//    cv::Mat testImg = camera1.getImage();
//    //cv::imwrite("remappedTestImg.png", testImg);

//    //returns true if we got a connection. if it returns false, we thread is finished executing and has been joined.
//    std::cout << camera1.isConnected() << std::endl;


    //if called with a image it will automagically load the image and perform table detection.
    imageHandler imgHandler(cv::imread("../src/testImg.png"));

    cv::imshow("table", imgHandler.getTable());
    cv::waitKey();



//    while (true) {
//            cv::imshow("Test Image", camera1.getImage());

//        int keyPressed = cv::waitKey(1);
//        //exit
//        if(keyPressed == 'q'){ //quit
//            std::cout << "Exiting" << std::endl;

//            camera1.shutdown();

//            cv::destroyAllWindows();
//            break;
//        }
//    }


    return 1;
}
