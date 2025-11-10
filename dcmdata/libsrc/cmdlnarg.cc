/*
 *
 *  Copyright (C) 1996-2025, OFFIS e.V.
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
#include "dcmtk/ofstd/ofconsol.h"

#ifdef HAVE_IO_H
#include <io.h>
#endif

BEGIN_EXTERN_C
#include <fcntl.h>       /* for O_BINARY */
END_EXTERN_C

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"

void prepareCmdLineArgs(int& /* argc */, char** /* argv */,
                        const char* /* progname */)
{
#ifdef _WIN32
#ifndef DCMTK_GUI
#ifndef __CYGWIN__

#ifdef DCMTK_MERGE_STDERR_TO_STDOUT
    /* Map stderr onto stdout (cannot redirect stderr under Windows).
     * We also remove any buffering since Windows by default uses buffered mode
     * for stdout when not writing to the console. Since dcmtk uses mixed
     * stdout, stderr, cout and cerr, this results in _very_ mixed up output).
     */
    OFConsole::mergeStderrStdout();
#endif

#ifdef USE_BINARY_MODE_FOR_STDOUT_ON_WINDOWS
    /* use binary mode for stdout in order to be more consistent with common Unix behavior */
    setmode(fileno(stdout), O_BINARY);
#endif

#endif
#endif
#endif
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


