/*
 *
 *  Copyright (C) 1998-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmpstat
 *
 *  Authors: Marco Eichelberg
 *
 *  Purpose: Presentation State Viewer - Network Receive Component (Store SCP)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:50:09 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmpsrcv.cc,v $
 *  CVS/RCS Revision: $Revision: 1.29 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */
 
#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

BEGIN_EXTERN_C
#ifdef HAVE_FCNTL_H
#include <fcntl.h>    /* for O_RDONLY */
#endif
END_EXTERN_C

#include "dvpsdef.h"  /* for constants */
#include "dviface.h"
#include "ofbmanip.h" /* for OFBitmanipTemplate */
#include "dcuid.h"    /* for dcmtk version name */
#include "diutil.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "imagedb.h"     /* for LOCK_IMAGE_FILES */
#include "dvpsmsg.h"     /* for class DVPSIPCClient */
#include "dcmlayer.h"
#ifdef WITH_OPENSSL
#include "tlstrans.h"
#include "tlslayer.h"
#endif

#ifdef HAVE_STRSTREAM_H
#include <strstream.h>
#endif

#ifdef HAVE_STRSTREA_H
#include <strstrea.h>
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmpsrcv"

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

static int errorCond(CONDITION cond, const char *message)
{
  int result = (!SUCCESS(cond));
  if (result) 
  {  
    CERR << message << endl;
    COND_DumpConditions(); 
  }
  return result;
}

static void cleanChildren()
{
#ifdef HAVE_WAITPID
    int stat_loc;
#elif HAVE_WAIT3
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
           if (errno != ECHILD) CERR << "wait for child failed: " << strerror(errno) << endl;
        }
    }
#endif
}

static void dropAssociation(T_ASC_Association **assoc)
{
  if ((assoc == NULL)||(*assoc == NULL)) return;
  CONDITION cond = ASC_dropSCPAssociation(*assoc);
  errorCond(cond, "Cannot Drop Association:");
  cond = ASC_destroyAssociation(assoc);
  errorCond(cond, "Cannot Destroy Association:");
  *assoc = NULL;
  COND_PopCondition(OFTrue);
  return;
}


static CONDITION
refuseAssociation(T_ASC_Association *assoc, refuseReason reason)
{
    CONDITION cond;
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

    cond = ASC_rejectAssociation(assoc, &rej);
    errorCond(cond, "Association Reject Failed:");

    return cond;
}

