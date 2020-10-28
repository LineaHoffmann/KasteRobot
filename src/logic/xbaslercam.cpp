#include "xbaslercam.hpp"
#include <chrono>

xBaslerCam::xBaslerCam(){
    // Atomic bool default values
    mIsRunning.exchange(false);
    mExit.exchange(false);
    mHasNewImage.exchange(false);
}
xBaslerCam::xBaslerCam(std::string calibrationPath)
    : xBaslerCam() {
    path = calibrationPath;
}
xBaslerCam::xBaslerCam(std::string calibrationPath, uint32_t exposure)
    : xBaslerCam(calibrationPath) {
    myExposure = exposure;
}
xBaslerCam::xBaslerCam(std::string calibrationPath, uint32_t exposure, uint32_t maxFrameRate)
    : xBaslerCam(calibrationPath, exposure) {
    frameRate = maxFrameRate;
}
xBaslerCam::~xBaslerCam(){}
bool xBaslerCam::isConnected() {return mIsRunning.load();} // This must be thread safe
bool xBaslerCam::start()
{
    calibrate();
    baslerCamThread = new std::thread(&xBaslerCam::GrabPictures,this);
//    auto start_time = std::chrono::steady_clock::now();
//    auto current_time = std::chrono::steady_clock::now();
//    while(true) {
//        current_time = std::chrono::steady_clock::now();
//        if (running){
//            return 1;
//        }
//        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count() > 10){
//            //|| baslerCamThread->joinable()){
//            return 0;
//        }
//    }
    return true;
}
void xBaslerCam::shutdown() {mExit.exchange(true);}
void xBaslerCam::calibrate()
{
    //calibrate
    std::stringstream print; // String stream for printing to wxLog
    print << path;
    logstd(print.str().c_str());
    //std::cout << "calibration started. Using pictures from folder: " + path << std::endl;

    cv::Mat frame;
    std::vector<cv::String> images;
    cv::glob(path, images);
    if (images.size() == 0) // If the vector is empty, it didn't get any images
        throw x_err::error(x_err::what::CAMERA_WRONG_PATH);

    std::vector<std::vector<cv::Point3f> > objpoints;
    std::vector<std::vector<cv::Point2f> > imgpoints;

    // vector to store the pixel coordinates of detected checker board corners
    std::vector<cv::Point2f> corner_pts;

    // Defining the world coordinates for 3D points
    std::vector<cv::Point3f> objp;
    for(int32_t i{0}; i<CHECKERBOARD[1]; i++)
    {
        for(int32_t j{0}; j<CHECKERBOARD[0]; j++)
            objp.push_back(cv::Point3f(j,i,0));
    }

    bool success;
    for(uint i{0}; i<images.size(); i++)
    {
        //std::cout << "." << std::flush;
        frame = cv::imread(images[i]);
        cv::cvtColor(frame,frame,cv::COLOR_BGR2GRAY);
        //find corners
        success = cv::findChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
        if(success)
        {
            cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);
            cv::cornerSubPix(frame,corner_pts,cv::Size(11,11), cv::Size(-1,-1),criteria);
            //std::cout << "." << std::flush;
            cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);
            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        }
    }
    cv::calibrateCamera(objpoints, imgpoints, cv::Size(frame.rows,frame.cols), cameraMatrix, distCoeffs, R, T);
    print.clear();
    print << "Camera Matric: " << cameraMatrix;
    logstd(print.str().c_str());
    print.clear();
    print << "Distortion Coefficients: " << distCoeffs;
    logstd(print.str().c_str());
    print.clear();
    print << "Rotation Vector: " << R;
    logstd(print.str().c_str());
    print.clear();
    print << "Translation Vector: " << T;
    logstd(print.str().c_str());
    //std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
    //std::cout << "distCoeffs : " << distCoeffs << std::endl;
    //std::cout << "Rotation vector : " << R << std::endl;
    //std::cout << "Translation vector : " << T << std::endl;
    isRectified = false;
}

void xBaslerCam::updateCameraMatrix(cv::Mat NewCameraMatrix, cv::Mat NewCoeffs)
{
    cameraMatrix = NewCameraMatrix;
    distCoeffs = NewCoeffs;
    isRectified = false;
}

/*
 * THESE TWO CALLS MUST BE THREAD SAFE!
*/
bool xBaslerCam::hasNewImage() {return mHasNewImage.load();}
const cv::Mat xBaslerCam::getImage()
{
    std::lock_guard<std::mutex> lock(mMtx);
    if (openCvImage.data || mIsRunning.load()) {
        // Returning a clone, not by reference, to ensure that the
        // local data isn't accessed from caller after this function
        return openCvImage.clone();
    } else {
        // Maybe a warning image should be allocated permanently ..
        // We generally shouldn't end here
        return cv::imread("../resources/testImg.png", cv::IMREAD_COLOR);
    }

    // TODO: [srp] Don't do remapping in here, do it in the worker thread
    //       Remapping here would be much worse to make thread safe

//    //get pic and remap
//    if (!openCvImage.data || !mIsRunning.load()) {
//    openCvImage = cv::imread("../resources/testImg.png", cv::IMREAD_COLOR);

//    //fjern hvis billede skal gennem remapping
//    return openCvImage;
//    }

//    if (!isRectified){
//        cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cameraMatrix, cv::Size(openCvImage.cols,openCvImage.rows), CV_32FC1, map1, map2);
//        isRectified  = true;
//    }

//    cv::remap(openCvImage,remapped_image,map1,map2,cv::INTER_LINEAR);
//    return remapped_image;
}

