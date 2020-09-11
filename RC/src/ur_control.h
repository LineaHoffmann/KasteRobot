#ifndef UR_CONTROL_H
#define UR_CONTROL_H

#include <string>
#include <vector>
#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>


class UR_Control
{
public:
    UR_Control(std::string IP);

    ~UR_Control();

    //move with rad input
    bool moveJ(const std::vector<double> &q);
    bool moveJ(const std::vector<double> &q, double speed, double acceleration);

    //move with degree input
    bool moveJDeg(const std::vector<double> &qDeg);
    bool moveJDeg(const std::vector<double> &qDeg, double speed, double acceleration);

    std::vector<double> getCurrentPose();
    std::vector<double> getCurrentPoseDeg();

    //Getter setter for defining IP addres of host server AKA the UR5 robot
    std::string getIP() const;
    void setIP(const std::string &value);

private:

    //helping functions
    std::vector<double> degToRad(const std::vector<double> &qDeg);
    std::vector<double> radToDeg(const std::vector<double> &qRad);

    std::string mIP;
    std::vector<double> *mJoints; //pointer to
    ur_rtde::RTDEControlInterface *mUrControl;
    ur_rtde::RTDEReceiveInterface *mUrRecieve;
};

#endif // UR_CONTROL_H
