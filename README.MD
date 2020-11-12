REVISED REINSTALL GUIDE [08/11/2020]
<<<< COMMON FOR ALL UBUNTU INSTALLS >>>>
	sudo apt-get install build-essential libboost-all-dev cmake
	sudo apt-get install git qt5-default qtcreator
	sudo apt-get install libssl-dev libopengl-dev kcachegrind valgrind
	sudo snap install gitkraken --classic
	sudo apt-get install python3-pip plantuml
	sudo pip3 install hpp2plantuml
	sudo apt-get install wx-common libwxbase3.0-dev libwxgtk-media3.0-gtk3-dev libwxgtk3.0-gtk3-dev
	sudo apt-get install libopencv-dev
	
	<<<< project_pkgs >>>>
	# Shared packages, push for deploy. OR Script. 
	# Install from basler: https://www.baslerweb.com/en/sales-support/downloads/software-downloads/pylon-6-1-1-linux-x86-64-bit-debian/
	sudo dpkg -i pylon_6.1.1.19861-deb0_amd64.deb

<<<< FOR UBUNTU 18.04 >>>>
<<<< /project_pkgs/mysqlcppconn8 >>>>
#for download of packeges: https://dev.mysql.com/downloads/connector/cpp/
#Select ubuntu linux 64 bit
	sudo dpkg -i mysql-apt-config_0.8.15-1_all.deb
	sudo apt-get install mysql-community-client-plugins		
	sudo dpkg -i libmysqlcppconn7_8.0.22-1ubuntu18.04_amd64.deb
	sudo dpkg -i libmysqlcppconn7-dbgsym_8.0.22-1ubuntu18.04_amd64.deb
	sudo dpkg -i libmysqlcppconn8-2_8.0.22-1ubuntu18.04_amd64.deb
	sudo dpkg -i libmysqlcppconn8-2-dbgsym_8.0.22-1ubuntu18.04_amd64.deb
	sudo dpkg -i libmysqlcppconn-dev_8.0.22-1ubuntu18.04_amd64.deb

<<<< FOR UBUNTU 20.04 >>>>
<<<< /project_pkgs/mysqlcppconn8 >>>>
#for download of packeges: https://dev.mysql.com/downloads/connector/cpp/
#Select ubuntu linux 64 bit
	sudo dpkg -i mysql-apt-config_0.8.15-1_all.deb
	sudo apt-get install mysql-community-client-plugins
	sudo dpkg -i libmysqlcppconn7_8.0.22-1ubuntu20.04_amd64.deb
	sudo dpkg -i libmysqlcppconn7-dbgsym_8.0.22-1ubuntu20.04_amd64.deb
	sudo dpkg -i libmysqlcppconn8-2_8.0.22-1ubuntu20.04_amd64.deb
	sudo dpkg -i libmysqlcppconn8-2-dbgsym_8.0.22-1ubuntu20.04_amd64.deb
	sudo dpkg -i libmysqlcppconn-dev_8.0.22-1ubuntu20.04_amd64.deb

<<<< COMMON FOR ALL UBUNTU INSTALLS >>>>
	sudo apt-get install mysql-community-server mysql-community-client mysql-workbench-community
	sudo add-apt-repository ppa:sdurobotics/robwork
	sudo add-apt-repository ppa:sdurobotics/ur-rtde
	sudo apt-get update
	sudo apt-get install librtde-dev libsdurw-all-dev libsdurwhw-all-dev libsdurws-all-dev libsdurwsim-all-dev

!! Project should compile in Qt Creator directly after git pull !!  
!! Only SQL Server setup and URSIM not installed at this point! !! 

	For server/client/workbench, run:
		sudo apt-get install mysql-community-server mysql-community-client mysql-workbench-community

	To set up the server for connections, in terminal:
		sudo mysql_secure_installation
	Don't use password validation
	Set a non-blank root password
	After this, in terminal:
		mysql -u root -p
			mysql> CREATE USER 'username'@'host' IDENTIFIED WITH mysql_native_password BY 'password';
	Replace the following: 
	username with the wanted login username, 
	host with localhost,
	password with actual password
			mysql> GRANT ALL PRIVILEGES on *.* TO 'username'@'localhost' WITH GRANT OPTION;
	Replace username with the previously selected username and localhost with previously selected host
			mysql> ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'password'
	Replace password with the root user password
			mysql> FLUSH PRIVILEGES;
	Ensure that it's running on the default port
			mysql> SELECT @@port;
	It should be 3306. Quit after this.
			mysql> \q
	Try logging in to the server with the new user:
		mysql -u username -p
	Try opening the MySql Workbench. Remeber to add the key for the root user if not asked. 

INSTALLING URSIM
	download package from:
		https://www.universal-robots.com/download/?option=89457#section16632
	Install java openjdk:
		sudo apt-get install openjdk-8-jre
		sudo update-alternatives --config java
			select open jdk just installed. 
	run in terminal:
		cd ~
		tar xvzf [FILE NAME]
		cd ursim-3.X.X.XXXXX
		./install.sh
	to run URSim run in terminal:
		cd ..
		sudo ursim-3.X.X.XXXXX/start-ursim.sh UR5

USING THE UML GENERATOR SCRIPT
	To run the script, enter the UML subfolder and run:
		./hpp2uml_script.sh
