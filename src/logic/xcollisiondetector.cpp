#include "xcollisiondetector.h"


xCollisionDetector::xCollisionDetector()
{

}

xCollisionDetector::xCollisionDetector(std::string filePath)
{
    loadWorkcell(filePath);
}

void xCollisionDetector::loadWorkcell(std::string filePath)
{
    rw::models::WorkCell::Ptr workcell = rw::loaders::WorkCellLoader::Factory::load(filePath);
    if (workcell.isNull()) {
        logstd("Workcell could not be loaded");
    }
    else {
        logstd("Workcell loaded succesfully");
    }
    state = workcell->getDefaultState();
    robot = workcell->findDevice<rw::models::SerialDevice>("UR5");
    detector = rw::common::ownedPtr(new rw::proximity::CollisionDetector(workcell, rwlibs::proximitystrategies::ProximityStrategyFactory::makeDefaultCollisionStrategy()));
}

bool xCollisionDetector::checkCollision(rw::math::Q jointConfig)
{
    const rw::math::Q myPose = jointConfig;
    robot ->setQ(myPose, state);
    if (detector->inCollision(state))
    {
        return true;
    }

    return false;
}

bool xCollisionDetector::checkCollision(std::vector<std::vector<double>> jointConfigs)
{
    std::vector<rw::math::Q> vectorQ;
    //konverter til Vector af Q-værdier
    for (size_t i = 0; i < jointConfigs.size(); i++)
    {
        rw::math::Q q (jointConfigs[i][0],jointConfigs[i][1], jointConfigs[i][2], jointConfigs[i][3], jointConfigs[i][4], jointConfigs[i][5]);
        vectorQ.push_back(q);
    }
    //kontroller alle Q-værdier i vectoren
    for (size_t i = 0; i < vectorQ.size(); i++)
    {
        if (checkCollision(vectorQ[i]))
        {
            logstd("Collision detected");
            return true;
        }
        else
        {
            logstd("No collision detected");
        }
    }
    return false;
}

bool xCollisionDetector::checkCollision(std::vector<std::vector<double>> *jointConfigs)
{
    std::vector<rw::math::Q> vectorQ;
    //konverter til Vector af Q-værdier
    for (size_t i = 0; i < jointConfigs->size(); i++)
    {
        rw::math::Q q (jointConfigs->at(i)[0], jointConfigs->at(i)[1], jointConfigs->at(i)[2], jointConfigs->at(i)[3], jointConfigs->at(i)[4], jointConfigs->at(i)[5]);
        vectorQ.push_back(q);
    }
    //kontroller alle Q-værdier i vectoren for kollision
    for (size_t i = 0; i < vectorQ.size(); i++)
    {
        if (checkCollision(vectorQ[i]))
        {
            logstd("Collision detected");
            return true;
        }
        else
        {
            logstd("No collision detected");
        }
    }
    return false;
}

bool xCollisionDetector::checkCollision(std::vector<rw::math::Q> jointConfigs)
{
    //kontroller alle Q-værdier i vectoren
    for (size_t i = 0; i < jointConfigs.size(); i++)
    {
        if (checkCollision(jointConfigs[i]))
        {
            logstd("Collision detected");
            return true;
        }
        else
        {
            logstd("No collision detected");
        }
    }
    return false;
}





