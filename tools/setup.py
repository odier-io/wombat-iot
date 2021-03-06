#!/usr/bin/env python3
# -*- coding:utf-8 -*-
########################################################################################################################
# Wombat-IOT
#
# Copyright (C) 2020 Jérôme Odier (jerome@odier.xyz)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
########################################################################################################################

import os, sys, stat, uuid, shutil, hashlib, argparse, subprocess

########################################################################################################################

WOMBAT_IOT_DIR = os.path.dirname(os.path.abspath(__file__))

WOMBAT_IOT_GIT_URL = 'https://github.com/odier-io/wombat-iot.git'

########################################################################################################################

def print_logo1():

    print('''
██     ██  ██████  ███    ███ ██████   █████  ████████       ██  ██████  ████████
██     ██ ██    ██ ████  ████ ██   ██ ██   ██    ██          ██ ██    ██    ██   
██  █  ██ ██    ██ ██ ████ ██ ██████  ███████    ██    █████ ██ ██    ██    ██   
██ ███ ██ ██    ██ ██  ██  ██ ██   ██ ██   ██    ██          ██ ██    ██    ██   
 ███ ███   ██████  ██      ██ ██████  ██   ██    ██          ██  ██████     ██   
                                                                                 '''[1: ])

########################################################################################################################

def print_logo2():

    print('◖ᵔᴥᵔ◗ ♪ ♫')

########################################################################################################################

def saveText(fileName, data):

    print('Saving `%s`...' % fileName)

    with open(fileName, 'wt') as f:

        f.write(data)

########################################################################################################################

def setup(verbose, mode, with_ssl):

    ####################################################################################################################

    binDir = os.path.join(WOMBAT_IOT_DIR, 'bin')
    libDir = os.path.join(WOMBAT_IOT_DIR, 'lib')
    incDir = os.path.join(WOMBAT_IOT_DIR, 'include')
    gitDir = os.path.join(WOMBAT_IOT_DIR, '.wombat-iot')

    ####################################################################################################################
    # BUILD                                                                                                            #
    ####################################################################################################################

    if os.path.isdir(gitDir):

        shutil.rmtree(gitDir)

    ####################################################################################################################

    subprocess.check_call(['git', 'clone', WOMBAT_IOT_GIT_URL, gitDir])

    if with_ssl:

        subprocess.check_call('make deps-with-ssl', cwd = gitDir, shell = True)

        if   mode == 'py':

            subprocess.check_call('make wombat-iot-py-with-ssl', cwd = gitDir, shell = True)

        elif mode == 'lua':

            subprocess.check_call('make wombat-iot-lua-with-ssl', cwd = gitDir, shell = True)

    else:

        subprocess.check_call('make deps-without-ssl', cwd = gitDir, shell = True)

        if   mode == 'py':

            subprocess.check_call('make wombat-iot-py-without-ssl', cwd = gitDir, shell = True)

        elif mode == 'lua':

            subprocess.check_call('make wombat-iot-lua-without-ssl', cwd = gitDir, shell = True)

    ####################################################################################################################
    # INSTALL                                                                                                          #
    ####################################################################################################################

    if not os.path.isdir(binDir):

        os.mkdir(binDir)

    if not os.path.isdir(libDir):

        os.mkdir(libDir)

    if not os.path.isdir(incDir):

        os.mkdir(incDir)

    ####################################################################################################################

    subprocess.check_call('cp bin/wombat-iot %s' % binDir, cwd = gitDir, shell = True)

    subprocess.check_call(   'cp lib/*.a %s'     % libDir, cwd = gitDir, shell = True)

    subprocess.check_call( 'cp include/*.h %s'   % incDir, cwd = gitDir, shell = True)

    ####################################################################################################################

    os.chmod(os.path.join(binDir, 'wombat-iot'), os.stat(os.path.join(binDir, 'wombat-iot')).st_mode | stat.S_IEXEC)

    ####################################################################################################################

    if not os.path.isfile(os.path.join(WOMBAT_IOT_DIR, '.gitignore')):

        saveText(os.path.join(WOMBAT_IOT_DIR, '.gitignore'), WOMBAT_GITIGNORE);

    ####################################################################################################################

    shutil.rmtree(gitDir)

    ####################################################################################################################

    print_logo2()

########################################################################################################################

def createPy(verbose):

    ####################################################################################################################

    fileName = os.path.join(WOMBAT_IOT_DIR, 'wombat-iot.py')

    if os.path.isfile(fileName):

        print('error: file `%s` already exists' % fileName)

        return 1

    ####################################################################################################################

    try:

        ################################################################################################################

        saveText(fileName, WOMBAT_IOT_PY)

        ################################################################################################################

        print_logo2()

        ################################################################################################################

        return 0

    except Exception as e:

        print('error: %s' % e)

        return 1

