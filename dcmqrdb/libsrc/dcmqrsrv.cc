/*
 *
 *  Copyright (C) 1993-2010, OFFIS e.V.
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
 *  Purpose: class DcmQueryRetrieveSCP
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:36 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmqrdb/dcmqrsrv.h"
#include "dcmtk/dcmqrdb/dcmqropt.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmqrdb/dcmqrdba.h"
#include "dcmtk/dcmqrdb/dcmqrcbf.h"    /* for class DcmQueryRetrieveFindContext */
#include "dcmtk/dcmqrdb/dcmqrcbm.h"    /* for class DcmQueryRetrieveMoveContext */
#include "dcmtk/dcmqrdb/dcmqrcbg.h"    /* for class DcmQueryRetrieveGetContext */
#include "dcmtk/dcmqrdb/dcmqrcbs.h"    /* for class DcmQueryRetrieveStoreContext */


static void findCallback(
        /* in */
        void *callbackData,
        OFBool cancelled, T_DIMSE_C_FindRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        /* out */
        T_DIMSE_C_FindRSP *response,
        DcmDataset **responseIdentifiers,
        DcmDataset **stDetail)
{
  DcmQueryRetrieveFindContext *context = OFstatic_cast(DcmQueryRetrieveFindContext *, callbackData);
  context->callbackHandler(cancelled, request, requestIdentifiers, responseCount, response, responseIdentifiers, stDetail);
}


static void getCallback(
        /* in */
        void *callbackData,
        OFBool cancelled, T_DIMSE_C_GetRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        /* out */
        T_DIMSE_C_GetRSP *response, DcmDataset **stDetail,
        DcmDataset **responseIdentifiers)
{
  DcmQueryRetrieveGetContext *context = OFstatic_cast(DcmQueryRetrieveGetContext *, callbackData);
  context->callbackHandler(cancelled, request, requestIdentifiers, responseCount, response, stDetail, responseIdentifiers);
}


static void moveCallback(
        /* in */
        void *callbackData,
        OFBool cancelled, T_DIMSE_C_MoveRQ *request,
        DcmDataset *requestIdentifiers, int responseCount,
        /* out */
        T_DIMSE_C_MoveRSP *response, DcmDataset **stDetail,
        DcmDataset **responseIdentifiers)
{
  DcmQueryRetrieveMoveContext *context = OFstatic_cast(DcmQueryRetrieveMoveContext *, callbackData);
  context->callbackHandler(cancelled, request, requestIdentifiers, responseCount, response, stDetail, responseIdentifiers);
}


static void storeCallback(
    /* in */
    void *callbackData,
    T_DIMSE_StoreProgress *progress,    /* progress state */
    T_DIMSE_C_StoreRQ *req,             /* original store request */
    char *imageFileName,                /* being received into */
    DcmDataset **imageDataSet,          /* being received into */
    /* out */
    T_DIMSE_C_StoreRSP *rsp,            /* final store response */
    DcmDataset **stDetail)
{
  DcmQueryRetrieveStoreContext *context = OFstatic_cast(DcmQueryRetrieveStoreContext *, callbackData);
  context->callbackHandler(progress, req, imageFileName, imageDataSet, rsp, stDetail);
}


/*
 * ============================================================================================================
 */


DcmQueryRetrieveSCP::DcmQueryRetrieveSCP(
  const DcmQueryRetrieveConfig& config,
  const DcmQueryRetrieveOptions& options,
  const DcmQueryRetrieveDatabaseHandleFactory& factory)
: config_(&config)
, dbCheckFindIdentifier_(OFFalse)
, dbCheckMoveIdentifier_(OFFalse)
, factory_(factory)
, options_(options)
{
}


