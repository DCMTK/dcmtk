/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Update Date:      $Date: 2002-11-27 12:06:43 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/Attic/dcdebug.cc,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDARG
#define INCLUDE_CSTDIO
#include "ofstdinc.h"

#include "dcdebug.h"
#include "ofconsol.h"

OFGlobal<int> DcmDebugLevel(0);

#ifdef DEBUG

void debug_print(const char* text, ... )
{
#ifdef HAVE_VPRINTF
    char buf[4096]; // we hope that debug messages are never larger than 4K.
    va_list argptr;
    va_start( argptr, text );
    vsprintf(buf, text, argptr );  // vsnprintf is better but not available everywhere
    va_end( argptr );
    // we don't lock or unlock the console because this is already done by the calling debug or Cdebug macro.
    ofConsole.getCerr() << buf << endl;
#else
#error Need vprintf to compile this code
#endif
}

#endif


/*
** CVS/RCS Log:
** $Log: dcdebug.cc,v $
** Revision 1.10  2002-11-27 12:06:43  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.9  2001/06/01 15:49:00  meichel
** Updated copyright header
**
** Revision 1.8  2000/04/14 15:45:31  meichel
** Dcmdata debug facility now uses ofConsole for output.
**
** Revision 1.7  2000/03/08 16:26:31  meichel
** Updated copyright header.
**
** Revision 1.6  2000/03/03 14:05:30  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.5  1999/03/31 09:25:19  meichel
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


