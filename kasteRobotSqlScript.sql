CREATE DATABASE kasteRobot;
USE kasteRobot;
CREATE TABLE IF NOT EXISTS position (
positionID INT AUTO_INCREMENT PRIMARY KEY,
x_pos DOUBLE,
y_pos DOUBLE,
z_pos DOUBLE,
x_rotation DOUBLE,
y_rotation DOUBLE,
z_rotation DOUBLE
);
CREATE TABLE IF NOT EXISTS log (
created_at VARCHAR(30) PRIMARY KEY,
logType VARCHAR(20),
moveType VARCHAR(50),
start_positionID INT,
stop_positionID INT,
FOREIGN KEY (start_positionID) REFERENCES kasteRobot.position(positionID),
FOREIGN KEY (stop_positionID) REFERENCES kasteRobot.position(positionID)
);
CREATE TABLE IF NOT EXISTS throw (
throwID INT AUTO_INCREMENT PRIMARY KEY,
created_at VARCHAR(30),
FOREIGN KEY (created_at) REFERENCES kasteRobot.log(created_at),
successful BOOLEAN,
deviation DOUBLE,
xy_plane_angle DOUBLE,
cz_plane_angle DOUBLE,
release_velocity DOUBLE,
tcp_velocity_cal DOUBLE,
tcp_velocity_act DOUBLE
);
