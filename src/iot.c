/* Author : Jerome ODIER
 * Email : jerome@odier.xyz
 *
 * Version : 1.0 (2016-![VALUE YEAR])
 *
 *
 * This file is part of WOMBAT-IOT.
 *
 *
 * Batkens RESTRICTED
 * __________________
 *
 *  Batkens
 *  All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Batkens and its suppliers, if any.
 * The intellectual and technical concepts contained herein are
 * proprietary to Batkens and its suppliers and are protected by
 * trade secret or copyright law. Dissemination of this information
 * or reproduction of this material is strictly forbidden unless
 * prior written permission is obtained from Batkens.
 */

/*--------------------------------------------------------------------------------------------------------------------*/

#include <Python.h>
#include <string.h>
#include <unistd.h>

#include <frameobject.h>

#define PyFrame_GetFileName(x) \
			PyUnicode_AsUTF8((x)->f_code->co_filename)

/*--------------------------------------------------------------------------------------------------------------------*/

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static iot_t *_python_iot;

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_stop_service(PyObject *self, PyObject *args)
{
	iot_log(IOT_LOG_TYPE_DEBUG, "Stopping service `" IOT_NAME "-iot`...\n");

	system("systemctl stop " IOT_NAME "-iot");

	Py_INCREF(Py_None);

	return Py_None;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_restart_service(PyObject *self, PyObject *args)
{
	iot_log(IOT_LOG_TYPE_DEBUG, "Restarting service `" IOT_NAME "-iot`...\n");

	system("systemctl restart " IOT_NAME "-iot");

	Py_INCREF(Py_None);

	return Py_None;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_get_uid(PyObject *self, PyObject *args)
{
	return PyUnicode_DecodeUTF8(_python_iot->uid, strlen(_python_iot->uid), "strict");
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_get_descr(PyObject *self, PyObject *args)
{
	return PyUnicode_DecodeUTF8(_python_iot->descr, strlen(_python_iot->descr), "strict");
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_config_get_str(PyObject *self, PyObject *args)
{
	STR_t key;
	STR_t val = ("");

	return Py_BuildValue(
		"s",
		PyArg_ParseTuple(args, "s|s", &key, &val) ? iot_config_get_str(_python_iot->config, key, val) : val
	);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_config_get_int(PyObject *self, PyObject *args)
{
	STR_t key;
	int_t val = 0x00;

	return Py_BuildValue(
		"i",
		PyArg_ParseTuple(args, "s|i", &key, &val) ? iot_config_get_int(_python_iot->config, key, val) : val
	);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_config_get_flt(PyObject *self, PyObject *args)
{
	STR_t key;
	float val = 0.0f;

	return Py_BuildValue(
		"f",
		PyArg_ParseTuple(args, "s|f", &key, &val) ? iot_config_get_flt(_python_iot->config, key, val) : val
	);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_log_debug(PyObject *self, PyObject *args)
{
	STR_t message;

	if(PyArg_ParseTuple(args, "s", &message))
	{
		PyFrameObject *pFrameObject = PyEval_GetFrame();

		STR_t file_name = PyFrame_GetFileName(pFrameObject);
		int_t line_number = PyFrame_GetLineNumber(pFrameObject);

		_iot_log(IOT_LOG_TYPE_DEBUG, file_name, line_number, "%s\n", message);
	}

	Py_INCREF(Py_None);

	return Py_None;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_log_ooops(PyObject *self, PyObject *args)
{
	STR_t message;

	if(PyArg_ParseTuple(args, "s", &message))
	{
		PyFrameObject *pFrameObject = PyEval_GetFrame();

		STR_t file_name = PyFrame_GetFileName(pFrameObject);
		int_t line_number = PyFrame_GetLineNumber(pFrameObject);

		_iot_log(IOT_LOG_TYPE_OOOPS, file_name, line_number, "%s\n", message);
	}

	Py_INCREF(Py_None);

	return Py_None;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_log_error(PyObject *self, PyObject *args)
{
	STR_t message;

	if(PyArg_ParseTuple(args, "s", &message))
	{
		PyFrameObject *pFrameObject = PyEval_GetFrame();

		STR_t file_name = PyFrame_GetFileName(pFrameObject);
		int_t line_number = PyFrame_GetLineNumber(pFrameObject);

		_iot_log(IOT_LOG_TYPE_ERROR, file_name, line_number, "%s\n", message);
	}

	Py_INCREF(Py_None);

	return Py_None;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_log_fatal(PyObject *self, PyObject *args)
{
	STR_t message;

	if(PyArg_ParseTuple(args, "s", &message))
	{
		PyFrameObject *pFrameObject = PyEval_GetFrame();

		STR_t file_name = PyFrame_GetFileName(pFrameObject);
		int_t line_number = PyFrame_GetLineNumber(pFrameObject);

		_iot_log(IOT_LOG_TYPE_FATAL, file_name, line_number, "%s\n", message);
	}

	Py_INCREF(Py_None);

	return Py_None;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_mqtt_subscribe(PyObject *self, PyObject *args)
{
	STR_t topic = "unknown";
	int_t qos;

	if(PyArg_ParseTuple(args, "si", &topic, &qos) != 0 && iot_mqtt_subscribe(
		&_python_iot->mqtt,
		topic,
		qos
	 ) >= 0) {
		Py_INCREF(Py_None);

		return Py_None;
	}
	else
	{
		return PyErr_Format(PyExc_IOError, "Cannot not subscribe to topic '%s'", topic);
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_mqtt_unsubscribe(PyObject *self, PyObject *args)
{
	STR_t topic = "unknown";
	/*------*/

	if(PyArg_ParseTuple(args, "s", &topic) != 0 && iot_mqtt_unsubscribe(
		&_python_iot->mqtt,
		topic
		/**/
	 ) >= 0) {
		Py_INCREF(Py_None);

		return Py_None;
	}
	else
	{
		return PyErr_Format(PyExc_IOError, "Cannot not unsubscribe to topic '%s'", topic);
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

struct _iot_mqtt_send_callback_context_s
{
	PyObject *success_callback;

	PyObject *failure_callback;
};

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_mqtt_message_success_callback(void *context, iot_mqtt_t *mqtt, int_t token)
{
	struct _iot_mqtt_send_callback_context_s *mqtt_send_callback_context = (struct _iot_mqtt_send_callback_context_s *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(mqtt_send_callback_context->success_callback != NULL)
	{
		PyObject *pParam = PyLong_FromLong(token);

		if(pParam != NULL)
		{
			PyObject *pResult = PyObject_CallFunctionObjArgs(mqtt_send_callback_context->success_callback, pParam, NULL);

			if(pResult != NULL)
			{
				Py_DECREF(pResult);
			}

			Py_DECREF(pParam);
		}
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_free(mqtt_send_callback_context);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_mqtt_message_failure_callback(void *context, iot_mqtt_t *mqtt, STR_t message)
{
	struct _iot_mqtt_send_callback_context_s *mqtt_send_callback_context = (struct _iot_mqtt_send_callback_context_s *) context;

	/*----------------------------------------------------------------------------------------------------------------*/

	if(mqtt_send_callback_context->failure_callback != NULL)
	{
		PyObject *pParam = PyUnicode_DecodeUTF8(message, strlen(message), "strict");

		if(pParam != NULL)
		{
			PyObject *pResult = PyObject_CallFunctionObjArgs(mqtt_send_callback_context->failure_callback, pParam, NULL);

			if(pResult != NULL)
			{
				Py_DECREF(pResult);
			}

			Py_DECREF(pParam);
		}
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_free(mqtt_send_callback_context);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_mqtt_send(PyObject *self, PyObject *args, PyObject *kwargs)
{
	STR_t topic;
	BUFF_t payload_buff;
	int_t payload_size;
	int_t /**/qos/**/;
	int_t retained;
	PyObject *success_callback = NULL;
	PyObject *failure_callback = NULL;

	static char *arg_names[] = {"topic", "payload", "qos", "retained", "success_callback", "failure_callback", NULL};

	if(PyArg_ParseTupleAndKeywords(args, kwargs, "ss#ii|OO", arg_names, &topic, &payload_buff, &payload_size, &/**/qos/**/, &retained, &success_callback, &failure_callback) != 0)
	{
		/*------------------------------------------------------------------------------------------------------------*/

		struct _iot_mqtt_send_callback_context_s *mqtt_send_callback_context = (struct _iot_mqtt_send_callback_context_s *) iot_malloc(sizeof(struct _iot_mqtt_send_callback_context_s));

		mqtt_send_callback_context->success_callback = success_callback;
		mqtt_send_callback_context->failure_callback = failure_callback;

		/*------------------------------------------------------------------------------------------------------------*/

		if(iot_mqtt_send(
			&_python_iot->mqtt,
			_iot_mqtt_message_success_callback,
			_iot_mqtt_message_failure_callback,
			mqtt_send_callback_context,
			topic,
			payload_size,
			payload_buff,
			/**/qos/**/,
			retained
		 ) >= 0) {
			Py_INCREF(Py_None);

			return Py_None;
		}
		else
		{
			return PyErr_Format(PyExc_IOError, "Cannot not send message");
		}
	}
	else
	{
		return PyErr_Format(PyExc_IOError, "Cannot not send message");
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyMethodDef _iot_builtins_functions[] = {
	{"iot_stop_service", (PyCFunction) _iot_stop_service, METH_VARARGS, ""},
	{"iot_restart_service", (PyCFunction) _iot_restart_service, METH_VARARGS, ""},
	/**/
	{"iot_get_uid", (PyCFunction) _iot_get_uid, METH_VARARGS, ""},
	{"iot_get_descr", (PyCFunction) _iot_get_descr, METH_VARARGS, ""},
	/**/
	{"iot_config_get_str", (PyCFunction) _iot_config_get_str, METH_VARARGS, ""},
	{"iot_config_get_int", (PyCFunction) _iot_config_get_int, METH_VARARGS, ""},
	{"iot_config_get_flt", (PyCFunction) _iot_config_get_flt, METH_VARARGS, ""},
	/**/
	{"iot_log_debug", (PyCFunction) _iot_log_debug, METH_VARARGS, ""},
	{"iot_log_ooops", (PyCFunction) _iot_log_ooops, METH_VARARGS, ""},
	{"iot_log_error", (PyCFunction) _iot_log_error, METH_VARARGS, ""},
	{"iot_log_fatal", (PyCFunction) _iot_log_fatal, METH_VARARGS, ""},
	/**/
	{"iot_mqtt_subscribe", (PyCFunction) _iot_mqtt_subscribe, METH_VARARGS, ""},
	{"iot_mqtt_unsubscribe", (PyCFunction) _iot_mqtt_unsubscribe, METH_VARARGS, ""},
	/**/
	{"iot_mqtt_send", (PyCFunction) _iot_mqtt_send, METH_VARARGS | METH_KEYWORDS, ""},
	/**/
	{NULL, NULL, 0x00, NULL},
};

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_init_success_callback(iot_mqtt_t *mqtt, STR_t iot_uid)
{
	if(_python_iot->pFuncInitSuccess != NULL)
	{
		PyObject *pParam = PyUnicode_DecodeUTF8(iot_uid, strlen(iot_uid), "strict");

		if(pParam != NULL)
		{
			PyObject *pResult = PyObject_CallFunctionObjArgs(_python_iot->pFuncInitSuccess, pParam, NULL);

			if(pResult != NULL)
			{
				Py_DECREF(pResult);
			}

			Py_DECREF(pParam);
		}
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_init_failure_callback(iot_mqtt_t *mqtt, STR_t message)
{
	if(_python_iot->pFuncInitFailure != NULL)
	{
		PyObject *pParam = PyUnicode_DecodeUTF8(message, strlen(message), "strict");

		if(pParam != NULL)
		{
			PyObject *pResult = PyObject_CallFunctionObjArgs(_python_iot->pFuncInitFailure, pParam, NULL);

			if(pResult != NULL)
			{
				Py_DECREF(pResult);
			}

			Py_DECREF(pParam);
		}
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_connection_opened_callback(iot_mqtt_t *mqtt, STR_t message)
{
	if(_python_iot->pFuncConnectionOpened != NULL)
	{
		PyObject *pParam = PyUnicode_DecodeUTF8(message, strlen(message), "strict");

		if(pParam != NULL)
		{
			PyObject *pResult = PyObject_CallFunctionObjArgs(_python_iot->pFuncConnectionOpened, pParam, NULL);

			if(pResult != NULL)
			{
				Py_DECREF(pResult);
			}

			Py_DECREF(pParam);
		}
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_connection_lost_callback(iot_mqtt_t *mqtt, STR_t message)
{
	if(_python_iot->pFuncConnectionLost != NULL)
	{
		PyObject *pParam = PyUnicode_DecodeUTF8(message, strlen(message), "strict");

		if(pParam != NULL)
		{
			PyObject *pResult = PyObject_CallFunctionObjArgs(_python_iot->pFuncConnectionLost, pParam, NULL);

			if(pResult != NULL)
			{
				Py_DECREF(pResult);
			}

			Py_DECREF(pParam);
		}
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int_t _iot_message_callback(iot_mqtt_t *mqtt, size_t topic_size, STR_t topic_str, size_t payload_size, BUFF_t payload_buff)
{
	if(_python_iot->pFuncMessage != NULL)
	{
		PyObject *pParam1 = PyUnicode_DecodeUTF8(topic_str, topic_size, "strict");

		if(pParam1 != NULL)
		{
			PyObject *pParam2 = PyUnicode_DecodeUTF8(payload_buff, payload_size, "strict");

			if(pParam2 != NULL)
			{
				PyObject *pResult = PyObject_CallFunctionObjArgs(_python_iot->pFuncMessage, pParam1, pParam2, NULL);

				if(pResult != NULL)
				{
					if(PyLong_Check(pResult))
					{
						return PyLong_AsLong(pResult);
					}

					Py_DECREF(pResult);
				}

				Py_DECREF(pParam2);
			}

			Py_DECREF(pParam1);
		}
	}

	return 1;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void _iot_delivery_callback(iot_mqtt_t *mqtt, int_t token)
{
	if(_python_iot->pFuncDelivery != NULL)
	{
		PyObject *pParam = PyLong_FromLong(token);

		if(pParam != NULL)
		{
			PyObject *pResult = PyObject_CallFunctionObjArgs(_python_iot->pFuncDelivery, pParam, NULL);

			if(pResult != NULL)
			{
				Py_DECREF(pResult);
			}

			Py_DECREF(pParam);
		}
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

	_python_iot = iot;

	/*----------------------------------------------------------------------------------------------------------------*/
	/* INITIALIZE PYTHON                                                                                              */
	/*----------------------------------------------------------------------------------------------------------------*/

	Py_Initialize();

	/*----------------------------------------------------------------------------------------------------------------*/

	PyObject *pBuiltinsModule = PyImport_ImportModule("builtins");

	PyModule_AddFunctions(pBuiltinsModule, _iot_builtins_functions);

	Py_DECREF(pBuiltinsModule);

	/*----------------------------------------------------------------------------------------------------------------*/

	FILE *fp = fopen(script_fname, "rt");

	if(fp == NULL)
	{
		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not load Python script `%s`\n", script_fname);
	}

	ret = PyRun_SimpleFileEx(fp, script_fname, 1);

	if(ret < 0)
	{
		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not execute Python script `%s`\n", script_fname);
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	PyRun_SimpleString(
		"#############################################################################"						"\n"
		""																									"\n"
		"import json"																						"\n"
		""																									"\n"
		"#############################################################################"						"\n"
		""																									"\n"
		"def int_init_success(iot_uid):"																	"\n"
		""																									"\n"
		"	try:"																							"\n"
		""																									"\n"
		"		iot_init_success('Hi! I\\'m %s.' % iot_uid)"												"\n"
		""																									"\n"
		"	except Exception as e:"																			"\n"
		""																									"\n"
		"		iot_log_ooops(e.__str__())"																	"\n"
		""																									"\n"
		"#############################################################################"						"\n"
		""																									"\n"
		"def int_init_failure(message):"																	"\n"
		""																									"\n"
		"	try:"																							"\n"
		""																									"\n"
		"		iot_init_failure(message)"																	"\n"
		""																									"\n"
		"	except Exception as e:"																			"\n"
		""																									"\n"
		"		iot_log_ooops(e.__str__())"																	"\n"
		""																									"\n"
		"#############################################################################"						"\n"
		""																									"\n"
		"def int_loop(connected):"																			"\n"
		""																									"\n"
		"	try:"																							"\n"
		""																									"\n"
		"		return iot_loop(connected)"																	"\n"
		""																									"\n"
		"	except Exception as e:"																			"\n"
		""																									"\n"
		"		iot_log_ooops(e.__str__())"																	"\n"
		""																									"\n"
		"#############################################################################"						"\n"
		""																									"\n"
		"def int_message(topic, payload):"																	"\n"
		""																									"\n"
		"	try:"																							"\n"
		""																									"\n"
		"		if topic == '%s/cmd' % iot_get_uid():"														"\n"
		""																									"\n"
		"			_json = json.loads(payload)"															"\n"
		""																									"\n"
		"			_type = _json.get('type', '')"															"\n"
		""																									"\n"
		"			##################################################################"						"\n"
		""																									"\n"
		"			if   _type == 'stop':"																	"\n"
		""																									"\n"
		"				iot_stop_service()"																	"\n"
		""																									"\n"
		"			##################################################################"						"\n"
		""																									"\n"
		"			elif _type == 'restart':"																"\n"
		""																									"\n"
		"				iot_restart_service()"																"\n"
		""																									"\n"
		"			##################################################################"						"\n"
		""																									"\n"
		"			else:"																					"\n"
		""																									"\n"
		"				return iot_message(topic, _json)"													"\n"
		""																									"\n"
		"		else:"																						"\n"
		""																									"\n"
		"			return iot_message(topic, payload)"														"\n"
		""																									"\n"
		"	except Exception as e:"																			"\n"
		""																									"\n"
		"		iot_log_error(e.__str__())"																	"\n"
		""																									"\n"
		"#############################################################################"						"\n"
	);

	/*----------------------------------------------------------------------------------------------------------------*/

	PyObject *pMainModule = PyImport_ImportModule("__main__");

	if(pMainModule == NULL)
	{
		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not import Python module `__main__`\n");
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	/**/	if(PyObject_HasAttrString(pMainModule, "int_init_success"))
	/**/	{
	/**/		iot->pFuncInitSuccess = PyObject_GetAttrString(pMainModule, "int_init_success");
	/**/	}
	/**/	else
	/**/	{
	/**/		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not load Python function `int_init_success`\n");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "int_init_failure"))
	/**/	{
	/**/		iot->pFuncInitFailure = PyObject_GetAttrString(pMainModule, "int_init_failure");
	/**/	}
	/**/	else
	/**/	{
	/**/		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not load Python function `int_init_failure`\n");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "int_loop"))
	/**/	{
	/**/		iot->pFuncLoop = PyObject_GetAttrString(pMainModule, "int_loop");
	/**/	}
	/**/	else
	/**/	{
	/**/		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not load Python function `int_loop`\n");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "iot_connection_opened"))
	/**/	{
	/**/		iot->pFuncConnectionOpened = PyObject_GetAttrString(pMainModule, "iot_connection_opened");
	/**/	}
	/**/	else
	/**/	{
	/**/		iot_log(IOT_LOG_TYPE_DEBUG, "Cannot not load Python function `iot_connection_opened`\n");
	/**/	}
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "iot_connection_lost"))
	/**/	{
	/**/		iot->pFuncConnectionLost = PyObject_GetAttrString(pMainModule, "iot_connection_lost");
	/**/	}
	/**/	else
	/**/	{
	/**/		iot_log(IOT_LOG_TYPE_DEBUG, "Cannot not load Python function `iot_connection_lost`\n");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "int_message"))
	/**/	{
	/**/		iot->pFuncMessage = PyObject_GetAttrString(pMainModule, "int_message");
	/**/	}
	/**/	else
	/**/	{
	/**/		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not load Python function `int_message`\n");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "iot_delivery"))
	/**/	{
	/**/		iot->pFuncDelivery = PyObject_GetAttrString(pMainModule, "iot_delivery");
	/**/	}
	/**/	else
	/**/	{
	/**/		iot_log(IOT_LOG_TYPE_DEBUG, "Cannot not load Python function `iot_delivery`\n");
	/**/	}
	/**/

	/*----------------------------------------------------------------------------------------------------------------*/

	Py_DECREF(pMainModule);

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
		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not initialize MQTT\n");
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
			PyObject *pParam = PyBool_FromLong(iot_mqtt_is_connected(&iot->mqtt));

			if(pParam != NULL)
			{
				PyObject *pResult = PyObject_CallFunctionObjArgs(_python_iot->pFuncLoop, pParam, NULL);

				if(pResult != NULL)
				{
					Py_DECREF(pResult);
				}

				Py_DECREF(pParam);
			}
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
	/* FINALIZE PYTHON                                                                                                */
	/*----------------------------------------------------------------------------------------------------------------*/

	if(iot->pFuncDelivery != NULL)
	{
		Py_XDECREF(iot->pFuncDelivery);
	}

	if(iot->pFuncMessage != NULL)
	{
		Py_XDECREF(iot->pFuncMessage);
	}

	if(iot->pFuncConnectionLost != NULL)
	{
		Py_XDECREF(iot->pFuncConnectionLost);
	}

	if(iot->pFuncConnectionOpened != NULL)
	{
		Py_XDECREF(iot->pFuncConnectionOpened);
	}

	if(iot->pFuncLoop != NULL)
	{
		Py_XDECREF(iot->pFuncLoop);
	}

	if(iot->pFuncInitFailure != NULL)
	{
		Py_XDECREF(iot->pFuncInitFailure);
	}

	if(iot->pFuncInitSuccess != NULL)
	{
		Py_XDECREF(iot->pFuncInitSuccess);
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	Py_Finalize();

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
