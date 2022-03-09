%{
/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c)2003 Citrus Project,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "dcmtk/config/osconfig.h"
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_QUEUE_H
#include <sys/queue.h>
#else
#include "dcmtk/oficonv/queue.h"
#endif

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#ifdef _MSC_VER
/* suppress warnings about unreachable (generated) code on MSVC */
#pragma warning(disable: 4702)
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else

#define BADCH   (int)'?'
#define BADARG  (int)':'
static char EMSG[] = "";

int     opterr = 1,             /* if error message should be printed */
        optind = 1,             /* index into parent argv vector */
        optopt,                 /* character checked for validity */
        optreset;               /* reset getopt */
char    *optarg;                /* argument associated with option */

int getopt(int nargc, char * const nargv[], const char *ostr)
{
        static char *place = EMSG;              /* option letter processing */
        char *oli;                              /* option letter list index */

        if (optreset || *place == 0) {          /* update scanning pointer */
                optreset = 0;
                place = nargv[optind];
                if (optind >= nargc || *place++ != '-') {
                        /* Argument is absent or is not an option */
                        place = EMSG;
                        return (-1);
                }
                optopt = *place++;
                if (optopt == '-' && *place == 0) {
                        /* "--" => end of options */
                        ++optind;
                        place = EMSG;
                        return (-1);
                }
                if (optopt == 0) {
                        /* Solitary '-', treat as a '-' option
                           if the program (eg su) is looking for it. */
                        place = EMSG;
                        if (strchr(ostr, '-') == NULL)
                                return (-1);
                        optopt = '-';
                }
        } else
                optopt = *place++;

        /* See if option letter is one the caller wanted... */
        if (optopt == ':' || (oli = (char *)strchr(ostr, optopt)) == NULL) {
                if (*place == 0)
                        ++optind;
                if (opterr && *ostr != ':')
                        (void)fprintf(stderr,
                            "mkesdb: illegal option -- %c\n", optopt);
                return (BADCH);
        }

        /* Does this option need an argument? */
        if (oli[1] != ':') {
                /* don't need argument */
                optarg = NULL;
                if (*place == 0)
                        ++optind;
        } else {
                /* Option-argument is either the rest of this argument or the
                   entire next argument. */
                if (*place)
                        optarg = place;
                else if (oli[2] == ':')
                        /*
                         * GNU Extension, for optional arguments if the rest of
                         * the argument is empty, we return NULL
                         */
                        optarg = NULL;
                else if (nargc > ++optind)
                        optarg = nargv[optind];
                else {
                        /* option-argument absent */
                        place = EMSG;
                        if (*ostr == ':')
                                return (BADARG);
                        if (opterr)
                                (void)fprintf(stderr,
                                    "mkesdb: option requires an argument -- %c\n", optopt);
                        return (BADCH);
                }
                place = EMSG;
                ++optind;
        }
        return (optopt);                        /* return option letter */
}

#endif

#ifdef HAVE_ERR_H
#include <err.h>
#else

static void verrx(int eval, const char *fmt, va_list ap)
{
    fprintf(stderr, "mkcsmapper: ");
    if (fmt != NULL) vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(eval);
}

static void errx(int eval, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    verrx(eval, fmt, ap);
    /* va_end(ap); */
}

static void verrc(int eval, int code, const char *fmt, va_list ap)
{
    fprintf(stderr, "mkcsmapper: ");
    if (fmt != NULL) {
            vfprintf(stderr, fmt, ap);
            fprintf(stderr, ": ");
    }
    fprintf(stderr, "%s\n", strerror(code));
    exit(eval);
}

static void err(int eval, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    verrc(eval, errno, fmt, ap);
    /* va_end(ap); */
}

#endif /* HAVE_ERR_H */

#include "citrus_types.h"
#include "citrus_region.h"
#include "citrus_esdb_file.h"
#include "citrus_db_hash.h"
#include "citrus_db_factory.h"
#include "citrus_lookup_factory.h"

#include "mkesdb_ldef.h"

extern FILE			*yyin;

static struct named_csid_list	 named_csids;
static char			*encoding, *name, *output = NULL, *variable;
static uint32_t		 invalid;
static int			 debug = 0, num_csids = 0, use_invalid = 0;

static void	 dump_file(void);
static void	 register_named_csid(char *, uint32_t);
static void	 set_invalid(uint32_t);
static void	 set_prop_string(const char *, char **, char **);
%}
%union {
	uint32_t	i_value;
	char		*s_value;
}

%token			R_NAME R_ENCODING R_VARIABLE R_DEFCSID R_INVALID
%token			R_LN
%token <i_value>	L_IMM
%token <s_value>	L_STRING

%%

file		: property
		{ dump_file(); }

property	: /* empty */
		| property R_LN
		| property name R_LN
		| property encoding R_LN
		| property variable R_LN
		| property defcsid R_LN
		| property invalid R_LN

name		: R_NAME L_STRING
		{
			set_prop_string("NAME", &name, &$2);
		}

encoding	: R_ENCODING L_STRING
		{
			set_prop_string("ENCODING", &encoding, &$2);
		}
variable	: R_VARIABLE L_STRING
		{
			set_prop_string("VARIABLE", &variable, &$2);
		}
defcsid		: R_DEFCSID L_STRING L_IMM
		{
			register_named_csid($2, $3);
			$2 = NULL;
		}
invalid		: R_INVALID L_IMM
		{
			set_invalid($2);
		}
%%

int
yyerror(const char *s)
{

	fprintf(stderr, "%s in %d\n", s, linenumber);

	return (0);
}

