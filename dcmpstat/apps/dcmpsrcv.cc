/*
 *
 *  Copyright (C) 1999-2019, OFFIS e.V.
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
 *  Module:  dcmpstat
 *
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Presentation State Viewer - Network Receive Component (Store SCP)
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>       /* for O_RDONLY */
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>   /* required for sys/stat.h */
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>    /* for stat, fstat */
#endif
END_EXTERN_C

#include "dcmtk/dcmpstat/dvpsdef.h"     /* for constants */
#include "dcmtk/dcmpstat/dvpscf.h"      /* for class DVConfiguration */
#include "dcmtk/ofstd/ofbmanip.h"       /* for OFBitmanipTemplate */
#include "dcmtk/dcmdata/dcuid.h"        /* for dcmtk version name */
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmqrdb/dcmqrdbi.h"     /* for LOCK_IMAGE_FILES */
#include "dcmtk/dcmqrdb/dcmqrdbs.h"     /* for DcmQueryRetrieveDatabaseStatus */
#include "dcmtk/dcmpstat/dvpsmsg.h"     /* for class DVPSIPCClient */
#include "dcmtk/dcmnet/dcmlayer.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmpstat/dcmpstat.h"

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#include "dcmtk/ofstd/ofstream.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmpsrcv"

static OFLogger dcmpsrcvLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";


DVPSIPCClient *messageClient  = NULL; // global pointer to IPC message client, if present


enum associationType
{
  assoc_success,
  assoc_error,
  assoc_terminate
};

enum refuseReason
{
  ref_TooManyAssociations,
  ref_CannotFork,
  ref_BadAppContext,
  ref_BadAEPeer,
  ref_BadAEService,
  ref_NoReason
};

static int errorCond(OFCondition cond, const char *message)
{
  int result = (cond.bad());
  if (result)
  {
    OFString temp_str;
    OFLOG_ERROR(dcmpsrcvLogger, message << OFendl << DimseCondition::dump(temp_str, cond));
  }
  return result;
}

static void cleanChildren()
{
#ifdef HAVE_WAITPID
    int stat_loc;
#elif defined(HAVE_WAIT3)
    struct rusage rusage;
#if defined(__NeXT__)
    /* some systems need a union wait as argument to wait3 */
    union wait status;
#else
    int        status;
#endif
#endif

#if defined(HAVE_WAITPID) || defined(HAVE_WAIT3)
    int child = 1;
    int options = WNOHANG;
    while (child > 0)
    {
#ifdef HAVE_WAITPID
        child = (int)(waitpid(-1, &stat_loc, options));
#elif defined(HAVE_WAIT3)
        child = wait3(&status, options, &rusage);
#endif
        if (child < 0)
        {
            if (errno != ECHILD)
            {
                char buf[256];
                OFLOG_ERROR(dcmpsrcvLogger, "wait for child failed: " << OFStandard::strerror(errno, buf, sizeof(buf)));
            }
        }
    }
#endif
}

static void dropAssociation(T_ASC_Association **assoc)
{
  if ((assoc == NULL)||(*assoc == NULL)) return;
  OFCondition cond = ASC_dropSCPAssociation(*assoc);
  errorCond(cond, "Cannot Drop Association:");
  cond = ASC_destroyAssociation(assoc);
  errorCond(cond, "Cannot Destroy Association:");
  *assoc = NULL;
  return;
}


static OFCondition
refuseAssociation(T_ASC_Association *assoc, refuseReason reason)
{
    T_ASC_RejectParameters rej;

    switch (reason)
    {
    case ref_TooManyAssociations:
        rej.result = ASC_RESULT_REJECTEDTRANSIENT;
        rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
        rej.reason = ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED;
        break;
    case ref_CannotFork:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED;
        rej.reason = ASC_REASON_SP_PRES_TEMPORARYCONGESTION;
        break;
    case ref_BadAppContext:
        rej.result = ASC_RESULT_REJECTEDTRANSIENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED;
        break;
    case ref_BadAEPeer:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED;
        break;
    case ref_BadAEService:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED;
        break;
    case ref_NoReason:
    default:
        rej.result = ASC_RESULT_REJECTEDPERMANENT;
        rej.source = ASC_SOURCE_SERVICEUSER;
        rej.reason = ASC_REASON_SU_NOREASON;
        break;
    }

    OFCondition cond = ASC_rejectAssociation(assoc, &rej);
    errorCond(cond, "Association Reject Failed:");
    return cond;
}

