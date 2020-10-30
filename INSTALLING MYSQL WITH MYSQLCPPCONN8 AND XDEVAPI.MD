INSTALLING MYSQL WITH MYSQLCPPCONN8 AND XDEVAPI

	UNINSTALL ALL MYSQL PACKAGES FIRST!
	Using apt-get (the -get part matters here):
		sudo apt-get purge libmysqlcppconn*
		sudo apt-get purge mysql-*

	This will also hit unintentional stuff and remove OpenCV
	Just reinstall using
		sudo apt-get install libopencv-dev

	Go to 
		https://dev.mysql.com/downloads/repo/apt/
	Download file
		mysql-apt-config_0.8.15-1_all.deb
	Go to
		https://dev.mysql.com/downloads/connector/cpp/
	Select "Ubuntu Linux" and the correct (18.04 64bit) version
	Download ALL 5 files!
	In terminal: (The order is VERY IMPORTANT!)
		cd Downloads
		sudo dpkg -i mysql-apt-config_0.8.15-1_all.deb
		sudo apt install mysql-community-client-plugins
		sudo dpkg -i libmysqlcppconn7_8.0.22-1ubuntu18.04_amd64.deb
		sudo dpkg -i libmysqlcppconn7-dbgsym_8.0.22-1ubuntu18.04_amd64.deb
		sudo dpkg -i libmysqlcppconn8-2_8.0.22-1ubuntu18.04_amd64.deb
		sudo dpkg -i libmysqlcppconn8-2-dbgsym_8.0.22-1ubuntu18.04_amd64.deb
		sudo dpkg -i libmysqlcppconn-dev_8.0.22-1ubuntu18.04_amd64.deb
	The MySql C++ Connector should be installed now.
	For server/client/workbench, run:
		sudo apt-get install mysql-*community*
	After install, run
		sudo mysql_

	For compilation, add mysqlcppconn8, ssl, pthread, resolv and crypto in includes. 
	Header should be in "mysql-cppconn-8/mysqlx/xdevapi.h"

	To set up the server for connections, in terminal:
		sudo mysql_sercure_installation
	Don't use password validation
	Set a non-blank root password
	After this, in terminal:
		sudo mysql
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