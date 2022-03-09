/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30705

/* Bison version string.  */
#define YYBISON_VERSION "3.7.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"

/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c)2003, 2006 Citrus Project,
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
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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
                            "mkcsmapper: illegal option -- %c\n", optopt);
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
                                    "mkcsmapper: option requires an argument -- %c\n", optopt);
                        return (BADCH);
                }
                place = EMSG;
                ++optind;
        }
        return (optopt);                        /* return option letter */
}

#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
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


#include "mkcsmapper_ldef.h"
#include "citrus_types.h"
#include "citrus_mapper_std_file.h"
#include "citrus_region.h"
#include "citrus_db_factory.h"
#include "citrus_db_hash.h"
#include "citrus_lookup_factory.h"
#include "citrus_pivot_factory.h"

extern FILE		*yyin;

int			 debug = 0;

static linear_zone_t	 rowcol[_CITRUS_MAPPER_STD_ROWCOL_MAX];
static char		*map_name;
static char		*output = NULL;
static void		*table = NULL;
static size_t		 rowcol_len = 0;
static size_t		 table_size;
static uint32_t	 done_flag = 0;
static uint32_t	 dst_ilseq, dst_invalid, dst_unit_bits, oob_mode;
static uint32_t	 rowcol_bits = 0, rowcol_mask = 0;
static uint32_t	 src_next;
static int		 map_type;
static void		 (*putfunc)(void *, size_t, uint32_t) = NULL;

#define DF_TYPE			0x00000001
#define DF_NAME			0x00000002
#define DF_SRC_ZONE		0x00000004
#define DF_DST_INVALID		0x00000008
#define DF_DST_ILSEQ		0x00000010
#define DF_DST_UNIT_BITS	0x00000020
#define DF_OOB_MODE		0x00000040

static void	dump_file(void);
static void	setup_map(void);
static void	set_type(int);
static void	set_name(char *);
static void	set_src_zone(uint32_t);
static void	set_dst_invalid(uint32_t);
static void	set_dst_ilseq(uint32_t);
static void	set_dst_unit_bits(uint32_t);
static void	set_oob_mode(uint32_t);
static int	check_src(uint32_t, uint32_t);
static void	store(const linear_zone_t *, uint32_t, int);
static void	put8(void *, size_t, uint32_t);
static void	put16(void *, size_t, uint32_t);
static void	put32(void *, size_t, uint32_t);
static void	set_range(uint32_t, uint32_t);
static void	set_src(linear_zone_t *, uint32_t, uint32_t);

