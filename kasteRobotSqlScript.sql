CREATE DATABASE kasteRobot;
USE kasteRobot;
CREATE TABLE IF NOT EXISTS position (
position_ID CHAR(36) PRIMARY KEY NOT NULL,
x_pos DOUBLE,
y_pos DOUBLE,
z_pos DOUBLE,
x_rotation DOUBLE,
y_rotation DOUBLE,
z_rotation DOUBLE
);
CREATE TABLE IF NOT EXISTS log (
log_ID CHAR(36) PRIMARY KEY,
created_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
entryType VARCHAR(50)
);
CREATE TABLE IF NOT EXISTS moveEntry(
moveEntry_ID CHAR(36) PRIMARY KEY,
log_ID CHAR(36),
movetype VARCHAR(30),
a_positionID CHAR(36),
b_positionID CHAR(36),
FOREIGN KEY (log_ID) REFERENCES kasteRobot.log(log_ID),
FOREIGN KEY (a_positionID) REFERENCES kasteRobot.position(position_ID),
FOREIGN KEY (b_positionID) REFERENCES kasteRobot.position(position_ID)
);
CREATE TABLE IF NOT EXISTS throw (
throw_ID CHAR(36) PRIMARY KEY,
log_ID CHAR(36),
position_ID CHAR(36),
FOREIGN KEY (position_ID) REFERENCES kasteRobot.position(position_ID),
FOREIGN KEY (log_ID) REFERENCES kasteRobot.log(log_ID),
successful BOOLEAN,
deviation DOUBLE,
xy_plane_angle DOUBLE,
cz_plane_angle DOUBLE,
tcp_velocity_cal DOUBLE,
tcp_velocity_act DOUBLE
);
# Trigger, UUID INSERT log. 
DELIMITER ;;
CREATE TRIGGER log_before_insert
BEFORE INSERT ON log FOR EACH ROW 
BEGIN
	IF new.log_ID IS NULL THEN
    SET new.log_ID = uuid();
END IF;
END;;
DELIMITER ;

# Trigger, UUID INSERT throw. 
DELIMITER ;;
CREATE TRIGGER throw_before_insert
BEFORE INSERT ON throw FOR EACH ROW 
BEGIN
	IF new.throw_ID IS NULL THEN
    SET new.throw_ID = uuid();
END IF;
END;;
DELIMITER ;

# Trigger, UUID INSERT moveEntry. 
DELIMITER ;;
CREATE TRIGGER moveEntry_before_insert
BEFORE INSERT ON moveEntry FOR EACH ROW 
BEGIN
	IF new.moveEntry_ID IS NULL THEN
    SET new.moveEntry_ID = uuid();
END IF;
END;;
DELIMITER ;

# Trigger, UUID INSERT position. 
DELIMITER ;;
CREATE TRIGGER positionID_before_insert
BEFORE INSERT ON position FOR EACH ROW 
BEGIN
	IF new.position_ID IS NULL THEN
    SET new.position_ID = uuid();
END IF;
END;;
DELIMITER ;

# Created a view for newest entries
CREATE VIEW newest_entry AS
SELECT created_at, entryType, moveType, moveEntry.log_ID, a_positionID AS A_POS, b_positionID AS B_POS
FROM moveEntry
INNER JOIN log
ON log.log_ID = moveEntry.log_ID
WHERE created_at in(SELECT MAX(created_at) FROM log);

