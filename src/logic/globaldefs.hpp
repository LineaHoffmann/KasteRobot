#ifndef GLOBALDEFS_HPP
#define GLOBALDEFS_HPP
#pragma once

// INCLUDES
#include <string>
#include <array>

////////////////////////////////////////
// This is a shared definition header
// Contains shared definitions for:
//   Robot
//     Move Types
//     Data sharing struct
//   Gripper
//     Move types
//     Data sharing struct
//   GUI information struct
//   GUI object states
////////////////////////////////////////


////////////////////////////////////////
// ROBOT MOVE TYPES
////////////////////////////////////////
enum ROBOT_MOVE_TYPE {
    MOVE_JLIN,
    MOVE_JPATH,
    MOVE_JIK,
    MOVE_LFK,
    MOVE_L,
    MOVE_TLIN,
    SERVOJ,
    SPEEDJ,
    HOME,
    PICKUP,
    MOVE_DEFAULT
};
////////////////////////////////////////
// ROBOT DATA SHARING STRUCT
////////////////////////////////////////
struct RobotData {
    int robotState;
    bool isConnected = false;
    std::string IP;
    uint16_t robotPollingRate{0};
    std::array<float, 6> robotTcpPosition{0,0,0,0,0,0};
    std::array<float, 6> robotJointPosition{0,0,0,0,0,0};
    std::array<float, 6> robotJointSpeed{0,0,0,0,0,0};
    std::array<float, 6> robotTcpSpeed{0,0,0,0,0,0};
};

////////////////////////////////////////
// GRIPPER MOVE TYPES
////////////////////////////////////////
enum GRIPPER_MOVE_TYPE {
    // Something, something, same as above ..
};
////////////////////////////////////////
// GRIPPER DATA SHARING STRUCT
////////////////////////////////////////
struct gripperData {
    std::string pos;
    std::string force;
    std::string temp;
    std::string speed;
    std::string gripstate;
};

////////////////////////////////////////
// GUI OBJECT STATES
////////////////////////////////////////
enum OBJSTATE {
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
////////////////////////////////////////
// GUI INFORMATION TREE STRUCT
////////////////////////////////////////
struct treeInfo {
    // Robot
    OBJSTATE robotState{DEFAULT};
    std::string robotIP{"0.0.0.0"};
    uint16_t robotPort{0};
    uint16_t robotPollingRate{0};
    std::array<float, 6> robotTcpPosition{0,0,0,0,0,0};
    std::array<float, 6> robotJointPosition{0,0,0,0,0,0};
    // Camera
    OBJSTATE cameraState{DEFAULT};
    uint16_t cameraExposure{0};
    uint16_t cameraFramerate{0};
    // Gripper
    OBJSTATE gripperState{DEFAULT};
    std::string gripperIP{"0.0.0.0"};
    uint16_t gripperPort{0};
    std::string gripperSpeed{0};
    std::string gripperTemp{0};
    std::string gripperForce{0};
    std::string gripperPosition{0};
    // Database
    OBJSTATE databaseState{DEFAULT};
    std::string databaseHost{"0.0.0.0"};
    uint16_t databasePort{0};
    std::string databaseUser{"-"};
    std::string databasePassword{"-"};
    std::string databaseSchema{"-"};
    // General?
    bool isAvailable{true};
};

#endif // GLOBALDEFS_H
