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
 *  Update Date:      $Date: 2000-03-03 14:05:22 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdebug.h,v $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCDEBUG_H
#define DCDEBUG_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>
extern int DcmDebugLevel;
extern ostream *DcmDebugDevice;

#ifdef DEBUG

void debug_print(const char* text, ... );

// Set the debug level
#define SetDebugLevel(level) DcmDebugLevel = (level);

// debug prints a debug message in param if lev <= DcmDebugLevel. param has the
// format of the printf parameters (with round brackets)!
#define debug(lev, param)  { if ((lev) <= DcmDebugLevel) { *DcmDebugDevice << __FILE__ << ", LINE " << __LINE__ << ":"; debug_print param ; }}

// Cdebug does the same as debug but only if a condition cond is OFTrue
#define Cdebug(lev, cond, param) { if ((lev) <= DcmDebugLevel && (cond)) { *DcmDebugDevice << __FILE__ << ", LINE " << __LINE__ << ":"; debug_print param ; }}

#else  // DEBUG

#define SetDebugLevel(param)
#define debug(lev, param)
#define Cdebug(lev, cond, param)

#endif // DEBUG

#endif // DCDEBUG_H

/*
 * CVS/RCS Log:
 * $Log: dcdebug.h,v $
 * Revision 1.6  2000-03-03 14:05:22  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.5  1999/03/31 09:24:33  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