########################################################################################################################

def createLua(verbose):

    ####################################################################################################################

    fileName = os.path.join(WOMBAT_IOT_DIR, 'wombat-iot.lua')

    if os.path.isfile(fileName):

        print('error: file `%s` already exists' % fileName)

        return 1

    ####################################################################################################################

    try:

        ################################################################################################################

        saveText(fileName, WOMBAT_IOT_LUA)

        ################################################################################################################

        print_logo2()

        ################################################################################################################

        return 0

    except Exception as e:

        print('error: %s' % e)

        return 1

########################################################################################################################

def createIni(verbose, mqtt_url, mqtt_username, mqtt_password):

    ####################################################################################################################

    fileName = os.path.join(WOMBAT_IOT_DIR, 'wombat-iot.ini')

    if os.path.isfile(fileName):

        print('error: file `%s` already exists' % fileName)

        return 1

    ####################################################################################################################

    try:

        ################################################################################################################

        iot_uid = uuid.uuid4().__str__()

        ################################################################################################################

        if mqtt_url is None:

            print('MQTT URL:')

            mqtt_url = input()

        ################################################################################################################

        if mqtt_username is None:

            print('MQTT username:')

            mqtt_username = input()

        ################################################################################################################

        if mqtt_password is None:

            print('MQTT password:')

            mqtt_password = input()

        ################################################################################################################

        saveText(fileName, WOMBAT_IOT_INI % (
            iot_uid,
            mqtt_url,
            mqtt_username,
            mqtt_password,
        ))

        ################################################################################################################

        print_logo2()

        ################################################################################################################

        return 0

    except Exception as e:

        print('error: %s' % e)

        return 1

########################################################################################################################

def createService(verbose, service_name, ext):

    ####################################################################################################################

    fileName = '/etc/systemd/system/%s.service' % service_name

    if os.path.isfile(fileName):

        print('error: file `%s` already exists' % fileName)

        return 1

    ####################################################################################################################

    try:

        ################################################################################################################

        saveText(fileName, WOMBAT_IOT_SERVICE % (
            os.path.join(WOMBAT_IOT_DIR, 'bin', 'wombat-iot'),
            os.path.join(WOMBAT_IOT_DIR, 'wombat-iot.' + 'ini'),
            os.path.join(WOMBAT_IOT_DIR, 'wombat-iot.' +  ext ),
            os.path.join(WOMBAT_IOT_DIR),
        ))

        ################################################################################################################

        print_logo2()

        ################################################################################################################

        return 0

    except Exception as e:

        print('error: %s' % e)

        return 1

########################################################################################################################

def main():

    ####################################################################################################################

    print_logo1()

    ####################################################################################################################

    parser = argparse.ArgumentParser(epilog = 'Authors:\n  Jérôme ODIER (jerome@odier.xyz)')

    parser.add_argument('--setup-py-with-ssl', help = 'Setup Wombat-IOT with SSL support', action = 'store_true')
    parser.add_argument('--setup-py-without-ssl', help = 'Setup Wombat-IOT without SSL support', action = 'store_true')

    parser.add_argument('--setup-lua-with-ssl', help = 'Setup Wombat-IOT with SSL support', action = 'store_true')
    parser.add_argument('--setup-lua-without-ssl', help = 'Setup Wombat-IOT without SSL support', action = 'store_true')

    parser.add_argument('--create-py'         , help = 'Create the main .py file'       , action = 'store_true')
    parser.add_argument('--create-lua'        , help = 'Create the main .lua file'      , action = 'store_true')
    parser.add_argument('--create-ini'        , help = 'Create the configuration file'  , action = 'store_true')
    parser.add_argument('--create-service-py' , help = 'Create the systemd service file (py mode)', action = 'store_true')
    parser.add_argument('--create-service-lua', help = 'Create the systemd service file (lua mode)', action = 'store_true')

    parser.add_argument('--mqtt-url'     , help = 'MQTT URL (use with --create-ini, default: None)'     , type = str, default = None)
    parser.add_argument('--mqtt-username', help = 'MQTT username (use with --create-ini, default: None)', type = str, default = None)
    parser.add_argument('--mqtt-password', help = 'MQTT password (use with --create-ini, default: None)', type = str, default = None)

    parser.add_argument('--service-name', help = 'Service name (use with --create-service-py or --create-service-lua, default: wombat-iot)', type = str, default = 'wombat-iot')

    parser.add_argument('--verbose', help = 'make this tool verbose', action = 'store_true')

    args = parser.parse_args()

    ####################################################################################################################

    result = 0

    if   args.setup_py_with_ssl:
        if setup(args.verbose, 'py', True) != 0:
            result = 1

    elif args.setup_py_without_ssl:
        if setup(args.verbose, 'py', False) != 0:
            result = 1

    elif args.setup_lua_with_ssl:
        if setup(args.verbose, 'lua', True) != 0:
            result = 1

    elif args.setup_lua_without_ssl:
        if setup(args.verbose, 'lua', False) != 0:
            result = 1

    elif args.create_py:
        if createPy(args.verbose) != 0:
            result = 1

    elif args.create_lua:
        if createLua(args.verbose) != 0:
            result = 1

    elif args.create_ini:
        if createIni(args.verbose, args.mqtt_url, args.mqtt_username, args.mqtt_password) != 0:
            result = 1

    elif args.create_service_py:
        if createService(args.verbose, args.service_name, 'py') != 0:
            result = 1

    elif args.create_service_lua:
        if createService(args.verbose, args.service_name, 'lua') != 0:
            result = 1

    else:
        parser.print_help()

    ####################################################################################################################

    return result

