#include "gripperclient.h"

GripperClient::GripperClient() {
    this->mT1 = new std::thread(&GripperClient::EntryThread, this);
}


void GripperClient::EntryThread() {
    std::cout << "Client Thread started" << std::endl;


}

//void GripperClient::CreateSocket() {
//    int sock_ = socket(AF_INET, SOCK_STREAM, 0);
// }


void GripperClient::ConnectSocket() {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int port = 1000;
    std::string ipAddress = "192.168.0.1";

    sockaddr_in mHint;
    mHint.sin_family = AF_INET;
    mHint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &mHint.sin_addr);

    connect(mSock, (sockaddr*)&mHint, sizeof(mHint));
    return;
}

std::string GripperClient::WriteRead(std::string mCommand) {
    char buf[32];
    int bytesRecieved = -1; //Resetting response length
    int sendRes = send(mSock, mCommand.c_str(), mCommand.size() + 1, 0); // Sending command

    memset(buf, 0, 32);
    bytesRecieved = recv(mSock, buf, 32, 0); //Reading response
        if (bytesRecieved == -1) {              //Waiting and retrying if no response
            bytesRecieved = recv(mSock, buf, 32, 0);
            usleep(250000);  //Microseconds; 0,25 second
        }
     std::string mAnswer(buf);
     return mAnswer;
}
