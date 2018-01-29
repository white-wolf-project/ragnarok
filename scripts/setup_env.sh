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

# update repos before installing
sudo apt-get update && sudo apt-get upgrade -y

# install build tools 
sudo apt-get install -y make build-essential libssl-dev libreadline-dev libsqlite3-dev wget git python3 \
libnl-3-dev apache2 nmap m4 autoconf libtool autotools-dev libiw-dev libxml2-dev vim zsh htop python3.5-dev

if [[ $(arch) != "armv71" && $(arch) != "aarch64" ]];then
	sudo apt-get install -y gcc-arm-linux-gnueabihf
fi

# init and clone git modules
git submodule update --init

# install git depends
git clone https://github.com/matteyeux/sysnet; cd sysnet
./scripts/install.sh
sudo make install
cd ..

git clone https://github.com/white-wolf-project/client-srv
make -C client-srv

# wireless-tools are needed for iwlist.c in our project
git clone https://github.com/white-wolf-project/wireless-tools.git
sudo make -C wireless-tools/wireless_tools install

sudo ldconfig