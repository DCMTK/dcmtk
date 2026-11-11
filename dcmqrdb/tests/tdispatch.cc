/*
 *
 *  Copyright (C) 2026, OFFIS e.V.
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
 *  Author:  Michael Onken
 *
 *  Purpose: test program for the DIMSE command / abstract syntax check
 *           performed before dispatching an incoming message.
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmqrdb/dcmqrsrv.h"
#include "dcmtk/dcmnet/dcmlayer.h"

OFTEST(dcmqrdb_commandAbstractSyntaxMatch)
{
    /* the regular combinations of DIMSE command and abstract syntax must
     * all be accepted
     */
    OFCHECK(DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_ECHO_RQ, UID_VerificationSOPClass));
    OFCHECK(DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_STORE_RQ, UID_SecondaryCaptureImageStorage));
    OFCHECK(DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_STORE_RQ, UID_CTImageStorage));
    OFCHECK(DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_FIND_RQ, UID_FINDStudyRootQueryRetrieveInformationModel));
    OFCHECK(DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_FIND_RQ, UID_FINDPatientRootQueryRetrieveInformationModel));
    OFCHECK(DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_MOVE_RQ, UID_MOVEStudyRootQueryRetrieveInformationModel));
    OFCHECK(DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_GET_RQ, UID_GETStudyRootQueryRetrieveInformationModel));
    /* a late C-CANCEL is ignored and may appear on any Q/R context */
    OFCHECK(DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_CANCEL_RQ, UID_FINDStudyRootQueryRetrieveInformationModel));
}

OFTEST(dcmqrdb_commandAbstractSyntaxMismatch)
{
    /* This is the actual regression test: a C-STORE request received on a
     * presentation context that was negotiated for the Verification SOP Class
     * must not be processed. Otherwise a peer can store (and, through the
     * quota mechanism, delete) objects in a storage area for which all
     * storage presentation contexts have been refused because it is
     * configured read-only.
     */
    OFCHECK(!DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_STORE_RQ, UID_VerificationSOPClass));
    OFCHECK(!DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_STORE_RQ, UID_FINDStudyRootQueryRetrieveInformationModel));
    OFCHECK(!DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_STORE_RQ, UID_GETStudyRootQueryRetrieveInformationModel));

    /* the same applies to every other command/abstract syntax mismatch */
    OFCHECK(!DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_ECHO_RQ, UID_SecondaryCaptureImageStorage));
    OFCHECK(!DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_FIND_RQ, UID_MOVEStudyRootQueryRetrieveInformationModel));
    OFCHECK(!DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_MOVE_RQ, UID_FINDStudyRootQueryRetrieveInformationModel));
    OFCHECK(!DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_GET_RQ, UID_MOVEStudyRootQueryRetrieveInformationModel));

    /* commands that this SCP does not implement at all, and the case where
     * no abstract syntax could be determined for the presentation context
     */
    OFCHECK(!DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_N_CREATE_RQ, UID_VerificationSOPClass));
    OFCHECK(!DcmQueryRetrieveSCP::isCommandAllowedForAbstractSyntax(
        DIMSE_C_STORE_RQ, NULL));
}

#ifndef WITH_OPENSSL

// This dummy function creates a dependency on libdcmnet that is required when compiling
// on NetBSD with libwrap support enabled and OpenSSL support disabled. Otherwise there
// would be a linker error complaining about unresolved symbols allow_severity and deny_severity.

DcmTransportLayer *tdispatch_dummyFunction()
{
  return new DcmTransportLayer();
}

#endif
