#include "ur_control.h"

#include <iostream>
#include <math.h>

#define USE_MATH_DEFINES



UR_Control::UR_Control(std::string IP)
{
    try {
        mUrControl = new ur_rtde::RTDEControlInterface(IP);
        mUrRecieve = new ur_rtde::RTDEReceiveInterface(IP);

    } catch (std::exception &e) {
        std::cout << "ur_rtde exception: " << e.what() << std::endl; //check up on exception handling to deny further trow.
    };
}

UR_Control::~UR_Control()
{
    delete mUrControl, mUrRecieve;

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

void UR_Control::setIP(const std::string &value)
{
    mIP = value;
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
