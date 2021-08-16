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

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static void version(void)
{
	printf("%s\n", IOT_VERSION);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static void help(STR_t program_name)
{
	printf("%s [--config=<file>] [--script=<file>] [--version] [--help]\n", program_name);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static iot_t iot;

/*--------------------------------------------------------------------------------------------------------------------*/

static void __exit(int_t signal)
{
	iot_log(IOT_LOG_TYPE_DEBUG, "Stopping " IOT_NAME "-IoT...\n");

	iot_stop(&iot);
}

/*--------------------------------------------------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
	/*----------------------------------------------------------------------------------------------------------------*/

	static struct option long_options[] = {
		{"config", required_argument, NULL, 'c'},
		{"script", required_argument, NULL, 's'},
		{"version", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	/*----------------------------------------------------------------------------------------------------------------*/

	int_t c;

	STR_t config_fname = IOT_NAME "-iot.ini";
	STR_t script_fname = IOT_NAME "-iot.py";

	while((c = getopt_long(argc, argv, "c:s:vh", long_options, NULL)) != -1)
	{
		switch(c)
		{
			case 'c':
				config_fname = optarg;
				break;

			case 's':
				script_fname = optarg;
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
	/* INITIALIZE IOT                                                                                                 */
	/*----------------------------------------------------------------------------------------------------------------*/

	printf("Starting " IOT_NAME "-IoT...\n");

	/*----------------------------------------------------------------------------------------------------------------*/

	signal(SIGINT, __exit);

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_config_t config;

	iot_config_initialize(&config, config_fname);

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_loop(
		&iot,
		&config,
		script_fname,
		iot_config_get_str(&config, "iot::uid", IOT_DEFAULT_IOT_UID),
		iot_config_get_str(&config, "iot::descr", IOT_DEFAULT_IOT_DESCR),
		iot_config_get_int(&config, "iot::sleep_ms", IOT_DEFAULT_IOT_SLEEP_MS),
		iot_config_get_str(&config, "mqtt::server_url", IOT_DEFAULT_MQTT_SERVER_URL),
		iot_config_get_str(&config, "mqtt::server_user", IOT_DEFAULT_MQTT_SERVER_USER),
		iot_config_get_str(&config, "mqtt::server_pass", IOT_DEFAULT_MQTT_SERVER_PASS),
		iot_config_get_int(&config, "mqtt::connect_timeout_ms", IOT_DEFAULT_MQTT_CONNECT_TIMEOUT_MS),
		iot_config_get_int(&config, "mqtt::disconnnect_timeout_ms", IOT_DEFAULT_MQTT_DISCONNECT_TIMEOUT_MS),
		iot_config_get_int(&config, "mqtt::log_level", IOT_DEFAULT_MQTT_LOG_LEVEL)
	);

	/*----------------------------------------------------------------------------------------------------------------*/

	iot_config_finalize(&config);

	/*----------------------------------------------------------------------------------------------------------------*/

	printf("\nBye.\n");

	/*----------------------------------------------------------------------------------------------------------------*/

	return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/
