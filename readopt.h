
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

#ifndef READOPT_H
#define READOPT_H

#include <stdio.h>
#include "array_length.h"

#define READOPT_ARG_NO		0
#define READOPT_ARG_OPT		1
#define READOPT_ARG_REQ		2

#define READOPT_SET		1
#define READOPT_UNSET		0

struct readopt_opt
{
	char *long_name;
	char *short_name;
	char *desc;
	char *def;

	int arg_req;
	char *arg_desc;

	int set;

	char *arg;
};


int readopt(int argc, char *argv[], struct readopt_opt *opts, const int opt_count);
int readopt_usage(FILE *s, char *argv_0, struct readopt_opt *opts, const int opt_count);
struct readopt_opt *readopt_getoption(char *option, struct readopt_opt *opts, const int opt_count);

extern const int __optlen;
extern struct readopt_opt *__opts;

#define REGISTER_READOPT(opts)	struct readopt_opt *__opts = (opts); \
	const int __optlen = array_length(opts);
#define readopts(argc, argv) readopt(argc, argv, __opts, __optlen)
#define readopts_usage() readopt_usage(stdout, argv[0], __opts, __optlen)
#define readopts_option(option) readopt_getoption(option, __opts, __optlen)

#endif

