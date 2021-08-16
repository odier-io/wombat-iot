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

#ifndef __IOT_H_
#define __IOT_H_

/*--------------------------------------------------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include <sys/types.h>

/*--------------------------------------------------------------------------------------------------------------------*/

#define int_t int32_t
#define uint_t uint32_t

#define lint_t int64_t
#define ulint_t uint64_t

#define str_t /* */ char *
#define STR_t const char *

#define wstr_t /* */ wchar_t *
#define WSTR_t const wchar_t *

#define buff_t /* */ void *
#define BUFF_t const void *

/*--------------------------------------------------------------------------------------------------------------------*/

#define _EXPAND(x) \
			#x
#define __EXPAND(x) \
			_EXPAND(x)

/*--------------------------------------------------------------------------------------------------------------------*/

#include "wombat-default.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/* NAME AND VERSION                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

#define IOT_NAME "wombat"

#define IOT_VERSION "1.0.0"

/*--------------------------------------------------------------------------------------------------------------------*/
/* LOG                                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef enum iot_log_type_e
{
	IOT_LOG_TYPE_DEBUG
	,
	IOT_LOG_TYPE_OOOPS
	,
	IOT_LOG_TYPE_ERROR
	,
	IOT_LOG_TYPE_FATAL

} iot_log_type_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/* CONFIG                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct iot_config_entry_s
{
	STR_t key;
	STR_t val;

} iot_config_entry_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct iot_config_s
{
	struct iot_config_entry_s entries[64];

	uint_t nb_of_entries;

} iot_config_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/* MQTT                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

struct iot_mqtt_s;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef void (* iot_mqtt_init_success_callback_t)(
	struct iot_mqtt_s *mqtt,
	STR_t iot_uid
);

/*--------------------------------------------------------------------------------------------------------------------*/

typedef void (* iot_mqtt_init_failure_callback_t)(
	struct iot_mqtt_s *mqtt,
	STR_t message
);

/*--------------------------------------------------------------------------------------------------------------------*/

typedef void (* iot_mqtt_connection_opened_callback_t)(
	struct iot_mqtt_s *mqtt,
	STR_t message
);

/*--------------------------------------------------------------------------------------------------------------------*/

typedef void (* iot_mqtt_connection_lost_callback_t)(
	struct iot_mqtt_s *mqtt,
	STR_t message
);

/*--------------------------------------------------------------------------------------------------------------------*/

typedef int_t (* iot_mqtt_message_callback_t)(
	struct iot_mqtt_s *mqtt,
	size_t topic_size,
	STR_t topic_str,
	size_t payload_size,
	BUFF_t payload_buff
);

/*--------------------------------------------------------------------------------------------------------------------*/

typedef void (* iot_mqtt_message_success_callback_t)(
	void *context,
	struct iot_mqtt_s *mqtt,
	int_t token
);

/*--------------------------------------------------------------------------------------------------------------------*/

typedef void (* iot_mqtt_message_failure_callback_t)(
	void *context,
	struct iot_mqtt_s *mqtt,
	STR_t message
);

/*--------------------------------------------------------------------------------------------------------------------*/

typedef void (* iot_mqtt_delivery_callback_t)(
	struct iot_mqtt_s *mqtt,
	int_t token
);

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct iot_mqtt_s
{
	/*-----------------*/
	volatile int_t alive;
	/*-----------------*/

	void *client;

	STR_t iot_uid;

	STR_t server_uri;
	STR_t server_user;
	STR_t server_pass;

	int_t connect_timeout;
	int_t disconnect_timeout;

	int_t log_level;

	iot_mqtt_init_success_callback_t init_success_callback;
	iot_mqtt_init_failure_callback_t init_failure_callback;

	iot_mqtt_connection_lost_callback_t connection_lost_callback;
	iot_mqtt_connection_lost_callback_t connection_opened_callback;

	iot_mqtt_message_callback_t message_callback;
	iot_mqtt_delivery_callback_t delivery_callback;

} iot_mqtt_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/* IOT                                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct iot_s 
{
	STR_t uid;
	STR_t descr;

	struct iot_config_s *config;

	struct iot_mqtt_s mqtt;

	void *pFuncLoop;
	/**/
	void *pFuncInitSuccess;
	void *pFuncInitFailure;
	/**/
	void *pFuncConnectionOpened;
	void *pFuncConnectionLost;
	/**/
	void *pFuncMessage;
	void *pFuncDelivery;

} iot_t;

