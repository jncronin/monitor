
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

#include "packet.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "debug.h"

#define	LINE1_LEN		40
#define LINE2_LEN		20
#define WRITE_STRING_MSG	0x11

#define TEST_MSG	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]+=-_<>,."
char test_msg[] = TEST_MSG;

char *make_test_message(int *packet_len)
{
	int msglen = 1 + strlen(test_msg) + 1;
	char *newstr = (char *)malloc(msglen);
	char *ret;
	int i;
	for(i = 0; i < strlen(test_msg); i++)
		newstr[1 + i] = test_msg[i];
	newstr[0] = WRITE_STRING_MSG;
	newstr[1 + i] = 0x0;
	ret = make_packet(newstr, msglen, packet_len);
	free(newstr);
	return ret;
}

char *make_strout_message(char *l1, char *l2, int *packet_len)
{
	int len1 = strlen(l1);
	int len2 = strlen(l2);
	int msglen = 1 + LINE1_LEN + 0 + LINE2_LEN + 1;
	int i;
	char *ret;

	char *newstr = (char *)malloc(msglen);
	for(i = 0; i < msglen; i++)
		newstr[i] = ' ';
	newstr[0] = WRITE_STRING_MSG;
	for(i = 0; i < len1; i++)
		newstr[1 + i] = l1[i];
	for(i = 0; i < len2; i++)
		newstr[1 + LINE1_LEN + 0 + i] = l2[i];
//	newstr[1 + LINE1_LEN] = '\0';
	newstr[1 + LINE1_LEN + 0 + LINE2_LEN] = '\0';

	ret = make_packet(newstr, msglen, packet_len);
	free(newstr);
	return ret;
}

int write_message(int fd, char *l1, char *l2)
{
	char *msg;
	int msg_len;

	ENTER("write_message");

	msg = make_strout_message(l1, l2, &msg_len);
	write(fd, msg, msg_len);
	free(msg);
	return 0;
}

