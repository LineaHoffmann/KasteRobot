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
#include <array>
#include <tuple>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

#include "wx/log.h"

#include "xbaslercam.hpp"
#include "xurcontrol.hpp"
#include "xgripperclient.hpp"
#include "xexceptions.hpp"
#include "../gui/cidbindings.hpp"
#include "xcollisiondetector.hpp"
#include "ximagehandler.hpp"

#include "xmath.hpp"
#include "globaldefs.hpp"

#include "../database/qdatabasehandler.hpp"

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
    void fillInfo(treeInfo& info);
    std::vector<qDatabaseEntry*> getDatabaseEntries(); // Return som new allocated objects
    // For GUI button calls. Don't actually do stuff here, only post the tasks!
    // The GUI thread does not like working in here
    template<typename T = bool>
    void guiButtonPressed(BINDING_ID id, T data = false)
    {
        switch (id) {

        //NOTE: ROBOT buttons implementation.
        case ID_BTN_ROBOT_CONNECT:
            logstd("Robot connect from xController");
            if constexpr (std::is_same_v<T, std::string>) {
                static_cast<std::string>(data);
                std::string ip(data);

                if(mRobot){
                mRobot->setConnect(ip);
                }
            }
            break;
        case ID_BTN_ROBOT_SEND_POS:
            logstd("Robot send pos from xController");
        {
            if constexpr (std::is_same_v<T, std::vector<double>>){
                static_cast<std::vector<double>>(data);
                std::vector<std::vector<double>> q;
                q.push_back(data);
                std::string s;
                for (size_t i = 0; i < data.size(); ++i) {
                    s.append(std::to_string(q[0].at(i)) + " | ");
                }
                logstd(std::string("New TCP Position sent to robot: ").append(s).c_str());

                std::thread(&xController::createPath, this, q).detach();

            }
        }
            break;
        case ID_BTN_ROBOT_DISCONNECT:
            logstd("Robot disconnect from xController");
            if(mRobot){
            mRobot->setDisconnect();
            }
            break;
        case ID_BTN_ROBOT_SEND_HOME:
            logstd("Robot Moving to home from xController");
            if(mRobot){
            mRobot->setMove(ROBOT_MOVE_TYPE::HOME);
            }
            break;
        case ID_BTN_ROBOT_SEND_PICKUP:
            logstd("Robot Moving to Pickup from xController");
            if(mRobot){
            mRobot->setMove(ROBOT_MOVE_TYPE::PICKUP);
            }
            break;

        //NOTE: CAMERA buttons implementation
        case ID_BTN_CAMERA_START: //
            logstd("Updating exposure and framerate, and starting the camera ..");
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
            logstd("Recalibrating lens distortion ..");
            if constexpr (std::is_same_v<T, std::string>) {
                static_cast<std::string>(data);
                std::thread(xBaslerCam::liveCalibration, mCamera, data).detach();
            }
            break;
        case ID_BTN_CAMERA_FINDBALL:
            if (mWithBall.load()) {
                mWithBall.exchange(false);
                logstd("Ball detection on live image turned off ..");
                return;
            } else {
                mWithBall.exchange(true);
                logstd("Ball detection on live image turned on ..");
                return;
            }
            break;
        case ID_BTN_CAMERA_CUT_TABLE:
            logstd("Updating table ROI using new picture from live camera ..");
            mImagehandler->loadImage(mCamera->getImage());
            mImagehandler->cutOutTable();
            break;
        case ID_BTN_CAMERA_LOAD_DETECTOR_SETTINGS:
        {
            if constexpr (std::is_same_v<T, std::tuple<long, long, float, float>>) {
                static_cast<std::tuple<long, long, float, float>>(data);
                std::stringstream s;
                s.str(std::string()); // Reset the stringstream
                s << "Updating values for camera detector: "
                  << std::get<0>(data) << " | "
                  << std::get<1>(data) << " | "
                  << std::get<2>(data) << " | "
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
                mGripper->writeRead(data);
            }
        }
            break;
        case ID_BTN_GRIPPER_DISCONNECT:
        {
            mGripper->disconnectReq();
            logstd("Gripper disconnected");
        }
            break;
        case ID_BTN_GRIPPER_GRIP:
        {
            mGripper->grip();
            logstd("Gripper grip object");
        }
            break;
        case ID_BTN_GRIPPER_RELEASE:
        {
            logstd("Gripper releasing object...");
            mGripper->release();

        }
            break;
       case ID_BTN_GRIPPER_HOME:
        {
            mGripper->home();
            logstd("Gripper homed");
        }
            break;
        case ID_BTN_GRIPPER_CONNECT:
        {
            if constexpr (std::is_same_v<T, std::pair<std::string, int>>) {
                static_cast<std::pair<std::string, int>>(data);
                mGripper->connectReq(data);
            }
        }
            break;
        case ID_BTN_GRIPPER_AUTOSEND:
        {
            mGripper->autoreadReq();
            logstd("Autoread acticvated");
        }
            break;
    //NOTE: TESTING BTN xController
        case ID_BTN_TESTING_DETECT_PICK:
        {
            logstd("Starting ball detection and pick-up sequence ..");
            //std::thread(testDetectAndPickUp, mImagehandler, mCamera, mRobot, mGripper, mCollisionDetector).detach();
            std::thread(&xController::testDetectAndPickUp2,this).detach();
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
            break;
        }
        case ID_BTN_TESTING_SPEEDJ:
        {
            logstd("SPEEDJ test from controller");
            std::thread(&xController::testThrowSpeedJ, this, -0.85).detach();
            //mRobot->setMove(ROBOT_MOVE_TYPE::SPEEDJ);
        }
            break;
        case ID_BTN_TESTING_THROW:
        {
            logstd("THROW test from controller");
            if constexpr (std::is_same_v<T,std::array<double,2>>){
                static_cast<std::array<double,2>>(data);
                double x{data[0]}, y{data[1]};
                mIsAvailable.exchange(false);
                std::thread(&xController::throwBall, this, x,y).detach();
            }
        }
            break;

    // Databse BTN implementation
        case ID_BTN_DATABASE_TRY_CONNECTION:
            logstd("Connecting to database");
            try{
            if constexpr (std::is_same_v<T, std::tuple<std::string, std::string, std::string, std::string, uint32_t>>){
                static_cast<std::tuple<std::string, std::string, std::string, std::string, uint32_t>>(data);
                std::string user = get<0>(data);
                std::string password = get<1>(data);
                std::string hostname = get<2>(data);
                std::string schema = get<3>(data);
                uint32_t port = get<4>(data);

                mDatabase->setDatabaseCredentials(user, password, hostname, schema, port);
                }
                return;
            } catch(const std::exception &e){
                logwar(e.what());
                break;
            }
        case ID_BTN_TESTING_DATABASE_INSERT:
        {
            qDatabaseGripperEntry gripperItem = qDatabaseGripperEntry(true, (double) 0.0, (double) 1.25);
            mDatabase->pushLogEntry(gripperItem);
            qDatabaseThrowEntry throwItem = qDatabaseThrowEntry(false, point6D<double>(1,2,3,4,5,6), (double) 1.1, (double) 1.2, (double) 10.6);
            mDatabase->pushLogEntry(throwItem);
            qDatabaseBallEntry ballItem = qDatabaseBallEntry((double) 23.2, point2D<double>(21.3, 57.6));
            mDatabase->pushLogEntry(ballItem);
            qDatabaseMoveEntry moveItem = qDatabaseMoveEntry(point6D<double>(0,0,0,0,0,0), point6D<double>(1,1,1,1,1,1), HOME);
            mDatabase->pushLogEntry(moveItem);
        }
            break;
        default:
            throw x_err::error(x_err::what::NO_IMPLEMENTATION);
            break;
        }
        return;
    }

    static void testDetectAndPickUp(std::shared_ptr<ximageHandler> mImagehandler,
                                    std::shared_ptr<xBaslerCam> mCamera,
                                    std::shared_ptr<xUrControl> mRobot,
                                    std::shared_ptr<xGripperClient> mGripper,
                                    std::shared_ptr<xCollisionDetector> mCollisionDetector);
    void createPath(std::vector<std::vector<double>> q);

    void testDetectAndPickUp2();

    void testThrowSpeedJ(double angle);

    void throwBall(double x, double y);

private:



    std::shared_ptr<ximageHandler> mImagehandler;
    std::shared_ptr<xBaslerCam> mCamera;
    std::shared_ptr<xUrControl> mRobot;
    std::shared_ptr<xGripperClient> mGripper;
    std::shared_ptr<xCollisionDetector> mCollisionDetector;
    std::shared_ptr<qDatabaseHandler> mDatabase;

    std::mutex mMtx;
    std::thread *mCameraCalibrationThread;
    std::atomic<bool> mWithBall;
    std::atomic<bool> mIsAvailable;


};

#endif // XCONTROLLER_H
