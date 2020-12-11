# Insert Queries

# For each insert query, we need to split into two calls.

# 1. Insert into log, create a entry in the log. 
INSERT INTO kasteRobot.log(descriptor) VALUES();

# Insert into position, because we need the postion_ID in other tables, unless its gripper. 

# 2. Insert position 6D point
INSERT INTO kasteRobot.position(x_pos,y_pos,z_pos,x_rotation,y_rotaion,z_rotaion) VALUES();

# 2.a Insert into 3D point
INSERT INTO kasteRobot.position(x_pos,y_pos,z_pos) VALUES ();

# 2.b Insert into 2d point
INSERT INTO kasteRobot.position(x_pos,y_pos) VALUES ();


# 3. Get the log_ID, position_ID to bind tables.
# We assume that the newest log entry is the one we want. 

# 3.A MOVE: 
	SELECT * FROM log.log_ID ORDER BY created_at DESC; # GET THE log_ID
	#EXTRACT IN C++
	SELECT * FROM position ORDER BY created_at_position DESC LIMIT 2; # returns the two newest positions, with end position first. 
	#EXTRACT IN c++

	# INSERT INTO TABLE
	# Move table, start/end_positionID are position_ID
	INSERT INTO kasteRobot.move(log_ID,moveType,start_positionID,end_positionID) VALUES();

# 3.B THROW: 
	SELECT log_ID FROM log ORDER BY created_at DESC; # GET THE log_ID
	#EXTRACT IN C++
	SELECT position_ID FROM position ORDER BY created_at_position DESC; # GET THE position_ID
	#EXTRACT IN C++

	#INSERT INTO TABLE
	INSERT INTO kasteRobot.throw(log_ID,position_ID,
		successful,deviation,tcp_velocity_cal,tcp_velocity_act) VALUES ();


# 3.C BALL
	SELECT * FROM log.log_ID ORDER BY created_at DESC;
	# EXTRACT IN C++
	SELECT * FROM position ORDER BY created_at_position DESC;
	# EXRACT IN C++

	#INSERT INTO TABLE
	INSERT INTO kasteRobot.ball(log_ID,diameter,ball_position) VALUES ();

# 3.D Gripper
	SELECT * FROM log.log_ID ORDER BY created_at DESC;
	# EXTRACT IN C++

	#INSERT INTO TABLE
	INSERT INTO kasteRobot.gripper(log_ID,gripper_ID,start_width,end_width,successful) VALUES(); 
