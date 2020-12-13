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

struct qDatabaseEntry {
    std::string timestamp;
    std::string entryType;
    virtual ~qDatabaseEntry() = default; // Virtual to make dynamic_cast possible
protected:
    qDatabaseEntry(){}
    qDatabaseEntry(const std::string& t, const std::string& desc) :
        timestamp(t), entryType(desc) {}
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseEntry &p) {
        return os << "Entry type: " << p.entryType << "\n"
                  << "Timestamp: " << p.timestamp;
    }
};
template <typename T>
struct qDatabaseMoveEntry : public qDatabaseEntry {
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseMoveEntry(){}
    qDatabaseMoveEntry(const std::string& t, const std::string& d,
                       const point6D<T>& s, const point6D<T>& e, ROBOT_MOVE_TYPE m) :
        qDatabaseEntry(t, d), start(s), end(e), moveType(m) {}
    point6D<T> start, end;
    ROBOT_MOVE_TYPE moveType;
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseMoveEntry &p) {
        return os << (qDatabaseEntry) p << "\n"
                  << "Start point: " << p.start << "\n" << "End point: " << p.end << "\n"
                  << "Move type: " << p.moveType;
    }
};
template <typename T>
struct qDatabaseThrowEntry : public qDatabaseEntry {
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseThrowEntry(){}
    qDatabaseThrowEntry(const std::string& t, const std::string& d,
                        bool s, const point6D<T>& rp, T v1, T v2, T de) :
        qDatabaseEntry(t, d), successful(s), releasePoint(rp),
        releaseVelocityCalced(v1), releaseVelocityActual(v2), deviation(de) {}
    bool successful;
    point6D<T> releasePoint;
    T releaseVelocityCalced, releaseVelocityActual, deviation;
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseThrowEntry &p) {
        return os << (qDatabaseEntry) p << "\n"
                  << "Succesful: " << p.successful << "\n"
                  << "Release point: " << p.releasePoint << "\n"
                  << "Calc release velocity: " << p.releaseVelocityCalced << "\n"
                  << "Actual release velocity: " << p.releaseVelocityActual << "\n"
                  << "~Deviation: " << p.deviation;
    }
};
template <typename T>
struct qDatabaseGripperEntry : public qDatabaseEntry {
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseGripperEntry(){}
    qDatabaseGripperEntry(const std::string& t, const std::string& d, bool suc, T s, T e) :
        qDatabaseEntry(t, d), successful(suc), start(s), end(e) {}
    bool successful;
    T start, end;
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseGripperEntry &p) {
        return os << (qDatabaseEntry) p << "\n"
                  << "Start width: " << p.start << "\n"
                  << "End width: " << p.end << "\n"
                  << "Succesful: " << p.successful;
    }
};
template <typename T>
struct qDatabaseBallEntry : public qDatabaseEntry {
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseBallEntry(){}
    qDatabaseBallEntry(const std::string& t, const std::string& d, T di, const point2D<T>& p) :
        qDatabaseEntry(t, d), ballDiameter(di), ballPosition(p) {}
    T ballDiameter;
    point2D<T> ballPosition;
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseBallEntry &p) {
        return os << (qDatabaseEntry) p << "\n"
                  << "Position: " << p.ballPosition << "\n"
                  << "Size: " << p.ballDiameter;
    }
};

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
    }
    void pushLogEntry(qDatabaseEntry* entry);
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
