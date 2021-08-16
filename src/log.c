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

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

/*--------------------------------------------------------------------------------------------------------------------*/

int_t _iot_vlog(iot_log_type_t type, STR_t file_name, int_t file_line, STR_t format, va_list ap)
{
	pthread_mutex_lock(&_mutex);

	/**/	switch(type)
	/**/	{
	/**/		case IOT_LOG_TYPE_DEBUG:
	/**/			fprintf(stderr, "DEBUG, %s:%d - ", file_name, file_line);
	/**/			break;
	/**/
	/**/		case IOT_LOG_TYPE_OOOPS:
	/**/			fprintf(stderr, "OOOPS, %s:%d - ", file_name, file_line);
	/**/			break;
	/**/
	/**/		case IOT_LOG_TYPE_ERROR:
	/**/			fprintf(stderr, "ERROR, %s:%d - ", file_name, file_line);
	/**/			break;
	/**/
	/**/		case IOT_LOG_TYPE_FATAL:
	/**/			fprintf(stderr, "FATAL, %s:%d - ", file_name, file_line);
	/**/			break;
	/**/
	/**/		default:
	/**/			pthread_mutex_unlock(&_mutex);
	/**/
	/**/			return -EINVAL;
	/**/	}
	/**/
	/**/	vfprintf(stderr, format, ap);

	pthread_mutex_unlock(&_mutex);

	if(type == IOT_LOG_TYPE_FATAL)
	{
		_exit(1);
	}

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int_t _iot_log(iot_log_type_t type, STR_t file_name, int_t file_line, STR_t format, ...)
{
	va_list ap;

	va_start(ap, format);
	int_t ret = _iot_vlog(type, file_name, file_line, format, ap);
	va_end(ap);

	return ret;
}

/*--------------------------------------------------------------------------------------------------------------------*/
