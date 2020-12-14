#!/usr/bin/bash
# Script for installing dependencies for running KasteRobot
# Written by Søren Pedersen

################################################################################
# !!! Be careful adding stuff, this is only run as root and we can destroy stuff
################################################################################

# There is very little error handling at the moment (probably never), run at your
# own risk! Except for one rm -R, it's only downloads and installs.
# mysql-apt-config and pylon version updates will break it ..

# Only 20.04.1 / 18.04.5 LTS tested/supported for now

ppa_list_sdu="sdurobotics/ur-rtde
sdurobotics/robwork"

repo_list=""

# Otherlibs also getting installed:
# mysql-apt-config, pylon, 

libs_for_install_pip="hpp2plantuml"

libs_for_install_snap_dev="gitkraken,classic"

libs_for_install_snap_run=""

libs_for_install_apt_dev="qt5-default
qtcreator
valgrind
kcachegrind
git"

libs_for_install_apt_run="build-essential
cmake
libwxbase3.0-dev
libwxgtk3.0-gtk3-dev
wx-common
wx3.0-i18n
plantuml
curl
libboost-all-dev
libclang-common-8-dev
libglu1-mesa-dev
libopencv-dev
libopengl-dev
libssl-dev
openjdk-8-jre
openssh-client
python3-pip
librtde-dev
libsdurw-all-dev
libsdurws-all-dev
libsdurwhw-all-dev
libsdurwsim-all-dev
mysql-community-client-plugins
libmysqlcppconn7
libmysqlcppconn7-dbgsym
libmysqlcppconn8-2
libmysqlcppconn8-2-dbgsym
libmysqlcppconn-dev
mysql-community-client
mysql-community-server"


#################################
# This is where the script starts
#################################

echo "This script sets up some dependencies for the KasteRobot project." 
echo "Do you want to run development setup or basic runtime setup? ['0' for basic, '1' for development]"
read AS_DEV

# check input
if [ $AS_DEV != 0 ] && [ $AS_DEV != 1 ]
  then
    echo "Wrong option, aborting .."
    exit
fi

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
It will also get the current update / upgrade through apt.
If you're the least bit in doubt, quit and read the script!"

# Confirmation from user .. 
echo "Are you sure you want to run this script? ['y' for yes]"
read ans
if [ "$ans" != y ]
  then echo "Aborting .."
  exit
fi

# Setting up a temporary dir for getting packages using wget
TEMP_DIR="$PWD/temp"
echo "Creating a temporary directory for install files ( $TEMP_DIR ) .."
mkdir $TEMP_DIR


##################################
# DO STUFF HERE
# need tree early if it isn't installed
if [ "$(dpkg -s tree | grep -c installed)" == 0 ]
  then
    echo "Installing tree through apt .."
    apt install tree -y
  else
    echo "Apt package tree already installed .."
fi
# setup the sources from ppa and repo list
for source in $ppa_list_sdu
do
  if [ "$(tree /etc/apt/ | grep -c ${source//\//-ubuntu-})" == 0 ]
    then
      add-apt-repository ppa:${source} -y
      echo "Added ppa:${source} .."
    else
      echo "Found $source in apt source list .."
  fi
done
# We also have to get the mysql apt repo with wget
if [ "$(dpkg -s mysql-apt-config | grep -c installed)" == 0 ]
  then
    echo "Adding MySQL apt configuration .. "
    wget https://dev.mysql.com/get/mysql-apt-config_0.8.16-1_all.deb -P $TEMP_DIR
    dpkg -i $TEMP_DIR/mysql-apt-config_0.8.16-1_all.deb
  else
    echo "MySQL apt already configured .. "
fi
apt update
apt-get upgrade --with-new-pkgs -y

# install packages
if [ "$AS_DEV" == 1 ]
  then
    for package in $libs_for_install_apt_dev
      do
        if [ "$(dpkg -s $package | grep -c installed)" == 0 ]
          then
            echo "Installing ${package} through apt .."
            apt install $package -y
          else
            echo "Apt package $package already installed .."
        fi
    done
fi
for package in $libs_for_install_apt_run
do
  if [ "$(dpkg -s $package | grep -c installed)" == 0 ] 
    then
      echo "Installing ${package} through apt .."
      apt install $package -y
    else
      echo "Apt package $package already installed .."
  fi
done
if [ "$AS_DEV" == 1 ]
  then
    for package in $libs_for_install_snap_dev
      do
        package_tag=""
        if [[ $package = *,* ]]
          then
            package_tag=" --${package//*,/}"
            package="${package//,*/}"
        fi
        if [ "$(tree /snap | grep -c $package)" == 0 ]
          then
            echo "Installing ${package}${package_tag} through snap .."
            snap install ${package}${package_tag}
          else
            echo "Snap package $package already installed .."
        fi
    done
fi
for package in $libs_for_install_snap_run
do
  package_tag=""
  if [[ $package = *,* ]]
    then
      package_tag=" --${package//*,/}"
      package="${package//,*/}"
  fi
  if [ "$(tree /snap | grep -c $package)" == 0 ]
    then
      echo "Installing ${package}${package_tag} through snap .."
      snap install ${package}${package_tag}
    else
      echo "Snap package $package already installed .."
  fi
done
for package in $libs_for_install_pip
do
  if [ "$(pip3 list | grep -c $package)" == 0 ]
    then
      echo "Installing $package through pip3 .."
      pip3 install $package
    else
      echo "Pip3 package $package already installed .."
  fi
done

# wget pylon to temp folder and install
if [ "$(dpkg -s pylon | grep -c installed)" == 0 ]
  then
    echo "Installing pylon 6.1.1.19861 .."
    wget https://www.baslerweb.com/fp-1589378344/media/downloads/software/pylon_software/pylon_6.1.1.19861-deb0_amd64.deb -P $TEMP_DIR
    dpkg -i $TEMP_DIR/pylon_6.1.1.19861-deb0_amd64.deb
  else
    echo "Pylon already installed .."
fi

# wget ursim to temp folder and setup java default
if [ "$(tree /etc | grep -c runsvdir-ursim)" == 0 ]
  then
    echo "Getting and installing UR-sim 3.14.1031212 .."
    wget https://s3-eu-west-1.amazonaws.com/ur-support-site/89458/URSim_Linux-3.14.2.1031212.tar.gz -P $TEMP_DIR
    java_bin_path="$(find /usr/bin -name "*java*")"
    java_jre8_path="$(find /usr/lib/jvm -name "java-8-openjdk*")"
    sudo update-alternatives --install $java_bin_path java $java-8-openjdk 1
    echo "Unpacking the UR-sim installation package .."
    tar -xvzf $TEMP_DIR/URSim_Linux-3.14.2.1031212.tar.gz
    ursim_unpacked_path="$(find $TEMP_DIR -type d -name "ursim-3.14*")"
    # getting the default user? 
    install_user="$(cat /etc/passwd | grep "x:1000")"
    install_user="${install_user//:*/}"
    # change to the base user
    echo "Installing UR-Sim for user $install_user .."
    chmod 755 $TEMP_DIR
    sudo -su $install_user
    cd $TEMP_DIR
    ./install.sh
    sudo -i
  else
    echo "UR-sim found to be already installed in /etc/runit/..."
fi

apt-get autoremove -y

# Cleaning up after any downloads
echo "Cleaning temporary directory .."
rm -R $TEMP_DIR

echo "Script done."
exit