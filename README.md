<div style="text-align: center;">
	<img src="https://raw.githubusercontent.com/odier-io/wombat-iot/master/wombat-iot.jpg" alt="Wombat-IOT" width="350" />
</div>

Wombat-IOT
==========

High availability and low footprint IOT microframework with MQTT and Websocket over TLS/SSL support.

Installing Wombat-IOT
=====================

Two flavors:
  * Lua
  * Python

Dependencies: `cmake`, `python3-dev`, ` openssl-dev` (optional).

```bash
mkdir wombat-iot
cd wombat-iot

curl https://raw.githubusercontent.com/odier-io/wombat-iot/master/tools/setup.py > setup.py
chmod a+x setup.py

./setup.py --setup-py-with-ssl
# or
./setup.py --setup-py-without-ssl
# or
./setup.py --setup-lua-with-ssl
# or
./setup.py --setup-lua-without-ssl
```

Environment variables for cross-compiling: `CC`, `STRIP`, `AR`, `RANLIB`, `PYTHON_CFLAGS`, `PYTHON_LDFLAGS`, `LUA_CFLAGS`, `LUA_LDFLAGS`.

Creating the main main file
===========================

```bash
./setup.py --create-py
# or
./setup.py --create-lua
```

Creating the configuration file
===============================

```bash
./setup.py --create-ini --mqtt-url=<URL> --mqtt-username=<USERNAME> --mqtt-password=<PASSWORD>
```

See the available options there: `include/wombat-default.h`.

Creating the systemd service file
=================================

```bash
sudo ./setup.py --create-service-py
# or
sudo ./setup.py --create-service-lua

sudo systemctl enable wombat-iot
sudo systemctl restart wombat-iot
sudo systemctl status wombat-iot
```

Callbacks (Python version)
==========================

```python
#############################################################################
# Thing                                                                     #
#############################################################################

def iot_init_success(message: str) -> None:

def iot_init_failure(message: str) -> None:

#############################################################################

def iot_loop(connected: bool) -> None:

#############################################################################
# MQTT                                                                      #
#############################################################################

def iot_connection_opened(message: str) -> None:

def iot_connection_lost(message: str) -> None:

#############################################################################

def iot_message(topic: str, payload: str) -> bool†:

def iot_delivery(token: str) -> None:

#############################################################################
```

† This function must return `True` or `False` indicating whether or not the message has been safely received by the client application.

Callbacks (Lua version)
=======================

```lua
-----------------------------------------------------------------------------
- Thing                                                                     -
-----------------------------------------------------------------------------

function iot_init_success(message)

end

function iot_init_failure(message)

end

-----------------------------------------------------------------------------

function iot_loop(connected)

end

-----------------------------------------------------------------------------
- MQTT                                                                      -
-----------------------------------------------------------------------------

function iot_connection_opened(message)

end

function iot_connection_lost(message)

end

-----------------------------------------------------------------------------

function iot_message(topic, payload) †

end

function iot_delivery(token)

end

-----------------------------------------------------------------------------
```

† This function must return `true` or `false` indicating whether or not the message has been safely received by the client application.

API (Python version)
====================

```python
#############################################################################
# Service                                                                   #
#############################################################################

def iot_service_stop() -> None:
def iot_service_restart() -> None:

#############################################################################
# Thing                                                                     #
#############################################################################

def iot_get_uid() -> str:
def iot_get_descr() -> str:

#############################################################################
# Config                                                                    #
#############################################################################

def iot_config_get_str(key: str, val = '') -> str:
def iot_config_get_int(key: str, val = 0x0) -> int:
def iot_config_get_flt(key: str, val = 0.0) -> float:

#############################################################################
# Log                                                                       #
#############################################################################

def iot_log_debug(message: str) -> None:
def iot_log_ooops(message: str) -> None:
def iot_log_error(message: str) -> None:
def iot_log_fatal(message: str) -> None:

#############################################################################
# MQTT (these functions can raise IOError)                                  #
#############################################################################

def iot_mqtt_subscribe(topic: str, qos = 0) -> None:
def iot_mqtt_unsubscribe(topic: str, qos = 0) -> None:

def iot_mqtt_send(topic: str, payload: str, qos = 0, retained = False) -> None:

#############################################################################
```
