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
#include <rwlibs/pathplanners/rrt/RRTPlanner.hpp>

#include<rwlibs/pathoptimization/ClearanceOptimizer.hpp>



class xCollisionDetector
{
public:
    xCollisionDetector();
    xCollisionDetector(std::string filePath);

    void loadWorkcell(std::string filePath);
    bool checkQ(rw::math::Q jointConfig);
    bool checkCollision(std::vector<std::vector<double>> *jointConfigs);
    std::vector<std::vector<double>> makePath(std::vector<double> Qbeg, std::vector<double> Qend);

private:
    std::string filepath;
    rw::models::WorkCell::Ptr workcell;
    rw::kinematics::State defState;
    rw::models::Device::Ptr device;
    rw::proximity::CollisionStrategy::Ptr cdstrategy;
    rw::proximity::CollisionDetector::Ptr detector;
    rw::pathplanning::PlannerConstraint plannerConstraint;
    rw::pathplanning::QToQPlanner::Ptr planner;
};

#endif // XCOLLISIONDETECTOR_H
