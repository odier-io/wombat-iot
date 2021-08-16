#!/bin/bash

########################################################################################################################

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

  LIBDIR=$(python3 -c 'import sys, sysconfig; sys.stdout.write("%s" % sysconfig.get_config_var("LIBDIR"))')

  if [[ -n $(uname 2>/dev/null | grep Darwin) ]]
  then
    CC=clang CFLAGS="-fPIC -O3 -I ../../include" LDFLAGS="-L ../../lib -L $LIBDIR" \
    cmake -DCMAKE_INSTALL_PREFIX=$WOMBAT_IOT_HOME -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=TRUE -DPAHO_ENABLE_TESTING=FALSE -DPAHO_ENABLE_CPACK=FALSE ..
  else
    CC=gcc CFLAGS="-fPIC -O3 -I ../../include" LDFLAGS="-L ../../lib -L $LIBDIR" \
    cmake -DCMAKE_INSTALL_PREFIX=$WOMBAT_IOT_HOME -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=TRUE -DPAHO_ENABLE_TESTING=FALSE -DPAHO_ENABLE_CPACK=FALSE ..
  fi

  make paho-mqtt3as-static

  ######################################################################################################################

  cp ./src/libpaho-mqtt3as.a ../../lib

  ######################################################################################################################

  cp ../src/MQTTAsync.h ../../include
  cp ../src/MQTTClientPersistence.h ../../include
  cp ../src/MQTTExportDeclarations.h ../../include
  cp ../src/MQTTProperties.h ../../include
  cp ../src/MQTTReasonCodes.h ../../include
  cp ../src/MQTTSubscribeOpts.h ../../include

  ######################################################################################################################
) || exit 1
fi

########################################################################################################################

#rm -fr $WOMBAT_IOT_HOME/paho.mqtt.c-*/

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
