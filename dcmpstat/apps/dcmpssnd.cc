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
 *  Purpose: Presentation State Viewer - Network Send Component (Store SCU)
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
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmpstat/dvpshlp.h"     /* for class DVPSHelper */
#include "dcmtk/dcmqrdb/dcmqrdbi.h"     /* for LOCK_IMAGE_FILES */
#include "dcmtk/dcmqrdb/dcmqrdbs.h"     /* for DcmQueryRetrieveDatabaseStatus */
#include "dcmtk/dcmpstat/dvpsmsg.h"

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#include "dcmtk/ofstd/ofstream.h"

#define OFFIS_CONSOLE_APPLICATION "dcmpssnd"

static OFLogger dcmpssndLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

DVPSIPCClient *messageClient  = NULL; // global pointer to IPC message client, if present

/** sends a single DICOM instance over an association which must be already established.
 *  @param assoc DICOM network association
 *  @param sopClass SOP Class UID of the image (used for the C-Store-RQ)
 *  @param sopInstance SOP Instance UID of the image (used for the C-Store-RQ)
 *  @param imgFile path to the image file to be transmitted
 *  @return EC_Normal if successful, a different DIMSE code otherwise.
 */
static OFCondition sendImage(T_ASC_Association *assoc, const char *sopClass, const char *sopInstance, const char *imgFile)
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
      OFLOG_INFO(dcmpssndLogger, "unable to lock image file '" << imgFile << "'");
      return DIMSE_BADDATA;
    }
    dcmtk_flock(lockfd, LOCK_SH);
