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


//    //returns true if we got a connection. if it returns false, we thread is finished executing and has been joined.
//    std::cout << camera1.isConnected() << std::endl;


    //if called with a image it will automagically load the image and perform table detection.
    imageHandler imgHandler(cv::imread("../src/testImg.png"));
    cv::imshow("table", imgHandler.getTable());

    imgHandler.loadImage(cv::imread("../balls/remappedBall1.png"));
    cv::imshow("table", imgHandler.getTable());
    imgHandler.dectectBall();
    cv::waitKey();

    imgHandler.loadImage(cv::imread("../balls/remappedBall2.png"));
    cv::imshow("table", imgHandler.getTable());
    imgHandler.dectectBall();
    cv::waitKey();

    imgHandler.loadImage(cv::imread("../balls/remappedBall3.png"));
    cv::imshow("table", imgHandler.getTable());
    imgHandler.dectectBall();
    cv::waitKey();

    imgHandler.loadImage(cv::imread("../balls/remappedBall4.png"));
    cv::imshow("table", imgHandler.getTable());
    imgHandler.dectectBall();
    cv::waitKey();





//    while (true) {

//            cv::Mat testImg = camera1.getImage();
//            cv::imshow("Test Image", testImg);


//        int keyPressed = cv::waitKey(1);
//        //exit
//        if(keyPressed == 's'){ //quit
//            cv::imwrite("remappedBall.png", testImg);
//           }
//        if(keyPressed == 'q'){ //quit
//            std::cout << "Exiting" << std::endl;

//            camera1.shutdown();

//            cv::destroyAllWindows();
//            break;
//        }
//    }


    return 1;
}
