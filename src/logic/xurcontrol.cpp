#include "xurcontrol.hpp"
//#include "../includeheader.h"

// WARNING: To-do plugin doesn't see the headers .. There are a few notes in there
// TODO: Should be renamed to xUrControl for conformity

xUrControl::xUrControl()
{
    this->mThreadMain = new std::thread(&xUrControl::entryThread, this);
}

xUrControl::xUrControl(std::string IP)
{
    //TODO: make thread on init and infinite threads.
    this->mThreadMain = new std::thread(&xUrControl::entryThread, this);
    //Connecting to robot
    try{
        connect(IP);
        isConnected = true;
    } catch(std::exception &e){
        throw x_err::error(x_err::what::ROBOT_NOT_CONNECTED);
        //std::rethrow_exception(mEptr);
    }
}

void xUrControl::entryThread()
{
    init();
    while (mCont){

    }
}

xUrControl::~xUrControl()
{
    stopPolling();
    mUrControl->disconnect();

    //check if pointer types exists and delete if they exists.
    if (mURStruct)              {delete mURStruct;}
    if (mJoints)                {delete mJoints;}
    if (mThreadData)            {delete mThreadData;}
    if (mUrControl)             {delete mUrControl;}
    if (mUrRecieve != nullptr)  {delete mUrRecieve;}
}

/**
 * @brief connect tries to connect to robot meanwhile constructing the objects of ur_RTDE
 * @param IP
 */
void xUrControl::connect(std::string IP){

    if(isConnected){    return; }

    if(mUrRecieve && mUrControl && !isConnected){
        mUrControl->reconnect();
        mUrRecieve->reconnect();
        logstd("Robot->Reconnected!");
    }

    try {
        initRobot(IP);
    } catch (const x_err::error &e) {
        throw;
    }

    if(!mUrRecieve){
        try {
            mUrRecieve = new ur_rtde::RTDEReceiveInterface(IP);
            mURStruct->IP = IP;
            logstd("UR_Control: connect: RTDE Recieve connected");

        } catch (std::exception &e) {
            //mEptr = std::current_exception();
            //std::cout << "ur_rtde Recieve exception: " << e.what() << std::endl;
            throw x_err::error(e.what());
            //std::rethrow_exception(mEptr);
            return;
        };
    }

    if(!mUrControl){
        try {
            mUrControl = new ur_rtde::RTDEControlInterface(IP);
            logstd("UR_Control: connect: RTDE control connected");

        } catch (std::exception &e) {
            throw x_err::error(e.what());
//            mEptr = std::current_exception();
//            std::cout << "ur_rtde Control exception: " << e.what() << std::endl;
//            std::rethrow_exception(mEptr);
        };
    }
}

void xUrControl::disconnect()
{
    std::lock_guard<std::mutex> lock(mMtx);
    if(isConnected){
        mUrControl->disconnect();
        mUrRecieve->disconnect();
        isConnected = false;
        logstd("UR_Control: disconnect: robot disconnected!");
    }
}

bool xUrControl::move(std::vector<std::vector<double>> &q, double &speed, double &acc, xUrControl::moveEnum moveMode)
{
    if (!isConnected) {
        //std::cerr << "UR_Control::move: Host not connected!" << std::endl;
        throw x_err::error(x_err::what::ROBOT_NOT_CONNECTED);
        //throw(UR_NotConnected());
        return false;
    }

    /*NOTE: if robot is connected, switch statement will choose correct movefunction to execute!
     * chosen as enum to ease calling from controller-class
     */
    std::cout << "UR_Control::move: ";
        switch (moveMode) {
        case MOVE_JLIN:
            std::cout << "MOVE_JLIN: move commenced!" << std::endl;
            if(mUrControl->moveJ(q[0], speed, acc)){
                std::cout << "MOVE_JLIN: move completed!" << std::endl;
                return true;
            }
            break;
        case MOVE_JPATH :
            std::cout << "MOVE_JPATH: move commenced!" << std::endl;
            if (mUrControl->moveJ(q)){
                std::cout << "MOVE_JPATH: move completed!" << std::endl;
                return true;
            }
            break;
        case MOVE_LFK:
            std::cout << "MOVE_LFK: move commenced!" << std::endl;
            if (mUrControl->moveL_FK(q[0], speed, acc)){
                std::cout << "MOVE_LFK: move completed!" << std::endl;
                return true;
            break;
        }
        default:
            std::cerr << "Wrong mode set!" << std::endl;
            break;
    }
    return false;
}

