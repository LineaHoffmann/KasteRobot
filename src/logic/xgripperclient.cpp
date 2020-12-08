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
    mGetDataReady.exchange(false);
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
                this->writeRead("RELEASE(0.5)");
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
            }
        }
        if (mDisconnectReq.load()) {                //DISCONNECT
            if (mConnected.load()) {
                this->disconnectGripper();
                mConnected.exchange(false);
                mDisconnectReq.exchange(false);
            }
            else {
                logstd("Gripper not connected");
                mDisconnectReq.exchange(false);
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
    auto t1Start = std::chrono::high_resolution_clock::now();
    char buf[32];
    send(mSock, mCommand.c_str(), mCommand.size(), 0); // Sending command

    memset(buf, 0, 32);
    read(mSock, buf, 32); //Reading response
    std::string answer(buf);
    mAnswer = answer;
    auto t1Stop = std::chrono::high_resolution_clock::now();
    logstd(mAnswer.c_str());
    if (mAnswer[0] == 'E') {
        mReady.exchange(true);
        return false;
    }
    memset(buf, 0, 32);
    read(mSock, buf, 32); //Reading response
    std::string test(buf);
    auto t2Stop = std::chrono::high_resolution_clock::now();
    if (test[0] == 'F') {
        mReady.exchange(true);
    }
    logstd(test.c_str());
    std::chrono::duration<double> timerStartAck = (t1Stop - t1Start);
    std::chrono::duration<double> timerStartFin = (t2Stop - t1Start);
    if (mCommand[0] == 'R') {
        std::cout << "Timer start to ACK: " << timerStartAck.count()*1000 << " ms" << std::endl;
        std::cout << "Timer start to FIN: " << timerStartFin.count()*1000 << " ms" << std::endl;
    }
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
        mTData->join();
        delete mTData;
        send(mSock, "AUTOSEND(POS, 0, true)",23 , 0);
        send(mSock, "AUTOSEND(SPEED, 0, true)",25 , 0);
        send(mSock, "AUTOSEND(FORCE, 0, true)",25 , 0);
        send(mSock, "AUTOSEND(TEMP, 0, true)",24 , 0);
        send(mSock, "AUTOSEND(GRIPSTATE, 0, true)",29 , 0);
        mAutosendCmd = false;
    }
    else {mAutosend.exchange(true);
        logstd("Autosend activated");
        mTData = new std::thread(&xGripperClient::autoread, this);
    }

}
void xGripperClient::autoread() {
    if (!mAutosendCmd && isConnected()) {
        send(mSock, "AUTOSEND(POS, 10, true)",23 , 0);
        send(mSock, "AUTOSEND(SPEED, 10, true)",25 , 0);
        send(mSock, "AUTOSEND(FORCE, 10, true)",25 , 0);
        send(mSock, "AUTOSEND(TEMP, 10, true)",24 , 0);
        send(mSock, "AUTOSEND(GRIPSTATE, 10, true)",29 , 0);
        mAutosendCmd = true;
    }

    char buf[20];
    memset(buf, 0, 20);

    while (mAutosend.load() && isConnected()) {
        read(mSock, buf, 20);
        std::string answer(buf);

        std::string data = answer.substr(answer.find("="), 5);
        if (answer[1] == 'P') {
            std::lock_guard<std::mutex> dataLock(mMtx);
            mPos = data;
        }
        else if (answer[1] == 'F') {
            std::lock_guard<std::mutex> dataLock(mMtx);
            mForce = data;
        }
        else if (answer[1] == 'T') {
            std::lock_guard<std::mutex> dataLock(mMtx);
            mTemp = data;
        }
        else if (answer[1] == 'S') {
            std::lock_guard<std::mutex> dataLock(mMtx);
            mSpeed = data;
        }
        else if (answer[1] == 'G') {
            std::lock_guard<std::mutex> dataLock(mMtx);
            mGripstate = data;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
gripperData xGripperClient::getData() {
    gripperData fullData;
    std::lock_guard<std::mutex> dataLock(mMtx);
    fullData.pos = mPos;
    fullData.temp = mTemp;
    fullData.force = mForce;
    fullData.speed = mSpeed;
    fullData.gripstate = mGripstate;

    return fullData;

}

