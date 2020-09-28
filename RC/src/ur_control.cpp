#include "ur_control.h"

#include <iostream>
#include <math.h>
#include <chrono>
#include <mutex>

#define USE_MATH_DEFINES



UR_Control::UR_Control()
{
    init();
}

UR_Control::UR_Control(std::string IP)
{
    init();
    try{
        connect(IP);
        isConnected = true;
    } catch(std::exception &e){
        std::rethrow_exception(mEptr);
    }

}

UR_Control::~UR_Control()
{
    stopPolling();

    //check if pointer types exists and delete if they exists.
    if(mURStruct) {delete mURStruct;}
    if(mJoints) {delete mJoints;}
    if(mThread) {delete mThread;}
    if (mUrControl)   {delete mUrControl;}
    if (mUrRecieve != nullptr)   {delete mUrRecieve;}
}

/**
 * @brief connect tries to connect to robot meanwhile constructing the objects of ur_RTDE
 * @param IP
 */
void UR_Control::connect(std::string IP){

    if(isConnected){    return; }

    if(!mUrRecieve){
        try {
            mUrRecieve = new ur_rtde::RTDEReceiveInterface(IP);

        } catch (std::exception &e) {
            mEptr = std::current_exception();
            std::cout << "ur_rtde Recieve exception: " << e.what() << std::endl;
            std::rethrow_exception(mEptr);
        };
    }

    if(!mUrControl){
        try {
            mUrControl = new ur_rtde::RTDEControlInterface(IP);

        } catch (std::exception &e) {
            mEptr = std::current_exception();
            std::cout << "ur_rtde Control exception: " << e.what() << std::endl;
            std::rethrow_exception(mEptr);
        };
    }
}

/**
 * @brief UR_Control::moveJ use ur_rtde to move to specified TCP position in linear path
 *          using default speed, and default acceleration.
 * @param q vector of double values in radian range -2pi< q_n < 2pi
 * @return true, if no error caught from ur_rtde moveJ funktion.
 */
bool UR_Control::moveJ(const std::vector<double> &q)
{
    if(!isConnected){   return false; }

    // TODO: add check if all vector-entries are within value of -2pi< q_n < 2pi
    try {
        mUrControl->moveJ(q,0.2,0.2);
    } catch (std::exception &e) {
        std::cerr << "moveJ exception: " << e.what() << std::endl;
        return false;
    }
    return true;
}


/**
 * @brief UR_Control::moveJ Use ur_rtde lib to move to specified TCP position in linear path, using speed and acceleration parameters and position vector6d
 * @param q Vector6d of double values in radians
 * @param speed
 * @param acceleration
 * @return
 */
