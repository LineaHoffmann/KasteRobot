DROP DATABASE kasteRobot;
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
CREATE TABLE IF NOT EXISTS moveEntry(
moveEntry_ID INT AUTO_INCREMENT PRIMARY KEY,
movetype VARCHAR(30),
a_positionID INT,
b_positionID INT,
FOREIGN KEY (a_positionID) REFERENCES kasteRobot.position(positionID),
FOREIGN KEY (b_positionID) REFERENCES kasteRobot.position(positionID)
);
CREATE TABLE IF NOT EXISTS log (
log_ID INT AUTO_INCREMENT PRIMARY KEY,
created_at VARCHAR(30),
entryType VARCHAR(50)
);
CREATE TABLE IF NOT EXISTS throw (
throwID INT AUTO_INCREMENT PRIMARY KEY,
log_ID INT,
FOREIGN KEY (log_ID) REFERENCES kasteRobot.log(log_ID),
successful BOOLEAN,
deviation DOUBLE,
xy_plane_angle DOUBLE,
cz_plane_angle DOUBLE,
release_velocity DOUBLE,
tcp_velocity_cal DOUBLE,
tcp_velocity_act DOUBLE
);
