/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:04:24 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log:
 *  $Log: vrscanl.l,v $
 *  Revision 1.6  2010-10-14 13:04:24  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.5  2010-09-03 07:26:19  uli
 *  Make the VR scanner work on windows again.
 *
 *  Revision 1.4  2010-09-02 12:02:06  uli
 *  Use longjmp() for error handling in the VR scanner.
 *
 *  Revision 1.3  2010-09-02 10:37:37  uli
 *  Fix a compiler warning about an unused function.
 *
 *  Revision 1.2  2010-09-02 09:23:15  uli
 *  Made the VR scanner reentrant again.
 *
 *  Revision 1.1  2010-08-26 12:29:48  uli
 *  Ported vrscan from ancient flex++ to current flex version.
 *
 *  Revision 1.15  2009-08-03 09:20:39  joergr
 *  Fixed various bugs and inconsistencies in the grammar of the VR scanner.
 *
 *  Revision 1.14  2009-05-11 14:43:55  joergr
 *  Fixed wrong definition of value representation DS (Decimal String): According
 *  to the referenced standard (ANSI X3.9 - 1978), the sign symbol after the "E"
 *  or "e" is optional).
 *
 *  Revision 1.13  2008-07-18 13:55:57  meichel
 *  New test for dubious date field (with years before 1850 or after 2050)
 *    generating new warning message W-33.
 *
 *  Revision 1.12  2006-08-16 16:31:34  meichel
 *  Updated all code in module dcmcheck to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.11  2003/08/26 16:13:06  meichel
 *  Extended VR scanner to explicitly test for IS value range.
 *    Added warning for retired PN form (without caret).
 *
 *  Revision 1.10  2003/08/26 15:10:37  meichel
 *  Extended VR scanner to detect leading zeroes in UIDs and illegal values
 *    in date and time expressions.
 *
 *  Revision 1.9  2003/08/25 15:42:59  meichel
 *  Changed VR scanner to allow empty values in multi-VM elements.
 *    This is explicitly needed, for example, for ImageType in DX objects.
 *
 *  Revision 1.8  2001/01/18 15:14:14  meichel
 *  Updated copyright header.
 *
 *  Revision 1.7  2000/03/08 16:11:49  meichel
 *  Updated copyright header.
 *
 *  Revision 1.6  2000/03/03 14:21:58  meichel
 *  Implemented library support for redirecting error messages into memory
 *    instead of printing them to stdout/stderr for GUI applications.
 *
 *  Revision 1.5  1998/07/02 09:03:16  meichel
 *  Some code purifications to avoid gcc 2.8.1 -Weffc++ warnings.
 *
 *  Revision 1.4  1997/09/24 13:04:13  hewett
 *  Corrected default and charset_without_control_chars regular expressions used for checking
 *  the allowed character repertoirs for VRs.  The '+' character was missing
 *  as a valid character.
 *
 *  Revision 1.3  1997/07/09 16:33:52  meichel
 *  Adapted dcmcheck to use the new OFString and OFBool classes instead
 *    of Gnu libg++. Removed references to Gnu-specific code (e.g. <builtin.h>).
 *  #includes in header files now restricted to required minimum,
 *    speeds up compilation and keeps the code lean.
 *  reimplementation of some helper functions (hex.cc, tools.cc)
 *    to improve efficiency.
 *  Added copyright message to all source files.
 *
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
/* This seems to be a bug: The .c file won't include it, but .h will still.
 * %option nounistd
 */
%{
static int isatty(int fd) { return 0; }
%}

default_charset_without_control_chars [\041-\133\135-\176][\040-\133\135-\176]*
charset_without_control_chars         [\040-\133\135-\176\240-\377\033]+
charset_with_control_chars            [\040-\176\240-\377\012\014\015\033]+
spaces                                [ ]*
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
pn_chars                              {spaces}[\041-\074\076-\133\135\137-\176\240-\377\033][\040-\074\076-\133\135\137-\176\240-\377\033]*
pn_component_group                    {pn_chars}?\^{pn_chars}?(\^{pn_chars}?(\^{pn_chars}?(\^{pn_chars}?)?)?)?

normalized-as       [0-9]{3}[DWMY]
normalized-da       {da_year}{da_month}{da_day}
normalized-da-old   {da_year_dub}\.{da_month}\.{da_day}
normalized-da-dub   {da_year_dub}{da_month}{da_day}
normalized-tm       {tm_hour}({tm_minute}({tm_second}(\.[0-9]{1,6})?)?)?
normalized-tm-old   {tm_hour}:{tm_minute}:{tm_second}\.[0-9]{6}
normalized-ds       ([\-\+]?[0-9]*[\.]?[0-9]+)|([\-\+]?[0-9]+[\.]?[0-9]*)|([\-\+]?[0-9]*[\.]?[0-9]+[Ee][\+\-]?[0-9]+)|([\-\+]?[0-9]+[\.]?[0-9]*[Ee][\+\-]?[0-9]+)
normalized-dt       {normalized-da}{normalized-tm}{dt_offset}?
normalized-dt-dub   {da_year_dub}{da_month}{da_day}{normalized-tm}{dt_offset}?
normalized-is       {is_positive}|{is_negative}
normalized-ui       (([0-9]\.)|([1-9][0-9]+\.))*(([0-9])|([1-9][0-9]+))
normalized-cs       [0-9A-Z _]+
normalized-pn       ({pn_component_group}(\={pn_component_group}?(\={pn_component_group}?)?)?)|(\={pn_component_group}?(\={pn_component_group}?)?)
normalized-pn-old   {pn_chars}
normalized-lo-sh    {charset_without_control_chars}
normalized-ae       {default_charset_without_control_chars}
normalized-lt-st-ut {charset_with_control_chars}

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
ui       {normalized-ui}
cs       {spaces}{normalized-cs}{spaces}
pn       {normalized-pn}{spaces}
pn-old   {normalized-pn-old}{spaces}
lo-sh    {spaces}{normalized-lo-sh}{spaces}
ae       {spaces}{normalized-ae}{spaces}
lt-st-ut {normalized-lt-st-ut}{spaces}

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
pn{pn-old}?(\\{pn-old}?)*  {return 15; /* OLD_PN */}
lo{lo-sh}?(\\{lo-sh}?)*    {return 12; /* LO | SH */}
ae{ae}?(\\{ae}?)*          {return 13; /* AE */}
lt{lt-st-ut}               {return 14; /* LT | ST | UT */}
.                          {return 16; /* UNKNOWN */;}

%%
