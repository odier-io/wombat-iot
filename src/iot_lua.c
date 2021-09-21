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
	int_t val = luaL_optnumber(state, 2, 0x00);

	lua_pushnumber(state, iot_config_get_int(_lua_iot->config, key, val));

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
	STR_t file_name = "?";
	int_t line_number = 0.0;

	STR_t message = luaL_checkstring(state, 1);

	_iot_log(IOT_LOG_TYPE_DEBUG, file_name, line_number, "%s\n", message);

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_log_ooops(lua_State *state)
{
	STR_t file_name = "?";
	int_t line_number = 0.0;

	STR_t message = luaL_checkstring(state, 1);

	_iot_log(IOT_LOG_TYPE_OOOPS, file_name, line_number, "%s\n", message);

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_log_error(lua_State *state)
{
	STR_t file_name = "?";
	int_t line_number = 0.0;

	STR_t message = luaL_checkstring(state, 1);

	_iot_log(IOT_LOG_TYPE_ERROR, file_name, line_number, "%s\n", message);

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_log_fatal(lua_State *state)
{
	STR_t file_name = "?";
	int_t line_number = 0.0;

	STR_t message = luaL_checkstring(state, 1);

	_iot_log(IOT_LOG_TYPE_FATAL, file_name, line_number, "%s\n", message);

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_mqtt_subscribe(lua_State *state)
{
	STR_t topic = luaL_checkstring(state, 1);
	int_t qos = luaL_optnumber(state, 2, 0);

	if(iot_mqtt_subscribe(
		&_lua_iot->mqtt,
		topic,
		qos
	 ) >= 0) {

		return 0;
	}
	else
	{
		/*return PyErr_Format(PyExc_IOError, "Cannot not subscribe to topic '%s'", topic);*/

		return 0;
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_mqtt_unsubscribe(lua_State *state)
{
	STR_t topic = luaL_checkstring(state, 1);
	int_t qos = luaL_optnumber(state, 2, 0);

	if(iot_mqtt_unsubscribe(
		&_lua_iot->mqtt,
		topic,
		qos
	 ) >= 0) {

		return 0;
	}
	else
	{
		/*return PyErr_Format(PyExc_IOError, "Cannot not unsubscribe to topic '%s'", topic);*/

		return 0;
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

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_free(mqtt_send_callback_context);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_mqtt_message_failure_callback(void *context, iot_mqtt_t *mqtt, STR_t message)
{
	struct _iot_mqtt_send_callback_context_s *mqtt_send_callback_context = (struct _iot_mqtt_send_callback_context_s *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_free(mqtt_send_callback_context);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_mqtt_send(lua_State *state)
{
	/*----------------------------------------------------------------------------------------------------------------*/

	STR_t topic = luaL_checkstring(state, 1);
	STR_t payload = luaL_checkstring(state, 2);
	int_t /**/qos/**/ = luaL_optnumber(state, 3, 0);
	int_t retained = luaL_optnumber(state, 4, 0);

	/*----------------------------------------------------------------------------------------------------------------*/

	struct _iot_mqtt_send_callback_context_s *mqtt_send_callback_context = (struct _iot_mqtt_send_callback_context_s *) iot_malloc(sizeof(struct _iot_mqtt_send_callback_context_s));

	mqtt_send_callback_context->success_callback = NULL;
	mqtt_send_callback_context->failure_callback = NULL;

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
		/*return PyErr_Format(PyExc_IOError, "Cannot not send message (network issue)");*/

		return 0;
	}

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_init_success_callback(iot_mqtt_t *mqtt, STR_t iot_uid)
{
	if(_lua_iot->pFuncInitSuccess != NULL)
	{
		/* TODO */
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_init_failure_callback(iot_mqtt_t *mqtt, STR_t message)
{
	if(_lua_iot->pFuncInitFailure != NULL)
	{
		/* TODO */
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_connection_opened_callback(iot_mqtt_t *mqtt, STR_t message)
{
	if(_lua_iot->pFuncConnectionOpened != NULL)
	{
		/* TODO */
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_connection_lost_callback(iot_mqtt_t *mqtt, STR_t message)
{
	if(_lua_iot->pFuncConnectionLost != NULL)
	{
		/* TODO */
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_message_callback(iot_mqtt_t *mqtt, size_t topic_size, STR_t topic_str, size_t payload_size, BUFF_t payload_buff)
{
	if(_lua_iot->pFuncMessage != NULL)
	{
		/* TODO */
	}

	return 1;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_delivery_callback(iot_mqtt_t *mqtt, int_t token)
{
	if(_lua_iot->pFuncDelivery != NULL)
	{
		/* TODO */
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

	lua_pushcfunction(state, _iot_service_stop);
	lua_setglobal(state, "iot_service_stop");
	lua_pushcfunction(state, _iot_service_restart);
	lua_setglobal(state, "iot_service_restart");
	lua_pushcfunction(state, _iot_machine_reboot);
	lua_setglobal(state, "iot_machine_reboot");
	lua_pushcfunction(state, _iot_get_uid);
	lua_setglobal(state, "iot_get_uid");
	lua_pushcfunction(state, _iot_get_descr);
	lua_setglobal(state, "iot_get_descr");
	lua_pushcfunction(state, _iot_config_get_str);
	lua_setglobal(state, "iot_config_get_str");
	lua_pushcfunction(state, _iot_config_get_int);
	lua_setglobal(state, "iot_config_get_int");
	lua_pushcfunction(state, _iot_config_get_flt);
	lua_setglobal(state, "iot_config_get_flt");
	lua_pushcfunction(state, _iot_log_debug);
	lua_setglobal(state, "iot_log_debug");
	lua_pushcfunction(state, _iot_log_ooops);
	lua_setglobal(state, "iot_log_ooops");
	lua_pushcfunction(state, _iot_log_error);
	lua_setglobal(state, "iot_log_error");
	lua_pushcfunction(state, _iot_log_fatal);
	lua_setglobal(state, "iot_log_fatal");
	lua_pushcfunction(state, _iot_mqtt_subscribe);
	lua_setglobal(state, "iot_mqtt_subscribe");
	lua_pushcfunction(state, _iot_mqtt_unsubscribe);
	lua_setglobal(state, "iot_mqtt_unsubscribe");
	lua_pushcfunction(state, _iot_mqtt_send);
	lua_setglobal(state, "iot_mqtt_send");

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = luaL_dostring(
		state,
		"-----------------------------------------------------------------------------"						"\n"
		""																									"\n"
		"function int_init_success(iot_uid)"																"\n"
		""																									"\n"
		"	local ok, message = pcall(iot_init_success, string.format('Hi! I\\'m %%s.', iot_get_uid()))"	"\n"
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

	if(ret > 0)
	{
		iot_log(IOT_LOG_TYPE_ERROR, "Cannot not execute internal Lua script `%s`\n", script_fname);
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = luaL_dofile(state, script_fname);

	if(ret > 0)
	{
		iot_log(IOT_LOG_TYPE_ERROR, "Cannot not execute Lua script `%s`\n", script_fname);
	}

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

		if(iot->pFuncLoop)
		{
			iot_mqtt_lock(&iot->mqtt);

			/* TODO */

			iot_mqtt_unlock(&iot->mqtt);
		}

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