#line 307 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "mkcsmapper_bison.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_R_TYPE = 3,                     /* R_TYPE  */
  YYSYMBOL_R_NAME = 4,                     /* R_NAME  */
  YYSYMBOL_R_SRC_ZONE = 5,                 /* R_SRC_ZONE  */
  YYSYMBOL_R_DST_UNIT_BITS = 6,            /* R_DST_UNIT_BITS  */
  YYSYMBOL_R_DST_INVALID = 7,              /* R_DST_INVALID  */
  YYSYMBOL_R_DST_ILSEQ = 8,                /* R_DST_ILSEQ  */
  YYSYMBOL_R_BEGIN_MAP = 9,                /* R_BEGIN_MAP  */
  YYSYMBOL_R_END_MAP = 10,                 /* R_END_MAP  */
  YYSYMBOL_R_INVALID = 11,                 /* R_INVALID  */
  YYSYMBOL_R_ROWCOL = 12,                  /* R_ROWCOL  */
  YYSYMBOL_R_ILSEQ = 13,                   /* R_ILSEQ  */
  YYSYMBOL_R_OOB_MODE = 14,                /* R_OOB_MODE  */
  YYSYMBOL_R_LN = 15,                      /* R_LN  */
  YYSYMBOL_L_IMM = 16,                     /* L_IMM  */
  YYSYMBOL_L_STRING = 17,                  /* L_STRING  */
  YYSYMBOL_18_ = 18,                       /* '-'  */
  YYSYMBOL_19_ = 19,                       /* '/'  */
  YYSYMBOL_20_ = 20,                       /* '='  */
  YYSYMBOL_YYACCEPT = 21,                  /* $accept  */
  YYSYMBOL_file = 22,                      /* file  */
  YYSYMBOL_property = 23,                  /* property  */
  YYSYMBOL_name = 24,                      /* name  */
  YYSYMBOL_type = 25,                      /* type  */
  YYSYMBOL_types = 26,                     /* types  */
  YYSYMBOL_range = 27,                     /* range  */
  YYSYMBOL_ranges = 28,                    /* ranges  */
  YYSYMBOL_src_zone = 29,                  /* src_zone  */
  YYSYMBOL_zone = 30,                      /* zone  */
  YYSYMBOL_dst_invalid = 31,               /* dst_invalid  */
  YYSYMBOL_dst_ilseq = 32,                 /* dst_ilseq  */
  YYSYMBOL_dst_unit_bits = 33,             /* dst_unit_bits  */
  YYSYMBOL_oob_mode = 34,                  /* oob_mode  */
  YYSYMBOL_oob_mode_sel = 35,              /* oob_mode_sel  */
  YYSYMBOL_mapping = 36,                   /* mapping  */
  YYSYMBOL_begin_map = 37,                 /* begin_map  */
  YYSYMBOL_map_elems = 38,                 /* map_elems  */
  YYSYMBOL_map_elem = 39,                  /* map_elem  */
  YYSYMBOL_dst = 40,                       /* dst  */
  YYSYMBOL_src = 41,                       /* src  */
  YYSYMBOL_lns = 42                        /* lns  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   44

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  21
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  41
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  63

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   272


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    18,     2,    19,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    20,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   256,   256,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   269,   270,   271,   272,   274,   275,   277,   278,
     281,   285,   286,   287,   288,   290,   291,   293,   294,   296,
     297,   299,   301,   303,   307,   311,   317,   320,   324,   328,
     332,   333
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "R_TYPE", "R_NAME",
  "R_SRC_ZONE", "R_DST_UNIT_BITS", "R_DST_INVALID", "R_DST_ILSEQ",
  "R_BEGIN_MAP", "R_END_MAP", "R_INVALID", "R_ROWCOL", "R_ILSEQ",
  "R_OOB_MODE", "R_LN", "L_IMM", "L_STRING", "'-'", "'/'", "'='",
  "$accept", "file", "property", "name", "type", "types", "range",
  "ranges", "src_zone", "zone", "dst_invalid", "dst_ilseq",
  "dst_unit_bits", "oob_mode", "oob_mode_sel", "mapping", "begin_map",
  "map_elems", "map_elem", "dst", "src", "lns", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,    45,    47,
      61
};
#endif

#define YYPACT_NINF (-39)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -39,    12,    -1,   -39,     3,     4,     7,     9,    10,    11,
      13,    -2,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,
      13,   -39,   -39,   -39,   -39,    14,    15,   -39,   -39,   -39,
     -39,   -39,    16,   -39,   -39,   -39,    16,     0,    17,     7,
     -39,   -39,    18,    19,    13,    20,   -39,    22,    21,   -39,
      16,     6,   -39,   -39,   -39,   -39,    24,   -39,    23,   -39,
      14,    25,   -39
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     0,     0,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     4,     5,     6,     7,     8,     9,    10,    11,
       0,    29,    14,    13,    12,     0,    19,    18,    23,    21,
      22,    40,    28,    25,    26,    24,     2,    36,     0,     0,
      41,    27,    37,     0,     0,     0,    15,     0,     0,    39,
      30,     0,    16,    38,    34,    35,    33,    31,     0,    32,
      20,     0,    17
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -39,   -39,   -39,   -39,   -39,   -39,   -38,   -39,   -39,   -39,
     -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,   -39,
     -39,   -20
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     2,    13,    14,    23,    26,    58,    15,    27,
      16,    17,    18,    19,    35,    20,    21,    37,    44,    57,
      45,    32
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      36,    47,     4,     5,     6,     7,     8,     9,    10,    33,
      41,    34,     3,    11,    12,    22,    42,    54,    43,    55,
      61,    24,    56,    25,    50,    28,    29,    30,    31,     0,
       0,    40,    38,    46,    39,    49,    48,    53,     0,    60,
      51,    52,    59,     0,    62
};

static const yytype_int8 yycheck[] =
{
      20,    39,     3,     4,     5,     6,     7,     8,     9,    11,
      10,    13,     0,    14,    15,    12,    16,    11,    18,    13,
      58,    17,    16,    16,    44,    16,    16,    16,    15,    -1,
      -1,    15,    18,    16,    19,    16,    18,    16,    -1,    16,
      20,    19,    18,    -1,    19
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    22,    23,     0,     3,     4,     5,     6,     7,     8,
       9,    14,    15,    24,    25,    29,    31,    32,    33,    34,
      36,    37,    12,    26,    17,    16,    27,    30,    16,    16,
      16,    15,    42,    11,    13,    35,    42,    38,    18,    19,
      15,    10,    16,    18,    39,    41,    16,    27,    18,    16,
      42,    20,    19,    16,    11,    13,    16,    40,    28,    18,
      16,    27,    19
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    21,    22,    23,    23,    23,    23,    23,    23,    23,
      23,    23,    24,    25,    26,    27,    28,    28,    29,    30,
      30,    31,    32,    33,    34,    35,    35,    36,    37,    38,
      38,    39,    39,    40,    40,    40,    41,    41,    41,    41,
      42,    42
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     3,     0,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     3,     0,     3,     2,     1,
       6,     2,     2,     2,     2,     1,     1,     3,     2,     0,
       3,     3,     4,     1,     1,     1,     0,     1,     3,     2,
       1,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* file: property mapping lns  */
