#ifndef DETECTCOLLISION_H
#define DETECTCOLLISION_H

#include <iostream>
#include <rw/loaders/WorkCellLoader.hpp>
#include <rw/models/WorkCell.hpp>
#include <rw/models/Device.hpp>
#include <rw/core/Log.hpp>
#include <rw/kinematics/FixedFrame.hpp>
#include <rw/kinematics/MovableFrame.hpp>
#include <rw/kinematics/State.hpp>
#include <rw/models/SerialDevice.hpp>
#include <rw/proximity/CollisionDetector.hpp>
#include <rw/models/Models.hpp>
#include <rw/loaders/path/PathLoader.hpp>
#include <rw/pathplanning/QSampler.hpp>
#include <rw/pathplanning/QToQPlanner.hpp>
#include <rwlibs/proximitystrategies/ProximityStrategyYaobi.hpp>
#include <rwlibs/pathplanners/sbl/SBLPlanner.hpp>
#include <rwlibs/proximitystrategies/ProximityStrategyFactory.hpp>

class xCollisionDetector
{
public:
    xCollisionDetector();
    xCollisionDetector(std::string filePath);
    xCollisionDetector(std::string filePath, rw::math::Q pose);

    void loadWorkcell(std::string filePath);

    bool checkCollision(rw::math::Q jointConfig);
    //bool checkCollision(std::vector jointConfig);

    //getters, workcell, pose, state, incollision?
    bool getCollision();

private:
    std::string filepath;
    rw::models::WorkCell::Ptr workcell;
    rw::kinematics::State state;
    rw::proximity::CollisionDetector::Ptr detector;
    bool willCollide;

};

#endif // DETECTCOLLISION_H
