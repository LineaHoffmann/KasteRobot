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

class xGripperClient
{
public:
    xGripperClient();
    ~xGripperClient();
    bool writeRead(std::string mCommand);
    std::string getAnswer();

    void grip();
    void release();
    void home();
    void disconnectReq();
    void connectReq();
    void setIpPort(std::pair<std::string, int>);
    bool isReady();

private:
    void connectSocket();
    void disconnectGripper();
    void entryThread();
    void startThread();



private:
    int mSock, mConnectS, mPort;
    std::string mIpAddress, mCommand, mUserInput, mAnswer;
    std::string mDisconnectCmd = "BYE()\n";
    sockaddr_in mHint;
    std::thread* mT1;
    std::mutex mMtx;
    std::atomic<bool> mTRuntime, mReady, mConnected, mGripReq, mReleaseReq, mHomeReq,
                      mConnectReq, mDisconnectReq, mUpdateReq;
    std::pair<std::string, int> mIpPort;

    std::thread *mWorker;

};


#endif // XGRIPPERCLIENT_H
