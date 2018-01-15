#!/bin/bash

sudo apt-get install -y make build-essential libssl-dev wget git python3  m4 autoconf libtool autotools-dev libiw-dev libxml2-dev

 git clone https://github.com/HewlettPackard/wireless-tools.git
 ls
 cd wireless-tools
 ls
 cd wireless-tools
 ls
 sudo make install
 
 sudo ldconfig

 make