OFCondition DcmQueryRetrieveSCP::dispatch(T_ASC_Association *assoc, OFBool correctUIDPadding)
{
    OFCondition cond = EC_Normal;
    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presID;
    OFBool firstLoop = OFTrue;

    // this while loop is executed exactly once unless the "keepDBHandleDuringAssociation_"
    // flag is not set, in which case the inner loop is executed only once and this loop
    // repeats for each incoming DIMSE command. In this case, the DB handle is created
    // and released for each DIMSE command.
    while (cond.good())
    {
        /* Create a database handle for this association */
        DcmQueryRetrieveDatabaseHandle *dbHandle = factory_.createDBHandle(
              assoc->params->DULparams.callingAPTitle,
          assoc->params->DULparams.calledAPTitle, cond);

        if (cond.bad())
        {
          DCMQRDB_ERROR("dispatch: cannot create DB Handle");
          return cond;
        }

        if (dbHandle == NULL)
        {
          // this should not happen, but we check it anyway
          DCMQRDB_ERROR("dispatch: cannot create DB Handle");
          return EC_IllegalCall;
        }

        dbHandle->setIdentifierChecking(dbCheckFindIdentifier_, dbCheckMoveIdentifier_);
        firstLoop = OFTrue;

        // this while loop is executed exactly once unless the "keepDBHandleDuringAssociation_"
        // flag is set, in which case the DB handle remains open until something goes wrong
        // or the remote peer closes the association
        while (cond.good() && (firstLoop || options_.keepDBHandleDuringAssociation_) )
        {
            firstLoop = OFFalse;
            cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL);

            /* did peer release, abort, or do we have a valid message ? */
            if (cond.good())
            {
                /* process command */
                switch (msg.CommandField) {
                case DIMSE_C_ECHO_RQ:
                    cond = echoSCP(assoc, &msg.msg.CEchoRQ, presID);
                    break;
                case DIMSE_C_STORE_RQ:
                    cond = storeSCP(assoc, &msg.msg.CStoreRQ, presID, *dbHandle, correctUIDPadding);
                    break;
                case DIMSE_C_FIND_RQ:
                    cond = findSCP(assoc, &msg.msg.CFindRQ, presID, *dbHandle);
                    break;
                case DIMSE_C_MOVE_RQ:
                    cond = moveSCP(assoc, &msg.msg.CMoveRQ, presID, *dbHandle);
                    break;
                case DIMSE_C_GET_RQ:
                    cond = getSCP(assoc, &msg.msg.CGetRQ, presID, *dbHandle);
                    break;
                case DIMSE_C_CANCEL_RQ:
                    /* This is a late cancel request, just ignore it */
                    DCMQRDB_INFO("dispatch: late C-CANCEL-RQ, ignoring");
                    break;
                default:
                    /* we cannot handle this kind of message */
                    cond = DIMSE_BADCOMMANDTYPE;
                    DCMQRDB_ERROR("Cannot handle command: 0x" << STD_NAMESPACE hex <<
                            (unsigned)msg.CommandField);
                    /* the condition will be returned, the caller will abort the association. */
                }
            }
            else if ((cond == DUL_PEERREQUESTEDRELEASE)||(cond == DUL_PEERABORTEDASSOCIATION))
            {
                // association gone
            }
            else
            {
                // the condition will be returned, the caller will abort the assosiation.
            }
        }

        // release DB handle
        delete dbHandle;
    }

    // Association done
    return cond;
}


OFCondition DcmQueryRetrieveSCP::handleAssociation(T_ASC_Association * assoc, OFBool correctUIDPadding)
{
    OFCondition         cond = EC_Normal;
    DIC_NODENAME        peerHostName;
    DIC_AE              peerAETitle;
    DIC_AE              myAETitle;
    OFString            temp_str;

    ASC_getPresentationAddresses(assoc->params, peerHostName, NULL);
    ASC_getAPTitles(assoc->params, peerAETitle, myAETitle, NULL);

    /* now do the real work */
    cond = dispatch(assoc, correctUIDPadding);

    /* clean up on association termination */
    if (cond == DUL_PEERREQUESTEDRELEASE) {
        DCMQRDB_INFO("Association Release");
        cond = ASC_acknowledgeRelease(assoc);
        ASC_dropSCPAssociation(assoc);
    } else if (cond == DUL_PEERABORTEDASSOCIATION) {
        DCMQRDB_INFO("Association Aborted");
    } else {
        DCMQRDB_ERROR("DIMSE Failure (aborting association): " << DimseCondition::dump(temp_str, cond));
        /* some kind of error so abort the association */
        cond = ASC_abortAssociation(assoc);
    }

    cond = ASC_dropAssociation(assoc);
    if (cond.bad()) {
        DCMQRDB_ERROR("Cannot Drop Association: " << DimseCondition::dump(temp_str, cond));
    }
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad()) {
        DCMQRDB_ERROR("Cannot Destroy Association: " << DimseCondition::dump(temp_str, cond));
    }

    return cond;
}


OFCondition DcmQueryRetrieveSCP::echoSCP(T_ASC_Association * assoc, T_DIMSE_C_EchoRQ * req,
        T_ASC_PresentationContextID presId)
{
    OFCondition cond = EC_Normal;

    DCMQRDB_INFO("Received Echo SCP RQ: MsgID " << req->MessageID);
    /* we send an echo response back */
    cond = DIMSE_sendEchoResponse(assoc, presId,
        req, STATUS_Success, NULL);

    if (cond.bad()) {
        OFString temp_str;
        DCMQRDB_ERROR("echoSCP: Echo Response Failed: " << DimseCondition::dump(temp_str, cond));
    }
    return cond;
}


OFCondition DcmQueryRetrieveSCP::findSCP(T_ASC_Association * assoc, T_DIMSE_C_FindRQ * request,
        T_ASC_PresentationContextID presID,
        DcmQueryRetrieveDatabaseHandle& dbHandle)

{
    OFCondition cond = EC_Normal;
    DcmQueryRetrieveFindContext context(dbHandle, options_, STATUS_Pending);

    DIC_AE aeTitle;
    aeTitle[0] = '\0';
    ASC_getAPTitles(assoc->params, NULL, aeTitle, NULL);
    context.setOurAETitle(aeTitle);

    OFString temp_str;
    DCMQRDB_INFO("Received Find SCP:" << OFendl << DIMSE_dumpMessage(temp_str, *request, DIMSE_INCOMING));

    cond = DIMSE_findProvider(assoc, presID, request,
        findCallback, &context, options_.blockMode_, options_.dimse_timeout_);
    if (cond.bad()) {
        DCMQRDB_ERROR("Find SCP Failed: " << DimseCondition::dump(temp_str, cond));
    }
    return cond;
}


