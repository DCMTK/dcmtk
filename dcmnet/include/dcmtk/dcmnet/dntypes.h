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
 *  Module:  dcmnet
 *
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: global type and constant definitions
 *
 */

#ifndef DNTYPES_H
#define DNTYPES_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmnet/dndefine.h"

#ifdef _WIN32
typedef OFuintptr_t DcmNativeSocketType;
#define DCMNET_INVALID_SOCKET INVALID_SOCKET
#elif !defined(DOXYGEN)
typedef int DcmNativeSocketType;
#define DCMNET_INVALID_SOCKET -1
#else
/// The data type for representing a TCP socket that is used by the underlying network API.
typedef /* unspecified */ DcmNativeSocketType;
#endif

#endif /* !DNTYPES_H */
