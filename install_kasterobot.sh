#!/bin/bash
# Script for installing dependencies for running KasteRobot
# Written by Søren Pedersen

# Tested for install on 20.04.1 Xubuntu fresh install virtual machine, running in
# KVM on Xubuntu 20.04.1 16-11-2020

################################################################################
# !!! Be careful adding stuff, this is only run as root and we can destroy stuff
################################################################################

# There is very little error handling at the moment (probably never), run at your
# own risk! Except for one rm -R, it's only downloads and installs.
# mysql-apt-config and pylon version updates will break it ..

# Only 20.04.1 / 18.04.5 LTS tested/supported for now

# Naming some global variables here for use in functions
DISTRO=""
AS_DEV=0

install_mysql() {
  # MySQL
  # Common for both install types
  MYSQL_CPPCONN_APT_OK="$(dpkg -s mysql-apt-config | grep -c "Status: install ok installed")"
  if [ "$MYSQL_CPPCONN_APT_OK" -eq 0 ]
    then 
      echo "MySQL Apt Config not installed, getting version 8.16 .."
      wget https://dev.mysql.com/get/mysql-apt-config_0.8.16-1_all.deb -P $TEMP_DIR
      # It's annoying, but user has to select some stuff .. 
      dpkg -i $TEMP_DIR/mysql-apt-config_0.8.16-1_all.deb
      apt update
      MYSQL_CPPCONN_APT_OK="$(dpkg -s mysql-apt-config | grep -c "Status: install ok installed")"
      if [ "$MYSQL_CPPCONN_APT_OK" -eq 0 ]
        then 
          echo "Failed to install MySQL Apt Config. Aborting .. "
          exit
      fi
    else
      echo "MySQL Apt Config found!"
  fi
  MYSQL_CLIENT_PLUGINS_OK="$(dpkg -s mysql-community-client-plugins | grep -c "Status: install ok installed")"
  if [ "$MYSQL_CLIENT_PLUGINS_OK" -eq 0 ]
    then 
      echo "MySQL Community Client Plugins not installed, getting from MySQL Apt .."
      apt-get install mysql-community-client-plugins -y
    else
      echo "MySQL Client plugins found!"
  fi
  MYSQL_CPPCONN_7_OK="$(dpkg -s libmysqlcppconn7 | grep -c "Status: install ok installed")"
  if [ "$MYSQL_CPPCONN_7_OK" -eq 0 ]
    then 
      echo "MySQL C++ Connector 7 not installed, getting from MySQL Apt .."
      apt-get install libmysqlcppconn7 -y
    else
      echo "MySQL C++ Connector 7 found!"
  fi
  MYSQL_CPPCONN_8_OK="$(dpkg -s libmysqlcppconn8-2 | grep -c "Status: install ok installed")"
  if [ "$MYSQL_CPPCONN_8_OK" -eq 0 ]
    then 
      echo "MySQL C++ Connector 8 not installed, getting from MySQL Apt .."
      apt-get install libmysqlcppconn8-2 -y
    else
      echo "MySQL C++ Connector 8 found!"
  fi
  # Development specific installs
  if [ "$AS_DEV" -eq 1 ]
    then
      MYSQL_CPPCONN_DEV_OK="$(dpkg -s libmysqlcppconn-dev | grep -c "Status: install ok installed")"
      if [ "$MYSQL_CPPCONN_DEV_OK" -eq 0 ]
        then 
          apt-get install libmysqlcppconn-dev -y
      fi
      MYSQL_CPPCONN_7_DBG_OK="$(dpkg -s libmysqlcppconn7-dbgsym | grep -c "Status: install ok installed")"
      if [ "$MYSQL_CPPCONN_7_DBG_OK" -eq 0 ]
        then
          apt-get install libmysqlcppconn7-dbgsym -y
      fi
      MYSQL_CPPCONN_8_DBG_OK="$(dpkg -s libmysqlcppconn8-2-dbgsym | grep -c "Status: install ok installed")"
      if [ "$MYSQL_CPPCONN_8_DBG_OK" -eq 0 ]
        then
          apt-get install libmysqlcppconn8-2-dbgsym -y
      fi
      MYSQL_COMMUNITY_CLIENT="$(dpkg -s mysql-community-client | grep -c "Status: install ok installed")"
      if [ "$MYSQL_COMMUNITY_CLIENT" -eq 0 ]
        then
          apt-get install mysql-community-client -y
      fi
      MYSQL_COMMUNITY_SERVER="$(dpkg -s mysql-community-server | grep -c "Status: install ok installed")"
      if [ "$MYSQL_COMMUNITY_SERVER" -eq 0 ]
        then
          apt-get install mysql-community-server -y
      fi
      MYSQL_COMMUNITY_WORKBENCH="$(dpkg -s mysql-workbench-community | grep -c "Status: install ok installed")"
      if [ "$MYSQL_COMMUNITY_WORKBENCH" -eq 0 ]
        then
          apt-get install mysql-workbench-community -y
      fi
  fi
}

