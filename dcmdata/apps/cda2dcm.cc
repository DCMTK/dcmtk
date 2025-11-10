/*
 *
 *  Copyright (C) 2025, OFFIS e.V.
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
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Proxy stub that calls dcmencap
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstub.h"
#include "dcmtk/ofstd/ofstd.h"
#include <cstring>

int main(int argc, char** argv)
{
    // create an argv array that is one entry larger than the one specified by the user
    char **my_argv = new char *[argc+2];
    char filetype[100];
    OFStandard::strlcpy(filetype, "--filetype-cda", sizeof(filetype));

    // copy arguments, then add file type argument and NULL pointer
    memcpy(my_argv, argv, argc * sizeof(char *));
    my_argv[argc] = filetype;
    my_argv[argc+1] = NULL;

    // call stub
    int result;
    if (argc == 1)
    {
       // no command line arguments given. Just forward call.
       result = OFstub_main(argc, argv, "cda2dcm", "dcmencap");
    }
    else
    {
      // Forward call with additional command line argument.
      result = OFstub_main(argc+1, my_argv, "cda2dcm", "dcmencap");
    }

    // clean up (Windows only, on Posix systems the stub will not return)
    delete[] my_argv;
    return result;
}
