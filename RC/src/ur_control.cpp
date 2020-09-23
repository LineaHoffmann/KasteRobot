#include "ur_control.h"

#include <iostream>
#include <math.h>
#include <chrono>
#include <mutex>

#define USE_MATH_DEFINES



UR_Control::UR_Control(std::string IP)
{
    try {
        mUrControl = new ur_rtde::RTDEControlInterface(IP);
        mUrRecieve = new ur_rtde::RTDEReceiveInterface(IP);

    } catch (std::exception &e) {
        mEptr = std::current_exception();
        std::cout << "ur_rtde exception: " << e.what() << std::endl; //check up on exception handling to deny further trow.
        std::rethrow_exception(mEptr);
    };

    //datasharing struct
    mURStruct = new UR_STRUCT;
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
 * @brief UR_Control::moveJ use ur_rtde to move to specified TCP position in linear path
 *          using default speed, and default acceleration.
 * @param q vector of double values in radian range -2pi< q_n < 2pi
 * @return true, if no error caught from ur_rtde moveJ funktion.
 */
bool UR_Control::moveJ(const std::vector<double> &q)
{
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
    std::vector<double> out = mUrRecieve->getActualQ();

    return out;
}

std::vector<double> UR_Control::getCurrentPoseDeg()
{
    std::vector<double> out = mUrRecieve->getActualQ();

    return radToDeg(out);
}

/**
 * @brief UR_Control::getIP
 * @return
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
 * @brief UR_Control::getData
 */
void UR_Control::getData()
{
    //preparing timers
    std::chrono::system_clock::time_point timePoint;
    long waitTime = 1000 / mPollingRate; //pollingrate in millis

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

void UR_Control::startPolling()
{
    std::cout << "starting polling thread" << std::endl;
    mThread = new std::thread(&UR_Control::getData, this);
}

void UR_Control::stopPolling()
{
    mContinue = false;

    std::cout << "stopping polling from robot" << std::endl;

    while (!mThread->joinable()){
    }
}

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

int UR_Control::getPollingRate() const
{
    return mPollingRate;
}

void UR_Control::setPollingRate(int pollingRate)
{
    mPollingRate = pollingRate;
}
