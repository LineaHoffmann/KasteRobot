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

public:
    xUrControl();
    xUrControl(std::string IP);

    ~xUrControl();

    void setConnect(std::string IP);
    void setDisconnect();

    //move ENUM
    enum moveEnum {MOVE_JLIN, MOVE_JPATH, MOVE_LFK, MOVE_TLIN, SERVOJ, SPEEDJ}; // WARNING: Update Enums to fit code, before final export:

    //move function to access private move functions of UR_RTDE
    void setMove(std::vector<std::vector<double>> &q, double &acc, double &speed, xUrControl::moveEnum moveMode);

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
