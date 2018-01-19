CC = gcc
CLIENT = ragnarok
SERVER = ragnarok-srv
uname_s = $(shell uname -s)
arch = $(shell arch)
INSTALL_DIR ?= /usr/local/bin/

CLIENT_SRC = src/client
SRV_SRC = src/server

CLIENT_SRV = client-srv
VERSION = $(shell cat resources/control| grep Version | cut -d:  -f 2)
DEBUG ?=
ADV ?=
CFLAGS = $(DBG) $(ADV) -I. -I/usr/include/libxml2/ -I$(CLIENT_SRV) -c -Wall -g -o
LDFLAGS = -liw -lxml2
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

EXTRA_OBJ = client-srv/src/client_tool.o \
			client-srv/src/server_tool.o

ifeq ($(DEBUG), 1)
	DBG = -DDEBUG
endif

.PHONY : all clean $(CLIENT_SRV)

all : $(CLIENT) $(SERVER)

$(CLIENT) : $(CLIENT_SRV) $(OBJECTS) $(CLIENT_OBJECTS)
	@echo "LD	$@"
	@$(CC) $(CLIENT_OBJECTS) $(OBJECTS) $(EXTRA_OBJ) $(LDFLAGS) -o $(CLIENT)

$(SERVER) : $(CLIENT_SRV) $(OBJECTS) $(SRV_OBJECTS)
	@echo "LD 	$@"
	@$(CC) $(SRV_OBJECTS) $(OBJECTS) $(EXTRA_OBJ) $(LDFLAGS) -o $(SERVER)

$(CLIENT_SRV) :
	make -C $(CLIENT_SRV)/

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
	rm -rf 	$(CLIENT) $(CLIENT_OBJECTS) \
	$(SERVER) $(SRV_OBJECTS) $(OBJECTS)
