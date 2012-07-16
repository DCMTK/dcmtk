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
 *  Purpose: class DcmQueryRetrieveGetContext
 *
 */

#ifndef DCMQRCBG_H
#define DCMQRCBG_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmqrdb/qrdefine.h"

class DcmQueryRetrieveDatabaseHandle;
class DcmQueryRetrieveOptions;
class DcmQueryRetrieveDatabaseStatus;

/** this class maintains the context information that is passed to the
 *  callback function called by DIMSE_getProvider.
 */
class DCMTK_DCMQRDB_EXPORT DcmQueryRetrieveGetContext
{
public:
    /** constructor
     *  @param handle reference to database handle
     *  @param options options for the Q/R service
     *  @param priorstatus prior DIMSE status
     *  @param origassoc pointer to DIMSE association
     *  @param origmsgid DIMSE message ID
     *  @param prior DIMSE priority
     *  @param origpresid presentation context ID
     */
    DcmQueryRetrieveGetContext(DcmQueryRetrieveDatabaseHandle& handle,
      const DcmQueryRetrieveOptions& options,
      DIC_US priorstatus,
      T_ASC_Association *origassoc,
      DIC_US origmsgid,
      T_DIMSE_Priority prior,
      T_ASC_PresentationContextID origpresid)
    : dbHandle(handle)
    , options_(options)
    , priorStatus(priorstatus)
    , origAssoc(origassoc)
    , assocStarted(OFFalse)
    , origPresId(origpresid)
    , origMsgId(origmsgid)
    , origAETitle()
//    , origHostName()
    , priority(prior)
    , ourAETitle()
    , failedUIDs(NULL)
    , nRemaining(0)
    , nCompleted(0)
    , nFailed(0)
    , nWarning(0)
    , getCancelled(OFFalse)
    {
      origHostName[0] = '\0';
    }

    /** set the AEtitle under which this application operates
     *  @param ae AEtitle, is copied into this object.
     */
    void setOurAETitle(const char *ae)
    {
      if (ae) ourAETitle = ae; else ourAETitle.clear();
    }

    /** callback handler called by the DIMSE_storeProvider callback function.
     *  @param cancelled (in) flag indicating whether a C-CANCEL was received
     *  @param request original get request (in)
     *  @param requestIdentifiers original get request identifiers (in)
     *  @param responseCount get response count (in)
     *  @param response get response (out)
     *  @param stDetail status detail for get response (out)
     *  @param responseIdentifiers get response identifiers (out)
     */
    void callbackHandler(
        /* in */
        OFBool cancelled, T_DIMSE_C_GetRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        /* out */
        T_DIMSE_C_GetRSP *response, DcmDataset **stDetail,
        DcmDataset **responseIdentifiers);

private:

    /// private undefined copy constructor
    DcmQueryRetrieveGetContext(const DcmQueryRetrieveGetContext& other);

    /// private undefined assignment operator
    DcmQueryRetrieveGetContext& operator=(const DcmQueryRetrieveGetContext& other);

    void addFailedUIDInstance(const char *sopInstance);
    OFCondition performGetSubOp(DIC_UI sopClass, DIC_UI sopInstance, char *fname);
    void getNextImage(DcmQueryRetrieveDatabaseStatus * dbStatus);
    void buildFailedInstanceList(DcmDataset ** rspIds);

    /// reference to database handle
    DcmQueryRetrieveDatabaseHandle& dbHandle;

    /// reference to Q/R service options
    const DcmQueryRetrieveOptions& options_;

    /// prior DIMSE status
    DIC_US  priorStatus;

    /// pointer to association on which the C-GET-RQ was received
    T_ASC_Association   *origAssoc;

    /// true if the association was started
    OFBool assocStarted;

    /// presentation context id of request
    T_ASC_PresentationContextID origPresId;

    /// message id of request
    DIC_US origMsgId;

    /// title of requestor
    OFString origAETitle;

    /// hostname of requestor
    DIC_NODENAME origHostName;


    /// priority of request
    T_DIMSE_Priority priority;

    /// our current title
    OFString ourAETitle;

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

    /// true if the get sub-operations have been cancelled
    OFBool getCancelled;

};

#endif
