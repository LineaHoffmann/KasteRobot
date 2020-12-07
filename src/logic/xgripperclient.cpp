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
    mTRuntime.exchange(true);
    mAutosend.exchange(false);
    mReady.exchange(false);
    mAutosendCmd = false;

    logstd("Gripper client thread started .. ");


    while (mTRuntime.load()) {
        if (mGripReq.load()) {                      //GRIP
            if (mConnected.load()) {
                this->writeRead("GRIP(10, 40)");    // 10 N gripping force, 40 mm part width
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
                logstd("Try home");
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
                }
        }
        if (mDisconnectReq.load()) {                //DISCONNECT
            if (mConnected.load()) {
                this->disconnectGripper();
                mConnected.exchange(false);
                mDisconnectReq.exchange(false);
            }
        }
        if (mAutosend.load()) {
            if (mConnected.load()) {
                this->autoread();
            }
            else {
                mAutosend.exchange(false);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

//CONNECTION
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

bool xGripperClient::isConnected() {
    return mConnected.load();
}

void xGripperClient::setIpPort(std::pair<std::string, int> ipPort) {
    {
        std::lock_guard<std::mutex>ipPortLock(mMtx);
        mIpAddress = ipPort.first;
        mPort = ipPort.second;
    }
}

void xGripperClient::connectReq(std::pair<std::string, int> ipPort) {
    setIpPort(ipPort);
    mConnectReq.exchange(true);
}


void xGripperClient::disconnectReq() {
    mDisconnectReq.exchange(true);
}


//MOVEMENT
void xGripperClient::grip() {
    mGripReq.exchange(true);
}

void xGripperClient::release() {
    mReleaseReq.exchange(true);
}

void xGripperClient::home() {
    mHomeReq.exchange(true);
}

bool xGripperClient::writeRead(std::string command) {
    mReady.exchange(false);
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
        mReady.exchange(true);
        return false;
    }
    memset(buf, 0, 32);
    read(mSock, buf, 32); //Reading response
    std::string test(buf);
    if (test[0] == 'F') {
        mReady.exchange(true);
    }
    logstd(test.c_str());
    return true;
}

bool xGripperClient::isReady() {
    if (mReady.load()) {
        return true;
    }
    else {return false;}
}


//AUTOREAD
void xGripperClient::autoreadReq() {
    if (mAutosend.load()) {
        mAutosend.exchange(false);
        logstd("Autosend diabled");
    }
    else {mAutosend.exchange(true);
        logstd("Autosend activated");
    }

}
void xGripperClient::autoread() {
    if (!mAutosendCmd) {
        send(mSock, "AUTOSEND(POS, 10, true)",23 , 0);
        send(mSock, "AUTOSEND(SPEED, 10, true)",25 , 0);
        send(mSock, "AUTOSEND(FORCE, 10, true)",25 , 0);
        send(mSock, "AUTOSEND(TEMP, 10, true)",24 , 0);
        send(mSock, "AUTOSEND(GRIPSTATE, 10, true)",29 , 0);
        mAutosendCmd = true;
    }
    else {
        char buf[20];
        memset(buf, 0, 20);
        read(mSock, buf, 20);
        std::string answer(buf);

        std::string data = answer.substr(answer.find("="), 5);
        if (answer[1] == 'P') {
            mPos = data;
        }
        if (answer[1] == 'F') {
            mForce = data;
        }
        if (answer[1] == 'T') {
            mTemp = data;
        }
        if (answer[1] == 'S') {
            mSpeed = data;
        }
        if (answer[1] == 'G') {
            mGripstate = data;
        }
    }
}

gripperData xGripperClient::getData() {
    gripperData fullData;
    fullData.pos = mPos;
    fullData.temp = mTemp;
    fullData.force = mForce;
    fullData.speed = mSpeed;
    fullData.gripstate = mGripstate;

    return fullData;
}

