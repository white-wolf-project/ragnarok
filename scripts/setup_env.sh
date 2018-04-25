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
sudo apt-get install -y make build-essential libssl-dev libreadline-dev mysql-server mysql-client \
libmysqlclient-dev libmysql++-dev wget git python3 python3-pip \
libnl-3-dev apache2 nmap m4 autoconf libtool autotools-dev libiw-dev libxml2-dev vim zsh htop python3.5-dev

if [[ $(arch) != "armv71" && $(arch) != "aarch64" ]];then
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
