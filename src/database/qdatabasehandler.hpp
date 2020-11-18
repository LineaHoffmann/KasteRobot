#ifndef QDATABASEHANDLER_H
#define QDATABASEHANDLER_H

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
template <class T>
struct point6D {
    static_assert(std::is_floating_point_v<T>, "Only for floating point numbers!");
    point6D(T _x, T _y, T _z, T _rx, T _ry, T _rz) :
        x(_x), y(_y), z(_z), rx(_rx), ry(_ry), rz(_rz) {};
    T x, y, z, rx, ry, rz;
    point6D<T> operator+(const point6D<T> &p) {return point6D<T>(x + p.x, y + p.y, z + p.z, rx + p.rx, ry + p.ry, rz + p.rz);}
    point6D<T> operator-(const point6D<T> &p) {return point6D<T>(x - p.x, y - p.y, z - p.z, rx - p.rx, ry - p.ry, rz - p.rz);}
    point6D<T> operator/(const double &div) {return point6D<T>(x / div, y / div, z / div, rx / div, ry / div, rz / div);}
    point6D<T> operator*(const double &mul) {return point6D<T>(x * mul, y * mul, z * mul, rx * mul, ry * mul, rz * mul);}
    bool operator==(const point6D<T> &p) {return (x == p.x && y == p.y && z == p.z && rx == p.x && ry == p.y && rz == p.z) ? true : false;}
    friend std::ostream& operator<<(std::ostream &os, const point6D &p) {return os << "(" << p.x << ", " << p.y << ", " << p.z
                                                                                   << ", " << p.rx << ", " << p.ry << ", " << p.rz << ")";}
};
template <class T>
struct point2D {
    static_assert(std::is_floating_point_v<T>, "Only for floating point numbers!");
    point2D(T _x, T _y) : x(_x), y(_y) {};
    T x, y;
    point2D<T> operator+(const point2D<T> &p) {return point2D<T>(x + p.x, y + p.y);}
    point2D<T> operator-(const point2D<T> &p) {return point2D<T>(x - p.x, y - p.y);}
    point2D<T> operator/(const double &div) {return point2D<T>(x / div, y / div);}
    point2D<T> operator*(const double &mul) {return point2D<T>(x * mul, y * mul);}
    bool operator==(const point2D<T> &p) {return (x == p.x && y == p.y) ? true : false;}
    friend std::ostream& operator<<(std::ostream &os, const point2D &p) {return os << "(" << p.x << ", " << p.y << ")";}
};
struct qDatabaseEntry {
    qDatabaseEntry(const std::string& t, const std::string& desc) :
        timestamp(t), description(desc) {};
    std::string timestamp;
    std::string description;
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
                        bool s, const point6D<T>& rp, double v, double xy,
                        double cz, double de) :
        qDatabaseEntry(t, d), successful(s), releasePoint(rp),
        releaseVelocity(v), angleXY(xy),
        angleCZ(cz), deviation(de) {};
    bool successful;
    point6D<T> releasePoint;
    double releaseVelocity, angleXY, angleCZ, deviation;
};
template <class T>
struct qDatabaseGripperEntry : qDatabaseEntry {
    static_assert (std::is_floating_point_v<T>, "Must be a floating point value!");
    qDatabaseGripperEntry(const std::string& t, const std::string& d, T s, T e) :
        qDatabaseEntry(t, d), start(s), end(e) {};
    bool successful;
    T start, end;
};
template <class T>
struct qDatabaseBallEntry : qDatabaseEntry {
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

    Session *connect();
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