static associationType negotiateAssociation(
  T_ASC_Network *net, 
  T_ASC_Association **assoc, 
  const char *aetitle,
  unsigned long maxPDU, 
  OFBool opt_networkImplicitVROnly,
  OFBool opt_verbose,
  OFBool useTLS)
{
    associationType result = assoc_success;
    char buf[BUFSIZ];    
    OFBool dropAssoc = OFFalse;
    
    CONDITION cond = ASC_receiveAssociation(net, assoc, maxPDU, NULL, NULL, useTLS);

    if (errorCond(cond, "Failed to receive association:")) 
    {
      dropAssoc = OFTrue;
      result = assoc_error;

      if (messageClient)
      {
        // notify about failed association setup
        ostrstream out;
        out << "Unable to Receive DIMSE Association Request:" << endl;
        COND_DumpConditions(out); 
        out << ends;
        char *theString = out.str();              
        if (useTLS) 
          messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
          else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
        delete[] theString;
      }
    }
    else 
    {

      if (opt_verbose)
      {
          time_t t = time(NULL);
          CERR << "Association Received (" << (*assoc)->params->DULparams.callingPresentationAddress
             << ":" << (*assoc)->params->DULparams.callingAPTitle << " -> " 
             << (*assoc)->params->DULparams.calledAPTitle
             << ") " << ctime(&t);
      }

      ASC_setAPTitles((*assoc)->params, NULL, NULL, aetitle);
      
      /* Application Context Name */
      if ((cond = ASC_getApplicationContextName((*assoc)->params, buf) != ASC_NORMAL) || strcmp(buf, DICOM_STDAPPLICATIONCONTEXT) != 0)
      {
          /* reject: the application context name is not supported */
          if (opt_verbose) CERR << "Bad AppContextName: " << buf << endl;
          cond = refuseAssociation(*assoc, ref_BadAppContext);

          if (messageClient)
          {
            // notify about rejected association
            ostrstream out;
            out << "DIMSE Association Rejected:" << endl
                << "  reason: bad application context name '" << buf << "'" << endl
                << "  calling presentation address: " << (*assoc)->params->DULparams.callingPresentationAddress << endl
                << "  calling AE title: " << (*assoc)->params->DULparams.callingAPTitle << endl
                << "  called AE title: " << (*assoc)->params->DULparams.calledAPTitle << endl;
            ASC_dumpConnectionParameters(*assoc, out);
            out << ends;
            char *theString = out.str();              
            if (useTLS) 
              messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
              else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
            delete[] theString;
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
          dcmStorageSOPClassUIDs, numberOfDcmStorageSOPClassUIDs,
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
  DB_Handle *dbHandle;
  DIC_US status;
  DcmDataset *statusDetail;
  const char *fileName;
  DcmFileFormat *dcmff;
  
  StoreContext(DB_Handle *handle, DIC_US aStatus, const char *fname, DcmFileFormat *ff)
  : dbHandle(handle), status(aStatus), statusDetail(NULL), fileName(fname), dcmff(ff)
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
    T_DIMSE_C_StoreRSP *rsp)    /* final store response */
{
    DcmFileFormat ff;

    if (dataSet == NULL)
    {
      /* load the data from file */
      DcmFileStream istrm(fname, DCM_ReadMode);
      if (istrm.Fail())
      {
        CERR << "Cannot open file: " << fname << endl;
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
        return;
      }
      ff.read(istrm, EXS_Unknown, EGL_noChange);
      dataSet = ff.getDataset();
    }

    /* which SOP class and SOP instance ? */
    DIC_UI sopClass;
    DIC_UI sopInstance;
    
    if (!DU_findSOPClassAndInstanceInDataSet(dataSet, sopClass, sopInstance))
    {
      CERR << "Bad image file: " << fname << endl;
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
      DVPresentationState pstate;
      if (EC_Normal != pstate.read(*dataSet))
      {
        CERR << "Grayscale softcopy presentation state object cannot be displayed - rejected." << endl;
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
    DB_Status dbStatus;
    dbStatus.status = STATUS_Success;
    dbStatus.statusDetail = NULL;
    
    /* Store image */    
    if (context->status == STATUS_Success)
    {
      if (DB_NORMAL != DB_storeRequest(context->dbHandle,
          req->AffectedSOPClassUID, req->AffectedSOPInstanceUID,
          imageFileName, &dbStatus))
      {
        CERR << "storeSCP: Database: DB_storeRequest Failed (" 
             << DU_cstoreStatusString(dbStatus.status) << ")" << endl;
        COND_DumpConditions();
      }
    }
    else
    {
      dbStatus.status = context->status;
      dbStatus.statusDetail = context->statusDetail;
    }
    rsp->DimseStatus = dbStatus.status;
    *statusDetail = dbStatus.statusDetail;
    context->statusDetail = dbStatus.statusDetail;
    context->status = dbStatus.status;
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
        checkRequestAgainstDataset(req, NULL, *imageDataSet, rsp);
      } else {
        checkRequestAgainstDataset(req, imageFileName, NULL, rsp);
      }
    }

    if (rsp->DimseStatus == STATUS_Success)
    {
      if ((imageDataSet)&&(*imageDataSet))
      {
        
        DcmFileStream outf(context->fileName, DCM_WriteMode);
        if (outf.Fail())
        {
          CERR << "Cannot write image file: " << context->fileName << endl;
          rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
        } 
        else
        {
          context->dcmff->transferInit();
          context->dcmff->write(outf, EXS_LittleEndianExplicit, EET_ExplicitLength, EGL_recalcGL);
          context->dcmff->transferEnd();
          if (context->dcmff->error() != EC_Normal)
          {
            CERR << "Cannot write image file: " << context->fileName << endl;
            rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
          }
        }
      }
      saveImageToDB(context, req, context->fileName, rsp, statusDetail);
    }
  }
}

static CONDITION echoSCP(T_ASC_Association *assoc, T_DIMSE_C_EchoRQ *req, T_ASC_PresentationContextID presId, OFBool opt_verbose)
{
    if (opt_verbose) CERR << "Received Echo SCP RQ: MsgID " << req->MessageID << endl;
    CONDITION cond = DIMSE_sendEchoResponse(assoc, presId, req, STATUS_Success, NULL);
    errorCond(cond, "echoSCP: Echo Response Failed:");
    return cond;
}


static CONDITION storeSCP(
  T_ASC_Association *assoc, 
  T_DIMSE_C_StoreRQ *request,
  T_ASC_PresentationContextID presId,
  const char *dbfolder,
  OFBool opt_verbose, 
  OFBool opt_bitpreserving)
{
    if (opt_verbose) 
    {
      fprintf(stderr, "Received Store SCP: ");
      DIMSE_printCStoreRQ(stderr, request);
      fflush(stderr);
    }
    
    CONDITION cond = DIMSE_NORMAL;
    char imageFileName[MAXPATHLEN+1];
    DcmFileFormat dcmff;
    DcmDataset *dset = dcmff.getDataset();
    DIC_US status = STATUS_Success;
    DB_Handle *dbhandle = NULL;
        
    if (!dcmIsaStorageSOPClassUID(request->AffectedSOPClassUID))
    {
        /* callback will send back sop class not supported status */ 
        status = STATUS_STORE_Refused_SOPClassNotSupported;
        /* must still receive data */
        strcpy(imageFileName, NULL_DEVICE_NAME);
    } 
    else
    {
      if (DB_NORMAL != DB_createHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, &dbhandle))
      {
        CERR << "Unable to access database '" << dbfolder << "'" << endl;
        COND_DumpConditions();
        /* must still receive data */
        strcpy(imageFileName, NULL_DEVICE_NAME);     
        /* callback will send back out of resources status */ 
        status = STATUS_STORE_Refused_OutOfResources;
        DB_destroyHandle(&dbhandle);
        dbhandle = NULL;
      } 
      else 
      {
        if (DB_NORMAL != DB_makeNewStoreFileName(dbhandle,
            request->AffectedSOPClassUID,
            request->AffectedSOPInstanceUID,
            imageFileName))
        {
            CERR << "storeSCP: Database: DB_makeNewStoreFileName Failed" << endl;
            /* must still receive data */
            strcpy(imageFileName, NULL_DEVICE_NAME); 
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
    StoreContext context(dbhandle, status, imageFileName, &dcmff);    
    
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

    if (!SUCCESS(cond) || (context.status != STATUS_Success))
    {
        /* remove file */
        if (strcpy(imageFileName, NULL_DEVICE_NAME) != 0) 
        {
          if (opt_verbose) CERR << "Store SCP: Deleting Image File: " << imageFileName << endl;
          unlink(imageFileName);
        }
        if (dbhandle) DB_pruneInvalidRecords(dbhandle);
    }

#ifdef LOCK_IMAGE_FILES
    /* unlock image file */
    dcmtk_flock(lockfd, LOCK_UN);
    close(lockfd);
#endif
    
    /* free DB handle */
    if (dbhandle) DB_destroyHandle(&dbhandle);

    if (messageClient)
    {
      ostrstream out;
      Uint32 operationStatus = DVPSIPCMessage::statusError;
      if (SUCCESS(cond))
      {
        if (context.status == STATUS_Success) operationStatus = DVPSIPCMessage::statusOK; 
        else operationStatus = DVPSIPCMessage::statusWarning;        
      }
      const char *sopClassName = dcmFindNameOfUID(request->AffectedSOPClassUID);
      const char *successName = "failed";
      if (operationStatus == DVPSIPCMessage::statusOK) successName = "successful";
      if (sopClassName==NULL) sopClassName = request->AffectedSOPClassUID;

      out << "DICOM C-STORE receipt " << successName << ": " << endl
          << "\tSOP class UID          : " << sopClassName << endl
          << "\tSOP instance UID       : " << request->AffectedSOPInstanceUID << endl;

      if (operationStatus == DVPSIPCMessage::statusOK)
      {
        unsigned long fileSize = 0;
        struct stat fileStat;      
        if (0 == stat(imageFileName, &fileStat)) fileSize = fileStat.st_size;      
        out
          << "\tTarget file path       : " << imageFileName << endl
          << "\tTarget file size (kB)  : " << (fileSize+1023)/1024 << endl;
      }      

      out << "\tDIMSE presentation ctx : " << (int)presId << endl
          << "\tDIMSE message ID       : " << request->MessageID << endl
          << "\tDIMSE status           : " << DU_cstoreStatusString(context.status) << endl
          << ends;
      char *theString = out.str();
      messageClient->notifyReceivedDICOMObject(operationStatus, theString);
      delete[] theString;
    }

    return cond;
}

static void handleClient(T_ASC_Association **assoc, const char *dbfolder, OFBool opt_verbose, OFBool opt_bitpreserving, OFBool useTLS)
{
  CONDITION cond = ASC_acknowledgeAssociation(*assoc);
  if (! errorCond(cond, "Cannot acknowledge association:"))
  {
    if (opt_verbose)
    {
      CERR << "Association Acknowledged (Max Send PDV: " << (*assoc)->sendPDVLength << ")" << endl;
      if (ASC_countAcceptedPresentationContexts((*assoc)->params) == 0) CERR << "    (but no valid presentation contexts)" << endl;
    }

    if (messageClient)
    {
      // notify about successfully negotiated association
      ostrstream out;
      out << "DIMSE Association Acknowledged:" << endl
          << "  calling presentation address: " << (*assoc)->params->DULparams.callingPresentationAddress << endl
          << "  calling AE title: " << (*assoc)->params->DULparams.callingAPTitle << endl
          << "  called AE title: " << (*assoc)->params->DULparams.calledAPTitle << endl
          << "  max send PDV: " << (*assoc)->sendPDVLength << endl
          << "  presentation contexts: " << ASC_countAcceptedPresentationContexts((*assoc)->params) << endl;
      ASC_dumpConnectionParameters(*assoc, out);
      out << ends;
      char *theString = out.str();              
      if (useTLS) 
        messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusOK, theString);
        else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusOK, theString);
      delete[] theString;
    }

    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presID;
    cond = DIMSE_NORMAL;
    
    /* do real work */
    while (cond == DIMSE_NORMAL)
    {
      cond = DIMSE_receiveCommand(*assoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL);
      /* did peer release, abort, or do we have a valid message ? */
      
      switch (cond)
      {
        case DIMSE_NORMAL:
          /* process command */
          switch (msg.CommandField)
          {
            case DIMSE_C_ECHO_RQ:
              cond = echoSCP(*assoc, &msg.msg.CEchoRQ, presID, opt_verbose);
              break;
            case DIMSE_C_STORE_RQ:
              cond = storeSCP(*assoc, &msg.msg.CStoreRQ, presID, dbfolder, opt_verbose, opt_bitpreserving);
              break;
            default:
              cond = DIMSE_BADCOMMANDTYPE; /* unsupported command */
              CERR << "Cannot handle command: 0x" << hex << (unsigned)msg.CommandField << dec << endl;
              break;
          }
          break;
        case DIMSE_PEERREQUESTEDRELEASE:
        case DIMSE_PEERABORTEDASSOCIATION:
        default:
         /* finish processing loop */
          break;
      }
    } /* while */

    /* close association */
    if (cond == DIMSE_PEERREQUESTEDRELEASE)
    {
      COND_PopCondition(OFFalse);
      if (opt_verbose) CERR << "Association Release" << endl;
      cond = ASC_acknowledgeRelease(*assoc);
      errorCond(cond, "Cannot release association:");
      if (messageClient) messageClient->notifyConnectionClosed(DVPSIPCMessage::statusOK);
    } 
    else if (cond == DIMSE_PEERABORTEDASSOCIATION)
    {
      COND_PopCondition(OFFalse);       /* pop DIMSE abort */
      COND_PopCondition(OFFalse);       /* pop DUL abort */
      if (opt_verbose) CERR << "Association Aborted" << endl;
      if (messageClient) messageClient->notifyConnectionAborted(DVPSIPCMessage::statusWarning, "DIMSE association aborted by remote peer.");
    } 
    else
    {
      errorCond(cond, "DIMSE Failure (aborting association):");
      cond = ASC_abortAssociation(*assoc);
      errorCond(cond, "Cannot abort association:");
      if (messageClient) messageClient->notifyConnectionAborted(DVPSIPCMessage::statusError, "DIMSE failure, aborting association.");
    }
  }
  dropAssociation(assoc);
}

static void terminateAllReceivers(DVConfiguration& dvi, OFBool opt_verbose)
{

  if (opt_verbose) CERR << "Terminating all receivers" << endl;

  const char *recID=NULL;
  const char *recAETitle=NULL;
  unsigned short recPort=0;
  OFBool recUseTLS=OFFalse;
  T_ASC_Network *net=NULL;
  T_ASC_Parameters *params=NULL;
  DIC_NODENAME localHost;
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
  int keyFileFormat = SSL_FILETYPE_PEM;
  if (! dvi.getTLSPEMFormat()) keyFileFormat = SSL_FILETYPE_ASN1;
#endif  

  if (!SUCCESS(ASC_initializeNetwork(NET_REQUESTOR, 0, 1000, &net))) return;

  Uint32 numReceivers = dvi.getNumberOfTargets(DVPSE_receiver);
  for (Uint32 i=0; i<numReceivers; i++)
  {
    prepared = OFTrue;
    recID = dvi.getTargetID(i, DVPSE_receiver);
    recPort = dvi.getTargetPort(recID);
    recUseTLS = dvi.getTargetUseTLS(recID);
    recAETitle = dvi.getTargetAETitle(recID);
    if (opt_verbose) 
    {
      CERR << "Receiver " << recID << " on port " << recPort;
      if (recUseTLS) CERR << " with TLS" << endl; else CERR << endl;
    }

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
      OFString tlsCiphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
      Uint32 tlsNumberOfCiphersuites = dvi.getTargetNumberOfCipherSuites(recID);
      if (tlsNumberOfCiphersuites > 0)
      {
        tlsCiphersuites.clear();
        OFString currentSuite;
        const char *currentOpenSSL;
        for (Uint32 ui=0; ui<tlsNumberOfCiphersuites; ui++)
        {
          dvi.getTargetCipherSuite(recID, ui, currentSuite);
          if (NULL != (currentOpenSSL = DcmTLSTransportLayer::findOpenSSLCipherSuiteName(currentSuite.c_str())))
          {
            if (tlsCiphersuites.length() > 0) tlsCiphersuites += ":";
            tlsCiphersuites += currentOpenSSL;
          }
        }
      }
      DcmTLSTransportLayer *tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, tlsRandomSeedFile.c_str());
      if (tLayer)
      {    
        if (tlsCACertificateFolder) tLayer->addTrustedCertificateDir(tlsCACertificateFolder, keyFileFormat);
        if (tlsDHParametersFile.size() > 0) tLayer->setTempDHParameters(tlsDHParametersFile.c_str());
        tLayer->setPrivateKeyPasswd(tlsPrivateKeyPassword); // never prompt on console
        tLayer->setPrivateKeyFile(tlsPrivateKeyFile.c_str(), keyFileFormat);
        tLayer->setCertificateFile(tlsCertificateFile.c_str(), keyFileFormat);
        tLayer->setCipherSuites(tlsCiphersuites.c_str());    
        tLayer->setCertificateVerification(DCV_ignoreCertificate);
        ASC_setTransportLayer(net, tLayer, 1);
      }
#else
      prepared = OFFalse;
#endif
    } else {
      DcmTransportLayer *dLayer = new DcmTransportLayer(DICOM_APPLICATION_REQUESTOR);
      ASC_setTransportLayer(net, dLayer, 1);
    }
    if (prepared && recAETitle && (recPort > 0))
    {
      if (SUCCESS(ASC_createAssociationParameters(&params, DEFAULT_MAXPDU)))
      {
        ASC_setTransportLayerType(params, recUseTLS);
        ASC_setAPTitles(params, dvi.getNetworkAETitle(), recAETitle, NULL);
        gethostname(localHost, sizeof(localHost) - 1);
        sprintf(peerHost, "%s:%d", "localhost", (int)recPort);
        ASC_setPresentationAddresses(params, localHost, peerHost);
        // we propose only the "shutdown" SOP class in implicit VR
        ASC_addPresentationContext(params, 1, UID_PrivateShutdownSOPClass, &xfer, 1);
        // request shutdown association, abort if some strange peer accepts it
        if (ASC_NORMAL == ASC_requestAssociation(net, params, &assoc)) ASC_abortAssociation(assoc);
        ASC_dropAssociation(assoc);
        ASC_destroyAssociation(&assoc);
      }
    }
  } /* for loop */

  ASC_dropNetwork(&net);      
#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif
  return;
}