OFCondition DcmQueryRetrieveSCP::getSCP(T_ASC_Association * assoc, T_DIMSE_C_GetRQ * request,
        T_ASC_PresentationContextID presID, DcmQueryRetrieveDatabaseHandle& dbHandle)
{
    OFCondition cond = EC_Normal;
    DcmQueryRetrieveGetContext context(dbHandle, options_, STATUS_Pending, assoc, request->MessageID, request->Priority, presID);

    DIC_AE aeTitle;
    aeTitle[0] = '\0';
    ASC_getAPTitles(assoc->params, NULL, aeTitle, NULL);
    context.setOurAETitle(aeTitle);

    OFString temp_str;
    DCMQRDB_INFO("Received Get SCP:" << OFendl << DIMSE_dumpMessage(temp_str, *request, DIMSE_INCOMING));

    cond = DIMSE_getProvider(assoc, presID, request,
        getCallback, &context, options_.blockMode_, options_.dimse_timeout_);
    if (cond.bad()) {
        DCMQRDB_ERROR("Get SCP Failed: " << DimseCondition::dump(temp_str, cond));
    }
    return cond;
}


OFCondition DcmQueryRetrieveSCP::moveSCP(T_ASC_Association * assoc, T_DIMSE_C_MoveRQ * request,
        T_ASC_PresentationContextID presID, DcmQueryRetrieveDatabaseHandle& dbHandle)
{
    OFCondition cond = EC_Normal;
    DcmQueryRetrieveMoveContext context(dbHandle, options_, config_, STATUS_Pending, assoc, request->MessageID, request->Priority);

    DIC_AE aeTitle;
    aeTitle[0] = '\0';
    ASC_getAPTitles(assoc->params, NULL, aeTitle, NULL);
    context.setOurAETitle(aeTitle);

    OFString temp_str;
    DCMQRDB_INFO("Received Move SCP:" << OFendl << DIMSE_dumpMessage(temp_str, *request, DIMSE_INCOMING));

    cond = DIMSE_moveProvider(assoc, presID, request,
        moveCallback, &context, options_.blockMode_, options_.dimse_timeout_);
    if (cond.bad()) {
        DCMQRDB_ERROR("Move SCP Failed: " << DimseCondition::dump(temp_str, cond));
    }
    return cond;
}


OFCondition DcmQueryRetrieveSCP::storeSCP(T_ASC_Association * assoc, T_DIMSE_C_StoreRQ * request,
             T_ASC_PresentationContextID presId,
             DcmQueryRetrieveDatabaseHandle& dbHandle,
             OFBool correctUIDPadding)
{
    OFCondition cond = EC_Normal;
    OFCondition dbcond = EC_Normal;
    char imageFileName[MAXPATHLEN+1];
    DcmFileFormat dcmff;

    DcmQueryRetrieveStoreContext context(dbHandle, options_, STATUS_Success, &dcmff, correctUIDPadding);

    OFString temp_str;
    DCMQRDB_INFO("Received Store SCP:" << OFendl << DIMSE_dumpMessage(temp_str, *request, DIMSE_INCOMING));

    if (!dcmIsaStorageSOPClassUID(request->AffectedSOPClassUID)) {
        /* callback will send back sop class not supported status */
        context.setStatus(STATUS_STORE_Refused_SOPClassNotSupported);
        /* must still receive data */
        strcpy(imageFileName, NULL_DEVICE_NAME);
    } else if (options_.ignoreStoreData_) {
        strcpy(imageFileName, NULL_DEVICE_NAME);
    } else {
        dbcond = dbHandle.makeNewStoreFileName(
            request->AffectedSOPClassUID,
            request->AffectedSOPInstanceUID,
            imageFileName);
        if (dbcond.bad()) {
            DCMQRDB_ERROR("storeSCP: Database: makeNewStoreFileName Failed");
            /* must still receive data */
            strcpy(imageFileName, NULL_DEVICE_NAME);
            /* callback will send back out of resources status */
            context.setStatus(STATUS_STORE_Refused_OutOfResources);
        }
    }

#ifdef LOCK_IMAGE_FILES
    /* exclusively lock image file */
#ifdef O_BINARY
    int lockfd = open(imageFileName, (O_WRONLY | O_CREAT | O_TRUNC | O_BINARY), 0666);
#else
    int lockfd = open(imageFileName, (O_WRONLY | O_CREAT | O_TRUNC), 0666);
#endif
    if (lockfd < 0)
    {
        DCMQRDB_ERROR("storeSCP: file locking failed, cannot create file");

        /* must still receive data */
        strcpy(imageFileName, NULL_DEVICE_NAME);

        /* callback will send back out of resources status */
        context.setStatus(STATUS_STORE_Refused_OutOfResources);
    }
    else
      dcmtk_flock(lockfd, LOCK_EX);
#endif

    context.setFileName(imageFileName);

    // store SourceApplicationEntityTitle in metaheader
    if (assoc && assoc->params)
    {
      const char *aet = assoc->params->DULparams.callingAPTitle;
      if (aet) dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
    }

    DcmDataset *dset = dcmff.getDataset();

    /* we must still retrieve the data set even if some error has occured */

    if (options_.bitPreserving_) { /* the bypass option can be set on the command line */
        cond = DIMSE_storeProvider(assoc, presId, request, imageFileName, (int)options_.useMetaheader_,
                                   NULL, storeCallback,
                                   (void*)&context, options_.blockMode_, options_.dimse_timeout_);
    } else {
        cond = DIMSE_storeProvider(assoc, presId, request, (char *)NULL, (int)options_.useMetaheader_,
                                   &dset, storeCallback,
                                   (void*)&context, options_.blockMode_, options_.dimse_timeout_);
    }

    if (cond.bad()) {
        DCMQRDB_ERROR("Store SCP Failed: " << DimseCondition::dump(temp_str, cond));
    }
    if (!options_.ignoreStoreData_ && (cond.bad() || (context.getStatus() != STATUS_Success)))
    {
      /* remove file */
      if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) // don't try to delete /dev/null
      {
        DCMQRDB_INFO("Store SCP: Deleting Image File: %s" << imageFileName);
        unlink(imageFileName);
      }
      dbHandle.pruneInvalidRecords();
    }

