#ifndef UR_CONTROL_H
#define UR_CONTROL_H

#include "../includeheader.h"
#include "xrobotexceptions.h"

//definition of data struct
struct UR_STRUCT {

    std::vector<double> pose{0};
    bool isConnected = false;
    std::string IP;

};

class xUR_Control
{

public:
    xUR_Control();
    xUR_Control(std::string IP);

    ~xUR_Control();

    void connect(std::string IP);
    void disconnect();

    //move ENUM
    enum moveEnum {MOVE_JLIN, MOVE_JPATH, MOVE_LFK, MOVE_TLIN, SERVOJ, SPEEDJ}; // WARNING: Update Enums to fit code, before final export:

    //move function to access private move functions of UR_RTDE
    bool move(std::vector<std::vector<double>> &q, double &acc, double &speed, xUR_Control::moveEnum moveMode);

    //read current pose in rads or deg
    // TODO: Should probably return const references
    std::vector<double> getCurrentPose();
    std::vector<double> getCurrentPoseDeg();

    //Getter setter for defining IP addres of host server AKA the UR5 robot
    // TODO: Should probably return const reference
    std::string getIP() const;
    void setIP(const std::string &value);

    //Data polling for datasharing
    void startPolling();
    void stopPolling();

    // NOTE: Really should probably be returned as const reference
    std::vector<double> getLastPose();

    int getPollingRate() const;
    void setPollingRate(int pollingRate);

    //returning pointer to the datastruct.
    UR_STRUCT *getURStruct() const;

private:

    //private functions
    void getData();
    void init();
    void initRobot();

        //move with rad input
        bool moveJ(const std::vector<double> &q);
        bool moveJ(const std::vector<double> &q, double speed, double acceleration);


    //flags
    bool isConnected = false;

    //Member Variables
    bool mContinue = true;
    int mPollingRate = 20; //defined in hz
    std::string mIP = "127.0.0.0";
    std::vector<double> *mJoints = nullptr; //pointer to

    ur_rtde::RTDEControlInterface *mUrControl = nullptr;
    ur_rtde::RTDEReceiveInterface *mUrRecieve = nullptr;

    UR_STRUCT *mURStruct = nullptr;

    std::exception_ptr mEptr = nullptr;

    //threads
    std::thread *mThread = nullptr;
    std::mutex urMutex;
};

#endif // UR_CONTROL_H