#line 257 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                { dump_file(); }
#line 1381 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 12: /* name: R_NAME L_STRING  */
#line 269 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                                  { set_name((yyvsp[0].s_value)); (yyvsp[0].s_value) = NULL; }
#line 1387 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 13: /* type: R_TYPE types  */
#line 270 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                               { set_type((yyvsp[0].i_value)); }
#line 1393 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 14: /* types: R_ROWCOL  */
#line 271 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                           { (yyval.i_value) = R_ROWCOL; }
#line 1399 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 15: /* range: L_IMM '-' L_IMM  */
#line 272 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                                  { set_range((yyvsp[-2].i_value), (yyvsp[0].i_value)); }
#line 1405 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 18: /* src_zone: R_SRC_ZONE zone  */
#line 277 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                                  { set_src_zone((yyvsp[0].i_value)); }
#line 1411 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 19: /* zone: range  */
#line 278 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                        {
			(yyval.i_value) = 32;
		}
#line 1419 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 20: /* zone: range '/' range '/' ranges L_IMM  */
#line 281 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                                                   {
			(yyval.i_value) = (yyvsp[0].i_value);
		}
#line 1427 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 21: /* dst_invalid: R_DST_INVALID L_IMM  */
#line 285 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                                      { set_dst_invalid((yyvsp[0].i_value)); }
#line 1433 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 22: /* dst_ilseq: R_DST_ILSEQ L_IMM  */
#line 286 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                                    { set_dst_ilseq((yyvsp[0].i_value)); }
#line 1439 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 23: /* dst_unit_bits: R_DST_UNIT_BITS L_IMM  */
#line 287 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                                        { set_dst_unit_bits((yyvsp[0].i_value)); }
#line 1445 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 24: /* oob_mode: R_OOB_MODE oob_mode_sel  */
#line 288 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                                          { set_oob_mode((yyvsp[0].i_value)); }
#line 1451 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 25: /* oob_mode_sel: R_INVALID  */
#line 290 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                            { (yyval.i_value) = _CITRUS_MAPPER_STD_OOB_NONIDENTICAL; }
#line 1457 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 26: /* oob_mode_sel: R_ILSEQ  */
#line 291 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                          { (yyval.i_value) = _CITRUS_MAPPER_STD_OOB_ILSEQ; }
#line 1463 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 28: /* begin_map: R_BEGIN_MAP lns  */
#line 294 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                                  { setup_map(); }
#line 1469 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 31: /* map_elem: src '=' dst  */
#line 300 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                { store(&(yyvsp[-2].lz_value), (yyvsp[0].i_value), 0); }
#line 1475 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 32: /* map_elem: src '=' L_IMM '-'  */
#line 302 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                { store(&(yyvsp[-3].lz_value), (yyvsp[-1].i_value), 1); }
#line 1481 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 33: /* dst: L_IMM  */
#line 304 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                {
			(yyval.i_value) = (yyvsp[0].i_value);
		}
