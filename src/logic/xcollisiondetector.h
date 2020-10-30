#ifndef XCOLLISIONDETECTOR_H
#define XCOLLISIONDETECTOR_H

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

#include <vector>

class xcollisiondetector
{
public:
    xcollisiondetector();
    xcollisiondetector(std::string filePath);

    void loadWorkcell(std::string filePath);
    bool checkCollision(rw::math::Q jointConfig);//
    bool checkCollision(std::vector<std::vector<double>> jointConfigs);
    bool checkCollision(std::vector<rw::math::Q> jointConfigs);
    //std::vector<rw::math::Q> findQFromTcp(std::vector<double> tcp);

private:
    std::string filepath;
    rw::models::WorkCell::Ptr workcell;
    rw::kinematics::State state;
    rw::proximity::CollisionDetector::Ptr detector;
    rw::models::SerialDevice::Ptr robot;
};

#endif // XCOLLISIONDETECTOR_H
