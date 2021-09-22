/* Wombat-IOT
 *
 * Copyright (C) 2020 Jérôme Odier (jerome@odier.xyz)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/*--------------------------------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/lua.h"
#include "../include/lualib.h"
#include "../include/lauxlib.h"

/*--------------------------------------------------------------------------------------------------------------------*/

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static iot_t *_lua_iot;

/*--------------------------------------------------------------------------------------------------------------------*/

static void *__alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	return iot_realloc(ptr, nsize);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_service_stop(lua_State *state)
{
	iot_log(IOT_LOG_TYPE_DEBUG, "Stopping service `" IOT_NAME "-iot`...\n");

	system("sudo systemctl stop " IOT_NAME "-iot");

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_service_restart(lua_State *state)
{
	iot_log(IOT_LOG_TYPE_DEBUG, "Restarting service `" IOT_NAME "-iot`...\n");

	system("sudo systemctl restart " IOT_NAME "-iot");

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_machine_reboot(lua_State *state)
{
	iot_log(IOT_LOG_TYPE_DEBUG, "Rebooting machine...\n");

	system("sudo reboot");

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_get_uid(lua_State *state)
{
	lua_pushstring(state, _lua_iot->uid);

	return 1;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_get_descr(lua_State *state)
{
	lua_pushstring(state, _lua_iot->descr);

	return 1;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_config_get_str(lua_State *state)
{
	STR_t key = luaL_checkstring(state, 1);
	STR_t val = luaL_optstring(state, 2, (""));

	lua_pushstring(state, iot_config_get_str(_lua_iot->config, key, val));

	return 1;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_config_get_int(lua_State *state)
{
	STR_t key = luaL_checkstring(state, 1);
	int_t val = luaL_optinteger(state, 2, 0x00);

	lua_pushinteger(state, iot_config_get_int(_lua_iot->config, key, val));

	return 1;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_config_get_flt(lua_State *state)
{
	STR_t key = luaL_checkstring(state, 1);
	float val = luaL_optnumber(state, 2, 0.0f);

	lua_pushnumber(state, iot_config_get_flt(_lua_iot->config, key, val));

	return 1;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_log_debug(lua_State *state)
{
	lua_Debug debug;

	lua_getstack(state, 1, &debug);
	lua_getinfo(state, "Sl", &debug);

	STR_t message = luaL_checkstring(state, 1);

	_iot_log(IOT_LOG_TYPE_DEBUG, debug.source, debug.currentline, "%s\n", message);

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_log_ooops(lua_State *state)
{
	lua_Debug debug;

	lua_getstack(state, 1, &debug);
	lua_getinfo(state, "Sl", &debug);

	STR_t message = luaL_checkstring(state, 1);

	_iot_log(IOT_LOG_TYPE_OOOPS, debug.source, debug.currentline, "%s\n", message);

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_log_error(lua_State *state)
{
	lua_Debug debug;

	lua_getstack(state, 1, &debug);
	lua_getinfo(state, "Sl", &debug);

	STR_t message = luaL_checkstring(state, 1);

	_iot_log(IOT_LOG_TYPE_ERROR, debug.source, debug.currentline, "%s\n", message);

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_log_fatal(lua_State *state)
{
	lua_Debug debug;

	lua_getstack(state, 1, &debug);
	lua_getinfo(state, "Sl", &debug);

	STR_t message = luaL_checkstring(state, 1);

	_iot_log(IOT_LOG_TYPE_FATAL, debug.source, debug.currentline, "%s\n", message);

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_mqtt_subscribe(lua_State *state)
{
	STR_t topic = luaL_checkstring(state, 1);
	int_t qos = luaL_optinteger(state, 2, 0);

	if(iot_mqtt_subscribe(
		&_lua_iot->mqtt,
		topic,
		qos
	 ) >= 0) {

		return 0;
	}
	else
	{
		return luaL_error(state, "Cannot not subscribe to topic '%s'", topic);
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_mqtt_unsubscribe(lua_State *state)
{
	STR_t topic = luaL_checkstring(state, 1);
	int_t qos = luaL_optinteger(state, 2, 0);

	if(iot_mqtt_unsubscribe(
		&_lua_iot->mqtt,
		topic,
		qos
	 ) >= 0) {

		return 0;
	}
	else
	{
		return luaL_error(state, "Cannot not unsubscribe to topic '%s'", topic);
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

struct _iot_mqtt_send_callback_context_s
{
	void *success_callback;

	void *failure_callback;
};

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_mqtt_message_success_callback(void *context, iot_mqtt_t *mqtt, int_t token)
{
	struct _iot_mqtt_send_callback_context_s *mqtt_send_callback_context = (struct _iot_mqtt_send_callback_context_s *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	/* TODO */

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_free(mqtt_send_callback_context);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_mqtt_message_failure_callback(void *context, iot_mqtt_t *mqtt, STR_t message)
{
	struct _iot_mqtt_send_callback_context_s *mqtt_send_callback_context = (struct _iot_mqtt_send_callback_context_s *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	/* TODO */

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_free(mqtt_send_callback_context);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_mqtt_send(lua_State *state)
{
	/*----------------------------------------------------------------------------------------------------------------*/

	STR_t topic = luaL_checkstring(state, 1);
	STR_t payload = luaL_checkstring(state, 2);
	int_t /**/qos/**/ = luaL_optinteger(state, 3, 0);
	int_t retained = luaL_optinteger(state, 4, 0);

	/*----------------------------------------------------------------------------------------------------------------*/

	struct _iot_mqtt_send_callback_context_s *mqtt_send_callback_context = (struct _iot_mqtt_send_callback_context_s *) iot_malloc(sizeof(struct _iot_mqtt_send_callback_context_s));

	mqtt_send_callback_context->success_callback = NULL; /* TODO */
	mqtt_send_callback_context->failure_callback = NULL; /* TODO */

	/*----------------------------------------------------------------------------------------------------------------*/

	if(iot_mqtt_send(
		&_lua_iot->mqtt,
		_iot_mqtt_message_success_callback,
		_iot_mqtt_message_failure_callback,
		mqtt_send_callback_context,
		topic,
		strlen(payload),
		/*--*/(payload),
		/**/qos/**/,
		retained
	 ) >= 0) {

		return 0;
	}
	else
	{
		return luaL_error(state, "Cannot not send message (network issue)");
	}

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

static const struct luaL_Reg _iot_builtins_functions[] = {
	{"iot_service_stop", _iot_service_stop},
	{"iot_service_restart", _iot_service_restart},
	{"iot_machine_reboot", _iot_machine_reboot},
	{"iot_get_uid", _iot_get_uid},
	{"iot_get_descr", _iot_get_descr},
	{"iot_config_get_str", _iot_config_get_str},
	{"iot_config_get_int", _iot_config_get_int},
	{"iot_config_get_flt", _iot_config_get_flt},
	{"iot_log_debug", _iot_log_debug},
	{"iot_log_ooops", _iot_log_ooops},
	{"iot_log_error", _iot_log_error},
	{"iot_log_fatal", _iot_log_fatal},
	{"iot_mqtt_subscribe", _iot_mqtt_subscribe},
	{"iot_mqtt_unsubscribe", _iot_mqtt_unsubscribe},
	{"iot_mqtt_send", _iot_mqtt_send},
	{NULL, NULL},
};

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_init_success_callback(iot_mqtt_t *mqtt, STR_t iot_uid)
{
	lua_State *state = (lua_State *) _lua_iot->pFuncInitSuccess;

	if(state != NULL)
	{
		lua_getglobal(state, "int_init_success");

		lua_pushstring(state, iot_uid);

		if(lua_pcall(state, 1, 0, 0) != LUA_OK)
		{
			iot_log(IOT_LOG_TYPE_OOOPS, "Error running function `int_init_success`: %s\n", lua_tostring(state, -1));
		}

		lua_pop(state, lua_gettop(state));
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_init_failure_callback(iot_mqtt_t *mqtt, STR_t message)
{
	lua_State *state = (lua_State *) _lua_iot->pFuncInitFailure;

	if(state != NULL)
	{
		lua_getglobal(state, "int_init_failure");

		lua_pushstring(state, message);

		if(lua_pcall(state, 1, 0, 0) != LUA_OK)
		{
			iot_log(IOT_LOG_TYPE_OOOPS, "Error running function `int_init_failure`: %s\n", lua_tostring(state, -1));
		}

		lua_pop(state, lua_gettop(state));
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_connection_opened_callback(iot_mqtt_t *mqtt, STR_t message)
{
	lua_State *state = (lua_State *) _lua_iot->pFuncConnectionOpened;

	if(state != NULL)
	{
		lua_getglobal(state, "iot_connection_opened");

		lua_pushstring(state, message);

		if(lua_pcall(state, 1, 0, 0) != LUA_OK)
		{
			iot_log(IOT_LOG_TYPE_OOOPS, "Error running function `iot_connection_opened`: %s\n", lua_tostring(state, -1));
		}

		lua_pop(state, lua_gettop(state));
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_connection_lost_callback(iot_mqtt_t *mqtt, STR_t message)
{
	lua_State *state = (lua_State *) _lua_iot->pFuncConnectionLost;

	if(state != NULL)
	{
		lua_getglobal(state, "iot_connection_lost");

		lua_pushstring(state, message);

		if(lua_pcall(state, 1, 0, 0) != LUA_OK)
		{
			iot_log(IOT_LOG_TYPE_OOOPS, "Error running function `iot_connection_lost`: %s\n", lua_tostring(state, -1));
		}

		lua_pop(state, lua_gettop(state));
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_message_callback(iot_mqtt_t *mqtt, size_t topic_size, STR_t topic_str, size_t payload_size, BUFF_t payload_buff)
{
	int_t result = 1;

	lua_State *state = (lua_State *) _lua_iot->pFuncMessage;

	if(state != NULL)
	{
		lua_getglobal(state, "int_message");

		lua_pushlstring(state, topic_str, topic_size);
		lua_pushlstring(state, payload_buff, payload_size);

		if(lua_pcall(state, 2, 1, 0) != LUA_OK)
		{
			iot_log(IOT_LOG_TYPE_OOOPS, "Error running function `int_message`: %s\n", lua_tostring(state, -1));
		}
		else
		{
			if(lua_isboolean(state, -1))
			{
				result = lua_toboolean(state, -1);
			}
		}

		lua_pop(state, lua_gettop(state));
	}

	return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_delivery_callback(iot_mqtt_t *mqtt, int_t token)
{
	lua_State *state = (lua_State *) _lua_iot->pFuncDelivery;

	if(state != NULL)
	{
		lua_getglobal(state, "iot_delivery");

		lua_pushinteger(state, token);

		if(lua_pcall(state, 1, 0, 0) != LUA_OK)
		{
			iot_log(IOT_LOG_TYPE_OOOPS, "Error running function `iot_delivery`: %s\n", lua_tostring(state, -1));
		}

		lua_pop(state, lua_gettop(state));
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

void iot_loop(iot_t *iot, iot_config_t *config, STR_t script_fname, STR_t uid, STR_t descr, int_t sleep_ms, STR_t mqtt_server_uri, STR_t mqtt_server_user, STR_t mqtt_server_pass, int_t mqtt_connect_timeout, int_t mqtt_disconnect_timeout, int_t mqtt_log_level)
{
	int_t ret;

	/*----------------------------------------------------------------------------------------------------------------*/
	/* INITIALIZE IOT                                                                                                 */
	/*----------------------------------------------------------------------------------------------------------------*/

	memset(iot, 0x00, sizeof(iot_t));

	/*----------------------------------------------------------------------------------------------------------------*/

	iot->uid = uid;
	iot->descr = descr;
	iot->config = config;

	/*----------------------------------------------------------------------------------------------------------------*/

	_lua_iot = iot;

	/*----------------------------------------------------------------------------------------------------------------*/
	/* INITIALIZE LUA                                                                                                 */
	/*----------------------------------------------------------------------------------------------------------------*/

	lua_State *state = lua_newstate(__alloc, NULL);

	if(state == NULL)
	{
		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not initialize Lua\n");
	}

	luaL_openlibs(state);

	/*----------------------------------------------------------------------------------------------------------------*/

	const struct luaL_Reg *reg;

	for(reg = _iot_builtins_functions; reg->name != NULL && reg->func != NULL; reg++)
	{
		lua_pushcfunction(state, reg->func);

	    lua_setglobal(state, reg->name);
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = luaL_dostring(
		state,
		"-----------------------------------------------------------------------------"						"\n"
		""																									"\n"
		"function int_init_success(iot_uid)"																"\n"
		""																									"\n"
		"	local ok, message = pcall(iot_init_success, string.format('Hi! I\\'m %s.', iot_get_uid()))"		"\n"
		""																									"\n"
		"	if not ok then"																					"\n"
		"		iot_log_ooops(message)"																		"\n"
		"	end"																							"\n"
		"end"																								"\n"
		""																									"\n"
		"-----------------------------------------------------------------------------"						"\n"
		""																									"\n"
		"function int_init_failure(message)"																"\n"
		""																									"\n"
		"	local ok, message = pcall(iot_init_success, message)"											"\n"
		""																									"\n"
		"	if not ok then"																					"\n"
		"		iot_log_ooops(message)"																		"\n"
		"	end"																							"\n"
		"end"																								"\n"
		""																									"\n"
		"-----------------------------------------------------------------------------"						"\n"
		""																									"\n"
		"function int_loop(connected)"																		"\n"
		""																									"\n"
		"	local ok, message = pcall(iot_loop, connected)"													"\n"
		""																									"\n"
		"	if not ok then"																					"\n"
		"		iot_log_ooops(message)"																		"\n"
		"	end"																							"\n"
		"end"																								"\n"
		""																									"\n"
		"-----------------------------------------------------------------------------"						"\n"
		""																									"\n"
		"function int_message(topic, payload)"																"\n"
		""																									"\n"
		"	local ok, message = pcall(iot_message, topic, payload)"											"\n"
		""																									"\n"
		"	if not ok then"																					"\n"
		"		iot_log_ooops(message)"																		"\n"
		"	end"																							"\n"
		"end"																								"\n"
		""																									"\n"
		"-----------------------------------------------------------------------------"						"\n"
	);

	if(ret != LUA_OK)
	{
		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not execute internal Lua script `%s`\n", script_fname);
	}

	lua_pop(state, lua_gettop(state));

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = luaL_dofile(state, script_fname);

	if(ret != LUA_OK)
	{
		iot_log(IOT_LOG_TYPE_ERROR, "Cannot not execute Lua script `%s`\n", script_fname);
	}

	lua_pop(state, lua_gettop(state));

	/*----------------------------------------------------------------------------------------------------------------*/

	/**/	lua_getglobal(state, "int_init_success");
	/**/
	/**/	if(lua_isfunction(state, -1)) {
	/**/		_lua_iot->pFuncInitSuccess = (void *) state;
	/**/		lua_pop(state, 1);
	/**/	}
	/**/	else {
	/**/		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not load Python function `int_init_success`\n");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	lua_getglobal(state, "int_init_failure");
	/**/
	/**/	if(lua_isfunction(state, -1)) {
	/**/		_lua_iot->pFuncInitFailure = (void *) state;
	/**/		lua_pop(state, 1);
	/**/	}
	/**/	else {
	/**/		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not load Python function `int_init_failure`\n");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	lua_getglobal(state, "int_loop");
	/**/
	/**/	if(lua_isfunction(state, -1)) {
	/**/		_lua_iot->pFuncLoop = (void *) state;
	/**/		lua_pop(state, 1);
	/**/	}
	/**/	else {
	/**/		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not load Python function `int_loop`\n");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	lua_getglobal(state, "iot_connection_opened");
	/**/
	/**/	if(lua_isfunction(state, -1)) {
	/**/		_lua_iot->pFuncConnectionOpened = (void *) state;
	/**/		lua_pop(state, 1);
	/**/	}
	/**/	else {
	/**/		iot_log(IOT_LOG_TYPE_DEBUG, "Cannot not load Python function `iot_connection_opened`\n");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	lua_getglobal(state, "iot_connection_lost");
	/**/
	/**/	if(lua_isfunction(state, -1)) {
	/**/		_lua_iot->pFuncConnectionLost = (void *) state;
	/**/		lua_pop(state, 1);
	/**/	}
	/**/	else {
	/**/		iot_log(IOT_LOG_TYPE_DEBUG, "Cannot not load Python function `iot_connection_lost`\n");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	lua_getglobal(state, "int_message");
	/**/
	/**/	if(lua_isfunction(state, -1)) {
	/**/		_lua_iot->pFuncMessage = (void *) state;
	/**/		lua_pop(state, 1);
	/**/	}
	/**/	else {
	/**/		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not load Python function `int_message`\n");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	lua_getglobal(state, "iot_delivery");
	/**/
	/**/	if(lua_isfunction(state, -1)) {
	/**/		_lua_iot->pFuncDelivery = (void *) state;
	/**/		lua_pop(state, 1);
	/**/	}
	/**/	else {
	/**/		iot_log(IOT_LOG_TYPE_DEBUG, "Cannot not load Python function `iot_delivery`\n");
	/**/	}

	/*----------------------------------------------------------------------------------------------------------------*/
	/* INITIALIZE MQTT                                                                                                */
	/*----------------------------------------------------------------------------------------------------------------*/

	ret = iot_mqtt_initialize(
		&iot->mqtt,
		uid,
		mqtt_server_uri,
		mqtt_server_user,
		mqtt_server_pass,
		mqtt_connect_timeout,
		mqtt_disconnect_timeout,
		mqtt_log_level,
		_iot_init_success_callback,
		_iot_init_failure_callback,
		_iot_connection_opened_callback,
		_iot_connection_lost_callback,
		_iot_message_callback,
		_iot_delivery_callback
	);

	if(ret < 0)
	{
		iot_log(IOT_LOG_TYPE_ERROR, "Cannot not initialize MQTT\n");

		goto __err;
	}

	/*----------------------------------------------------------------------------------------------------------------*/
	/* MAIN LOOP                                                                                                      */
	/*----------------------------------------------------------------------------------------------------------------*/

	while(iot->mqtt.alive)
	{
		/*------------------------------------------------------------------------------------------------------------*/

		usleep(1000 * sleep_ms);

		/*------------------------------------------------------------------------------------------------------------*/

		iot_mqtt_lock(&iot->mqtt);

		/**/	lua_getglobal(state, "int_loop");
		/**/
		/**/	lua_pushboolean(state, iot_mqtt_is_connected(&iot->mqtt));
		/**/
		/**/	if(lua_pcall(state, 1, 0, 0) != LUA_OK)
		/**/	{
		/**/		iot_log(IOT_LOG_TYPE_OOOPS, "Error running function `int_loop`: %s\n", lua_tostring(state, -1));
		/**/	}
		/**/
		/**/	lua_pop(state, lua_gettop(state));

		iot_mqtt_unlock(&iot->mqtt);

		/*------------------------------------------------------------------------------------------------------------*/
	}

	/*----------------------------------------------------------------------------------------------------------------*/
	/* FINALIZE MQTT                                                                                                  */
	/*----------------------------------------------------------------------------------------------------------------*/

	if(iot_mqtt_finalize(&iot->mqtt) < 0)
	{
		iot_log(IOT_LOG_TYPE_ERROR, "Cannot not finalize MQTT\n");
	}

	/*----------------------------------------------------------------------------------------------------------------*/
	/* FINALIZE LUA                                                                                                   */
	/*----------------------------------------------------------------------------------------------------------------*/
__err:
	lua_close(state);

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
