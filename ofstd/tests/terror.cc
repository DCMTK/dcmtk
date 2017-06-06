/*
 *
 *  Copyright (C) 2017, OFFIS e.V.
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
 *  Module: ofstd
 *
 *  Author: Nikolas Goldhammer
 *
 *  Purpose: test program for OFerror_code et al.
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oferror.h"
#include <iostream>
#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#endif

OFTEST(ofstd_error)
{
    // test constructor and operators
    OFerror_code ec_success;
    if(ec_success)
    {
        OFCHECK_FAIL("Error: 'Success' should not be an error.");
        return;
    }

    OFCHECK(ec_success == OFerror_code(0, OFsystem_category()));

    OFerror_code ec_error(42, OFsystem_category());
    if(!ec_error)
    {
        OFCHECK_FAIL("Error: This should be an error and not return 0.");
        return;
    }

    OFerror_code ec_error_generic(43, OFgeneric_category());
    if(ec_error.category() != ec_success.category())
    {
        OFCHECK_FAIL("Error: Categories should be equal.");
        return;
    }

    if(ec_error.category() == ec_error_generic.category())
    {
        OFCHECK_FAIL("Error: Categories should not be equal.");
        return;
    }

    // test assignment
    ec_error.assign(23, OFgeneric_category());
    OFCHECK(ec_error.value() == 23);
    OFCHECK(ec_error.category() == OFgeneric_category());

    // test system and network error functions
#ifdef _WIN32
    ec_error.assign(301, OFsystem_category());
    SetLastError(301);
#else
    ec_error.assign(71, OFsystem_category());
    errno = 71;
#endif
    OFCHECK(OFStandard::getLastSystemErrorCode() == ec_error);

#ifdef _WIN32
    ec_error.assign(WSA_IO_INCOMPLETE, OFsystem_category());
    WSASetLastError(WSA_IO_INCOMPLETE);
#elif __OpenBSD__
    ec_error.assign(EPROTOTYPE, OFsystem_category());
    errno = EPROTOTYPE;
#else
    ec_error.assign(EPROTO, OFsystem_category());
    errno = EPROTO;
#endif
    OFCHECK(OFStandard::getLastNetworkErrorCode() == ec_error);
}
