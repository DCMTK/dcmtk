/*
 *
 *  Copyright (C) 1994-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Print debug information
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:25:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/Attic/dcdebug.cc,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdarg.h>
#include "dcdebug.h"

#ifndef DEBUGLEVEL
#define DEBUGLEVEL 0
#endif

#ifdef DEBUG
int DcmDebugLevel = DEBUGLEVEL;
FILE * DcmDebugDevice = stdout;

void debug_print(const char* text, ... )
{
#ifdef HAVE_VPRINTF
    va_list argptr;
    va_start( argptr, text );
    vfprintf(DcmDebugDevice, text, argptr );
    va_end( argptr );
    fprintf(DcmDebugDevice, "\n" );
#elif HAVE_DOPRNT
#else
#error  I need vprintf or doprnt
#endif
}

#endif


/*
** CVS/RCS Log:
** $Log: dcdebug.cc,v $
** Revision 1.5  1999-03-31 09:25:19  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.4  1997/07/03 15:09:54  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/04/16 16:02:41  andreas
** - set default debug mode to 0
**
*/


