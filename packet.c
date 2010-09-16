
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
#include <arpa/inet.h>
#include "debug.h"

char *make_packet(char *message, int message_len, int *packet_len)
{
	char *packet = (char *)malloc(message_len + 5);
	int i;

	ENTER("make_packet");

	packet[0] = 0x02;
	packet[1] = 0x01;
	*((uint16_t *)&packet[2]) = htons((uint16_t)message_len);
	for(i = 0; i < message_len; i++)
		packet[4 + i] = message[i];
	packet[4 + i] = 0x03;
	*packet_len = message_len + 5;
	return packet;
}

char *read_packet(char *packet, int *message_len)
{
	char *message;
	int i;

	ENTER("read_packet");

	*message_len = (int)ntohs(*((uint16_t *)&packet[2]));
	message = (char *)malloc(*message_len);
	for(i = 0; i < *message_len; i++)
		message[i] = packet[4 + i];
	return message;
}

