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
#include <stdlib.h>
#include <string.h>

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/

void iot_config_initialize(iot_config_t *config, STR_t fname)
{
	/*----------------------------------------------------------------------------------------------------------------*/

	config->nb_of_entries = 0;

	/*----------------------------------------------------------------------------------------------------------------*/

	FILE *fp = fopen(fname, "rt");

	if(fp == NULL)
	{
		iot_log(IOT_LOG_TYPE_FATAL, "Could not open configuration file `%s`\n", fname);
	}

	/*----------------------------------------------------------------------------------------------------------------*/

	str_t part1;
	str_t part2;

	char buffer[1024];

	iot_config_entry_t *entry;

	while(config->nb_of_entries < 64 && fgets(buffer, 1024, fp) != NULL)
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
