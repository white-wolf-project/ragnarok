#!/usr/bin/env bash
#######################################################################
#
#  Project......: travis_test.sh
#  Creator......: matteyeux
#  Description..: script to run tests on Travis-CI
#  Type.........: Public
#
######################################################################
# Language :
#               bash
# Version : 0.2
#

# install few tools
sudo apt-get install -y make build-essential libssl-dev wget git python3 python3-dev m4 autoconf libtool autotools-dev libiw-dev libxml2-dev
# clone dependencie 
git clone https://github.com/HewlettPackard/wireless-tools.git
# build wireless-tools
sudo make -C wireless-tools/wireless_tools install

# Install libcpuid
git clone https://github.com/matteyeux/libcpuid
cd libcpuid
./autogen.sh
make
sudo make install
cd ..
# links libs
sudo ldconfig
# build ragnarok
make CFLAGS="-DDEBUG -I. -I/usr/include/libxml2/ -I/usr/include/python3.4 -I/usr/include/mysql -Isysnet -c -Wall -g -DDEVELOPMENT -DTRAVIS" -n
make CFLAGS="-DDEBUG -I. -I/usr/include/libxml2/ -I/usr/include/python3.4 -I/usr/include/mysql -Isysnet -c -Wall -g -DDEVELOPMENT -DTRAVIS"