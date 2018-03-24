#!/usr/bin/env bash
#######################################################################
#
#  Project......: build_wireless_check.sh
#  Creator......: matteyeux
#  Description..: build simple tool to get first wireless interface 
#  Type.........: Public
#
######################################################################
# Language :
#               bash
# Version : 0.1
#

# get python3 info
python3v=`python3 --version | cut -d . -f 2 | tr -d '\n'`
python3m=`python3 --version | cut -d . -f 2 | tr -d '\n ' && echo "m"`
LDFLAGS="-I../ -I/usr/include/libxml2/ -I/usr/include/python3.$python3v -I../client-srv -I../sysnet"
CFLAGS="-Wall -o"

# build wireless checker
gcc $CFLAGS check_wireless check_wireless.c ../sysnet/src/network.c  ../src/common.c $LDFLAGS -lxml2 -lpython3.$python3m -liw -lm