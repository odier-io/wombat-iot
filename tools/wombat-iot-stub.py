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

WOMBAT_IOT_TOOLS_IMAGE_URL = 'https://raw.githubusercontent.com/odier-io/wombat-iot/master/tools/wombat-iot-tools.img'

########################################################################################################################

import ssl, sys, zlib, base64

########################################################################################################################

try:

    import urllib.request as urllib_request

except ImportError:

    import urllib2 as urllib_request

########################################################################################################################
# MAIN                                                                                                                 #
########################################################################################################################

if __name__ == '__main__':

    ####################################################################################################################

    try:

        f = urllib_request.urlopen(WOMBAT_IOT_TOOLS_IMAGE_URL, context = hasattr(ssl, '_create_unverified_context') and ssl._create_unverified_context() or None)

        try:

            code = zlib.decompress(base64.b64decode(f.read()))

        finally:

            f.close()

    except Exception as e:

        print(e)

        sys.exit(1)

    ####################################################################################################################

    exec(code)

########################################################################################################################