bool UR_Control::moveJ(const std::vector<double> &q, double speed, double acceleration)
{
    if(!isConnected){   return false; }

    // TODO: add check if all vector-entries are within value of -2pi< q_n < 2pi
    try {
        if(mUrControl)
            mUrControl->moveJ(q,speed,acceleration);
    } catch (std::exception &e) {
        std::cerr << "moveJ exception: " << e.what() << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief UR_Control::moveJDeg
 * @param qDeg
 * @return
 */
bool UR_Control::moveJDeg(const std::vector<double> &qDeg)
{
    if(!isConnected){   return false; }

    try {
        mUrControl->moveJ(degToRad(qDeg), 0.2, 0.2);
    } catch (std::exception &e) {
        std::cerr << "moveJ exception: " << e.what() << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief UR_Control::moveJDeg
 * @param qDeg
 * @param speed
 * @param acceleration
 * @return
 */
bool UR_Control::moveJDeg(const std::vector<double> &qDeg, double speed, double acceleration)
{
    if(!isConnected){   return false; }

    std::vector<double> q = degToRad(qDeg);

    try {
        mUrControl->moveJ(q,speed,acceleration);
    } catch (std::exception &e) {
        std::cerr << "moveJ exception: " << e.what() << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief UR_Control::getCurrentPose
 * @return
 */
std::vector<double> UR_Control::getCurrentPose()
{
    if(!isConnected){   return std::vector<double>(0); }

    std::vector<double> out = mUrRecieve->getActualQ();

    return out;
}

/**
 * @brief UR_Control::getCurrentPoseDeg
 * @return
 */
std::vector<double> UR_Control::getCurrentPoseDeg()
{
    if(!isConnected){   return std::vector<double>(0); }

    std::vector<double> out = mUrRecieve->getActualQ();

    return radToDeg(out);
}

/**
 * @brief UR_Control::getIP
 * @return string with stored IP address
 */
std::string UR_Control::getIP() const
{
    return mIP;
}
/**
 * @brief UR_Control::setIP
 * @param value
 */
void UR_Control::setIP(const std::string &value)
{
    mIP = value;
}

/**
 * @brief UR_Control::getData private function to poll data from the robot at the given polling rate. used by "startPolling()"
 */
void UR_Control::getData()
{

    //preparing timers
    std::chrono::system_clock::time_point timePoint;
    long waitTime = 1000 / mPollingRate; //polling rate in millis

    while (mContinue) {
        timePoint = std::chrono::system_clock::now() + std::chrono::milliseconds(waitTime);

        //lock Scope
        {
        std::unique_lock<std::mutex> dataLock(urMutex); //NOTE: unique lock applied, check type when merging programs.

        //get values from RTDE
        // TODO: define the struct and get the remaining struct members
        mURStruct->isConnected = mUrRecieve->isConnected();
        mURStruct->pose = mUrRecieve->getActualTCPPose();
        } //lock scope ends

        //sleep until time + waitTime
        std::this_thread::sleep_until(timePoint);
    }

}

/**
 * @brief UR_Control::init private function to initialize and allocate memory, to be used in contructors.
 */
void UR_Control::init()
{
    //datasharing struct
    mURStruct = new UR_STRUCT;
    isConnected = false;
}


/**
 * @brief UR_Control::getURStruct
 * @return pointer to the URStruct, for data exchange
 */
UR_Control::UR_STRUCT *UR_Control::getURStruct() const
{
    return mURStruct;
}

/**
 * @brief UR_Control::startPolling starts a new thread, to poll data from the robot via UR RTDE recieve interface.
 */
void UR_Control::startPolling()
{
    if(!isConnected){   return; }

    std::cout << "starting polling thread" << std::endl;
    mThread = new std::thread(&UR_Control::getData, this);
}

/**
 * @brief UR_Control::stopPolling stopping the polling thread, to poll realtime data from the robot
 */
void UR_Control::stopPolling()
{
    mContinue = false;

   if(mThread){
        std::cout << "stopping polling from robot" << std::endl;

        while (!mThread->joinable()){
        }
    }
}

/**
 * @brief UR_Control::getLastPose getting last postion stored in the URStruct
 * @return return vector with latest saved position from the URStruct
 */
std::vector<double> UR_Control::getLastPose()
{
    return mURStruct->pose;
}

/**
 * @brief UR_Control::DegToRad private function to convert degrees to radians
 * @param qDeg  vector6d in degrees
 * @return vector6d converted to radians
 */
std::vector<double> UR_Control::degToRad(const std::vector<double> &qDeg)
{
    std::vector<double> out;
    out.reserve(6);

    for (int i = 0, total = qDeg.size(); i < total; ++i) {
        out.push_back(qDeg.at(i) * (M_PI/180)); //convert from deg to rad.
    }
    return out;
}
/**
 * @brief UR_Control::radToDeg
 * @param qRad
 * @return
 */
std::vector<double> UR_Control::radToDeg(const std::vector<double> &qRad)
{
    std::vector<double> out;
    out.reserve(6);

    for(double q : qRad){
        out.push_back(q * (180/M_PI));
    }

    return out;
}

/**
 * @brief UR_Control::getPollingRate getting pollingrate in hz
 * @return pollingrate as integer representing polling rate in [Hz]
 */
int UR_Control::getPollingRate() const
{
    return mPollingRate;
}

/**
 * @brief UR_Control::setPollingRate setting private parameter "pollingRate", and checking if its within limits of the robots pollingrate
 * @param pollingRate int in hertz [Hz] between 0 and 125 [Hz]
 */
void UR_Control::setPollingRate(int pollingRate)
{
    if(pollingRate <= 125 && pollingRate > 0){
        mPollingRate = pollingRate;
    } else {
        std::cerr << "Input not within specified range, polling rate not changed!" << std::endl;
    }
}
