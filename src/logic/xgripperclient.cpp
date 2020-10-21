#include "xgripperclient.h"

GripperClient::GripperClient() {
    this->mT1 = new std::thread(&GripperClient::entryThread, this);
}


void GripperClient::entryThread() {
    std::cout << "Client Thread started" << std::endl;
    // Use thread inhere
    connectSocket("192.168.0.1", 1000);
    // Use mutex to keep variables in thread safe

    //Check if new incoming data from gripper in a queue
    char buf[32];
    int bytesRecieved;
    memset(buf, 0, 32);
    bytesRecieved = recv(mSock, buf, 32, 0);
    std::string answer(buf);
    bool readBool = true;

    while (readBool = true) {
        if (bytesRecieved == 0 || answer == mAnswer) {
            usleep(25000);
            }
            else mAnswer = answer;
        }
}


void GripperClient::connectSocket(std::string ipAddress, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int mPort = port;
    std::string mIpAddress = ipAddress;

    sockaddr_in mHint;
    mHint.sin_family = AF_INET;
    mHint.sin_port = htons(mPort);
    inet_pton(AF_INET, mIpAddress.c_str(), &mHint.sin_addr);

    connect(mSock, (sockaddr*)&mHint, sizeof(mHint));
    }


std::string GripperClient::writeRead(std::string command) {
    mCommand = command;
    char buf[32];
    int bytesRecieved = 0; //Resetting response length
    int sendRes = send(mSock, mCommand.c_str(), mCommand.size() + 1, 0); // Sending command

    memset(buf, 0, 32);
    bytesRecieved = recv(mSock, buf, 32, 0); //Reading response
        if (bytesRecieved == 0) {              //Waiting and retrying if no response
            bytesRecieved = recv(mSock, buf, 32, 0);
            usleep(250000);  //Microseconds; 0,25 second
        }
     std::string mAnswer(buf);
     return mAnswer;
}

// Make functions for gripper functionality (Set grip, width, open, home, release etc.)