// ********************************************

#define SHORTCOL 2
#define LONGCOL 9

int main(int argc, char *argv[])
{
        
#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif
        
    OFString str;
    int         opt_debugMode   = 0;                   /* default: no debug */
    int         opt_verbose     = 0;                   /* default: not verbose */
    int         opt_terminate   = 0;                   /* default: no terminate mode */
    const char *opt_cfgName     = NULL;                /* config file name */
    const char *opt_cfgID       = NULL;                /* name of entry in config file */

    SetDebugLevel(( 0 ));
    dcmDisableGethostbyaddr.set(OFTrue);               // disable hostname lookup

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Network receive for presentation state viewer", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  
    cmd.addParam("config-file",  "configuration file to be read");
    cmd.addParam("receiver-id",  "identifier of receiver in config file", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                      "-h",        "print this help text and exit");
     cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
     cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");
     cmd.addOption("--terminate",                 "-t",        "terminate all running receivers");
 
    /* evaluate command line */                           
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      cmd.getParam(1, opt_cfgName);
      if (cmd.getParamCount() >= 2) cmd.getParam(2, opt_cfgID);
      if (cmd.findOption("--verbose")) opt_verbose = 1;
      if (cmd.findOption("--debug")) opt_debugMode = 3;
      if (cmd.findOption("--terminate")) opt_terminate = 1;
    }
   
    if ((opt_cfgID == 0)&&(! opt_terminate))
    {
        CERR << "error: paramter receiver-id required unless --terminate is specified." << endl;
        return 10;
    }
  
    if (opt_verbose)
    {
      CERR << rcsid << endl << endl;
    }
    
    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        CERR << "error: can't open configuration file '" << opt_cfgName << "'" << endl;
        return 10;
      }
    } else {
        CERR << "error: missing configuration file name" << endl;
        return 10;
    }

    SetDebugLevel((opt_debugMode));

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        CERR << "Warning: no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }
    
    DVInterface dvi(opt_cfgName);
    if (opt_terminate)
    {
      terminateAllReceivers(dvi, opt_verbose);
      return 0;  // application terminates here
    }

    /* get network configuration from configuration file */
    OFBool networkImplicitVROnly  = dvi.getTargetImplicitOnly(opt_cfgID);
    OFBool networkBitPreserving   = dvi.getTargetBitPreservingMode(opt_cfgID);
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
    int keyFileFormat = SSL_FILETYPE_PEM;
    if (! dvi.getTLSPEMFormat()) keyFileFormat = SSL_FILETYPE_ASN1;

    /* ciphersuites */
    OFString tlsCiphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
    Uint32 tlsNumberOfCiphersuites = dvi.getTargetNumberOfCipherSuites(opt_cfgID);
    if (tlsNumberOfCiphersuites > 0)
    {
      tlsCiphersuites.clear();
      OFString currentSuite;
      const char *currentOpenSSL;
      for (Uint32 ui=0; ui<tlsNumberOfCiphersuites; ui++)
      {
      	dvi.getTargetCipherSuite(opt_cfgID, ui, currentSuite);
        if (NULL == (currentOpenSSL = DcmTLSTransportLayer::findOpenSSLCipherSuiteName(currentSuite.c_str())))
        {
          cerr << "ciphersuite '" << currentSuite << "' is unknown. Known ciphersuites are:" << endl;
          unsigned long numSuites = DcmTLSTransportLayer::getNumberOfCipherSuites();
          for (unsigned long cs=0; cs < numSuites; cs++)
          {
            cerr << "    " << DcmTLSTransportLayer::getTLSCipherSuiteName(cs) << endl;
          }
          return 1;
        } else {
          if (tlsCiphersuites.length() > 0) tlsCiphersuites += ":";
          tlsCiphersuites += currentOpenSSL;
        }
      }
    }
