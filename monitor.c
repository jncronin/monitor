
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


/* A monitor which periodically updates the lcd display and will eventually
 * listen to button presses and RAID events */

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>

#include "string_out.h"
#include "raid_monitor.h"
#include "fifo_interface.h"
#include "device_db.h"
#include "util.h"
#include "serial_interface.h"
#include "array_length.h"
#include "readopt.h"

#include "debug.h"

#define DEBUG

void worker(void);
void kill_handler(int);
void read_handler(int);
void alarm_handler(int);
void display_status();

int fd, fifofd;

char setbto[] = { 0x02, 0x01, 0x00, 0x02, 0x13, 0x0f, 0x03 };
char heartbt[] = { 0x02, 0x01, 0x00, 0x01, 0x14, 0x03 };
char startwd[] = { 0x02, 0x01, 0x00, 0x01, 0x15, 0x03 };

char raid_status[21];

int debug_level;

int update_interval;
int message_duration;
int raid_info_update_delay;

char version[] = "monitor 0.0.1\n";
char bplate[] = "Copyright (C) 2010 John Cronin <johncronin@scifa.co.uk>\n"
"This is free software; see the source for copying conditions.  There is NO\n"
"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n";

int main(int argc, char *argv[])
{
	int readopt_ret = readopts(argc, argv);
	ENTER("main");

	if(readopt_ret < 0)
	{
		printf("Invalid argument %s\n", argv[-readopt_ret]);
		readopts_option("h")->set = READOPT_SET;
	}

	debug_level = atoi(readopts_option("debug-level")->arg);
	update_interval = atoi(readopts_option("update-interval")->arg);
	message_duration = atoi(readopts_option("message-duration")->arg);
	raid_info_update_delay =
		atoi(readopts_option("raid-info-update-delay")->arg);
	
	if(readopts_option("h")->set)
	{
		readopts_usage();
		return 0;
	}

	if(readopts_option("v")->set)
	{
		printf(version);
		printf(bplate);
		return 0;
	}

	if(readopts_option("send")->set)
	{
		int this_fd;
		this_fd = open(readopts_option("fifo-file")->arg, O_WRONLY | O_NOCTTY | O_NONBLOCK);
		if(this_fd != -1)
		{
			write(this_fd, readopts_option("send")->arg,
				strlen(readopts_option("send")->arg) + 1);
			close(this_fd);
			return 0;
		}
		else
			return -1;
	}

	if(readopts_option("w")->set)
	{
		int this_fd;
		struct termios t;

		this_fd = open(readopts_option("port")->arg, O_RDWR | O_NOCTTY);
		/* Set up serial port */
		tcgetattr(this_fd, &t);
		t.c_iflag |= ISIG;
		t.c_iflag |= ICANON;
		t.c_iflag |= ECHO;
		t.c_oflag |= OPOST;
		cfsetospeed(&t, B9600);
		tcsetattr(this_fd, TCSANOW, &t);

		fcntl(this_fd, F_SETOWN, getpid());
		fcntl(this_fd, F_SETFL, O_RDONLY | O_ASYNC);

		write_message(this_fd, readopts_option("w")->arg, "");

		close(this_fd);
		return 0;
	}

	if(readopts_option("d")->set)
	{
		pid_t fork_val = fork();
		if(fork_val == -1)
		{
			printf("Error forking\n");
			return -1;
		}
		if(fork_val == 0)
			worker();
	}
	else
	{
		printf(version);
		printf(bplate);
		worker();
	}

	return 0;
}

void worker()
{
	struct termios t;

	ENTER("worker");

	signal(SIGTERM, kill_handler);
	signal(SIGIO, read_handler);
	
	refresh_device_db();
	get_raid_status(raid_status, 21);

	fd = open(readopts_option("port")->arg, O_RDWR | O_NOCTTY | O_NONBLOCK);
	/* Set up serial port */
	tcgetattr(fd, &t);
	t.c_iflag |= ISIG;
	t.c_iflag |= ICANON;
	t.c_iflag |= ECHO;
	t.c_oflag |= OPOST;
	cfsetospeed(&t, B9600);
	tcsetattr(fd, TCSANOW, &t);

	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETFL, O_RDONLY | O_ASYNC);

	mkfifo(readopts_option("fifo-file")->arg, O_RDWR | O_NONBLOCK);
	fifofd = open(readopts_option("fifo-file")->arg, O_RDWR | O_NONBLOCK);
	fcntl(fifofd, F_SETOWN, getpid());
	fcntl(fifofd, F_SETFL, FASYNC);

	//write(fd, setbto, array_length(setbto));
	//write(fd, heartbt, array_length(heartbt));
	//write(fd, startwd, array_length(startwd));
		
	alarm_handler(SIGALRM);

	while(1)
	{
		pause();
	}
}

void kill_handler(int signo)
{
	ENTER("kill_handler");

	signal(SIGIO, SIG_IGN);
	signal(SIGALRM, SIG_IGN);
	write_message(fd, "Shutting down...", "");
	fsync(fd);
	close(fd);
	close(fifofd);
	unlink(readopts_option("fifo-file")->arg);
	exit(0);
}

void read_handler(int signo)
{
	int do_read = 1;

	ENTER("read_handler");

	while (do_read) 
	{
		struct pollfd pollfds[2];

		do_read = 0;

		pollfds[0].fd = fd;
		pollfds[0].events = POLLIN;
		pollfds[0].revents = 0;
		pollfds[1].fd = fifofd;
		pollfds[1].events = POLLIN;
		pollfds[1].revents = 0;

		poll(pollfds, 2, 0);

		if(pollfds[0].revents & POLLIN)
		{
			char msg[256];
			int len = read_until_with_ignore(msg, 256, '\3', 2, 1,
					fd);
			read_serial(msg, len);
			do_read = 1;
		}

		if(pollfds[1].revents & POLLIN)
		{
			char msg[256];
			read_until(msg, 256, '\0', fifofd);
			read_fifo(msg);
			do_read = 1;
		}
	}
	signal(signo, read_handler);
}

void alarm_handler(int signo)
{
	ENTER("alarm_handler");

	display_status();

	signal(signo, alarm_handler);
	alarm(update_interval);
}

void display_status()
{
	char line1[21];

	time_t rawtime;
	struct tm *ti;
	static int raid_update_countdown = 0;

	ENTER("display_status");

	time(&rawtime);
	ti = localtime(&rawtime);

	strftime(line1, 21, "%d-%b-%Y %H:%M:%S", ti);

	if(raid_update_countdown == 0)
	{
		get_raid_status(raid_status, 21);
		raid_update_countdown = raid_info_update_delay;
	}
	else
		raid_update_countdown--;

	write_message(fd, line1, raid_status);
}

