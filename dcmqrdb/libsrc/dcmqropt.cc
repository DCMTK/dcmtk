/*
 *
 *  Copyright (C) 1993-2017, OFFIS e.V.
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
 *  Module:  dcmqrdb
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQueryRetrieveOptions
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmqrdb/dcmqropt.h"

#define INCLUDE_CSTDARG
#include "dcmtk/ofstd/ofstdinc.h"

makeOFConditionConst(QR_EC_IndexDatabaseError, OFM_dcmqrdb, 1, OF_error, "Index database error");
makeOFConditionConst(QR_EC_InvalidPeer,        OFM_dcmqrdb, 2, OF_error, "Invalid peer for move operation");

DcmQueryRetrieveOptions::DcmQueryRetrieveOptions()
: allowShutdown_(OFFalse)
, bitPreserving_(OFFalse)
, correctUIDPadding_(OFFalse)
, disableGetSupport_(OFFalse)
, filepad_(0)
, groupLength_(EGL_recalcGL)
, ignoreStoreData_(OFFalse)
, itempad_(0)
, maxAssociations_(20)
, maxPDU_(ASC_DEFAULTMAXPDU)
, net_(NULL)
, networkTransferSyntax_(EXS_Unknown)
#ifndef DISABLE_COMPRESSION_EXTENSION
,  networkTransferSyntaxOut_(EXS_Unknown)
#endif
, paddingType_(EPD_withoutPadding)
, refuseMultipleStorageAssociations_(OFFalse)
, refuse_(OFFalse)
, rejectWhenNoImplementationClassUID_(OFFalse)
, requireFindForMove_(OFFalse)
, restrictMoveToSameAE_(OFFalse)
, restrictMoveToSameHost_(OFFalse)
, restrictMoveToSameVendor_(OFFalse)
, sequenceType_(EET_ExplicitLength)
#ifdef HAVE_FORK
, singleProcess_(OFFalse)
#else
, singleProcess_(OFTrue)
#endif
, supportPatientRoot_(OFTrue)
#ifdef NO_PATIENTSTUDYONLY_SUPPORT
, supportPatientStudyOnly_(OFFalse)
#else
, supportPatientStudyOnly_(OFTrue)
#endif
, supportStudyRoot_(OFTrue)
, useMetaheader_(OFTrue)
, keepDBHandleDuringAssociation_(OFTrue)
, writeTransferSyntax_(EXS_Unknown)
, blockMode_(DIMSE_BLOCKING)
, dimse_timeout_(0)
, acse_timeout_(30)
, associationConfigFile()
, incomingProfile()
, outgoingProfile()
{
}

DcmQueryRetrieveOptions::~DcmQueryRetrieveOptions()
{
}
