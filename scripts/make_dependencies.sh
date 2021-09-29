#!/bin/bash

########################################################################################################################

PAHO_TARBALL=v1.3.9.tar.gz

LUA_TARBALL=lua-5.4.3.tar.gz

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

########################################################################################################################

if [[ 0 == 0 ]]
then
(
  ######################################################################################################################

  cd $WOMBAT_IOT_HOME

  ######################################################################################################################

  curl -L https://github.com/eclipse/paho.mqtt.c/archive/$PAHO_TARBALL | tar xzf -

  ######################################################################################################################

  cd ./paho.mqtt.c-*/
  mkdir ./build/
  cd ./build/

  if [[ -n $(uname 2>/dev/null | grep Darwin) ]]
  then
    if [[ $WITH_SSL == 1 ]]
    then
      CC="${CC:-clang}" CFLAGS="-fPIC -O3" cmake -DCMAKE_INSTALL_PREFIX=$WOMBAT_IOT_HOME -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=TRUE -DPAHO_ENABLE_TESTING=FALSE -DPAHO_ENABLE_CPACK=FALSE ..
      make paho-mqtt3as-static
    else
      CC="${CC:-clang}" CFLAGS="-fPIC -O3" cmake -DCMAKE_INSTALL_PREFIX=$WOMBAT_IOT_HOME -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=FALSE -DPAHO_ENABLE_TESTING=FALSE -DPAHO_ENABLE_CPACK=FALSE ..
      make paho-mqtt3a-static
    fi
  else
    if [[ $WITH_SSL == 1 ]]
    then
      CC="${CC:-gcc}" CFLAGS="-fPIC -O3" cmake -DCMAKE_INSTALL_PREFIX=$WOMBAT_IOT_HOME -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=TRUE -DPAHO_ENABLE_TESTING=FALSE -DPAHO_ENABLE_CPACK=FALSE ..
      make paho-mqtt3as-static
    else
      CC="${CC:-gcc}" CFLAGS="-fPIC -O3" cmake -DCMAKE_INSTALL_PREFIX=$WOMBAT_IOT_HOME -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=FALSE -DPAHO_ENABLE_TESTING=FALSE -DPAHO_ENABLE_CPACK=FALSE ..
      make paho-mqtt3a-static
    fi
  fi

  ######################################################################################################################

  mkdir -p ../../lib/

  if [[ $WITH_SSL == 1 ]]
  then
    cp ./src/libpaho-mqtt3as.a ../../lib/
  else
    cp ./src/libpaho-mqtt3a.a ../../lib/
  fi

  ######################################################################################################################

  mkdir -p ../../include/

  cp ../src/MQTTAsync.h ../../include/
  cp ../src/MQTTClientPersistence.h ../../include/
  cp ../src/MQTTExportDeclarations.h ../../include/
  cp ../src/MQTTProperties.h ../../include/
  cp ../src/MQTTReasonCodes.h ../../include/
  cp ../src/MQTTSubscribeOpts.h ../../include/

  ######################################################################################################################
) || exit 1
fi

########################################################################################################################

rm -fr $WOMBAT_IOT_HOME/paho.mqtt.c-*/

########################################################################################################################

echo ''
echo '#############################################################################'
echo '# Compiling Lua...                                                          #'
echo '#############################################################################'
echo ''

########################################################################################################################

rm -fr $WOMBAT_IOT_HOME/lua-*/

########################################################################################################################

if [[ 0 == 0 ]]
then
(
  ######################################################################################################################

  cd $WOMBAT_IOT_HOME

  ######################################################################################################################

  curl -L https://www.lua.org/ftp/$LUA_TARBALL | tar xzf -

  ######################################################################################################################

  cd ./lua-*/

  mkdir -p ../lib/
  mkdir -p ../include/

  ######################################################################################################################

  CC=${CC:-'gcc'}
  AR=${AR:-'ar'}
  RANLIB=${RANLIB:-'ranlib'}
  CFLAGS=${CFLAGS:-'-std=c99 -O3'}

  ######################################################################################################################

  if [[ -n $(uname 2>/dev/null | grep Darwin) ]]
  then
    PLATFORM='LUA_USE_MACOSX'
  else
    PLATFORM='LUA_USE_LINUX'
  fi

  ######################################################################################################################

  cat >> ./src/lauxlib.c << EOF
LUALIB_API int luaL_loadmodulebuffer(lua_State *L, const char *buff, size_t size, const char *modname, const char *filename)
{
	lua_getglobal(L, "package");

	lua_getfield(L, -1, "preload");

	int result = luaL_loadbufferx(L, buff, size, filename, NULL);

	if(result == LUA_OK)
	{
		lua_setfield(L, -2, modname);
	}

	lua_pop(L, 2);

	return result;
}
EOF

  ######################################################################################################################

  cat >> ./src/lauxlib.h << EOF
#ifndef lauxlib_wombat_iot_h
#define lauxlib_wombat_iot_h

LUALIB_API int luaL_loadmodulebuffer(lua_State *L, const char *buff, size_t size, const char *modname, const char *filename);

#endif
EOF

  ######################################################################################################################

  for filename in ./src/*.c
  do
    if [[ $filename != './src/lua.c' ]] && [[ $filename != './src/luac.c' ]]
    then
      echo "Compiling \`${filename}\`..."

      $CC $CFLAGS -fPIC -D$PLATFORM -c -o ${filename%.c}.o $filename
    fi
  done

  ######################################################################################################################

  $AR rcs ../lib/liblua.a ./src/*.o

  $RANLIB ../lib/liblua.a

  ######################################################################################################################

  cp ./src/lua.h ../include/
  cp ./src/lualib.h ../include/
  cp ./src/luaconf.h ../include/
  cp ./src/lauxlib.h ../include/

  ######################################################################################################################
) || exit 1
fi

########################################################################################################################

rm -fr $WOMBAT_IOT_HOME/lua-*/

########################################################################################################################

echo ''
echo '#############################################################################'
echo '# Compiling Lua-JSON...                                                     #'
echo '#############################################################################'
echo ''

########################################################################################################################

rm -fr $WOMBAT_IOT_HOME/json.lua

########################################################################################################################

if [[ 0 == 0 ]]
then
(
  ######################################################################################################################

  cd $WOMBAT_IOT_HOME

  ######################################################################################################################

  curl -L https://raw.githubusercontent.com/rxi/json.lua/master/json.lua > ./json.lua

  ######################################################################################################################

  ./scripts/bin2h.py ./json.lua json_module > include/luajson.h

  ######################################################################################################################

) || exit 1
fi

########################################################################################################################

rm -fr $WOMBAT_IOT_HOME/json.lua

########################################################################################################################

echo ''
echo '#############################################################################'
echo ''
echo 'Done with success :-)'
echo ''

########################################################################################################################
