/*
** 
** Author: Gerd Ehlers      Created:  03-28-94
**                          Modified: 02-07-95
** 
** Module: dcdebug.cc
** 
** Purpose:
** Print debug information
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-03 15:09:54 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/Attic/dcdebug.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
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
** Revision 1.4  1997-07-03 15:09:54  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/04/16 16:02:41  andreas
** - set default debug mode to 0
**
**
*/


