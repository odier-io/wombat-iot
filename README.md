<div style="text-align: center;">
	<img src="https://raw.githubusercontent.com/odier-io/wombat-iot/master/wombat-iot.jpg" alt="Wombat-IOT" width="350" />
</div>

Wombat-IOT
==========

High availability and low footprint IOT microframework with MQTT and Websocket over TLS/SSL support.

Installing Wombat-IOT
=====================

Dependencies: `cmake`, `python3-dev`, ` openssl-dev` (optional).

```bash
mkdir wombat-iot
cd wombat-iot

curl https://raw.githubusercontent.com/odier-io/wombat-iot/master/tools/setup.py > setup.py
chmod a+x setup.py

./setup.py --setup-with-ssl
# or
./setup.py --setup-without-ssl
```

Creating the main Python file
=============================

```bash
./setup.py --create-py
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
sudo ./setup.py --create-service

sudo systemctl enable wombat-iot
sudo systemctl restart wombat-iot
sudo systemctl status wombat-iot
```

Callbacks
=========

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

def iot_message(topic: str, payload: str) -> bool†:

def iot_delivery(token: str) -> None:

#############################################################################

def iot_connection_lost(message: str) -> None:

def iot_connection_opened(message: str) -> None:

#############################################################################
```

† This function must return `True` or `False` indicating whether or not the message has been safely received by the client application.

API
===

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

def iot_mqtt_send(topic: str, payload: str, qos = 0, retained = False, success_callback = None, failure_callback = None) -> None:

#############################################################################
```
