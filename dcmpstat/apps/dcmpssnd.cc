/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *  Purpose: Presentation State Viewer - Network Send Component (Store SCU)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:28:43 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmpstat/apps/dcmpssnd.cc,v $
 *  CVS/RCS Revision: $Revision: 1.14 $
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

#include "dviface.h"
#include "ofbmanip.h" /* for OFBitmanipTemplate */
#include "dcuid.h"    /* for dcmtk version name */
#include "diutil.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dvpshlp.h"     /* for class DVPSHelper */
#include "imagedb.h"     /* for LOCK_IMAGE_FILES */

#define OFFIS_CONSOLE_APPLICATION "dcmpssnd"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default Max PDU size to be used when no different value is defined in the configuration file */
#define DEFAULT_MAXPDU 16384

/** sends a single DICOM instance over an association which must be already established.
 *  @param assoc DICOM network association
 *  @param sopClass SOP Class UID of the image (used for the C-Store-RQ)
 *  @param sopInstance SOP Instance UID of the image (used for the C-Store-RQ)
 *  @param imgFile path to the image file to be transmitted
 *  @param opt_verbose flag indicating whether "verbose mode" is active.
 *  @return DIMSE_NORMAL if successful, a different DIMSE code otherwise.
 */
static CONDITION sendImage(T_ASC_Association *assoc, const char *sopClass, const char *sopInstance, const char *imgFile, int opt_verbose)
{
    DcmDataset *statusDetail = NULL;
    T_ASC_PresentationContextID presId=0;
    T_DIMSE_C_StoreRQ req;
    T_DIMSE_C_StoreRSP rsp;
    
    if (assoc == NULL) return DIMSE_NULLKEY;
    if ((sopClass == NULL)||(strlen(sopClass) == 0)) return DIMSE_NULLKEY;
    if ((sopInstance == NULL)||(strlen(sopInstance) == 0)) return DIMSE_NULLKEY;
    if ((imgFile == NULL)||(strlen(imgFile) == 0)) return DIMSE_NULLKEY;
        
#ifdef LOCK_IMAGE_FILES
    /* shared lock image file */
#ifdef O_BINARY
    int lockfd = open(imgFile, O_RDONLY | O_BINARY, 0666);
#else
    int lockfd = open(imgFile, O_RDONLY, 0666);
#endif
    if (lockfd < 0)
    {
      if (opt_verbose) CERR << "error: unable to lock image file '" << imgFile << "'" << endl;
      return DIMSE_BADDATA;
    }
    dcmtk_flock(lockfd, LOCK_SH);
#endif

    /* which presentation context should be used */
    presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    if (presId == 0)
    {
      if (opt_verbose) CERR << "error: no presentation context for: (" << dcmSOPClassUIDToModality(sopClass) << ") " << sopClass << endl;
      return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    /* start store */
    OFBitmanipTemplate<char>::zeroMem((char *)&req, sizeof(req));
    req.MessageID = assoc->nextMsgID++;
    strcpy(req.AffectedSOPClassUID, sopClass);
    strcpy(req.AffectedSOPInstanceUID, sopInstance);
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_MEDIUM;

    CONDITION cond = DIMSE_storeUser(assoc, presId, &req,
        imgFile, NULL, NULL, NULL, DIMSE_BLOCKING, 0, &rsp, &statusDetail);

#ifdef LOCK_IMAGE_FILES
    /* unlock image file */
    dcmtk_flock(lockfd, LOCK_UN);
    close(lockfd);
#endif

    if (cond == DIMSE_NORMAL) 
    {
       if (opt_verbose) CERR << "[MsgID " << req.MessageID << "] Complete [Status: " 
          << DU_cstoreStatusString(rsp.DimseStatus) << "]" << endl;
    } else {
       if (opt_verbose) 
       { 
          CERR << "[MsgID " << req.MessageID << "] Failed [Status: " 
          << DU_cstoreStatusString(rsp.DimseStatus) << "]" << endl;
          COND_DumpConditions();
       }
       COND_PopCondition(OFTrue);
    }
    if (statusDetail) delete statusDetail;
    return cond;
}

/** sends a complete DICOM study, series or a single instance
 *  over an association which must be already established.
 *  The instances (files) to be transmitted are derived from the database.
 *  @param handle open database handle
 *  @param assoc DICOM network association
 *  @param studyUID Study Instance UID of the study/series/image to be transmitted.
 *  @param seriesUID Series Instance UID of the series/image to be transmitted.
 *    If NULL, a complete study is transmitted.
 *  @param instanceUID SOP Instance UID of the image to be transmitted.
 *    If NULL, a complete series is transmitted.
 *  @param opt_verbose flag indicating whether "verbose mode" is active.
 *  @return DIMSE_NORMAL if successful, a different DIMSE code otherwise.
 */

static CONDITION sendStudy(
  DB_Handle *handle,
  T_ASC_Association *assoc, 
  const char *studyUID,
  const char *seriesUID,
  const char *instanceUID,
  int opt_verbose)
{
    if ((handle==NULL)||(assoc==NULL)||(studyUID==NULL)) return DIMSE_NULLKEY;
    
    /* build query */
    DcmDataset query;
    if (EC_Normal != DVPSHelper::putStringValue(&query, DCM_StudyInstanceUID, studyUID)) return DIMSE_BUILDFAILED;
    if (seriesUID && instanceUID)
    {
      if (EC_Normal != DVPSHelper::putStringValue(&query, DCM_QueryRetrieveLevel, "IMAGE")) return DIMSE_BUILDFAILED;
      if (EC_Normal != DVPSHelper::putStringValue(&query, DCM_SeriesInstanceUID, seriesUID)) return DIMSE_BUILDFAILED;
      if (EC_Normal != DVPSHelper::putStringValue(&query, DCM_SOPInstanceUID, instanceUID)) return DIMSE_BUILDFAILED;
      if (opt_verbose)
      {
        CERR << "Sending at IMAGE level:" << endl
             << "  Study Instance UID : " << studyUID << endl
             << "  Series Instance UID: " << seriesUID << endl
             << "  SOP Instance UID   : " << instanceUID << endl << endl;            
      }   
    } 
    else if (seriesUID)
    {
      if (EC_Normal != DVPSHelper::putStringValue(&query, DCM_QueryRetrieveLevel, "SERIES")) return DIMSE_BUILDFAILED;
      if (EC_Normal != DVPSHelper::putStringValue(&query, DCM_SeriesInstanceUID, seriesUID)) return DIMSE_BUILDFAILED;
      if (opt_verbose)
      {
        CERR << "Sending at SERIES level:" << endl
             << "  Study Instance UID : " << studyUID << endl
             << "  Series Instance UID: " << seriesUID << endl << endl;    
      }   
    }
    else
    {
      if (EC_Normal != DVPSHelper::putStringValue(&query, DCM_QueryRetrieveLevel, "STUDY")) return DIMSE_BUILDFAILED;
      if (opt_verbose)
      {
        CERR << "Sending at STUDY level:" << endl
             << "  Study Instance UID : " << studyUID << endl << endl;  
      }   
    }

    DB_Status dbStatus;
    DIC_UI sopClass;
    DIC_UI sopInstance;
    char imgFile[MAXPATHLEN+1];
    DIC_US nRemaining = 0;

    dbStatus.status = STATUS_Pending;
    dbStatus.statusDetail = NULL;

    CONDITION cond = DB_startMoveRequest(handle, UID_MOVEStudyRootQueryRetrieveInformationModel, &query, &dbStatus);
    if (cond != DB_NORMAL) return cond;

    while (dbStatus.status == STATUS_Pending)
    {
      cond = DB_nextMoveResponse(handle, sopClass, sopInstance, imgFile, &nRemaining, &dbStatus);
      if (cond != DB_NORMAL) return cond;
        
      if (dbStatus.status == STATUS_Pending)
      {
        cond = sendImage(assoc, sopClass, sopInstance, imgFile, opt_verbose);
        if (cond != DIMSE_NORMAL)
        {
          DB_cancelMoveRequest(handle, &dbStatus);
          return cond;
        }
      }
    }
    return cond;
}

/** adds presentation contexts for all storage SOP classes
 *  to the association parameters.
 *  If the opt_implicitOnly flag is set, only Implicit VR Little Endian
 *  is offered as transfer syntax. Otherwise, three xfer syntaxes are offered:
 *  first the explicit VR with local byte ordering, followed by explicit VR
 *  with opposite byte ordering, followed by implicit VR little endian.
 *  @param params parameter set to which presentation contexts are added
 *  @param opt_implicitOnly flag defining whether only Implicit VR Little Endian
 *    should be offered as xfer syntax.
 *  @return ASC_NORMAL upon success, an error code otherwise.
 */
static CONDITION addAllStoragePresentationContexts(T_ASC_Parameters *params, int opt_implicitOnly)
{
    CONDITION cond = ASC_NORMAL;
    int pid = 1;

    const char* transferSyntaxes[3];
    int transferSyntaxCount = 0;

    if (opt_implicitOnly)
    {
	transferSyntaxes[0] = UID_LittleEndianImplicitTransferSyntax;
	transferSyntaxCount = 1;
    } else {
	/* gLocalByteOrder is defined in dcxfer.h */
	if (gLocalByteOrder == EBO_LittleEndian) {
	    /* we are on a little endian machine */
	    transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
	    transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
	} else {
	    /* we are on a big endian machine */
	    transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
	    transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
	}
	transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
	transferSyntaxCount = 3;
    }

    for (int i=0; i<numberOfDcmStorageSOPClassUIDs && SUCCESS(cond); i++) {
	cond = ASC_addPresentationContext(
	    params, pid, dcmStorageSOPClassUIDs[i],
	    transferSyntaxes, transferSyntaxCount);
	pid += 2;	/* only odd presentation context id's */
    }

    return cond;
}

// ********************************************

#define SHORTCOL 2
#define LONGCOL 10

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
    const char *opt_cfgName     = NULL;                /* config file name */
    const char *opt_target      = NULL;                /* send target name */
    const char *opt_studyUID    = NULL;                /* study instance UID */
    const char *opt_seriesUID   = NULL;                /* series instance UID */
    const char *opt_instanceUID = NULL;                /* instance instance UID */

    SetDebugLevel(( 0 ));

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Network send for presentation state viewer", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  
    cmd.addParam("config-file",  "configuration file to be read");
    cmd.addParam("target",       "symbolic identifier of send target in config file");
    cmd.addParam("study",        "study instance UID of study in database to be sent");
    cmd.addParam("series",       "series instance UID (default: send complete study)", OFCmdParam::PM_Optional);
    cmd.addParam("instance",     "SOP instance UID (default: send complete series)", OFCmdParam::PM_Optional);
  
    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
     cmd.addOption("--help",                      "-h",        "print this help text and exit");
     cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
     cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");
 
    /* evaluate command line */                           
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      cmd.getParam(1, opt_cfgName);
      cmd.getParam(2, opt_target);
      cmd.getParam(3, opt_studyUID);
      if (cmd.getParamCount() >= 4) cmd.getParam(4, opt_seriesUID);
      if (cmd.getParamCount() >= 5) cmd.getParam(5, opt_instanceUID);

      if (cmd.findOption("--verbose")) opt_verbose = 1;
      if (cmd.findOption("--debug")) opt_debugMode = 3;
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

    /* get send target from configuration file */
    const char *targetHostname    = dvi.getTargetHostname(opt_target);
    const char *targetDescription = dvi.getTargetDescription(opt_target);
    const char *targetAETitle     = dvi.getTargetAETitle(opt_target);
    unsigned short targetPort     = dvi.getTargetPort(opt_target);
    unsigned long  targetMaxPDU   = dvi.getTargetMaxPDU(opt_target);
    OFBool targetImplicitOnly     = dvi.getTargetImplicitOnly(opt_target);
    OFBool targetDisableNewVRs    = dvi.getTargetDisableNewVRs(opt_target);

    if (targetHostname==NULL)
    {
        CERR << "error: no hostname for send target '" << opt_target << "'" << endl;
        return 10;
    }

    if (targetAETitle==NULL)
    {
        CERR << "error: no aetitle for send target '" << opt_target << "'" << endl;
        return 10;
    }

    if (targetPort==0)
    {
        CERR << "error: no or invalid port number for send target '" << opt_target << "'" << endl;
        return 10;
    }

    if (targetMaxPDU==0) targetMaxPDU = DEFAULT_MAXPDU;
    else if (targetMaxPDU > ASC_MAXIMUMPDUSIZE)
    {
        CERR << "warning: max PDU size " << targetMaxPDU << " too big for send target '" 
             << opt_target << "', using default: " << DEFAULT_MAXPDU << endl;
        targetMaxPDU = DEFAULT_MAXPDU;
    }
    
    if (targetDisableNewVRs)
    {
      dcmEnableUnknownVRGeneration = OFFalse;
      dcmEnableUnlimitedTextVRGeneration = OFFalse;
    }
    
    if (opt_verbose)
    {
      CERR << "Send target parameters:" << endl
           << "  hostname   : " << targetHostname << endl
           << "  port       : " << targetPort << endl
           << "  description: ";
      if (targetDescription) CERR << targetDescription; else CERR << "(none)";
      CERR << endl
           << "  aetitle    : " << targetAETitle << endl
           << "  max pdu    : " << targetMaxPDU << endl
           << "  options    : ";
      if (targetImplicitOnly && targetDisableNewVRs) CERR << "implicit xfer syntax only, disable post-1993 VRs";
      else if (targetImplicitOnly) CERR << "implicit xfer syntax only";
      else if (targetDisableNewVRs) CERR << "disable post-1993 VRs";
      else CERR << "none.";
      CERR << endl << endl;
    }
    
    /* open database */
    const char *dbfolder = dvi.getDatabaseFolder();
    DB_Handle *dbhandle = NULL; 

    if (opt_verbose)
    {
      CERR << "Opening database in directory '" << dbfolder << "'" << endl;
    }
    if (DB_NORMAL != DB_createHandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, &dbhandle))
    {
      CERR << "Unable to access database '" << dbfolder << "'" << endl;
      COND_DumpConditions();
      return 1;
    }

    /* open network connection */
    T_ASC_Network *net=NULL;
    T_ASC_Parameters *params=NULL;
    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;
    T_ASC_Association *assoc=NULL;

    CONDITION cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 1000, &net);
    if (!SUCCESS(cond))
    {
      COND_DumpConditions();
      return 1;
    }
    cond = ASC_createAssociationParameters(&params, targetMaxPDU);
    if (!SUCCESS(cond))
    {
      COND_DumpConditions();
      return 1;
    }

    ASC_setAPTitles(params, dvi.getNetworkAETitle(), targetAETitle, NULL);

    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", targetHostname, (int)targetPort);
    ASC_setPresentationAddresses(params, localHost, peerHost);

    cond = addAllStoragePresentationContexts(params, targetImplicitOnly);
    if (!SUCCESS(cond))
    {
      COND_DumpConditions();
      return 1;
    }

    /* create association */
    if (opt_verbose) CERR << "Requesting Association" << endl;
    
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond != ASC_NORMAL)
    {
	if (cond == ASC_ASSOCIATIONREJECTED)
	{
	    T_ASC_RejectParameters rej;

	    ASC_getRejectParameters(params, &rej);
            CERR << "Association Rejected" << endl;
	    ASC_printRejectParameters(stderr, &rej);
	    return 1;
	} else {
            CERR << "Association Request Failed" << endl;
	    COND_DumpConditions();
	    return 1;
	}
    }
    
    if (ASC_countAcceptedPresentationContexts(params) == 0)
    {
      CERR << "No Acceptable Presentation Contexts" << endl;
      return 1;
    }

    if (opt_verbose) CERR << "Association accepted (Max Send PDV: " << assoc->sendPDVLength << ")" << endl;

    /* do the real work */
    cond = sendStudy(dbhandle, assoc, opt_studyUID, opt_seriesUID, opt_instanceUID, opt_verbose);

    /* tear down association */
    switch (cond)
    {
      case DIMSE_NORMAL:
      case DB_NORMAL:
	/* release association */
	if (opt_verbose) CERR << "Releasing Association" << endl;
	cond = ASC_releaseAssociation(assoc);
	if (cond != ASC_NORMAL && cond != ASC_RELEASECONFIRMED)
	{
          CERR << "Association Release Failed" << endl;
	  COND_DumpConditions();
          return 1;
	}
	break;
      case DIMSE_PEERREQUESTEDRELEASE:
        CERR << "Protocol Error: peer requested release (Aborting)" << endl;
	if (opt_verbose) CERR << "Aborting Association" << endl;
	cond = ASC_abortAssociation(assoc);
	if (!SUCCESS(cond))
	{
            CERR << "Association Abort Failed" << endl;
	    COND_DumpConditions();
            return 1;
	}
	break;
      case DIMSE_PEERABORTEDASSOCIATION:
	if (opt_verbose) CERR << "Peer Aborted Association" << endl;
	break;
      default:
        CERR << "SCU Failed" << endl;
	COND_DumpConditions();
	if (opt_verbose) CERR << "Aborting Association" << endl;
	cond = ASC_abortAssociation(assoc);
	if (!SUCCESS(cond))
	{
            CERR << "Association Abort Failed" << endl;
	    COND_DumpConditions();
            return 1;
	}
	break;
    }

    cond = ASC_destroyAssociation(&assoc);
    if (!SUCCESS(cond))
    {
      COND_DumpConditions();
      return 1;
    }
    cond = ASC_dropNetwork(&net);
    if (!SUCCESS(cond))
    {
      COND_DumpConditions();
      return 1;
    }

    /* clean up DB handle */
    DB_destroyHandle(&dbhandle);
    
