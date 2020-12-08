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

#include "../logic/globaldefs.hpp"
#include "../logic/xgeometrytypes.hpp"
#include "mysql-cppconn-8/mysqlx/xdevapi.h"

using namespace mysqlx;

struct qDatabaseEntry {
    std::string timestamp;
    std::string entryType;
protected:
    qDatabaseEntry(){}
    qDatabaseEntry(const std::string& t, const std::string& desc) :
        timestamp(t), entryType(desc) {}
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseEntry &p) {
        return os << "Entry type: " << p.entryType << "\n"
                  << "Timestamp: " << p.timestamp;
    }
};
template <class T>
struct qDatabaseMoveEntry : public qDatabaseEntry {
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
template <class T>
struct qDatabaseThrowEntry : public qDatabaseEntry {
    qDatabaseThrowEntry(){}
    qDatabaseThrowEntry(const std::string& t, const std::string& d,
                        bool s, const point6D<T>& rp, double v1, double v2, double de) :
        qDatabaseEntry(t, d), successful(s), releasePoint(rp),
        releaseVelocityCalced(v1), releaseVelocityActual(v2), deviation(de) {}
    bool successful;
    point6D<T> releasePoint;
    double releaseVelocityCalced, releaseVelocityActual, deviation;
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseThrowEntry &p) {
        return os << (qDatabaseEntry) p << "\n"
                  << "Succesful: " << p.successful << "\n"
                  << "Release point: " << p.releasePoint << "\n"
                  << "Calc release velocity: " << p.releaseVelocityCalced << "\n"
                  << "Actual release velocity: " << p.releaseVelocityActual << "\n"
                  << "~Deviation: " << p.deviation;
    }
};
template <class T>
struct qDatabaseGripperEntry : public qDatabaseEntry {
    qDatabaseGripperEntry(){}
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseGripperEntry(const std::string& t, const std::string& d, T s, T e) :
        qDatabaseEntry(t, d), start(s), end(e) {}
    bool successful;
    T start, end;
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseGripperEntry &p) {
        return os << (qDatabaseEntry) p << "\n"
                  << "Start width: " << p.start << "\n"
                  << "End width: " << p.end << "\n"
                  << "Succesful: " << p.successful;
    }
};
template <class T>
struct qDatabaseBallEntry : public qDatabaseEntry {
    qDatabaseBallEntry(){}
    qDatabaseBallEntry(const std::string& t, const std::string& d, T di, const point2D<T>& p) :
        qDatabaseEntry(t, d), ballDiameter(di), ballPosition(p) {}
    double ballDiameter;
    point2D<T> ballPosition;
    friend std::ostream& operator<<(std::ostream&os, const qDatabaseBallEntry &p) {
        return os << (qDatabaseEntry) p << "\n"
                  << "Position: " << p.ballPosition << "\n"
                  << "Size: " << p.ballSize;
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
    std::vector<qDatabaseEntry> retriveData();

    template <class T>
    bool pushLogEntry(T _t) {
        // Detect sub-type
        // Build SQL statements and execute them
        // Read the just added data from the database to check succes
        if constexpr (std::is_same_v<T, qDatabaseBallEntry>) {
            static_cast<qDatabaseBallEntry<double>>(_t);
            std::stringstream s;

        } else if constexpr (std::is_same_v<T, qDatabaseMoveEntry>) {
            static_cast<qDatabaseMoveEntry<double>>(_t);
            std::stringstream s;

        } else if constexpr (std::is_same_v<T, qDatabaseThrowEntry>) {
            static_cast<qDatabaseThrowEntry<double>>(_t);
            std::stringstream s;

        } else if constexpr (std::is_same_v<T, qDatabaseGripperEntry>) {
            static_cast<qDatabaseGripperEntry<double>>(_t);
            std::stringstream s;

        } else {
            logerr("Cannot push this type of data to the database!");
            return false;
        }
    }

private:
    // Member variables
    std::string mUser;
    std::string mPassword;
    std::string mHost;
    std::string mDatabase;
    uint32_t mPort;
    SSLMode mSsl_mode = SSLMode::DISABLED;

    Session *mSession = nullptr;
    Schema *mSchema = nullptr;
    Table *mTable = nullptr;
    std::vector<Row> *mRes;


    // Functions
    Session* connect();

};

#endif // QDATABASEHANDLER_H