#ifdef LOCK_IMAGE_FILES
    /* unlock image file */
    if (lockfd >= 0)
    {
      dcmtk_flock(lockfd, LOCK_UN);
      close(lockfd);
    }
#endif

    return cond;
}


/* Association negotiation */

void DcmQueryRetrieveSCP::refuseAnyStorageContexts(T_ASC_Association * assoc)
{
    int i;
    T_ASC_PresentationContextID pid;

    for (i = 0; i < numberOfAllDcmStorageSOPClassUIDs; i++) {
        do {
          pid = ASC_findAcceptedPresentationContextID(assoc, dcmAllStorageSOPClassUIDs[i]);
          if (pid != 0) ASC_refusePresentationContext(assoc->params, pid, ASC_P_USERREJECTION);
        } while (pid != 0); // repeat as long as we find presentation contexts for this SOP class - there might be multiple ones.
    }
}


OFCondition DcmQueryRetrieveSCP::refuseAssociation(T_ASC_Association ** assoc, CTN_RefuseReason reason)
{
    OFCondition cond = EC_Normal;
    T_ASC_RejectParameters rej;
    OFString temp_str;

    const char *reason_string;
    switch (reason)
    {
      case CTN_TooManyAssociations:
          reason_string = "TooManyAssociations";
          break;
      case CTN_CannotFork:
          reason_string = "CannotFork";
          break;
      case CTN_BadAppContext:
          reason_string = "BadAppContext";
          break;
      case CTN_BadAEPeer:
          reason_string = "BadAEPeer";
          break;
      case CTN_BadAEService:
          reason_string = "BadAEService";
          break;
      case CTN_NoReason:
          reason_string = "NoReason";
        break;
      default:
          reason_string = "???";
          break;
    }
    DCMQRDB_INFO("Refusing Association (" << reason_string << ")");

    switch (reason)
    {
      case CTN_TooManyAssociations:
        rej.result = ASC_RESULT_REJECTEDTRANSIENT;
        rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
        rej.reason = ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED;
        break;
      case CTN_CannotFork:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
        rej.reason = ASC_REASON_SP_PRES_TEMPORARYCONGESTION;
        break;
      case CTN_BadAppContext:
        rej.result = ASC_RESULT_REJECTEDTRANSIENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
        break;
      case CTN_BadAEPeer:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED;
        break;
      case CTN_BadAEService:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
        break;
      case CTN_NoReason:
      default:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_NOREASON;
        break;
    }

    cond = ASC_rejectAssociation(*assoc, &rej);

    if (cond.bad())
    {
      DCMQRDB_ERROR("Association Reject Failed: " << DimseCondition::dump(temp_str, cond));
    }

    cond = ASC_dropAssociation(*assoc);
    if (cond.bad())
    {
      DCMQRDB_ERROR("Cannot Drop Association: " << DimseCondition::dump(temp_str, cond));
    }
    cond = ASC_destroyAssociation(assoc);
    if (cond.bad())
    {
      DCMQRDB_ERROR("Cannot Destroy Association: " << DimseCondition::dump(temp_str, cond));
    }

    return cond;
}


