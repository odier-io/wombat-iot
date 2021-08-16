<div style="text-align: center;">
	<img src="https://raw.githubusercontent.com/odier-io/wombat-iot/master/wombat-iot.jpg" alt="Wombat-IOT" width="300" />
</div>

Wombat-IOT
==========

High availability and low footprint IOT microframework with MQTT and Websocket over TLS/SSL support.

Installing Wombat-IOT
=====================

Dependencies: `python3-dev`, ` openssl-dev`.

```bash
mkdir wombat-iot
cd wombat-iot

curl https://raw.githubusercontent.com/odier-io/wombat-iot/master/tools/setup.py > setup.py
chmod a+x setup.py

./setup.py --setup
```

Creating the main Python file
=============================

```bash
./setup.py --create-py
```

Creating the configuration file
===============================

```bash
./setup.py --create-ini --url=<URL> --username=<USERNAME> --password=<PASSWORD>
```

Creating the systemd service file
=================================

```bash
sudo ./setup.py --create-service

sudo systemctl enable wombat-iot
sudo systemctl restart wombat-iot
sudo systemctl status wombat-iot
```

API
===

```python
#############################################################################
# Service                                                                   #
#############################################################################

def iot_service_stop()
def iot_service_restart()

#############################################################################
# Thing                                                                     #
#############################################################################

def iot_get_uid()
def iot_get_descr()

#############################################################################
# Config                                                                    #
#############################################################################

def iot_config_get_str(key, val = '')
def iot_config_get_int(key, val = 0x0)
def iot_config_get_flt(key, val = 0.0)

#############################################################################
# Log                                                                       #
#############################################################################

def iot_log_debug(message)
def iot_log_ooops(message)
def iot_log_error(message)
def iot_log_fatal(message)

#############################################################################
# MQTT                                                                      #
#############################################################################

def iot_mqtt_subscribe(topic, qos)
def iot_mqtt_unsubscribe(topic)

def iot_mqtt_send(topic, payload, qos, retained, success_callback = None, failure_callback = None)

#############################################################################
```
