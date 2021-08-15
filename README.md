Wombat-IOT
==========

High availability and low footprint IOT microframework with MQTT and Websocket over TLS/SSL support.

Installing Wombat-IOT
=====================

Dependencies: `python3-dev`, ` openssl-dev`.

```bash
mkdir wombat-iot
cd wombat-iot

curl https://raw.githubusercontent.com/odier-io/wombat-iot/master/tools/wombat-iot.py > wombat-iot.py
chmod a+x wombat-iot.py

./wombat-iot.py --setup
```

Creating the main Python file
=============================

```bash
./wombat-iot.py --create-py
```

Creating the configuration file
===============================

```bash
./wombat-iot.py --create-ini
```

Creating the systemd service file
=================================

```bash
sudo ./wombat-iot.py --create-service

sudo systemctl enable wombat-iot
sudo systemctl restart wombat-iot
sudo systemctl status wombat-iot
```
