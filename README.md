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
./setup.py --create-ini
```

Creating the systemd service file
=================================

```bash
sudo ./setup.py --create-service

sudo systemctl enable wombat-iot
sudo systemctl restart wombat-iot
sudo systemctl status wombat-iot
```
