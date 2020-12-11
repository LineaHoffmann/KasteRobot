#include "xcollisiondetector.hpp"

using rw::invkin::ClosedFormIKSolverUR;

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
        defState            = workcell->getDefaultState();
        device              = workcell->findDevice<rw::models::SerialDevice>("UR5_2017");
        cdstrategy          = rwlibs::proximitystrategies::ProximityStrategyFactory::makeCollisionStrategy ("YAOBI");
        if (cdstrategy.isNull ())
            logstd("Yaobi Collision Strategy could not be found.");
        detector            = rw::common::ownedPtr(new rw::proximity::CollisionDetector(workcell, cdstrategy));
        plannerConstraint   = rw::pathplanning::PlannerConstraint::make(detector, device, defState);
        planner             = rwlibs::pathplanners::RRTPlanner::makeQToQPlanner(plannerConstraint, device);
        std::cout << planner->getProperties().empty() << "planner i load" << std::endl;
        logstd("Workcell loaded succesfully");
    }
}

std::vector<double> xCollisionDetector::qToVec(rw::math::Q q)
{
    std::vector<double> output;
    for (size_t i = 0; i < q.size(); ++i) {
        output.push_back(q[i]);
    }
    return output;
}

bool xCollisionDetector::checkQ(rw::math::Q jointConfig)
{
    const rw::math::Q myPose = jointConfig;
    device ->setQ(myPose, defState);
    if (detector->inCollision(defState))
    {
        return true;
    }
    return false;
}



bool xCollisionDetector::checkCollision(std::vector<std::vector<double>> jointConfigs)
{

    //kontroller alle Q-værdier i vectoren for kollision
    for (size_t i = 0; i < jointConfigs.size(); i++)
    {
        if (checkQ(jointConfigs[i]))
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


std::vector<std::vector<double>> xCollisionDetector::makePath(std::vector<double> beg, std::vector<double> end)
{
    std::cout << planner->getProperties().empty() << "planner i makepath1" << std::endl;
    std::cout << "MP1" << std::endl;
    std::vector<std::vector<double>> output;
    rw::proximity::ProximityData pdata;
    rw::kinematics::State state = defState;

    rw::math::Q Qbeg = beg;
    rw::math::Q Qend = end;

    std::cout << "MP2" << std::endl;
    device->setQ (beg, state);
    if (detector->inCollision (state, pdata))
        RW_THROW ("Initial configuration in collision! can not plan a path.");
    device->setQ (end, state);
    if (detector->inCollision (state, pdata))
        RW_THROW ("Final configuration in collision! can not plan a path.");

    std::cout << "MP3" << std::endl;
    std::cout << planner->getProperties().empty() << "planner i makepath2" << std::endl;
    rw::trajectory::QPath result;
    if (planner->query (Qbeg, Qend, result)) {
        std::cout << "Planned path with " << result.size ();
        std::cout << " configuration steps" << std::endl;
    } else {
        std::cout << "No path planned" << std::endl;
    }
    std::cout << "MP4" << std::endl;
    for (size_t i = 0; i < result.size(); i++) {
        output.push_back(result.at(i).toStdVector());
        for (size_t j = 0; j < output[i].size(); j++) {
            std::cout << output[i][j] << "\t";
        }
        std::cout <<"q"<< i << std::endl;
    }
    return output;
}

std::vector<double> xCollisionDetector::inverseKinematic( std::vector<double> target)
{
    std::cout << planner->getProperties().empty() << "planner i IK" << std::endl;
    const ClosedFormIKSolverUR solver(device, defState);
    rw::math::Vector3D<double> targetXYZ(target[0], target[1], target[2]);
    rw::math::Vector3D<double> gripOffset(0,0,0.195);
    targetXYZ.dot(gripOffset);
    rw::math::Transform3D<> Tdesired(targetXYZ,
            rw::math::RPY<>(target[3], target[4], target[5]).toRotation3D());

    const std::vector<rw::math::Q> solutions = solver.solve(Tdesired, defState);

    for (size_t i = 0; i < solutions.size(); ++i) {
        if (!checkQ(solutions[i])) {
            return qToVec(solutions[i]);
        }
    }
    logstd("No valid joint configuration");
    std::vector<double> noTarget;
    return noTarget;
}

std::vector<std::vector<double>> xCollisionDetector::moveFromTo(std::vector<double> currentPose, std::vector<double> targetXYZ)
{
    std::cout << planner->getProperties().empty() << "planner i movefromto" << std::endl;
    std::vector<double> targetQ = inverseKinematic(targetXYZ);
    std::cout << "moveto3" << std::endl;
    std::vector<std::vector<double>> path = makePath(currentPose, targetQ);
    std::cout << "moveto4" << std::endl;
    std::cout << &targetQ << std::endl;
    if (!checkCollision(path)) {
        std::cout << "moveto5" << std::endl;
        return path;
    } else {
        logstd("No path found");
        std::cout << "moveto6" << std::endl;
        std::vector<std::vector<double>> noPath;
        return noPath;
    }
}
