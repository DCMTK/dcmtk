/*
**
**  Copyright (C) 1996, OFFIS
**
**  This software and supporting documentation were developed by
**
**    Kuratorium OFFIS e.V.
**    Forschungsbereich 2: Kommunikationssysteme
**    Escherweg 2
**    D-26121 Oldenburg, Germany
**
**  for CEN/TC251/WG4 as a contribution to the Computer Assisted Radiology
**  (CAR) 1996 DICOM Demonstration.
**
**  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
**  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
**  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
**  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
**  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
**
**  Copyright of the software  and  supporting  documentation  is,  unless
**  otherwise stated, owned by OFFIS, and free access is hereby granted as
**  a license to  use  this  software,  copy  this  software  and  prepare
**  derivative works based upon this software.  However, any  distribution
**  of this software source code or supporting documentation or derivative
**  works  (source code and  supporting documentation)  must  include  the
**  three paragraphs of this copyright notice.
**
*/

/*
**
** Author: Andrew Hewett      Created:  12-03-96
**
** Module: cmdlnarg.h
**
** Purpose:
** Provide prototype of command line argument gathering routine
** for OS environments which cannot pass arguments on the command line.
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-09-27 08:22:21 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/cmdlnarg.h,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef CMDLNARG_H
#define CMDLNARG_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

/*
** prepareCmdLineArgs 
**
** Should do nothing on Unix OS's.
** On other OS's (e.g. MacOS with CW6) allows command line arguments
** to be input from the standard input.
*/
void prepareCmdLineArgs(int& argc, char** argv, 
			const char* progname = "aProgram");


#endif

/*
** CVS/RCS Log:
** $Log: cmdlnarg.h,v $
** Revision 1.2  1996-09-27 08:22:21  hewett
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
**
*/

