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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--------------------------------------------------------------------------------------------------------------------*/

#include "../include/wombat-iot.h"

/*--------------------------------------------------------------------------------------------------------------------*/

buff_t iot_malloc(size_t size)
{
	buff_t result = malloc(size);

	if(result == NULL)
	{
		fprintf(stderr, "out of memory\n");

		exit(1);
	}

	return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

buff_t iot_realloc(buff_t buff, size_t size)
{
	buff_t result = realloc(buff, size);

	if(result == NULL)
	{
		fprintf(stderr, "out of memory\n");

		exit(1);
	}

	return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void iot_free(buff_t buff)
{
	free(buff);
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t iot_strdup(STR_t s)
{
	return strcpy(iot_malloc(strlen(s) + 1), s);
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t iot_strip(str_t s)
{
	size_t i;

	const size_t l = strlen(s);

	register str_t p1 = s + 0x000;
	register str_t p2 = s + l - 1;

	for(i = 0; i < l && isspace(*p1); i++)
	{
		*p1++ = '\0';
	}

	for(i = 0; i < l && isspace(*p2); i++)
	{
		*p2-- = '\0';
	}

	return p1;
}

/*--------------------------------------------------------------------------------------------------------------------*/
