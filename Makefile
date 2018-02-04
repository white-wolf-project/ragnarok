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

VERSION = $(shell cat resources/control| grep Version | cut -d:  -f 2)
DEBUG ?=
ADV ?=
CFLAGS = $(DBG) $(ADV) -I. -I/usr/include/libxml2/ -I/usr/include/python3.$(python3v) -I$(CLIENT_SRV) -I$(SYSNET) -c -Wall -g -o
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

.PHONY : all clean $(CLIENT_SRV) $(SYSNET)

all : $(SERVER) $(CLIENT)

$(CLIENT) : $(CLIENT_SRV) $(OBJECTS) $(CLIENT_OBJECTS)
	@echo "LD	$@"
	@$(CC) $(CLIENT_OBJECTS) $(OBJECTS) $(CLI_SRV_OBJ) $(LDFLAGS) -o $(CLIENT)


$(SERVER) : $(CLIENT_SRV) $(SYSNET) $(OBJECTS) $(SRV_OBJECTS)
	@echo "LD 	$@"
	@$(CC) $(SRV_OBJECTS) $(OBJECTS) $(CLI_SRV_OBJ) $(SYSNET_OBJ) $(LDFLAGS) -o $(SERVER)

$(CLIENT_SRV) :
	@make -C $(CLIENT_SRV)/

$(SYSNET) :
	@make -C $(SYSNET)

$(CLIENT_SRC)/%.o : $(CLIENT_SRC)/%.c
	@echo "CC	$<"
	@$(CC) $<  $(CFLAGS) $@

$(SRV_SRC)/%.o : $(SRV_SRC)/%.c
	@echo "CC 	$<"
	@$(CC) $< $(CFLAGS) $@

src/%.o : src/%.c
	@echo "CC 	$<"
	@$(CC) $< $(CFLAGS) $@

clean :
	make -C client-srv clean
	make -C sysnet clean
	rm -rf 	$(CLIENT) $(CLIENT_OBJECTS) \
	$(SERVER) $(SRV_OBJECTS) $(OBJECTS)
