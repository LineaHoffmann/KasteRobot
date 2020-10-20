#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pylon/PylonIncludes.h>
#include <iostream>
#include "basler_cam.h"
#include "ximagehandler.h"

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
    ximageHandler imgHandler(cv::imread("../src/testImg.png"));
    imgHandler.showResult = true;


    //setup search crits
    imgHandler.ballColor(10, 20); //set what color ball we are looking for
    imgHandler.setMinMaxRadius(3, 4);
    imgHandler.setRobotBase(42.2, 8.8);

    //load img and find ball
    imgHandler.loadImage(cv::imread("../balls/remappedBall1.png"));
    if (imgHandler.dectectBall()) { //find the ball
    std::cout << "position: " << imgHandler.getPositionCM() << " || ball radius: " << imgHandler.getRadiusCM() << std::endl;
    cv::waitKey();
    }

    imgHandler.loadImage(cv::imread("../balls/remappedBall2.png"));
    if (imgHandler.dectectBall()) {
    std::cout << "position: " << imgHandler.getPositionCM() << " || ball radius: " << imgHandler.getRadiusCM() << std::endl;
    cv::waitKey();
    }



    std::cout << "position: " << imgHandler.findBallAndPosition(cv::imread("../balls/remappedBall3.png")).first
              << " || ball radius: " << imgHandler.findBallAndPosition(cv::imread("../balls/remappedBall3.png")).second << std::endl;
    cv::waitKey();

    std::pair<cv::Point2f, float> ball = imgHandler.findBallAndPosition(cv::imread("../balls/remappedBall4.png"));
    std::cout << "position: " << ball.first
              << " || ball radius: " << ball.second << std::endl;
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
