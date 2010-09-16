
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "readopt.h"

static void init_readopts(struct readopt_opt *opts, const int opt_count)
{
	int i;
	for(i = 0; i < opt_count; i++)
	{
		if(opts[i].def != NULL)
		{
			opts[i].arg = (char *)malloc(strlen(opts[i].def) + 1);
			strcpy(opts[i].arg, opts[i].def);
			opts[i].set = READOPT_SET;
		}
		else
			opts[i].set = READOPT_UNSET;
	}
}

int readopt(int argc, char *argv[], struct readopt_opt *opts,
		const int opt_count)
{
	int read_opts = 0;
	int cur_argc = 1;

	init_readopts(opts, opt_count);

	while(cur_argc < argc)
	{
		int cur_readopt;
		char *start_arg = NULL;
		char *cur_arg;
		int arg_len;
		struct readopt_opt *found_readopt = NULL;
		char *has_arg = NULL;

		cur_arg = argv[cur_argc];
		arg_len = strlen(cur_arg);
		if(arg_len >= 3)
		{
			if((cur_arg[0] == '-') && (cur_arg[1]) == '-')
				start_arg = &cur_arg[2];
		}
		if((arg_len >= 2) && (start_arg == NULL))
		{
			if(cur_arg[0] == '-')
				start_arg = &cur_arg[1];
		}
		if(start_arg == NULL)
			return -cur_argc;

		for(cur_readopt = 0; cur_readopt < opt_count; cur_readopt++)
		{
			struct readopt_opt *cur_readopt_opt;

			cur_readopt_opt = &opts[cur_readopt];
			if(((cur_readopt_opt->long_name != NULL) &&
			   (!strcmp(start_arg, cur_readopt_opt->long_name))) ||
			   ((cur_readopt_opt->short_name != NULL) &&
			   (!strcmp(start_arg, cur_readopt_opt->short_name))))
			{
				found_readopt = cur_readopt_opt;
				break;
			}
		}

		if(found_readopt == NULL)
			return -cur_argc;

		found_readopt->set = READOPT_SET;

		if(found_readopt->arg_req == READOPT_ARG_REQ)
		{
			if((cur_argc + 1) < argc)
			{
				cur_argc++;
				has_arg = argv[cur_argc];
			}
			else
				return -cur_argc;
		}
		else if(found_readopt->arg_req == READOPT_ARG_OPT)
		{
			if(((cur_argc + 1) < argc) &&
					(argv[cur_argc + 1][0] != '-'))
			{
				cur_argc++;
				has_arg = argv[cur_argc];
			}
		}

		if(has_arg != NULL)
		{
			found_readopt->arg = (char *)malloc(strlen(has_arg)
					+ 1);
			strcpy(found_readopt->arg, has_arg);
		}

		cur_argc++;

		read_opts++;
	}
	return read_opts;
}

int readopt_usage(FILE *s, char *argv_0, struct readopt_opt *opts,
		const int opt_count)
{
	int j;

	fprintf(s, "Usage %s [options]\n", argv_0);
	for(j = 0; j < opt_count; j++)
	{
		struct readopt_opt *i = &opts[j];

		fprintf(s, "  ");
		if(i->short_name != NULL) {
			fprintf(s, "-%s", i->short_name);
			if(i->long_name != NULL)
				fprintf(s, ", ");
		}
		if(i->long_name != NULL)
			fprintf(s, "--%s", i->long_name);

		if(i->arg_req != READOPT_ARG_NO)
		{
			fprintf(s, " ");
			if(i->arg_req == READOPT_ARG_OPT)
				fprintf(s, "[");
			else
				fprintf(s, "<");
			if(i->arg_desc != NULL)
				fprintf(s, "%s", i->arg_desc);
			else
				fprintf(s, "option");
			if(i->arg_req == READOPT_ARG_OPT)
				fprintf(s, "]");
			else
				fprintf(s, ">");
		}

		fprintf(s, "\n");

		if(i->desc != NULL)
			fprintf(s, "    %s", i->desc);
		if(i->def != NULL)
			fprintf(s, " (default = %s)\n", i->def);
		else if(i->desc != NULL)
			fprintf(s, "\n");
	}
	return 0;
}

struct readopt_opt *readopt_getoption(char *option, struct readopt_opt *opts,
		const int opt_count)
{
	int i;

	for(i = 0; i < opt_count; i++)
	{
		if(((opts[i].long_name != NULL) &&
		   (!strcmp(opts[i].long_name, option))) ||
		  ((opts[i].short_name != NULL) &&
		   (!strcmp(opts[i].short_name, option))))
			return &opts[i];
	}

	return NULL;
}

