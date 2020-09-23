#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pylon/PylonIncludes.h>
#include <iostream>
#include "basler_cam.h"

int main(int argc, char* argv[])
{
    basler_cam camera1("../imgs/*.bmp", 3000);

    int myExposure = 12500;
    // The exit code of the sample application.
    int exitCode = 0;
    int frame = 1;
    // Create a PylonImage that will be used to create OpenCV images later.
    Pylon::CPylonImage pylonImage;
    // Create an OpenCV image.
    cv::Mat openCvImage;


    //my vars
    // Path of the folder containing checkerboard images
    std::string path = "../imgs/*.bmp";

    bool isCalibrated = false;
    std::vector<cv::Mat> caliPics;
    int CHECKERBOARD[2]{9,6};
    cv::Mat map1,map2;
    cv::Mat remapped_image;

    //Create calibration matrix
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    cv::Mat R;
    cv::Mat T;

    //run precalibration
    {
            std::vector<cv::String> images;
            cv::glob(path, images);

            std::cout << "calibration started" << std::endl;
            std::cout << "using pictures from folder: " + path << std::endl;
            std::vector<std::vector<cv::Point3f> > objpoints;
            std::vector<std::vector<cv::Point2f> > imgpoints;

            // Defining the world coordinates for 3D points
            std::vector<cv::Point3f> objp;
            for(int i{0}; i<CHECKERBOARD[1]; i++)
            {
                for(int j{0}; j<CHECKERBOARD[0]; j++)
                    objp.push_back(cv::Point3f(j,i,0));
            }
            cv::Mat frame; //frame not needed in normal use
            // vector to store the pixel coordinates of detected checker board corners
            std::vector<cv::Point2f> corner_pts;
            bool success;
            for(int i{0}; i<images.size(); i++)
                  {
                    frame = cv::imread(images[i]);
                    cv::cvtColor(frame,frame,cv::COLOR_BGR2GRAY);
                //find corners
                success = cv::findChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
                if(success)
                {
                    cv::TermCriteria criteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.001);
                    cv::cornerSubPix(frame,corner_pts,cv::Size(11,11), cv::Size(-1,-1),criteria);
                    cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);
                    objpoints.push_back(objp);
                    imgpoints.push_back(corner_pts);
                }
            }
            cv::calibrateCamera(objpoints, imgpoints, cv::Size(frame.rows,frame.cols), cameraMatrix, distCoeffs, R, T);
            std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
            std::cout << "distCoeffs : " << distCoeffs << std::endl;
            std::cout << "Rotation vector : " << R << std::endl;
            std::cout << "Translation vector : " << T << std::endl;
        }



    // Automagically call PylonInitialize and PylonTerminate to ensure the pylon runtime system
    // is initialized during the lifetime of this object.
    Pylon::PylonAutoInitTerm autoInitTerm;

    try
    {

        // Create an instant camera object with the camera device found first.
        Pylon::CInstantCamera camera( Pylon::CTlFactory::GetInstance().CreateFirstDevice());

        // Get a camera nodemap in order to access camera parameters.
        GenApi::INodeMap& nodemap= camera.GetNodeMap();

        // Open the camera before accessing any parameters.
        camera.Open();
        // Create pointers to access the camera Width and Height parameters.
        GenApi::CIntegerPtr width= nodemap.GetNode("Width");
        GenApi::CIntegerPtr height= nodemap.GetNode("Height");

        // The parameter MaxNumBuffer can be used to control the count of buffers
        // allocated for grabbing. The default value of this parameter is 10.
        //camera.MaxNumBuffer = 5;

        // Create a pylon ImageFormatConverter object.
        Pylon::CImageFormatConverter formatConverter;
        // Specify the output pixel format.
        formatConverter.OutputPixelFormat= Pylon::PixelType_BGR8packed;




        // Set exposure to manual
        GenApi::CEnumerationPtr exposureAuto( nodemap.GetNode( "ExposureAuto"));
        if ( GenApi::IsWritable( exposureAuto)){
            exposureAuto->FromString("Off");
            std::cout << "Exposure auto disabled." << std::endl;
        }


        // Set gain auto once
        GenApi::CEnumerationPtr gainAuto( nodemap.GetNode( "GainAuto"));
        if ( GenApi::IsWritable( gainAuto)){
            gainAuto->FromString("Once");
            std::cout << "Auto gain once enabled." << std::endl;
        }

        // Set Balance White Auto once
        GenApi::CEnumerationPtr balanceWhiteAuto( nodemap.GetNode( "BalanceWhiteAuto"));
        if ( GenApi::IsWritable( balanceWhiteAuto)){
            balanceWhiteAuto->FromString("Once");
            std::cout << "BalanceWhiteAuto once enabled." << std::endl;
        }


        // Set custom exposure
        GenApi::CFloatPtr exposureTime = nodemap.GetNode("ExposureTime");
        std::cout << "Old exposure: " << exposureTime->GetValue() << std::endl;
        if(exposureTime.IsValid()) {
            if(myExposure >= exposureTime->GetMin() && myExposure <= exposureTime->GetMax()) {
                exposureTime->SetValue(myExposure);
            }else {
                exposureTime->SetValue(exposureTime->GetMin());
                std::cout << ">> Exposure has been set with the minimum available value." << std::endl;
                std::cout << ">> The available exposure range is [" << exposureTime->GetMin() << " - " << exposureTime->GetMax() << "] (us)" << std::endl;
            }
        }else {

            std::cout << ">> Failed to set exposure value." << std::endl;
            return false;
        }
        std::cout << "New exposure: " << exposureTime->GetValue() << std::endl;

        // Start the grabbing of c_countOfImagesToGrab images.
        // The camera device is parameterized with a default configuration which
        // sets up free-running continuous acquisition.
        camera.StartGrabbing(Pylon::GrabStrategy_LatestImageOnly);

        // This smart pointer will receive the grab result data.
        Pylon::CGrabResultPtr ptrGrabResult;


        // image grabbing loop
        while ( camera.IsGrabbing())
        {
            // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
            camera.RetrieveResult( 5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded())
            { // Convert the grabbed buffer to openCV image
                {
                // Convert the grabbed buffer to a pylon image.
                formatConverter.Convert(pylonImage, ptrGrabResult);
                // Create an OpenCV image from a pylon image.
                openCvImage= cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) pylonImage.GetBuffer());
                }

                // Create an OpenCV display window.
                cv::namedWindow( "live view", CV_WINDOW_AUTOSIZE); // other options: CV_AUTOSIZE, CV_FREERATIO
                // Display the current image in the OpenCV display window.
                cv::imshow( "live view", openCvImage);

                //remapping and displaying
                if (!isCalibrated){
                cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cameraMatrix, cv::Size(openCvImage.cols,openCvImage.rows), CV_32FC1, map1, map2);
                isCalibrated = true;
                }
                cv::remap(openCvImage,remapped_image,map1,map2,cv::INTER_LINEAR);
                cv::imshow("live view with remapping",remapped_image);

                //detect keypress
                int keyPressed = cv::waitKey(1);

                //press c for calibration
                if (keyPressed == 'c') {
                    std::cout << "calibration started" << std::endl;

                    std::vector<std::vector<cv::Point3f> > objpoints; // Creating vector to store vectors of 3D points for each checkerboard image
                    std::vector<std::vector<cv::Point2f> > imgpoints; // Creating vector to store vectors of 2D points for each checkerboard image

                    // Defining the world coordinates for 3D points
                    std::vector<cv::Point3f> objp;
                    for(int i{0}; i<CHECKERBOARD[1]; i++)
                    {
                        for(int j{0}; j<CHECKERBOARD[0]; j++)
                            objp.push_back(cv::Point3f(j,i,0));
                    }

                    // vector to store the pixel coordinates of detected checker board corners
                    std::vector<cv::Point2f> corner_pts;
                    bool success;
                    for (cv::Mat saved_image : caliPics) {
                        //find corners
                        success = cv::findChessboardCorners(saved_image, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

                        /*
 * If desired number of corner are detected,
 * we refine the pixel coordinates and display
 * them on the images of checker board
 */
                        if(success)
                        {
                            cv::TermCriteria criteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.001);
                            cv::cornerSubPix(saved_image,corner_pts,cv::Size(11,11), cv::Size(-1,-1),criteria); // refining pixel coordinates for given 2d points.
                            cv::drawChessboardCorners(saved_image, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success); // Displaying the detected corner points on the checker board
                            objpoints.push_back(objp);
                            imgpoints.push_back(corner_pts);
                        }
                        cv::imshow("Calibration Image",saved_image);
                        cv::waitKey(0);
                    }
                    /*
            Performing camera calibration by
            passing the value of known 3D points (objpoints)
            and corresponding pixel coordinates of the
            detected corners (imgpoints)
            */
                    cv::calibrateCamera(objpoints, imgpoints, cv::Size(openCvImage.rows,openCvImage.cols), cameraMatrix, distCoeffs, R, T);
                    std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
                    std::cout << "distCoeffs : " << distCoeffs << std::endl;
                    std::cout << "Rotation vector : " << R << std::endl;
                    std::cout << "Translation vector : " << T << std::endl;
                    //end of calibration
                    isCalibrated = false;
                }

                //press q to exit
                if(keyPressed == 'q'){ //quit
                    std::cout << "Shutting down camera..." << std::endl;
                    camera.Close();
                    std::cout << "Camera successfully closed." << std::endl;
                    break;
                }

                //take and save images press p
                if(keyPressed == 'p'){ //take image
                    cv::cvtColor(openCvImage, openCvImage,cv::COLOR_BGR2GRAY);
                    caliPics.push_back(openCvImage);
                    std::cout << "image taken" << std::endl;
                    cv::imshow( "saved image", caliPics.back());
                }

                frame++;

            }
            else
            {
                std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
            }
        }

    }
    catch (GenICam::GenericException &e)
    {
        // Error handling.
        std::cerr << "An exception occurred." << std::endl
                  << e.GetDescription() << std::endl;
        std::cout << "Could not find camera, do you want to exit?" << std::endl;
        std::cout << "press q to exit, or t to test with pictures from disk" << std::endl;

        cv::Mat warningImg;
        warningImg = cv::imread("../src/warning.jpg", CV_LOAD_IMAGE_COLOR);
        cv::imshow( "warning", warningImg);
        while (true) {
            int keyPressed = cv::waitKey(1);

            //exit
            if(keyPressed == 'q'){ //quit
                std::cout << "Exiting" << std::endl;
                exitCode = 1;
                break;
            }

            //start test
            if(keyPressed == 't'){
                std::cout << "starting test" << std::endl;
                exitCode = 2;
                break;
            }
        }
    }
    // run test code  using pictures from folder.
    if (exitCode == 2) {

        // Extracting path of individual image stored in a given directory
        // for reading from folder
        std::vector<cv::String> images;
        cv::glob(path, images);
        int CHECKERBOARD[2]{9,6};


        std::cout << "using pictures from folder: " + path << std::endl;

        //run calibration
        std::cout << "calibration started" << std::endl;

        // Creating vector to store vectors of 3D points for each checkerboard image
        std::vector<std::vector<cv::Point3f> > objpoints;
        // Creating vector to store vectors of 2D points for each checkerboard image
        std::vector<std::vector<cv::Point2f> > imgpoints;

        // Defining the world coordinates for 3D points
        std::vector<cv::Point3f> objp;
        for(int i{0}; i<CHECKERBOARD[1]; i++)
        {
            for(int j{0}; j<CHECKERBOARD[0]; j++)
                objp.push_back(cv::Point3f(j,i,0));
        }
        std::cout << "1" << std::endl;




        cv::Mat frame, gray; //frame not needed in normal use
        // vector to store the pixel coordinates of detected checker board corners
        std::vector<cv::Point2f> corner_pts;
        bool success;
        for(int i{0}; i<images.size(); i++)
              {
                frame = cv::imread(images[i]);
                cv::cvtColor(frame,gray,cv::COLOR_BGR2GRAY);
            //find corners
            success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
            //success = cv::findChessboardCorners(gray,cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts);

            /*
If desired number of corner are detected,
we refine the pixel coordinates and display
them on the images of checker board
*/


            if(success)
            {
                cv::TermCriteria criteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.001);
                // refining pixel coordinates for given 2d points.
                cv::cornerSubPix(gray,corner_pts,cv::Size(11,11), cv::Size(-1,-1),criteria);
                // Displaying the detected corner points on the checker board
                cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);
                objpoints.push_back(objp);
                imgpoints.push_back(corner_pts);
            }
            cv::imshow("Calibration Image",frame);
            std::cout << "Done" << std::endl;
            cv::waitKey(0);
        }
        cv::destroyAllWindows();

        cv::Mat cameraMatrix,distCoeffs,R,T;
        /*
Performing camera calibration by
passing the value of known 3D points (objpoints)
and corresponding pixel coordinates of the
detected corners (imgpoints)
*/
        cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows,gray.cols), cameraMatrix, distCoeffs, R, T);
        std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
        std::cout << "distCoeffs : " << distCoeffs << std::endl;
        std::cout << "Rotation vector : " << R << std::endl;
        std::cout << "Translation vector : " << T << std::endl;


        //remapping function
        cv::Mat map1,map2;
        cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cameraMatrix, cv::Size(frame.cols,frame.rows), CV_32FC1, map1, map2);

        for(int i{0}; i<images.size(); i++)
        {
            frame = cv::imread(images[i]);
            cv::imshow("before",frame);

            cv::remap(frame,frame,map1,map2,cv::INTER_LINEAR);

            cv::imshow("after",frame);
            cv::waitKey(0);
        }


        exitCode = 1;
    }


    return exitCode;
}