OFCondition DcmQueryRetrieveSCP::negotiateAssociation(T_ASC_Association * assoc)
{
    OFCondition cond = EC_Normal;
    int i;
    T_ASC_PresentationContextID movepid, findpid;
    OFString temp_str;
    struct { const char *moveSyntax, *findSyntax; } queryRetrievePairs[] =
    {
      { UID_MOVEPatientRootQueryRetrieveInformationModel,
        UID_FINDPatientRootQueryRetrieveInformationModel },
      { UID_MOVEStudyRootQueryRetrieveInformationModel,
        UID_FINDStudyRootQueryRetrieveInformationModel },
      { UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel,
        UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel }
    };

    DIC_AE calledAETitle;
    ASC_getAPTitles(assoc->params, NULL, calledAETitle, NULL);

    const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL };
    int numTransferSyntaxes = 0;

    switch (options_.networkTransferSyntax_)
    {
      case EXS_LittleEndianImplicit:
        /* we only support Little Endian Implicit */
        transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 1;
        break;
      case EXS_LittleEndianExplicit:
        /* we prefer Little Endian Explicit */
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[2]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
      case EXS_BigEndianExplicit:
        /* we prefer Big Endian Explicit */
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
#ifndef DISABLE_COMPRESSION_EXTENSION
      case EXS_JPEGProcess14SV1TransferSyntax:
        /* we prefer JPEGLossless:Hierarchical-1stOrderPrediction (default lossless) */
        transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_JPEGProcess1TransferSyntax:
        /* we prefer JPEGBaseline (default lossy for 8 bit images) */
        transferSyntaxes[0] = UID_JPEGProcess1TransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_JPEGProcess2_4TransferSyntax:
        /* we prefer JPEGExtended (default lossy for 12 bit images) */
        transferSyntaxes[0] = UID_JPEGProcess2_4TransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_JPEG2000LosslessOnly:
        /* we prefer JPEG 2000 lossless */
        transferSyntaxes[0] = UID_JPEG2000LosslessOnlyTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_JPEG2000:
        /* we prefer JPEG 2000 lossy or lossless */
        transferSyntaxes[0] = UID_JPEG2000TransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_JPEGLSLossless:
        /* we prefer JPEG-LS Lossless */
        transferSyntaxes[0] = UID_JPEGLSLosslessTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_JPEGLSLossy:
        /* we prefer JPEG-LS Lossy */
        transferSyntaxes[0] = UID_JPEGLSLossyTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_MPEG2MainProfileAtMainLevel:
        /* we prefer MPEG2 MP@ML */
        transferSyntaxes[0] = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_MPEG2MainProfileAtHighLevel:
        /* we prefer MPEG2 MP@HL */
        transferSyntaxes[0] = UID_MPEG2MainProfileAtHighLevelTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
      case EXS_RLELossless:
        /* we prefer RLE Lossless */
        transferSyntaxes[0] = UID_RLELosslessTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
#ifdef WITH_ZLIB
      case EXS_DeflatedLittleEndianExplicit:
        /* we prefer deflated transmission */
        transferSyntaxes[0] = UID_DeflatedExplicitVRLittleEndianTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 4;
        break;
#endif
#endif
      default:
        /* We prefer explicit transfer syntaxes.
         * If we are running on a Little Endian machine we prefer
         * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
         */
        if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
        {
          transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        } else {
          transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    }

    const char * const nonStorageSyntaxes[] =
    {
        UID_VerificationSOPClass,
        UID_FINDPatientRootQueryRetrieveInformationModel,
        UID_MOVEPatientRootQueryRetrieveInformationModel,
        UID_GETPatientRootQueryRetrieveInformationModel,
#ifndef NO_PATIENTSTUDYONLY_SUPPORT
        UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel,
        UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel,
        UID_RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel,
#endif
        UID_FINDStudyRootQueryRetrieveInformationModel,
        UID_MOVEStudyRootQueryRetrieveInformationModel,
        UID_GETStudyRootQueryRetrieveInformationModel,
        UID_PrivateShutdownSOPClass
    };

    const int numberOfNonStorageSyntaxes = DIM_OF(nonStorageSyntaxes);
    const char *selectedNonStorageSyntaxes[DIM_OF(nonStorageSyntaxes)];
    int numberOfSelectedNonStorageSyntaxes = 0;
    for (i = 0; i < numberOfNonStorageSyntaxes; i++)
    {
        if (0 == strcmp(nonStorageSyntaxes[i], UID_FINDPatientRootQueryRetrieveInformationModel))
        {
          if (options_.supportPatientRoot_) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
        }
        else if (0 == strcmp(nonStorageSyntaxes[i], UID_MOVEPatientRootQueryRetrieveInformationModel))
        {
          if (options_.supportPatientRoot_) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
        }
        else if (0 == strcmp(nonStorageSyntaxes[i], UID_GETPatientRootQueryRetrieveInformationModel))
        {
          if (options_.supportPatientRoot_ && (! options_.disableGetSupport_)) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
        }
        else if (0 == strcmp(nonStorageSyntaxes[i], UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel))
        {
          if (options_.supportPatientStudyOnly_) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
        }
        else if (0 == strcmp(nonStorageSyntaxes[i], UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel))
        {
          if (options_.supportPatientStudyOnly_) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
        }
        else if (0 == strcmp(nonStorageSyntaxes[i], UID_RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel))
        {
          if (options_.supportPatientStudyOnly_ && (! options_.disableGetSupport_)) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
        }
        else if (0 == strcmp(nonStorageSyntaxes[i], UID_FINDStudyRootQueryRetrieveInformationModel))
        {
          if (options_.supportStudyRoot_) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
        }
        else if (0 == strcmp(nonStorageSyntaxes[i], UID_MOVEStudyRootQueryRetrieveInformationModel))
        {
          if (options_.supportStudyRoot_) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
        }
        else if (0 == strcmp(nonStorageSyntaxes[i], UID_GETStudyRootQueryRetrieveInformationModel))
        {
          if (options_.supportStudyRoot_ && (! options_.disableGetSupport_)) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
        }
        else if (0 == strcmp(nonStorageSyntaxes[i], UID_PrivateShutdownSOPClass))
        {
          if (options_.allowShutdown_) selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
        } else {
            selectedNonStorageSyntaxes[numberOfSelectedNonStorageSyntaxes++] = nonStorageSyntaxes[i];
        }
    }

    /*  accept any of the non-storage syntaxes */
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
    assoc->params,
    (const char**)selectedNonStorageSyntaxes, numberOfSelectedNonStorageSyntaxes,
    (const char**)transferSyntaxes, numTransferSyntaxes);
    if (cond.bad()) {
        DCMQRDB_ERROR("Cannot accept presentation contexts: " << DimseCondition::dump(temp_str, cond));
    }

    /*  accept any of the storage syntaxes */
    if (options_.disableGetSupport_)
    {
      /* accept storage syntaxes with default role only */
      cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
        assoc->params,
        dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs,
        (const char**)transferSyntaxes, DIM_OF(transferSyntaxes));
      if (cond.bad()) {
        DCMQRDB_ERROR("Cannot accept presentation contexts: " << DimseCondition::dump(temp_str, cond));
      }
    } else {
      /* accept storage syntaxes with proposed role */
      T_ASC_PresentationContext pc;
      T_ASC_SC_ROLE role;
      int npc = ASC_countPresentationContexts(assoc->params);
      for (i = 0; i < npc; i++)
      {
        ASC_getPresentationContext(assoc->params, i, &pc);
        if (dcmIsaStorageSOPClassUID(pc.abstractSyntax))
        {
          /*
          ** We are prepared to accept whatever role he proposes.
          ** Normally we can be the SCP of the Storage Service Class.
          ** When processing the C-GET operation we can be the SCU of the Storage Service Class.
          */
          role = pc.proposedRole;

          /*
          ** Accept in the order "least wanted" to "most wanted" transfer
          ** syntax.  Accepting a transfer syntax will override previously
          ** accepted transfer syntaxes.
          */
          for (int k = numTransferSyntaxes - 1; k >= 0; k--)
          {
            for (int j = 0; j < (int)pc.transferSyntaxCount; j++)
            {
              /* if the transfer syntax was proposed then we can accept it
               * appears in our supported list of transfer syntaxes
               */
              if (strcmp(pc.proposedTransferSyntaxes[j], transferSyntaxes[k]) == 0)
              {
                cond = ASC_acceptPresentationContext(
                    assoc->params, pc.presentationContextID, transferSyntaxes[k], role);
                if (cond.bad()) return cond;
              }
            }
          }
        }
      } /* for */
    } /* else */

    /*
     * check if we have negotiated the private "shutdown" SOP Class
     */
    if (0 != ASC_findAcceptedPresentationContextID(assoc, UID_PrivateShutdownSOPClass))
    {
      DCMQRDB_INFO("Shutting down server ... (negotiated private \"shut down\" SOP class)");
      refuseAssociation(&assoc, CTN_NoReason);
      return ASC_SHUTDOWNAPPLICATION;
    }

    /*
     * Refuse any "Storage" presentation contexts to non-writable
     * storage areas.
     */
    if (!config_->writableStorageArea(calledAETitle))
    {
      refuseAnyStorageContexts(assoc);
    }

    /*
     * Enforce RSNA'93 Demonstration Requirements about only
     * accepting a context for MOVE if a context for FIND is also present.
     */

    for (i = 0; i < (int)DIM_OF(queryRetrievePairs); i++) {
        movepid = ASC_findAcceptedPresentationContextID(assoc,
        queryRetrievePairs[i].moveSyntax);
        if (movepid != 0) {
          findpid = ASC_findAcceptedPresentationContextID(assoc,
              queryRetrievePairs[i].findSyntax);
          if (findpid == 0) {
            if (options_.requireFindForMove_) {
              /* refuse the move */
              ASC_refusePresentationContext(assoc->params,
                  movepid, ASC_P_USERREJECTION);
            } else {
              DCMQRDB_ERROR("Move Presentation Context but no Find (accepting for now)");
            }
          }
        }
    }

    /*
     * Enforce an Ad-Hoc rule to limit storage access.
     * If the storage area is "writable" and some other association has
     * already negotiated a "Storage" class presentation context,
     * then refuse any "storage" presentation contexts.
     */

    if (options_.refuseMultipleStorageAssociations_)
    {
        if (config_->writableStorageArea(calledAETitle))
        {
          if (processtable_.haveProcessWithWriteAccess(calledAETitle))
          {
            refuseAnyStorageContexts(assoc);
          }
        }
    }

    return cond;
}


