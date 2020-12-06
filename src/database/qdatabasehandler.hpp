#ifndef QDATABASEHANDLER_H
#define QDATABASEHANDLER_H
#pragma once

#include "../logic/globaldefs.hpp"
#include "../logic/xgeometrytypes.hpp"
#include "mysql-cppconn-8/mysqlx/xdevapi.h"
#include <iostream>
#include <vector>
#include <sstream>

using namespace mysqlx;

struct qDatabaseEntry {
    std::string timestamp;
    std::string entryType;
protected:
    qDatabaseEntry(){};
    qDatabaseEntry(const std::string& t, const std::string& desc) :
        timestamp(t), entryType(desc) {};
};
template <class T>
struct qDatabaseMoveEntry : public qDatabaseEntry {
    qDatabaseMoveEntry(){};
    qDatabaseMoveEntry(const std::string& t, const std::string& d,
                       const point6D<T>& s, const point6D<T>& e, ROBOT_MOVE_TYPE m) :
        qDatabaseEntry(t, d), start(s), end(e), moveType(m) {};
    point6D<T> start, end;
    ROBOT_MOVE_TYPE moveType;
};
template <class T>
struct qDatabaseThrowEntry : public qDatabaseEntry {
    qDatabaseThrowEntry(){};
    qDatabaseThrowEntry(const std::string& t, const std::string& d,
                        bool s, const point6D<T>& rp, double v, double de) :
        qDatabaseEntry(t, d), successful(s), releasePoint(rp),
        releaseVelocity(v), deviation(de) {};
    bool successful;
    point6D<T> releasePoint;
    double releaseVelocity, deviation;
};
template <class T>
struct qDatabaseGripperEntry : public qDatabaseEntry {
    qDatabaseGripperEntry(){};
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseGripperEntry(const std::string& t, const std::string& d, T s, T e) :
        qDatabaseEntry(t, d), start(s), end(e) {};
    bool successful;
    T start, end;
};
template <class T>
struct qDatabaseBallEntry : public qDatabaseEntry {
    qDatabaseBallEntry(){};
    qDatabaseBallEntry(const std::string& t, const std::string& d, T di, const point2D<T>& p) :
        qDatabaseEntry(t, d), ballDiameter(di), ballPosition(p) {};
    double ballDiameter;
    point2D<T> ballPosition;
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
    void retriveData(); // Perhaps make private

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
    Session *connect();
};

#endif // QDATABASEHANDLER_H