/**
 * @brief Worker thread function
 */
void xBaslerCam::GrabPictures()
{
    std::stringstream s; // For printing to the wxLog
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
            logstd("Exposure auto disabled.");
            //std::cout << "Exposure auto disabled." << std::endl;
        }

        // Set gain auto once
        GenApi::CEnumerationPtr gainAuto( nodemap.GetNode( "GainAuto"));
        if ( GenApi::IsWritable( gainAuto)){
            gainAuto->FromString("Once");
            logstd("Auto gain once enabled.");
            //std::cout << "Auto gain once enabled." << std::endl;
        }

        // Set Balance White Auto once
        GenApi::CEnumerationPtr balanceWhiteAuto( nodemap.GetNode( "BalanceWhiteAuto"));
        if ( GenApi::IsWritable( balanceWhiteAuto)){
            balanceWhiteAuto->FromString("Once");
            logstd("BalanceWhiteAuto once enabled.");
            //std::cout << "BalanceWhiteAuto once enabled." << std::endl;
        }

        // Set custom exposure
        GenApi::CFloatPtr exposureTime = nodemap.GetNode("ExposureTime");

        s.str(std::string()); // Reset the stringstream
        s << "Old exposure: " << exposureTime->GetValue();
        logstd(s.str().c_str());
        //std::cout << "Old exposure: " << exposureTime->GetValue() << std::endl;
        if(exposureTime.IsValid()) {
            if(myExposure >= exposureTime->GetMin() && myExposure <= exposureTime->GetMax()) {
                exposureTime->SetValue(myExposure);
            } else {
                exposureTime->SetValue(exposureTime->GetMin());
                logstd(">> Exposure has been set with the minimum available value.");
                s.str(std::string());
                s << ">> The available exposure range is [" << exposureTime->GetMin() << " - " << exposureTime->GetMax() << "] (us)";
                logstd(s.str().c_str());
                //std::cout << ">> Exposure has been set with the minimum available value." << std::endl;
                //std::cout << ">> The available exposure range is [" << exposureTime->GetMin() << " - " << exposureTime->GetMax() << "] (us)" << std::endl;
            }
        } else {
            logstd(">> Failed to set exposure value.");
            //std::cout << ">> Failed to set exposure value." << std::endl;
        }
        s.str(std::string());
        s << "New exposure: " << exposureTime->GetValue();
        logstd(s.str().c_str());
        //std::cout << "New exposure: " << exposureTime->GetValue() << std::endl;

        //TEST FOR FRAMERATE CAP!! HAVE NOT BEEN TESTED LIVE YET!//
        // enable framerate cap
        GenApi::CEnumerationPtr AcquisitionFrameRateEnable( nodemap.GetNode( "AcquisitionFrameRateEnable"));
        if ( GenApi::IsWritable( AcquisitionFrameRateEnable)){
            AcquisitionFrameRateEnable->FromString("true");
            logstd("AcquisitionFrameRateEnable enabled.");
            //std::cout << "AcquisitionFrameRateEnable enabled." << std::endl;
        }
        // set framerate cap
        GenApi::CEnumerationPtr AcquisitionFrameRate( nodemap.GetNode( "AcquisitionFrameRate"));
        if(AcquisitionFrameRate.IsValid()) {
            AcquisitionFrameRate->SetIntValue(frameRate);
            s.str(std::string());
            s << "AcquisitionFrameRate set to: " << frameRate;
            logstd(s.str().c_str());
            //std::cout << "AcquisitionFrameRate set to: " << frameRate << std::endl;
        }

        // sets up free-running continuous acquisition.
        camera.StartGrabbing(Pylon::GrabStrategy_LatestImageOnly);

        // This smart pointer will receive the grab result data.
        Pylon::CGrabResultPtr ptrGrabResult;

        // image grabbing loop
        while ( camera.IsGrabbing())
        {
            mIsRunning.exchange(true);

            // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
            camera.RetrieveResult( 5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded()) {
                // Convert the grabbed buffer to a pylon image.
                formatConverter.Convert(pylonImage, ptrGrabResult);

                // Create an OpenCV image from a pylon image.
                std::lock_guard<std::mutex> lock(mMtx);
                openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) pylonImage.GetBuffer());
                mHasNewImage.exchange(true); // Set flag for new image

                frame++;
                if(mExit.load()) {
                    camera.Close();
                    mIsRunning.exchange(false);
                    return;
                }
            } else {
                s.str(std::string());
                s  << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription();
                logstd(s.str().c_str());
                //std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << std::endl;
                return;
            }
        }
    }
    catch (GenICam::GenericException &e)
    {
        // Error handling.

        s.str(std::string());
        s << "An exception occurred." << std::endl
          << e.GetDescription();
        logerr(s.str().c_str());

//        std::cerr << "An exception occurred." << std::endl
//                  << e.GetDescription() << std::endl;
        mIsRunning.exchange(false);
        // Rethrowing as logic error
        //throw x_err::error(x_err::what::CAMERA_GRAB_ERROR + e.what());
    }
}