#endif

    /* which presentation context should be used */
    presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    if (presId == 0)
    {
      OFLOG_INFO(dcmpssndLogger, "no presentation context for: ("
        << dcmSOPClassUIDToModality(sopClass, "OT") << ") " << sopClass);
      if (messageClient)
      {
        OFString buf("unable to send image: no presentation context for ");
        const char *sopClassName = dcmFindNameOfUID(sopClass);
        if (sopClassName == NULL) buf.append(sopClass); else buf.append(sopClassName);
        buf.append("\n");
        messageClient->notifySentDICOMObject(DVPSIPCMessage::statusWarning, buf.c_str());
      }
      return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    /* start store */
    OFBitmanipTemplate<char>::zeroMem((char *)&req, sizeof(req));
    req.MessageID = assoc->nextMsgID++;

    OFStandard::strlcpy(req.AffectedSOPClassUID, sopClass, sizeof(req.AffectedSOPClassUID));
    OFStandard::strlcpy(req.AffectedSOPInstanceUID, sopInstance, sizeof(req.AffectedSOPInstanceUID));
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_MEDIUM;

    OFCondition cond = DIMSE_storeUser(assoc, presId, &req,
        imgFile, NULL, NULL, NULL, DIMSE_BLOCKING, 0, &rsp, &statusDetail);

#ifdef LOCK_IMAGE_FILES
    /* unlock image file */
    dcmtk_flock(lockfd, LOCK_UN);
    close(lockfd);
#endif

    if (cond.good())
    {
        OFLOG_INFO(dcmpssndLogger, "[MsgID " << req.MessageID << "] Complete [Status: "
           << DU_cstoreStatusString(rsp.DimseStatus) << "]");
    } else {
        OFString temp_str;
        OFLOG_INFO(dcmpssndLogger, "[MsgID " << req.MessageID << "] Failed [Status: "
           << DU_cstoreStatusString(rsp.DimseStatus) << "]\n" << DimseCondition::dump(temp_str, cond));
    }
    if (statusDetail) delete statusDetail;

    if (messageClient)
    {
      OFOStringStream out;
      Uint32 operationStatus = DVPSIPCMessage::statusError;
      if (cond.good())
      {
        if (rsp.DimseStatus == STATUS_Success) operationStatus = DVPSIPCMessage::statusOK;
        else operationStatus = DVPSIPCMessage::statusWarning;
      }
      const char *sopClassName = dcmFindNameOfUID(sopClass);
      const char *successName = "failed";
      if (operationStatus == DVPSIPCMessage::statusOK) successName = "successful";
      if (sopClassName==NULL) sopClassName = sopClass;
      unsigned long fileSize = 0;
      struct stat fileStat;
      if (0 == stat(imgFile, &fileStat)) fileSize = fileStat.st_size;
      out << "DICOM C-STORE transmission " << successName << ": " << OFendl
          << "\tSOP class UID          : " << sopClassName << OFendl
          << "\tSOP instance UID       : " << sopInstance << OFendl
          << "\tSource file path       : " << imgFile << OFendl
          << "\tSource file size (kB)  : " << (fileSize+1023)/1024 << OFendl
          << "\tDIMSE presentation ctx : " << (int)presId << OFendl
          << "\tDIMSE message ID       : " << req.MessageID << OFendl
          << "\tDIMSE status           : " << DU_cstoreStatusString(rsp.DimseStatus) << OFendl
          << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(out, theString)
      messageClient->notifySentDICOMObject(operationStatus, theString);
      OFSTRINGSTREAM_FREESTR(theString)
    }
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
 *  @return EC_Normal if successful, a different DIMSE code otherwise.
 */

static OFCondition sendStudy(
  DcmQueryRetrieveIndexDatabaseHandle &handle,
  T_ASC_Association *assoc,
  const char *studyUID,
  const char *seriesUID,
  const char *instanceUID)
{
    if ((assoc==NULL)||(studyUID==NULL)) return DIMSE_NULLKEY;

    /* build query */
    DcmDataset query;
    OFCondition cond = DVPSHelper::putStringValue(&query, DCM_StudyInstanceUID, studyUID);
    if (cond.bad()) return cond;
    if (seriesUID && instanceUID)
    {
      cond = DVPSHelper::putStringValue(&query, DCM_QueryRetrieveLevel, "IMAGE");
      if (cond.bad()) return cond;
      cond = DVPSHelper::putStringValue(&query, DCM_SeriesInstanceUID, seriesUID);
      if (cond.bad()) return cond;
      cond = DVPSHelper::putStringValue(&query, DCM_SOPInstanceUID, instanceUID);
      if (cond.bad()) return cond;

      OFLOG_INFO(dcmpssndLogger, "Sending at IMAGE level:" << OFendl
                              << "  Study Instance UID : " << studyUID << OFendl
                              << "  Series Instance UID: " << seriesUID << OFendl
                              << "  SOP Instance UID   : " << instanceUID);
    }
    else if (seriesUID)
    {
      cond = DVPSHelper::putStringValue(&query, DCM_QueryRetrieveLevel, "SERIES");
      if (cond.bad()) return cond;
      cond = DVPSHelper::putStringValue(&query, DCM_SeriesInstanceUID, seriesUID);
      if (cond.bad()) return cond;

      OFLOG_INFO(dcmpssndLogger, "Sending at SERIES level:" << OFendl
                              << "  Study Instance UID : " << studyUID << OFendl
                              << "  Series Instance UID: " << seriesUID);
    }
    else
    {
      cond = DVPSHelper::putStringValue(&query, DCM_QueryRetrieveLevel, "STUDY");
      if (cond.bad()) return cond;

      OFLOG_INFO(dcmpssndLogger, "Sending at STUDY level:" << OFendl
                              << "  Study Instance UID : " << studyUID);
    }

    DcmQueryRetrieveDatabaseStatus dbStatus(STATUS_Pending);
    DIC_UI sopClass;
    DIC_UI sopInstance;
    char imgFile[MAXPATHLEN+1];
    DIC_US nRemaining = 0;

    cond = handle.startMoveRequest(UID_MOVEStudyRootQueryRetrieveInformationModel, &query, &dbStatus);
    if (cond.bad()) return cond;

    while (dbStatus.status() == STATUS_Pending)
    {
      cond = handle.nextMoveResponse(sopClass, sizeof(sopClass), sopInstance, sizeof(sopInstance), imgFile, sizeof(imgFile), &nRemaining, &dbStatus);
      if (cond.bad()) return cond;

      if (dbStatus.status() == STATUS_Pending)
      {
        cond = sendImage(assoc, sopClass, sopInstance, imgFile);
        if (cond.bad())
        {
          handle.cancelMoveRequest(&dbStatus);
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
 *  @return EC_Normal upon success, an error code otherwise.
 */
static OFCondition addAllStoragePresentationContexts(T_ASC_Parameters *params, int opt_implicitOnly)
{
    OFCondition cond = EC_Normal;
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

    for (int i=0; i<numberOfDcmLongSCUStorageSOPClassUIDs && cond.good(); i++) {
        cond = ASC_addPresentationContext(
            params, pid, dcmLongSCUStorageSOPClassUIDs[i],
            transferSyntaxes, transferSyntaxCount);
        pid += 2;       /* only odd presentation context id's */
    }

    return cond;
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

    OFString temp_str;

    const char *opt_cfgName     = NULL;                /* config file name */
    const char *opt_target      = NULL;                /* send target name */
    const char *opt_studyUID    = NULL;                /* study instance UID */
    const char *opt_seriesUID   = NULL;                /* series instance UID */
    const char *opt_instanceUID = NULL;                /* instance instance UID */

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Network send for presentation state viewer", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 2);

    cmd.addParam("config-file", "configuration file to be read");
    cmd.addParam("target",      "symbolic identifier of send target in config file");
    cmd.addParam("study",       "study instance UID of study in database to be sent");
    cmd.addParam("series",      "series instance UID (default: send complete study)", OFCmdParam::PM_Optional);
    cmd.addParam("instance",    "SOP instance UID (default: send complete series)", OFCmdParam::PM_Optional);

    cmd.addGroup("general options:");
     cmd.addOption("--help",    "-h", "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",       "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

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

      /* command line parameters */
      cmd.getParam(1, opt_cfgName);
      cmd.getParam(2, opt_target);
      cmd.getParam(3, opt_studyUID);
      if (cmd.getParamCount() >= 4) cmd.getParam(4, opt_seriesUID);
      if (cmd.getParamCount() >= 5) cmd.getParam(5, opt_instanceUID);

      OFLog::configureFromCommandLine(cmd, app);
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmpssndLogger, rcsid << OFendl);

    if (opt_cfgName)
    {
      FILE *cfgfile = fopen(opt_cfgName, "rb");
      if (cfgfile) fclose(cfgfile); else
      {
        OFLOG_FATAL(dcmpssndLogger, "can't open configuration file '" << opt_cfgName << "'");
        return 10;
      }
    } else {
        OFLOG_FATAL(dcmpssndLogger, "missing configuration file name");
        return 10;
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(dcmpssndLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    DVConfiguration dvi(opt_cfgName);

    /* get send target from configuration file */
    const char *targetHostname    = dvi.getTargetHostname(opt_target);
    const char *targetDescription = dvi.getTargetDescription(opt_target);
    const char *targetAETitle     = dvi.getTargetAETitle(opt_target);
    unsigned short targetPort     = dvi.getTargetPort(opt_target);
    unsigned long  targetMaxPDU   = dvi.getTargetMaxPDU(opt_target);
    OFBool targetImplicitOnly     = dvi.getTargetImplicitOnly(opt_target);
    OFBool targetDisableNewVRs    = dvi.getTargetDisableNewVRs(opt_target);

    unsigned short messagePort    = dvi.getMessagePort();   /* port number for IPC */
    OFBool keepMessagePortOpen    = dvi.getMessagePortKeepOpen();
    OFBool useTLS = dvi.getTargetUseTLS(opt_target);

    Sint32 timeout = dvi.getTargetTimeout(opt_target);
    if (timeout > 0) dcmConnectionTimeout.set(timeout);

#ifdef WITH_OPENSSL
    /* TLS directory */
    const char *current = NULL;
    const char *tlsFolder = dvi.getTLSFolder();
    if (tlsFolder==NULL) tlsFolder = ".";

    /* certificate file */
    OFString tlsCertificateFile(tlsFolder);
    tlsCertificateFile += PATH_SEPARATOR;
    current = dvi.getTargetCertificate(opt_target);
    if (current) tlsCertificateFile += current; else tlsCertificateFile.clear();

    /* private key file */
    OFString tlsPrivateKeyFile(tlsFolder);
    tlsPrivateKeyFile += PATH_SEPARATOR;
    current = dvi.getTargetPrivateKey(opt_target);
    if (current) tlsPrivateKeyFile += current; else tlsPrivateKeyFile.clear();

    /* private key password */
    const char *tlsPrivateKeyPassword = dvi.getTargetPrivateKeyPassword(opt_target);

    /* certificate verification */
    DcmCertificateVerification tlsCertVerification = DCV_requireCertificate;
    switch (dvi.getTargetPeerAuthentication(opt_target))
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
    current = dvi.getTargetDiffieHellmanParameters(opt_target);
    if (current)
    {
      tlsDHParametersFile = tlsFolder;
      tlsDHParametersFile += PATH_SEPARATOR;
      tlsDHParametersFile += current;
    }

    /* random seed file */
    OFString tlsRandomSeedFile(tlsFolder);
    tlsRandomSeedFile += PATH_SEPARATOR;
    current = dvi.getTargetRandomSeed(opt_target);
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
        OFLOG_FATAL(dcmpssndLogger, "not compiled with OpenSSL, cannot use TLS");
        return 10;
    }
#endif

    if (targetHostname==NULL)
    {
        OFLOG_FATAL(dcmpssndLogger, "no hostname for send target '" << opt_target << "'");
        return 10;
    }

    if (targetAETitle==NULL)
    {
        OFLOG_FATAL(dcmpssndLogger, "no aetitle for send target '" << opt_target << "'");
        return 10;
    }

    if (targetPort==0)
    {
        OFLOG_FATAL(dcmpssndLogger, "no or invalid port number for send target '" << opt_target << "'");
        return 10;
    }

    if (targetMaxPDU==0) targetMaxPDU = DEFAULT_MAXPDU;
    else if (targetMaxPDU > ASC_MAXIMUMPDUSIZE)
    {
        OFLOG_WARN(dcmpssndLogger, "max PDU size " << targetMaxPDU << " too big for send target '"
            << opt_target << "', using default: " << DEFAULT_MAXPDU);
        targetMaxPDU = DEFAULT_MAXPDU;
    }

    if (targetDisableNewVRs)
    {
        dcmDisableGenerationOfNewVRs();
    }

    OFOStringStream verboseParameters;

    verboseParameters << "Send target parameters:" << OFendl
        << "\thostname        : " << targetHostname << OFendl
        << "\tport            : " << targetPort << OFendl
        << "\tdescription     : ";
    if (targetDescription) verboseParameters << targetDescription; else verboseParameters << "(none)";
    verboseParameters << OFendl
        << "\taetitle         : " << targetAETitle << OFendl
        << "\tmax pdu         : " << targetMaxPDU << OFendl
        << "\ttimeout         : " << timeout << OFendl
        << "\toptions         : ";
    if (targetImplicitOnly && targetDisableNewVRs) verboseParameters << "implicit xfer syntax only, disable post-1993 VRs";
    else if (targetImplicitOnly) verboseParameters << "implicit xfer syntax only";
    else if (targetDisableNewVRs) verboseParameters << "disable post-1993 VRs";
    else verboseParameters << "none";
    verboseParameters << OFendl;

    verboseParameters << "\tTLS             : ";
    if (useTLS) verboseParameters << "enabled" << OFendl; else verboseParameters << "disabled" << OFendl;

    /* open database */
    const char *dbfolder = dvi.getDatabaseFolder();

    OFLOG_INFO(dcmpssndLogger, "Opening database in directory '" << dbfolder << "'");

    OFCondition result;
    DcmQueryRetrieveIndexDatabaseHandle dbhandle(dbfolder, PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, result);
    if (result.bad())
    {
      OFLOG_FATAL(dcmpssndLogger, "Unable to access database '" << dbfolder << "'");
      return 1;
    }

#ifdef WITH_OPENSSL

    DcmTLSTransportLayer *tLayer = NULL;
    if (useTLS)
    {
      tLayer = new DcmTLSTransportLayer(NET_REQUESTOR, tlsRandomSeedFile.c_str(), OFFalse);
      if (tLayer == NULL)
      {
        OFLOG_FATAL(dcmpssndLogger, "unable to create TLS transport layer");
        return 1;
      }

      // determine TLS profile
      OFString profileName;
      const char *profileNamePtr = dvi.getTargetTLSProfile(opt_target);
      if (profileNamePtr) profileName = profileNamePtr;
      DcmTLSSecurityProfile tlsProfile = TSP_Profile_BCP195;  // default
      if (profileName == "BCP195") tlsProfile = TSP_Profile_BCP195;
      else if (profileName == "BCP195-ND") tlsProfile = TSP_Profile_BCP195_ND;
      else if (profileName == "BCP195-EX") tlsProfile = TSP_Profile_BCP195_Extended;
      else if (profileName == "AES") tlsProfile = TSP_Profile_AES;
      else if (profileName == "BASIC") tlsProfile = TSP_Profile_Basic;
      else if (profileName == "NULL") tlsProfile = TSP_Profile_IHE_ATNA_Unencrypted;
      else
      {
        OFLOG_WARN(dcmpssndLogger, "unknown TLS profile '" << profileName << "', ignoring");
      }

      if (TCS_ok != tLayer->setTLSProfile(tlsProfile))
      {
        OFLOG_FATAL(dcmpssndLogger, "unable to select the TLS security profile");
        return 1;
      }

      // activate cipher suites
      if (TCS_ok != tLayer->activateCipherSuites())
      {
        OFLOG_FATAL(dcmpssndLogger, "unable to activate the selected list of TLS ciphersuites");
        return 1;
      }

      if (tlsCACertificateFolder && (TCS_ok != tLayer->addTrustedCertificateDir(tlsCACertificateFolder, keyFileFormat)))
      {
        OFLOG_WARN(dcmpssndLogger, "unable to load certificates from directory '" << tlsCACertificateFolder << "', ignoring");
      }
      if ((tlsDHParametersFile.size() > 0) && ! (tLayer->setTempDHParameters(tlsDHParametersFile.c_str())))
      {
        OFLOG_WARN(dcmpssndLogger, "unable to load temporary DH parameter file '" << tlsDHParametersFile << "', ignoring");
      }
      tLayer->setPrivateKeyPasswd(tlsPrivateKeyPassword); // never prompt on console

      if (!tlsPrivateKeyFile.empty() && !tlsCertificateFile.empty())
      {
        if (TCS_ok != tLayer->setPrivateKeyFile(tlsPrivateKeyFile.c_str(), keyFileFormat))
        {
          OFLOG_FATAL(dcmpssndLogger, "unable to load private TLS key from '" << tlsPrivateKeyFile<< "'");
          return 1;
        }
        if (TCS_ok != tLayer->setCertificateFile(tlsCertificateFile.c_str(), keyFileFormat))
        {
          OFLOG_FATAL(dcmpssndLogger, "unable to load certificate from '" << tlsCertificateFile << "'");
          return 1;
        }
        if (! tLayer->checkPrivateKeyMatchesCertificate())
        {
          OFLOG_FATAL(dcmpssndLogger, "private key '" << tlsPrivateKeyFile << "' and certificate '" << tlsCertificateFile << "' do not match");
          return 1;
        }
      }

      tLayer->setCertificateVerification(tlsCertVerification);

      // a generated UID contains the process ID and current time.
      // Adding it to the PRNG seed guarantees that we have different seeds for different processes.
      char randomUID[65];
      dcmGenerateUniqueIdentifier(randomUID);
      tLayer->addPRNGseed(randomUID, strlen(randomUID));
    }

    if (useTLS)
    {
      OFString cslist;
      if (tLayer) tLayer->getListOfCipherSuitesForOpenSSL(cslist);
      verboseParameters << "\tTLS certificate : " << tlsCertificateFile << OFendl
           << "\tTLS key file    : " << tlsPrivateKeyFile << OFendl
           << "\tTLS DH params   : " << tlsDHParametersFile << OFendl
           << "\tTLS PRNG seed   : " << tlsRandomSeedFile << OFendl
           << "\tTLS CA directory: " << tlsCACertificateFolder << OFendl
           << "\tTLS ciphersuites: " << cslist << OFendl
           << "\tTLS key format  : ";
      if (keyFileFormat == DCF_Filetype_PEM) verboseParameters << "PEM" << OFendl; else verboseParameters << "DER" << OFendl;
      verboseParameters << "\tTLS cert verify : ";
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
    OFSTRINGSTREAM_GETOFSTRING(verboseParameters, verboseParametersString)
    OFLOG_INFO(dcmpssndLogger, verboseParametersString);

    /* open network connection */
    T_ASC_Network *net=NULL;
    T_ASC_Parameters *params=NULL;
    DIC_NODENAME peerHost;
    T_ASC_Association *assoc=NULL;

    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 30, &net);
    if (cond.bad())
    {
      OFLOG_FATAL(dcmpssndLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }

#ifdef WITH_OPENSSL
    if (tLayer)
    {
      cond = ASC_setTransportLayer(net, tLayer, 0);
      if (cond.bad())
      {
        OFLOG_FATAL(dcmpssndLogger, DimseCondition::dump(temp_str, cond));
        return 1;
      }
    }
#endif

    cond = ASC_createAssociationParameters(&params, targetMaxPDU);
    if (cond.bad())
    {
      OFLOG_FATAL(dcmpssndLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }

    cond = ASC_setTransportLayerType(params, useTLS);
    if (cond.bad())
    {
      OFLOG_FATAL(dcmpssndLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }

    ASC_setAPTitles(params, dvi.getNetworkAETitle(), targetAETitle, NULL);

    sprintf(peerHost, "%s:%d", targetHostname, (int)targetPort);
    ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), peerHost);

    cond = addAllStoragePresentationContexts(params, targetImplicitOnly);
    if (cond.bad())
    {
      OFLOG_FATAL(dcmpssndLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }

    if (messagePort > 0)
    {
      messageClient = new DVPSIPCClient(DVPSIPCMessage::clientStoreSCU, verboseParametersString.c_str(), messagePort, keepMessagePortOpen);
      if (! messageClient->isServerActive())
      {
        OFLOG_WARN(dcmpssndLogger, "no IPC message server found at port " << messagePort << ", disabling IPC");
      }
    }

    /* create association */
    OFLOG_INFO(dcmpssndLogger, "Requesting Association");

    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad())
    {
        if (cond == DUL_ASSOCIATIONREJECTED)
        {
            T_ASC_RejectParameters rej;

            ASC_getRejectParameters(params, &rej);
            OFLOG_ERROR(dcmpssndLogger, "Association Rejected" << OFendl << ASC_printRejectParameters(temp_str, &rej));
            if (messageClient)
            {
              // notify about rejected association
              OFOStringStream out;
              out << "DIMSE Association Rejected:" << OFendl
                  << "\t" << ASC_printRejectParameters(temp_str, &rej) << OFendl;
              out << "\tcalled presentation address: " << assoc->params->DULparams.calledPresentationAddress << OFendl
                  << "\tcalling AE title: " << assoc->params->DULparams.callingAPTitle << OFendl
                  << "\tcalled AE title: " << assoc->params->DULparams.calledAPTitle << OFendl;
              out << ASC_dumpConnectionParameters(temp_str, assoc) << OFendl;
              out << OFStringStream_ends;
              OFSTRINGSTREAM_GETSTR(out, theString)
              if (useTLS)
                messageClient->notifyRequestedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
                else messageClient->notifyRequestedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
              messageClient->notifyApplicationTerminates(DVPSIPCMessage::statusError);
              OFSTRINGSTREAM_FREESTR(theString)
              delete messageClient;
            }
            return 1;
        } else {
          OFLOG_ERROR(dcmpssndLogger, "Association Request Failed " << DimseCondition::dump(temp_str, cond));
            if (messageClient)
            {
              // notify about rejected association
              OFOStringStream out;
              out << "DIMSE Association Request Failed:" << OFendl
                  << "\tcalled presentation address: " << assoc->params->DULparams.calledPresentationAddress << OFendl
                  << "\tcalling AE title: " << assoc->params->DULparams.callingAPTitle << OFendl
                  << "\tcalled AE title: " << assoc->params->DULparams.calledAPTitle << OFendl
                  << ASC_dumpConnectionParameters(temp_str, assoc) << OFendl
                  << cond.text() << OFendl << OFStringStream_ends;
              OFSTRINGSTREAM_GETSTR(out, theString)
              if (useTLS)
                messageClient->notifyRequestedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
                else messageClient->notifyRequestedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
              messageClient->notifyApplicationTerminates(DVPSIPCMessage::statusError);
              OFSTRINGSTREAM_FREESTR(theString)
              delete messageClient;
            }
            return 1;
        }
    }

    if (ASC_countAcceptedPresentationContexts(params) == 0)
    {
      OFLOG_ERROR(dcmpssndLogger, "No Acceptable Presentation Contexts");
      cond = ASC_abortAssociation(assoc);
      if (cond.bad())
      {
        OFLOG_ERROR(dcmpssndLogger, "Association Abort Failed\n" << DimseCondition::dump(temp_str, cond));
      }
      if (messageClient)
      {
        // notify about rejected association
        OFOStringStream out;
        out << "DIMSE association accepted, but no acceptable presentation contexts - aborting" << OFendl
            << "\tcalled presentation address: " << assoc->params->DULparams.calledPresentationAddress << OFendl
            << "\tcalling AE title: " << assoc->params->DULparams.callingAPTitle << OFendl
            << "\tcalled AE title: " << assoc->params->DULparams.calledAPTitle << OFendl
            << ASC_dumpConnectionParameters(temp_str, assoc) << OFendl
            << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(out, theString)
        if (useTLS)
          messageClient->notifyRequestedEncryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
          else messageClient->notifyRequestedUnencryptedDICOMConnection(DVPSIPCMessage::statusError, theString);
        messageClient->notifyApplicationTerminates(DVPSIPCMessage::statusError);
        OFSTRINGSTREAM_FREESTR(theString)
        delete messageClient;
      }
      return 1;
    }

    OFLOG_INFO(dcmpssndLogger, "Association accepted (Max Send PDV: " << assoc->sendPDVLength << ")");

    if (messageClient)
    {
      // notify about successfully negotiated association
      OFOStringStream out;
      out << "DIMSE Association Accepted:" << OFendl
          << "\tcalled presentation address: " << assoc->params->DULparams.calledPresentationAddress << OFendl
          << "\tcalling AE title: " << assoc->params->DULparams.callingAPTitle << OFendl
          << "\tcalled AE title: " << assoc->params->DULparams.calledAPTitle << OFendl
          << "\tmax send PDV: " << assoc->sendPDVLength << OFendl
          << "\tpresentation contexts: " << ASC_countAcceptedPresentationContexts(assoc->params) << OFendl;
      out << ASC_dumpConnectionParameters(temp_str, assoc) << OFendl;
      out << OFStringStream_ends;
      OFSTRINGSTREAM_GETSTR(out, theString)
      if (useTLS)
        messageClient->notifyRequestedEncryptedDICOMConnection(DVPSIPCMessage::statusOK, theString);
        else messageClient->notifyRequestedUnencryptedDICOMConnection(DVPSIPCMessage::statusOK, theString);
      OFSTRINGSTREAM_FREESTR(theString)
    }


    /* do the real work */
    cond = sendStudy(dbhandle, assoc, opt_studyUID, opt_seriesUID, opt_instanceUID);

    /* tear down association */
    if (cond.good())
    {
        /* release association */
        OFLOG_INFO(dcmpssndLogger, "Releasing Association");
        cond = ASC_releaseAssociation(assoc);
        if (cond.bad())
        {
          OFLOG_ERROR(dcmpssndLogger, "Association Release Failed\n" << DimseCondition::dump(temp_str, cond));
          if (messageClient)
          {
            messageClient->notifyApplicationTerminates(DVPSIPCMessage::statusError);
            delete messageClient;
          }
          return 1;
        }
        if (messageClient) messageClient->notifyConnectionClosed(DVPSIPCMessage::statusOK);
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        OFLOG_ERROR(dcmpssndLogger, "Protocol Error: peer requested release (Aborting)");
        OFLOG_INFO(dcmpssndLogger, "Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad())
        {
            OFLOG_ERROR(dcmpssndLogger, "Association Abort Failed" << DimseCondition::dump(temp_str, cond));
            if (messageClient)
            {
              messageClient->notifyApplicationTerminates(DVPSIPCMessage::statusError);
              delete messageClient;
            }
            return 1;
        }
        if (messageClient) messageClient->notifyConnectionAborted(DVPSIPCMessage::statusError, "Protocol error: peer requested release, aborting association");
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        OFLOG_INFO(dcmpssndLogger, "Peer Aborted Association");
        if (messageClient) messageClient->notifyConnectionAborted(DVPSIPCMessage::statusError, "Peer aborted association");
    }
    else
    {
        OFLOG_ERROR(dcmpssndLogger, "SCU Failed" << DimseCondition::dump(temp_str, cond));
        OFLOG_INFO(dcmpssndLogger, "Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad())
        {
            OFLOG_ERROR(dcmpssndLogger, "Association Abort Failed" << DimseCondition::dump(temp_str, cond));
            if (messageClient)
            {
              messageClient->notifyApplicationTerminates(DVPSIPCMessage::statusError);
              delete messageClient;
            }
            return 1;
        }
        if (messageClient) messageClient->notifyConnectionAborted(DVPSIPCMessage::statusError, "Storage SCU failed, aborting association");
    }

    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad())
    {
      OFLOG_ERROR(dcmpssndLogger, DimseCondition::dump(temp_str, cond));
      if (messageClient)
      {
        messageClient->notifyApplicationTerminates(DVPSIPCMessage::statusError);
        delete messageClient;
      }
      return 1;
    }
    cond = ASC_dropNetwork(&net);
    if (cond.bad())
    {
      OFLOG_ERROR(dcmpssndLogger, DimseCondition::dump(temp_str, cond));
      if (messageClient)
      {
        messageClient->notifyApplicationTerminates(DVPSIPCMessage::statusError);
        delete messageClient;
      }
      return 1;
    }

    // tell the IPC server that we're going to terminate.
    // We need to do this before we shutdown WinSock.
    if (messageClient)
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
          OFLOG_WARN(dcmpssndLogger, "cannot write back random seed file '" << tlsRandomSeedFile << "', ignoring");
        }
      } else {
        OFLOG_WARN(dcmpssndLogger, "cannot write back random seed, ignoring");
      }
    }
    delete tLayer;
#endif

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif

    return 0;
}
