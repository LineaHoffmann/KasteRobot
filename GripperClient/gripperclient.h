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
    void CreateSocket();
    void ConnectSocket();
    std::string WriteRead(std::string mCommand);
    void EntryThread();
    void StartThread();

private:
    int mSock, mConnectS;
    std::string mIpAddress, mCommand, mUserInput, mAnswer;
    sockaddr_in mHint;
    std::thread* mT1;
    std::mutex mMtx;

    std::thread *mWorker;

};


#endif // GRIPPERCLIENT_H
