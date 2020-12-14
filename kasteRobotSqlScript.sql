CREATE DATABASE IF NOT EXISTS kasteRobot;
USE kasteRobot;
CREATE TABLE IF NOT EXISTS kasteRobot.position (
position_ID CHAR(36) PRIMARY KEY NOT NULL,
created_at_position DATETIME(3) DEFAULT CURRENT_TIMESTAMP(3) ON UPDATE CURRENT_TIMESTAMP(3),
x_pos DOUBLE,
y_pos DOUBLE,
z_pos DOUBLE,
x_rotation DOUBLE,
y_rotation DOUBLE,
z_rotation DOUBLE
);
CREATE TABLE IF NOT EXISTS kasteRobot.log (
log_ID CHAR(36) PRIMARY KEY,
created_at DATETIME(3) DEFAULT CURRENT_TIMESTAMP(3) ON UPDATE CURRENT_TIMESTAMP(3),
descriptor VARCHAR(50)
);
CREATE TABLE IF NOT EXISTS kasteRobot.move(
move_ID CHAR(36) PRIMARY KEY,
log_ID CHAR(36),
moveType VARCHAR(30),
start_positionID CHAR(36),
end_positionID CHAR(36),
FOREIGN KEY (log_ID) REFERENCES kasteRobot.log(log_ID),
FOREIGN KEY (start_positionID) REFERENCES kasteRobot.position(position_ID),
FOREIGN KEY (end_positionID) REFERENCES kasteRobot.position(position_ID)
);
CREATE TABLE IF NOT EXISTS kasteRobot.throw (
throw_ID CHAR(36) PRIMARY KEY,
log_ID CHAR(36),
position_ID CHAR(36),
FOREIGN KEY (position_ID) REFERENCES kasteRobot.position(position_ID),
FOREIGN KEY (log_ID) REFERENCES kasteRobot.log(log_ID),
successful BOOLEAN,
deviation DOUBLE,
tcp_velocity_cal DOUBLE,
tcp_velocity_act DOUBLE
);

CREATE TABLE IF NOT EXISTS kasteRobot.ball(
ball_ID CHAR(36) PRIMARY KEY NOT NULL,
log_ID CHAR(36),
FOREIGN KEY (log_ID) REFERENCES kasteRobot.log(log_ID),
diameter DOUBLE,
ball_position CHAR(36),
FOREIGN KEY (ball_position) REFERENCES kasteRobot.position(position_ID)
);

CREATE TABLE IF NOT EXISTS kasteRobot.gripper(
gripper_ID CHAR(36) PRIMARY KEY NOT NULL,
log_ID CHAR(36),
FOREIGN KEY(log_ID) REFERENCES log(log_ID),
start_width DOUBLE,
end_width DOUBLE,
successful BOOLEAN
);


# Trigger, UUID INSERT log. 
DELIMITER ;;
CREATE TRIGGER logID_before_insert
BEFORE INSERT ON log FOR EACH ROW 
BEGIN
	IF new.log_ID IS NULL THEN
    SET new.log_ID = uuid();
END IF;
END;;
DELIMITER ;

# Trigger, UUID INSERT throw. 
DELIMITER ;;
CREATE TRIGGER throwID_before_insert
BEFORE INSERT ON throw FOR EACH ROW 
BEGIN
	IF new.throw_ID IS NULL THEN
    SET new.throw_ID = uuid();
END IF;
END;;
DELIMITER ;

# Trigger, UUID INSERT move. 
DELIMITER ;;
CREATE TRIGGER move_before_insert
BEFORE INSERT ON move FOR EACH ROW 
BEGIN
	IF new.move_ID IS NULL THEN
    SET new.move_ID = uuid();
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

# Trigger, UUID INSERT ball. 
DELIMITER ;;
CREATE TRIGGER ballID_before_insert
BEFORE INSERT ON ball FOR EACH ROW 
BEGIN
	IF new.ball_ID IS NULL THEN
    SET new.ball_ID = uuid();
END IF;
END;;
DELIMITER ;

# Trigger, UUID INSERT gripper. 
DELIMITER ;;
CREATE TRIGGER gripperID_before_insert
BEFORE INSERT ON gripper FOR EACH ROW 
BEGIN
	IF new.gripper_ID IS NULL THEN
    SET new.gripper_ID = uuid();
END IF;
END;;
DELIMITER ;

#list all in log, and move tables. 
CREATE VIEW log_move AS
SELECT log.log_ID, created_at, descriptor, move_ID, moveType, start_positionID, end_positionID
FROM log
RIGHT JOIN move
ON log.log_ID = move.log_ID ORDER BY created_at DESC;

#list all in log, and gripper tables.
CREATE VIEW log_gripper AS
SELECT log.log_ID, created_at, descriptor, successful ,gripper_ID, start_width, end_width
FROM log
INNER JOIN gripper
ON gripper.log_ID = log.log_ID ORDER BY created_at DESC;

#list all in log, and throw tables.
CREATE VIEW log_throw AS
SELECT log.log_ID, created_at, descriptor, throw_ID,position_ID,successful,deviation,tcp_velocity_cal,tcp_velocity_act
FROM log
INNER JOIN throw
ON throw.log_ID = log.log_ID ORDER BY created_at DESC;

#list all in log, and ball tables.
CREATE VIEW log_ball AS
SELECT log.log_ID, created_at, descriptor, ball_ID, diameter, ball_position
FROM log
INNER JOIN ball
ON ball.log_ID = log.log_ID ORDER BY created_at DESC;