/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------------------------------------------------*/
/* STR                                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

buff_t iot_malloc(
	size_t size
);

buff_t iot_realloc(
	buff_t buff,
	size_t size
);

void iot_free(
	buff_t buff
);

/*--------------------------------------------------------------------------------------------------------------------*/

str_t iot_strdup(
	STR_t s
);

str_t iot_strip(
	str_t s
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* LOG                                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

#define iot_log(type, format, ...) \
		_iot_log(type, __FILE__, __LINE__, format, ##__VA_ARGS__)

/*--------------------------------------------------------------------------------------------------------------------*/

int_t _iot_vlog(
	enum iot_log_type_e type,
	STR_t file_name,
	int_t file_line,
	STR_t format,
	va_list ap
);

int_t _iot_log(
	enum iot_log_type_e type,
	STR_t file_name,
	int_t file_line,
	STR_t format,
	...
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* CONFIG                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

void iot_config_initialize(
	struct iot_config_s *config,
	STR_t fname
);

void iot_config_finalize(
	struct iot_config_s *config
);

/*--------------------------------------------------------------------------------------------------------------------*/

STR_t iot_config_get_str(
	struct iot_config_s *config,
	STR_t key,
	STR_t def
);

int_t iot_config_get_int(
	struct iot_config_s *config,
	STR_t key,
	int_t def
);

float iot_config_get_flt(
	struct iot_config_s *config,
	STR_t key,
	float def
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* MQTT                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_initialize(
	struct iot_mqtt_s *mqtt,
	STR_t iot_uid,
	STR_t server_uri,
	STR_t server_user, /* Nullable */
	STR_t server_pass, /* Nullable */
	int_t connect_timeout,
	int_t disconnect_timeout,
	int_t log_level,
	/**/
	iot_mqtt_init_success_callback_t init_success_callback,
	iot_mqtt_init_failure_callback_t init_failure_callback,
	/**/
	iot_mqtt_connection_opened_callback_t connection_opened_callback,
	iot_mqtt_connection_lost_callback_t connection_lost_callback,
	/**/
	iot_mqtt_message_callback_t message_callback,
	iot_mqtt_delivery_callback_t delivery_callback
);

int_t iot_mqtt_finalize(
	struct iot_mqtt_s *mqtt
);

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_is_connected(
	struct iot_mqtt_s *mqtt
);

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_subscribe(
	struct iot_mqtt_s *mqtt,
	STR_t topic,
	int_t qos
);

int_t iot_mqtt_unsubscribe(
	struct iot_mqtt_s *mqtt,
	STR_t topic
	/*-----*/
);

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_send(
	iot_mqtt_t *mqtt,
	iot_mqtt_message_success_callback_t message_success_callback, /* Nullable */
	iot_mqtt_message_failure_callback_t message_failure_callback, /* Nullable */
	void *context, /* Nullable */
	STR_t topic,
	size_t payload_size,
	BUFF_t payload_buff,
	int_t /**/qos/**/,
	int_t retained
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* IOT                                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

static inline void iot_stop(struct iot_s *iot)
{
	iot->mqtt.alive = 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void iot_loop(
	struct iot_s *iot,
	struct iot_config_s *config,
	STR_t script_fname,
	STR_t uid,
	STR_t descr,
	int_t sleep_ms,
	STR_t mqtt_server_uri,
	STR_t mqtt_server_user, /* Nullable */
	STR_t mqtt_server_pass, /* Nullable */
	int_t mqtt_connect_timeout,
	int_t mqtt_disconnect_timeout,
	int_t mqtt_log_level
);

/*--------------------------------------------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* __IOT_H_ */

/*--------------------------------------------------------------------------------------------------------------------*/