static associationType negotiateAssociation(
  T_ASC_Network *net,
  T_ASC_Association **assoc,
  const char *aetitle,
  unsigned long maxPDU,
  OFBool opt_networkImplicitVROnly,
  OFBool useTLS)
{
    associationType result = assoc_success;
    char buf[BUFSIZ];
    OFBool dropAssoc = OFFalse;

    OFCondition cond = ASC_receiveAssociation(net, assoc, maxPDU, NULL, NULL, useTLS);

    if (errorCond(cond, "Failed to receive association:"))
    {
      dropAssoc = OFTrue;
      result = assoc_error;

      if (messageClient)
      {
        // notify about failed association setup
        OFOStringStream out;
        out << "Unable to Receive DIMSE Association Request:" << OFendl << cond.text() << OFendl << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(out, theString)
        if (useTLS)
          messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
          else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
        OFSTRINGSTREAM_FREESTR(theString)
      }
    }
    else
    {
      OFLOG_INFO(dcmpsrcvLogger, "Association Received ("
          << (*assoc)->params->DULparams.callingPresentationAddress << ":"
          << (*assoc)->params->DULparams.callingAPTitle << " -> "
          << (*assoc)->params->DULparams.calledAPTitle << ")");

      ASC_setAPTitles((*assoc)->params, NULL, NULL, aetitle);
      /* Application Context Name */
      cond = ASC_getApplicationContextName((*assoc)->params, buf, sizeof(buf));
      if (cond.bad() || strcmp(buf, DICOM_STDAPPLICATIONCONTEXT) != 0)
      {
          /* reject: the application context name is not supported */
          OFLOG_INFO(dcmpsrcvLogger, "Bad AppContextName: " << buf);
          cond = refuseAssociation(*assoc, ref_BadAppContext);

          if (messageClient)
          {
            // notify about rejected association
            OFOStringStream out;
            OFString temp_str;
            out << "DIMSE Association Rejected:" << OFendl
                << "  reason: bad application context name '" << buf << "'" << OFendl
                << "  calling presentation address: " << (*assoc)->params->DULparams.callingPresentationAddress << OFendl
                << "  calling AE title: " << (*assoc)->params->DULparams.callingAPTitle << OFendl
                << "  called AE title: " << (*assoc)->params->DULparams.calledAPTitle << OFendl;
            out << ASC_dumpConnectionParameters(temp_str, *assoc) << OFendl;
            out << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(out, theString)
            if (useTLS)
              messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
              else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
            OFSTRINGSTREAM_FREESTR(theString)
          }
          dropAssoc = OFTrue;
          result = assoc_error;
      } else {

        const char *nonStorageSyntaxes[] =
        {
          UID_VerificationSOPClass,
          UID_PrivateShutdownSOPClass
        };

        const char* transferSyntaxes[] = { NULL, NULL, NULL };
        int numTransferSyntaxes = 0;
        if (opt_networkImplicitVROnly)
        {
          transferSyntaxes[0] = UID_LittleEndianImplicitTransferSyntax;
          numTransferSyntaxes = 1;
        } else {
          transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
          numTransferSyntaxes = 3;

          if (gLocalByteOrder == EBO_LittleEndian) {
            /* we are on a little endian machine */
            transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
          } else {
            /* we are on a big endian machine */
            transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
          }
        }

        /*  accept any of the non-storage syntaxes */
        cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
          (*assoc)->params,
          (const char**)nonStorageSyntaxes, DIM_OF(nonStorageSyntaxes),
          (const char**)transferSyntaxes, numTransferSyntaxes);
        errorCond(cond, "Cannot accept presentation contexts:");

        /*  accept any of the storage syntaxes */
        cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
          (*assoc)->params,
          dcmAllStorageSOPClassUIDs, numberOfDcmAllStorageSOPClassUIDs,
          (const char**)transferSyntaxes, numTransferSyntaxes);
        errorCond(cond, "Cannot accept presentation contexts:");
      }

      /* check if we have negotiated the private "shutdown" SOP Class */
      if (0 != ASC_findAcceptedPresentationContextID(*assoc, UID_PrivateShutdownSOPClass))
      {
        // we don't notify the IPC server about this incoming connection
        cond = refuseAssociation(*assoc, ref_NoReason);
        dropAssoc = OFTrue;
        result = assoc_terminate;
      }
    } /* receiveAssociation successful */

    if (dropAssoc) dropAssociation(assoc);
    return result;
}

class StoreContext
{
public:
  DcmQueryRetrieveIndexDatabaseHandle *dbHandle;
  DIC_US status;
  const char *fileName;
  DcmFileFormat *dcmff;
  OFBool opt_correctUIDPadding;

  StoreContext(DcmQueryRetrieveIndexDatabaseHandle *handle, DIC_US aStatus, const char *fname, DcmFileFormat *ff, OFBool correctUID)
  : dbHandle(handle)
  , status(aStatus)
  , fileName(fname)
  , dcmff(ff)
  , opt_correctUIDPadding(correctUID)
  {}

  ~StoreContext() {}

private:
  /* unimplemented */ StoreContext(const StoreContext&);
  /* unimplemented */ StoreContext& operator=(const StoreContext&);
};

static void
checkRequestAgainstDataset(
    T_DIMSE_C_StoreRQ *req,     /* original store request */
    const char* fname,          /* filename of dataset */
    DcmDataset *dataSet,        /* dataset to check */
    T_DIMSE_C_StoreRSP *rsp,    /* final store response */
    OFBool opt_correctUIDPadding)
{
    DcmFileFormat ff;
    if (dataSet == NULL)
    {
      /* load the data from file */
      if (ff.loadFile(fname).bad())
      {
        OFLOG_ERROR(dcmpsrcvLogger, "Cannot open file: " << fname);
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
        return;
      }
      dataSet = ff.getDataset();
    }

    /* which SOP class and SOP instance ? */
    DIC_UI sopClass;
    DIC_UI sopInstance;

    if (!DU_findSOPClassAndInstanceInDataSet(dataSet, sopClass, sizeof(sopClass), sopInstance, sizeof(sopInstance), opt_correctUIDPadding))
    {
      OFLOG_ERROR(dcmpsrcvLogger, "Bad image file: " << fname);
      rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
    }
    else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0)
    {
      rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
    }
    else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0)
    {
      rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
    }
    else if (strcmp(sopClass, UID_GrayscaleSoftcopyPresentationStateStorage) == 0)
    {
      /* we have received a presentation state. Check if we can parse it! */
      DcmPresentationState pstate;
      if (EC_Normal != pstate.read(*dataSet))
      {
        OFLOG_ERROR(dcmpsrcvLogger, "Grayscale softcopy presentation state object cannot be displayed - rejected");
        rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
      }
    }
    return;
}

static void
saveImageToDB(
    StoreContext *context,
    T_DIMSE_C_StoreRQ *req,             /* original store request */
    const char *imageFileName,
    /* out */
    T_DIMSE_C_StoreRSP *rsp,            /* final store response */
    DcmDataset **statusDetail)
{
    DcmQueryRetrieveDatabaseStatus dbStatus(STATUS_Success);

    /* Store image */
    if (context->status == STATUS_Success)
    {
      if (context->dbHandle->storeRequest(req->AffectedSOPClassUID, req->AffectedSOPInstanceUID,
          imageFileName, &dbStatus).bad())
      {
        OFLOG_ERROR(dcmpsrcvLogger, "storeSCP: Database: DB_storeRequest Failed ("
             << DU_cstoreStatusString(dbStatus.status()) << ")");
      }
      context->status = dbStatus.status();
    }

     rsp->DimseStatus = context->status;
    *statusDetail = dbStatus.extractStatusDetail();
    return;
}

