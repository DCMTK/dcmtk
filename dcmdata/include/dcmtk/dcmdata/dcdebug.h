/*
 *
 *  Copyright (C) 1994-2009, OFFIS
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2009-11-04 09:58:07 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCDEBUG_H
#define DCDEBUG_H

#warning Remove dcdebug.h as soon as nothing uses it anymore (= soon!)
#define SetDebugLevel(param) { int SetDebugLevel_was_removed; }

#endif // DCDEBUG_H


/*
 * CVS/RCS Log:
 * $Log: dcdebug.h,v $
 * Revision 1.15  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.14  2007-11-29 14:30:35  meichel
 * Updated doxygen API documentation
 *
 * Revision 1.13  2005/12/08 16:28:04  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.12  2005/11/28 15:53:16  meichel
 * Renamed macros in dcdebug.h
 *
 * Revision 1.11  2004/01/16 14:06:32  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.10  2002/04/16 13:41:43  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.9  2001/06/01 15:48:35  meichel
 * Updated copyright header
 *
 * Revision 1.8  2000/04/14 15:45:30  meichel
 * Dcmdata debug facility now uses ofConsole for output.
 *
 * Revision 1.7  2000/03/08 16:26:12  meichel
 * Updated copyright header.
 *
 * Revision 1.6  2000/03/03 14:05:22  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.5  1999/03/31 09:24:33  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
