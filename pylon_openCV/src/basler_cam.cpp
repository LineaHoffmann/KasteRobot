#include "basler_cam.h"

basler_cam::basler_cam()
{
    PicsMtx = new std::mutex();
}

basler_cam::basler_cam(std::string calibrationPath) : basler_cam()
{path = calibrationPath;}
basler_cam::basler_cam(std::string calibrationPath, int exposure) : basler_cam(calibrationPath)
{myExposure = exposure;}
basler_cam::basler_cam(std::string calibrationPath, int exposure, int maxFrameRate) : basler_cam(calibrationPath, exposure)
{frameRate = maxFrameRate;}



basler_cam::~basler_cam()
{
    delete PicsMtx;
}

bool basler_cam::isConnected()
{
    if(baslerCamThread->joinable()) {
        baslerCamThread->join();
        return 0;
    }
    return 1;
}

bool basler_cam::start()
{
    // !!!! husk at tilføje calibrate igen
    //calibrate();
    baslerCamThread = new std::thread(&basler_cam::GrabPictures,this);
    if (!openCvImage.data) {
        return 0;
    }
    return 1;
}

void basler_cam::shutdown()
{
    exit = true;
}


void basler_cam::calibrate()
{
    //calibrate
    std::cout << "calibration started. Using pictures from folder: " + path << std::endl;

    std::vector<cv::String> images;
    cv::glob(path, images);
    cv::Mat frame;

    std::vector<std::vector<cv::Point3f> > objpoints;
    std::vector<std::vector<cv::Point2f> > imgpoints;

    // vector to store the pixel coordinates of detected checker board corners
    std::vector<cv::Point2f> corner_pts;

    // Defining the world coordinates for 3D points
    std::vector<cv::Point3f> objp;
    for(int i{0}; i<CHECKERBOARD[1]; i++)
    {
        for(int j{0}; j<CHECKERBOARD[0]; j++)
            objp.push_back(cv::Point3f(j,i,0));
    }


    bool success;
    for(int i{0}; i<images.size(); i++)
    {
        std::cout << "." << std::flush;
        frame = cv::imread(images[i]);
        cv::cvtColor(frame,frame,cv::COLOR_BGR2GRAY);
        //find corners
        success = cv::findChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
        if(success)
        {
            cv::TermCriteria criteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.001);
            cv::cornerSubPix(frame,corner_pts,cv::Size(11,11), cv::Size(-1,-1),criteria);
            std::cout << "." << std::flush;
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
    isRectified = false;
}

void basler_cam::updateCameraMatrix(cv::Mat NewCameraMatrix, cv::Mat NewCoeffs)
{
    cameraMatrix = NewCameraMatrix;
    distCoeffs = NewCoeffs;
    isRectified = false;
}

cv::Mat basler_cam::getImage()
{
    std::lock_guard<std::mutex> lock(*PicsMtx);
    //get pic and remap
    if (!openCvImage.data) {
    openCvImage = cv::imread("../imgs/Image__2020-09-17__02-51-54.bmp", CV_LOAD_IMAGE_COLOR);

    //fjern hvis billede skal gennem remapping
    return openCvImage;
    }

    if (!isRectified){
        cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cameraMatrix, cv::Size(openCvImage.cols,openCvImage.rows), CV_32FC1, map1, map2);
        isRectified  = true;
    }

    cv::remap(openCvImage,remapped_image,map1,map2,cv::INTER_LINEAR);
    return remapped_image;
}

void basler_cam::GrabPictures()
{
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
        }
        std::cout << "New exposure: " << exposureTime->GetValue() << std::endl;


        //TEST FOR FRAMERATE CAP!! HAVE NOT BEEN TESTED LIVE YET!//
        // enable framerate cap
        GenApi::CEnumerationPtr AcquisitionFrameRateEnable( nodemap.GetNode( "AcquisitionFrameRateEnable"));
        if ( GenApi::IsWritable( AcquisitionFrameRateEnable)){
            AcquisitionFrameRateEnable->FromString("true");
            std::cout << "AcquisitionFrameRateEnable enabled." << std::endl;
        }
        // set framerate cap
        GenApi::CEnumerationPtr AcquisitionFrameRate( nodemap.GetNode( "AcquisitionFrameRate"));
        if(AcquisitionFrameRate.IsValid()) {
            AcquisitionFrameRate->SetIntValue(frameRate);
            std::cout << "AcquisitionFrameRate set to: " << frameRate << std::endl;
        }




        // sets up free-running continuous acquisition.
        camera.StartGrabbing(Pylon::GrabStrategy_LatestImageOnly);

        // This smart pointer will receive the grab result data.
        Pylon::CGrabResultPtr ptrGrabResult;


        // image grabbing loop
        while ( camera.IsGrabbing())
        {
            // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
            camera.RetrieveResult( 5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

            if(exit) {
                camera.Close();
                return;
            }

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded())
            { // Convert the grabbed buffer to openCV image
                {
                    // Convert the grabbed buffer to a pylon image.
                    formatConverter.Convert(pylonImage, ptrGrabResult);

                    // Create an OpenCV image from a pylon image.
                    std::lock_guard<std::mutex> lock(*PicsMtx);
                    openCvImage= cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) pylonImage.GetBuffer());
                }

                frame++;

            }
            else
            {
                std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
                return;
            }
        }

    }
    catch (GenICam::GenericException &e)
    {
        // Error handling.
        std::cerr << "An exception occurred." << std::endl
                  << e.GetDescription() << std::endl;

        cv::Mat warningImg;
        warningImg = cv::imread("../src/warning.jpg", CV_LOAD_IMAGE_COLOR);
        cv::imshow( "warning", warningImg);
    }
}
