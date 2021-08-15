#!/bin/bash

########################################################################################################################

PAHO_TARBALL=v1.3.9.tar.gz

########################################################################################################################

THIS_SCRIPT=${BASH_SOURCE[0]:-$0}

while [[ "$(readlink $THIS_SCRIPT)" != "" ]]
do
  THIS_SCRIPT=$(readlink $THIS_SCRIPT)
done

JEROLIV_CLIENT_HOME=$(cd "$(dirname $THIS_SCRIPT)" && pwd)/..

########################################################################################################################

export PKG_CONFIG_PATH=$JEROLIV_CLIENT_HOME/lib/pkgconfig

########################################################################################################################

echo ''
echo '#############################################################################'
echo '# Compiling Paho...                                                         #'
echo '#############################################################################'
echo ''

########################################################################################################################

rm -fr $JEROLIV_CLIENT_HOME/$PAHO_TARBALL
rm -fr $JEROLIV_CLIENT_HOME/paho.mqtt.c-*/

########################################################################################################################

if [[ 0 == 0 ]]
then
(
  ######################################################################################################################

  cd $JEROLIV_CLIENT_HOME

  ######################################################################################################################

  curl -O -L https://github.com/eclipse/paho.mqtt.c/archive/$PAHO_TARBALL

  tar xf $PAHO_TARBALL

  ######################################################################################################################

  cd paho.mqtt.c-*/
  mkdir build
  cd build 

  if [[ -n $(uname 2>/dev/null | grep Darwin) ]]
  then
    CC=clang CFLAGS="-fPIC -O3" \
    cmake -DCMAKE_INSTALL_PREFIX=$JEROLIV_CLIENT_HOME -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=TRUE -DPAHO_ENABLE_TESTING=FALSE -DPAHO_ENABLE_CPACK=FALSE ..
  else
    CC=gcc CFLAGS="-fPIC -O3"
    cmake -DCMAKE_INSTALL_PREFIX=$JEROLIV_CLIENT_HOME -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=TRUE -DPAHO_ENABLE_TESTING=FALSE -DPAHO_ENABLE_CPACK=FALSE ..
  fi

  make all install

  ######################################################################################################################

  rm -fr $JEROLIV_CLIENT_HOME/bin/MQTTVersion
  rm -fr $JEROLIV_CLIENT_HOME/lib/libpaho-mqtt3c.a
  rm -fr $JEROLIV_CLIENT_HOME/lib/libpaho-mqtt3cs.a

  ######################################################################################################################
) || exit 1
fi

########################################################################################################################

rm -fr $JEROLIV_CLIENT_HOME/$PAHO_TARBALL
rm -fr $JEROLIV_CLIENT_HOME/paho.mqtt.c-*/

########################################################################################################################

echo ''
echo '#############################################################################'
echo ''
echo 'Done with success :-)'
echo ''

rm -fr $JEROLIV_CLIENT_HOME/share/

########################################################################################################################