#define CHKERR(ret, func, a)						\
do {									\
	ret = func a;							\
	if (ret)							\
		errx(EXIT_FAILURE, "%s: %s", #func, strerror(ret));	\
} while (/*CONSTCOND*/0)
static void
dump_file(void)
{
	struct _citrus_db_factory *df;
	struct _citrus_region data;
	struct named_csid *csid;
	FILE *fp;
	char buf[100];
	void *serialized;
	size_t size;
	int i, ret;

	ret = 0;
	if (!name) {
		fprintf(stderr, "NAME is mandatory.\n");
		ret = 1;
	}
	if (!encoding) {
		fprintf(stderr, "ENCODING is mandatory.\n");
		ret = 1;
	}
	if (ret)
		exit(1);

	/*
	 * build database
	 */
	CHKERR(ret, _citrus_db_factory_create, (&df, _citrus_db_hash_std, NULL));

	/* store version */
	CHKERR(ret, _citrus_db_factory_add32_by_string, (df, _CITRUS_ESDB_SYM_VERSION,
	    _CITRUS_ESDB_VERSION));

	/* store encoding */
	CHKERR(ret, _citrus_db_factory_add_string_by_string, (df, _CITRUS_ESDB_SYM_ENCODING,
	    encoding));

	/* store variable */
	if (variable)
		CHKERR(ret, _citrus_db_factory_add_string_by_string,
		    (df, _CITRUS_ESDB_SYM_VARIABLE, variable));

	/* store invalid */
	if (use_invalid)
		CHKERR(ret, _citrus_db_factory_add32_by_string, (df,
		    _CITRUS_ESDB_SYM_INVALID, invalid));

	/* store num of charsets */
	CHKERR(ret, _citrus_db_factory_add32_by_string, (df, _CITRUS_ESDB_SYM_NUM_CHARSETS,
	    num_csids));
	i = 0;
	STAILQ_FOREACH(csid, &named_csids, ci_entry) {
		snprintf(buf, sizeof(buf), _CITRUS_ESDB_SYM_CSNAME_PREFIX "%d",
		    i);
		CHKERR(ret, _citrus_db_factory_add_string_by_string,
		    (df, buf, csid->ci_symbol));
		snprintf(buf, sizeof(buf), _CITRUS_ESDB_SYM_CSID_PREFIX "%d",
		    i);
		CHKERR(ret, _citrus_db_factory_add32_by_string, (df, buf, csid->ci_csid));
		i++;
	}

	/*
	 * dump database to file
	 */
	fp = output ? fopen(output, "wb") : stdout;
	if (fp == NULL) {
		perror("fopen");
		exit(1);
	}

	/* dump database body */
	size = _citrus_db_factory_calc_size(df);
	serialized = malloc(size);
	_citrus_region_init(&data, serialized, size);
	CHKERR(ret, _citrus_db_factory_serialize, (df, _CITRUS_ESDB_MAGIC, &data));
	if (fwrite(serialized, size, 1, fp) != 1)
		err(EXIT_FAILURE, "fwrite");

	fclose(fp);
}

static void
set_prop_string(const char *res, char **store, char **data)
{
	char buf[256];

	if (*store) {
		snprintf(buf, sizeof(buf),
		    "%s is duplicated. ignored the one", res);
		yyerror(buf);
		return;
	}

	*store = *data;
	*data = NULL;
}

static void
set_invalid(uint32_t inv)
{

	invalid = inv;
	use_invalid = 1;
}

static void
register_named_csid(char *sym, uint32_t val)
{
	struct named_csid *csid;

	STAILQ_FOREACH(csid, &named_csids, ci_entry) {
		if (strcmp(csid->ci_symbol, sym) == 0) {
			yyerror("multiply defined CSID");
			exit(1);
		}
	}

	csid = (named_csid *) malloc(sizeof(*csid));
	if (csid == NULL) {
		perror("malloc");
		exit(1);
	}
	csid->ci_symbol = sym;
	csid->ci_csid = val;
	STAILQ_INSERT_TAIL(&named_csids, csid, ci_entry);
	num_csids++;
}

static void
do_mkdb(FILE *in)
{
	FILE *out;
	int ret;

        /* dump DB to file */
	out = output ? fopen(output, "wb") : stdout;
	if (out == NULL)
		err(EXIT_FAILURE, "fopen");

	ret = _citrus_lookup_factory_convert(out, in);
	fclose(out);
	if (ret && output)
		unlink(output); /* dump failure */
	if (ret)
		errx(EXIT_FAILURE, "%s\n", strerror(ret));
}

static void
usage(void)
{
	errx(EXIT_FAILURE,
	    "usage:\n"
	    "\tmkesdb[-d] [-o outfile] [infile]\n"
	    "\tmkesdb -m [-d] [-o outfile] [infile]");
}

int
main(int argc, char **argv)
{
	FILE *in = NULL;
	int ch, mkdb = 0;

	while ((ch = getopt(argc, argv, "do:m")) != EOF) {
		switch (ch) {
		case 'd':
			debug = 1;
			break;
		case 'o':
			output = strdup(optarg);
			break;
		case 'm':
			mkdb = 1;
			break;
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;
	switch (argc) {
	case 0:
		in = stdin;
		break;
	case 1:
		in = fopen(argv[0], "r");
		if (!in)
			err(EXIT_FAILURE, "%s", argv[0]);
		break;
	default:
		usage();
	}

	if (mkdb)
		do_mkdb(in);
	else {
		STAILQ_INIT(&named_csids);
		yyin = in;
		yyparse();
	}

	return (0);
}