static void
storeProgressCallback(
    /* in */
    void *callbackData,
    T_DIMSE_StoreProgress *progress,    /* progress state */
    T_DIMSE_C_StoreRQ *req,             /* original store request */
    char *imageFileName,                /* being received into */
    DcmDataset **imageDataSet,          /* being received into */
    /* out */
    T_DIMSE_C_StoreRSP *rsp,            /* final store response */
    DcmDataset **statusDetail)
{
  if (progress->state == DIMSE_StoreEnd)
  {
    StoreContext *context = (StoreContext *)callbackData;
    if (rsp->DimseStatus == STATUS_Success)
    {
      if ((imageDataSet)&&(*imageDataSet))
      {
        checkRequestAgainstDataset(req, NULL, *imageDataSet, rsp, context->opt_correctUIDPadding);
      } else {
        checkRequestAgainstDataset(req, imageFileName, NULL, rsp, context->opt_correctUIDPadding);
      }
    }

    if (rsp->DimseStatus == STATUS_Success)
    {
      if ((imageDataSet)&&(*imageDataSet))
      {
        OFCondition cond = context->dcmff->saveFile(context->fileName, EXS_LittleEndianExplicit,
          EET_ExplicitLength, EGL_recalcGL);
        if (! cond.good())
        {
          OFLOG_ERROR(dcmpsrcvLogger, "Cannot write image file: " << context->fileName);
          rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;

          // delete incomplete file
          OFStandard::deleteFile(context->fileName);
        }
      }
      saveImageToDB(context, req, context->fileName, rsp, statusDetail);
    }
  }
}

static OFCondition echoSCP(T_ASC_Association *assoc, T_DIMSE_C_EchoRQ *req, T_ASC_PresentationContextID presId)
{
    OFLOG_INFO(dcmpsrcvLogger, "Received Echo SCP RQ: MsgID " << req->MessageID);
    OFCondition cond = DIMSE_sendEchoResponse(assoc, presId, req, STATUS_Success, NULL);
    errorCond(cond, "echoSCP: Echo Response Failed:");
    return cond;
}


static OFCondition storeSCP(
  T_ASC_Association *assoc,
  T_DIMSE_C_StoreRQ *request,
  T_ASC_PresentationContextID presId,
  const char *dbfolder,
  OFBool opt_bitpreserving,
  OFBool opt_correctUIDPadding)
{
    OFString str;
    OFLOG_INFO(dcmpsrcvLogger, "Received Store SCP:\n" << DIMSE_dumpMessage(str, *request, DIMSE_INCOMING));

    OFCondition cond = EC_Normal;
    char imageFileName[MAXPATHLEN+1];
    DcmFileFormat dcmff;

    // store SourceApplicationEntityTitle in metaheader
    if (assoc && assoc->params)
    {
      const char *aet = assoc->params->DULparams.callingAPTitle;
      if (aet) dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
    }

    DcmDataset *dset = dcmff.getDataset();
    DIC_US status = STATUS_Success;
    DcmQueryRetrieveIndexDatabaseHandle *dbhandle = NULL;

    if (!dcmIsaStorageSOPClassUID(request->AffectedSOPClassUID))
    {
        /* callback will send back sop class not supported status */
        status = STATUS_STORE_Refused_SOPClassNotSupported;
        /* must still receive data */
        OFStandard::strlcpy(imageFileName, NULL_DEVICE_NAME, sizeof(imageFileName));
    }
    else
    {
      dbhandle = new DcmQueryRetrieveIndexDatabaseHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, cond);
      if (cond.bad())
      {
        OFLOG_ERROR(dcmpsrcvLogger, "Unable to access database '" << dbfolder << "'");
        /* must still receive data */
        OFStandard::strlcpy(imageFileName, NULL_DEVICE_NAME, sizeof(imageFileName));
        /* callback will send back out of resources status */
        status = STATUS_STORE_Refused_OutOfResources;
        dbhandle = NULL;
      }
      else
      {
        if (dbhandle->makeNewStoreFileName(
            request->AffectedSOPClassUID,
            request->AffectedSOPInstanceUID,
            imageFileName, sizeof(imageFileName)).bad())
        {
            OFLOG_ERROR(dcmpsrcvLogger, "storeSCP: Database: DB_makeNewStoreFileName Failed");
            /* must still receive data */
            OFStandard::strlcpy(imageFileName, NULL_DEVICE_NAME, sizeof(imageFileName));
            /* callback will send back out of resources status */
            status = STATUS_STORE_Refused_OutOfResources;
        }
      }
    }

#ifdef LOCK_IMAGE_FILES
    /* exclusively lock image file, but only on Unix systems -
         * on Win32 we would prevent ourselves from writing the file!
         */
#ifdef O_BINARY
    int lockfd = open(imageFileName, (O_WRONLY | O_CREAT | O_TRUNC | O_BINARY), 0666);
#else
    int lockfd = open(imageFileName, (O_WRONLY | O_CREAT | O_TRUNC), 0666);
#endif

    dcmtk_flock(lockfd, LOCK_EX);
#endif

    /* we must still retrieve the data set even if some error has occured */
    StoreContext context(dbhandle, status, imageFileName, &dcmff, opt_correctUIDPadding);

    if (opt_bitpreserving)
    {
        cond = DIMSE_storeProvider(assoc, presId, request, imageFileName, 1,
                                   NULL, storeProgressCallback,
                                   (void*)&context, DIMSE_BLOCKING, 0);
    } else {
        cond = DIMSE_storeProvider(assoc, presId, request, (char *)NULL, 1,
                                   &dset, storeProgressCallback,
                                   (void*)&context, DIMSE_BLOCKING, 0);
    }
    errorCond(cond, "Store SCP Failed:");

    if (cond.bad() || (context.status != STATUS_Success))
    {
        /* remove file */
        OFLOG_INFO(dcmpsrcvLogger, "Store SCP: Deleting Image File: " << imageFileName);
        OFStandard::deleteFile(imageFileName);
        if (dbhandle) dbhandle->pruneInvalidRecords();
    }

#ifdef LOCK_IMAGE_FILES
    /* unlock image file */
    dcmtk_flock(lockfd, LOCK_UN);
    close(lockfd);
#endif

    /* free DB handle */
    delete dbhandle;

    if (messageClient)
    {
      OFOStringStream out;
      Uint32 operationStatus = DVPSIPCMessage::statusError;
      if (cond.good())
      {
        if (context.status == STATUS_Success) operationStatus = DVPSIPCMessage::statusOK;
        else operationStatus = DVPSIPCMessage::statusWarning;
      }
      const char *sopClassName = dcmFindNameOfUID(request->AffectedSOPClassUID);
      const char *successName = "failed";
      if (operationStatus == DVPSIPCMessage::statusOK) successName = "successful";
      if (sopClassName==NULL) sopClassName = request->AffectedSOPClassUID;

      out << "DICOM C-STORE receipt " << successName << ": " << OFendl
          << "\tSOP class UID          : " << sopClassName << OFendl
          << "\tSOP instance UID       : " << request->AffectedSOPInstanceUID << OFendl;

      if (operationStatus == DVPSIPCMessage::statusOK)
      {
        unsigned long fileSize = 0;
        struct stat fileStat;
        if (0 == stat(imageFileName, &fileStat)) fileSize = fileStat.st_size;
        out
          << "\tTarget file path       : " << imageFileName << OFendl
          << "\tTarget file size (kB)  : " << (fileSize+1023)/1024 << OFendl;
      }

      out << "\tDIMSE presentation ctx : " << (int)presId << OFendl
          << "\tDIMSE message ID       : " << request->MessageID << OFendl
          << "\tDIMSE status           : " << DU_cstoreStatusString(context.status) << OFendl
          << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(out, theString)
      messageClient->notifyReceivedDICOMObject(operationStatus, theString);
      OFSTRINGSTREAM_FREESTR(theString)
    }

    return cond;
}

