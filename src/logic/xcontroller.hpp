#ifndef XCONTROLLER_H
#define XCONTROLLER_H

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

#include <utility>
#include <type_traits>
#include <mutex>
#include <thread>


#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

#include "wx/wx.h"

#include "xbaslercam.hpp"
#include "xurcontrol.hpp"
#include "xgripperclient.hpp"
#include "xexceptions.hpp"
#include "../gui/cidbindings.hpp"
#include "xcollisiondetector.h"
#include "ximagehandler.hpp"

#include "xmath.hpp"

class xController
{
public:
    xController();
    ~xController();

    void setCamera(std::shared_ptr<xBaslerCam> camera);
    void setRobot(std::shared_ptr<xUrControl> robot);
    void setGripper(std::shared_ptr<xGripperClient> gripper);

    // ********* //
    // GUI CALLS //
    // ********* //
    // For GUI when grabbing from camera
    bool hasNewImage();
    cv::Mat getImage();
    // For GUI when updating info tree
    void fillInfo(struct treeInfo); // TODO: Perhaps a struct to keep all info? Who knows
    // For GUI button calls. Don't actually do stuff here, only post the tasks!
    // The GUI thread does not like working in here
    template<typename T = bool>
    void guiButtonPressed(BINDING_ID id, T data = false)
    {
        if (id == BINDING_ID::ID_BTN_CAMERA_STOP) {
            mCamera->shutdown();
            logstd("Stopping camera...");
            return;
        }
        if (id == BINDING_ID::ID_BTN_CAMERA_CUT_TABLE) {
            logstd("taking picture from live camera and using it for table ROI");
            mImagehandler->loadImage(mCamera->getImage());
            mImagehandler->cutOutTable();
            logstd("ImageHandler ROI has been updated");

            return;
        }
        if (id == BINDING_ID::ID_BTN_CAMERA_TRIG_FINDBALL) {
            if (withBall) {
                withBall = false;
                logstd("Ball detection on live image turned off");
                return;
            } else {
                withBall = true;
                logstd("Ball detection on live image turned on");
                return;
            }
        }
        if (id == BINDING_ID::ID_BTN_TESTING_XYZ_VVA) {
            logstd("XYZ_VVA from xController");
            try {
                if constexpr (std::is_same_v<T, std::array<double,7>>) {
                    static_cast<std::array<double,7>>(data);
                    std::array<double, 3> xyz{ data[0], data[1], data[2] };
                    std::array<double, 2> aLims{ data[3], data[4] };
                    std::array<double, 2> vLims{ data[5], data[6] };
                    std::array<double, 3> res = xMath::distance3d_to_v0_xyAngle_czAngle(xyz, aLims, vLims);
                    std::stringstream s; s << "V0: " << res[0] << " | Theta_xy: " << res[1] << " | Theta_cz: " << res[2];
                    logstd(s.str().c_str());
                    return;
                }
            } catch ( const std::exception &e ) {
                logwar(e.what());
                return;
            }
        }
        throw x_err::error(x_err::what::NO_IMPLEMENTATION);
    }



    void guiButtonPressed(BINDING_ID id, std::pair<std::pair<long, long>, std::pair<double, double>> data)
    {
        if (id == BINDING_ID::ID_BTN_CAMERA_LOAD_DETECTOR_SETTINGS){
            std::stringstream s;
            s.str(std::string()); // Reset the stringstream
            s << "updating values for detector: " << data.first.first << " || " << data.first.second << " || " << data.second.first << " || " << data.second.second;
            logstd(s.str().c_str());

            mImagehandler->ballColor(static_cast<int>(data.first.first), static_cast<int>(data.first.second)); //set what color ball we are looking for
            mImagehandler->setMinMaxRadius(static_cast<float>(data.second.first), static_cast<float>(data.second.second)); //i cm
            return;
        }
        throw x_err::error(x_err::what::NO_IMPLEMENTATION);
    }

    void guiButtonPressed(BINDING_ID id, std::pair<double, uint64_t> data)
    {
        if (id == BINDING_ID::ID_BTN_CAMERA_START){
            mCamera->setMyExposure(data.first);
            mCamera->setFrameRate(data.second);
            mCamera->start();
            logstd("updating values and starting the camera");


            return;
        }
        throw x_err::error(x_err::what::NO_IMPLEMENTATION);
    }
    void guiButtonPressed(BINDING_ID id, std::string data)
    {
        if (id == BINDING_ID::ID_BTN_CAMERA_RECALIBRATE) {
            logstd("recalibrating linse destortion");

            caliThread = new std::thread(xBaslerCam::liveCalibration, mCamera, data);

            return;





        }
        throw x_err::error(x_err::what::NO_IMPLEMENTATION);

    }



private:
    std::shared_ptr<ximageHandler> mImagehandler;
    std::shared_ptr<xBaslerCam> mCamera;
    std::shared_ptr<xUrControl> mRobot;
    std::shared_ptr<xGripperClient> mGripper;
    std::shared_ptr<xCollisionDetector> mCollisionDetector;

    std::mutex mMtx;
    std::thread *caliThread;
    bool withBall = false;


};

#endif // XCONTROLLER_H
