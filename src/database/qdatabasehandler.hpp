#ifndef QDATABASEHANDLER_H
#define QDATABASEHANDLER_H

#include "../logic/xgeometrytypes.hpp"
#include "mysql-cppconn-8/mysqlx/xdevapi.h"
#include <iostream>
#include <vector>

using namespace mysqlx;


enum ROBOT_MOVE_TYPE {
    // xUrControl has this currently, we just might need some access systemwide
};
enum GRIPPER_MOVE_TYPE {
    // Something, something, same as above ..
};
struct qDatabaseEntry {
    std::string timestamp;
    std::string description;
protected:
    qDatabaseEntry(const std::string& t, const std::string& desc) :
        timestamp(t), description(desc) {};
};
template <class T>
struct qDatabaseMoveEntry : public qDatabaseEntry {
    qDatabaseMoveEntry(const std::string& t, const std::string& d,
                       const point6D<T>& s, const point6D<T>& e, ROBOT_MOVE_TYPE m) :
        qDatabaseEntry(t, d), start(s), end(e), moveType(m) {};
    point6D<T> start, end;
    ROBOT_MOVE_TYPE moveType;
};
template <class T>
struct qDatabaseThrowEntry : public qDatabaseEntry {
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
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseGripperEntry(const std::string& t, const std::string& d, T s, T e) :
        qDatabaseEntry(t, d), start(s), end(e) {};
    bool successful;
    T start, end;
};
template <class T>
struct qDatabaseBallEntry : public qDatabaseEntry {
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

    Session *connect(); // KIg her.
    void disconnect();

    void setDatabaseCredentials(std::tuple<std::string, std::string, std::string, std::string, uint32_t> credentialsInput);


    std::vector<Row>* showTables();
    bool qInsert();

    std::vector<Row>* getDbData(std::string tableName);
    //bool writeSql(std::vector inputList); //WARNING must add type specifier for the vector, therefor commented.


private:
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
};

#endif // QDATABASEHANDLER_H
