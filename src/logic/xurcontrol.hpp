#ifndef XURCONTROL_H
#define XURCONTROL_H
#pragma once

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <exception>
#include <iostream>
#include <atomic>

#include "ur_rtde/rtde_control_interface.h"
#include "ur_rtde/rtde_receive_interface.h"
#include "ur_rtde/script_client.h"

#include "wx/log.h"

#include "xexceptions.hpp"
#include "globaldefs.hpp"
#include "xcollisiondetector.hpp"

class xUrControl
{
    //Static pre-defines positions.
//    inline static const std::vector<double> HOMEQ{-1.15192, -1.39626,-0.39392081,-1.5708,1.5708,0};
    inline static const std::vector<double> HOMEQ{-1.15192, -1.39626,-0.39392081, -1.5708,1.5708,1.5708};


//    inline static const std::vector<double> PICKUPQ{.07327, -.43385,0.1,0.720,-3.062,0.044};
    inline static const std::vector<double> PICKUPQ{.07327, -.43385,0.1,1.778,2.562,0.1};


    inline static const double SPEED_DEF{0.5};
    inline static const double ACC_DEF {0.5};


public:
    xUrControl();
    xUrControl(std::string IP);

    ~xUrControl();

    void setConnect(std::string IP);
    void setDisconnect();

    //move function to access private move functions of UR_RTDE
    void setMove(ROBOT_MOVE_TYPE moveMode);
    void setMove(ROBOT_MOVE_TYPE moveMode, std::vector<std::vector<double> > inputQ);
    void setMove(ROBOT_MOVE_TYPE moveMode, std::vector<std::vector<double> > inputQ, double acc, double speed);

    void speedJMove(double t = 0.5);
    void speedJStop();

    //read current pose in rads or deg
    std::vector<double> getCurrentPose();

    //Getter setter for defining IP addres of host server AKA the UR5 robot
    const std::string &getIP() const;
    void setIP(const std::string &value);

    //Data polling for datasharing
    void startPolling();
    void stopPolling();

    void setPollingRate(int pollingRate);

    //returning pointer to the datastruct.
    RobotData getURStruct();

    std::atomic<int> getPollingRate() const;

    std::atomic<bool> getIsBusy() const;

    bool getIsConnected() const;

private:

    //private functions
    void getData();
    void init();
    void initRobot(std::string IP);
    void entryThread();

    void move();

    void connect(std::string IP);
    void disconnect();


    //flags
      std::atomic<bool> isConnected = false;
      std::atomic<bool> mCont = true;
      std::atomic<bool> mConnect = false;
      std::atomic<bool> mDisconnect = false;
      std::atomic<bool> mMove = false;
      std::atomic<bool> mContinue = true;
      std::atomic<bool> mIsBusy = false;

    //Member Variables

          //Variables for Move function
              std::vector<std::vector<double>> mQ;
              std::atomic<int> mMoveMode;
              std::atomic<double> acc, speed;

    std::atomic<int> mPollingRate = 2;
    std::string mIP = "127.0.0.1";
    std::vector<double> *mJoints = nullptr; //pointer to

    ur_rtde::RTDEControlInterface *mUrControl = nullptr;
    ur_rtde::RTDEReceiveInterface *mUrRecieve = nullptr;

    xCollisionDetector *mDetector;

    RobotData mURStruct;

    std::exception_ptr mEptr = nullptr;

    //threads
    std::thread *mThreadMain = nullptr;
    std::thread *mThreadData = nullptr;
    std::mutex mMtx;
};

#endif // XURCONTROL_H
