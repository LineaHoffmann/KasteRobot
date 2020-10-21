INSTALLING WXWIDGETS
	Get wxWidgets 3.0.5 Stable from https://www.wxwidgets.org/downloads/
	Choose "Source for Linux, MacOS, etc"
	Extract to ~/wxWidgets-3.0.5
	In a terminal run
		cd wxWidgets-3.0.5
		mkdir gtkbuild
		cd gtkbuild
		../configure --enable-unicode --with-gtk=3 --enable-debug
		make (evt med -jN, hvor N er antal cores, det tager ret lang tid med kun 1)
		sudo make install
		make clean
		sudo ldconfig
	Check install with 
		wx-config --version

INSTALLING MYSQL
	In terminal, run:
		openssl version -a
	If no installation: https://www.howtoforge.com/tutorial/how-to-install-openssl-from-source-on-linux/
	Download mysql apt repo from https://dev.mysql.com/downloads/repo/apt/
		sudo dpkg -i ~/Downloads/mysql-apt-config_0.8.15-1_all.deb
		sudo apt-get update
		sudo apt-get install mysql-common mysql-community-server* mysql-community-client* mysql-workbench-community*
	Check installation with
		systemctl status mysql.service
	Install some libs with
		sudo apt-get install libmysql++-dev libmysqlclient-dev libmysqlcppconn8-*
	Check your installation with
		mysql -u root -p

	If this fails, reset the sql server password to the root password
		sudo nano /etc/mysql/mysql.conf.d/mysqld.cnf
	If this shows an empty text file, your configuration file is located elsewhere, find it with
		sudo find /etc -name "mysqld.cnf"
	Under the [mysqld] bracket, on a new line add
		skip-grant-tables
	In terminal, run 
		service mysql restart
		mysql -u root -p
	This will enter the sql database, where you should run
		flush privileges
	To clean after the skip-grant-tables intervention

INSTALLING UR_RTDE
	Run this in terminal
		sudo add-apt-repository ppa:sdurobotics/ur-rtde
		sudo apt-get update
		sudo apt-get install librtde librtde-dev libboost-all-dev

INSTALLING ROBWORKS
	Run in terminal
		sudo add-apt-repository ppa:sdurobotics/robwork
		sudo apt-get update
		sudo apt-get install libsdurw-all-dev libsdurws-all-dev libsdurwhw-all-dev libsdurwsim-all-dev

INSTALLING OPENCV AND PYLON
	In terminal, run
		sudo apt-get install libopencv-dev
 	download deb (Linux x64 .deb) package from:
		https://www.baslerweb.com/en/sales-support/downloads/software-downloads/
	Install using EITHER the package manager interface or the following
		sudo dpkg -i ~/Downloads/pylon_X.X.X.XXXXX-deb0_amd64.deb
	Where the X's are the version you downloaded