#else
    if (useTLS)
    {
        CERR << "error: not compiled with OpenSSL, cannot use TLS." << endl;
        return 10;
    }    
#endif
    
    if (networkAETitle==NULL)
    {
        CERR << "error: no application entity title" << endl;
        return 10;
    }

    if (networkPort==0)
    {
        CERR << "error: no or invalid port number" << endl;
        return 10;
    }

#ifdef HAVE_GETEUID
    /* if port is privileged we must be as well */
    if ((networkPort < 1024)&&(geteuid() != 0))
    {
        CERR << "error: cannot listen on port " << networkPort << ", insufficient privileges" << endl;
        return 10;
    }
#endif

    if (networkMaxPDU==0) networkMaxPDU = DEFAULT_MAXPDU;
    else if (networkMaxPDU > ASC_MAXIMUMPDUSIZE)
    {
        CERR << "warning: max PDU size " << networkMaxPDU << " too big, using default: " << DEFAULT_MAXPDU << endl;
        networkMaxPDU = DEFAULT_MAXPDU;
    }
    
    if (networkDisableNewVRs)
    {
      dcmEnableUnknownVRGeneration.set(OFFalse);
      dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
    }

    ostrstream verboseParameters;
        
    OFBool comma=OFFalse;
    verboseParameters << "Network parameters:" << endl
         << "  port            : " << networkPort << endl
         << "  aetitle         : " << networkAETitle << endl
         << "  max pdu         : " << networkMaxPDU << endl
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
    verboseParameters << endl;
    verboseParameters << "  TLS             : ";
    if (useTLS) verboseParameters << "enabled" << endl; else verboseParameters << "disabled" << endl;