install_opencv() {
  # OpenCV
  OPENCV_OK="$(dpkg -s libopencv-dev | grep -c "Status: install ok installed")"
  if [ "$OPENCV_OK" -eq 0 ]
    then 
      echo "OpenCV not installed, getting from Universe .."
      apt-get install libopencv-dev -y
    else
      echo "OpenCV found!"
  fi
}

install_pylon() {
  # Pylon
  PYLON_OK="$(dpkg -s pylon | grep -c "Status: install ok installed")"
  if [ "$PYLON_OK" -eq 0 ]
    then 
      echo "Pylon not installed, getting version 6.1.1.19864 .."
      wget https://www.baslerweb.com/fp-1589378344/media/downloads/software/pylon_software/pylon_6.1.1.19861-deb0_amd64.deb -P $TEMP_DIR
      dpkg -i $TEMP_DIR/pylon_6.1.1.19861-deb0_amd64.deb
      apt update
      PYLON_OK="$(dpkg -s pylon | grep -c "Status: install ok installed")"
      if [ "$PYLON_OK" -eq 0 ]
        then 
          echo "Failed to install pylon. Aborting .. "
          exit
      fi
    else
      echo "Pylon found!"
  fi
}

install_sdurobotics() {
  # Robwork
  ROBWORK_OK="$(dpkg -s libsdurw-all-dev | grep -c "Status: install ok installed")"
  if [ "$ROBWORK_OK" -eq 0 ]
    then 
      echo "Robwork not installed, checking for repo .."
      ROBWORK_REPO_OK="$(grep ^[^#] /etc/apt/sources.list.d/* | grep -c robwork)"
      if [ "$ROBWORK_REPO_OK" -eq 0 ]
        then
          echo "Repo not found, adding Robwork repository .. "
          add-apt-repository ppa:sdurobotics/robwork -y
          apt update
        else
          echo "Repository seems to be available, trying to install .. "
      fi
      apt-get install libsdurw-all-dev -y
      ROBWORK_OK="$(dpkg -s libsdurw-all-dev | grep -c "Status: install ok installed")"
      if [ "$ROBWORK_OK" -eq 0 ]
        then 
          echo "Failed to install Robwork. Aborting .. "
          exit
      fi
    else
      echo "Robwork found!"
  fi

  # UR-RTDE
  RTDE_OK="$(dpkg -s librtde | grep -c "Status: install ok installed")"
  if [ "$RTDE_OK" -eq 0 ]
    then 
      echo "RTDE not installed, checking for repo .."
      RTDE_REPO_OK="$(grep ^[^#] /etc/apt/sources.list.d/* | grep -c rtde)"
      if [ "$RTDE_REPO_OK" -eq 0 ]
        then
          echo "Repo not found, adding UR-RTDE repository .. "
          add-apt-repository ppa:sdurobotics/ur-rtde -y
          apt update
        else
          echo "Repository seems to be available, trying to install .. "
      fi
      apt-get install librtde -y
      RTDE_OK="$(dpkg -s librtde | grep -c "Status: install ok installed")"
      if [ "$RTDE_OK" -eq 0 ]
        then 
          echo "Failed to install RTDE. Aborting .. "
          exit
      fi
    else
      echo "UR-RTDE found!"
  fi

  # Development specific installs
  if [ "$AS_DEV" -eq 1 ]
    then
      RTDE_DEV_OK="$(dpkg -s librtde-dev | grep -c "Status: install ok installed")"
      if [ "$RTDE_DEV_OK" -eq 0 ]
        then
          apt-get install librtde-dev -y
      fi

      ROBWORK_S_ALL_OK="$(dpkg -s libsdurws-all-dev | grep -c "Status: install ok installed")"
      if [ "$ROBWORK_S_ALL_OK" -eq 0 ]
        then
          apt-get install libsdurws-all-dev -y
      fi
      ROBWORK_HW_ALL_OK="$(dpkg -s libsdurwhw-all-dev | grep -c "Status: install ok installed")"
      if [ "$ROBWORK_HW_ALL_OK" -eq 0 ]
        then
          apt-get install libsdurwhw-all-dev -y
      fi
      ROBWORK_SIM_ALL_OK="$(dpkg -s libsdurwsim-all-dev | grep -c "Status: install ok installed")"
      if [ "$ROBWORK_SIM_ALL_OK" -eq 0 ]
        then
          apt-get install libsdurwsim-all-dev -y
      fi
  fi

}

