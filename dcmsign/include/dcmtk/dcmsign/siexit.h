/*
 *
 *  Copyright (C) 1998-2020, OFFIS e.V.
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
 *  Module: dcmsign
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose: exit codes for dcmsign
 *
 */

#ifndef SIEXIT_H
#define SIEXIT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_OPENSSL

// exit code constants in addition to those defined in "dcmtk/ofstd/ofexit.h"

// input file errors (20-39)
#define EXITCODE_CANNOT_READ_TAG_FILE            30
#define EXITCODE_CANNOT_READ_TSQ_FILE            31
#define EXITCODE_CANNOT_READ_TSR_FILE            32
#define EXITCODE_CANNOT_READ_UID_FILE            33

// output file errors (40-59)
#define EXITCODE_CANNOT_WRITE_SUPPORT_FILE       46

// processing errors (80-99)
#define EXITCODE_CANNOT_ACCESS_SIGNATURE         80
#define EXITCODE_CANNOT_ACCESS_TS                81
#define EXITCODE_CANNOT_INSERT_TS                82
#define EXITCODE_SIGNATURE_REMOVAL_FAILED        83
#define EXITCODE_SIGNATURE_UID_NOT_FOUND         84
#define EXITCODE_SIGNATURE_CREATION_FAILED       85
#define EXITCODE_SYNTAX_ERROR_IN_TAG_FILE        86
#define EXITCODE_TS_CONSISTENCY_CHECK_FAILED     87

// application specific errors (100-119)
#define EXITCODE_NO_SIGNATURES_PRESENT           100
#define EXITCODE_SIGNATURE_VERIFICATION_FAILED   101
#define EXITCODE_SIGNATURE_VERIFICATION_POLICY   102

#endif
#endif
