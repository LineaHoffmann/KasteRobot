#ifndef GRIPPERCLIENT_H
#define GRIPPERCLIENT_H
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <thread>
#include <mutex>


class GripperClient
{
public:
    GripperClient();
    void connectSocket(std::string ipAddress, int port);
    std::string writeRead(std::string mCommand);
    void entryThread();
    void startThread();

private:
    int mSock, mConnectS, mPort;
    std::string mIpAddress, mCommand, mUserInput, mAnswer;
    sockaddr_in mHint;
    std::thread* mT1;
    std::mutex mMtx;

    std::thread *mWorker;

};


#endif // GRIPPERCLIENT_H