#ifdef WITH_OPENSSL
    if (useTLS)
    {
      verboseParameters << "  TLS certificate : " << tlsCertificateFile << endl
           << "  TLS key file    : " << tlsPrivateKeyFile << endl
           << "  TLS DH params   : " << tlsDHParametersFile << endl
           << "  TLS PRNG seed   : " << tlsRandomSeedFile << endl
           << "  TLS CA directory: " << tlsCACertificateFolder << endl
           << "  TLS ciphersuites: " << tlsCiphersuites << endl
           << "  TLS key format  : ";
      if (keyFileFormat == SSL_FILETYPE_PEM) verboseParameters << "PEM" << endl; else verboseParameters << "DER" << endl;
      verboseParameters << "  TLS cert verify : ";
      switch (tlsCertVerification)
      {
          case DCV_checkCertificate:
            verboseParameters << "verify" << endl;
            break;
          case DCV_ignoreCertificate:
            verboseParameters << "ignore" << endl;
            break;
          default:
            verboseParameters << "require" << endl;
            break;
      }
    }
#endif

    verboseParameters << ends;
    char *verboseParametersString = verboseParameters.str();
    if (opt_verbose) CERR << verboseParametersString << endl;
    
    /* check if we can get access to the database */
    const char *dbfolder = dvi.getDatabaseFolder();
    DB_Handle *dbhandle = NULL; 

    if (opt_verbose)
    {
      CERR << "Using database in directory '" << dbfolder << "'" << endl;
    }
    if (DB_NORMAL != DB_createHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, &dbhandle))
    {
      CERR << "Unable to access database '" << dbfolder << "'" << endl;
      COND_DumpConditions();
      return 1;
    }
    DB_destroyHandle(&dbhandle);
    
    T_ASC_Network *net = NULL; /* the DICOM network and listen port */
    T_ASC_Association *assoc = NULL; /* the DICOM association */
    OFBool finished1 = OFFalse;
    OFBool finished2 = OFFalse;
    int connected = 0;
    CONDITION cond = ASC_NORMAL;

