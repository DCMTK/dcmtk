/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Update Date:      $Date: 1999-01-27 15:58:57 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmpsrcv.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
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

#include "cmdlnarg.h"
#include "ofcmdln.h"
#include "dviface.h"
#include "ofbmanip.h" /* for OFBitmanipTemplate */
#include "dcuid.h"    /* for dcmtk version name */
#include "diutil.h"

static char rcsid[] = "$dcmtk: dcmpsrcv v" OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default Max PDU size to be used when no different value is defined in the configuration file */
#define DEFAULT_MAXPDU 16384

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

static void
printHeader()
{
    cerr << "dcmpsrcv: network receive for presentation state viewer" << endl;
}

static void
printUsage(const OFCommandLine &cmd)
{
    OFString str;
    cmd.getOptionString(str);
    printHeader();
    cerr << "usage: dcmpsrcv [options] config-file" << endl;
    cerr << "options are:" << endl << endl;
    cerr << str << endl;
    exit(0);
}

static void
printError(const OFString &str)
{
    printHeader();
    cerr << "error: " << str << endl;
    exit(1);
}

static int errorCond(CONDITION cond, const char *message)
{
  int result = (!SUCCESS(cond));
  if (result) 
  {  
    cerr << message << endl;
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
           if (errno != ECHILD) cerr << "wait for child failed: " << strerror(errno) << endl;
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
  OFBool opt_verbose)
{
    associationType result = assoc_success;
    char buf[BUFSIZ];    
    OFBool dropAssoc = OFFalse;
    
    CONDITION cond = ASC_receiveAssociation(net, assoc, maxPDU);
    if (errorCond(cond, "Failed to receive association:")) 
    {
      dropAssoc = OFTrue;
      result = assoc_error;
    }
    else 
    {

      if (opt_verbose)
      {
          time_t t = time(NULL);
          cerr << "Association Received (" << (*assoc)->params->DULparams.callingPresentationAddress
             << ":" << (*assoc)->params->DULparams.callingAPTitle << " -> " 
             << (*assoc)->params->DULparams.calledAPTitle
             << ") " << ctime(&t);
      }

      ASC_setAPTitles((*assoc)->params, NULL, NULL, aetitle);
      
      /* Application Context Name */
      if ((cond = ASC_getApplicationContextName((*assoc)->params, buf) != ASC_NORMAL) || strcmp(buf, DICOM_STDAPPLICATIONCONTEXT) != 0)
      {
          /* reject: the application context name is not supported */
          if (opt_verbose) cerr << "Bad AppContextName: " << buf << endl;
          cond = refuseAssociation(*assoc, ref_BadAppContext);
          dropAssoc = OFTrue;
          result = assoc_error;
      } else {
        
        const char *nonStorageSyntaxes[] =
        {
          UID_VerificationSOPClass,
          PSTAT_PRIVATESOPCLASSUID
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
      if (0 != ASC_findAcceptedPresentationContextID(*assoc, PSTAT_PRIVATESOPCLASSUID)) 
      {
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
  
  StoreContext(DB_Handle *handle, DIC_US stat, const char *fname, DcmFileFormat *ff)
  : dbHandle(handle), status(stat), statusDetail(NULL), fileName(fname), dcmff(ff)
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
        cerr << "Cannot open file: " << fname << endl;
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
      cerr << "Bad image file: " << fname << endl;
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
        cerr << "Grayscale softcopy presentation state object cannot be displayed - rejected." << endl;
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
        cerr << "storeSCP: Database: DB_storeRequest Failed (" 
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
          cerr << "Cannot write image file: " << context->fileName << endl;
          rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
        } 
        else
        {
          context->dcmff->transferInit();
          context->dcmff->write(outf, EXS_LittleEndianExplicit, EET_ExplicitLength, EGL_recalcGL);
          context->dcmff->transferEnd();
          if (context->dcmff->error() != EC_Normal)
          {
            cerr << "Cannot write image file: " << context->fileName << endl;
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
    if (opt_verbose) cerr << "Received Echo SCP RQ: MsgID " << req->MessageID << endl;
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
        strcpy(imageFileName, "/dev/null");
    } 
    else
    {
      if (DB_NORMAL != DB_createHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, &dbhandle))
      {
        cerr << "Unable to access database '" << dbfolder << "'" << endl;
        COND_DumpConditions();
        /* must still receive data */
        strcpy(imageFileName, "/dev/null");     
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
            cerr << "storeSCP: Database: DB_makeNewStoreFileName Failed" << endl;
            /* must still receive data */
            strcpy(imageFileName, "/dev/null"); 
            /* callback will send back out of resources status */ 
            status = STATUS_STORE_Refused_OutOfResources;
        }
      }
    }

#ifdef HAVE_FORK
    /* exclusively lock image file, but only on Unix systems -
         * on Win32 we would prevent ourselves from writing the file!
         */
#ifdef O_BINARY
    int lockfd = open(imageFileName, (O_WRONLY | O_CREAT | O_TRUNC | O_BINARY), 0666);
#else
    int lockfd = open(imageFileName, (O_WRONLY | O_CREAT | O_TRUNC), 0666);
#endif

    flock(lockfd, LOCK_EX);
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
        if (opt_verbose) cerr << "Store SCP: Deleting Image File: " << imageFileName << endl;
        unlink(imageFileName);
        if (dbhandle) DB_pruneInvalidRecords(dbhandle);
    }

#ifdef HAVE_FORK
    /* unlock image file */
    flock(lockfd, LOCK_UN);
    close(lockfd);
#endif
    
    /* free DB handle */
    if (dbhandle) DB_destroyHandle(&dbhandle);
    
    return cond;
}

static void handleClient(T_ASC_Association **assoc, const char *dbfolder, OFBool opt_verbose, OFBool opt_bitpreserving)
{
  CONDITION cond = ASC_acknowledgeAssociation(*assoc);
  if (! errorCond(cond, "Cannot acknowledge association:"))
  {
    if (opt_verbose)
    {
      cerr << "Association Acknowledged (Max Send PDV: " << (*assoc)->sendPDVLength << ")" << endl;
      if (ASC_countAcceptedPresentationContexts((*assoc)->params) == 0) cerr << "    (but no valid presentation contexts)" << endl;
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
              cerr << "Cannot handle command: 0x" << hex << (unsigned)msg.CommandField << dec << endl;
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
      if (opt_verbose) cerr << "Association Release" << endl;
      cond = ASC_acknowledgeRelease(*assoc);
      errorCond(cond, "Cannot release association:");
    } 
    else if (cond == DIMSE_PEERABORTEDASSOCIATION)
    {
      COND_PopCondition(OFFalse);       /* pop DIMSE abort */
      COND_PopCondition(OFFalse);       /* pop DUL abort */
      if (opt_verbose) cerr << "Association Aborted" << endl;
    } 
    else
    {
      errorCond(cond, "DIMSE Failure (aborting association):");
      cond = ASC_abortAssociation(*assoc);
      errorCond(cond, "Cannot abort association:");
    }
  }
  dropAssociation(assoc);
}


// ********************************************

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
        
    OFCommandLine cmd;
    OFString str;

    int         opt_verbose     = 0;                   /* default: not verbose */
    const char *opt_cfgName     = NULL;                /* config file name */

    SetDebugLevel(( 0 ));
  
    prepareCmdLineArgs(argc, argv, "dcmp2pgm");
      
    cmd.addGroup("options:");
     cmd.addOption("--help",        "-h",    "print this help screen");
     cmd.addOption("--verbose",     "+V",    "verbose mode, print image details");
     
    switch (cmd.parseLine(argc, argv))    
    {
        case OFCommandLine::PS_NoArguments:
            printUsage(cmd);
            break;
        case OFCommandLine::PS_UnknownOption:
            cmd.getStatusString(OFCommandLine::PS_UnknownOption, str);
            printError(str);
            break;
        case OFCommandLine::PS_MissingValue:
            cmd.getStatusString(OFCommandLine::PS_MissingValue, str);
            printError(str);
            break;
        case OFCommandLine::PS_Normal:
            if ((cmd.getArgCount() == 1) && cmd.findOption("--help"))
                printUsage(cmd);
            else if (cmd.getParamCount() == 0)
                printError("Missing configuration file name");
            else if (cmd.getParamCount() > 1)
                printError("Too many arguments");
            else 
            {
                cmd.getParam(1, opt_cfgName);
                if (cmd.findOption("--verbose")) opt_verbose = 1;
            }
    }

    if (opt_verbose)
    {
      cerr << rcsid << endl << endl;
    }
    
    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        cerr << "error: can't open configuration file '" << opt_cfgName << "'" << endl;
        return 10;
      }
    } else {
        cerr << "error: missing configuration file name" << endl;
        return 10;
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        cerr << "Warning: no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
    }
    
    DVInterface dvi(0, opt_cfgName);

    /* get network configuration from configuration file */
    OFBool networkImplicitVROnly  = dvi.getNetworkImplicitVROnly();
    OFBool networkBitPreserving   = dvi.getNetworkBitPreserving();
    OFBool networkDisableNewVRs   = dvi.getNetworkDisableNewVRs();
    unsigned short networkPort    = dvi.getNetworkPort();
    unsigned long  networkMaxPDU  = dvi.getNetworkMaxPDU();
    const char *networkAETitle    = dvi.getNetworkAETitle();
    
    if (networkAETitle==NULL)
    {
        cerr << "error: no application entity title" << endl;
        return 10;
    }

    if (networkPort==0)
    {
        cerr << "error: no or invalid port number" << endl;
        return 10;
    }