static void handleClient(
  T_ASC_Association **assoc,
  const char *dbfolder,
  OFBool opt_bitpreserving,
  OFBool useTLS,
  OFBool opt_correctUIDPadding)
{
  OFCondition cond = ASC_acknowledgeAssociation(*assoc);
  if (! errorCond(cond, "Cannot acknowledge association:"))
  {
    OFLOG_INFO(dcmpsrcvLogger, "Association Acknowledged (Max Send PDV: " << (*assoc)->sendPDVLength << ")"
        << (ASC_countAcceptedPresentationContexts((*assoc)->params) == 0 ? "    (but no valid presentation contexts)" : ""));

    if (messageClient)
    {
      // notify about successfully negotiated association
      OFOStringStream out;
      OFString temp_str;
      out << "DIMSE Association Acknowledged:" << OFendl
          << "  calling presentation address: " << (*assoc)->params->DULparams.callingPresentationAddress << OFendl
          << "  calling AE title: " << (*assoc)->params->DULparams.callingAPTitle << OFendl
          << "  called AE title: " << (*assoc)->params->DULparams.calledAPTitle << OFendl
          << "  max send PDV: " << (*assoc)->sendPDVLength << OFendl
          << "  presentation contexts: " << ASC_countAcceptedPresentationContexts((*assoc)->params) << OFendl;
      out << ASC_dumpConnectionParameters(temp_str, *assoc) << OFendl;
      out << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(out, theString)
      if (useTLS)
        messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusOK, theString);
        else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusOK, theString);
      OFSTRINGSTREAM_FREESTR(theString)
    }

    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presID;
    cond = EC_Normal;

    /* do real work */
    while (cond.good())
    {
      cond = DIMSE_receiveCommand(*assoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL);
      /* did peer release, abort, or do we have a valid message ? */

      if (cond.good())
      {
          /* process command */
          switch (msg.CommandField)
          {
            case DIMSE_C_ECHO_RQ:
              cond = echoSCP(*assoc, &msg.msg.CEchoRQ, presID);
              break;
            case DIMSE_C_STORE_RQ:
              cond = storeSCP(*assoc, &msg.msg.CStoreRQ, presID, dbfolder, opt_bitpreserving, opt_correctUIDPadding);
              break;
            default:
              cond = DIMSE_BADCOMMANDTYPE; /* unsupported command */
              OFLOG_ERROR(dcmpsrcvLogger, "Cannot handle command: 0x" << STD_NAMESPACE hex << (unsigned)msg.CommandField);
              break;
          }
      }
      else
      {
         /* finish processing loop */
      }
    } /* while */

    /* close association */
    if (cond == DUL_PEERREQUESTEDRELEASE)
    {
      OFLOG_INFO(dcmpsrcvLogger, "Association Release");
      cond = ASC_acknowledgeRelease(*assoc);
      errorCond(cond, "Cannot release association:");
      if (messageClient) messageClient->notifyConnectionClosed(DVPSIPCMessage::statusOK);
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
      OFLOG_INFO(dcmpsrcvLogger, "Association Aborted");
      if (messageClient) messageClient->notifyConnectionAborted(DVPSIPCMessage::statusWarning, "DIMSE association aborted by remote peer");
    }
    else
    {
      errorCond(cond, "DIMSE Failure (aborting association):");
      cond = ASC_abortAssociation(*assoc);
      errorCond(cond, "Cannot abort association:");
      if (messageClient) messageClient->notifyConnectionAborted(DVPSIPCMessage::statusError, "DIMSE failure, aborting association");
    }
  }
  dropAssociation(assoc);
}