#ifdef WITH_OPENSSL

    DcmTLSTransportLayer *tLayer = NULL;
    if (useTLS)
    {    
      tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_ACCEPTOR, tlsRandomSeedFile.c_str());
      if (tLayer == NULL)
      {
        app.printError("unable to create TLS transport layer");
      }

      if (tlsCACertificateFolder && (TCS_ok != tLayer->addTrustedCertificateDir(tlsCACertificateFolder, keyFileFormat)))
      {
        cerr << "warning unable to load certificates from directory '" << tlsCACertificateFolder << "', ignoring" << endl;
      }
      if ((tlsDHParametersFile.size() > 0) && ! (tLayer->setTempDHParameters(tlsDHParametersFile.c_str())))
      {
        cerr << "warning unable to load temporary DH parameter file '" << tlsDHParametersFile << "', ignoring" << endl;
      }
      tLayer->setPrivateKeyPasswd(tlsPrivateKeyPassword); // never prompt on console

      if (TCS_ok != tLayer->setPrivateKeyFile(tlsPrivateKeyFile.c_str(), keyFileFormat))
      {
        cerr << "unable to load private TLS key from '" << tlsPrivateKeyFile<< "'" << endl;
        return 1;
      }
      if (TCS_ok != tLayer->setCertificateFile(tlsCertificateFile.c_str(), keyFileFormat))
      {
        cerr << "unable to load certificate from '" << tlsCertificateFile << "'" << endl;
        return 1;
      }
      if (! tLayer->checkPrivateKeyMatchesCertificate())
      {
        cerr << "private key '" << tlsPrivateKeyFile << "' and certificate '" << tlsCertificateFile << "' do not match" << endl;
        return 1;
      }      
      if (TCS_ok != tLayer->setCipherSuites(tlsCiphersuites.c_str()))
      {
        cerr << "unable to set selected cipher suites" << endl;
        return 1;
      }

      tLayer->setCertificateVerification(tlsCertVerification);
        
    }

