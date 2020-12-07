#ifndef XGRIPPERCLIENT_H
#define XGRIPPERCLIENT_H

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif

#include "wx/wx.h"

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include <thread>
#include <mutex>
#include <iostream>
#include <atomic>

#include "globaldefs.hpp"

class xGripperClient
{
public:
    xGripperClient();
    ~xGripperClient();
    bool writeRead(std::string mCommand);

    void grip();
    void release();
    void home();

    void autoreadReq();
    void disconnectReq();
    void connectReq(std::pair<std::string, int> ipPort);
    void setIpPort(std::pair<std::string, int>);
    bool isReady();
    gripperData getData();
    bool isConnected();

    bool ackValue = false;

private:
    void connectSocket();
    void disconnectGripper();
    void entryThread();
    void startThread();
    void autoread();



private:
    int mSock, mConnectS, mPort;
    std::string mIpAddress, mCommand, mUserInput, mAnswer, mPos, mForce, mTemp, mSpeed, mGripstate;
    std::string mDisconnectCmd = "BYE()\n";
    sockaddr_in mHint;
    std::thread* mT1;
    std::mutex mMtx;
    std::atomic<bool> mTRuntime, mReady, mConnected, mGripReq, mReleaseReq, mHomeReq,
                      mConnectReq, mDisconnectReq, mAutosend;
    std::pair<std::string, int> mIpPort;

    std::thread *mWorker;
    bool mAutosendCmd;
};


#endif // XGRIPPERCLIENT_H
