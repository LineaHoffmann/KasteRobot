#include "xcollisiondetector.hpp"


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

        logstd("Workcell loaded succesfully");
    }
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
        if (checkQ(vectorQ[i]))
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
    std::vector<std::vector<double>> output;
    rw::proximity::ProximityData pdata; //Skal flyttes i hpp?
    rw::kinematics::State state = defState;

    rw::math::Q Qbeg = beg;
    rw::math::Q Qend = end;

    device->setQ (beg, state);
    if (detector->inCollision (state, pdata))
        RW_THROW ("Initial configuration in collision! can not plan a path.");
    device->setQ (end, state);
    if (detector->inCollision (state, pdata))
        RW_THROW ("Final configuration in collision! can not plan a path.");

    rw::trajectory::QPath result;
    if (planner->query (beg, end, result)) {
        std::cout << "Planned path with " << result.size ();
        std::cout << " configuration steps" << std::endl;
    }
    std::cout << "0" << std::endl;
    rw::math::QMetric::CPtr metric;
    std::cout << "1" << std::endl;
    rw::core::Ptr<rwlibs::pathoptimization::ClearanceCalculator> calc;
    std::cout << "2" << std::endl;
    rwlibs::pathoptimization::ClearanceOptimizer optimizer = rwlibs::pathoptimization::ClearanceOptimizer(device, state, metric, calc);
    std::cout << "3" << std::endl;
    std::cout << optimizer.getClearanceCalculator() << std::endl;
    rw::trajectory::QPath result2;
    std::cout << "3.5" << std::endl;
    result2 = optimizer.optimize(result);
    std::cout << "4" << std::endl;
    for (size_t i = 0; i < result.size(); i++) {
        output.push_back(result.at(i).toStdVector());
        for (size_t j = 0; j < output[i].size(); j++) {
            std::cout << output[i][j] << "\t";
        }
        std::cout << std::endl;
    }
    return output;
}
