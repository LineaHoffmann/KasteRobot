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
    }
}

void xUrControl::entryThread()
{
    init();
    while (mCont){
        if (mConnect) {
            try{
            connect(mIP);
            mConnect = false;
            } catch (x_err::error& e) {
                std::string s("URControl: connection Failed: ");
                s.append(e.what());
                logerr(s.c_str());
                mConnect = false;
            }
        }

        if (mDisconnect) {
            try{
                disconnect();
                mDisconnect = false;
            } catch (x_err::error& e) {
                std::string s("URControl: disconnect Failed: ");
                s.append(e.what());
                logerr(s.c_str());
                mDisconnect = false;
            }
        }

        if (mMove) {
            try {
                logstd("ROBOT -> Move called");
                move();
            } catch (x_err::error& e) {
                std::string s("URControl: Move Failed: ");
                s.append(e.what());
                logerr(s.c_str());
                mMove = false;
            }
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

xUrControl::~xUrControl()
{
    //NOTE: make agreement as to whom calls these upon destruction.
    stopPolling();
    mUrControl->disconnect();   // WARNING [srp]: SEGV here on destruction in debug?

    //check if pointer types exists and delete if they exists.
    if (mURStruct)              {delete mURStruct;}
    if (mJoints)                {delete mJoints;}
    if (mThreadData)            {delete mThreadData;}
    if (mUrControl)             {delete mUrControl;}
    if (mUrRecieve != nullptr)  {delete mUrRecieve;}
}

void xUrControl::setConnect(std::string IP)
{
    {std::lock_guard<std::mutex> ipLock(mMtx);
    mIP = IP;
    }
    mConnect = true;
    logstd("[ROBOT]: SetConnect flag succesfull");
}

void xUrControl::setDisconnect()
{
    mDisconnect = true;
    logstd("[ROBOT]: SetDisconnect flag succesfull");
}

void xUrControl::setMove(std::vector<std::vector<double> > q, double &acc, double &speed, xUrControl::moveEnum moveMode)
{
    {
        std::lock_guard<std::mutex> setMoveLock(mMtx);
        this->q = new std::vector<std::vector<double>>(q);
    }
    this->acc = acc;
    this->speed = speed;
    this->mMoveMode = moveMode;

    mMove = true;
    logstd("[ROBOT]: SetMove flag succesfull");
}

void xUrControl::setMove(xUrControl::moveEnum moveMode){
    this->acc = ACC_DEF;
    this->speed = SPEED_DEF;
    this->mMoveMode = moveMode;

    mMove = true;
    logstd("[ROBOT]: SetMove flag succesfull");
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
        isConnected = true;
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
            std::string s = "[ROBOT] RTDE Recieve: ";
            s.append(e.what());
            throw x_err::error(s.c_str());
            return;
        };
    }

    if(!mUrControl){
        try {
            mUrControl = new ur_rtde::RTDEControlInterface(IP);
            logstd("UR_Control: connect: RTDE control connected");
            isConnected = true;

        } catch (std::exception &e) {
            throw x_err::error(e.what());
        };
    }
}

void xUrControl::disconnect()
{
    if(!mUrRecieve || !mUrControl){
        throw x_err::error(x_err::what::ROBOT_NOT_CONNECTED);
        return;
    }

    std::lock_guard<std::mutex> lock(mMtx);
    if(isConnected){
        mUrControl->stopScript();
        mUrControl->disconnect();
        mUrRecieve->disconnect();
        isConnected = false;
        logstd("UR_Control: disconnect: robot disconnected!");
    } else {
        logstd("UR_Control: disconnect: robot already disconnected!");
    }
}

std::atomic<int> xUrControl::getPollingRate() const
{
    return mPollingRate.load();
}

void xUrControl::move()
{
    if (!isConnected) {
        throw x_err::error(x_err::what::ROBOT_NOT_CONNECTED);
        return;
    }

    try{
        if (mMoveMode == HOME) {
            logstd("MOVE_HOME: move commenced!");
            if(!mUrControl->moveL_FK(HOMEQ, speed, acc)){mUrControl->reuploadScript();}
        } else if (mMoveMode == PICKUP)  {
            logstd("MOVE_PICKUP: move commenced!");
            mUrControl->moveL_FK(PICKUPQ, speed, acc);
        }
        else {
        /*NOTE: if robot is connected, switch statement will choose correct movefunction to execute!
         * chosen as enum to ease calling from controller-class
         */
            if(q){
                switch (mMoveMode) {
                    case MOVE_JLIN:
                        logstd( "MOVE_JLIN: move commenced!");
                        //std::vector<double> tempQ = q[0];
                        if(mUrControl->moveJ(q->at(0), speed, acc)){
                            logstd("MOVE_JLIN: move completed!");
                        }
                        break;
                    case MOVE_JIK:
                        logstd( "MOVE_JIK: move commenced!");
                        //std::vector<double> tempQ = q[0];
                            if(mUrControl->moveJ_IK(q->at(0), speed, acc)){
                                logstd("MOVE_JIK: move completed!");
                            }
                        break;
                    case MOVE_JPATH :
                        logstd("MOVE_JPATH: move commenced!");
                        if (mUrControl->moveJ(*q)){
                            logstd("MOVE_JPATH: move completed!");
                        }
                        break;
                    case MOVE_LFK: //Linear tool forward kinematics
                        logstd("MOVE_LFK: move commenced!");
                        if (mUrControl->moveL_FK(q->at(0), speed, acc)){
                            logstd("MOVE_LFK: move completed!");
                        break;
                    }
                    case MOVE_L: //Linear tool
                        logstd("MOVE_L: move commenced!");
                        if (mUrControl->moveL(q->at(0), speed, acc)){
                            logstd("MOVE_L: move completed!");
                        }
                        break;
                    default:
                        mMove = false;
                        throw (x_err::error(x_err::what::ROBOT_MOVE_NOT_FOUND));
                        break;
                    }
            } else {
                mMove = false;
                throw (x_err::error(x_err::what::ROBOT_QVEC_NOT_FOUND));
            }
        }
    } catch (std::exception &e){
        logerr(e.what());
        mUrControl->reuploadScript();
    }
    //reset params for move function
    delete q;
    mMoveMode = 0;
    mMove = false;
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
 * @brief UR_Control::setPollingRate setting private parameter "pollingRate", and checking if its within limits of the robots pollingrate
 * @param pollingRate int in hertz [Hz] between 0 and 125 [Hz]
 */
void xUrControl::setPollingRate(int pollingRate)
{
    if(pollingRate <= 125 && pollingRate > 0){
        mPollingRate = pollingRate;
    } else {
        logerr("Input not within specified range, polling rate not changed!");
    }
}
