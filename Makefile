qSHELL:=/bin/bash

########################################################################################################################

PYTHON_LIBDIR:=$(shell python3 -c 'import sys, sysconfig; sys.stdout.write("%s" % sysconfig.get_config_var("LIBDIR"))')

########################################################################################################################

CC ?= gcc
AR ?= ar
RANLIB ?= ranlib
PYTHONCFLAGS ?= $(shell python3-config --cflags)
PYTHONLDFLAGS ?= $(shell python3-config --ldflags)

########################################################################################################################

CFLAGS=-std=c99 -fPIC -O3 $(PYTHONCFLAGS) -Wall -Wextra -Wno-unused-result -Wno-unused-parameter -I include -Dinline=__inline__ -DMQTTAsync_setConnectedCallback=MQTTAsync_setConnected

LDFLAGS_WITH_SSL= -L lib -L /opt/local/lib $(PYTHONLDFLAGS)
LDFLAGS_WITHOUT_SSL= -L lib -L /opt/local/lib $(PYTHONLDFLAGS)

ifeq ($(shell uname -s),Darwin)
	LDFLAGS_WITH_SSL+=$(PYTHON_LIBDIR)/libpython*.dylib -lssl -lcrypto -pthread
	LDFLAGS_WITHOUT_SSL+=$(PYTHON_LIBDIR)/libpython*.dylib -pthread
else
	LDFLAGS_WITH_SSL+=-lssl -lcrypto -pthread
	LDFLAGS_WITHOUT_SSL+=-pthread
endif

########################################################################################################################

all:
	@echo 'make wombat-iot-with-ssl'
	@echo 'make wombat-iot-without-ssl'
	@echo 'make deps-with-ssl'
	@echo 'make deps-without-ssl'

########################################################################################################################

wombat-iot-with-ssl: wombat-iot.a

	####################################################################################################################
	# WOMBAT-IOT WITH SSL                                                                                              #
	####################################################################################################################

	$(CC) -o bin/wombat-iot src/wombat-iot.c -lwombat-iot -lpaho-mqtt3as $(LDFLAGS_WITH_SSL)

########################################################################################################################

wombat-iot-without-ssl: wombat-iot.a

	####################################################################################################################
	# WOMBAT-IOT WITHOUT SSL                                                                                           #
	####################################################################################################################

	$(CC) -o bin/wombat-iot src/wombat-iot.c -lwombat-iot -lpaho-mqtt3a $(LDFLAGS_WITHOUT_SSL)

########################################################################################################################

wombat-iot.a:

	mkdir -p bin
	mkdir -p lib

	####################################################################################################################
	# LIBWOMBAT-IOT.A WITHOUT SSL                                                                                      #
	####################################################################################################################

	$(CC) $(CFLAGS) -c -o src/str.o src/str.c
	$(CC) $(CFLAGS) -c -o src/log.o src/log.c
	$(CC) $(CFLAGS) -c -o src/config.o src/config.c
	$(CC) $(CFLAGS) -c -o src/mqtt.o src/mqtt.c
	$(CC) $(CFLAGS) -c -o src/iot.o src/iot.c

	####################################################################################################################

	$(AR) rcs lib/libwombat-iot.a src/str.o src/log.o src/config.o src/mqtt.o src/iot.o && $(RANLIB) lib/libwombat-iot.a

########################################################################################################################

.PHONY: wombat-iot.a

########################################################################################################################

deps-with-ssl:
	WITH_SSL=1 ./scripts/make_dependencies.sh

########################################################################################################################

deps-without-ssl:
	WITH_SSL=0 ./scripts/make_dependencies.sh

########################################################################################################################

clean:
	rm -fr src/*.o lib/libwombat-iot.a bin/wombat-iot

########################################################################################################################
