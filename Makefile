CC = gcc
CLIENT = ragnarok
SERVER = ragnarok-srv
uname_s = $(shell uname -s)
arch = $(shell arch)
INSTALL_DIR ?= /usr/local/bin/

# print python3 version
python3v = $(shell python3 --version | cut -d . -f 2 | tr -d '\n')
python3m = $(shell python3 --version | cut -d . -f 2 | tr -d '\n ' && echo "m")

CLIENT_SRC = src/client
SRV_SRC = src/server

CLIENT_SRV = client-srv
SYSNET = sysnet

# I assume client and srv have the same version
VERSION = $(shell cat resources/control.srv| grep Version | cut -d:  -f 2)
DEBUG ?= 1
BUILD ?= DEVELOPMENT
CFLAGS = $(DBG) -I. -I/usr/include/libxml2/ -I/usr/include/python3.$(python3v) -I$(CLIENT_SRV) -I$(SYSNET) -c -Wall -g
LDFLAGS = -liw -lxml2 -lpython3.$(python3m)
CROSS_COMPILE ?=

CLIENT_OBJECTS = $(CLIENT_SRC)/main.o \
                 $(CLIENT_SRC)/client.o \
                 $(CLIENT_SRC)/iwlist.o

CLIENT_SOURCES = $(CLIENT_OBJECTS:.o=.c)

SRV_OBJECTS = $(SRV_SRC)/main.o \
              $(SRV_SRC)/server.o

SRV_SOURCES = $(SRV_OBJECTS:o=.c)	

OBJECTS = src/common.o

SOURCES = $(OBJECTS:o=.c)

CLI_SRV_OBJ = client-srv/src/client_tool.o \
			client-srv/src/server_tool.o \
			client-srv/src/xml.o

SYSNET_OBJ = sysnet/src/network.o

ifeq ($(DEBUG), 1)
	DBG = -DDEBUG
endif

ifeq ($(DEBUG), 2)
	DBG = -DDEBUG_ADV
endif

# Useful to locate config files
ifeq ($(BUILD), RELEASE)
	CFLAGS += -DRELEASE
endif

ifeq ($(BUILD), DEVELOPMENT)
	CFLAGS += -DDEVELOPMENT
endif

.PHONY : all clean $(CLIENT_SRV) $(SYSNET) package mrproper srv_package client_package

all : $(SERVER) $(CLIENT)

$(CLIENT) : $(CLIENT_SRV) $(OBJECTS) $(CLIENT_OBJECTS) $(SYSNET)
	@echo "LD	$@"
	@$(CC) $(CLIENT_OBJECTS) $(OBJECTS) $(CLI_SRV_OBJ) sysnet/src/network.o $(LDFLAGS) -o $(CLIENT)


$(SERVER) : $(CLIENT_SRV) $(SYSNET) $(OBJECTS) $(SRV_OBJECTS)
	@echo "LD 	$@"
	@$(CC) $(SRV_OBJECTS) $(OBJECTS) $(CLI_SRV_OBJ) sysnet/src/network.o $(LDFLAGS) -o $(SERVER)

$(CLIENT_SRV) :
	@make -C $(CLIENT_SRV)/

$(SYSNET) :
	@make -C $(SYSNET)

$(CLIENT_SRC)/%.o : $(CLIENT_SRC)/%.c
	@echo "CC	$<"
	@$(CC) $<  $(CFLAGS) -o $@

$(SRV_SRC)/%.o : $(SRV_SRC)/%.c
	@echo "CC 	$<"
	@$(CC) $< $(CFLAGS) -o $@

src/%.o : src/%.c
	@echo "CC 	$<"
	@$(CC) $< $(CFLAGS) -o $@

# build debs to install on the boxes
package : srv_package client_package

srv_package : $(SERVER)
	@echo "packing..."
	mkdir -p release
	mkdir -p srv_deb/etc/ragnarok/ srv_deb/usr/local/bin/ srv_deb/DEBIAN
	cp resources/control.srv srv_deb/DEBIAN/control
	cp $(SERVER) srv_deb/usr/local/bin/
	cp config/server.xml srv_deb/etc/ragnarok
	dpkg-deb --build srv_deb release/$(SERVER)_$(VERSION)_$(arch).deb

client_package : $(CLIENT)
	mkdir -p release
	mkdir -p client_deb/etc/ragnarok/ client_deb/usr/local/bin/ client_deb/DEBIAN
	cp resources/control.client client_deb/DEBIAN/control
	cp $(CLIENT) client_deb/usr/local/bin/
	cp config/client.xml client_deb/etc/ragnarok/
	dpkg-deb --build client_deb release/$(CLIENT)_$(VERSION)_$(arch).deb

# clean files
clean :
	make -C client-srv clean
	make -C sysnet clean
	rm -rf 	$(CLIENT) $(CLIENT_OBJECTS) \
	$(SERVER) $(SRV_OBJECTS) $(OBJECTS)

# clean more files
mrproper : clean
	rm -rf libcpuid srv_deb client_deb release wireless-tools
