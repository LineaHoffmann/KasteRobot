#ifndef GLOBALDEFS_HPP
#define GLOBALDEFS_HPP
#pragma once

// INCLUDES
#include <string>
#include <array>

#include "xgeometrytypes.hpp"

////////////////////////////////////////
// This is a shared definition header
// Contains shared definitions for:
//   Robot
//     Move Types
//     Data sharing struct
//   Gripper
//     Move types
//     Data sharing struct
//   Database
//     Log entry types
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
// ROBOT ENUM STRING CONVERSIONS
////////////////////////////////////////
static inline std::string getRobotMoveTypeAsString(const ROBOT_MOVE_TYPE type) {
    std::string s;
    switch (type) {
    case ROBOT_MOVE_TYPE::MOVE_JLIN:
        s = "JLIN";
        break;
    case ROBOT_MOVE_TYPE::MOVE_JPATH:
        s = "JPATH";
        break;
    case ROBOT_MOVE_TYPE::MOVE_JIK:
        s = "JIK";
        break;
    case ROBOT_MOVE_TYPE::MOVE_LFK:
        s = "LFK";
        break;
    case ROBOT_MOVE_TYPE::MOVE_L:
        s = "L";
        break;
    case ROBOT_MOVE_TYPE::MOVE_TLIN:
        s = "TLIN";
        break;
    case ROBOT_MOVE_TYPE::SERVOJ:
        s = "SERVOJ";
        break;
    case ROBOT_MOVE_TYPE::SPEEDJ:
        s = "SPEEDJ";
        break;
    case ROBOT_MOVE_TYPE::HOME:
        s = "HOME";
        break;
    case ROBOT_MOVE_TYPE::PICKUP:
        s = "PICKUP";
        break;
    case ROBOT_MOVE_TYPE::MOVE_DEFAULT:
        s = "MOVE_DEFAULT";
        break;
    }
    return s;
}
static inline ROBOT_MOVE_TYPE getRobotMoveTypeFromString(const std::string& s) {
    ROBOT_MOVE_TYPE type;
    if (s == "JLIN") {
        type = ROBOT_MOVE_TYPE::MOVE_JLIN;
    } else if (s == "JPATH") {
        type = ROBOT_MOVE_TYPE::MOVE_JPATH;
    } else if (s == "JIK") {
        type = ROBOT_MOVE_TYPE::MOVE_JIK;
    } else if (s == "LFK") {
        type = ROBOT_MOVE_TYPE::MOVE_LFK;
    } else if (s == "L") {
        type = ROBOT_MOVE_TYPE::MOVE_L;
    } else if (s == "TLIN") {
        type = ROBOT_MOVE_TYPE::MOVE_TLIN;
    } else if (s == "SERVOJ") {
        type = ROBOT_MOVE_TYPE::SERVOJ;
    } else if (s == "SPEEDJ") {
        type = ROBOT_MOVE_TYPE::SPEEDJ;
    } else if (s == "HOME") {
        type = ROBOT_MOVE_TYPE::HOME;
    } else if (s == "PICKUP") {
        type = ROBOT_MOVE_TYPE::PICKUP;
    } else if (s == "MOVE_DEAFULT") {
        type = ROBOT_MOVE_TYPE::MOVE_DEFAULT;
    }
    return type;
}
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
// DATABASE LOG ENTRY TYPES
////////////////////////////////////////
struct qDatabaseEntry {
    std::string timestamp;
    std::string entryType;
    virtual ~qDatabaseEntry() = default; // Virtual to make dynamic_cast possible
protected:
    qDatabaseEntry(const std::string& desc) : entryType(desc) {}
    qDatabaseEntry(const std::string& t, const std::string& desc) :
        timestamp(t), entryType(desc) {}
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseEntry &p) {
        return os << "Entry type: " << p.entryType << "\n"
                  << "Timestamp: " << p.timestamp;
    }
};
template <typename T>
struct qDatabaseMoveEntry : public qDatabaseEntry {
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseMoveEntry(){}
    qDatabaseMoveEntry(const std::string& t, const std::string& d,
                       const point6D<T>& s, const point6D<T>& e, ROBOT_MOVE_TYPE m) :
        qDatabaseEntry(t, d), start(s), end(e), moveType(m) {}
    qDatabaseMoveEntry(const point6D<T>& s, const point6D<T>& e, ROBOT_MOVE_TYPE m) :
        qDatabaseEntry("move"), start(s), end(e), moveType(m) {}
    point6D<T> start, end;
    ROBOT_MOVE_TYPE moveType;
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseMoveEntry &p) {
        return os << (qDatabaseEntry) p << "\n"
                  << "Start point: " << p.start << "\n" << "End point: " << p.end << "\n"
                  << "Move type: " << p.moveType;
    }
};
template <typename T>
struct qDatabaseThrowEntry : public qDatabaseEntry {
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseThrowEntry(){}
    qDatabaseThrowEntry(const std::string& t, const std::string& d,
                        bool s, const point6D<T>& rp, T v1, T v2, T de) :
        qDatabaseEntry(t, d), successful(s), releasePoint(rp),
        releaseVelocityCalced(v1), releaseVelocityActual(v2), deviation(de) {}
    qDatabaseThrowEntry(bool s, const point6D<T>& rp, T v1, T v2, T de) :
        qDatabaseEntry("move"), successful(s), releasePoint(rp),
        releaseVelocityCalced(v1), releaseVelocityActual(v2), deviation(de) {}
    bool successful;
    point6D<T> releasePoint;
    T releaseVelocityCalced, releaseVelocityActual, deviation;
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseThrowEntry &p) {
        return os << (qDatabaseEntry) p << "\n"
                  << "Succesful: " << p.successful << "\n"
                  << "Release point: " << p.releasePoint << "\n"
                  << "Calc release velocity: " << p.releaseVelocityCalced << "\n"
                  << "Actual release velocity: " << p.releaseVelocityActual << "\n"
                  << "~Deviation: " << p.deviation;
    }
};
template <typename T>
struct qDatabaseGripperEntry : public qDatabaseEntry {
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseGripperEntry(){}
    qDatabaseGripperEntry(const std::string& t, const std::string& d, bool suc, T s, T e) :
        qDatabaseEntry(t, d), successful(suc), start(s), end(e) {}
    qDatabaseGripperEntry(bool suc, T s, T e) :
        qDatabaseEntry("move"), successful(suc), start(s), end(e) {}
    bool successful;
    T start, end;
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseGripperEntry &p) {
        return os << (qDatabaseEntry) p << "\n"
                  << "Start width: " << p.start << "\n"
                  << "End width: " << p.end << "\n"
                  << "Succesful: " << p.successful;
    }
};
template <typename T>
struct qDatabaseBallEntry : public qDatabaseEntry {
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseBallEntry(){}
    qDatabaseBallEntry(const std::string& t, const std::string& d, T di, const point2D<T>& p) :
        qDatabaseEntry(t, d), ballDiameter(di), ballPosition(p) {}
    qDatabaseBallEntry(T di, const point2D<T>& p) :
        qDatabaseEntry("move"), ballDiameter(di), ballPosition(p) {}
    T ballDiameter;
    point2D<T> ballPosition;
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseBallEntry &p) {
        return os << (qDatabaseEntry) p << "\n"
                  << "Position: " << p.ballPosition << "\n"
                  << "Size: " << p.ballDiameter;
    }
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
