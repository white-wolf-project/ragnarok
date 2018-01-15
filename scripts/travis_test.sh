#!/bin/bash
# install few tools
sudo apt-get install -y make build-essential libssl-dev wget git python3  m4 autoconf libtool autotools-dev libiw-dev libxml2-dev
# clone dependencie 
git clone https://github.com/HewlettPackard/wireless-tools.git
# build wireless-tools
sudo make -C wireless-tools/wireless_tools install
# links libs
sudo ldconfig
# build ragnarok
make