install_wxwidgets() {
  # wxWidgets
  WX_BASE_OK="$(dpkg -s libwxbase3.0-dev | grep -c "Status: install ok installed")"
  if [ "$WX_BASE_OK" -eq 0 ]
    then 
      echo "wxWidgets Base not installed, getting from Universe .."
      apt-get install libwxbase3.0-dev -y
    else
      echo "wxWidgets Base found!"
  fi
  WX_MEDIA_OK="$(dpkg -s libwxgtk-media3.0-gtk3-dev | grep -c "Status: install ok installed")"
  if [ "$WX_MEDIA_OK" -eq 0 ]
    then 
      echo "wxWidgets Media not installed, getting from Universe .."
      apt-get install libwxgtk-media3.0-gtk3-dev -y
    else
      echo "wxWidgets Media found!"
  fi
  WX_GTK3_OK="$(dpkg -s libwxgtk3.0-gtk3-dev | grep -c "Status: install ok installed")"
  if [ "$WX_GTK3_OK" -eq 0 ]
    then 
      echo "wxWidgets GTK3 not installed, getting from Universe .."
      apt-get install libwxgtk3.0-gtk3-dev -y
    else
      echo "wxWidgets GTK3 found!"
  fi
}

install_misc() {
  # Check for boost, ssl, cmake, qt, ssh, clang, gitkraken, git, opengl
  if [ "$AS_DEV" -eq 1 ]
    then
      OPENGL_OK="$(dpkg -s libopengl-dev | grep -c "Status: install ok installed")"
      if [ "$OPENGL_OK" -eq 0 ]
        then
          apt-get install libopengl-dev -y
      fi
      GIT_OK="$(dpkg -s git | grep -c "Status: install ok installed")"
      if [ "$GIT_OK" -eq 0 ]
        then
          apt-get install git -y
      fi
      GITKRAKEN_OK="$(snap info gitkraken | grep -c "installed:")"
      if [ "$GITKRAKEN_OK" -eq 0 ]
        then
          snap install gitkraken --classic
      fi
      BOOST_OK="$(dpkg -s libboost-all-dev | grep -c "Status: install ok installed")"
      if [ "$BOOST_OK" -eq 0 ]
        then
          apt-get install libboost-all-dev -y
      fi
      ESSENTIAL_OK="$(dpkg -s build-essential | grep -c "Status: install ok installed")"
      if [ "$ESSENTIAL_OK" -eq 0 ]
        then
          apt-get install build-essential -y
      fi
      CMAKE_OK="$(dpkg -s cmake | grep -c "Status: install ok installed")"
      if [ "$CMAKE_OK" -eq 0 ]
        then
          apt-get install cmake -y
      fi

      QT5_OK="$(dpkg -s qt5-default | grep -c "Status: install ok installed")"
      if [ "$QT5_OK" -eq 0 ]
        then
          apt-get install qt5-default -y
      fi
      QT_CREATOR_OK="$(dpkg -s qtcreator | grep -c "Status: install ok installed")"
      if [ "$QT_CREATOR_OK" -eq 0 ]
        then
          apt-get install qtcreator -y
      fi

      SSL_DEV_OK="$(dpkg -s libssl-dev | grep -c "Status: install ok installed")"
      if [ "$SSL_DEV_OK" -eq 0 ]
        then
          apt-get install libssl-dev -y
      fi

      VALGRIND_OK="$(dpkg -s valgrind | grep -c "Status: install ok installed")"
      if [ "$VALGRIND_OK" -eq 0 ]
        then
          apt-get install valgrind -y
      fi

	  OPENSSH_CLIENT_OK="$(dpkg -s openssh-client | grep -c "Status: install ok installed")"
      if [ "$OPENSSH_CLIENT_OK" -eq 0 ]
        then
          apt-get install openssh-client -y
      fi

      # 20.04 seems to need libclang-common-8-dev for Qt5 Clang Backend, at least on Xubuntu
      if [ DISTRO == "2004" ]
        then
          CLANG_8_DEV_OK="$(dpkg -s libclang-8-common-dev | grep -c "Status: install ok installed")"
          if [ "$CLANG_8_DEV_OK" -eq 0 ]
            then
              apt-get install libclang-8-common-dev -y
          fi
      fi

  fi
}

