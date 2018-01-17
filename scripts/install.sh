#!/bin/bash
# script to install everything on the box

if [[ $(whoami) != "root" ]];then
	echo "[i] user : $(whoami)"
	echo "[-] run this script as root"
	exit 1
fi

guess_user= pwd | cut -f 3 -d/ 	# will be used to chown downloaded files
#$guess_user
# write to interfaces file to start wlan0
echo "allow-hotplug wlan0" >> /etc/network/interfaces
echo "auto wlan0" >> /etc/network/interfaces
