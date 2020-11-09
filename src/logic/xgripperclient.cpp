#include "xgripperclient.hpp"

xGripperClient::xGripperClient() {
    mTRuntime.exchange(true);
    mConnected.exchange(false);
    this->mT1 = new std::thread(&xGripperClient::entryThread, this);
}


void xGripperClient::entryThread() {
    mConnected.exchange(false);                     //DEFAULT FLAG VALUES
    mConnectReq.exchange(false);
    mDisconnectReq.exchange(false);
    mGripReq.exchange(false);
    mReleaseReq.exchange(false);
    mHomeReq.exchange(false);
    mUpdateReq.exchange(false);
    mTRuntime.exchange(true);

    mIpAddress = "192.168.100.10";
    mPort = 1000;                                   //DEFAULT VALUES

    logstd("Gripper client thread started .. ");


    while (mTRuntime.load()) {
        if (mGripReq.load()) {                      //GRIP
            if (mConnected.load()) {
                this->writeRead("GRIP()");
            }
                else {logstd("Gripper not connected");}
            mGripReq.exchange(false);
        }
        if (mReleaseReq.load()) {                   //RELEASE
            if (mConnected.load()) {
                this->writeRead("RELEASE()");
            }
                else {logstd("Gripper not connected");}
            mReleaseReq.exchange(false);
        }
        if (mHomeReq.load()) {                      //HOME
            if (mConnected.load()) {
                this->writeRead("HOME()");
            }
                else {logstd("Gripper not connected");}
            mHomeReq.exchange(false);
        }
        if (mConnectReq.load()) {                   //CONNECT
            if (mConnected.load()) {
                logstd("Already connected");
                mConnectReq.exchange(false);
            }
            else {
                this->connectSocket();
                mConnectReq.exchange(false);
                mConnected.exchange(true);
            }
        }
        if (mDisconnectReq.load()) {                //DISCONNECT
            if (mConnected.load()) {
                this->disconnectGripper();
                mConnected.exchange(false);
                mDisconnectReq.exchange(false);
            }
        }
        if (mUpdateReq.load()) {                    //UPDATE IP/PORT
            if (!mConnected.load()) {

            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


void xGripperClient::connectSocket() {
    mSock = socket(AF_INET, SOCK_STREAM, 0);
    mHint.sin_family = AF_INET;
    mHint.sin_port = htons(mPort);
    inet_pton(AF_INET, mIpAddress.c_str(), &mHint.sin_addr);

    if (connect(mSock, (sockaddr*)&mHint, sizeof(mHint)) == 0) {
    logstd("Gripper client connected");
    mConnected.exchange(true);
    }
    else {
        logstd("Gripper connection failed");
        mConnected.exchange(false);
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
    mGripReq.exchange(true);
}

void xGripperClient::release() {
    mReleaseReq.exchange(true);
}

void xGripperClient::home() {
    mHomeReq.exchange(true);
}

void xGripperClient::setIpPort(std::pair<std::string, int> ipPort) {
    {
    std::lock_guard<std::mutex>ipPortLock(mMtx);
    mIpAddress = ipPort.first;
    mPort = ipPort.second;
    }
}

void xGripperClient::connectReq() {
    mConnectReq.exchange(true);
}

void xGripperClient::disconnectReq() {
    mDisconnectReq.exchange(true);
}

bool xGripperClient::writeRead(std::string command) {
    mCommand = command + "\n";
    std::cout << mCommand << std::endl;
    char buf[32];
    send(mSock, mCommand.c_str(), mCommand.size(), 0); // Sending command


    memset(buf, 0, 32);
    read(mSock, buf, 32); //Reading response
    std::string answer(buf);
    mAnswer = answer;
    logstd(mAnswer.c_str());
    if (mAnswer[0] == 'E') {
        return false;
    }


    memset(buf, 0, 32);
    read(mSock, buf, 32); //Reading response
    std::string test(buf);
    logstd(test.c_str());
    return true;

}

std::string xGripperClient::getAnswer() {
    if (!mConnected) {
       return std::string();
    }
    std::string answer = mAnswer;
    mAnswer = "";
    mReady.exchange(true);
    return answer;
}









