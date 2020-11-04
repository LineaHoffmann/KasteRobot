#pragma once
#ifndef XINFOSTRUCT_H
#define XINFOSTRUCT_H

#include <string>
#include <array>

// Just a list of states to reduce struct size
enum STATE {
    DEFAULT,

    ROBOT_NOT_CONNECTED,
    ROBOT_NOT_RUNNING,
    ROBOT_RUNNING,

    CAMERA_NOT_CONNECTED,
    CAMERA_NOT_RUNNING,
    CAMERA_RUNNING,

    GRIPPER_NOT_CONNECTED,
    GRIPPER_RUNNING,

    DATABASE_NOT_CONNECTED,
    DATABASE_RUNNING
};

// Small struct for getting data to the gui treelist
struct treeInfo {
    // Robot
    STATE robotState{DEFAULT};
    std::string robotIP{"0.0.0.0"};
    uint16_t robotPort{0};
    uint16_t robotPollingRate{0};
    std::array<float, 6> robotTcpPosition{0,0,0,0,0,0};
    std::array<float, 6> robotJointPosition{0,0,0,0,0,0};
    // Camera
    STATE cameraState{DEFAULT};
    uint16_t cameraExposure{0};
    uint16_t cameraFramerate{0};
    // Gripper
    STATE gripperState{DEFAULT};
    std::string gripperIP{"0.0.0.0"};
    uint16_t gripperPort{0};
    float gripperPosition{0};
    // Database
    STATE databaseState{DEFAULT};
    std::string databaseHost{"0.0.0.0"};
    uint16_t databasePort{0};
    std::string databaseUser{"-"};
    std::string databasePassword{"-"};
    std::string databaseSchema{"-"};
    // General?
};

#endif // XINFOSTRUCT_H
