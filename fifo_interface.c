
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "device_db.h"
#include "raid_monitor.h"
#include "debug.h"
#include "string_out.h"

static int token(char *src, char **out, int out_size);

extern int fd;
extern char raid_status[];

extern int debug_level;
extern int message_duration;

int read_fifo(char *msg)
{
	int argc = 8;
	char *argv[argc];

	ENTER("read_fifo");

	// For all events we should recheck the raid status
	get_raid_status(raid_status, 21);

	argc = token(msg, argv, argc);

	if(argc >= 1)
	{
		if(!strcmp(argv[0], "add"))
		{
			if(argc == 2)
			{
				struct disk_device *dev;
								
				refresh_device_db();

				dev = get_device_by_dev_name(argv[1]);
				if(dev != NULL)
				{
					char l1[25];
					int size = dev->size / 2;
					int multiplier = 1;
					int divisor = 1024;
					float f = (float)size;

					snprintf(l1, 21, "Added drive %i ", dev->slot_no);
					while(f > divisor)
					{
						f /= (float)divisor;
						multiplier *= divisor;
					}
					if(size < 10.0f)
					{
						sprintf(l1 + strlen(l1),
								"%.1f", f);
					}
					else
						sprintf(l1 + strlen(l1),
								"%.0f", f);
					if(multiplier == 1)
						sprintf(l1 + strlen(l1), "kB");
					else if(multiplier == 1024)
						sprintf(l1 + strlen(l1), "MB");
					else if(multiplier == (1024 * 1024))
						sprintf(l1 + strlen(l1), "GB");
					else if(multiplier == (1024 * 1024 *
								1024))
						sprintf(l1 + strlen(l1), "TB");

					write_message(fd, l1, dev->model);
					alarm(message_duration);
				}
			}
		}
		else if(!strcmp(argv[0], "remove"))
		{
			if(argc == 2)
			{
				struct disk_device *dev;

				dev = get_device_by_dev_name(argv[1]);
				if(dev != NULL)
				{
					char l1[21];
					snprintf(l1, 21, "Removed drive %i", dev->slot_no);
					write_message(fd, l1, dev->model);
					refresh_device_db();
					alarm(message_duration);
				}
			}
		}
		else if(!strcmp(argv[0], "write"))
		{
			if(argc >= 3)
			{
				char msg[40];
				char msg2[21];
				int count = 0;
				int cur_argc;
				int i;

				for(i = 0; i < 40; i++)
					msg[i] = ' ';

				for(cur_argc = 2; cur_argc < argc; cur_argc++)
				{
					int l = strlen(argv[cur_argc]);
					for(i = count; ((i - count) < l) &&
						(i < 40); i++)
					{
						msg[i] = argv[cur_argc][i - count];
					}
					count += (l + 1);
				}

				strncpy(msg2, &msg[20], 20);
				msg[20] = '\0';
				msg2[20] = '\0';

				write_message(fd, msg, msg2);
				alarm(atoi(argv[1]));
			}
		}	
	}

	return 0;
}

// Tokenize a string
static int token(char *src, char **out, int out_size)
{
	int count, i, len;
	int start_string_with_next = 1;

	ENTER("token");

	len = strlen(src);

	if(debug_level >= DEBUG_TOKEN)
		fprintf(stdout, "tokenizing \'%s\': ", src);

	for(count = 0, i = 0; ((i < len) && (count < out_size)); src++, i++)
	{
		if(start_string_with_next)
		{
			out[count] = src;
			start_string_with_next = 0;
			count++;
		}

		if(((*src == ' ') || (*src == '\n')) && (count < out_size))
		{
			*src = '\0';
			start_string_with_next = 1;
		}
	}

	if(debug_level >= DEBUG_TOKEN)
	{
		fprintf(stdout, "%i items: ", count);
		for(i = 0; i < count; i++)
		{
			if(i > 0)
				fprintf(stdout, ", ");
			fprintf(stdout, "\'%s\'", out[count]);
		}
		fprintf(stdout, "\n");
	}

	return count;
}

