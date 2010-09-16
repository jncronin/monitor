
/* Copyright (c) 2010, John Cronin <johncronin@scifa.co.uk>
   All rights reserved.
   
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
       * Redistributions of source code must retain the above copyright
         notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.
       * Neither the name of the authors nor the
         names of its contributors may be used to endorse or promote products
         derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "readopt.h"
#include "debug.h"

void set_led(int led, int brightness)
{
	static char *fail_led = NULL;
	static char *degrade_led = NULL;
	static char *fail_brightness;
	static char *fail_max_brightness;
	static char *degrade_brightness;
	static char *degrade_max_brightness;
	char *val = NULL;
	char *b, *mb;
	FILE *led_fd;
	size_t val_len = 256;

	ENTER("set_led");

	if(fail_led == NULL)
	{
		fail_led = readopts_option("fail-led")->arg;
		fail_brightness = (char *)malloc(strlen(fail_led) + 64);
		sprintf(fail_brightness, "%s/brightness", fail_led);
		fail_max_brightness = (char *)malloc(strlen(fail_led) + 64);
		sprintf(fail_max_brightness, "%s/max_brightness", fail_led);
	}
	if(degrade_led == NULL)
	{
		degrade_led = readopts_option("degrade-led")->arg;
		degrade_brightness = (char *)malloc(strlen(degrade_led) + 64);
		sprintf(degrade_brightness, "%s/brightness", degrade_led);
		degrade_max_brightness = (char *)malloc(strlen(degrade_led)
				+ 64);
		sprintf(degrade_max_brightness, "%s/max_brightness",
				degrade_led);
	}

	if(led == LED_FAIL)
	{
		b = fail_brightness;
		mb = fail_max_brightness;
	}
	else if(led == LED_DEGRADE)
	{
		b = degrade_brightness;
		mb = degrade_max_brightness;
	}
	else
		return;

	val = (char *)malloc(val_len * sizeof(char));

	if(brightness == LED_OFF)
		snprintf(val, 256, "0\n");
	else if(brightness == LED_MAX)
	{
		FILE *mb_fd = fopen(mb, "r");
		if(mb_fd == NULL)
			return;
		getline(&val, &val_len, mb_fd);
		fclose(mb_fd);
	}
	else
		snprintf(val, 256, "%i\n", brightness);

	led_fd = fopen(b, "w");
	if(led_fd == NULL)
	{
		free(val);
		return;
	}

	fwrite(val, sizeof(char), strlen(val), led_fd);
	fclose(led_fd);
	free(val);
	val = NULL;
}

int read_until(char *out, int outlen, char separator, int fd)
{ return read_until_with_ignore(out, outlen, separator, 0, 0, fd); }
int read_until_with_ignore(char *out, int outlen, char separator, int ignore,
		int include, int fd)
{
	int len = 0;
	char t;
	int cont = 1;

	ENTER("read_until");

	if(outlen == 0)
		return 0;

	do {
		read(fd, &t, 1);
		if((t == separator) && (len >= ignore))
		{
			if(include && ((len + 1) < outlen))
			{
				out[len] = t;
				out[len + 1] = '\0';
			}
			else
				out[len] = '\0';
			cont = 0;
		}
		else
			out[len] = t;
		len++;
	} while (cont && (len < outlen));
	return len;
}
	 
