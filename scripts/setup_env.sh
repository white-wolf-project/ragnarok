#!/usr/bin/env bash
#######################################################################
#
#  Project......: bootstrap.sh
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
sudo apt-get update && apt-get upgrade -y

# install build tools 
sudo apt-get install -y make build-essential libssl-dev libreadline-dev libsqlite3-dev wget git python3 libnl-3-dev apache2 nmap m4 autoconf libtool autotools-dev libiw-dev libxml2-dev gcc-arm-linux-gnueabihf vim zsh htop

# install git depends
git clone https://github.com/matteyeux/sysnet; cd sysnet
./scripts/install.sh
sudo make install
cd ..

git clone https://github.com/white-wolf-project/client-srv
make -C client-srv

# wireless-tools are needed for iwlist.c in our project
git clone https://github.com/white-wolf-project/wireless-tools.git
make -C wireless-tools/wireless_tools install

# few vim settings
sudo echo "set nu" >> /etc/vim/vimrc 			# set line numbers
sudo echo "set autoindent" >> /etc/vim/vimrc	# auto indentation
sudo echo "set tabstop=4" >> /etc/vim/vimrc		# tab = 4 spaces (but tabs)
sudo echo "set showmatch" >> /etc/vim/vimrc		# parentheses/brackets highlight when matching
