#!/bin/bash

########################################################################################################################

LIBRESSL_TARBALL=libressl-3.3.3.tar.gz

PAHO_TARBALL=v1.3.9.tar.gz

########################################################################################################################

THIS_SCRIPT=${BASH_SOURCE[0]:-$0}

while [[ "$(readlink $THIS_SCRIPT)" != "" ]]
do
  THIS_SCRIPT=$(readlink $THIS_SCRIPT)
done

WOMBAT_IOT_HOME=$(cd "$(dirname $THIS_SCRIPT)" && pwd)/..

########################################################################################################################

echo ''
echo '#############################################################################'
echo '# Compiling LibreSSL...                                                     #'
echo '#############################################################################'
echo ''

########################################################################################################################

########################################################################################################################

if [[ 0 == 0 ]] && [[ -n $(uname 2>/dev/null | grep Darwin) ]]
then
(
  ######################################################################################################################

  cd $WOMBAT_IOT_HOME

  ######################################################################################################################

  curl -L https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/$LIBRESSL_TARBALL | tar xzf -

  ######################################################################################################################

  cd libressl-*/
  mkdir build
  cd build

  if [[ -n $(uname 2>/dev/null | grep Darwin) ]]
  then
    CC=clang CFLAGS="-fPIC -O3" \
    cmake -DCMAKE_INSTALL_PREFIX=$WOMBAT_IOT_HOME -DLIBRESSL_APPS=FALSE -DLIBRESSL_TESTS=FALSE -DBUILD_SHARED_LIBS=FALSE ..
  else
    CC=gcc CFLAGS="-fPIC -O3"
    cmake -DCMAKE_INSTALL_PREFIX=$WOMBAT_IOT_HOME -DLIBRESSL_APPS=FALSE -DLIBRESSL_TESTS=FALSE -DBUILD_SHARED_LIBS=FALSE ..
  fi

  make all install

  ######################################################################################################################

  rm -fr $WOMBAT_IOT_HOME/include/tls.h

  rm -fr $WOMBAT_IOT_HOME/lib/libtls.a

  ######################################################################################################################
) || exit 1
fi

########################################################################################################################

rm -fr $WOMBAT_IOT_HOME/libressl-*/

########################################################################################################################

echo ''
echo '#############################################################################'
echo '# Compiling Paho...                                                         #'
echo '#############################################################################'
echo ''

########################################################################################################################

rm -fr $WOMBAT_IOT_HOME/paho.mqtt.c-*/
rm -fr $WOMBAT_IOT_HOME/share/

########################################################################################################################

if [[ 0 == 0 ]]
then
(
  ######################################################################################################################

  cd $WOMBAT_IOT_HOME

  ######################################################################################################################

  curl -L https://github.com/eclipse/paho.mqtt.c/archive/$PAHO_TARBALL | tar xzf -

  ######################################################################################################################

  cd paho.mqtt.c-*/
  mkdir build
  cd build

  if [[ -n $(uname 2>/dev/null | grep Darwin) ]]
  then
    CC=clang CFLAGS="-fPIC -O3 -I ../../include" LDFLAGS="-L ../../lib" \
    cmake -DCMAKE_INSTALL_PREFIX=$WOMBAT_IOT_HOME -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=TRUE -DPAHO_ENABLE_TESTING=FALSE -DPAHO_ENABLE_CPACK=FALSE ..
  else
    CC=gcc CFLAGS="-fPIC -O3 -I ../../include" LDFLAGS="-L ../../lib" \
    cmake -DCMAKE_INSTALL_PREFIX=$WOMBAT_IOT_HOME -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=TRUE -DPAHO_ENABLE_TESTING=FALSE -DPAHO_ENABLE_CPACK=FALSE ..
  fi

  make all install

  ######################################################################################################################

  rm -fr $WOMBAT_IOT_HOME/bin/MQTTVersion

  rm -fr $WOMBAT_IOT_HOME/include/MQTTClient.h

  rm -fr $WOMBAT_IOT_HOME/lib/libpaho-mqtt3c.a
  rm -fr $WOMBAT_IOT_HOME/lib/libpaho-mqtt3cs.a

  ######################################################################################################################
) || exit 1
fi

########################################################################################################################

rm -fr $WOMBAT_IOT_HOME/paho.mqtt.c-*/

########################################################################################################################

echo ''
echo '#############################################################################'
echo ''
echo 'Done with success :-)'
echo ''

########################################################################################################################

rm -fr $WOMBAT_IOT_HOME/lib/pkgconfig/
rm -fr $WOMBAT_IOT_HOME/lib/cmake/
rm -fr $WOMBAT_IOT_HOME/share/

########################################################################################################################
