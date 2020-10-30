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
        std::cout << "WorkCell could not be loaded." << std::endl;
    }
    else {
        std::cout << "WorkCell succesfully loaded." << std::endl;
    }
    state = workcell->getDefaultState();
    robot = workcell->findDevice<rw::models::SerialDevice>("UR5");
    detector = rw::common::ownedPtr(new rw::proximity::CollisionDetector(workcell, rwlibs::proximitystrategies::ProximityStrategyFactory::makeDefaultCollisionStrategy()));

    //solver = rw::invkin::ClosedFormIKSolverUR(robot, state)
}

bool xCollisionDetector::checkCollision(rw::math::Q jointConfig)
{
    bool willCollide = false;
    const rw::math::Q myPose = jointConfig;
    robot ->setQ(myPose, state);
    if (detector->inCollision(state))
    {
        willCollide = true;
    }

    return willCollide;
}

bool xCollisionDetector::checkCollision(std::vector<std::vector<double>> jointConfigs)
{
    bool willCollide = false;
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
            willCollide = true;
            std::cout << "Collision detected" << std::endl;
        }
        else
        {
            std::cout << "No collision detected" << std::endl;
        }
    }
    return willCollide;
}

bool xCollisionDetector::checkCollision(std::vector<rw::math::Q> jointConfigs)
{
    bool willCollide = false;
    //kontroller alle Q-værdier i vectoren
    for (size_t i = 0; i < jointConfigs.size(); i++)
    {
        if (checkCollision(jointConfigs[i]))
        {
            willCollide = true;
            std::cout << "Collision detected" << std::endl;
        }
        else
        {
            std::cout << "No collision detected" << std::endl;
        }
    }
    return willCollide;
}

//std::vector<rw::math::Q> xcollisiondetector::findQFromTcp(std::vector<double> tcp)
//{
//    //Skal vi bare returnere den første solution hver gang?
//    ClosedFormIKSolverUR solver(robot, state);
//    double x = tcp[0];
//    double y = tcp[1];
//    double z = tcp[2];
//    double rx = tcp[3];
//    double ry = tcp[4];
//    double rz = tcp[5];

////    rw::math::Vector3D<> vec(rx, ry, rz);
////    rw::math::EAA<> eaa(vec);
//    const rw::math::Transform3D<> Tdesired(rw::math::Vector3D<>(x, y, z), rw::math::EAA<>(0, 3.14, 0).toRotation3D());
//    const std::vector<rw::math::Q> solutions = solver.solve(Tdesired, state);

//    for (size_t i = 0; i < solutions.size(); i++)
//    {
//        std::cout << "virker" << std::endl;
//        std::cout << " " << solutions[i] << std::endl;
//    }
//    //std::cout << eaa.toRotation3D() << std::endl;
//    return solutions;
//}





