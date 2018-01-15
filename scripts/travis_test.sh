#!/bin/bash

sudo apt-get install -y make build-essential libssl-dev wget git python3  m4 autoconf libtool autotools-dev libiw-dev libxml2-dev

 git clone https://github.com/white-wolf-project/wireless-tools.git
 ls
 cd wireless-tools/wireless-tools && ls && sudo make install
 sudo ldconfig

 make
