CC = gcc
CLIENT = ragnarok
SERVER = ragnarok-srv
uname_s = $(shell uname -s)
arch = $(shell arch)
INSTALL_DIR ?= /usr/local/bin/

CLIENT_SRC = src/client
SRV_SRC = src/server

VERSION = $(shell cat resources/control| grep Version | cut -d:  -f 2)
DEBUG ?=
ADV ?=
CFLAGS = $(DBG) $(ADV) -I. -c -Wall -g -o
LDFLAGS = -liw
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

ifeq ($(DEBUG), 1)
	DBG = -DDEBUG
endif

ifeq ($(ADV), 1)
	DBG = -DADVANCED # advanced outputs not sure
endif

.PHONY : all clean

all : $(CLIENT) $(SERVER)


$(CLIENT) : $(OBJECTS) $(CLIENT_OBJECTS)
	@echo "LD	$@"
	@$(CC) $(CLIENT_OBJECTS) $(OBJECTS) $(LDFLAGS) -o $(CLIENT)

$(SERVER) : $(OBJECTS) $(SRV_OBJECTS)
	@echo "LD 	$@"
	@$(CC) $(SRV_OBJECTS) $(LDFLAGS) -o $(SERVER)

$(CLIENT_SRC)/%.o : $(CLIENT_SRC)/%.c
	@echo "CC	$<"
	$(CC) $<  $(CFLAGS) $@

$(SRV_SRC)/%.o : $(SRV_SRC)/%.c
	@echo "CC 	$<"
	@$(CC) $< $(CFLAGS) $@

src/%.o : src/%.c
	@echo "CC 	$<"
	@$(CC) $< $(CFLAGS) $@

clean :
	rm -rf 	$(CLIENT) $(CLIENT_OBJECTS) \
	$(SERVER) $(SRV_OBJECTS) $(OBJECTS)
