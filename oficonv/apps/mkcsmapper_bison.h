/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_MEICHEL_DICOM_DCMTK_FULL_PUBLIC_OFICONV_APPS_MKCSMAPPER_BISON_H_INCLUDED
# define YY_YY_HOME_MEICHEL_DICOM_DCMTK_FULL_PUBLIC_OFICONV_APPS_MKCSMAPPER_BISON_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    R_TYPE = 258,                  /* R_TYPE  */
    R_NAME = 259,                  /* R_NAME  */
    R_SRC_ZONE = 260,              /* R_SRC_ZONE  */
    R_DST_UNIT_BITS = 261,         /* R_DST_UNIT_BITS  */
    R_DST_INVALID = 262,           /* R_DST_INVALID  */
    R_DST_ILSEQ = 263,             /* R_DST_ILSEQ  */
    R_BEGIN_MAP = 264,             /* R_BEGIN_MAP  */
    R_END_MAP = 265,               /* R_END_MAP  */
    R_INVALID = 266,               /* R_INVALID  */
    R_ROWCOL = 267,                /* R_ROWCOL  */
    R_ILSEQ = 268,                 /* R_ILSEQ  */
    R_OOB_MODE = 269,              /* R_OOB_MODE  */
    R_LN = 270,                    /* R_LN  */
    L_IMM = 271,                   /* L_IMM  */
    L_STRING = 272                 /* L_STRING  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 237 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper.y"

	uint32_t	 i_value;
	char		*s_value;
	linear_zone_t	 lz_value;

#line 87 "/home/meichel/dicom/dcmtk-full/public/oficonv/apps/mkcsmapper_bison.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_HOME_MEICHEL_DICOM_DCMTK_FULL_PUBLIC_OFICONV_APPS_MKCSMAPPER_BISON_H_INCLUDED  */
