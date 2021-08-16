/* Author : Jerome ODIER
 * Email : jerome@odier.xyz
 *
 * Version : 1.0 (2016-![VALUE YEAR])
 *
 *
 * This file is part of WOMBAT-IOT.
 *
 *
 * odier.io RESTRICTED
 * ___________________
 *
 *  odier.io
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of odier.io and its suppliers, if any.
 * The intellectual and technical concepts contained herein are
 * proprietary to odier.io and its suppliers and are protected by
 * trade secret or copyright law. Dissemination of this information
 * or reproduction of this material is strictly forbidden unless
 * prior written permission is obtained from odier.io.
 */

/*--------------------------------------------------------------------------------------------------------------------*/

#include <string.h>

/*--------------------------------------------------------------------------------------------------------------------*/

#include "../include/MQTTAsync.h"

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static void _trace_callback(enum MQTTASYNC_TRACE_LEVELS level, str_t message)
{
	switch(level)
	{
		case MQTTASYNC_TRACE_FATAL:
			iot_log(IOT_LOG_TYPE_FATAL, "%s\n", message);
			break;

		case MQTTASYNC_TRACE_ERROR:
			iot_log(IOT_LOG_TYPE_ERROR, "%s\n", message);
			break;

		case MQTTASYNC_TRACE_SEVERE:
			iot_log(IOT_LOG_TYPE_ERROR, "%s\n", message);
			break;

		default:
			iot_log(IOT_LOG_TYPE_DEBUG, "%s\n", message);
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
		mqtt->init_success_callback(mqtt, mqtt->iot_uid != NULL ? mqtt->iot_uid : "unknown");
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
		mqtt->init_failure_callback(mqtt, response->message != NULL ? response->message : "unknown");
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
		mqtt->connection_opened_callback(mqtt, message != NULL ? message : "unknown");
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
		mqtt->connection_lost_callback(mqtt, message != NULL ? message : "unknown");
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

	int_t result = mqtt->message_callback != NULL ? mqtt->message_callback(mqtt, topiclen == 0 ? (int_t) strlen(topic) : topiclen, topic, message->payloadlen, message->payload) : 1;

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
		mqtt->delivery_callback(mqtt, token);
	}
	else
	{
		iot_log(IOT_LOG_TYPE_DEBUG, "Message delivery confirmed, token: %d \n", token);
	}

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_initialize(iot_mqtt_t *mqtt, STR_t iot_uid, STR_t server_uri, STR_t server_user, STR_t server_pass, int_t connect_timeout, int_t disconnect_timeout, int_t log_level, iot_mqtt_init_success_callback_t init_success_callback, iot_mqtt_init_failure_callback_t init_failure_callback, iot_mqtt_connection_opened_callback_t connection_opened_callback, iot_mqtt_connection_lost_callback_t connection_lost_callback, iot_mqtt_message_callback_t message_callback, iot_mqtt_delivery_callback_t delivery_callback)
{
	int_t ret;

	/*----------------------------------------------------------------------------------------------------------------*/

	mqtt->alive = 0x01;
	mqtt->client = NULL;

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

	MQTTAsync_setTraceLevel(log_level);

	MQTTAsync_setTraceCallback(_trace_callback);

	/*----------------------------------------------------------------------------------------------------------------*/
 
	ret = MQTTAsync_create(&mqtt->client, server_uri, iot_uid, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	if(ret != MQTTASYNC_SUCCESS)
	{
		mqtt->alive = 0x00;

		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = MQTTAsync_setCallbacks(mqtt->client, mqtt, _connection_lost_callback, _message_callback, _delivery_callback);

	if(ret != MQTTASYNC_SUCCESS)
	{
		MQTTAsync_destroy(&mqtt->client);

		mqtt->alive = 0x00;

		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	ret = MQTTAsync_setConnectedCallback(mqtt->client, mqtt, _connection_opened_callback);

	if(ret != MQTTASYNC_SUCCESS)
	{
		MQTTAsync_destroy(&mqtt->client);

		mqtt->alive = 0x00;

		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_SSLOptions ssl_options = MQTTAsync_SSLOptions_initializer;

	ssl_options.verify = 1;

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_connectOptions connect_options = MQTTAsync_connectOptions_initializer;

	connect_options.automaticReconnect = 0x0000000000001;
	connect_options.connectTimeout = connect_timeout;
	connect_options.cleansession = 0;

	connect_options.ssl = &ssl_options;

	if(server_user != NULL
	   &&
	   server_pass != NULL
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

		mqtt->alive = 0x00;

		return -1;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_finalize(iot_mqtt_t *mqtt)
{
	int_t result;

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_disconnectOptions disconnect_options = MQTTAsync_disconnectOptions_initializer;

	disconnect_options.timeout = mqtt->disconnect_timeout;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(MQTTAsync_disconnect(mqtt->client, &disconnect_options) != MQTTASYNC_SUCCESS)
	{
		result = -1;
	}
	else
	{
		result = 0;
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_destroy(&mqtt->client);

	/*----------------------------------------------------------------------------------------------------------------*/

	mqtt->alive = 0;

	/*----------------------------------------------------------------------------------------------------------------*/

	return result;
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

int_t iot_mqtt_unsubscribe(iot_mqtt_t *mqtt, STR_t topic)
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

struct _mqtt_send_callback_context_s
{
	void *context;

	iot_mqtt_t *mqtt;

	iot_mqtt_message_success_callback_t message_success_callback;

	iot_mqtt_message_failure_callback_t message_failure_callback;
};

/*--------------------------------------------------------------------------------------------------------------------*/

static void _mqtt_message_success_callback(void *context, MQTTAsync_successData *response)
{
	struct _mqtt_send_callback_context_s *send_callback_context = (struct _mqtt_send_callback_context_s *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(send_callback_context->message_success_callback != NULL)
	{
		send_callback_context->message_success_callback(send_callback_context->context, send_callback_context->mqtt, response->token);
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_free(send_callback_context);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _mqtt_message_failure_callback(void *context, MQTTAsync_failureData *response)
{
	struct _mqtt_send_callback_context_s *send_callback_context = (struct _mqtt_send_callback_context_s *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(send_callback_context->message_failure_callback != NULL)
	{
		send_callback_context->message_failure_callback(send_callback_context->context, send_callback_context->mqtt, response->message != NULL ? response->message : "unknown");
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_free(send_callback_context);
}

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_mqtt_send(iot_mqtt_t *mqtt, iot_mqtt_message_success_callback_t message_success_callback, iot_mqtt_message_failure_callback_t message_failure_callback, void *context, STR_t topic, size_t payload_size, BUFF_t payload_buff, int_t qos, int_t retained)
{
	int_t ret;

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_message message = MQTTAsync_message_initializer;

	message.payloadlen = (int_t) payload_size;
	message.payload = (buff_t) payload_buff;
	message./**/qos/**/ = /**/qos/**/;
	message.retained = retained;

	/*----------------------------------------------------------------------------------------------------------------*/

	struct _mqtt_send_callback_context_s *send_callback_context = (struct _mqtt_send_callback_context_s *) iot_malloc(sizeof(struct _mqtt_send_callback_context_s));

	send_callback_context->context = context;
	send_callback_context->mqtt    = mqtt   ;

	send_callback_context->message_success_callback = message_success_callback;
	send_callback_context->message_failure_callback = message_failure_callback;

	/*----------------------------------------------------------------------------------------------------------------*/

	MQTTAsync_responseOptions response_options = MQTTAsync_responseOptions_initializer;

	response_options.context = send_callback_context;

	response_options.onSuccess = _mqtt_message_success_callback;
	response_options.onFailure = _mqtt_message_failure_callback;

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