static void terminateAllReceivers(DVConfiguration& dvi)
{
  OFLOG_INFO(dcmpsrcvLogger, "Terminating all receivers");

  const char *recID=NULL;
  const char *recAETitle=NULL;
  unsigned short recPort=0;
  OFBool recUseTLS=OFFalse;
  T_ASC_Network *net=NULL;
  T_ASC_Parameters *params=NULL;
  DIC_NODENAME peerHost;
  T_ASC_Association *assoc=NULL;
  OFBool prepared = OFTrue;
  const char *xfer = UID_LittleEndianImplicitTransferSyntax;

#ifdef WITH_OPENSSL
  /* TLS directory */
  const char *current = NULL;
  const char *tlsFolder = dvi.getTLSFolder();
  if (tlsFolder==NULL) tlsFolder = ".";

  /* key file format */
  DcmKeyFileFormat keyFileFormat = DCF_Filetype_PEM;
  if (! dvi.getTLSPEMFormat()) keyFileFormat = DCF_Filetype_PEM;
#endif

  if ((ASC_initializeNetwork(NET_REQUESTOR, 0, 30, &net).bad())) return;

  Uint32 numReceivers = dvi.getNumberOfTargets(DVPSE_receiver);
  for (Uint32 i=0; i<numReceivers; i++)
  {
    prepared = OFTrue;
    recID = dvi.getTargetID(i, DVPSE_receiver);
    recPort = dvi.getTargetPort(recID);
    recUseTLS = dvi.getTargetUseTLS(recID);
    recAETitle = dvi.getTargetAETitle(recID);

    OFLOG_INFO(dcmpsrcvLogger, "Receiver " << recID << " on port " << recPort
         << (recUseTLS ? " with TLS" : ""));

    if (recUseTLS)
    {
#ifdef WITH_OPENSSL
      /* certificate file */
      OFString tlsCertificateFile(tlsFolder);
      tlsCertificateFile += PATH_SEPARATOR;
      current = dvi.getTargetCertificate(recID);
      if (current) tlsCertificateFile += current; else tlsCertificateFile += "sitecert.pem";

      /* private key file */
      OFString tlsPrivateKeyFile(tlsFolder);
      tlsPrivateKeyFile += PATH_SEPARATOR;
      current = dvi.getTargetPrivateKey(recID);
      if (current) tlsPrivateKeyFile += current; else tlsPrivateKeyFile += "sitekey.pem";

      /* private key password */
      const char *tlsPrivateKeyPassword = dvi.getTargetPrivateKeyPassword(recID);

      /* DH parameter file */
      OFString tlsDHParametersFile;
      current = dvi.getTargetDiffieHellmanParameters(recID);
      if (current)
      {
        tlsDHParametersFile = tlsFolder;
        tlsDHParametersFile += PATH_SEPARATOR;
        tlsDHParametersFile += current;
      }

      /* random seed file */
      OFString tlsRandomSeedFile(tlsFolder);
      tlsRandomSeedFile += PATH_SEPARATOR;
      current = dvi.getTargetRandomSeed(recID);
      if (current) tlsRandomSeedFile += current; else tlsRandomSeedFile += "siteseed.bin";

      /* CA certificate directory */
      const char *tlsCACertificateFolder = dvi.getTLSCACertificateFolder();
      if (tlsCACertificateFolder==NULL) tlsCACertificateFolder = ".";

      /* ciphersuites */
      DcmTLSTransportLayer *tLayer = new DcmTLSTransportLayer(NET_REQUESTOR, tlsRandomSeedFile.c_str(), OFFalse);
      if (tLayer)
      {

        // determine TLS profile
        OFString profileName;
        const char *profileNamePtr = dvi.getTargetTLSProfile(recID);
        if (profileNamePtr) profileName = profileNamePtr;
        DcmTLSSecurityProfile tlsProfile = TSP_Profile_BCP195;  // default
        if (profileName == "BCP195") tlsProfile = TSP_Profile_BCP195;
        else if (profileName == "BCP195-ND") tlsProfile = TSP_Profile_BCP195_ND;
        else if (profileName == "BCP195-EX") tlsProfile = TSP_Profile_BCP195_Extended;
        else if (profileName == "AES") tlsProfile = TSP_Profile_AES;
        else if (profileName == "BASIC") tlsProfile = TSP_Profile_Basic;
        else if (profileName == "NULL") tlsProfile = TSP_Profile_IHE_ATNA_Unencrypted;

        (void) tLayer->setTLSProfile(tlsProfile);
        (void) tLayer->activateCipherSuites();

        if (tlsCACertificateFolder) tLayer->addTrustedCertificateDir(tlsCACertificateFolder, keyFileFormat);
        if (tlsDHParametersFile.size() > 0) tLayer->setTempDHParameters(tlsDHParametersFile.c_str());
        tLayer->setPrivateKeyPasswd(tlsPrivateKeyPassword); // never prompt on console
        tLayer->setPrivateKeyFile(tlsPrivateKeyFile.c_str(), keyFileFormat);
        tLayer->setCertificateFile(tlsCertificateFile.c_str(), keyFileFormat);
        tLayer->setCertificateVerification(DCV_ignoreCertificate);
        ASC_setTransportLayer(net, tLayer, 1);
      }
#else
      prepared = OFFalse;
#endif
    } else {
      DcmTransportLayer *dLayer = new DcmTransportLayer();
      ASC_setTransportLayer(net, dLayer, 1);
    }
    if (prepared && recAETitle && (recPort > 0))
    {
      if ((ASC_createAssociationParameters(&params, DEFAULT_MAXPDU)).good())
      {
        ASC_setTransportLayerType(params, recUseTLS);
        ASC_setAPTitles(params, dvi.getNetworkAETitle(), recAETitle, NULL);
        sprintf(peerHost, "%s:%d", "localhost", (int)recPort);
        ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), peerHost);
        // we propose only the "shutdown" SOP class in implicit VR
        ASC_addPresentationContext(params, 1, UID_PrivateShutdownSOPClass, &xfer, 1);
        // request shutdown association, abort if some strange peer accepts it
        if (ASC_requestAssociation(net, params, &assoc).good()) ASC_abortAssociation(assoc);
        ASC_dropAssociation(assoc);
        ASC_destroyAssociation(&assoc);
      }
    }
  } /* for loop */

  ASC_dropNetwork(&net);
  OFStandard::shutdownNetwork();
  return;
}


// ********************************************

#define SHORTCOL 3
#define LONGCOL 12

