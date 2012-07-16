/*
 *
 *  Copyright (C) 1994-2012, OFFIS e.V.
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

#ifndef CMDLNARG_H
#define CMDLNARG_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcdefine.h"

/** this function is used on certain operating systems (in particular, classic MacOS)
 *  and allows command line arguments to be input from stdin.
 *  MT level: Thread unsafe!
 *  @param argc the argc parameter of main() is passed in this parameter
 *  @param argv the argv parameter of main() is passed in this parameter
 *  @param progname program name
 */
DCMTK_DCMDATA_EXPORT void prepareCmdLineArgs(int& argc, char** argv, const char* progname = "aProgram");

#ifdef HAVE_WINDOWS_H

/** this function is used on certain operating systems (in particular, classic MacOS)
 *  and allows command line arguments to be input from stdin.
 *  MT level: Thread unsafe!
 *  This is a Windows-specific version supporting the wide character encoding (UTF-16).
 *  @param argc the argc parameter of main() is passed in this parameter (not used)
 *  @param argv the argv parameter of main() is passed in this parameter (not used)
 *  @param progname program name (not used)
 */
DCMTK_DCMDATA_EXPORT void prepareCmdLineArgs(int& argc, wchar_t** argv, const char* progname = "aProgram");

#endif  // HAVE_WINDOWS_H

#endif
