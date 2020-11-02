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
        switch (id) {

        //NOTE: ROBOT buttons implementation.
        case ID_BTN_ROBOT_CONNECT:
            logstd("Robot connect from xController");
            try{
                if constexpr (std::is_same_v<T, std::string>) {
                    static_cast<std::string>(data);
                    std::string ip(data);
                    mRobot->setConnect(ip);
                    return;
                }
            } catch ( const std::exception &e ) {
                logwar(e.what());
                return;
            }


        case ID_BTN_ROBOT_DISCONNECT:
            logstd("Robot disconnect from xController");
            try {
                mRobot->setDisconnect();
                return;
            } catch (const std::exception &e) {
                logwar (e.what());
                return;
            }

        //NOTE: CAMERA buttons implementation
        case ID_BTN_CAMERA_START:
            logstd("updating values and starting the camera ..");
            if constexpr (std::is_same_v<T, std::pair<double, uint64_t>>) {
                static_cast<std::pair<double, uint64_t>>(data);
                mCamera->setMyExposure(data.first);
                mCamera->setFrameRate(data.second);
                mCamera->start();
            }
            break;
        case ID_BTN_CAMERA_STOP:
            logstd("Stopping camera ..");
            mCamera->shutdown();
            break;
        case ID_BTN_CAMERA_RECALIBRATE:
            logstd("Recalibrating lens destortion calibration ..");
            if constexpr (std::is_same_v<T, std::string>) {
                static_cast<std::string>(data);
                std::thread(xBaslerCam::liveCalibration, mCamera, data).detach();
                //caliThread = new std::thread(xBaslerCam::liveCalibration, mCamera, data);
                //caliThread->detach();
            }
            break;
        case ID_BTN_CAMERA_FINDBALL:
            if (withBall) {
                withBall = false;
                logstd("Ball detection on live image turned off ..");
                return;
            } else {
                withBall = true;
                logstd("Ball detection on live image turned on ..");
                return;
            }
            break;
        case ID_BTN_CAMERA_CUT_TABLE:
            logstd("taking picture from live camera and using it for table ROI");
            mImagehandler->loadImage(mCamera->getImage());
            mImagehandler->cutOutTable();
            logstd("ImageHandler ROI has been updated");
            break;
        case ID_BTN_CAMERA_LOAD_DETECTOR_SETTINGS:
        {
            if constexpr (std::is_same_v<T, std::tuple<long, long, float, float>>) {
                static_cast<std::tuple<long, long, float, float>>(data);
                std::stringstream s;
                s.str(std::string()); // Reset the stringstream
                s << "Updating values for camera detector: "
                  << std::get<0>(data) << " || "
                  << std::get<1>(data) << " || "
                  << std::get<2>(data) << " || "
                  << std::get<3>(data);
                logstd(s.str().c_str());
                mImagehandler->ballColor(std::get<0>(data), std::get<1>(data)); //set what color ball we are looking for
                mImagehandler->setMinMaxRadius(std::get<2>(data), std::get<3>(data)); //i cm
            }
        }
            break;
        case ID_BTN_GRIPPER_SEND_CMD:
        {
            if constexpr (std::is_same_v<T, std::string>) {
                static_cast<std::string>(data);
                //mGripper->writeRead(data);
            }
        }
            break;
        case ID_BTN_TESTING_XYZ_VVA:
            logstd("XYZ_VVA from xController ..");
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
        default:
            throw x_err::error(x_err::what::NO_IMPLEMENTATION);
            break;
        }
        return;
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