OFCondition DcmQueryRetrieveSCP::waitForAssociation(T_ASC_Network * theNet)
{
    OFCondition cond = EC_Normal;
    OFString temp_str;
#ifdef HAVE_FORK
    int                 pid;
#endif
    T_ASC_Association  *assoc;
    char                buf[BUFSIZ];
    int timeout;
    OFBool go_cleanup = OFFalse;

    if (options_.singleProcess_) timeout = 1000;
    else
    {
      if (processtable_.countChildProcesses() > 0)
      {
        timeout = 1;
      } else {
        timeout = 1000;
      }
    }

    if (ASC_associationWaiting(theNet, timeout))
    {
        cond = ASC_receiveAssociation(theNet, &assoc, (int)options_.maxPDU_);
        if (cond.bad())
        {
          DCMQRDB_INFO("Failed to receive association: " << DimseCondition::dump(temp_str, cond));
          go_cleanup = OFTrue;
        }
    } else return EC_Normal;

    if (! go_cleanup)
    {
        time_t t = time(NULL);
        DCMQRDB_INFO("Association Received (" << assoc->params->DULparams.callingPresentationAddress
                << ":" << assoc->params->DULparams.callingAPTitle << " -> "
                << assoc->params->DULparams.calledAPTitle << ") " << ctime(&t));

        DCMQRDB_DEBUG("Parameters:" << OFendl << ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_RQ));

        if (options_.refuse_)
        {
            DCMQRDB_INFO("Refusing Association (forced via command line)");
            cond = refuseAssociation(&assoc, CTN_NoReason);
            go_cleanup = OFTrue;
        }
    }

    if (! go_cleanup)
    {
        /* Application Context Name */
        cond = ASC_getApplicationContextName(assoc->params, buf);
        if (cond.bad() || strcmp(buf, DICOM_STDAPPLICATIONCONTEXT) != 0)
        {
            /* reject: the application context name is not supported */
            DCMQRDB_INFO("Bad AppContextName: " << buf);
            cond = refuseAssociation(&assoc, CTN_BadAppContext);
            go_cleanup = OFTrue;
        }
    }

    if (! go_cleanup)
    {
        /* Implementation Class UID */
        if (options_.rejectWhenNoImplementationClassUID_ &&
        strlen(assoc->params->theirImplementationClassUID) == 0)
        {
            /* reject: no implementation Class UID provided */
            DCMQRDB_INFO("No implementation Class UID provided");
            cond = refuseAssociation(&assoc, CTN_NoReason);
            go_cleanup = OFTrue;
        }
    }

    if (! go_cleanup)
    {
        /* Does peer AE have access to required service ?? */
        if (! config_->peerInAETitle(assoc->params->DULparams.calledAPTitle,
        assoc->params->DULparams.callingAPTitle,
        assoc->params->DULparams.callingPresentationAddress))
        {
            cond = refuseAssociation(&assoc, CTN_BadAEService);
            go_cleanup = OFTrue;
        }
    }

    if (! go_cleanup)
    {
        // too many concurrent associations ??
        if (processtable_.countChildProcesses() >= OFstatic_cast(size_t, options_.maxAssociations_))
        {
            cond = refuseAssociation(&assoc, CTN_TooManyAssociations);
            go_cleanup = OFTrue;
        }
    }

    if (! go_cleanup)
    {
        cond = negotiateAssociation(assoc);
        if (cond.bad()) go_cleanup = OFTrue;
    }

    if (! go_cleanup)
    {
        cond = ASC_acknowledgeAssociation(assoc);
        if (cond.bad())
        {
            DCMQRDB_ERROR(DimseCondition::dump(temp_str, cond));
            go_cleanup = OFTrue;
        }
    }

    if (! go_cleanup)
    {
        DCMQRDB_INFO("Association Acknowledged (Max Send PDV: " << assoc->sendPDVLength << ")");
        if (ASC_countAcceptedPresentationContexts(assoc->params) == 0)
            DCMQRDB_INFO("    (but no valid presentation contexts)");
        DCMQRDB_DEBUG(ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_AC));

        if (options_.singleProcess_)
        {
            /* don't spawn a sub-process to handle the association */
            cond = handleAssociation(assoc, options_.correctUIDPadding_);
        }
