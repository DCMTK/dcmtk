/*
 *
 *  Copyright (C) 2024, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Main function for a proxy stub that simply calls another
 *   command line tool and forwards environment, command line arguments,
 *   standard I/O streams, and return code.
 *
 */

#ifndef OFSTUB_H
#define OFSTUB_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofdefine.h"    /* for DCMTK_OFSTD_EXPORT */

/** main function to be called in a command line stub application that
 *  calls another command line tool located in the same directory
 *  and forwards all command line parameters, all environment variables,
 *  and the standard I/O stream to that command line tool.
 *  @param argc number of command line parameters as passed to main(), must be > 0
 *  @param argv array of command line parameters as passed to main()
 *  @param stubName name of the stub command line tool
 *  @param appName name of the real command line tool to be called
 *  @return value to be returned from main(). If the command line tool given as appName
 *    cannot be executed, returns an error code. Otherwise, on Posix platform, this
 *    function will never returns, as it calls execve(). The calling process will instead
 *    receive the return code of the callled tool. On Windows, the function will wait
 *    until the child process terminates and will then return the child process's
 *    return code. In any case, the stub can simply return the result of this function.
 */
DCMTK_OFSTD_EXPORT int OFstub_main(int argc, char** argv, const char *stubName, const char *appName);

#endif // #ifndef OFSTUB_H
