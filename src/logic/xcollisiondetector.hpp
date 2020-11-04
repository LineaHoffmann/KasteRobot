#ifndef XCOLLISIONDETECTOR_H
#define XCOLLISIONDETECTOR_H

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

#include "wx/wx.h"
#include <iostream>

#include <rw/core.hpp>
#include <rw/models.hpp>
#include <rw/loaders.hpp>
#include <rw/math.hpp>
#include <rw/kinematics.hpp>
#include <rw/pathplanning.hpp>
#include <rw/proximity.hpp>
#include <rwlibs/proximitystrategies/ProximityStrategyFactory.hpp>
#include <rwlibs/proximitystrategies/ProximityStrategyYaobi.hpp>
#include <rwlibs/pathplanners/sbl/SBLPlanner.hpp>



class xCollisionDetector
{
public:
    xCollisionDetector();
    xCollisionDetector(std::string filePath);

    void loadWorkcell(std::string filePath);
    bool checkCollision(rw::math::Q jointConfig);
    bool checkCollision(std::vector<std::vector<double>> jointConfigs);
    bool checkCollision(std::vector<std::vector<double>> *jointConfigs);
    bool checkCollision(std::vector<rw::math::Q> jointConfigs);

private:
    std::string filepath;
    rw::models::WorkCell::Ptr workcell;
    rw::kinematics::State state;
    rw::proximity::CollisionDetector::Ptr detector;
    rw::models::SerialDevice::Ptr robot;
};

#endif // XCOLLISIONDETECTOR_H
