#!/usr/bin/env python3
# -*- coding:utf-8 -*-
########################################################################################################################
# Author : Jerome ODIER
# Email : jerome@odier.xyz
#
# Version : 1.0 (2016-![VALUE YEAR])
#
#
# This file is part of WOMBAT-IOT.
#
#
# Batkens RESTRICTED
# __________________
#
#  Batkens
#  All Rights Reserved.
#
# NOTICE:  All information contained herein is, and remains
# the property of Batkens and its suppliers, if any.
# The intellectual and technical concepts contained herein are
# proprietary to Batkens and its suppliers and are protected by
# trade secret or copyright law. Dissemination of this information
# or reproduction of this material is strictly forbidden unless
# prior written permission is obtained from Batkens.
########################################################################################################################

import os, sys, stat, shutil, hashlib, argparse, subprocess

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

    print('''
     _/\/\______/\/\____/\/\/\/\____/\/\______/\/\__/\/\/\/\/\________/\/\______/\/\/\/\/\/\____________/\/\/\/\____/\/\/\/\____/\/\/\/\/\/\_
    _/\/\__/\__/\/\__/\/\____/\/\__/\/\/\__/\/\/\__/\/\____/\/\____/\/\/\/\________/\/\__________________/\/\____/\/\____/\/\______/\/\_____ 
   _/\/\/\/\/\/\/\__/\/\____/\/\__/\/\/\/\/\/\/\__/\/\/\/\/\____/\/\____/\/\______/\/\______/\/\/\/\____/\/\____/\/\____/\/\______/\/\_____  
  _/\/\/\__/\/\/\__/\/\____/\/\__/\/\__/\__/\/\__/\/\____/\/\__/\/\/\/\/\/\______/\/\__________________/\/\____/\/\____/\/\______/\/\_____   
 _/\/\______/\/\____/\/\/\/\____/\/\______/\/\__/\/\/\/\/\____/\/\____/\/\______/\/\________________/\/\/\/\____/\/\/\/\________/\/\_____    
________________________________________________________________________________________________________________________________________     '''[1: ])

########################################################################################################################

def saveText(fileName, data):

    print('Saving `%s`...' % fileName)

    with open(fileName, 'wt') as f:

        f.write(data)

########################################################################################################################

def setup(verbose):

    ####################################################################################################################

    binDir = os.path.join(WOMBAT_IOT_DIR, 'bin')
    libDir = os.path.join(WOMBAT_IOT_DIR, 'lib')
    incDir = os.path.join(WOMBAT_IOT_DIR, 'include')
    gitDir = os.path.join(WOMBAT_IOT_DIR, '.wombat-iot')

    ####################################################################################################################
    #                                                                                                                  #
    ####################################################################################################################

    if os.path.isdir(gitDir):

        shutil.rmtree(gitDir)

    ####################################################################################################################

    subprocess.check_call(['git', 'clone', WOMBAT_IOT_GIT_URL, gitDir])

    subprocess.check_call('make deps', cwd = gitDir, shell = True)

    subprocess.check_call('make all', cwd = gitDir, shell = True)

    ####################################################################################################################
    #                                                                                                                  #
    ####################################################################################################################

    if not os.path.isdir(binDir):

        os.mkdir(binDir)

    if not os.path.isdir(libDir):

        os.mkdir(libDir)

    if not os.path.isdir(incDir):

        os.mkdir(incDir)

    ####################################################################################################################

    subprocess.check_call(   'cp bin/* %s'    % binDir, cwd = gitDir, shell = True)

    subprocess.check_call(  'cp lib/*.a %s'   % libDir, cwd = gitDir, shell = True)

    subprocess.check_call('cp include/*.h %s' % incDir, cwd = gitDir, shell = True)

    ####################################################################################################################

    os.chmod(os.path.join(binDir, 'wombat-iot'), os.stat(os.path.join(binDir, 'wombat-iot')).st_mode | stat.S_IEXEC)

    ####################################################################################################################

    shutil.rmtree(gitDir)

########################################################################################################################

