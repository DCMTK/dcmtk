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
** Module: cmdlnarg.cc
**
** Purpose:
** Provide a command line argument gathering routine
** for OS environments which cannot pass arguments on the command line.
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1999-03-17 11:09:21 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/cmdlnarg.cc,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "cmdlnarg.h"

/*
** prepareCmdLineArgs 
**
** Should do nothing on Unix OS's.
** On other OS's (e.g. MacOS with CW6) allows command line arguments
** to be input from the standard input.
*/

#ifdef HAVE_EMPTY_ARGC_ARGV

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <iostream.h>
#include <sstream.h>

void prepareCmdLineArgs(int& argc, char* argv[], 
			const char* progname)
{
    const int bufsize = 2024;
    char buf[bufsize];
    char arg[1024];

    argv[0] = new char[strlen(progname)+1];
    strcpy(argv[0], progname);
    argc = 1;
	
    cout << "CmdLineArgs-> ";
    cin.getline(buf, bufsize);

    istringstream is(buf);
    
    arg[0] = '\0';
    while (is.good()) {
        is >> arg;
        if (strlen(arg) > 0) {
            argv[argc] = new char[strlen(arg)+1];
            strcpy(argv[argc], arg);
            argc++;
        }
        arg[0] = '\0';
    }
	
}

#else

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <string.h>
#include <iostream.h>
#ifdef HAVE_IO_H
#include <io.h>
#endif

void prepareCmdLineArgs(int& /* argc */, char** /* argv */, 
			const char* /* progname */)
{
#ifdef windows
    /* Map stderr onto stdout (cannot redirect stderr under windows).
     * Remove any buffering (windows uses a 2k buffer for stdout when not
     * writing to the console.  since dcmtk uses mixed stdout, stderr 
     * cout and cerr, this results in _very_ mixed up output).
     */

    /* duplicate the stderr file descriptor be the same as stdout */ 
    close(fileno(stderr));
    int fderr = dup(fileno(stdout));
    if (fderr != fileno(stderr)) {
        fprintf(stderr, "INTERNAL ERROR: cannot map stderr to stdout: ");
        perror(NULL);
    }
    /* make cout refer to cerr */
    cout = cerr;
    /* make stdout the same as stderr */
    *stdout = *stderr;

    /* make sure the buffering is removed */
    if (setvbuf(stdout, NULL, _IONBF, 0 ) != 0 ) {
        fprintf(stderr, "INTERNAL ERROR: cannot unbuffer stdout: ");
        perror(NULL);
    }
    if (setvbuf(stderr, NULL, _IONBF, 0 ) != 0 ) {
        fprintf(stderr, "INTERNAL ERROR: cannot unbuffer stderr: ");
        perror(NULL);
    }
#endif

    /* no need to process the arguments */
}

#endif


/*
** CVS/RCS Log:
** $Log: cmdlnarg.cc,v $
** Revision 1.3  1999-03-17 11:09:21  meichel
** Added code to prepareCmdLineArgs() to redirect stderr, cout,
**   cerr to stdout on Windows and make output unbuffered.
**   This allows to redirect the output of DCMTK tools to file on Windows.
**
** Revision 1.2  1997/01/13 15:39:23  hewett
** Now includes cmdlnarg.h
**
** Revision 1.1  1996/09/24 16:18:41  hewett
** Moved from dcmdata/apps and dcmnet/apps to be part of the dcmdata library.
**
** Revision 1.2  1996/09/03 11:39:00  hewett
** Added copyright information.
**
** Revision 1.1.1.1  1996/03/26 18:38:44  hewett
** Initial Release.
**
** Revision 1.1.1.1  1996/03/25 15:17:00  hewett
** dcmnet DICOM network libraries.  Adapted from CEN network libraries
** for use with the DICOM C++ dcmdata toolkit.
**
** Revision 1.1  1996/03/12 15:08:06  hewett
** Main program argc, argv handling for environments which do not make
** argc,argv available in main().
**
**
**
*/

