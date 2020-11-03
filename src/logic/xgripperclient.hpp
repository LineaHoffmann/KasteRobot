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
    void writeRead(std::string mCommand);
    void disconnectGripper();
    void grip();
    void release();
    void home();

private:
    void connectSocket(std::string ipAddress, int port);
    void entryThread();
    void startThread();

private:
    int mSock, mConnectS, mPort;
    std::string mIpAddress, mCommand, mUserInput, mAnswer;
    std::string mDisconnectCmd = "BYE()\n";
    sockaddr_in mHint;
    std::thread* mT1;
    std::mutex mMtx;
    std::atomic<bool> mTRuntime;

    std::thread *mWorker;

};


#endif // XGRIPPERCLIENT_H
