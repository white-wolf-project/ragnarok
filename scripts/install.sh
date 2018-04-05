#!/bin/bash
# script to install everything on the box

if [[ $(whoami) != "root" ]];then
	echo "[i] user : $(whoami)"
	echo "[-] run this script as root"
	exit 1
fi

guess_user= pwd | cut -f 3 -d/ 	# will be used to chown downloaded files
#$guess_user
wireless_iface=`./scripts/check_wireless`

if [[ $? != 0 ]]; then
	echo $?
	exit 1
else
	# write to interfaces file to start wlan0
	echo "allow-hotplug $wireless_iface" >> /etc/network/interfaces
	echo "auto $wireless_iface" >> /etc/network/interfaces
fi


mkdir -p /etc/ragnarok/client /etc/ragnarok/server/
cp config/client.xml /etc/ragnarok/client/
cp config/server.xml /etc/ragnarok/server/