#line 1489 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 34: /* dst: R_INVALID  */
#line 308 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                {
			(yyval.i_value) = dst_invalid;
		}
#line 1497 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 35: /* dst: R_ILSEQ  */
#line 312 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                {
			(yyval.i_value) = dst_ilseq;
		}
#line 1505 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 36: /* src: %empty  */
#line 317 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                {
			set_src(&(yyval.lz_value), src_next, src_next);
		}
#line 1513 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 37: /* src: L_IMM  */
#line 321 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                {
			set_src(&(yyval.lz_value), (yyvsp[0].i_value), (yyvsp[0].i_value));
		}
#line 1521 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 38: /* src: L_IMM '-' L_IMM  */
#line 325 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                {
			set_src(&(yyval.lz_value), (yyvsp[-2].i_value), (yyvsp[0].i_value));
		}
#line 1529 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;

  case 39: /* src: '-' L_IMM  */
#line 329 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"
                {
			set_src(&(yyval.lz_value), src_next, (yyvsp[0].i_value));
		}
#line 1537 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"
    break;


#line 1541 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.cc"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 335 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"


static void
warning(const char *s)
{

	fprintf(stderr, "%s in %d\n", s, linenumber);
}

int
yyerror(const char *s)
{

	warning(s);
	exit(1);
}

void
put8(void *ptr, size_t ofs, uint32_t val)
{

	*((uint8_t *)ptr + ofs) = (uint8_t) val;
}

void
put16(void *ptr, size_t ofs, uint32_t val)
{

	uint16_t oval = htons((uint16_t)val);
	memcpy((uint16_t *)ptr + ofs, &oval, 2);
}

void
put32(void *ptr, size_t ofs, uint32_t val)
{

	uint32_t oval = htonl(val);
	memcpy((uint32_t *)ptr + ofs, &oval, 4);
}

static void
alloc_table(void)
{
	linear_zone_t *p;
	size_t i;
	uint32_t val = 0;

	i = rowcol_len;
	p = &rowcol[--i];
	table_size = p->width;
	while (i > 0) {
		p = &rowcol[--i];
		table_size *= p->width;
	}
	table = (void *)malloc(table_size * dst_unit_bits / 8);
	if (table == NULL) {
		perror("malloc");
		exit(1);
	}

	switch (oob_mode) {
	case _CITRUS_MAPPER_STD_OOB_NONIDENTICAL:
		val = dst_invalid;
		break;
	case _CITRUS_MAPPER_STD_OOB_ILSEQ:
		val = dst_ilseq;
		break;
	default:
		break;
	}
	for (i = 0; i < table_size; i++)
		(*putfunc)(table, i, val);
}

static void
setup_map(void)
{

	if ((done_flag & DF_SRC_ZONE)==0) {
		fprintf(stderr, "SRC_ZONE is mandatory.\n");
		exit(1);
	}
	if ((done_flag & DF_DST_UNIT_BITS)==0) {
		fprintf(stderr, "DST_UNIT_BITS is mandatory.\n");
		exit(1);
	}

	if ((done_flag & DF_DST_INVALID) == 0)
		dst_invalid = 0xFFFFFFFF;
	if ((done_flag & DF_DST_ILSEQ) == 0)
		dst_ilseq = 0xFFFFFFFE;
	if ((done_flag & DF_OOB_MODE) == 0)
		oob_mode = _CITRUS_MAPPER_STD_OOB_NONIDENTICAL;

	alloc_table();
}

static void
create_rowcol_info(struct _citrus_region *r)
{
	void *ptr;
	size_t i, len, ofs;

	ofs = 0;
	ptr = malloc(_CITRUS_MAPPER_STD_ROWCOL_INFO_SIZE);
	if (ptr == NULL)
		err(EXIT_FAILURE, "malloc");
	put32(ptr, ofs, rowcol_bits); ofs++;
	put32(ptr, ofs, dst_invalid); ofs++;

	/* XXX: keep backward compatibility */
	switch (rowcol_len) {
	case 1:
		put32(ptr, ofs, 0); ofs++;
		put32(ptr, ofs, 0); ofs++;
	/*FALLTHROUGH*/
	case 2:
		len = 0;
		break;
	default:
		len = rowcol_len;
	}
	for (i = 0; i < rowcol_len; ++i) {
		put32(ptr, ofs, rowcol[i].begin); ofs++;
		put32(ptr, ofs, rowcol[i].end); ofs++;
	}
	put32(ptr, ofs, dst_unit_bits); ofs++;
	put32(ptr, ofs, (uint32_t)len); ofs++;

	_citrus_region_init(r, ptr, ofs * 4);
}


