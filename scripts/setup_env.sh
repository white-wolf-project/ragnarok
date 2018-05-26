#!/usr/bin/env bash
#######################################################################
#
#  Project......: setup_env.sh
#  Creator......: matteyeux
#  Description..: setup dev env for Ragnarok
#  Type.........: Public
#
######################################################################
# Language :
#               bash
# Version : 0.1
#

# function to install libcpuid for sysnet
function install_libcpuid(){
	if [[ $(arch) == "x86_64" || $(arch) == "i686" || $(arch) == "i386" ]]; then
		git clone https://github.com/anrieff/libcpuid.git
		cd libcpuid
		libtoolize
		aclocal -I m4
		autoreconf --install
		ls .. && ls .
		./configure
		make 
		sudo make install && cd ..
	else 
		echo "skipping libcpuid"
	fi
}


# update repos before installing
sudo apt-get update && sudo apt-get upgrade -y

# install build tools 
sudo apt-get install -y make build-essential libssl-dev mysql-server mysql-client \
libmysql++-dev wget git python3-dev python3-pip apache2 m4 autoconf libtool autotools-dev \
libiw-dev libxml2-dev php7.0-dev php7.0-fpm

if [[ `arch | cut -c1-3` != "arm" && `arch | cut -c1-3` != "aar" ]];then
	sudo apt-get install -y gcc-arm-linux-gnueabihf
fi

# install python3 mysql
pip3 install mysql-connector-python
# init and clone git modules
git submodule update --init

# install sysnet
install_libcpuid
sudo make -C sysnet install

# wireless-tools are needed for iwlist.c in our project
git clone https://github.com/white-wolf-project/wireless-tools.git
sudo make -C wireless-tools/wireless_tools install

sudo ldconfig

sudo a2enmod proxy_fcgi setenvif
sudo a2enconf php7.0-fpm
sudo service apache2 restart

