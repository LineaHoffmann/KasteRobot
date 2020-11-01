#include "xbaslercam.hpp"

xBaslerCam::xBaslerCam(const std::string& calibrationPath,
                       const double exposure,
                       const uint64_t framerate) {
    mPath = calibrationPath;
    mExposure = exposure;
    mFramerate = framerate;

    // Atomic bool default values
    mIsRunning.exchange(false);
    mExit.exchange(false);
    mHasNewImage.exchange(false);
    openCvImage = cv::imread("../resources/testImg.png");
}
xBaslerCam::~xBaslerCam(){}

void xBaslerCam::liveCalibration(std::shared_ptr<xBaslerCam> liveCamera, std::string path)
{
    xBaslerCam tempCamera(path);
    logstd("Calibration: start calculating mapping");
    tempCamera.calibrate();

    std::pair<cv::Mat, cv::Mat> test = tempCamera.getMapping();
    liveCamera->updateMapping(test);
    logstd("Calibration: Mapping updated");
}

void xBaslerCam::updateMapping(std::pair<cv::Mat, cv::Mat> newMapping)
{
    map1 = newMapping.first;
    map2 = newMapping.second;
}

std::pair<cv::Mat, cv::Mat> xBaslerCam::getMapping()
{
    return std::pair<cv::Mat, cv::Mat>(map1, map2);
}

bool xBaslerCam::isConnected() {return mIsRunning.load();} // This must be thread safe
bool xBaslerCam::start()
{
    if (!isRectified) calibrate();
    baslerCamThread = new std::thread(&xBaslerCam::GrabPictures,this);
    return true;
}
void xBaslerCam::shutdown() {mExit.exchange(true);}
void xBaslerCam::calibrate()
{
    //calibrate
    logstd(std::string("Calibrating camera using path: ").append(mPath).c_str());

    cv::Mat frame;
    std::vector<cv::String> images;
    cv::glob(mPath, images);
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
        frame = cv::imread(images[i]);
        cv::cvtColor(frame,frame,cv::COLOR_BGR2GRAY);
        //find corners
        success = cv::findChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
        if(success)
        {
            cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);
            cv::cornerSubPix(frame,corner_pts,cv::Size(11,11), cv::Size(-1,-1),criteria);
            cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);
            objpoints.push_back(objp);
            imgpoints.push_back(corner_pts);
        }
    }
    cv::calibrateCamera(objpoints, imgpoints, cv::Size(frame.rows,frame.cols), cameraMatrix, distCoeffs, R, T);

    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(), cameraMatrix, cv::Size(openCvImage.cols,openCvImage.rows), CV_32FC1, map1, map2);

    isRectified  = true;
    logstd("Camera calibration done ..");
}


/*
 * THESE TWO CALLS MUST BE THREAD SAFE!
*/
bool xBaslerCam::hasNewImage() {return mHasNewImage.load();}
const cv::Mat xBaslerCam::getImage()
{
    std::lock_guard<std::mutex> lock(mMtx);
    if (remapped_image.data || mIsRunning.load()) {
        // Returning a clone, not by reference, to ensure that the
        // local data isn't accessed from caller after this function
        return remapped_image.clone();
    } else {
        return cv::Mat(); // Just return an empty matrix if nothing else
    }
}

/**
 * @brief Worker thread function
 */
void xBaslerCam::GrabPictures()
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
        logstd(std::string("Old exposure: ").append(std::to_string(exposureTime->GetValue())).c_str());
        if(exposureTime.IsValid()) {
            if(mExposure >= exposureTime->GetMin() && mExposure <= exposureTime->GetMax()) {
                exposureTime->SetValue(mExposure);
            } else {
                exposureTime->SetValue(exposureTime->GetMin());
                logstd(">> Exposure has been set with the minimum available value.");
                std::stringstream s;
                s << ">> The available exposure range is [" << exposureTime->GetMin() << " - " << exposureTime->GetMax() << "] (us)";
                logstd(s.str().c_str());
            }
        } else {
            logstd(">> Failed to set exposure value.");
        }
        logstd(std::string("New exposure: ").append(std::to_string(exposureTime->GetValue())).c_str());

        GenApi::CBooleanPtr(nodemap.GetNode("AcquisitionFrameRateEnable"))->SetValue(true);
        logstd("AcquisitionFrameRate enabled.");

        // set framerate cap
        GenApi::CFloatPtr AcquisitionFrameRate( nodemap.GetNode( "AcquisitionFrameRate"));
        if(AcquisitionFrameRate.IsValid()) {
            AcquisitionFrameRate->SetValue(mFramerate);
            logstd(std::string("AcquisitionFrameRate set to: ").append(std::to_string(mFramerate)).c_str());
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
                openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) pylonImage.GetBuffer());

                std::lock_guard<std::mutex> lock(mMtx);
                if(isRectified){
                    cv::remap(openCvImage,remapped_image,map1,map2,cv::INTER_LINEAR);
                } else {
                    remapped_image = openCvImage;
                }
                mHasNewImage.exchange(true); // Set flag for new image
                mFrame++;
                if(mExit.load()) {
                    camera.Close();
                    mIsRunning.exchange(false);
                    return;
                }
            } else {
                std::stringstream s;
                s  << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription();
                logstd(s.str().c_str());
                return;
            }
        }
    }
    catch (GenICam::GenericException &e)
    {
        // Error handling.
        logerr(std::string("An exception occurred.").append(e.GetDescription()).c_str());
        mIsRunning.exchange(false);
        // Rethrowing as logic error
        //throw x_err::error(x_err::what::CAMERA_GRAB_ERROR + e.what());
    }
}

void xBaslerCam::setFrameRate(uint64_t value)
{
    mFramerate = value;
}

void xBaslerCam::setMyExposure(double value)
{
    mExposure = value;
}