static void
create_rowcol_ext_ilseq_info(struct _citrus_region *r)
{
	void *ptr;
	size_t ofs;

	ofs = 0;
	ptr = malloc(_CITRUS_MAPPER_STD_ROWCOL_EXT_ILSEQ_SIZE);
	if (ptr == NULL)
		err(EXIT_FAILURE, "malloc");

	put32(ptr, ofs, oob_mode); ofs++;
	put32(ptr, ofs, dst_ilseq); ofs++;

	_citrus_region_init(r, ptr, _CITRUS_MAPPER_STD_ROWCOL_EXT_ILSEQ_SIZE);
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
	void *serialized;
	FILE *fp;
	size_t size;
	int ret;

	/*
	 * build database
	 */
	CHKERR(ret, _citrus_db_factory_create, (&df, _citrus_db_hash_std, NULL));

	/* store type */
	CHKERR(ret, _citrus_db_factory_add_string_by_string,
	    (df, _CITRUS_MAPPER_STD_SYM_TYPE, _CITRUS_MAPPER_STD_TYPE_ROWCOL));

	/* store info */
	create_rowcol_info(&data);
	CHKERR(ret, _citrus_db_factory_add_by_string,
	    (df, _CITRUS_MAPPER_STD_SYM_INFO, &data, 1));

	/* ilseq extension */
	create_rowcol_ext_ilseq_info(&data);
	CHKERR(ret, _citrus_db_factory_add_by_string,
	    (df, _CITRUS_MAPPER_STD_SYM_ROWCOL_EXT_ILSEQ, &data, 1));

	/* store table */
	_citrus_region_init(&data, table, table_size*dst_unit_bits/8);
	CHKERR(ret, _citrus_db_factory_add_by_string,
	    (df, _CITRUS_MAPPER_STD_SYM_TABLE, &data, 1));

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
	CHKERR(ret, _citrus_db_factory_serialize,
	    (df, _CITRUS_MAPPER_STD_MAGIC, &data));
	if (fwrite(serialized, size, 1, fp) != 1)
		err(EXIT_FAILURE, "fwrite");

	fclose(fp);
}

static void
/*ARGSUSED*/
set_type(int type)
{

	if (done_flag & DF_TYPE) {
		warning("TYPE is duplicated. ignored this one");
		return;
	}

	map_type = type;

	done_flag |= DF_TYPE;
}

static void
/*ARGSUSED*/
set_name(char *str)
{

	if (done_flag & DF_NAME) {
		warning("NAME is duplicated. ignored this one");
		return;
	}

	map_name = str;

	done_flag |= DF_NAME;
}

static void
set_src_zone(uint32_t val)
{
	linear_zone_t *p;
	size_t i;

	if (done_flag & DF_SRC_ZONE) {
		warning("SRC_ZONE is duplicated. ignored this one");
		return;
	}
	rowcol_bits = val;

	/* sanity check */
	switch (rowcol_bits) {
	case 8: case 16: case 32:
		if (rowcol_len <= 32 / rowcol_bits)
			break;
	/*FALLTHROUGH*/
	default: 
		goto bad;
	}
	rowcol_mask = 1u << (rowcol_bits - 1);
	rowcol_mask |= rowcol_mask - 1;
	for (i = 0; i < rowcol_len; ++i) {
		p = &rowcol[i];
		if (p->end > rowcol_mask)
			goto bad;
	}
	done_flag |= DF_SRC_ZONE;
	return;

bad:
	yyerror("Illegal argument for SRC_ZONE");
}

static void
set_dst_invalid(uint32_t val)
{

	if (done_flag & DF_DST_INVALID) {
		warning("DST_INVALID is duplicated. ignored this one");
		return;
	}

	dst_invalid = val;

	done_flag |= DF_DST_INVALID;
}

