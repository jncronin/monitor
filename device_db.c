
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "device_db.h"
#include "array_length.h"
#include "debug.h"

#define MAX_DISKS	12

const char identify_dev[] =	"/bin/sh /var/disks/front_tray/identify-dev.sh";
const char identify_model[] =	"/bin/sh /var/disks/front_tray/identify-model.sh";
const char identify_sys[] =	"/bin/sh /var/disks/front_tray/identify-sys.sh";
const char identify_size[] =	"/bin/sh /var/disks/front_tray/identify-size.sh";

static struct disk_device disks[] = {
	{ .vdf_id = "/var/disks/front_tray/0" },
	{ .vdf_id = "/var/disks/front_tray/1" },
	{ .vdf_id = "/var/disks/front_tray/2" },
	{ .vdf_id = "/var/disks/front_tray/3" },
	{ .vdf_id = "/var/disks/front_tray/4" }
};

static void strncpy_without_newline(char *dest, const char *src, int size)
{
	ENTER("strncpy_without_newline");

	if(size == 0)
		return;
	while((*src != '\0') && (*src != '\n') && (size > 0))
	{
		*dest = *src;
		dest++;
		src++;
		size--;
	}
	if(size == 0)
		dest--;
	*dest = '\0';
}

struct disk_device *get_device_by_dev_name(const char *dev_name)
{
	int i;

	ENTER("get_device_by_dev_name");

	if(dev_name == NULL)
		return NULL;
	for(i = 0; i < array_length(disks); i++)
	{
		if(!strcmp(dev_name, disks[i].dev_name))
			return &disks[i];
	}
	return NULL;
}

struct disk_device *get_device_by_slot_no(int slot_no)
{
	int i;

	ENTER("get_device_by_slot_no");

	if(slot_no == -1)
		return NULL;
	for(i = 0; i < array_length(disks); i++)
	{
		if(disks[i].slot_no == slot_no)
			return &disks[i];
	}
	return NULL;
}

void refresh_device_db()
{
	int i;

	ENTER("refresh_device_db");

	for(i = 0; i < array_length(disks); i++)
	{
		char id_cmd[1024];
		FILE *pipe;
		char *out = NULL;
		size_t outlen = 0;

		snprintf(id_cmd, 1024, "%s %s", identify_dev, disks[i].vdf_id);
		pipe = popen(id_cmd, "r");
		if(pipe)
		{
			getline(&out, &outlen, pipe);
			if(out[0] != '\0')
			{
				disks[i].slot_no = i + 1;
				strncpy_without_newline(disks[i].dev_name,
						out, 128);
			}
			else
				disks[i].slot_no = -1;
			pclose(pipe);
		}

		if(disks[i].slot_no != -1)
		{
			snprintf(id_cmd, 1024, "%s %s", identify_model, disks[i].vdf_id);
			pipe = popen(id_cmd, "r");
			if(pipe)
			{
				getline(&out, &outlen, pipe);
				if(out[0] != '\0')
				{
					strncpy_without_newline(disks[i].model,
							out, 128);
				}
				else
					disks[i].slot_no = -1;
				pclose(pipe);
			}

			snprintf(id_cmd, 1024, "%s %s", identify_size, disks[i].vdf_id);
			pipe = popen(id_cmd, "r");
			if(pipe)
			{
				getline(&out, &outlen, pipe);
				if(out[0] != '\0')
					sscanf(out, "%i", &disks[i].size);
				else
					disks[i].slot_no = -1;
				pclose(pipe);
			}

			snprintf(id_cmd, 1024, "%s %s", identify_sys, disks[i].vdf_id);
			pipe = popen(id_cmd, "r");
			if(pipe)
			{
				getline(&out, &outlen, pipe);
				if(out[0] != '\0')
				{
					strncpy_without_newline(
							disks[i].sys_path,
							out, 256);
				}
				else
					disks[i].slot_no = -1;
				pclose(pipe);
			}
		}
		free(out);
	}
}

