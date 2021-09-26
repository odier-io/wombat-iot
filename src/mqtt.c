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

#include <string.h>

/*--------------------------------------------------------------------------------------------------------------------*/

#include "../include/MQTTAsync.h"

/*--------------------------------------------------------------------------------------------------------------------*/

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static void _trace_callback(enum MQTTASYNC_TRACE_LEVELS level, str_t message)
{
	switch(level)
	{
		case MQTTASYNC_TRACE_PROTOCOL:
			iot_log(IOT_LOG_TYPE_INFO, "%s\n", message);
			break;

		case MQTTASYNC_TRACE_ERROR:
			iot_log(IOT_LOG_TYPE_WARNING, "%s\n", message);
			break;

		case MQTTASYNC_TRACE_SEVERE:
			iot_log(IOT_LOG_TYPE_ERROR, "%s\n", message);
			break;

		case MQTTASYNC_TRACE_FATAL:
			iot_log(IOT_LOG_TYPE_FATAL, "%s\n", message);
			break;

		default:
			/* ignored */
			break;
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static void _init_success_callback(buff_t context, MQTTAsync_successData *response)
{
	iot_mqtt_t *mqtt = (iot_mqtt_t *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(mqtt->init_success_callback != NULL)
	{
		iot_mqtt_lock(mqtt);

		mqtt->init_success_callback(mqtt, mqtt->iot_uid != NULL ? mqtt->iot_uid : "unknown");

		iot_mqtt_unlock(mqtt);
	}

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static void _init_failure_callback(buff_t context, MQTTAsync_failureData *response)
{
	iot_mqtt_t *mqtt = (iot_mqtt_t *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(mqtt->init_failure_callback != NULL)
	{
		iot_mqtt_lock(mqtt);

		mqtt->init_failure_callback(mqtt, response->message != NULL ? response->message : "unknown");

		iot_mqtt_unlock(mqtt);
	}

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static void _connection_opened_callback(buff_t context, str_t message)
{
	iot_mqtt_t *mqtt = (iot_mqtt_t *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(mqtt->connection_opened_callback != NULL)
	{
		iot_mqtt_lock(mqtt);

		mqtt->connection_opened_callback(mqtt, message != NULL ? message : "unknown");

		iot_mqtt_unlock(mqtt);
	}
	else
	{
		iot_log(IOT_LOG_TYPE_ERROR, "Connection opened, cause: %s\n", message != NULL ? message : "unknown");
	}

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static void _connection_lost_callback(buff_t context, str_t message)
{
	iot_mqtt_t *mqtt = (iot_mqtt_t *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(mqtt->connection_lost_callback != NULL)
	{
		iot_mqtt_lock(mqtt);

		mqtt->connection_lost_callback(mqtt, message != NULL ? message : "unknown");

		iot_mqtt_unlock(mqtt);
	}
	else
	{
		iot_log(IOT_LOG_TYPE_ERROR, "Connection lost, cause: %s\n", message != NULL ? message : "unknown");
	}

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _message_callback(buff_t context, str_t topic, int_t topiclen, MQTTAsync_message *message)
{
	iot_mqtt_t *mqtt = (iot_mqtt_t *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	int_t result;

	if(mqtt->message_callback != NULL)
	{
		iot_mqtt_lock(mqtt);

		result = mqtt->message_callback(mqtt, topiclen == 0 ? (int_t) strlen(topic) : topiclen, topic, message->payloadlen, message->payload);

		iot_mqtt_unlock(mqtt);
	}
	else
	{
		result = 1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	if(result != 0)
	{
		MQTTAsync_freeMessage(&message);

		MQTTAsync_free(topic);
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static void _delivery_callback(buff_t context, int_t token)
{
	iot_mqtt_t *mqtt = (iot_mqtt_t *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(mqtt->delivery_callback != NULL)
	{
		iot_mqtt_lock(mqtt);

		mqtt->delivery_callback(mqtt, token);

		iot_mqtt_unlock(mqtt);
	}
	else
	{
		iot_log(IOT_LOG_TYPE_INFO, "Message delivery confirmed, token: %d \n", token);
	}

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_initialize(iot_mqtt_t *mqtt, STR_t iot_uid, STR_t server_uri, STR_t server_user, STR_t server_pass, int_t connect_timeout, int_t disconnect_timeout, int_t log_level, iot_mqtt_init_success_callback_t init_success_callback, iot_mqtt_init_failure_callback_t init_failure_callback, iot_mqtt_connection_opened_callback_t connection_opened_callback, iot_mqtt_connection_lost_callback_t connection_lost_callback, iot_mqtt_message_callback_t message_callback, iot_mqtt_delivery_callback_t delivery_callback)
{
	int_t ret;

	/*----------------------------------------------------------------------------------------------------------------*/

	memset(mqtt, 0x00, sizeof(iot_mqtt_t));

	/*----------------------------------------------------------------------------------------------------------------*/

	mqtt->iot_uid = iot_uid;
	mqtt->server_uri = server_uri;
	mqtt->server_user = server_user;
	mqtt->server_pass = server_pass;
	mqtt->connect_timeout = connect_timeout;
	mqtt->disconnect_timeout = disconnect_timeout;
	mqtt->log_level = log_level;

	mqtt->init_success_callback = init_success_callback;
	mqtt->init_failure_callback = init_failure_callback;
	mqtt->connection_lost_callback = connection_lost_callback;
	mqtt->connection_opened_callback = connection_opened_callback;
	mqtt->message_callback = message_callback;
	mqtt->delivery_callback = delivery_callback;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(pthread_mutex_init(&mqtt->mutex, NULL) != 0)
	{
		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_setTraceLevel(log_level);

	MQTTAsync_setTraceCallback(_trace_callback);

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = MQTTAsync_create(&mqtt->client, server_uri, iot_uid, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	if(ret != MQTTASYNC_SUCCESS)
	{
		pthread_mutex_destroy(&mqtt->mutex);

		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = MQTTAsync_setCallbacks(mqtt->client, mqtt, _connection_lost_callback, _message_callback, _delivery_callback);

	if(ret != MQTTASYNC_SUCCESS)
	{
		MQTTAsync_destroy(&mqtt->client);

		pthread_mutex_destroy(&mqtt->mutex);

		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = MQTTAsync_setConnectedCallback(mqtt->client, mqtt, _connection_opened_callback);

	if(ret != MQTTASYNC_SUCCESS)
	{
		MQTTAsync_destroy(&mqtt->client);

		pthread_mutex_destroy(&mqtt->mutex);

		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_SSLOptions ssl_options = MQTTAsync_SSLOptions_initializer;

	ssl_options.verify = 1;

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_connectOptions connect_options = MQTTAsync_connectOptions_initializer;

	connect_options.ssl = iot_mqtt_ssl_enabled() ? &ssl_options : NULL;

	connect_options.automaticReconnect = 0x0000000000001;
	connect_options.connectTimeout = connect_timeout;
	connect_options.cleansession = 0;

	if(server_user != NULL && strlen(server_user) > 0
	   &&
	   server_pass != NULL && strlen(server_pass) > 0
	 ) {
		connect_options.username = server_user;
		connect_options.password = server_pass;
	}

	connect_options.context = mqtt;

	connect_options.onSuccess = _init_success_callback;
	connect_options.onFailure = _init_failure_callback;

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = MQTTAsync_connect(mqtt->client, &connect_options);

	if(ret != MQTTASYNC_SUCCESS)
	{
		MQTTAsync_destroy(&mqtt->client);

		pthread_mutex_destroy(&mqtt->mutex);

		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	mqtt->alive = 0x01;

	/*----------------------------------------------------------------------------------------------------------------*/

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_finalize(iot_mqtt_t *mqtt)
{
	int_t result = 0;

	/*----------------------------------------------------------------------------------------------------------------*/

	mqtt->alive = 0;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(MQTTAsync_isConnected(mqtt->client))
	{
		/*------------------------------------------------------------------------------------------------------------*/

		MQTTAsync_disconnectOptions disconnect_options = MQTTAsync_disconnectOptions_initializer;

		disconnect_options.timeout = mqtt->disconnect_timeout;

		/*------------------------------------------------------------------------------------------------------------*/

		if(MQTTAsync_disconnect(mqtt->client, &disconnect_options) != MQTTASYNC_SUCCESS)
		{
			result = -1;
		}

		/*------------------------------------------------------------------------------------------------------------*/
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_destroy(&mqtt->client);

	/*----------------------------------------------------------------------------------------------------------------*/

	pthread_mutex_destroy(&mqtt->mutex);

	/*----------------------------------------------------------------------------------------------------------------*/

	return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_ssl_enabled(void)
{
	MQTTAsync_nameValue *nameValue;

	for(nameValue = MQTTAsync_getVersionInfo(); nameValue->name != NULL; nameValue++)
	{
		if(strcmp(nameValue->name, "OpenSSL version") == 0)
		{
			return 1;
		}
	}

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_is_connected(iot_mqtt_t *mqtt)
{
	return MQTTAsync_isConnected(mqtt->client);
}

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_subscribe(iot_mqtt_t *mqtt, STR_t topic, int_t qos)
{
	int_t ret;

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_responseOptions response_options = MQTTAsync_responseOptions_initializer;

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = MQTTAsync_subscribe(mqtt->client, topic, qos, &response_options);

	if(ret != MQTTASYNC_SUCCESS)
	{
		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_unsubscribe(iot_mqtt_t *mqtt, STR_t topic, int_t qos)
{
	int_t ret;

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_responseOptions response_options = MQTTAsync_responseOptions_initializer;

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = MQTTAsync_unsubscribe(mqtt->client, topic, &response_options);

	if(ret != MQTTASYNC_SUCCESS)
	{
		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_send(iot_mqtt_t *mqtt, STR_t topic, size_t payload_size, BUFF_t payload_buff, int_t qos, int_t retained)
{
	int_t ret;

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_message message = MQTTAsync_message_initializer;

	message.payloadlen = (int_t) payload_size;
	message.payload = (buff_t) payload_buff;
	message./**/qos/**/ = /**/qos/**/;
	message.retained = retained;

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_responseOptions response_options = MQTTAsync_responseOptions_initializer;

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = MQTTAsync_sendMessage(mqtt->client, topic, &message, &response_options);

	if(ret != MQTTASYNC_SUCCESS)
	{
		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void iot_mqtt_lock(iot_mqtt_t *mqtt)
{
	pthread_mutex_lock(&mqtt->mutex);
}

/*--------------------------------------------------------------------------------------------------------------------*/

void iot_mqtt_unlock(iot_mqtt_t *mqtt)
{
	pthread_mutex_unlock(&mqtt->mutex);
}

/*--------------------------------------------------------------------------------------------------------------------*/
