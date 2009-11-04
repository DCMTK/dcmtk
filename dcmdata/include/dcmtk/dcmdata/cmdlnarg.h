/*
 *
 *  Copyright (C) 1994-2007, OFFIS
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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Provide prototype of command line argument gathering routine
 *  for OS environments which cannot pass arguments on the command line.
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2009-11-04 09:58:07 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/dcmtk/dcmdata/cmdlnarg.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef CMDLNARG_H
#define CMDLNARG_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

/** this function is used on certain operating systems (in particular, classic MacOS)
 *  and allows command line arguments to be input from stdin.
 *  MT level: Thread unsafe!
 *  @param argc the argc parameter of main() is passed in this parameter
 *  @param argv the argv parameter of main() is passed in this parameter
 *  @param progname program name
 */
void prepareCmdLineArgs(int& argc, char** argv, const char* progname = "aProgram");

#endif

/*
** CVS/RCS Log:
** $Log: cmdlnarg.h,v $
** Revision 1.9  2009-11-04 09:58:07  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.8  2007-11-29 14:30:35  meichel
** Updated doxygen API documentation
**
** Revision 1.7  2005/12/08 16:27:58  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.6  2001/06/01 15:48:31  meichel
** Updated copyright header
**
** Revision 1.5  2000/04/14 16:01:22  meichel
** Minor changes for thread safety.
**
** Revision 1.4  2000/03/08 16:26:10  meichel
** Updated copyright header.
**
** Revision 1.3  1999/03/31 09:24:28  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.2  1996/09/27 08:22:21  hewett
** Added defines to protect from multiple inclusion.
**
** Revision 1.1  1996/09/24 16:17:59  hewett
** Moved from dcmdata/apps and dcmnet/apps (now part of the dcmdata library).
**
** Revision 1.2  1996/09/03 11:39:01  hewett
** Added copyright information.
**
** Revision 1.1.1.1  1996/03/26 18:38:44  hewett
** Initial Release.
**
** Revision 1.1.1.1  1996/03/25 15:17:00  hewett
** dcmnet DICOM network libraries.  Adapted from CEN network libraries
** for use with the DICOM C++ dcmdata toolkit.
**
** Revision 1.1  1996/03/12 15:08:05  hewett
** Main program argc, argv handling for environments which do not make
** argc,argv available in main().
**
**
*/

