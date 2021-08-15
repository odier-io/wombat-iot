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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
