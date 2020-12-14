#ifndef QDATABASEHANDLER_H
#define QDATABASEHANDLER_H
#pragma once

#ifndef LOG_DEFINES
#define LOG_DEFINES 1
#define logstd wxLogMessage
#define logwar wxLogWarning
#define logerr wxLogError
#endif
#include "wx/log.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <type_traits>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>

#include "../logic/globaldefs.hpp"
#include "../logic/xgeometrytypes.hpp"
#include "mysql-cppconn-8/mysqlx/xdevapi.h"

using namespace mysqlx;

class qDatabaseHandler
{
public:
    qDatabaseHandler();
    ~qDatabaseHandler();
    void disconnect();
    void setDatabaseCredentials(const std::string& user,
                                const std::string& password,
                                const std::string& hostname,
                                const std::string& schema,
                                uint32_t port);
    std::vector<Row>* showTables();
    std::vector<qDatabaseEntry*> retriveData();

    template <class qDatabaseEntryDerivative>
    void pushLogEntry(qDatabaseEntryDerivative entry) {
        // Test for derivative classes, allocate on heap, call ptr version of func
        if constexpr (std::is_same_v<qDatabaseEntryDerivative, qDatabaseBallEntry<double>>) {
            static_cast<qDatabaseBallEntry<double>>(entry);
            qDatabaseBallEntry<double>* e = new qDatabaseBallEntry(entry);
            pushLogEntryPtr(e);
        } else if constexpr (std::is_same_v<qDatabaseEntryDerivative, qDatabaseMoveEntry<double>>) {
            static_cast<qDatabaseMoveEntry<double>>(entry);
            qDatabaseMoveEntry<double>* e = new qDatabaseMoveEntry(entry);
            pushLogEntryPtr(e);
        } else if constexpr (std::is_same_v<qDatabaseEntryDerivative, qDatabaseThrowEntry<double>>) {
            static_cast<qDatabaseThrowEntry<double>>(entry);
            qDatabaseThrowEntry<double>* e = new qDatabaseThrowEntry(entry);
            pushLogEntryPtr(e);
        } else if constexpr (std::is_same_v<qDatabaseEntryDerivative, qDatabaseGripperEntry<double>>) {
            static_cast<qDatabaseGripperEntry<double>>(entry);
            qDatabaseGripperEntry<double>* e = new qDatabaseGripperEntry(entry);
            pushLogEntryPtr(e);
        }
    }
    void pushLogEntryPtr(qDatabaseEntry* entry);
private:
    void mWorkerThreadLoop();
    bool mPushEntryToLog(qDatabaseEntry* entry);
private:
    // Threading stuff
    std::mutex mMtx;
    std::thread *mThread;
    std::atomic<bool> mThreadLife;
    std::queue<qDatabaseEntry*> mWorkQueue;

    // Member variables
    std::string mUser;
    std::string mPassword;
    std::string mHost;
    std::string mDatabase;
    uint32_t mPort;
    SSLMode mSsl_mode = SSLMode::DISABLED;

    Session *mSession;
    Schema *mSchema;
    Table *mTable;
    std::vector<Row> *mRes;

    // Functions
    Session* connect();

};

#endif // QDATABASEHANDLER_H
