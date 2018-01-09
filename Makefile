CC = gcc
CLIENT = client
SERVER = server
uname_s = $(shell uname -s)
arch = $(shell arch)
INSTALL_DIR ?= /usr/local/bin/

CLIENT_SRC = src/client
SRV_SRC = src/server

VERSION = $(shell cat resources/control| grep Version | cut -d:  -f 2)
DEBUG ?=
DEBUGYESNO ?=
DBG ?=
CFLAGS = -I. -c -Wall
LDFLAGS ?= 
CROSS_COMPILE ?=

CLIENT_OBJECTS = 	$(CLIENT_SRC)/main.o \
					$(CLIENT_SRC)/client_tool.o

CLIENT_SOURCES = $(CLIENT_OBJECTS:.o=.c)

SRV_OBJECTS = 		$(SRV_SRC)/main.o \
					$(SRV_SRC)/server_tool.o

SRV_SOURCES = $(SRV_OBJECTS:o=.c)	

.PHONY : all clean

all : $(CLIENT) $(SERVER)


$(CLIENT) : $(CLIENT_OBJECTS)
	@echo "LD	$@"
	@$(CC) $(CLIENT_OBJECTS) $(LDFLAGS) -o $(CLIENT)

$(SERVER) : $(SRV_OBJECTS)
	@echo "LD 	$@"
	@$(CC) $(SRV_OBJECTS) $(LDFLAGS) -o $(SERVER)

$(CLIENT_SRC)/%.o : $(CLIENT_SRC)/%.c
	@echo "CC	$<"
	@$(CC) $(CFLAGS) $< -o $@


$(SRV_SRC)/%.o : $(SRV_SRC)/%.c
	@echo "CC 	$<"
	@$(CC) $(CFLAGS) $< -o $@

clean :
	rm -rf 	$(CLIENT) $(CLIENT_OBJECTS) \
	$(SERVER) $(SRV_OBJECTS)