#################################
# This is where the script starts
#################################

echo "This script sets up some dependencies for the KasteRobot project." 
echo "Do you want to run development setup (Qt5 env) or basic runtime setup? ['0' for basic, '1' for development]"
read AS_DEV

  # Abort if not run as root
  if [ "$EUID" -ne 0 ]
   then echo "Please run as root .."
   exit
  fi

  # Abort if distro isn't Ubuntu 18.04.5 or 20.04.1
  DISTRO=0
  if [ "$(lsb_release -a | grep -c "Ubuntu 20.04.1 LTS")" -eq 1 ]
    then 
      DISTRO="2004"
      echo "Distro found to be Ubuntu 20.04.1 .."
  elif [ "$(lsb_release -a | grep -c "Ubuntu 18.04.5 LTS")" -eq 1 ]
    then 
      DISTRO="1804"
      echo "Distro found to be Ubuntu 18.04.5 .."
  else
      echo "This system isn't supported. Aborting .."
      exit
  fi   

echo "This script will attempt to install a bunch of dependencies and requires root privileges, which is dangerous.
It will also get the current update / upgrade through apt."

  # Confirmation from user .. 
  echo "Are you sure you want to run this script? ['y' for yes]"
  read ans
  if [ "$ans" != y ]
    then echo "Aborting .."
    exit
  fi

gitkraken
git

apt-get update 
apt-get upgrade --with-new-pkgs -y

  # Setting up a temporary dir for getting packages using wget
  TEMP_DIR="$PWD/temp"
  echo "Creating a temporary directory for install files ( $TEMP_DIR ) .."
  mkdir $TEMP_DIR

install_misc          # Miscellaneous packages (Qt, boost, ssl, cmake)
install_mysql         # MySQL C++ Connector 
install_opencv        # OpenCV 
install_pylon         # Pylon
install_sdurobotics   # Robwork and RTDE
install_wxwidgets     # wxWidgets

apt-get autoremove -y

  # Cleaning up after any downloads
  echo "Cleaning temporary directory .."
  rm -R $TEMP_DIR

echo "Script done."
exit