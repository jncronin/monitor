
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


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "string_out.h"

#define LINE1		""
#define LINE2		""

char def_line1[] = LINE1;
char def_line2[] = LINE2;

int fd;

void sigio_handler(int signo)
{
	char buf[256];
	int c;
	int i;
	int n = 0;

	do {
		c = read(fd, &buf[n], 1);
		n++;
	} while (c != 0);

	printf("read %i bytes: ", c);
	for(i = 0; i < c; i++)
	{
		if(i != 0)
			printf(", ");
		printf("0x%x", buf[i]);
	}
	printf("\n");

//	signal(signo, sigio_handler);
}

int main(int argc, char *argv[])
{
//	int fd;

	char *srcline1 = def_line1;
	char *srcline2 = def_line2;
	char *newmsg;
	int msglen;

//	signal(SIGIO, sigio_handler);

	struct termios t;

	/*if(argc == 1)
	{
		printf("Usage: %s line1 [line2]\n", argv[0]);
		exit(-1);
	}*/

	if(argc >= 2)
		srcline1 = argv[1];
	if(argc >= 3)
		srcline2 = argv[2];

	fd = open("/dev/ttyS1", O_RDWR | O_NOCTTY | O_NONBLOCK);

	/* Set up serial port */
	tcgetattr(fd, &t);

	t.c_iflag |= ISIG;
	t.c_iflag |= ICANON;
	t.c_iflag |= ECHO;
	t.c_oflag |= OPOST;
	cfsetospeed(&t, B9600);
	tcsetattr(fd, TCSANOW, &t);

	fcntl(fd, F_SETOWN, getpid());
	fcntl(fd, F_SETFL, O_RDONLY);

	newmsg = make_strout_message(srcline1, srcline2, &msglen);
	//write(fd, newmsg, msglen);

	char heartbt[10];
	heartbt[0] = 0x02;
	heartbt[1] = 0x01;
	heartbt[2] = 0x00;
	heartbt[3] = 0x01;
	heartbt[4] = 0x15;
	heartbt[5] = 0x03;

	write(fd, heartbt, 6);

	while(1)
	{
		sigio_handler(SIGIO);
	}

	close(fd);
}

