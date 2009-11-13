/*
 *
 *  Copyright (C) 1996-2009, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2009-11-13 14:41:54 $
 *  CVS/RCS Revision: $Revision: 1.22 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dctypes.h"

/*
** prepareCmdLineArgs 
**
** Should do nothing on Unix OS's.
** On other OS's (e.g. MacOS with CW6) allows command line arguments
** to be input from the standard input.
*/

#ifdef HAVE_EMPTY_ARGC_ARGV

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"

void prepareCmdLineArgs(int& argc, char* argv[], 
			const char* progname)
{
    const int bufsize = 2024;
    char buf[bufsize];
    char arg[1024];

    argv[0] = new char[strlen(progname)+1];
    strcpy(argv[0], progname);
    argc = 1;

    ofConsole.lockCout() << "CmdLineArgs-> ";
    ofConsole.unlockCout();	
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

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_IO_H
#include <io.h>
#endif

#include "dcmtk/ofstd/ofstream.h"

void prepareCmdLineArgs(int& /* argc */, char** /* argv */, 
			const char* /* progname */)
{
#ifdef _WIN32
#ifndef DCMTK_GUI
#ifndef __CYGWIN__
    /* Map stderr onto stdout (cannot redirect stderr under windows).
     * Remove any buffering (windows uses a 2k buffer for stdout when not
     * writing to the console.  since dcmtk uses mixed stdout, stderr 
     * cout and cerr, this results in _very_ mixed up output).
     */

    /* duplicate the stderr file descriptor be the same as stdout */ 
    close(fileno(stderr));
    int fderr = dup(fileno(stdout));
    if (fderr != fileno(stderr))
    {
        DCMDATA_ERROR("INTERNAL ERROR: cannot map stderr to stdout: " << strerror(errno));
    }

#ifndef NO_IOS_BASE_ASSIGN    
    /* make cout refer to cerr. This does not work with all iostream implementations :-( */
    cout = cerr;
#endif
    
    /* make stdout the same as stderr */
    *stdout = *stderr;

#ifndef __BORLANDC__  /* setvbuf on stdout/stderr does not work with Borland C++ */
    /* make sure the buffering is removed */
    if (setvbuf(stdout, NULL, _IONBF, 0 ) != 0 )
    {
        DCMDATA_ERROR("INTERNAL ERROR: cannot unbuffer stdout: " << strerror(errno));
    }
    if (setvbuf(stderr, NULL, _IONBF, 0 ) != 0 )
    {
        DCMDATA_ERROR("INTERNAL ERROR: cannot unbuffer stderr: " << strerror(errno));
    }
#endif /* __BORLANDC__ */
#endif
#endif
#endif

    /* no need to process the arguments */
}

#endif


/*
** CVS/RCS Log:
** $Log: cmdlnarg.cc,v $
** Revision 1.22  2009-11-13 14:41:54  joergr
** Fixed wrong header include (now, "oflog.h" instead of "ofconsol.h" required).
**
** Revision 1.21  2009-11-10 12:38:29  uli
** Fix compilation on windows.
**
** Revision 1.20  2009-11-04 09:58:08  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.19  2006-08-15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.18  2005/12/08 15:40:55  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.17  2004/01/16 13:51:38  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.16  2003/10/13 13:32:15  meichel
** Disabled setvbuf calls on stdout/stderr on Win32/Borland C++.
**
** Revision 1.15  2002/11/27 12:06:42  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.14  2002/04/16 13:43:14  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.13  2001/06/01 15:48:57  meichel
** Updated copyright header
**
** Revision 1.12  2000/04/14 15:55:02  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.11  2000/03/08 16:26:28  meichel
** Updated copyright header.
**
** Revision 1.10  2000/03/06 18:13:08  joergr
** Added missing include (reported by MSVC).
**
** Revision 1.9  2000/03/03 14:05:30  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.8  2000/02/23 15:11:45  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.7  2000/02/01 10:12:04  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.6  1999/05/04 12:20:20  joergr
** Minor changes to support Cygwin B20.1 (check __CYGWIN__ to distinguish from
** MSVC which also defines _WIN32).
**
** Revision 1.5  1999/04/21 15:45:10  meichel
** Now always including <windows.h> instead of <winsock.h> on Win32 platforms.
**   This makes sure that <winsock2.h> is used if available.
**
** Revision 1.4  1999/03/31 09:25:14  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.3  1999/03/17 11:09:21  meichel
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
*/