def createPy(verbose):

    try:

        ################################################################################################################

        saveText(os.path.join(WOMBAT_IOT_DIR, 'wombat-iot.py'), WOMBAT_IOT_PY)

        ################################################################################################################

        print_logo2()

        ################################################################################################################

        return 0

    except Exception as e:

        print('error: %s' % e)

        return 1

########################################################################################################################

def createIni(verbose, url, username, password):

    try:

        ################################################################################################################

        if url is None:

            print('MQTT URL:')

            MQTT_URL = input()

        else:

            MQTT_URL = url

        ################################################################################################################

        if username is None:

            print('MQTT username:')

            MQTT_USERNAME = input()

        else:

            MQTT_USERNAME = username

        ################################################################################################################

        if password is None:

            print('MQTT password:')

            MQTT_PASSWORD = input()

        else:

            MQTT_PASSWORD = password

        ################################################################################################################

        saveText(os.path.join(WOMBAT_IOT_DIR, 'wombat-iot.ini'), WOMBAT_IOT_INI % (
            MQTT_URL,
            MQTT_USERNAME,
            MQTT_PASSWORD,
        ))

        ################################################################################################################

        print_logo2()

        ################################################################################################################

        return 0

    except Exception as e:

        print('error: %s' % e)

        return 1

########################################################################################################################

def createService(verbose):

    try:

        ################################################################################################################

        saveText('/etc/systemd/system/wombat-iot.service', WOMBAT_IOT_SERVICE % (
            os.path.join(WOMBAT_IOT_DIR, 'bin', 'wombat-iot'),
            os.path.join(WOMBAT_IOT_DIR, 'wombat-iot.ini'),
            os.path.join(WOMBAT_IOT_DIR, 'wombat-iot.py'),
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

    parser = argparse.ArgumentParser(formatter_class = argparse.RawTextHelpFormatter, epilog = 'Authors:\n  Jérôme ODIER (jerome@odier.xyz)')

    parser.add_argument('--setup', help = 'setup Wombat-IOT', action = 'store_true')

    parser.add_argument('--create-py'     , help = 'create the main Python file'    , action = 'store_true')
    parser.add_argument('--create-ini'    , help = 'create the configuration file'  , action = 'store_true')
    parser.add_argument('--create-service', help = 'create the systemd service file', action = 'store_true')

    parser.add_argument('--url'     , help = 'MQTT URL (use with --create-ini, default: None)'     , type = str, default = None)
    parser.add_argument('--username', help = 'MQTT username (use with --create-ini, default: None)', type = str, default = None)
    parser.add_argument('--password', help = 'MQTT password (use with --create-ini, default: None)', type = str, default = None)

    parser.add_argument('--verbose', help = 'make this tool verbose', action = 'store_true')

    args = parser.parse_args()

    ####################################################################################################################

    result = 0

    if   args.setup:
        if setup(args.verbose) != 0:
            result = 1

    if   args.create_py:
        if createPy(args.verbose) != 0:
            result = 1

    elif args.create_ini:
        if createIni(args.verbose, args.url, args.username, args.password) != 0:
            result = 1

    elif args.create_service:
        if createService(args.verbose) != 0:
            result = 1

    ####################################################################################################################

    return result

########################################################################################################################
# TEMPLATES                                                                                                            #
########################################################################################################################

WOMBAT_IOT_PY = '''
########################################################################################################################

def iot_init_success(message):

    iot_log_debug('Success: %s' % message)

########################################################################################################################

def iot_init_failure(message):

    iot_log_debug('Failure: %s' % message)

########################################################################################################################

def iot_loop(connected):

    iot_log_debug('Connected: %d' % connected)

########################################################################################################################

def iot_connection_opened(message):

    iot_log_debug('Connection opened, cause: %s' % (message, ))

########################################################################################################################

def iot_connection_lost(message):

    iot_log_debug('Connection lost, cause: %s' % (message, ))

########################################################################################################################

def iot_message(topic, payload):

    iot_log_debug('Topic: %s, payload: %s' % (topic, payload, ))

    return 1

########################################################################################################################

def iot_delivery(token):

    iot_log_debug('Token: %s' % (token, ))

########################################################################################################################
'''[1: ]

########################################################################################################################

WOMBAT_IOT_INI = '''
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
ExecStart=%s --config=%s --script=%s
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
