#include <iostream>
#include <iomanip>
#include <vector>

//exemple imports
#include <chrono>
#include <thread>

//RTDE specific imports
#include <ur_rtde/rtde_control_interface.h>

//sub-class import
#include "ur_control.h"

int main() {

    //Declaring vars
    std::string IP = "192.168.3.52"; //jw-internal
    //std::string IP = "192.168.37.130";
    //std::string IP = "127.0.0.1";
    std::vector<double> joints{-1.54, -2.1, -2.28, -0.35, 1.60, 0.023};
    std::vector<double> joints2{0.0, -90, 0.0, 0.0, 0.0, 0.0};
    double speed = 0.5;
    double acc = 0.5;

    double dt = 1.0/125; //refresh rate for UR5CB RTDE

    //ur_rtde::RTDEControlInterface *rtde_control;
    //contructing RTDE control object
    //rtde_control = new ur_rtde::RTDEControlInterface(IP);

    UR_Control ur_control(IP);

    ur_control.moveJ(joints,speed,acc);

    ur_control.moveJDeg(joints2, speed, acc);

    std::vector<double> currentPose(ur_control.getCurrentPoseDeg());

    for (auto q : currentPose) {
        std::cout << std::fixed << std::setprecision(4) << q << " ";
    };



//    //Example from SDU Robotics
//    rtde_control->moveJ(joints);  //simple move to joint values


//    for (unsigned int i=0; i<1000; i++)
//      {
//        auto t_start = std::chrono::high_resolution_clock::now();
//        rtde_control->speedJ(joints2, acc, dt);
//        joints2[0] += 0.0005;
//        joints2[1] += 0.0005;
//        auto t_stop = std::chrono::high_resolution_clock::now();
//        auto t_duration = std::chrono::duration<double>(t_stop - t_start);

//        if (t_duration.count() < dt)
//        {
//          std::this_thread::sleep_for(std::chrono::duration<double>(dt - t_duration.count()));
//        }
//      }

//      rtde_control->speedStop();
//      rtde_control->stopScript();

    return 0;
}
