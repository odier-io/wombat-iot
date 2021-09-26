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

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static iot_log_type_t _level = IOT_LOG_TYPE_INFO;

/*--------------------------------------------------------------------------------------------------------------------*/

static pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

/*--------------------------------------------------------------------------------------------------------------------*/

int_t _iot_vlog(iot_log_type_t type, STR_t file_name, int_t file_line, STR_t format, va_list ap)
{
	pthread_mutex_lock(&_mutex);

	/**/	switch(type)
	/**/	{
	/**/		case IOT_LOG_TYPE_INFO:
	/**/			if(_level >= IOT_LOG_TYPE_INFO) {
	/**/				fprintf(stderr, "INFO, %s:%d - ", file_name, file_line);
	/**/				vfprintf(stderr, format, ap);
	/**/			}
	/**/			break;
	/**/
	/**/		case IOT_LOG_TYPE_WARNING:
	/**/			if(_level >= IOT_LOG_TYPE_WARNING) {
	/**/				fprintf(stderr, "WARNING, %s:%d - ", file_name, file_line);
	/**/				vfprintf(stderr, format, ap);
	/**/			}
	/**/			break;
	/**/
	/**/		case IOT_LOG_TYPE_ERROR:
	/**/			if(_level >= IOT_LOG_TYPE_ERROR) {
	/**/				fprintf(stderr, "ERROR, %s:%d - ", file_name, file_line);
	/**/				vfprintf(stderr, format, ap);
	/**/			}
	/**/			break;
	/**/
	/**/		case IOT_LOG_TYPE_FATAL:
	/**/			if(_level >= IOT_LOG_TYPE_FATAL) {
	/**/				fprintf(stderr, "FATAL, %s:%d - ", file_name, file_line);
	/**/				vfprintf(stderr, format, ap);
	/**/			}
	/**/			break;
	/**/
	/**/		default:
	/**/			pthread_mutex_unlock(&_mutex);
	/**/
	/**/			return -EINVAL;
	/**/	}

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
