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

    mHint.sin_family = AF_INET;
    mHint.sin_port = htons(mPort);
    inet_pton(AF_INET, mIpAddress.c_str(), &mHint.sin_addr);

    if (connect(mSock, (sockaddr*)&mHint, sizeof(mHint)) == 0) {
    logstd("Gripper client connected");
    }
    else {
        logstd("Gripper connection failed");
    }

}

void xGripperClient::disconnectGripper() {
    if (connect(mSock, (sockaddr*)&mHint, sizeof(mHint) == 0)) {
    send(mSock, mDisconnectCmd.c_str(), 7, 0);
    logstd("Disconnected");
    }
    else {
        logstd("No connection to disconnect");
    }
}

xGripperClient::~xGripperClient() {
    disconnectGripper();
    mTRuntime.exchange(false);
    mT1->join();
    delete mT1;

}

void xGripperClient::grip() {
    xGripperClient::writeRead("GRIP()");
}

void xGripperClient::release() {
    xGripperClient::writeRead("RELEASE()");
}

void xGripperClient::home() {
    xGripperClient::writeRead("HOME()");
}

bool xGripperClient::writeRead(std::string command) {
    mCommand = command + "\n";
    std::cout << mCommand << std::endl;
    char buf[32];
    int bytesRecieved = 0; //Resetting response length
    send(mSock, mCommand.c_str(), mCommand.size(), 0); // Sending command


    memset(buf, 0, 32);
    bytesRecieved = read(mSock, buf, 32); //Reading response
    std::string mAnswer(buf);
    logstd(mAnswer.c_str());
    if (mAnswer[0] == 'E') {
        return false;
    }


    memset(buf, 0, 32);
    bytesRecieved = read(mSock, buf, 32); //Reading response
    std::string test(buf);
    logstd(test.c_str());
    return true;

<<<<<<< Updated upstream
=======

    return mAnswer;
>>>>>>> Stashed changes
}