#endif
    
    while (!finished1)
    {
      /* open listen socket */
      cond = ASC_initializeNetwork(NET_ACCEPTOR, networkPort, 10, &net);
      if (errorCond(cond, "Error initialising network:")) 
      {
      	return 1;
      }

#ifdef WITH_OPENSSL
      if (tLayer)
      {
        cond = ASC_setTransportLayer(net, tLayer, 0);
        if (!SUCCESS(cond))
        {
	    COND_DumpConditions();
	    return 1;
        }
      }
#endif

#if defined(HAVE_SETUID) && defined(HAVE_GETUID)
      /* return to normal uid so that we can't do too much damage in case
       * things go very wrong.   Only relevant if the program is setuid root,
       * and run by another user.  Running as root user may be
       * potentially disasterous if this program screws up badly.
       */
      setuid(getuid());
#endif

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
            CERR << "Warning: no IPC message server found at port " << messagePort << ", disabling IPC." << endl;
          }
        }
        connected = 0;
        while (!connected)
        {
           connected = ASC_associationWaiting(net, timeout);
           if (!connected) cleanChildren();
        }
        switch (negotiateAssociation(net, &assoc, networkAETitle, networkMaxPDU, networkImplicitVROnly, opt_verbose, useTLS))
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
              CERR << "Cannot create association sub-process: " << strerror(errno) << endl;
              refuseAssociation(assoc, ref_CannotFork);

              if (messageClient)
              {
                // notify about rejected association
                ostrstream out;
                out << "DIMSE Association Rejected:" << endl
                    << "  reason: cannot create association sub-process: " << strerror(errno) << endl
                    << "  calling presentation address: " << assoc->params->DULparams.callingPresentationAddress << endl
                    << "  calling AE title: " << assoc->params->DULparams.callingAPTitle << endl
                    << "  called AE title: " << assoc->params->DULparams.calledAPTitle << endl;
                ASC_dumpConnectionParameters(assoc, out);
                out << ends;
                char *theString = out.str();              
                if (useTLS) 
                  messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
                  else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
                delete[] theString;
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
              dcmGenerateUniqueIdentifer(randomUID);              
              if (tLayer) tLayer->addPRNGseed(randomUID, strlen(randomUID));
#endif
              handleClient(&assoc, dbfolder, opt_verbose, networkBitPreserving, useTLS);
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
              dcmGenerateUniqueIdentifer(randomUID);              
              if (tLayer) tLayer->addPRNGseed(randomUID, strlen(randomUID));
#endif
              handleClient(&assoc, dbfolder, opt_verbose, networkBitPreserving, useTLS);
              finished1=OFTrue;              
            } else {
              CERR << "Cannot execute command line: " << commandline << endl;
              refuseAssociation(assoc, ref_CannotFork);

              if (messageClient)
              {
                // notify about rejected association
                ostrstream out;
                out << "DIMSE Association Rejected:" << endl
                    << "  reason: cannot execute command line: " << commandline << endl
                    << "  calling presentation address: " << assoc->params->DULparams.callingPresentationAddress << endl
                    << "  calling AE title: " << assoc->params->DULparams.callingAPTitle << endl
                    << "  called AE title: " << assoc->params->DULparams.calledAPTitle << endl;
                ASC_dumpConnectionParameters(assoc, out);
                out << ends;
                char *theString = out.str();              
                if (useTLS) 
                  messageClient->notifyReceivedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
                  else messageClient->notifyReceivedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
                delete[] theString;
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
   
#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

#ifdef WITH_OPENSSL
    if (tLayer)
    {
      if (tLayer->canWriteRandomSeed())
      {
        if (!tLayer->writeRandomSeed(tlsRandomSeedFile.c_str()))
        {
  	  cerr << "Error while writing back random seed file '" << tlsRandomSeedFile << "', ignoring." << endl;
        }
      } else {
	cerr << "Warning: cannot write back random seed, ignoring." << endl;
      }
    }
    delete tLayer;
#endif

    delete[] verboseParametersString;
    verboseParametersString = NULL;

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return 0;    
}


/*
 * CVS/RCS Log:
 * $Log: dcmpsrcv.cc,v $
 * Revision 1.29  2001-06-01 15:50:09  meichel
 * Updated copyright header
 *
 * Revision 1.28  2001/06/01 11:02:06  meichel
 * Implemented global flag and command line option to disable reverse
 *   DNS hostname lookup using gethostbyaddr when accepting associations.
 *
 * Revision 1.27  2000/11/14 13:24:34  meichel
 * Fixed two problems with dcmpsrcv which caused the application not to
 *   terminate if the IPC server could not be found or not to start another
 *   receiver when run on Win32 platforms.
 *
 * Revision 1.26  2000/11/13 14:20:31  joergr
 * Added missing #include.
 *
 * Revision 1.25  2000/11/10 16:21:13  meichel
 * Fixed problem with DICOMscope being unable to shut down receiver processes
 *   that are operating with TLS encryption by adding a special shutdown mode to
 *   dcmpsrcv.
 *
 * Revision 1.24  2000/11/08 18:38:03  meichel
 * Updated dcmpstat IPC protocol for additional message parameters
 *
 * Revision 1.23  2000/10/23 12:19:15  joergr
 * Added missing parameter to call of function handleClient (only appeared
 * on systems not supporting 'fork' command).
 *
 * Revision 1.22  2000/10/16 12:33:53  joergr
 * Moved incorrectly placed #endif statement to correct position.
 *
 * Revision 1.20  2000/10/10 12:23:45  meichel
 * Added extensions for TLS encrypted communication
 *
 * Revision 1.19  2000/05/31 13:02:24  meichel
 * Moved dcmpstat macros and constants into a common header file
 *
 * Revision 1.18  2000/05/30 14:03:29  joergr
 * Added new private SOP class (UID definition) to allow external shutdown
 * of console applications via negotiation of this special SOP class
 * (currently used for imagectn and dcmpsrcv).
 *
 * Revision 1.17  2000/04/14 16:34:37  meichel
 * Global VR generation flags are now derived from OFGlobal and, thus,
 *   safe for use in multi-thread applications.
 *
 * Revision 1.16  2000/03/08 16:28:43  meichel
 * Updated copyright header.
 *
 * Revision 1.15  2000/03/06 18:21:46  joergr
 * Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
 *
 * Revision 1.14  2000/03/03 14:13:27  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.13  2000/02/29 12:13:43  meichel
 * Removed support for VS value representation. This was proposed in CP 101
 *   but never became part of the standard.
 *
 * Revision 1.12  2000/02/02 15:18:00  meichel
 * Replaced some #if statements by more robust #ifdef
 *
 * Revision 1.11  1999/11/24 10:21:55  meichel
 * Fixed locking problem in dcmpssnd and dcmpsrcv on Win9x platforms.
 *
 * Revision 1.10  1999/09/06 13:29:48  meichel
 * Enhanced max receive PDU range to 4-128K.
 *
 * Revision 1.9  1999/05/05 14:23:56  joergr
 * Modified parameter of CreateProcess call to avoid creation of new command
 * line window under Windows.
 *
 * Revision 1.8  1999/05/03 14:16:38  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.7  1999/04/30 16:36:56  meichel
 * Renamed all flock calls to dcmtk_flock to avoid name clash between flock()
 * emulation based on fcntl() and a constructor for struct flock.
 *
 * Revision 1.6  1999/04/28 15:45:07  meichel
 * Cleaned up module dcmpstat apps, adapted to new command line class
 *   and added short documentation.
 *
 * Revision 1.5  1999/02/25 18:34:54  joergr
 * Added debug code (explicitly delete data dictionary).
 *
 * Revision 1.4  1999/02/09 14:33:01  meichel
 * Changed some bool consts from true to OFTrue.
 *
 * Revision 1.3  1999/02/08 12:52:17  meichel
 * Removed dummy parameter from DVInterface constructor.
 *
 * Revision 1.2  1999/01/27 15:58:57  meichel
 * Corrected locking behaviour of dcmpsrcv on Win32 platforms.
 *
 * Revision 1.1  1999/01/27 14:59:24  meichel
 * Implemented DICOM network receive application "dcmpsrcv" which receives
 *   images and presentation states and stores them in the local database.
 *
 *
 */
