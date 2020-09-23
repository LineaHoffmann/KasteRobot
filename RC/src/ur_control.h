#ifndef UR_CONTROL_H
#define UR_CONTROL_H

#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>

class myExcep : public std::exception{

    const char* what() const throw()
    {
        return "Connection to host could not be resolved";
    }
};


class UR_Control
{
    //definition of data struct
    struct UR_STRUCT {

        std::vector<double> pose{0};
        bool isConnected = false;

    };

public:
    UR_Control(std::string IP);

    ~UR_Control();

    //move with rad input
    bool moveJ(const std::vector<double> &q);
    bool moveJ(const std::vector<double> &q, double speed, double acceleration);

    //move with degree input
    bool moveJDeg(const std::vector<double> &qDeg);
    bool moveJDeg(const std::vector<double> &qDeg, double speed, double acceleration);

    //read current pose in rads or deg
    std::vector<double> getCurrentPose();
    std::vector<double> getCurrentPoseDeg();

    //Getter setter for defining IP addres of host server AKA the UR5 robot
    std::string getIP() const;
    void setIP(const std::string &value);

    //Data polling for datasharing
    void startPolling();
    void stopPolling();

    std::vector<double> getLastPose();

    int getPollingRate() const;
    void setPollingRate(int pollingRate);

private:

    //helping functions
    std::vector<double> degToRad(const std::vector<double> &qDeg);
    std::vector<double> radToDeg(const std::vector<double> &qRad);
    void getData();

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
