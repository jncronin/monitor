
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

#include "readopt.h"

#define FIFO 		"/tmp/thecus-lcd"
#define PORT 		"/dev/ttyS1"
#define RAID 		"/sys/class/block/md127"
#define SYSTEM_LED	"/sys/class/leds/ich7:orange:system_led"
#define FAIL_LED	"/sys/class/leds/ich7:red:fail"

struct readopt_opt readopt_opts[] = {
	{ .long_name = NULL, .short_name = "d", .desc = "daemonize",
		.arg_req = READOPT_ARG_NO },
	{ .long_name = "fifo-file", .short_name = "f",
		.desc = "fifo file name", .def = FIFO,
		.arg_desc = "file name",
		.arg_req = READOPT_ARG_REQ },
	{ .long_name = "port", .short_name = "p",
		.desc = "port to communicate with lcd", .def = PORT,
		.arg_desc = "port",
		.arg_req = READOPT_ARG_REQ },
	{ .long_name = "write-message", .short_name = "w",
		.desc = "send a message to port",
		.arg_desc = "message",
		.arg_req = READOPT_ARG_REQ },
	{ .long_name = "send", .short_name = "s",
		.desc = "send a message to a listening daemon",
		.arg_desc = "message",
		.arg_req = READOPT_ARG_REQ },
	{ .long_name = "version", .short_name = "v",
		.desc = "display version info",
		.arg_req = READOPT_ARG_NO },
	{ .long_name = "help", .short_name = "h",
		.desc = "display this help",
		.arg_req = READOPT_ARG_NO },
	{ .long_name = "debug-level",
		.desc = "debugging level",
		.arg_req = READOPT_ARG_REQ,
		.arg_desc = "level",
		.def = "0" },
	{ .long_name = "update-interval", .short_name = "u",
		.desc = "time in seconds between screen updates",
		.arg_req = READOPT_ARG_REQ,
		.arg_desc = "interval",
		.def = "1" },
	{ .long_name = "message-duration",
		.desc = "time in seconds that messages are displayed",
		.arg_req = READOPT_ARG_REQ,
		.arg_desc = "duration",
		.def = "5" },
	{ .long_name = "raid-device", .short_name = "r",
		.desc = "RAID device to monitor",
		.arg_req = READOPT_ARG_REQ,
		.arg_desc = "device",
		.def = RAID },
	{ .long_name = "fail-led",
		.desc = "LED device to indicate RAID fail",
		.arg_req = READOPT_ARG_REQ,
		.arg_desc = "device",
		.def = FAIL_LED },
	{ .long_name = "degrade-led",
		.desc = "LED device to indicate RAID degraded",
		.arg_req = READOPT_ARG_REQ,
		.arg_desc = "device",
		.def = SYSTEM_LED },
	{ .long_name = "raid-info-update-delay",
		.desc = "time before refreshing RAID info (in multiples of update-interval)",
		.arg_req = READOPT_ARG_REQ,
		.arg_desc = "interval",
		.def = "60" },
};

REGISTER_READOPT(readopt_opts);

