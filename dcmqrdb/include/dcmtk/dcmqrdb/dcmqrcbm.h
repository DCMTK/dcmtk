/*
 *
 *  Copyright (C) 1993-2011, OFFIS e.V.
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
 *  Purpose: class DcmQueryRetrieveMoveContext
 *
 */

#ifndef DCMQRCBM_H
#define DCMQRCBM_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmqrdb/qrdefine.h"

class DcmQueryRetrieveDatabaseHandle;
class DcmQueryRetrieveOptions;
class DcmQueryRetrieveConfig;
class DcmQueryRetrieveDatabaseStatus;

/** this class maintains the context information that is passed to the
 *  callback function called by DIMSE_moveProvider.
 */
class DCMTK_DCMQRDB_EXPORT DcmQueryRetrieveMoveContext
{
public:
    /** constructor
     *  @param handle reference to database handle
     *  @param options options for the Q/R service
     *  @param cfg configuration for the Q/R service
     *  @param priorstatus prior DIMSE status
     *  @param assoc pointer to DIMSE association
     *  @param msgid DIMSE message ID
     *  @param pr DIMSE priority
     */
    DcmQueryRetrieveMoveContext(
      DcmQueryRetrieveDatabaseHandle& handle,
      const DcmQueryRetrieveOptions& options,
      const DcmQueryRetrieveConfig *cfg,
      DIC_US priorstatus,
      T_ASC_Association *assoc,
      DIC_US msgid,
      T_DIMSE_Priority pr)
    : dbHandle(handle)
    , options_(options)
    , priorStatus(priorstatus)
    , origAssoc(assoc)
    , subAssoc(NULL)
    , config(cfg)
    , assocStarted(OFFalse)
    , origMsgId(msgid)
//    , origAETitle()
//    , origHostName()
    , priority(pr)
    , ourAETitle()
//    , dstAETitle()
    , failedUIDs(NULL)
    , nRemaining(0)
    , nCompleted(0)
    , nFailed(0)
    , nWarning(0)
    {
      origAETitle[0] = '\0';
      origHostName[0] = '\0';
      dstAETitle[0] = '\0';
    }

    /** callback handler called by the DIMSE_storeProvider callback function.
     *  @param cancelled (in) flag indicating whether a C-CANCEL was received
     *  @param request original move request (in)
     *  @param requestIdentifiers original move request identifiers (in)
     *  @param responseCount move response count (in)
     *  @param response move response (out)
     *  @param stDetail status detail for move response (out)
     *  @param responseIdentifiers move response identifiers (out)
     */
    void callbackHandler(
        /* in */
        OFBool cancelled, T_DIMSE_C_MoveRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        /* out */
        T_DIMSE_C_MoveRSP *response, DcmDataset **stDetail,
        DcmDataset **responseIdentifiers);

    /** set the AEtitle under which this application operates
     *  @param ae AEtitle, is copied into this object.
     */
    void setOurAETitle(const char *ae)
    {
      if (ae) ourAETitle = ae; else ourAETitle.clear();
    }

private:

    /// private undefined copy constructor
    DcmQueryRetrieveMoveContext(const DcmQueryRetrieveMoveContext& other);

    /// private undefined assignment operator
    DcmQueryRetrieveMoveContext& operator=(const DcmQueryRetrieveMoveContext& other);

    void addFailedUIDInstance(const char *sopInstance);
    OFCondition performMoveSubOp(DIC_UI sopClass, DIC_UI sopInstance, char *fname);
    OFCondition buildSubAssociation(T_DIMSE_C_MoveRQ *request);
    OFCondition closeSubAssociation();
    void moveNextImage(DcmQueryRetrieveDatabaseStatus * dbStatus);
    void failAllSubOperations(DcmQueryRetrieveDatabaseStatus * dbStatus);
    void buildFailedInstanceList(DcmDataset ** rspIds);
    OFBool mapMoveDestination(
      const char *origPeer, const char *origAE,
      const char *dstAE, char *dstPeer, int *dstPort);
    OFCondition addAllStoragePresentationContexts(T_ASC_Parameters *params);

    /// reference to database handle
    DcmQueryRetrieveDatabaseHandle& dbHandle;

    /// reference to Q/R service options
    const DcmQueryRetrieveOptions& options_;

    /// prior DIMSE status
    DIC_US  priorStatus;

    /// pointer to original association on which the C-MOVE-RQ was received
    T_ASC_Association   *origAssoc;     /* association of requestor */

    /// pointer to sub-association for outgoing C-STORE-RQ
    T_ASC_Association   *subAssoc;      /* sub-association */

    /// pointer to Q/R configuration
    const DcmQueryRetrieveConfig *config;

    /// true if the association was started
    OFBool assocStarted;

    /// message id of request
    DIC_US origMsgId;

    /// title of requestor
    DIC_AE origAETitle;

    /// hostname of move requestor
    DIC_NODENAME origHostName;

    /// priority of move request
    T_DIMSE_Priority priority;

    /// our current title
    OFString ourAETitle;

    /// destination title for move
    DIC_AE dstAETitle;

    /// instance UIDs of failed store sub-ops
    char *failedUIDs;

    /// number of remaining sub-operations
    DIC_US nRemaining;

    /// number of completed sub-operations
    DIC_US nCompleted;

    /// number of failed sub-operations
    DIC_US nFailed;

    /// number of completed sub-operations that causes warnings
    DIC_US nWarning;

};

#endif