/**
 * @brief UR_Control::getCurrentPose
 * @return
 */
std::vector<double> xUrControl::getCurrentPose()
{
    if(!isConnected){   return std::vector<double>(0); }

    std::vector<double> out = mUrRecieve->getActualQ();

    return out;
}

/**
 * @brief UR_Control::getIP
 * @return string with stored IP address
 */
const std::string &xUrControl::getIP() const
{
    return mIP;
}

/**
 * @brief UR_Control::setIP
 * @param value
 */
void xUrControl::setIP(const std::string &value)
{
    mIP = value;
}

/**
 * @brief UR_Control::getData private function to poll data from the robot at the given polling rate. used by "startPolling()"
 */
void xUrControl::getData()
{

    //preparing timers
    // TODO: Use steady_clock instead, system_clock might not be as steady as one would think
    std::chrono::system_clock::time_point timePoint;
    long waitTime = 1000 / mPollingRate; //polling rate in millis

    while (mContinue) {
        timePoint = std::chrono::system_clock::now() + std::chrono::milliseconds(waitTime);

        //lock Scope
        {
        std::lock_guard<std::mutex> dataLock(mMtx);
        // SRP: unique_lock is good here, but should be used in a single instantiation and use lock/unlock inside the while loop
        // See this: https://stackoverflow.com/questions/20516773/stdunique-lockstdmutex-or-stdlock-guardstdmutex

        //get values from RTDE
        // TODO: define the struct and get the remaining struct members
        mURStruct->isConnected = mUrRecieve->isConnected();
        mURStruct->pose = mUrRecieve->getActualTCPPose();

        } //lock scope ends

        //sleep until time + waitTime
        std::this_thread::sleep_until(timePoint);
    }

}

/**
 * @brief UR_Control::init private function to initialize and allocate memory, to be used in contructors.
 */
void xUrControl::init()
{
    //datasharing struct
    mURStruct = new UR_STRUCT;
    isConnected = false;
}

/**
 * @brief UR_Control::initRobot send scriptfile to robot, to init TCP ect.
 */
void xUrControl::initRobot(std::string IP)
{
    try {
    ur_rtde::ScriptClient script(IP,3,14);
    script.connect();
    if (script.isConnected()) {
        script.sendScript("../src/logic/startupScript.txt");
    }
    } catch (std::exception &e){
        throw x_err::error(x_err::what::ROBOT_BAD_IP);
    }
}

/**
 * @brief UR_Control::getURStruct
 * @return pointer to the URStruct, for data exchange
 */
UR_STRUCT xUrControl::getURStruct() {
    std::lock_guard<std::mutex> dataLock(mMtx);
    return *mURStruct;
}

/**
 * @brief UR_Control::startPolling starts a new thread, to poll data from the robot via UR RTDE recieve interface.
 */
void xUrControl::startPolling()
{
    if(!isConnected){   return; }

    std::cout << "starting polling thread" << std::endl;
    mThreadData = new std::thread(&xUrControl::getData, this);
}

/**
 * @brief UR_Control::stopPolling stopping the polling thread, to poll realtime data from the robot
 */
void xUrControl::stopPolling()
{
    mContinue = false;

   if(mThreadData){
        std::cout << "stopping polling from robot" << std::endl;

        while (!mThreadData->joinable()){
        }
    }
}

/**
 * @brief UR_Control::getLastPose getting last postion stored in the URStruct
 * @return return vector with latest saved position from the URStruct
 */
const std::vector<double> &xUrControl::getLastPose()
{
    return mURStruct->pose;
}

/**
 * @brief UR_Control::getPollingRate getting pollingrate in hz
 * @return pollingrate as integer representing polling rate in [Hz]
 */
const int &xUrControl::getPollingRate() const
{
    return mPollingRate;
}

/**
 * @brief UR_Control::setPollingRate setting private parameter "pollingRate", and checking if its within limits of the robots pollingrate
 * @param pollingRate int in hertz [Hz] between 0 and 125 [Hz]
 */
void xUrControl::setPollingRate(int pollingRate)
{
    if(pollingRate <= 125 && pollingRate > 0){
        mPollingRate = pollingRate;
    } else {
        std::cerr << "Input not within specified range, polling rate not changed!" << std::endl;
    }
}