#ifdef HAVE_FORK
        else
        {
            /* spawn a sub-process to handle the association */
            pid = (int)(fork());
            if (pid < 0)
            {
                char errBuf[256];
                DCMQRDB_ERROR("Cannot create association sub-process: "
                   << OFStandard::strerror(errno, errBuf, sizeof(errBuf)));
                cond = refuseAssociation(&assoc, CTN_CannotFork);
                go_cleanup = OFTrue;
            }
            else if (pid > 0)
            {
                /* parent process, note process in table */
                processtable_.addProcessToTable(pid, assoc);
            }
            else
            {
                /* child process, handle the association */
                cond = handleAssociation(assoc, options_.correctUIDPadding_);
                /* the child process is done so exit */
                exit(0);
            }
        }
#endif
    }

    // cleanup code
    OFCondition oldcond = cond;    /* store condition flag for later use */
    if (!options_.singleProcess_ && (cond != ASC_SHUTDOWNAPPLICATION))
    {
        /* the child will handle the association, we can drop it */
        cond = ASC_dropAssociation(assoc);
        if (cond.bad())
        {
            DCMQRDB_ERROR("Cannot Drop Association: " << DimseCondition::dump(temp_str, cond));
        }
        cond = ASC_destroyAssociation(&assoc);
        if (cond.bad())
        {
            DCMQRDB_ERROR("Cannot Destroy Association: " << DimseCondition::dump(temp_str, cond));
        }
    }

    if (oldcond == ASC_SHUTDOWNAPPLICATION) cond = oldcond; /* abort flag is reported to top-level wait loop */
    return cond;
}


