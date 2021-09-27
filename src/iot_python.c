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

#define PY_SSIZE_T_CLEAN

/*--------------------------------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*--------------------------------------------------------------------------------------------------------------------*/

#include <Python.h>

#include <frameobject.h>

#define PyFrame_GetFileName(x) \
			PyUnicode_AsUTF8((x)->f_code->co_filename)

/*--------------------------------------------------------------------------------------------------------------------*/

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static iot_t *_python_iot;

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_service_stop(PyObject *self, PyObject *args)
{
	iot_log(IOT_LOG_TYPE_INFO, "Stopping service `" IOT_NAME "-iot`...\n");

	system("sudo systemctl stop " IOT_NAME "-iot");

	Py_INCREF(Py_None);

	return Py_None;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_service_restart(PyObject *self, PyObject *args)
{
	iot_log(IOT_LOG_TYPE_INFO, "Restarting service `" IOT_NAME "-iot`...\n");

	system("sudo systemctl restart " IOT_NAME "-iot");

	Py_INCREF(Py_None);

	return Py_None;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyObject *_iot_machine_reboot(PyObject *self, PyObject *args)
{
	iot_log(IOT_LOG_TYPE_INFO, "Rebooting machine...\n");

	system("sudo reboot");

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

		_iot_log(IOT_LOG_TYPE_INFO, file_name, line_number, "%s\n", message);
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

		_iot_log(IOT_LOG_TYPE_WARNING, file_name, line_number, "%s\n", message);
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

static PyObject *_iot_mqtt_subscribe(PyObject *self, PyObject *args, PyObject *kwargs)
{
	STR_t topic;
	int_t qos = 0;

	static str_t arg_names[] = {"topic", "qos", NULL};

	if(PyArg_ParseTupleAndKeywords(args, kwargs, "s|i", arg_names, &topic, &qos) != 0 && iot_mqtt_subscribe(
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

static PyObject *_iot_mqtt_unsubscribe(PyObject *self, PyObject *args, PyObject *kwargs)
{
	STR_t topic;
	int_t qos = 0;

	static str_t arg_names[] = {"topic", "qos", NULL};

	if(PyArg_ParseTupleAndKeywords(args, kwargs, "s|i", arg_names, &topic, &qos) != 0 && iot_mqtt_unsubscribe(
		&_python_iot->mqtt,
		topic,
		qos
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

static PyObject *_iot_mqtt_send(PyObject *self, PyObject *args, PyObject *kwargs)
{
	STR_t topic;
	STR_t payload;
	int_t /**/qos/**/ = 0;
	int_t retained = 0;

	static str_t arg_names[] = {"topic", "payload", "qos", "retained", NULL};

	if(PyArg_ParseTupleAndKeywords(args, kwargs, "ss|ipOO", arg_names, &topic, &payload, &/**/qos/**/, &retained) != 0)
	{
		if(iot_mqtt_send(
			&_python_iot->mqtt,
			topic,
			strlen(payload),
			/*--*/(payload),
			/**/qos/**/,
			retained
		 ) >= 0) {
			Py_INCREF(Py_None);

			return Py_None;
		}
		else
		{
			return PyErr_Format(PyExc_IOError, "Cannot not send message (network issue)");
		}
	}
	else
	{
		return PyErr_Format(PyExc_IOError, "Cannot not send message (bad parameters)");
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

static PyMethodDef _iot_builtins_functions[] = {
	{"iot_service_stop", (PyCFunction) _iot_service_stop, METH_VARARGS, ""},
	{"iot_service_restart", (PyCFunction) _iot_service_restart, METH_VARARGS, ""},
	{"iot_machine_reboot", (PyCFunction) _iot_machine_reboot, METH_VARARGS, ""},
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
	{"iot_mqtt_subscribe", (PyCFunction) _iot_mqtt_subscribe, METH_VARARGS | METH_KEYWORDS, ""},
	{"iot_mqtt_unsubscribe", (PyCFunction) _iot_mqtt_unsubscribe, METH_VARARGS | METH_KEYWORDS, ""},
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
	int_t result = 1;

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
						result = PyLong_AsLong(pResult);
					}

					Py_DECREF(pResult);
				}

				Py_DECREF(pParam2);
			}

			Py_DECREF(pParam1);
		}
	}

	return result;
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
		iot_log(IOT_LOG_TYPE_ERROR, "Cannot not load Python script `%s`\n", script_fname);

		goto __err1;
	}
	else
	{
		ret = PyRun_SimpleFileEx(fp, script_fname, 1);

		if(ret < 0)
		{
			iot_log(IOT_LOG_TYPE_ERROR, "Cannot not execute Python script `%s`\n", script_fname);

			goto __err1;
		}
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	PyObject *pMainModule = PyImport_ImportModule("__main__");

	if(pMainModule == NULL)
	{
		iot_log(IOT_LOG_TYPE_FATAL, "Cannot not import Python module `__main__`\n");
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	/**/	if(PyObject_HasAttrString(pMainModule, "iot_init_success"))
	/**/	{
	/**/		iot->pFuncInitSuccess = PyObject_GetAttrString(pMainModule, "iot_init_success");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "iot_init_failure"))
	/**/	{
	/**/		iot->pFuncInitFailure = PyObject_GetAttrString(pMainModule, "iot_init_failure");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "iot_loop"))
	/**/	{
	/**/		iot->pFuncLoop = PyObject_GetAttrString(pMainModule, "iot_loop");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "iot_connection_opened"))
	/**/	{
	/**/		iot->pFuncConnectionOpened = PyObject_GetAttrString(pMainModule, "iot_connection_opened");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "iot_connection_lost"))
	/**/	{
	/**/		iot->pFuncConnectionLost = PyObject_GetAttrString(pMainModule, "iot_connection_lost");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "iot_message"))
	/**/	{
	/**/		iot->pFuncMessage = PyObject_GetAttrString(pMainModule, "iot_message");
	/**/	}
	/**/
	/**/	/*--------------------------------------------------------------------------------------------------------*/
	/**/
	/**/	if(PyObject_HasAttrString(pMainModule, "iot_delivery"))
	/**/	{
	/**/		iot->pFuncDelivery = PyObject_GetAttrString(pMainModule, "iot_delivery");
	/**/	}

	/*----------------------------------------------------------------------------------------------------------------*/

	Py_DECREF(pMainModule);

	/*----------------------------------------------------------------------------------------------------------------*/

	if(iot->pFuncInitSuccess != NULL || iot->pFuncInitFailure != NULL || iot->pFuncLoop != NULL || iot->pFuncConnectionOpened != NULL || iot->pFuncConnectionLost != NULL || iot->pFuncMessage != NULL || iot->pFuncDelivery != NULL)
	{
		/*------------------------------------------------------------------------------------------------------------*/
		/* DISPLAY DEBUG INFORMATION                                                                                  */
		/*------------------------------------------------------------------------------------------------------------*/

		if(iot->pFuncInitSuccess == NULL)
		{
			iot_log(IOT_LOG_TYPE_INFO, "Cannot not find Python function `iot_init_success`\n");
		}

		if(iot->pFuncInitFailure == NULL)
		{
			iot_log(IOT_LOG_TYPE_INFO, "Cannot not find Python function `iot_init_failure`\n");
		}

		if(iot->pFuncLoop == NULL)
		{
			iot_log(IOT_LOG_TYPE_INFO, "Cannot not find Python function `iot_loop`\n");
		}

		if(iot->pFuncConnectionOpened == NULL)
		{
			iot_log(IOT_LOG_TYPE_INFO, "Cannot not find Python function `iot_connection_opened`\n");
		}

		if(iot->pFuncConnectionLost == NULL)
		{
			iot_log(IOT_LOG_TYPE_INFO, "Cannot not find Python function `iot_connection_lost`\n");
		}

		if(iot->pFuncMessage == NULL)
		{
			iot_log(IOT_LOG_TYPE_INFO, "Cannot not find Python function `iot_message`\n");
		}

		if(iot->pFuncDelivery == NULL)
		{
			iot_log(IOT_LOG_TYPE_INFO, "Cannot not find Python function `iot_delivery`\n");
		}

		/*------------------------------------------------------------------------------------------------------------*/
		/* INITIALIZE MQTT                                                                                            */
		/*------------------------------------------------------------------------------------------------------------*/

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

			goto __err2;
		}

		/*------------------------------------------------------------------------------------------------------------*/
		/* MAIN LOOP                                                                                                  */
		/*------------------------------------------------------------------------------------------------------------*/

		while(iot->mqtt.alive)
		{
			/*--------------------------------------------------------------------------------------------------------*/

			usleep(1000 * sleep_ms);

			/*--------------------------------------------------------------------------------------------------------*/

			if(iot->pFuncLoop)
			{
				iot_mqtt_lock(&iot->mqtt);

				/**/	PyObject *pParam = PyBool_FromLong(iot_mqtt_is_connected(&iot->mqtt));
				/**/
				/**/	if(pParam != NULL)
				/**/	{
				/**/		PyObject *pResult = PyObject_CallFunctionObjArgs(_python_iot->pFuncLoop, pParam, NULL);
				/**/
				/**/		if(pResult != NULL)
				/**/		{
				/**/			Py_DECREF(pResult);
				/**/		}
				/**/
				/**/		Py_DECREF(pParam);
				/**/	}

				iot_mqtt_unlock(&iot->mqtt);
			}

			/*--------------------------------------------------------------------------------------------------------*/
		}

		/*------------------------------------------------------------------------------------------------------------*/
		/* FINALIZE MQTT                                                                                              */
		/*------------------------------------------------------------------------------------------------------------*/

		if(iot_mqtt_finalize(&iot->mqtt) < 0)
		{
			iot_log(IOT_LOG_TYPE_ERROR, "Cannot not finalize MQTT\n");
		}

		/*------------------------------------------------------------------------------------------------------------*/
	}

	/*----------------------------------------------------------------------------------------------------------------*/
	/* FINALIZE PYTHON                                                                                                */
	/*----------------------------------------------------------------------------------------------------------------*/
__err2:
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
__err1:
	Py_Finalize();

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

STR_t iot_lang_version(void)
{
	return "Python " PY_VERSION;
}

/*--------------------------------------------------------------------------------------------------------------------*/
