#include "xgripperclient.hpp"

xGripperClient::xGripperClient() {
    mTRuntime.exchange(true);
    this->mT1 = new std::thread(&xGripperClient::entryThread, this);
}


void xGripperClient::entryThread() {

    logstd("Gripper client thread started .. ");
    xGripperClient::connectSocket("192.168.100.10", 1000);

    while (mTRuntime.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


void xGripperClient::connectSocket(std::string ipAddress, int port) {
    mSock = socket(AF_INET, SOCK_STREAM, 0);
    mPort = port;
    std::string mIpAddress = ipAddress;

    sockaddr_in mHint;
    mHint.sin_family = AF_INET;
    mHint.sin_port = htons(mPort);
    inet_pton(AF_INET, mIpAddress.c_str(), &mHint.sin_addr);

    std::cout << connect(mSock, (sockaddr*)&mHint, sizeof(mHint)) << std::endl;
}

xGripperClient::~xGripperClient() {
    std::string bye = "BYE()\n";
    send(mSock, bye.c_str(), bye.size() +1, 0);
    mTRuntime.exchange(false);
    mT1->join();
    delete mT1;

}


std::string xGripperClient::writeRead(std::string command) {
    mCommand = command + "\n";
    std::cout << mCommand << std::endl;
    char buf[32];
    int bytesRecieved = 0; //Resetting response length
    send(mSock, mCommand.c_str(), mCommand.size() + 1, 0); // Sending command


    //memset(buf, 0, 32);
    bytesRecieved = recv(mSock, buf, 32, 0); //Reading response
    std::string mAnswer(buf);
    std::cout << mAnswer << std::endl;



    //memset(buf, 0, 32);
    bytesRecieved = recv(mSock, buf, 32, 0); //Reading response
    std::string test(buf);
    std::cout << test << std::endl;


    return mAnswer;
}
