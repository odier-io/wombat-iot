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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/

void iot_config_initialize(iot_config_t *config, STR_t fname)
{
	/*----------------------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------------------*/

	struct timeval time;

	gettimeofday(&time, NULL);

	srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

	/*----------------------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------------------*/

	config->nb_of_entries = 0;

	/*----------------------------------------------------------------------------------------------------------------*/

	FILE *fp = fopen(fname, "rt");

	if(fp == NULL)
	{
		iot_log(IOT_LOG_TYPE_FATAL, "Could not open `%s`\n", fname);
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	str_t part1;
	str_t part2;

	char buffer[1024];

	iot_config_entry_t *entry;

	while(fgets(buffer, 1024, fp) != NULL)
	{
		part2 = strchr(part1 = buffer, '=');

		if(part2 != NULL)
		{
			*part2++ = '\0';

			entry = &config->entries[config->nb_of_entries++];

			entry->key = iot_strdup(iot_strip(part1));
			entry->val = iot_strdup(iot_strip(part2));
		}
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	fclose(fp);

	/*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

void iot_config_finalize(iot_config_t *config)
{
	uint_t i;

	register iot_config_entry_t *entry = config->entries;

	for(i = 0; i < config->nb_of_entries; i++, entry++)
	{
		iot_free((void *) entry->key);

		iot_free((void *) entry->val);
	}
}

/*--------------------------------------------------------------------------------------------------------------------*/

STR_t iot_config_get_str(iot_config_t *config, STR_t key, STR_t def)
{
	uint_t i;

	register iot_config_entry_t *entry = config->entries;

	for(i = 0; i < config->nb_of_entries; i++, entry++)
	{
		if(strcmp(entry->key, key) == 0)
		{
			return /**/(entry->val);
		}
	}

	return def;
}

/*--------------------------------------------------------------------------------------------------------------------*/

int_t iot_config_get_int(iot_config_t *config, STR_t key, int_t def)
{
	uint_t i;

	register iot_config_entry_t *entry = config->entries;

	for(i = 0; i < config->nb_of_entries; i++, entry++)
	{
		if(strcmp(entry->key, key) == 0)
		{
			return atoi(entry->val);
		}
	}

	return def;
}

/*--------------------------------------------------------------------------------------------------------------------*/

float iot_config_get_flt(iot_config_t *config, STR_t key, float def)
{
	uint_t i;

	register iot_config_entry_t *entry = config->entries;

	for(i = 0; i < config->nb_of_entries; i++, entry++)
	{
		if(strcmp(entry->key, key) == 0)
		{
			return atof(entry->val);
		}
	}

	return def;
}

/*--------------------------------------------------------------------------------------------------------------------*/
