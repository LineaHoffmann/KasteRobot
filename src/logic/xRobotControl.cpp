#include <iostream>
#include <iomanip>
#include <vector>

//exemple imports
#include <chrono>
#include <thread>

//RTDE specific imports
#include <ur_rtde/rtde_control_interface.h>

//sub-class import
#include "xur_control.h"

int main() {

    //Declaring vars
    //std::string IP = "192.168.3.52"; //jw-internal
    //std::string IP = "192.168.37.130";
//    std::string IP = "192.168.100.49"; //UR5 robot
    std::string IP = "127.0.0.1";
    std::vector<std::vector<double>> joints{{-2.756, -1.5, -2.28, -0.95, 1.60, 0.023}};
    std::vector<std::vector<double>> joints2{{-1.13, -1.57, 0.0, 0.0, 0.0, 0.0}};
    double speed = 0.5;
    double acc = 0.5;

    double dt = 1.0/125; //refresh rate for UR5CB RTDE

    UR_Control *ur_control;

    //ur_rtde::RTDEControlInterface *rtde_control;
    //contructing RTDE control object
    //rtde_control = new ur_rtde::RTDEControlInterface(IP);

    try{
        ur_control = new UR_Control(IP);
    } catch(std::exception &e){
        return -1;
    }

    ur_control->setPollingRate(10);
    ur_control->startPolling();

    ur_control->move(joints,speed,acc, UR_Control::MOVE_LFK);

    std::vector<double> jointsRet = ur_control->getLastPose();

    for (double d : jointsRet){
        std:: cout << d << "\t";
    } std::cout << std::endl;

    ur_control->move(joints2,speed,acc, UR_Control::MOVE_LFK);

    jointsRet = ur_control->getLastPose();

    for (double d : jointsRet){
        std:: cout << d << "\t";
    } std::cout << std::endl;


    std::vector<double> currentPose(ur_control->getCurrentPoseDeg());

    for (auto q : currentPose) {
        std::cout << std::fixed << std::setprecision(4) << q << " ";
    };

    return 0;
}