int main(int argc, char *argv[])
{
    OFStandard::initializeNetwork();
#ifdef WITH_OPENSSL
    DcmTLSTransportLayer::initializeOpenSSL();
#endif

    int         opt_terminate = 0;         /* default: no terminate mode */
    const char *opt_cfgName   = NULL;      /* config file name */
    const char *opt_cfgID     = NULL;      /* name of entry in config file */

    dcmDisableGethostbyaddr.set(OFTrue);               // disable hostname lookup

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Network receive for presentation state viewer", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 2);

    cmd.addParam("config-file",  "configuration file to be read");
    cmd.addParam("receiver-id",  "identifier of receiver in config file", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:");
     cmd.addOption("--help",      "-h", "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",         "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);
     cmd.addOption("--terminate", "-t", "terminate all running receivers");

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);
            COUT << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_OPENSSL)
            COUT << " none" << OFendl;
#else
            COUT << OFendl;
#endif
#ifdef WITH_ZLIB
            COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef WITH_OPENSSL
            COUT << "- " << DcmTLSTransportLayer::getOpenSSLVersionName() << OFendl;
#endif
            return 0;
         }
      }

      /* command line parameters and options */
      cmd.getParam(1, opt_cfgName);
      if (cmd.getParamCount() >= 2) cmd.getParam(2, opt_cfgID);
      if (cmd.findOption("--terminate")) opt_terminate = 1;

      OFLog::configureFromCommandLine(cmd, app);
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmpsrcvLogger, rcsid << OFendl);

    if ((opt_cfgID == 0)&&(! opt_terminate))
    {
      OFLOG_FATAL(dcmpsrcvLogger, "parameter receiver-id required unless --terminate is specified");
      return 10;
    }

    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        OFLOG_FATAL(dcmpsrcvLogger, "can't open configuration file '" << opt_cfgName << "'");
        return 10;
      }
    } else {
      OFLOG_FATAL(dcmpsrcvLogger, "missing configuration file name");
      return 10;
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
      OFLOG_WARN(dcmpsrcvLogger, "no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    DVConfiguration dvi(opt_cfgName);
    if (opt_terminate)
    {
      terminateAllReceivers(dvi);
      return 0;  // application terminates here
    }

    /* get network configuration from configuration file */
    OFBool networkImplicitVROnly  = dvi.getTargetImplicitOnly(opt_cfgID);
    OFBool networkBitPreserving   = dvi.getTargetBitPreservingMode(opt_cfgID);
    OFBool opt_correctUIDPadding  = dvi.getTargetCorrectUIDPadding(opt_cfgID);
    OFBool networkDisableNewVRs   = dvi.getTargetDisableNewVRs(opt_cfgID);
    unsigned short networkPort    = dvi.getTargetPort(opt_cfgID);
    unsigned long  networkMaxPDU  = dvi.getTargetMaxPDU(opt_cfgID);
    const char *networkAETitle    = dvi.getTargetAETitle(opt_cfgID);
    if (networkAETitle==NULL) networkAETitle = dvi.getNetworkAETitle();
    unsigned short messagePort    = dvi.getMessagePort();   /* port number for IPC */
    OFBool keepMessagePortOpen    = dvi.getMessagePortKeepOpen();
    OFBool useTLS = dvi.getTargetUseTLS(opt_cfgID);
    OFBool notifyTermination      = OFTrue;  // notify IPC server of application termination
#ifdef WITH_OPENSSL
    /* TLS directory */
    const char *current = NULL;
    const char *tlsFolder = dvi.getTLSFolder();
    if (tlsFolder==NULL) tlsFolder = ".";

    /* certificate file */
    OFString tlsCertificateFile(tlsFolder);
    tlsCertificateFile += PATH_SEPARATOR;
    current = dvi.getTargetCertificate(opt_cfgID);
    if (current) tlsCertificateFile += current; else tlsCertificateFile += "sitecert.pem";

    /* private key file */
    OFString tlsPrivateKeyFile(tlsFolder);
    tlsPrivateKeyFile += PATH_SEPARATOR;
    current = dvi.getTargetPrivateKey(opt_cfgID);
    if (current) tlsPrivateKeyFile += current; else tlsPrivateKeyFile += "sitekey.pem";

    /* private key password */
    const char *tlsPrivateKeyPassword = dvi.getTargetPrivateKeyPassword(opt_cfgID);

    /* certificate verification */
    DcmCertificateVerification tlsCertVerification = DCV_requireCertificate;
    switch (dvi.getTargetPeerAuthentication(opt_cfgID))
    {
      case DVPSQ_require:
        tlsCertVerification = DCV_requireCertificate;
        break;
      case DVPSQ_verify:
        tlsCertVerification = DCV_checkCertificate;
        break;
      case DVPSQ_ignore:
        tlsCertVerification = DCV_ignoreCertificate;
        break;
    }

    /* DH parameter file */
    OFString tlsDHParametersFile;
    current = dvi.getTargetDiffieHellmanParameters(opt_cfgID);
    if (current)
    {
      tlsDHParametersFile = tlsFolder;
      tlsDHParametersFile += PATH_SEPARATOR;
      tlsDHParametersFile += current;
    }

    /* random seed file */
    OFString tlsRandomSeedFile(tlsFolder);
    tlsRandomSeedFile += PATH_SEPARATOR;
    current = dvi.getTargetRandomSeed(opt_cfgID);
    if (current) tlsRandomSeedFile += current; else tlsRandomSeedFile += "siteseed.bin";

    /* CA certificate directory */
    const char *tlsCACertificateFolder = dvi.getTLSCACertificateFolder();
    if (tlsCACertificateFolder==NULL) tlsCACertificateFolder = ".";

    /* key file format */
    DcmKeyFileFormat keyFileFormat = DCF_Filetype_PEM;
    if (! dvi.getTLSPEMFormat()) keyFileFormat = DCF_Filetype_ASN1;

#else
    if (useTLS)
    {
      OFLOG_FATAL(dcmpsrcvLogger, "not compiled with OpenSSL, cannot use TLS");
      return 10;
    }
#endif

    if (networkAETitle==NULL)
    {
      OFLOG_FATAL(dcmpsrcvLogger, "no application entity title");
      return 10;
    }

    if (networkPort==0)
    {
      OFLOG_FATAL(dcmpsrcvLogger, "no or invalid port number");
      return 10;
    }

#ifndef DISABLE_PORT_PERMISSION_CHECK
#ifdef HAVE_GETEUID
    /* if port is privileged we must be as well */
    if ((networkPort < 1024)&&(geteuid() != 0))
    {
      OFLOG_FATAL(dcmpsrcvLogger, "cannot listen on port " << networkPort << ", insufficient privileges");
      return 10;
    }
#endif
#endif

    if (networkMaxPDU==0) networkMaxPDU = DEFAULT_MAXPDU;
    else if (networkMaxPDU > ASC_MAXIMUMPDUSIZE)
    {
      OFLOG_FATAL(dcmpsrcvLogger, "max PDU size " << networkMaxPDU << " too big, using default: " << DEFAULT_MAXPDU);
      networkMaxPDU = DEFAULT_MAXPDU;
    }

    if (networkDisableNewVRs)
    {
      dcmDisableGenerationOfNewVRs();
    }

    OFOStringStream verboseParameters;

    OFBool comma=OFFalse;
    verboseParameters << "Network parameters:" << OFendl
         << "  port            : " << networkPort << OFendl
         << "  aetitle         : " << networkAETitle << OFendl
         << "  max pdu         : " << networkMaxPDU << OFendl
         << "  options         : ";
    if (networkImplicitVROnly)
    {
      if (comma) verboseParameters << ", "; else comma=OFTrue;
      verboseParameters << "implicit xfer syntax only";
    }
    if (networkBitPreserving)
    {
      if (comma) verboseParameters << ", "; else comma=OFTrue;
      verboseParameters << "bit-preserving receive mode";
    }
    if (networkDisableNewVRs)
    {
      if (comma) verboseParameters << ", "; else comma=OFTrue;
      verboseParameters << "disable post-1993 VRs";
    }
    if (!comma) verboseParameters << "none";
    verboseParameters << OFendl;
    verboseParameters << "  TLS             : ";
    if (useTLS) verboseParameters << "enabled" << OFendl; else verboseParameters << "disabled" << OFendl;

    /* check if we can get access to the database */
    const char *dbfolder = dvi.getDatabaseFolder();

    OFLOG_INFO(dcmpsrcvLogger, "Using database in directory '" << dbfolder << "'");

    OFCondition cond2 = EC_Normal;
    DcmQueryRetrieveIndexDatabaseHandle *dbhandle = new DcmQueryRetrieveIndexDatabaseHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, cond2);
    delete dbhandle;

    if (cond2.bad())
    {
      OFLOG_FATAL(dcmpsrcvLogger, "Unable to access database '" << dbfolder << "'");
      return 1;
    }

    T_ASC_Network *net = NULL; /* the DICOM network and listen port */
    T_ASC_Association *assoc = NULL; /* the DICOM association */
    OFBool finished1 = OFFalse;
    OFBool finished2 = OFFalse;
    int connected = 0;
    OFCondition cond = EC_Normal;

#ifdef WITH_OPENSSL

    DcmTLSTransportLayer *tLayer = NULL;
    if (useTLS)
    {
      tLayer = new DcmTLSTransportLayer(NET_ACCEPTOR, tlsRandomSeedFile.c_str(), OFFalse);
      if (tLayer == NULL)
      {
        OFLOG_FATAL(dcmpsrcvLogger, "unable to create TLS transport layer");
        return 1;
      }

      // determine TLS profile
      OFString profileName;
      const char *profileNamePtr = dvi.getTargetTLSProfile(opt_cfgID);
      if (profileNamePtr) profileName = profileNamePtr;
      DcmTLSSecurityProfile tlsProfile = TSP_Profile_BCP195;  // default
      if (profileName == "BCP195") tlsProfile = TSP_Profile_BCP195;
      else if (profileName == "BCP195-ND") tlsProfile = TSP_Profile_BCP195_ND;
      else if (profileName == "AES") tlsProfile = TSP_Profile_AES;
      else if (profileName == "BASIC") tlsProfile = TSP_Profile_Basic;
      else if (profileName == "NULL") tlsProfile = TSP_Profile_IHE_ATNA_Unencrypted;
      else
      {
        OFLOG_WARN(dcmpsrcvLogger, "unknown TLS profile '" << profileName << "', ignoring");
      }

      if (TCS_ok != tLayer->setTLSProfile(tlsProfile))
      {
        OFLOG_FATAL(dcmpsrcvLogger, "unable to select the TLS security profile");
        return 1;
      }

      // activate cipher suites
      if (TCS_ok != tLayer->activateCipherSuites())
      {
        OFLOG_FATAL(dcmpsrcvLogger, "unable to activate the selected list of TLS ciphersuites");
        return 1;
      }

      if (tlsCACertificateFolder && (TCS_ok != tLayer->addTrustedCertificateDir(tlsCACertificateFolder, keyFileFormat)))
      {
        OFLOG_WARN(dcmpsrcvLogger, "unable to load certificates from directory '" << tlsCACertificateFolder << "', ignoring");
      }
      if ((tlsDHParametersFile.size() > 0) && ! (tLayer->setTempDHParameters(tlsDHParametersFile.c_str())))
      {
        OFLOG_WARN(dcmpsrcvLogger, "unable to load temporary DH parameter file '" << tlsDHParametersFile << "', ignoring");
      }
      tLayer->setPrivateKeyPasswd(tlsPrivateKeyPassword); // never prompt on console

      if (TCS_ok != tLayer->setPrivateKeyFile(tlsPrivateKeyFile.c_str(), keyFileFormat))
      {
        OFLOG_FATAL(dcmpsrcvLogger, "unable to load private TLS key from '" << tlsPrivateKeyFile<< "'");
        return 1;
      }
      if (TCS_ok != tLayer->setCertificateFile(tlsCertificateFile.c_str(), keyFileFormat))
      {
        OFLOG_FATAL(dcmpsrcvLogger, "unable to load certificate from '" << tlsCertificateFile << "'");
        return 1;
      }
      if (! tLayer->checkPrivateKeyMatchesCertificate())
      {
        OFLOG_FATAL(dcmpsrcvLogger, "private key '" << tlsPrivateKeyFile << "' and certificate '" << tlsCertificateFile << "' do not match");
        return 1;
      }

      tLayer->setCertificateVerification(tlsCertVerification);

    }

    if (useTLS)
    {
      OFString cslist;
      if (tLayer) tLayer->getListOfCipherSuitesForOpenSSL(cslist);
      verboseParameters << "  TLS certificate : " << tlsCertificateFile << OFendl
           << "  TLS key file    : " << tlsPrivateKeyFile << OFendl
           << "  TLS DH params   : " << tlsDHParametersFile << OFendl
           << "  TLS PRNG seed   : " << tlsRandomSeedFile << OFendl
           << "  TLS CA directory: " << tlsCACertificateFolder << OFendl
           << "  TLS ciphersuites: " << cslist << OFendl
           << "  TLS key format  : ";
      if (keyFileFormat == DCF_Filetype_PEM) verboseParameters << "PEM" << OFendl; else verboseParameters << "DER" << OFendl;
      verboseParameters << "  TLS cert verify : ";
      switch (tlsCertVerification)
      {
          case DCV_checkCertificate:
            verboseParameters << "verify" << OFendl;
            break;
          case DCV_ignoreCertificate:
            verboseParameters << "ignore" << OFendl;
            break;
          default:
            verboseParameters << "require" << OFendl;
            break;
      }
    }
#endif

    verboseParameters << OFStringStream_ends;
    OFSTRINGSTREAM_GETSTR(verboseParameters, verboseParametersString)
    OFLOG_INFO(dcmpsrcvLogger, verboseParametersString);

    while (!finished1)
    {
      /* open listen socket */
      cond = ASC_initializeNetwork(NET_ACCEPTOR, networkPort, 30, &net);
      if (errorCond(cond, "Error initialising network:"))
      {
        return 1;
      }

#ifdef WITH_OPENSSL
      if (tLayer)
      {
        cond = ASC_setTransportLayer(net, tLayer, 0);
        if (cond.bad())
        {
            OFString temp_str;
            OFLOG_FATAL(dcmpsrcvLogger, DimseCondition::dump(temp_str, cond));
            return 1;
        }
      }
#endif

      /* drop root privileges now and revert to the calling user id (if we are running as setuid root) */
      if (OFStandard::dropPrivileges().bad())
      {
          OFLOG_FATAL(dcmpsrcvLogger, "setuid() failed, maximum number of processes/threads for uid already running.");
          return 1;
      }

#ifdef HAVE_FORK
      int timeout=1;
#else
      int timeout=1000;
#endif
      while (!finished2)
      {
        /* now we connect to the IPC server and request an application ID */
        if (messageClient) // on Unix, re-initialize for each connect which is later inherited by the forked child
        {
          delete messageClient;
          messageClient = NULL;
        }
        if (messagePort > 0)
        {
          messageClient = new DVPSIPCClient(DVPSIPCMessage::clientStoreSCP, verboseParametersString, messagePort, keepMessagePortOpen);
          if (! messageClient->isServerActive())
          {
            OFLOG_WARN(dcmpsrcvLogger, "no IPC message server found at port " << messagePort << ", disabling IPC");
          }
        }
        connected = 0;
        while (!connected)
        {
           connected = ASC_associationWaiting(net, timeout);
           if (!connected) cleanChildren();
        }
        switch (negotiateAssociation(net, &assoc, networkAETitle, networkMaxPDU, networkImplicitVROnly, useTLS))
        {
          case assoc_error:
            // association has already been deleted, we just wait for the next client to connect.
            break;
          case assoc_terminate:
            finished2=OFTrue;
            finished1=OFTrue;
            notifyTermination = OFFalse; // IPC server will probably already be down
            cond = ASC_dropNetwork(&net);
            if (errorCond(cond, "Error dropping network:")) return 1;
            break;
          case assoc_success:
#ifdef HAVE_FORK
            // Unix version - call fork()
            int pid;
            pid = (int)(fork());
            if (pid < 0)
            {
              char buf[256];
              OFLOG_ERROR(dcmpsrcvLogger, "Cannot create association sub-process: " << OFStandard::strerror(errno, buf, sizeof(buf)));
              refuseAssociation(assoc, ref_CannotFork);

              if (messageClient)
              {
                // notify about rejected association
                OFOStringStream out;
                OFString temp_str;
                out << "DIMSE Association Rejected:" << OFendl
                    << "  reason: cannot create association sub-process: " << OFStandard::strerror(errno, buf, sizeof(buf)) << OFendl
                    << "  calling presentation address: " << assoc->params->DULparams.callingPresentationAddress << OFendl
                    << "  calling AE title: " << assoc->params->DULparams.callingAPTitle << OFendl
                    << "  called AE title: " << assoc->params->DULparams.calledAPTitle << OFendl;
                out << ASC_dumpConnectionParameters(temp_str, assoc) << OFendl;
                out << OFStringStream_ends;
                OFSTRINGSTREAM_GETSTR(out, theString)
                if (useTLS)
                  messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
                  else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
                OFSTRINGSTREAM_FREESTR(theString)
              }
              dropAssociation(&assoc);
            } else if (pid > 0)
            {
              /* parent process */
              assoc = NULL;
            } else {
              /* child process */

#ifdef WITH_OPENSSL
              // a generated UID contains the process ID and current time.
              // Adding it to the PRNG seed guarantees that we have different seeds for
              // different child processes.
              char randomUID[65];
              dcmGenerateUniqueIdentifier(randomUID);
              if (tLayer) tLayer->addPRNGseed(randomUID, strlen(randomUID));
#endif
              handleClient(&assoc, dbfolder, networkBitPreserving, useTLS, opt_correctUIDPadding);
              finished2=OFTrue;
              finished1=OFTrue;
            }
#else
            // Windows version - call CreateProcess()
            finished2=OFTrue;
            cond = ASC_dropNetwork(&net);
            if (errorCond(cond, "Error dropping network:"))
            {
              if (messageClient)
              {
                messageClient->notifyApplicationTerminates(DVPSIPCMessage::statusError);
                delete messageClient;
              }
              return 1;
            }

            // initialize startup info
            const char *receiver_application = dvi.getReceiverName();
            PROCESS_INFORMATION procinfo;
            STARTUPINFO sinfo;
            OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
            sinfo.cb = sizeof(sinfo);
            char commandline[4096];
            sprintf(commandline, "%s %s %s", receiver_application, opt_cfgName, opt_cfgID);
#ifdef DEBUG
            if (CreateProcess(NULL, commandline, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo))
#else
            if (CreateProcess(NULL, commandline, NULL, NULL, 0, DETACHED_PROCESS, NULL, NULL, &sinfo, &procinfo))
#endif
            {
#ifdef WITH_OPENSSL
              // a generated UID contains the process ID and current time.
              // Adding it to the PRNG seed guarantees that we have different seeds for
              // different child processes.
              char randomUID[65];
              dcmGenerateUniqueIdentifier(randomUID);
              if (tLayer) tLayer->addPRNGseed(randomUID, strlen(randomUID));
#endif
              handleClient(&assoc, dbfolder, networkBitPreserving, useTLS, opt_correctUIDPadding);
              finished1=OFTrue;
            } else {
              OFLOG_ERROR(dcmpsrcvLogger, "Cannot execute command line: " << commandline);
              refuseAssociation(assoc, ref_CannotFork);

              if (messageClient)
              {
                // notify about rejected association
                OFOStringStream out;
                out << "DIMSE Association Rejected:" << OFendl
                    << "  reason: cannot execute command line: " << commandline << OFendl
                    << "  calling presentation address: " << assoc->params->DULparams.callingPresentationAddress << OFendl
                    << "  calling AE title: " << assoc->params->DULparams.callingAPTitle << OFendl
                    << "  called AE title: " << assoc->params->DULparams.calledAPTitle << OFendl;
                ASC_dumpConnectionParameters(assoc, out);
                out << OFStringStream_ends;
                OFSTRINGSTREAM_GETSTR(out, theString)
                if (useTLS)
                  messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
                  else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
                OFSTRINGSTREAM_FREESTR(theString)
              }

              dropAssociation(&assoc);
            }
#endif
            break;
        }
      } // finished2
    } // finished1
    cleanChildren();

    // tell the IPC server that we're going to terminate.
    // We need to do this before we shutdown WinSock.
    if (messageClient && notifyTermination)
    {
      messageClient->notifyApplicationTerminates(DVPSIPCMessage::statusOK);
      delete messageClient;
    }

    OFStandard::shutdownNetwork();

#ifdef WITH_OPENSSL
    if (tLayer)
    {
      if (tLayer->canWriteRandomSeed())
      {
        if (!tLayer->writeRandomSeed(tlsRandomSeedFile.c_str()))
        {
          OFLOG_WARN(dcmpsrcvLogger, "cannot write back random seed file '" << tlsRandomSeedFile << "', ignoring");
        }
      } else {
        OFLOG_WARN(dcmpsrcvLogger, "cannot write back random seed, ignoring");
      }
    }
    delete tLayer;
#endif

    OFSTRINGSTREAM_FREESTR(verboseParametersString)

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return 0;
}
