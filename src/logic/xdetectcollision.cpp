#include "xdetectcollision.h"

using rw::math::Q;
using namespace rw::models;
using rw::kinematics::State;
using namespace rw::pathplanning;
using rw::proximity::CollisionDetector;
using rw::loaders::PathLoader;
using rwlibs::proximitystrategies::ProximityStrategyYaobi;
using namespace rwlibs::pathplanners;
using rw::core::Log;
using namespace rw::kinematics;
using rw::loaders::WorkCellLoader;
using rw::models::WorkCell;

xCollisionDetector::xCollisionDetector()
{

}

xCollisionDetector::xCollisionDetector(std::string filePath, rw::math::Q jointConfig)
{
    WorkCell::Ptr workcell = WorkCellLoader::Factory::load(filePath);
    if (workcell.isNull()) {
     std::cout << "WorkCell could not be loaded." << std::endl;
    }

    Device::Ptr robot = workcell->findDevice<SerialDevice>("UR5");
    const rw::math::Q myPose = jointConfig;
    robot ->setQ(myPose, state);
    detector = rw::common::ownedPtr(new rw::proximity::CollisionDetector(workcell, rwlibs::proximitystrategies::ProximityStrategyFactory::makeDefaultCollisionStrategy()));
    if (detector->inCollision(state))
    {
        willCollide = true;
    }
}

xCollisionDetector::xCollisionDetector(std::string filePath)
{
    loadWorkcell(filePath);

}

void xCollisionDetector::loadWorkcell(std::string filePath)
{
    WorkCell::Ptr workcell = WorkCellLoader::Factory::load(filePath);
    if (workcell.isNull()) {
        std::cout << "WorkCell could not be loaded." << std::endl;
    }
}

bool xCollisionDetector::checkCollision(rw::math::Q jointConfig)
{
    Device::Ptr robot = workcell->findDevice<SerialDevice>("UR5");
    const rw::math::Q myPose = jointConfig;
    robot ->setQ(myPose, state);
    detector = rw::common::ownedPtr(new rw::proximity::CollisionDetector(workcell, rwlibs::proximitystrategies::ProximityStrategyFactory::makeDefaultCollisionStrategy()));
    if (detector->inCollision(state))
    {
        willCollide = true;
    }


    return willCollide;
}

bool xCollisionDetector::getCollision()
{
    std::cout << willCollide << std::endl;
    return willCollide;
}