########################################################################################################################
# TEMPLATES                                                                                                            #
########################################################################################################################

WOMBAT_GITIGNORE = '''
.DS_Store

__pycache__/

bin/wombat-iot
lib/libwombat-iot.a
lib/libpaho-mqtt3a.a
lib/libpaho-mqtt3as.a
include/wombat-*.h
include/MQTT*.h
setup.py
'''[1: ]

########################################################################################################################

WOMBAT_IOT_PY = '''
########################################################################################################################

def iot_init_success(message):

    iot_log_debug('Success: Hi! I\\'m %s' % message)

########################################################################################################################

def iot_init_failure(message):

    iot_log_debug('Failure: %s' % message)

########################################################################################################################

def iot_loop(connected):

    iot_log_debug('Connected: %s' % str(connected))

########################################################################################################################

def iot_connection_opened(message):

    iot_log_debug('Connection opened, cause: %s' % (message, ))

########################################################################################################################

def iot_connection_lost(message):

    iot_log_debug('Connection lost, cause: %s' % (message, ))

########################################################################################################################

def iot_message(topic, payload):

    iot_log_debug('Topic: %s, payload: %s' % (topic, payload, ))

    return True

########################################################################################################################

def iot_delivery(token):

    iot_log_debug('Token: %s' % (token, ))

########################################################################################################################
'''[1: ]

########################################################################################################################

WOMBAT_IOT_LUA = '''
------------------------------------------------------------------------------------------------------------------------

function iot_init_success(message)

    iot_log_debug(string.format('Success: Hi! I\\'m %s', message))

end

------------------------------------------------------------------------------------------------------------------------

function iot_init_failure(message)

    iot_log_debug(string.format('Failure: %s', message))

end

------------------------------------------------------------------------------------------------------------------------

function iot_loop(connected)

    iot_log_debug(string.format('Connected: %s', tostring(connected)))

end

------------------------------------------------------------------------------------------------------------------------

function iot_connection_opened(message)

    iot_log_debug(string.format('Connection opened, cause: %s', message))

end

------------------------------------------------------------------------------------------------------------------------

function iot_connection_lost(message)

    iot_log_debug(string.format('Connection lost, cause: %s', message))

end

------------------------------------------------------------------------------------------------------------------------

function iot_message(topic, payload)

    iot_log_debug(string.format('Topic: %s, payload: %s', topic, payload))

    return true

end

------------------------------------------------------------------------------------------------------------------------

function iot_delivery(token)

    iot_log_debug(string.format('Token: %s', token))

end

------------------------------------------------------------------------------------------------------------------------
'''[1: ]

########################################################################################################################

WOMBAT_IOT_INI = '''
iot::uid=%s
iot::sleep_ms=1000

mqtt::server_url=%s
mqtt::server_user=%s
mqtt::server_pass=%s
'''[1: ]

########################################################################################################################

WOMBAT_IOT_SERVICE = '''
[Unit]
Description=Wombat-IOT
After=network.target

[Service]
ExecStart=%s --config=%s %s
WorkingDirectory=%s
StandardOutput=null
StandardError=inherit
Restart=always
User=pi

[Install]
WantedBy=multi-user.target
'''[1: ]

########################################################################################################################
# MAIN                                                                                                                 #
########################################################################################################################

if __name__ == '__main__':

    sys.exit(main())

########################################################################################################################
