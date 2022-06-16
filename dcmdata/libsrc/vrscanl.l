/*
 *
 *  Copyright (C) 1997-2022, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmdata
 *
 *  Author: Hermann Grevemeyer et al.
 *
 *  Purpose:
 *    These are the regular expressions for the value representations
 *
 */

%option reentrant
/* Avoid compiler warnings about unused functions */
%option noyywrap nounput noinput

%{
#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "vrscani.h"
%}

/* Make this work on windows (we don't need interactivity anyway) */
%option nointeractive
/* This seems to be a bug: The .c file won't include it, but .h will
 * nevertheless.
 *  %option nounistd
 * Our workaround: Define YY_NO_UNISTD_H in vrscani.h and make
 * sure that header is included before vrscanl.h.
 */

default_charset_without_control_chars [\041-\133\135-\176][\040-\133\135-\176]*
charset_without_control_chars         [\040-\133\135-\176\240-\377\033]+
charset_with_control_chars            [\040-\176\240-\377\011\012\014\015\033]+
spaces                                [ ]*
zero_byte_padding                     \000?
dt_offset                             (\-1200)|(\+1400)|(((\-((0[1-9])|(1[0-1])))|(\+((0[1-9])|(1[0-3])))){tm_minute})
da_year                               (18[5-9][0-9])|(19[0-9]{2})|(20[0-4][0-9])
da_year_dub                           [0-9]{4}
da_month                              (0[1-9])|(1[0-2])
da_day                                (0[1-9])|([1-2][0-9])|(3[0-1])
tm_hour                               ([0-1][0-9])|(2[0-3])
tm_minute                             [0-5][0-9]
tm_second                             [0-5][0-9]
is_positive                           \+?0*(([0-9]{1,9})|(1[0-9]{9})|(20[0-9]{8})|(21[0-3][0-9]{7})|(214[0-6][0-9]{6})|(2147[0-3][0-9]{5})|(21474[0-7][0-9]{4})|(214748[0-2][0-9]{3})|(2147483[0-5][0-9]{2})|(21474836[0-3][0-9])|(214748364[0-7]))
is_negative                           \-0*(([0-9]{1,9})|(1[0-9]{9})|(20[0-9]{8})|(21[0-3][0-9]{7})|(214[0-6][0-9]{6})|(2147[0-3][0-9]{5})|(21474[0-7][0-9]{4})|(214748[0-2][0-9]{3})|(2147483[0-5][0-9]{2})|(21474836[0-3][0-9])|(214748364[0-8]))
pn_chars                              [\040-\074\076-\133\135\137-\176\240-\377\033]+
ur_chars                              [\041-\133\135-\176]+
pn_component_group                    {pn_chars}?(\^{pn_chars}?(\^{pn_chars}?(\^{pn_chars}?(\^{pn_chars}?)?)?)?)?

normalized-as       [0-9]{3}[DWMY]
normalized-da       {da_year}{da_month}{da_day}
normalized-da-old   {da_year_dub}\.{da_month}\.{da_day}
normalized-da-dub   {da_year_dub}{da_month}{da_day}
normalized-tm       {tm_hour}({tm_minute}({tm_second}(\.[0-9]{1,6})?)?)?
normalized-tm-old   {tm_hour}:{tm_minute}:{tm_second}\.[0-9]{6}
normalized-ds       ([\-\+]?[0-9]*[\.]?[0-9]+)|([\-\+]?[0-9]+[\.]?[0-9]*)|([\-\+]?[0-9]*[\.]?[0-9]+[Ee][\+\-]?[0-9]+)|([\-\+]?[0-9]+[\.]?[0-9]*[Ee][\+\-]?[0-9]+)
normalized-dt       {da_year}({da_month}({da_day}({normalized-tm}{dt_offset}?)?)?)?
normalized-dt-dub   {da_year_dub}({da_month}({da_day}({normalized-tm}{dt_offset}?)?)?)?
normalized-is       {is_positive}|{is_negative}
normalized-ui       (([0-9]\.)|([1-9][0-9]+\.))*(([0-9])|([1-9][0-9]+))
normalized-cs       [0-9A-Z _]+
normalized-pn       {pn_component_group}(\={pn_component_group}(\={pn_component_group})?)?
normalized-pn-old   {pn_chars}
normalized-lo-sh    {charset_without_control_chars}
normalized-ae       {default_charset_without_control_chars}
normalized-lt-st-ut {charset_with_control_chars}
normalized-ur       {ur_chars}
normalized-uc       {charset_without_control_chars}

as       {normalized-as}
da       {normalized-da}
da-old   {normalized-da-old}
da-dub   {normalized-da-dub}
tm       {normalized-tm}{spaces}
tm-old   {normalized-tm-old}{spaces}
ds       {spaces}{normalized-ds}{spaces}
dt       {normalized-dt}{spaces}
dt-dub   {normalized-dt-dub}{spaces}
is       {spaces}{normalized-is}{spaces}
ui       {normalized-ui}{zero_byte_padding}
cs       {spaces}{normalized-cs}{spaces}
pn       {normalized-pn}{spaces}
pn-old   {normalized-pn-old}{spaces}
lo-sh    {spaces}{normalized-lo-sh}{spaces}
ae       {spaces}{normalized-ae}{spaces}
lt-st-ut {normalized-lt-st-ut}{spaces}
ur       {normalized-ur}{spaces}
uc       {normalized-uc}{spaces}

%%

as{as}?(\\{as}?)*          {return 1;  /* AS */}
da{da}?(\\{da}?)*          {return 2;  /* DA */}
da{da-old}?(\\{da-old}?)*  {return 3;  /* OLD_DA */}
da{da-dub}?(\\{da-dub}?)*  {return 17; /* dubious DA */}
tm{tm}?(\\{tm}?)*          {return 4;  /* TM */}
tm{tm-old}?(\\{tm-old}?)*  {return 5;  /* OLD_TM */}
ds{ds}?(\\{ds}?)*          {return 6;  /* DS */}
dt{dt}?(\\{dt}?)*          {return 7;  /* DT */}
dt{dt-dub}?(\\{dt-dub}?)*  {return 18; /* dubious DT */}
is{is}?(\\{is}?)*          {return 8;  /* IS */}
ui{ui}?(\\{ui}?)*          {return 9;  /* UI */}
cs{cs}?(\\{cs}?)*          {return 10; /* CS */}
pn{pn}?(\\{pn}?)*          {return 11; /* PN */}
oldpn{pn-old}?(\\{pn-old}?)* {return 15; /* OLD_PN: not used any longer within DCMTK */}
lo{lo-sh}?(\\{lo-sh}?)*    {return 12; /* LO | SH */}
ae{ae}?(\\{ae}?)*          {return 13; /* AE */}
lt{lt-st-ut}               {return 14; /* LT | ST | UT */}
ur{ur}                     {return 19; /* UR */}
uc{uc}                     {return 20; /* UC */}
                           /* . doesn't match \n, this needs a special case! */
\n                         |
.                          {return 16; /* UNKNOWN */;}

%%
