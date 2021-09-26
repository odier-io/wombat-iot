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

#include <stdio.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>

/*--------------------------------------------------------------------------------------------------------------------*/

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static void version(void)
{
	printf("%s\n", IOT_VERSION);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void help(STR_t program_name)
{
	printf("%s [--config=<file>] [--version] [--help] <script>\n", program_name);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static iot_t iot;

/*--------------------------------------------------------------------------------------------------------------------*/

static void __exit(int_t signal)
{
	iot_log(IOT_LOG_TYPE_INFO, "Stopping " IOT_NAME "-IoT...\n");

	iot_stop(&iot);
}

/*--------------------------------------------------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
	/*----------------------------------------------------------------------------------------------------------------*/

	printf(
		"██     ██  ██████  ███    ███ ██████   █████  ████████       ██  ██████  ████████\n"
		"██     ██ ██    ██ ████  ████ ██   ██ ██   ██    ██          ██ ██    ██    ██   \n"
		"██  █  ██ ██    ██ ██ ████ ██ ██████  ███████    ██    █████ ██ ██    ██    ██   \n"
		"██ ███ ██ ██    ██ ██  ██  ██ ██   ██ ██   ██    ██          ██ ██    ██    ██   \n"
		" ███ ███   ██████  ██      ██ ██████  ██   ██    ██          ██  ██████     ██   \n"
		"\n"
	);

	/*----------------------------------------------------------------------------------------------------------------*/

	static struct option long_options[] = {
		{"config", required_argument, NULL, 'c'},
		{"version", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	/*----------------------------------------------------------------------------------------------------------------*/

	int_t c;

	STR_t config_fname = IOT_NAME "-iot.ini";

	while((c = getopt_long(argc, argv, "c:vh", long_options, NULL)) != -1)
	{
		switch(c)
		{
			case 'c':
				config_fname = optarg;
				break;

			case 'v':
				version();
				return 0;

			case 'h':
				help(argv[0]);
				return 0;

			default:
				help(argv[0]);
				return 1;
		}
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	STR_t script_fname = argv[optind];

	if(script_fname == NULL)
	{
		script_fname = IOT_NAME "-iot." IOT_SCRIPT_EXT;
	}

	/*----------------------------------------------------------------------------------------------------------------*/
	/* INITIALIZE IOT                                                                                                 */
	/*----------------------------------------------------------------------------------------------------------------*/

	printf("Starting " IOT_NAME "-IoT (%s, Paho %s, SSL %s)...\n\n", iot_lang_version(), iot_mqtt_version(), iot_mqtt_ssl_version());

	/*----------------------------------------------------------------------------------------------------------------*/

	signal(SIGINT, __exit);

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_config_t config;

	iot_config_initialize(&config, config_fname);

	/*----------------------------------------------------------------------------------------------------------------*/

	STR_t uid = iot_config_get_str(&config, "iot::uid", IOT_DEFAULT_IOT_UID);
	STR_t descr = iot_config_get_str(&config, "iot::descr", IOT_DEFAULT_IOT_DESCR);
	int_t sleep_ms = iot_config_get_int(&config, "iot::sleep_ms", IOT_DEFAULT_IOT_SLEEP_MS);
	int_t log_level = iot_config_get_int(&config, "iot::log_level", IOT_DEFAULT_IOT_LOG_LEVEL);

	STR_t mqtt_server_uri = iot_config_get_str(&config, "mqtt::server_url", IOT_DEFAULT_MQTT_SERVER_URL);
	STR_t mqtt_server_user = iot_config_get_str(&config, "mqtt::server_user", IOT_DEFAULT_MQTT_SERVER_USER);
	STR_t mqtt_server_pass = iot_config_get_str(&config, "mqtt::server_pass", IOT_DEFAULT_MQTT_SERVER_PASS);
	int_t mqtt_connect_timeout = iot_config_get_int(&config, "mqtt::connect_timeout_ms", IOT_DEFAULT_MQTT_CONNECT_TIMEOUT_MS);
	int_t mqtt_disconnect_timeout = iot_config_get_int(&config, "mqtt::disconnnect_timeout_ms", IOT_DEFAULT_MQTT_DISCONNECT_TIMEOUT_MS);
	int_t mqtt_log_level = iot_config_get_int(&config, "mqtt::log_level", IOT_DEFAULT_MQTT_LOG_LEVEL);

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_set_log_level(log_level);

	/*----------------------------------------------------------------------------------------------------------------*/

	if(strcmp(uid, IOT_DEFAULT_IOT_UID) == 0)
	{
		iot_log(IOT_LOG_TYPE_FATAL, "Config parameter `iot:uid` not defined\n");
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_loop(
		&iot,
		&config,
		script_fname,
		uid,
		descr,
		sleep_ms,
		mqtt_server_uri,
		mqtt_server_user,
		mqtt_server_pass,
		mqtt_connect_timeout,
		mqtt_disconnect_timeout,
		mqtt_log_level
	);

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_config_finalize(&config);

	/*----------------------------------------------------------------------------------------------------------------*/

	printf("\nBye.\n");

	/*----------------------------------------------------------------------------------------------------------------*/

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/
