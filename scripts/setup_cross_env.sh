#!/bin/bash
# script to setup cross compilation environment
# it is used before building debs 
# for boards such as Raspberry Pi 


WD=~/tmp_cross_install #Working Directory

function check_dir(){
	if [[ ! -d "/usr/arm-linux-gnueabihf" && ! -d  "aarch64-linux-gnu" ]]; then
		sudo apt-get update
		sudo apt-get install gcc-arm-linux-gnueabihf -qq -y
		sudo apt-get install gcc-aarch64-linux-gnu gcc-arm-linux-gnueabihf -qq -y
	fi
	
	if [[ ! -d  "/usr/aarch64-linux-gnu" ]]; then
		sudo apt-get update; sudo apt-get install gcc-aarch64-linux-gnu  -qq -y
	fi

	if [[ ! -d "/usr/arm-linux-gnueabihf" ]]; then
		sudo apt-get update; sudo apt-get install gcc-arm-linux-gnueabihf -qq -y
	fi

}

function armhf_setup(){
	# TODO : for loop
	# download armhf deb for raspberry pi : libiw-dev and libxml2-dev
	deb_links=("http://ftp.us.debian.org/debian/pool/main/w/wireless-tools/libiw-dev_30~pre9-12+b1_armhf.deb"
			   "http://security.debian.org/debian-security/pool/updates/main/libx/libxml2/libxml2_2.9.4+dfsg1-2.2+deb9u2_armhf.deb"
			   "http://security.debian.org/debian-security/pool/updates/main/libx/libxml2/libxml2-dev_2.9.4+dfsg1-2.2+deb9u2_armhf.deb"
	)
	deb_files=("libiw-dev_30~pre9-12+b1_armhf.deb" "libxml2_2.9.4+dfsg1-2.2+deb9u2_armhf.deb"
			   "libxml2-dev_2.9.4+dfsg1-2.2+deb9u2_armhf.deb"
	)

	# download debs
	for i in "${deb_links[@]}"
	do 
		wget $i
	done

	# extract things from deb
	for i in "${deb_files[@]}"
	do 
		dpkg -x $i $(ls $i | cut -f 1 -d_)"_armhf"
	done 

	# install cross compiler stuff to cross compile directory for arm-gnu | libiw
	sudo cp libiw-dev_armhf/usr/include/*.h /usr/arm-linux-gnueabihf/include
	sudo cp libiw-dev_armhf/usr/lib/arm-linux-gnueabihf/libiw.a /usr/arm-linux-gnueabihf/lib/
	sudo ln -s /usr/arm-linux-gnueabihf/lib/libiw.a /usr/arm-linux-gnueabihf/lib/libiw.so
	#sudo make -C wireless-tools/wireless_tools CC=arm-linux-gnueabihf-gcc install INSTALL_LIB=/usr/arm-linux-gnueabihf/lib/ INSTALL_DIR=/usr/arm-linux-gnueabihf/bin INSTALL_INC=/usr/arm-linux-gnueabihf/include/

	# libxml2
	sudo cp libxml2_armhf/usr/lib/arm-linux-gnueabihf/libxml2.so.2.9.4 /usr/arm-linux-gnueabihf/lib
	sudo ln -s /usr/arm-linux-gnueabihf/lib/libxml2.so.2.9.4 /usr/arm-linux-gnueabihf/lib/libxml2.so.2

	# libxml2-dev
	sudo cp -r libxml2-dev_armhf/usr/include/libxml2 /usr/arm-linux-gnueabihf/include/
	sudo cp -r libxml2-dev_armhf/usr/lib/arm-linux-gnueabihf/* /usr/arm-linux-gnueabihf/lib/
	sudo cp libxml2-dev_armhf/usr/bin/xml2-config /usr/arm-linux-gnueabihf/bin
}

function aarch64_setup(){
	wget http://ftp.us.debian.org/debian/pool/main/w/wireless-tools/libiw-dev_30~pre9-12+b1_arm64.deb
	dpkg -x libiw-dev_30\~pre9-12+b1_arm64.deb libiw-aarch64

	sudo cp libiw-aarch64/usr/include/*.h /usr/aarch64-linux-gnu/include
	sudo cp libiw-aarch64/usr/lib/aarch64-linux-gnu/libiw.a /usr/aarch64-linux-gnu/lib/
	sudo ln -s /usr/aarch64-linux-gnu/lib/libiw.a /usr/aarch64-linux-gnu/lib/libiw.so
}

rm -rf $WD
mkdir -p $WD
cd $WD
check_dir

#git clone https://github.com/white-wolf-project/wireless-tools	
armhf_setup
# aarch64_setup