#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return 0;    
}


/*
 * CVS/RCS Log:
 * $Log: dcmpssnd.cc,v $
 * Revision 1.14  2000-03-08 16:28:43  meichel
 * Updated copyright header.
 *
 * Revision 1.13  2000/03/06 18:21:47  joergr
 * Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
 *
 * Revision 1.12  2000/03/03 14:13:28  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.11  2000/02/29 12:13:44  meichel
 * Removed support for VS value representation. This was proposed in CP 101
 *   but never became part of the standard.
 *
 * Revision 1.10  2000/02/03 11:50:45  meichel
 * Moved UID related functions from dcmnet (diutil.h) to dcmdata (dcuid.h)
 *   where they belong. Renamed access functions to dcmSOPClassUIDToModality
 *   and dcmGuessModalityBytes.
 *
 * Revision 1.9  1999/11/24 10:21:56  meichel
 * Fixed locking problem in dcmpssnd and dcmpsrcv on Win9x platforms.
 *
 * Revision 1.8  1999/09/17 14:29:07  meichel
 * Moved static helper functions to new class DVPSHelper, removed some unused code.
 *
 * Revision 1.7  1999/09/06 13:29:48  meichel
 * Enhanced max receive PDU range to 4-128K.
 *
 * Revision 1.6  1999/04/30 16:36:56  meichel
 * Renamed all flock calls to dcmtk_flock to avoid name clash between flock()
 * emulation based on fcntl() and a constructor for struct flock.
 *
 * Revision 1.5  1999/04/28 15:45:09  meichel
 * Cleaned up module dcmpstat apps, adapted to new command line class
 *   and added short documentation.
 *
 * Revision 1.4  1999/02/25 18:34:25  joergr
 * Added debug code (explicitly delete data dictionary).
 *
 * Revision 1.3  1999/02/08 12:52:17  meichel
 * Removed dummy parameter from DVInterface constructor.
 *
 * Revision 1.2  1999/01/25 13:05:47  meichel
 * Implemented DVInterface::startReceiver()
 *   and several config file related methods.
 *
 * Revision 1.1  1999/01/20 19:26:17  meichel
 * Implemented DICOM network send application "dcmpssnd" which sends studies/
 *   series/images contained in the local database to a remote DICOM
 *   communication peer.
 *
 *
 */