static void
set_dst_ilseq(uint32_t val)
{

	if (done_flag & DF_DST_ILSEQ) {
		warning("DST_ILSEQ is duplicated. ignored this one");
		return;
	}

	dst_ilseq = val;

	done_flag |= DF_DST_ILSEQ;
}

static void
set_oob_mode(uint32_t val)
{

	if (done_flag & DF_OOB_MODE) {
		warning("OOB_MODE is duplicated. ignored this one");
		return;
	}

	oob_mode = val;

	done_flag |= DF_OOB_MODE;
}

static void
set_dst_unit_bits(uint32_t val)
{

	if (done_flag & DF_DST_UNIT_BITS) {
		warning("DST_UNIT_BITS is duplicated. ignored this one");
		return;
	}

	switch (val) {
	case 8:
		putfunc = &put8;
		dst_unit_bits = val;
		break;
	case 16:
		putfunc = &put16;
		dst_unit_bits = val;
		break;
	case 32:
		putfunc = &put32;
		dst_unit_bits = val;
		break;
	default:
		yyerror("Illegal argument for DST_UNIT_BITS");
	}
	done_flag |= DF_DST_UNIT_BITS;
}

static int
check_src(uint32_t begin, uint32_t end)
{
	linear_zone_t *p;
	size_t i;
	uint32_t m, n;

	if (begin > end)
		return (1);
	if (begin < end) {
		m = begin & ~rowcol_mask;
		n = end & ~rowcol_mask;
		if (m != n)
			return (1);
	}
	for (i = rowcol_len * rowcol_bits, p = &rowcol[0]; i > 0; ++p) {
		i -= rowcol_bits;
		m = (begin >> i) & rowcol_mask;
		if (m < p->begin || m > p->end)
			return (1);
	}
	if (begin < end) {
		n = end & rowcol_mask;
		--p;
		if (n < p->begin || n > p->end)
			return (1);
	}
	return (0);
}

static void
store(const linear_zone_t *lz, uint32_t dst, int inc)
{
	linear_zone_t *p;
	size_t i, ofs;
	uint32_t n;

	ofs = 0;
	for (i = rowcol_len * rowcol_bits, p = &rowcol[0]; i > 0; ++p) {
		i -= rowcol_bits;
		n = ((lz->begin >> i) & rowcol_mask) - p->begin;
		ofs = (ofs * p->width) + n;
	}
	n = lz->width;
	while (n-- > 0) {
		(*putfunc)(table, ofs++, dst);
		if (inc)
			dst++;
	}
}

static void
set_range(uint32_t begin, uint32_t end)
{
	linear_zone_t *p;

	if (rowcol_len >= _CITRUS_MAPPER_STD_ROWCOL_MAX)
		goto bad;
	p = &rowcol[rowcol_len++];

	if (begin > end)
		goto bad;
	p->begin = begin, p->end = end;
	p->width = end - begin + 1;

	return;

bad:
	yyerror("Illegal argument for SRC_ZONE");
}

static void
set_src(linear_zone_t *lz, uint32_t begin, uint32_t end)
{

	if (check_src(begin, end) != 0)
		yyerror("illegal zone");

	lz->begin = begin, lz->end = end;
	lz->width = end - begin + 1;

	src_next = end + 1;
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
}

static void
do_mkpv(FILE *in)
{
	FILE *out;
	int ret;

        /* dump pivot to file */
	out = output ? fopen(output, "wb") : stdout;

	if (out == NULL)
		err(EXIT_FAILURE, "fopen");

	ret = _citrus_pivot_factory_convert(out, in);
	fclose(out);
	if (ret && output)
		unlink(output); /* dump failure */
	if (ret)
		errx(EXIT_FAILURE, "%s\n", strerror(ret));
}

static void
usage(void)
{
	fprintf(stderr, "Usage: mkcsmapper [-d] [-m|-p] [-o outfile] [infile]\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	FILE *in = NULL;
	int ch, mkdb = 0, mkpv = 0;

	while ((ch = getopt(argc, argv, "do:mp")) != EOF) {
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
		case 'p':
			mkpv = 1;
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
	else if (mkpv)
		do_mkpv(in);
	else {
		yyin = in;
		yyparse();
	}

	return (0);
}
