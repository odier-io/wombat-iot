SHELL:=/bin/bash

########################################################################################################################

PYTHON_LIBDIR:=$(shell python3 -c 'import sys, sysconfig; sys.stdout.write("%s" % sysconfig.get_config_var("LIBDIR"))')

PYTHON_INCDIR:=$(shell python3 -c 'import sys, sysconfig; sys.stdout.write("%s" % sysconfig.get_path("include"))')

########################################################################################################################

IOT_UID:=$(shell python3 -c 'import sys, uuid; sys.stdout.write("%s" % uuid.uuid4())')

IOT_DESCR:=N/A

########################################################################################################################

CC=gcc
AR=ar
RANLIB=ranlib
CFLAGS=-std=c89 -fPIC -O3 -Wall -Wextra -Wno-comment -Wno-unused-parameter -I include -I $(PYTHON_INCDIR) -Dinline=__inline__ -DMQTTAsync_setConnectedCallback=MQTTAsync_setConnected -DIOT_UID=$(IOT_UID) -DIOT_DESCR=$(IOT_DESCR)
LDFLAGS=-L lib -L $(PYTHON_LIBDIR) $(PYTHON_LIBDIR)/libpython*

########################################################################################################################

all:
	mkdir -p bin
	mkdir -p lib

	####################################################################################################################
	# LIBWOMBAT-IOT.A                                                                                                  #
	####################################################################################################################

	$(CC) $(CFLAGS) -c -o src/str.o src/str.c
	$(CC) $(CFLAGS) -c -o src/log.o src/log.c
	$(CC) $(CFLAGS) -c -o src/config.o src/config.c
	$(CC) $(CFLAGS) -c -o src/mqtt.o src/mqtt.c
	$(CC) $(CFLAGS) -c -o src/iot.o src/iot.c

	####################################################################################################################

	$(AR) rcs lib/libwombat-iot.a src/str.o src/log.o src/config.o src/mqtt.o src/iot.o && $(RANLIB) lib/libwombat-iot.a

	####################################################################################################################
	# WOMBAT-IOT                                                                                                       #
	####################################################################################################################

	$(CC) -o bin/wombat-iot src/wombat-iot.c -lwombat-iot -lpaho-mqtt3as $(LDFLAGS) -lssl -lcrypto -lpthread

########################################################################################################################

deps:
	./scripts/make_dependencies.sh

########################################################################################################################

clean:
	rm -fr src/*.o lib/libwombat-iot.a bin/wombat-iot

########################################################################################################################

.PHONY: plugins

########################################################################################################################
