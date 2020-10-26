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

#include "ur_rtde/rtde_control_interface.h"
#include "ur_rtde/rtde_receive_interface.h"
#include "ur_rtde/script_client.h"

#include "wx/log.h"

#include "xexceptions.h"

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

    void connect(std::string IP);
    void disconnect();

    //move ENUM
    enum moveEnum {MOVE_JLIN, MOVE_JPATH, MOVE_LFK, MOVE_TLIN, SERVOJ, SPEEDJ}; // WARNING: Update Enums to fit code, before final export:

    //move function to access private move functions of UR_RTDE
    bool move(std::vector<std::vector<double>> &q, double &acc, double &speed, xUrControl::moveEnum moveMode);

    //read current pose in rads or deg
    // TODO: Should probably return const references
    std::vector<double> getCurrentPose();
    std::vector<double> getCurrentPoseDeg();

    //Getter setter for defining IP addres of host server AKA the UR5 robot
    // TODO: Should probably return const reference
    std::string getIP() const;
    void setIP(const std::string &value);

    //Data polling for datasharing
    void startPolling();
    void stopPolling();

    // NOTE: Really should probably be returned as const reference
    std::vector<double> getLastPose();

    int getPollingRate() const;
    void setPollingRate(int pollingRate);

    //returning pointer to the datastruct.
    UR_STRUCT getURStruct();

private:

    //private functions
    void getData();
    void init();
    void initRobot();

        //move with rad input
        bool moveJ(const std::vector<double> &q);
        bool moveJ(const std::vector<double> &q, double speed, double acceleration);


    //flags
    bool isConnected = false;

    //Member Variables
    bool mContinue = true;
    int mPollingRate = 20; //defined in hz
    std::string mIP = "127.0.0.0";
    std::vector<double> *mJoints = nullptr; //pointer to

    ur_rtde::RTDEControlInterface *mUrControl = nullptr;
    ur_rtde::RTDEReceiveInterface *mUrRecieve = nullptr;

    UR_STRUCT *mURStruct = nullptr;

    std::exception_ptr mEptr = nullptr;

    //threads
    std::thread *mThread = nullptr;
    std::mutex mMtx;
};

#endif // XURCONTROL_H