/*
 *
 *  Copyright (C) 1996-2012, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Provide prototype of command line argument gathering routine
 *  for OS environments which cannot pass arguments on the command line.
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

#else // HAVE_EMPTY_ARGC_ARGV

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_IO_H
#include <io.h>
#endif

BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>       /* for O_BINARY */
#endif
END_EXTERN_C

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"

void prepareCmdLineArgs(int& /* argc */, char** /* argv */,
                        const char* /* progname */)
{
#ifdef _WIN32
#ifndef DCMTK_GUI
#ifndef __CYGWIN__
    /* Map stderr onto stdout (cannot redirect stderr under Windows).
     * Remove any buffering (windows uses a 2k buffer for stdout when not
     * writing to the console.  since dcmtk uses mixed stdout, stderr
     * cout and cerr, this results in _very_ mixed up output).
     */

    /* first of all, check whether stderr and stdout file descriptors are
     * already the same, e.g. from a previous call of this function
     */
    if (fileno(stderr) != fileno(stdout))
    {
        /* duplicate the stderr file descriptor to be the same as stdout */
        close(fileno(stderr));
        int fderr = dup(fileno(stdout));
        if (fderr != fileno(stderr))
        {
            char buf[256];
            DCMDATA_ERROR("INTERNAL ERROR: cannot map stderr to stdout: "
                << OFStandard::strerror(errno, buf, sizeof(buf)));
        }
    }

    /* make stdout the same as stderr */
    *stdout = *stderr;

#ifdef USE_BINARY_MODE_FOR_STDOUT_ON_WINDOWS
    /* use binary mode for stdout in order to be more consistent with common Unix behavior */
    setmode(fileno(stdout), O_BINARY);
#endif

#ifndef __BORLANDC__  /* setvbuf on stdout/stderr does not work with Borland C++ */
    /* make sure the buffering is removed */
    if (setvbuf(stdout, NULL, _IONBF, 0 ) != 0 )
    {
        char buf[256];
        DCMDATA_ERROR("INTERNAL ERROR: cannot unbuffer stdout: "
            << OFStandard::strerror(errno, buf, sizeof(buf)));
    }
    if (setvbuf(stderr, NULL, _IONBF, 0 ) != 0 )
    {
        char buf[256];
        DCMDATA_ERROR("INTERNAL ERROR: cannot unbuffer stderr: "
            << OFStandard::strerror(errno, buf, sizeof(buf)));
    }
#endif /* __BORLANDC__ */
#endif
#endif
#endif

    /* no need to process the arguments */
}


#ifdef HAVE_WINDOWS_H

void prepareCmdLineArgs(int& argc, wchar_t** /* argv */,
                        const char* progname)
{
    // value of parameters is ignored anyway
    char** argv = NULL;
    prepareCmdLineArgs(argc, argv, progname);
}

#endif // HAVE_WINDOWS_H


#endif // HAVE_EMPTY_ARGC_ARGV