#if HAVE_GETEUID
    /* if port is privileged we must be as well */
    if ((networkPort < 1024)&&(geteuid() != 0))
    {
        cerr << "error: cannot listen on port " << networkPort << ", insufficient privileges" << endl;
        return 10;
    }
#endif

    if (networkMaxPDU==0) networkMaxPDU = DEFAULT_MAXPDU;
    else if (networkMaxPDU > 65536)
    {
        cerr << "warning: max PDU size " << networkMaxPDU << " too big, using default: " << DEFAULT_MAXPDU << endl;
        networkMaxPDU = DEFAULT_MAXPDU;
    }
    
    if (networkDisableNewVRs)
    {
      dcmEnableUnknownVRGeneration = OFFalse;
      dcmEnableUnlimitedTextVRGeneration = OFFalse;
      dcmEnableVirtualStringVRGeneration = OFFalse;
    }
    
    if (opt_verbose)
    {
      OFBool comma=OFFalse;
      cerr << "Network parameters:" << endl
           << "  port       : " << networkPort << endl
           << "  aetitle    : " << networkAETitle << endl
           << "  max pdu    : " << networkMaxPDU << endl
           << "  options    : ";
      if (networkImplicitVROnly)
      {
        if (comma) cerr << ", "; else comma=OFTrue;
        cerr << "implicit xfer syntax only";
      }
      if (networkBitPreserving)
      {
        if (comma) cerr << ", "; else comma=OFTrue;
        cerr << "bit-preserving receive mode";
      }
      if (networkDisableNewVRs)
      {
        if (comma) cerr << ", "; else comma=OFTrue;
        cerr << "disable post-1993 VRs";
      }
      if (!comma) cerr << "none";
      cerr << endl << endl;
    }

    /* check if we can get access to the database */
    const char *dbfolder = dvi.getDatabaseFolder();
    DB_Handle *dbhandle = NULL; 

    if (opt_verbose)
    {
      cerr << "Using database in directory '" << dbfolder << "'" << endl;
    }
    if (DB_NORMAL != DB_createHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, &dbhandle))
    {
      cerr << "Unable to access database '" << dbfolder << "'" << endl;
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
    
    while (!finished1)
    {
      /* open listen socket */
      cond = ASC_initializeNetwork(NET_ACCEPTOR, networkPort, 10, &net);
      if (errorCond(cond, "Error initialising network:")) return 1;

#if HAVE_SETUID && HAVE_GETUID
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
        connected = 0;
        while (!connected)
        {
           connected = ASC_associationWaiting(net, timeout);
           if (!connected) cleanChildren();
        }
        switch (negotiateAssociation(net, &assoc, networkAETitle, networkMaxPDU, networkImplicitVROnly, opt_verbose))
        {
          case assoc_error:
            // association has already been deleted, we just wait for the next client to connect.
            break;
          case assoc_terminate:
            finished2=true;
            finished1=true;
            cond = ASC_dropNetwork(&net);
            if (errorCond(cond, "Error dropping network:")) return 1;
            break;
          case assoc_success:
#ifdef HAVE_FORK
            // Unix version - call fork()
            int pid = (int)(fork());
            if (pid < 0)
            {
              cerr << "Cannot create association sub-process: " << strerror(errno) << endl;
              refuseAssociation(assoc, ref_CannotFork);
              dropAssociation(&assoc);
            } else if (pid > 0)
            {
              /* parent process */
              assoc = NULL;
            } else {
              /* child process */
              handleClient(&assoc, dbfolder, opt_verbose, networkBitPreserving);
              finished2=true;
              finished1=true;
            }  
#else
            // Windows version - call CreateProcess()
            finished2=true;
            cond = ASC_dropNetwork(&net);
            if (errorCond(cond, "Error dropping network:")) return 1;

            // initialize startup info
            const char *receiver_application = dvi.getReceiverName();
            PROCESS_INFORMATION procinfo;
            STARTUPINFO sinfo;  
            OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
            sinfo.cb = sizeof(sinfo);
            char commandline[4096];
            sprintf(commandline, "%s %s", receiver_application, opt_cfgName);
            if (CreateProcess(NULL, commandline, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo))
            {
              handleClient(&assoc, dbfolder, opt_verbose, networkBitPreserving);
              finished1=true;              
            } else {
              cerr << "Cannot execute command line: " << commandline << endl;
              refuseAssociation(assoc, ref_CannotFork);
              dropAssociation(&assoc);
            }
#endif
            break;
        }
      } // finished2
    } // finished1
    cleanChildren();
    
#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

    return 0;    
}


/*
 * CVS/RCS Log:
 * $Log: dcmpsrcv.cc,v $
 * Revision 1.2  1999-01-27 15:58:57  meichel
 * Corrected locking behaviour of dcmpsrcv on Win32 platforms.
 *
 * Revision 1.1  1999/01/27 14:59:24  meichel
 * Implemented DICOM network receive application "dcmpsrcv" which receives
 *   images and presentation states and stores them in the local database.
 *
 *
 */
