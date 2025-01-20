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
 *  Module:  dcmdata
 *
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Proxy stub that calls dcmmkdir
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstub.h"

int main(int argc, char** argv)
{
    return OFstub_main(argc, argv, "dcmgpdir", "dcmmkdir");
}
