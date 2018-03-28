# ragnarok
[![Build Status](https://travis-ci.org/white-wolf-project/ragnarok.svg?branch=master)](https://travis-ci.org/white-wolf-project/ragnarok)
[![Platform](https://img.shields.io/badge/platform-multiples-yellowgreen.svg)](https://github.com/white-wolf-project/ragnarok)

### Dependencies

- [Python 3.5](https://www.python.org/downloads/release/python-350/)
- [wireless-tools](https://github.com/white-wolf-project/wireless-tools)
- [sysnet](https://github.com/matteyeux/sysnet)
- [client-srv](https://github.com/white-wolf-project/client-srv)
- [libiw-dev](https://packages.debian.org/fr/sid/libiw-dev)
- [libxml2-dev](https://packages.debian.org/fr/wheezy/libxml2-dev)

### Build

The easiest way is to run `scripts/set_env.sh`. This script will install dependencies to build client and server.
Then it compiles `ragnarok` and  `ragnarok-srv`.

### Installation

#### Automatic

To install the project on your Linux machine run `scripts/install.sh`. For the moment we only support Raspberry Pi but it may work on other boards/computers.

#### Manual

##### Dependencies
Install tools and dependencies like this :
```
$ sudo apt-get install -y make build-essential libssl-dev libreadline-dev wget git python3 apache2 m4 autoconf libtool autotools-dev libiw-dev libxml2-dev vim zsh htop python3.5-dev
```

Grab Git submodules : `git submodule update --init`

Install libcpuid needed by sysnet :
```
$ git clone https://github.com/anrieff/libcpuid.git
$ cd libcpuid
$ libtoolize
$ aclocal -I m4
$ autoreconf --install
$ ./configure
$ sudo make install
```

Install HP Wireless-Tools :

Clone : `git clone https://github.com/white-wolf-project/wireless-tools.git` <br>
Build and install `sudo make -C wireless-tools/wireless_tools install`


### Client

ragnarok is the client which is used to grab all Wifi access points informations such as ESSID, Signal Quality, Encryption, etc...

```
Usage : ragnarok [OPTIONS]
 -i, --ip                        specify server IP
 -p, --port                      specify server port
 -f, --interface <interface>     specify ethernet interface
 -w, --wireless <interface>      specify wireless interface to scan with
 -x, --xml <xmlfile>             XML file to parse
 -s, --stop                      stop server
 -v, --version                   print version
 -h, --help                      print this help
 ```

Here are some informations from a scan :

```
AP  08
MAC : 40:18:B1:CB:D6:97
channel : 11
frequency : 2.462
Quality : 33/70
Signal : -77 dBm
Encryption key:  on
ESSID: "IPSA"
last beacon: 868 ms
IEEE 802.11i/WPA2 Version 1
```

Everything's written to files named `client.log` and `client.xml`. The first one is get a trace of the scan and the second one will be sent to ragnarok-srv.

As it is running as a deamon, we recommend killing your ragnarok instances with the special argument `-s` or `--stop` like this :

```
$ sudo ./ragnarok -s
[i] ragnarok stopped
```


### Server

Server listens on the port specified by the user. Default port is set in config file. It is written to get data from ragnarok clients then it splits data in an XML file (server.xml) and a server log (server.log) for debugging purposes.

```
 Usage : ragnarok-srv [OPTIONS]
 -p, --port                      specify port to bind
 -f, --interface                 specify interface to grab info
 -x, --xml <xmlfile>             XML file to parse
 -s, --stop                      stop daemon
 -v, --version                   print version
 -h, --help                      print this help
```

Once all data is in XML file, the said file is parsed and sent into an SQL database.

### Configuration
There is a config file for the client (ragnarok) which is an XML file named `client.xml` for ragnarok and `server.xml` for ragnarok-srv. <br>
If you are in debug mode you can find it in `config` directory. Else file is located in /etc/ragnarok/client/client.xml

You can also specify a new config file with `-x` argument. <br>
You config file has to look like this :

```
<client_param>
	<ip>127.0.0.1</ip>
	<port>12345</port>
	<iface>eth0</iface>
</client_param>
```

Another option is to manually set IP address of the server, the port and/or wired interface. But by default we first check for config file.
