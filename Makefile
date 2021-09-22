SHELL := /bin/bash

########################################################################################################################

CC ?= gcc
STRIP ?= strip

AR ?= ar
RANLIB ?= ranlib

PYTHON_CFLAGS ?= $(shell python3-config --cflags)
PYTHON_LDFLAGS ?= $(shell python3-config --ldflags)

LUA_CFLAGS ?= 

LUA_LDFLAGS ?= -llua

########################################################################################################################

ifeq ($(shell uname -s),Darwin)
	PYTHON_LDFLAGS+=$(shell python3 -c 'import sys, sysconfig; sys.stdout.write("%s" % sysconfig.get_config_var("LIBDIR"))')/libpython*.dylib
endif

########################################################################################################################

CFLAGS_PY=-std=c99 -fPIC -O3 $(PYTHON_CFLAGS) -Wall -Wextra -Wno-unused-result -Wno-unused-parameter -I include -Dinline=__inline__ -DMQTTAsync_setConnectedCallback=MQTTAsync_setConnected

LDFLAGS_WITH_SSL_PY=-L lib -L /opt/local/lib $(PYTHON_LDFLAGS) -lssl -lcrypto -pthread
LDFLAGS_WITHOUT_SSL_PY=-L lib $(PYTHON_LDFLAGS) -pthread

########################################################################################################################

CFLAGS_LUA=-std=c99 -fPIC -O3 $(LUA_CFLAGS) -Wall -Wextra -Wno-unused-result -Wno-unused-parameter -I include -Dinline=__inline__ -DMQTTAsync_setConnectedCallback=MQTTAsync_setConnected

LDFLAGS_WITH_SSL_LUA=-L lib -L /opt/local/lib $(LUA_LDFLAGS) -lssl -lcrypto -pthread
LDFLAGS_WITHOUT_SSL_LUA=-L lib $(LUA_LDFLAGS) -pthread

########################################################################################################################

all:
	@echo 'make wombat-iot-py-with-ssl'
	@echo 'make wombat-iot-py-without-ssl'
	@echo 'make wombat-iot-lua-with-ssl'
	@echo 'make wombat-iot-lua-without-ssl'
	@echo 'make deps-with-ssl'
	@echo 'make deps-without-ssl'

########################################################################################################################

wombat-iot-py-with-ssl: wombat-iot-py.a

	####################################################################################################################
	# WOMBAT-IOT WITH SSL                                                                                              #
	####################################################################################################################

	$(CC) -o bin/wombat-iot src/wombat-iot.c -lwombat-iot-py -lpaho-mqtt3as -DIS_PY $(LDFLAGS_WITH_SSL_PY)

	$(STRIP) bin/wombat-iot

########################################################################################################################

wombat-iot-py-without-ssl: wombat-iot-py.a

	####################################################################################################################
	# WOMBAT-IOT WITHOUT SSL                                                                                           #
	####################################################################################################################

	$(CC) -o bin/wombat-iot src/wombat-iot.c -lwombat-iot-py -lpaho-mqtt3a -DIS_PY $(LDFLAGS_WITHOUT_SSL_PY)

	$(STRIP) bin/wombat-iot

########################################################################################################################

wombat-iot-lua-with-ssl: wombat-iot-lua.a

	####################################################################################################################
	# WOMBAT-IOT WITH SSL                                                                                              #
	####################################################################################################################

	$(CC) -o bin/wombat-iot src/wombat-iot.c -lwombat-iot-lua -lpaho-mqtt3as -DIS_LUA $(LDFLAGS_WITH_SSL_LUA)

	$(STRIP) bin/wombat-iot

########################################################################################################################

wombat-iot-lua-without-ssl: wombat-iot-lua.a

	####################################################################################################################
	# WOMBAT-IOT WITHOUT SSL                                                                                           #
	####################################################################################################################

	$(CC) -o bin/wombat-iot src/wombat-iot.c -lwombat-iot-lua -lpaho-mqtt3a -DIS_LUA $(LDFLAGS_WITHOUT_SSL_LUA)

	$(STRIP) bin/wombat-iot

########################################################################################################################

wombat-iot-py.a:

	mkdir -p bin
	mkdir -p lib

	####################################################################################################################
	# LIBWOMBAT-IOT-PYTHON.A WITHOUT SSL                                                                               #
	####################################################################################################################

	$(CC) $(CFLAGS_PY) -c -o src/str.o src/str.c
	$(CC) $(CFLAGS_PY) -c -o src/log.o src/log.c
	$(CC) $(CFLAGS_PY) -c -o src/config.o src/config.c
	$(CC) $(CFLAGS_PY) -c -o src/mqtt.o src/mqtt.c
	$(CC) $(CFLAGS_PY) -c -o src/iot_python.o src/iot_python.c

	####################################################################################################################

	$(AR) rcs lib/libwombat-iot-py.a src/str.o src/log.o src/config.o src/mqtt.o src/iot_python.o && $(RANLIB) lib/libwombat-iot-py.a

########################################################################################################################

.PHONY: wombat-iot-python.a

########################################################################################################################

wombat-iot-lua.a:

	mkdir -p bin
	mkdir -p lib

	####################################################################################################################
	# LIBWOMBAT-IOT-LUA.A WITHOUT SSL                                                                                  #
	####################################################################################################################

	$(CC) $(CFLAGS_LUA) -c -o src/str.o src/str.c
	$(CC) $(CFLAGS_LUA) -c -o src/log.o src/log.c
	$(CC) $(CFLAGS_LUA) -c -o src/config.o src/config.c
	$(CC) $(CFLAGS_LUA) -c -o src/mqtt.o src/mqtt.c
	$(CC) $(CFLAGS_LUA) -c -o src/iot_lua.o src/iot_lua.c

	####################################################################################################################

	$(AR) rcs lib/libwombat-iot-lua.a src/str.o src/log.o src/config.o src/mqtt.o src/iot_lua.o && $(RANLIB) lib/libwombat-iot-lua.a

########################################################################################################################

.PHONY: wombat-iot-lua.a

########################################################################################################################

deps-with-ssl:
	WITH_SSL=1 ./scripts/make_dependencies.sh

########################################################################################################################

deps-without-ssl:
	WITH_SSL=0 ./scripts/make_dependencies.sh

########################################################################################################################

clean:
	rm -fr src/*.o lib/libwombat-iot-py.a lib/libwombat-iot-lua.a bin/wombat-iot

########################################################################################################################
