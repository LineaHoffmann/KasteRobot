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

    template <class T>
    bool pushLogEntry(T _t) {

        // Create a new session and get the main log table
        Session *session = connect();
        Schema schema = session->getSchema("kasteRobot");
        Table table_log = schema.getTable("log");
        Table table_position = schema.getTable("position");

        // Detect sub-type
        //  Build SQL statements and execute them
        //  Read the just added data from the database to check success
        // NOTE SOEPE: constexpr
        if (std::is_same_v<T, qDatabaseBallEntry>) {
            table_log.insert("descriptor").values("ball").execute();
            Row logBallRow = table_log.select("log_ID").orderBy("created_at", "desc").execute().fetchOne();
            // Insert into position table
            std::string pos_values =
                    _t.ballPosition.x + "," +
                    _t.ballPosition.y;
            table_position.insert("x_pos,y_pos").values(pos_values);
            Row posBallRow = table_position.select("position_ID").orderBy("created_at_position", "desc").execute().fetchOne();
            // Insert into ball table
            Table table_ball = schema.getTable("ball");
            std::string t_values =
                    "'" + std::string(logBallRow[0]) + "'," +
                    _t.ballDiameter + ",'" +
                    std::string(posBallRow[0]) + "'";
            table_ball.insert("log_ID,diameter,ball_position").values(t_values).execute();
        } else if (std::is_same_v<T, qDatabaseMoveEntry>) {
            table_log.insert("descriptor").values("move").execute();
            Row logMoveRow = table_log.select("log_ID").orderBy("created_at", "desc").execute().fetchOne();
            // insert into position table
            std::string start_pos_values =
                    _t.start.x + "," +
                    _t.start.y + "," +
                    _t.start.z + "," +
                    _t.start.rx + "," +
                    _t.start.ry + "," +
                    _t.start.rz + ",";
            table_position.insert("x_pos,y_pos,z_pos,x_rotation,y_rotaion,z_rotaion").values(start_pos_values).execute();
            std::string end_pos_values =
                    _t.end.x + "," +
                    _t.end.y + "," +
                    _t.end.z + "," +
                    _t.end.rx + "," +
                    _t.end.ry + "," +
                    _t.end.rz + ",";
            table_position.insert("x_pos,y_pos,z_pos,x_rotation,y_rotaion,z_rotaion").values(end_pos_values).execute();

            // Get position ID
            Row posMoveRow = table_position.select("position_ID").orderBy("created_at_position", "DESC").limit(2).execute().fetchAll();
            // Insert into move table
            Table table_move = schema.getTable("move");

            std::string t_values =
                    "'" + std::string(logMoveRow[0]) + "'," +
                    getRobotMoveTypeAsString(_t.moveType) + ",'" +
                    _t.start + "','" +
                    _t.end + "'," ;
            table_move.insert("log_ID,moveType,start_positionID,end_positionID").values(t_values);
        } else if (std::is_same_v<T, qDatabaseThrowEntry>) {
            table_log.insert("descriptor").values("throw").execute();

            // Insert into position
            std::string position_values =
                    _t.releasePoint.x + "," +
                    _t.releasePoint.y + "," +
                    _t.releasePoint.z + "," +
                    _t.releasePoint.rx + "," +
                    _t.releasePoint.ry + "," +
                    _t.releasePoint.rz + "," ;
            table_position.insert("x_pos,y_pos,z_pos,x_rotation,y_rotaion,z_rotaion").values(position_values);
            Row logThrowRow = table_log.select("log_ID").orderBy("created_at","desc").execute().fetchOne();
            Row posThrowRow = table_position.select("position_ID").orderBy("created_at_position", "DESC").execute().fetchOne();

            // Insert into throw table
            Table table_throw = schema.getTable("throw");
            std::string t_values =
                    "'" + std::string(logThrowRow[0]) + "','" +
                    std::string(posThrowRow[0]) + "'," +
                    _t.successful + "," +
                    _t.deviation + "," +
                    _t.releasePoint + "," +
                    _t.releaseVelocityCalced + "," +
                    _t.releaseVelocityActual + ",";
           table_throw.insert("log_ID,position_ID,successful,deviation,tcp_velocity_cal,tcp_velocity_act").values(t_values).execute();
        } else if  (std::is_same_v<T, qDatabaseGripperEntry>) {
            table_log.insert("descriptor").values("gripper").execute();
            Row tempNewRow = table_log.select("log_ID").orderBy("created_at", "desc").execute().fetchOne();
            Table table_gripper = schema.getTable("gripper");
            std::string t_values =
                    "'" + std::string(tempNewRow[0]) + "','" +
                    _t.start + "','" +
                    _t.end + "'," +
                    _t.successful;
            table_gripper.insert("log_ID, start_width, end_width, successful").values(t_values).execute();
        } else {
            logerr("Cannot push this type of data to the database!");
            disconnect();
            return false;
        }
        disconnect();
        return true;
    }

private:
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