void DcmQueryRetrieveSCP::cleanChildren()
{
  processtable_.cleanChildren();
}


void DcmQueryRetrieveSCP::setDatabaseFlags(
  OFBool dbCheckFindIdentifier,
  OFBool dbCheckMoveIdentifier)
{
  dbCheckFindIdentifier_ = dbCheckFindIdentifier;
  dbCheckMoveIdentifier_ = dbCheckMoveIdentifier;
}


/*
 * CVS Log
 * $Log: dcmqrsrv.cc,v $
 * Revision 1.12  2010-10-14 13:14:36  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.11  2010-09-24 13:34:17  joergr
 * Compared names of SOP Class UIDs with 2009 edition of the DICOM standard. The
 * resulting name changes are mainly caused by the fact that the corresponding
 * SOP Class is now retired.
 *
 * Revision 1.10  2010-09-09 16:54:32  joergr
 * Further code clean-up and minor changes to log messages.
 *
 * Revision 1.9  2010-09-09 15:00:03  joergr
 * Made log messages more consistent. Replaced '\n' by OFendl where appropriate.
 *
 * Revision 1.8  2010-09-02 12:13:00  joergr
 * Added support for "MPEG2 Main Profile @ High Level" transfer syntax.
 *
 * Revision 1.7  2010-06-03 10:34:57  joergr
 * Replaced calls to strerror() by new helper function OFStandard::strerror()
 * which results in using the thread safe version of strerror() if available.
 *
 * Revision 1.6  2009-11-24 10:10:42  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.5  2009-08-21 09:54:11  joergr
 * Replaced tabs by spaces and updated copyright date.
 *
 * Revision 1.4  2009-08-05 14:54:54  meichel
 * Fixed bug that caused dcmqrscp to accept image transmissions to read-only
 *   storage areas when multiple presentation contexts were proposed for a
 *   single SOP class.
 *
 * Revision 1.3  2009-02-06 15:25:43  joergr
 * Added support for JPEG-LS and MPEG2 transfer syntaxes.
 *
 * Revision 1.2  2006/06/23 10:24:43  meichel
 * All Store SCPs in DCMTK now store the source application entity title in the
 *   metaheader, both in normal and in bit-preserving mode.
 *
 * Revision 1.1  2005/12/16 12:41:35  joergr
 * Renamed file to avoid naming conflicts when linking on SunOS 5.5.1 with
 * Sun CC 2.0.1.
 *
 * Revision 1.7  2005/12/08 15:47:13  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.6  2005/11/29 11:27:20  meichel
 * Added new flag keepDBHandleDuringAssociation_ which allows to determine
 *   whether a DB handle is kept open for a complete association or a single
 *   DIMSE message only. Also improved error handling of file locking.
 *
 * Revision 1.5  2005/11/29 10:54:52  meichel
 * Added minimal support for compressed transfer syntaxes to dcmqrscp.
 *   No on-the-fly decompression is performed, but compressed images can
 *   be stored and retrieved.
 *
 * Revision 1.4  2005/11/17 13:44:40  meichel
 * Added command line options for DIMSE and ACSE timeouts
 *
 * Revision 1.3  2005/10/25 08:56:18  meichel
 * Updated list of UIDs and added support for new transfer syntaxes
 *   and storage SOP classes.
 *
 * Revision 1.2  2005/04/22 15:36:32  meichel
 * Passing calling aetitle to DcmQueryRetrieveDatabaseHandleFactory::createDBHandle
 *   to allow configuration retrieval based on calling aetitle.
 *
 * Revision 1.1  2005/03/30 13:34:53  meichel
 * Initial release of module dcmqrdb that will replace module imagectn.
 *   It provides a clear interface between the Q/R DICOM front-end and the
 *   database back-end. The imagectn code has been re-factored into a minimal
 *   class structure.
 *
 *
 */
