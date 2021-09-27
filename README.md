<div style="text-align: center;">
	<img src="https://raw.githubusercontent.com/odier-io/wombat-iot/master/wombat-iot.jpg" alt="Wombat-IOT" width="350" />
</div>

Wombat-IOT
==========

High availability and low footprint IOT microframework with MQTT and Websocket over TLS/SSL support.

Installing Wombat-IOT
=====================

Two flavors:
  * Lua †
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

† Lua is extended with the [`json.lua`](https://github.com/rxi/json.lua) JavaScript Objet Notation (JSON) parser.

![json.lua](https://cloud.githubusercontent.com/assets/3920290/9281532/99e5e0cc-42bd-11e5-8fce-eaff2f7fc681.png)

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

See the available options there: [`src/wombat-iot.c`](https://github.com/odier-io/wombat-iot/blob/master/src/wombat-iot.c#L135-L149) and [`include/wombat-default.h`](https://github.com/odier-io/wombat-iot/blob/master/include/wombat-default.h#L25-L41).

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

def iot_mqtt_subscribe(topic: str, qos: int = 0) -> None:
def iot_mqtt_unsubscribe(topic: str, qos: int: int = 0) -> None:

def iot_mqtt_send(topic: str, payload: str, qos: int = 0, retained: bool = False) -> None:

#############################################################################
```

API (Lua version)
=================

```lua
-----------------------------------------------------------------------------
-- Service                                                                 --
-----------------------------------------------------------------------------

function iot_service_stop() -> nil
function iot_service_restart() -> nil

-----------------------------------------------------------------------------
-- Thing                                                                   --
-----------------------------------------------------------------------------

function iot_get_uid() -> string
function iot_get_descr() -> string

-----------------------------------------------------------------------------
-- Config                                                                  --
-----------------------------------------------------------------------------

function iot_config_get_str(key: string, val: string = '') -> string
function iot_config_get_int(key: string, val: number = 0x0) -> number
function iot_config_get_flt(key: string, val: number = 0.0) -> number

-----------------------------------------------------------------------------
-- Log                                                                     --
-----------------------------------------------------------------------------

function iot_log_debug(message: string) -> nil
function iot_log_ooops(message: string) -> nil
function iot_log_error(message: string) -> nil
function iot_log_fatal(message: string) -> nil

-----------------------------------------------------------------------------
-- MQTT                                                                    --
-----------------------------------------------------------------------------

function iot_mqtt_subscribe(topic: string, qos: number = 0) -> nil
function iot_mqtt_unsubscribe(topic: string, qos: number = 0) -> nil

function iot_mqtt_send(topic: string, payload: string, qos: number = 0, retained: boolean = false) -> nil

-----------------------------------------------------------------------------
```
