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

//definition of data struct
struct UR_STRUCT {

    std::vector<double> pose{0};
    bool isConnected = false;
    std::string IP;

};

class xUrControl
{
    //Static pre-defines positions.
    inline static const std::vector<double> HOMEQ{-1.15192, -1.5708,0,-1.5708,1.5708,0};
    inline static const std::vector<double> PICKUPQ{-1.15192, -1.93487201,-2.09230071
                                                    ,-0.733038,1.359437,0};

    inline static const double SPEED_DEF{0.5};
    inline static const double ACC_DEF {0.5};


public:
    xUrControl();
    xUrControl(std::string IP);

    ~xUrControl();

    void setConnect(std::string IP);
    void setDisconnect();

    //move ENUM
    enum moveEnum {MOVE_JLIN, MOVE_JPATH, MOVE_JIK, MOVE_LFK, MOVE_L, MOVE_TLIN, SERVOJ, SPEEDJ, HOME, PICKUP}; // WARNING: Update Enums to fit code, before final export:

    //move function to access private move functions of UR_RTDE
    void setMove(std::vector<std::vector<double> > q, double &acc, double &speed, xUrControl::moveEnum moveMode);
    void setMove(xUrControl::moveEnum moveMode);

    //read current pose in rads or deg
    std::vector<double> getCurrentPose();

    //Getter setter for defining IP addres of host server AKA the UR5 robot
    const std::string &getIP() const;
    void setIP(const std::string &value);

    //Data polling for datasharing
    void startPolling();
    void stopPolling();

    const std::vector<double> &getLastPose();

    void setPollingRate(int pollingRate);

    //returning pointer to the datastruct.
    UR_STRUCT getURStruct();

    std::atomic<int> getPollingRate() const;

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

    //Member Variables

          //Variables for Move function
              std::vector<std::vector<double>> *q = nullptr;
              std::atomic<int> mMoveMode;
              std::atomic<double> acc, speed;

    std::atomic<int> mPollingRate = 20;
    std::string mIP = "127.0.0.1";
    std::vector<double> *mJoints = nullptr; //pointer to

    ur_rtde::RTDEControlInterface *mUrControl = nullptr;
    ur_rtde::RTDEReceiveInterface *mUrRecieve = nullptr;

    UR_STRUCT *mURStruct = nullptr;

    std::exception_ptr mEptr = nullptr;

    //threads
    std::thread *mThreadMain = nullptr;
    std::thread *mThreadData = nullptr;
    std::mutex mMtx;
};

#endif // XURCONTROL_H
