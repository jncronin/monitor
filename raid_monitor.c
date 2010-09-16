
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

#include "array_length.h"
#include "raid_monitor.h"
#include "readopt.h"
#include "util.h"
#include "debug.h"

#ifdef TEST
#define MSG_LEN		20
char message[MSG_LEN];
#endif

/*char *statuses[] = {
	"Healthy", "Degraded", "Dead", "Unknown", "Missing"
};*/
char *statuses[] = {
	"Healthy", "Degraded", "Recovering"
};

const char unknown[] = "Unknown";

const char *get_status(int status)
{
	ENTER("get_status");

	if((status < 0) || (status >= array_length(statuses)))
		return unknown;
	else
		return statuses[status];
}

int get_raid_status(char *status, int ret_len)
{
	static char *sys_raid = NULL;
	static char *degraded = NULL;
	static char *sync_action = NULL;
	static char *sync_completed = NULL;

	//char system_str[256];
	int ret;
	char *raid_result = NULL;
	size_t raid_result_size;
	FILE *raid_fd;
	int percent_done = 0;

	ENTER("get_raid_status");

	if(sys_raid == NULL)
	{
		sys_raid = readopts_option("raid-device")->arg;
		
		degraded = (char *)malloc(strlen(sys_raid) + 64);
		sprintf(degraded, "%s/md/degraded", sys_raid);
		sync_action = (char *)malloc(strlen(sys_raid) + 64);
		sprintf(sync_action, "%s/md/sync_action", sys_raid);
		sync_completed = (char *)malloc(strlen(sys_raid) + 64);
		sprintf(sync_completed, "%s/md/sync_completed", sys_raid);
	}

	raid_fd = fopen(degraded, "r");
	if(raid_fd == NULL)
		return -1;
	getline(&raid_result, &raid_result_size, raid_fd);
	fclose(raid_fd);

	if(raid_result[0] == '0')
		ret = RAID_HEALTHY;
	else
	{
		raid_fd = fopen(sync_action, "r");
		if(raid_fd == NULL)
			ret = RAID_DEGRADED;
		else
		{
			getline(&raid_result, &raid_result_size, raid_fd);
			fclose(raid_fd);

			if(!strncmp(raid_result, "recover", 7))
			{
				ret = RAID_REBUILDING;
				raid_fd = fopen(sync_completed, "r");
				if(raid_fd != NULL)
				{
					long long int done, total;

					getline(&raid_result,
							&raid_result_size,
							raid_fd);
					fclose(raid_fd);

					sscanf(raid_result, "%Li / %Li", 
							&done, &total);

					percent_done =
						(int)(done * 100LL/ total);	
				}
			}
			else
				ret = RAID_DEGRADED;
		}
	}
	free(raid_result);

	/*snprintf(system_str, 256, "/sbin/mdadm --misc -t %s",
			readopts_option("raid-device")->arg);
	ret = system(system_str);*/

	//if(WIFEXITED(ret))
	{
//		ret = WEXITSTATUS(ret);
//
//		Update status

		switch(ret)
		{
			case RAID_HEALTHY:
				snprintf(status, ret_len, "RAID: Healthy");
				break;
			case RAID_DEGRADED:
				snprintf(status, ret_len, "RAID: Degraded");
				break;
			case RAID_REBUILDING:
				snprintf(status, ret_len,
						"RAID: Rebuild %i%%",
						percent_done);
				break;
		}

		//snprintf(status, ret_len, "RAID: %s", get_status(ret));

		// Update LEDs
		switch(ret)
		{
			case RAID_HEALTHY:
				set_led(LED_FAIL, LED_OFF);
				set_led(LED_DEGRADE, LED_OFF);
				break;
			case RAID_DEGRADED:
			case RAID_REBUILDING:
				set_led(LED_FAIL, LED_OFF);
				set_led(LED_DEGRADE, LED_MAX);
				break;
		}

		return ret;
	}
	/*else
		return -1;*/
}

#ifdef TEST
int main()
{
	int ret;
	if((ret = get_raid_status(message, MSG_LEN)) >= 0)
		printf("%s\n", message);
	else
		printf("error\n");
	return ret;
}
#endif

