
#include "xur_control.h"
//#include "../includeheader.h"

// WARNING: To-do plugin doesn't see the headers .. There are a few notes in there
// TODO: Should be renamed to xUrControl for conformity

xUR_Control::xUR_Control()
{
    init();
}

xUR_Control::xUR_Control(std::string IP)
{
    init();
    initRobot();
    try{
        connect(IP);
        isConnected = true;
    } catch(std::exception &e){
        std::rethrow_exception(mEptr);
    }

}

xUR_Control::~xUR_Control()
{
    stopPolling();
    mUrControl->disconnect();

    //check if pointer types exists and delete if they exists.
    if(mURStruct) {delete mURStruct;}
    if(mJoints) {delete mJoints;}
    if(mThread) {delete mThread;}
    if (mUrControl)   {delete mUrControl;}
    if (mUrRecieve != nullptr)   {delete mUrRecieve;}
}

/**
 * @brief connect tries to connect to robot meanwhile constructing the objects of ur_RTDE
 * @param IP
 */
void xUR_Control::connect(std::string IP){

    if(isConnected){    return; }

    if(mUrRecieve && mUrControl && !isConnected){
        mUrControl->reconnect();
        mUrRecieve->reconnect();
        logstd("Robot->Reconnected!");
    }

    if(!mUrRecieve){
        try {
            mUrRecieve = new ur_rtde::RTDEReceiveInterface(IP);
            mURStruct->IP = IP;
            logstd("UR_Control: connect: RTDE Recieve connected");

        } catch (std::exception &e) {
            mEptr = std::current_exception();
            std::cout << "ur_rtde Recieve exception: " << e.what() << std::endl;
            std::rethrow_exception(mEptr);
            return;
        };
    }

    if(!mUrControl){
        try {
            mUrControl = new ur_rtde::RTDEControlInterface(IP);
            logstd("UR_Control: connect: RTDE control connected");

        } catch (std::exception &e) {
            mEptr = std::current_exception();
            std::cout << "ur_rtde Control exception: " << e.what() << std::endl;
            std::rethrow_exception(mEptr);
        };
    }
}

void xUR_Control::disconnect()
{
    // Lock the mutex, the other threads thread come in through xLinker
    std::lock_guard<std::mutex> lock(mMtx);
    if(isConnected){
        mUrControl->disconnect();
        mUrRecieve->disconnect();
        isConnected = false;
        logstd("UR_Control: disconnect: robot disconnected!");
    }
}

bool xUR_Control::move(std::vector<std::vector<double>> &q, double &speed, double &acc, xUR_Control::moveEnum moveMode)
{
    if (!isConnected) {
        std::cerr << "UR_Control::move: Host not connected!" << std::endl;
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
std::vector<double> xUR_Control::getCurrentPose()
{
    if(!isConnected){   return std::vector<double>(0); }

    std::vector<double> out = mUrRecieve->getActualQ();

    return out;
}

/**
 * @brief UR_Control::getCurrentPoseDeg
 * @return
 */
std::vector<double> xUR_Control::getCurrentPoseDeg()
{
    if(!isConnected){   return std::vector<double>(0); }

    std::vector<double> out = mUrRecieve->getActualQ();

    return out;
}

/**
 * @brief UR_Control::getIP
 * @return string with stored IP address
 */
std::string xUR_Control::getIP() const
{
    return mIP;
}
/**
 * @brief UR_Control::setIP
 * @param value
 */
void xUR_Control::setIP(const std::string &value)
{
    mIP = value;
}

/**
 * @brief UR_Control::getData private function to poll data from the robot at the given polling rate. used by "startPolling()"
 */
void xUR_Control::getData()
{

    //preparing timers
    // TODO: Use steady_clock instead, system_clock might not be as steady as one would think
    std::chrono::system_clock::time_point timePoint;
    long waitTime = 1000 / mPollingRate; //polling rate in millis

    while (mContinue) {
        timePoint = std::chrono::system_clock::now() + std::chrono::milliseconds(waitTime);

        //lock Scope
        {
        std::unique_lock<std::mutex> dataLock(mMtx); //NOTE: unique lock applied, check type when merging programs.
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
void xUR_Control::init()
{
    //datasharing struct
    mURStruct = new UR_STRUCT;
    isConnected = false;
}

/**
 * @brief UR_Control::initRobot send scriptfile to robot, to init TCP ect.
 */
void xUR_Control::initRobot()
{
    ur_rtde::ScriptClient script("127.0.0.1",3,14);
    script.connect();
    if (script.isConnected()) {
        script.sendScript("../src/logic/startupScript.txt");
    }
}


/**
 * @brief UR_Control::getURStruct
 * @return pointer to the URStruct, for data exchange
 */
UR_STRUCT *xUR_Control::getURStruct() const
{
    return mURStruct;
}

/**
 * @brief UR_Control::startPolling starts a new thread, to poll data from the robot via UR RTDE recieve interface.
 */
void xUR_Control::startPolling()
{
    if(!isConnected){   return; }

    std::cout << "starting polling thread" << std::endl;
    mThread = new std::thread(&xUR_Control::getData, this);
}

/**
 * @brief UR_Control::stopPolling stopping the polling thread, to poll realtime data from the robot
 */
void xUR_Control::stopPolling()
{
    mContinue = false;

   if(mThread){
        std::cout << "stopping polling from robot" << std::endl;

        while (!mThread->joinable()){
        }
    }
}

/**
 * @brief UR_Control::getLastPose getting last postion stored in the URStruct
 * @return return vector with latest saved position from the URStruct
 */
std::vector<double> xUR_Control::getLastPose()
{
    return mURStruct->pose;
}

/**
 * @brief UR_Control::getPollingRate getting pollingrate in hz
 * @return pollingrate as integer representing polling rate in [Hz]
 */
int xUR_Control::getPollingRate() const
{
    return mPollingRate;
}

/**
 * @brief UR_Control::setPollingRate setting private parameter "pollingRate", and checking if its within limits of the robots pollingrate
 * @param pollingRate int in hertz [Hz] between 0 and 125 [Hz]
 */
void xUR_Control::setPollingRate(int pollingRate)
{
    if(pollingRate <= 125 && pollingRate > 0){
        mPollingRate = pollingRate;
    } else {
        std::cerr << "Input not within specified range, polling rate not changed!" << std::endl;
    }
}
