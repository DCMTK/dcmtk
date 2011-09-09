/*
 *
 *  Copyright (C) 2011, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Query/Retrieve Service Class User (C-GET operation)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-09-09 08:58:11 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/dcmnet/scu.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmdata/dcostrmz.h"   /* for dcmZlibCompressionLevel */
#include "dcmtk/dcmdata/dcpath.h"     /* for DcmPathProcessor */

#ifdef WITH_ZLIB
#include <zlib.h>     /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "getscu"

static OFLogger getscuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default application titles */
#define APPLICATIONTITLE        "GETSCU"
#define PEERAPPLICATIONTITLE    "ANY-SCP"

typedef enum {
  QMPatientRoot = 0,
  QMStudyRoot = 1,
  QMPatientStudyOnly = 2
} QueryModel;

static const char* querySyntax[3] = {
  UID_GETPatientRootQueryRetrieveInformationModel,
  UID_GETStudyRootQueryRetrieveInformationModel,
  UID_RETIRED_GETPatientStudyOnlyQueryRetrieveInformationModel
};

OFCmdUnsignedInt        opt_maxPDU = ASC_DEFAULTMAXPDU;
E_TransferSyntax        opt_store_networkTransferSyntax = EXS_Unknown;
E_TransferSyntax        opt_get_networkTransferSyntax = EXS_Unknown;
DcmStorageMode          opt_storageMode = DCMSCU_STORAGE_DISK;
OFBool                  opt_showPresentationContexts = OFFalse;
OFBool                  opt_abortAssociation = OFFalse;
OFCmdUnsignedInt        opt_repeatCount = 1;
QueryModel              opt_queryModel = QMPatientRoot;
T_DIMSE_BlockingMode    opt_blockMode = DIMSE_BLOCKING;
int                     opt_dimse_timeout = 0;
int                     opt_acse_timeout = 30;
OFString                opt_outputDirectory = ".";
static OFList<OFString> overrideKeys;

static void prepareTS(E_TransferSyntax ts,
                      OFList<OFString>& syntaxes);

static void applyOverrideKeys(DcmDataset *dataset);

#define SHORTCOL 4
#define LONGCOL 21

int
main(int argc, char *argv[])
{
  const char *opt_peer;
  OFCmdUnsignedInt opt_port = 104;;
  const char *opt_peerTitle = PEERAPPLICATIONTITLE;
  const char *opt_ourTitle = APPLICATIONTITLE;
  OFList<OFString> fileNameList;

#ifdef HAVE_GUSI_H
  /* needed for Macintosh */
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

  char tempstr[20];
  OFString temp_str;
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM retrieve (C-GET) SCU", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  cmd.addParam("peer", "hostname of DICOM peer");
  cmd.addParam("port", "tcp/ip port number of peer");
  cmd.addParam("dcmfile-in", "DICOM query file(s)", OFCmdParam::PM_MultiOptional);

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
   cmd.addOption("--help",                   "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",                           "print version information and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--verbose-pc",             "+v",      "show presentation contexts in verbose mode");
   OFLog::addOptions(cmd);

  cmd.addGroup("network options:");
    cmd.addSubGroup("override matching keys:");
      cmd.addOption("--key",                 "-k",   1, "[k]ey: gggg,eeee=\"str\", path or dic. name=\"str\"",
                                                        "override matching key");
    cmd.addSubGroup("retrieve information model:");
      cmd.addOption("--patient",             "-P",      "use patient root information model (default)");
      cmd.addOption("--study",               "-S",      "use study root information model");
      cmd.addOption("--psonly",              "-O",      "use patient/study only information model");
    cmd.addSubGroup("application entity titles:");
      OFString opt1 = "set my calling AE title (default: ";
      opt1 += APPLICATIONTITLE;
      opt1 += ")";
      cmd.addOption("--aetitle",             "-aet", 1, "[a]etitle: string", opt1.c_str());
      OFString opt2 = "set called AE title of peer (default: ";
      opt2 += PEERAPPLICATIONTITLE;
      opt2 += ")";
      cmd.addOption("--call",                "-aec", 1, "[a]etitle: string", opt2.c_str());
    cmd.addSubGroup("preferred storage transfer syntaxes (incoming associations):");
      cmd.addOption("--prefer-uncompr",      "+x=",     "prefer explicit VR local byte order (default)");
      cmd.addOption("--prefer-little",       "+xe",     "prefer explicit VR little endian TS");
      cmd.addOption("--prefer-big",          "+xb",     "prefer explicit VR big endian TS");
      cmd.addOption("--prefer-lossless",     "+xs",     "prefer default JPEG lossless TS");
      cmd.addOption("--prefer-jpeg8",        "+xy",     "prefer default JPEG lossy TS for 8 bit data");
      cmd.addOption("--prefer-jpeg12",       "+xx",     "prefer default JPEG lossy TS for 12 bit data");
      cmd.addOption("--prefer-j2k-lossless", "+xv",     "prefer JPEG 2000 lossless TS");
      cmd.addOption("--prefer-j2k-lossy",    "+xw",     "prefer JPEG 2000 lossy TS");
      cmd.addOption("--prefer-jls-lossless", "+xt",     "prefer JPEG-LS lossless TS");
      cmd.addOption("--prefer-jls-lossy",    "+xu",     "prefer JPEG-LS lossy TS");
      cmd.addOption("--prefer-mpeg2",        "+xm",     "prefer MPEG2 Main Profile @ Main Level TS");
      cmd.addOption("--prefer-mpeg2-high",   "+xh",     "prefer MPEG2 Main Profile @ High Level TS");
      cmd.addOption("--prefer-mpeg4",        "+xn",     "prefer MPEG4 AVC/H.264 HP / Level 4.1 TS");
      cmd.addOption("--prefer-mpeg4-bd",     "+xl",     "prefer MPEG4 AVC/H.264 BD-compatible TS");
      cmd.addOption("--prefer-rle",          "+xr",     "prefer RLE lossless TS");
#ifdef WITH_ZLIB
      cmd.addOption("--prefer-deflated",     "+xd",     "prefer deflated explicit VR little endian TS");
#endif
      cmd.addOption("--implicit",            "+xi",     "accept implicit VR little endian TS only");
    cmd.addSubGroup("proposed retrieve transfer syntaxes (outgoing associations):");
      cmd.addOption("--propose-uncompr",     "-x=",     "propose all uncompressed TS, explicit VR\nwith local byte ordering first (default)");
      cmd.addOption("--propose-little",      "-xe",     "propose all uncompressed TS, explicit VR\nlittle endian first");
      cmd.addOption("--propose-big",         "-xb",     "propose all uncompressed TS, explicit VR\nbig endian first");
#ifdef WITH_ZLIB
      cmd.addOption("--propose-deflated",    "-xd",     "propose deflated explicit VR little endian TS\nand all uncompressed transfer syntaxes");
#endif
      cmd.addOption("--propose-implicit",    "-xi",     "propose implicit VR little endian TS only");
#ifdef WITH_TCPWRAPPER
    cmd.addSubGroup("network host access control (tcp wrapper):");
      cmd.addOption("--access-full",         "-ac",     "accept connections from any host (default)");
      cmd.addOption("--access-control",      "+ac",     "enforce host access control rules");
#endif
    cmd.addSubGroup("other network options:");
      cmd.addOption("--timeout",             "-to",  1, "[s]econds: integer (default: unlimited)", "timeout for connection requests");
      cmd.addOption("--acse-timeout",        "-ta",  1, "[s]econds: integer (default: 30)", "timeout for ACSE messages");
      cmd.addOption("--dimse-timeout",       "-td",  1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");

      OFString opt3 = "set max receive pdu to n bytes (default: ";
      sprintf(tempstr, "%ld", OFstatic_cast(long, ASC_DEFAULTMAXPDU));
      opt3 += tempstr;
      opt3 += ")";
      OFString opt4 = "[n]umber of bytes: integer (";
      sprintf(tempstr, "%ld", OFstatic_cast(long, ASC_MINIMUMPDUSIZE));
      opt4 += tempstr;
      opt4 += "..";
      sprintf(tempstr, "%ld", OFstatic_cast(long, ASC_MAXIMUMPDUSIZE));
      opt4 += tempstr;
      opt4 += ")";
      cmd.addOption("--max-pdu",             "-pdu", 1, opt4.c_str(), opt3.c_str());
      cmd.addOption("--disable-host-lookup", "-dhl",    "disable hostname lookup");
      cmd.addOption("--repeat",                      1, "[n]umber: integer", "repeat n times");
      cmd.addOption("--abort",                          "abort association instead of releasing it");
  cmd.addGroup("output options:");
    cmd.addSubGroup("general:");
      cmd.addOption("--output-directory",    "-od",  1, "[d]irectory: string (default: \".\")", "write received objects to existing directory d");
    cmd.addSubGroup("storage mode:");
      cmd.addOption("--normal",              "-B",      "receive in memory, then write to disk (default)");
      cmd.addOption("--bit-preserving",      "+B",      "receive directly to disk");
      cmd.addOption("--ignore",                         "ignore store data, receive but do not store");

  /* evaluate command line */
  prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
  if (!app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
  {
    exit(1);
  }
  /* check exclusive options first */
  if (cmd.hasExclusiveOption())
  {
    if (cmd.findOption("--version"))
    {
      app.printHeader(OFTrue /*print host identifier*/);
      COUT << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_TCPWRAPPER)
      COUT << " none" << OFendl;
#else
      COUT << OFendl;
#endif
#ifdef WITH_ZLIB
      COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef WITH_TCPWRAPPER
      COUT << "- LIBWRAP" << OFendl;
#endif
      return 0;
    }
  }

  /* command line parameters */

  cmd.getParam(1, opt_peer);
  app.checkParam(cmd.getParamAndCheckMinMax(2, opt_port, 1, 65535));

  OFLog::configureFromCommandLine(cmd, app);
  if (cmd.findOption("--verbose-pc"))
  {
    app.checkDependence("--verbose-pc", "verbose mode", getscuLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL));
    opt_showPresentationContexts = OFTrue;
  }

  if (cmd.findOption("--key", 0, OFCommandLine::FOM_FirstFromLeft))
  {
    const char *ovKey = NULL;
    do
    {
      app.checkValue(cmd.getValue(ovKey));
      overrideKeys.push_back(ovKey);
    } while (cmd.findOption("--key", 0, OFCommandLine::FOM_NextFromLeft));
  }

  cmd.beginOptionBlock();
  if (cmd.findOption("--patient")) opt_queryModel = QMPatientRoot;
  if (cmd.findOption("--study")) opt_queryModel = QMStudyRoot;
  if (cmd.findOption("--psonly")) opt_queryModel = QMPatientStudyOnly;
  cmd.endOptionBlock();

  if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
  if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));

  cmd.beginOptionBlock();
  if (cmd.findOption("--prefer-uncompr"))
  {
    opt_store_networkTransferSyntax = EXS_Unknown;
  }
  if (cmd.findOption("--prefer-little")) opt_store_networkTransferSyntax = EXS_LittleEndianExplicit;
  if (cmd.findOption("--prefer-big")) opt_store_networkTransferSyntax = EXS_BigEndianExplicit;
  if (cmd.findOption("--prefer-lossless")) opt_store_networkTransferSyntax = EXS_JPEGProcess14SV1;
  if (cmd.findOption("--prefer-jpeg8")) opt_store_networkTransferSyntax = EXS_JPEGProcess1;
  if (cmd.findOption("--prefer-jpeg12")) opt_store_networkTransferSyntax = EXS_JPEGProcess2_4;
  if (cmd.findOption("--prefer-j2k-lossless")) opt_store_networkTransferSyntax = EXS_JPEG2000LosslessOnly;
  if (cmd.findOption("--prefer-j2k-lossy")) opt_store_networkTransferSyntax = EXS_JPEG2000;
  if (cmd.findOption("--prefer-jls-lossless")) opt_store_networkTransferSyntax = EXS_JPEGLSLossless;
  if (cmd.findOption("--prefer-jls-lossy")) opt_store_networkTransferSyntax = EXS_JPEGLSLossy;
  if (cmd.findOption("--prefer-mpeg2")) opt_store_networkTransferSyntax = EXS_MPEG2MainProfileAtMainLevel;
  if (cmd.findOption("--prefer-mpeg2-high")) opt_store_networkTransferSyntax = EXS_MPEG2MainProfileAtHighLevel;
  if (cmd.findOption("--prefer-mpeg4")) opt_store_networkTransferSyntax = EXS_MPEG4HighProfileLevel4_1;
  if (cmd.findOption("--prefer-mpeg4-bd")) opt_store_networkTransferSyntax = EXS_MPEG4BDcompatibleHighProfileLevel4_1;
  if (cmd.findOption("--prefer-rle")) opt_store_networkTransferSyntax = EXS_RLELossless;
#ifdef WITH_ZLIB
  if (cmd.findOption("--prefer-deflated")) opt_store_networkTransferSyntax = EXS_DeflatedLittleEndianExplicit;
#endif
  if (cmd.findOption("--implicit")) opt_store_networkTransferSyntax = EXS_LittleEndianImplicit;
  cmd.endOptionBlock();

  cmd.beginOptionBlock();
  if (cmd.findOption("--propose-uncompr")) opt_get_networkTransferSyntax = EXS_Unknown;
  if (cmd.findOption("--propose-little")) opt_get_networkTransferSyntax = EXS_LittleEndianExplicit;
  if (cmd.findOption("--propose-big")) opt_get_networkTransferSyntax = EXS_BigEndianExplicit;
  if (cmd.findOption("--propose-implicit")) opt_get_networkTransferSyntax = EXS_LittleEndianImplicit;
#ifdef WITH_ZLIB
  if (cmd.findOption("--propose-deflated")) opt_get_networkTransferSyntax = EXS_DeflatedLittleEndianExplicit;
#endif
  cmd.endOptionBlock();

#ifdef WITH_TCPWRAPPER
  cmd.beginOptionBlock();
  if (cmd.findOption("--access-full")) dcmTCPWrapperDaemonName.set(NULL);
  if (cmd.findOption("--access-control")) dcmTCPWrapperDaemonName.set(OFFIS_CONSOLE_APPLICATION);
  cmd.endOptionBlock();
#endif

  if (cmd.findOption("--timeout"))
  {
    OFCmdSignedInt opt_timeout = 0;
    app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
    dcmConnectionTimeout.set(OFstatic_cast(Sint32, opt_timeout));
  }

  if (cmd.findOption("--acse-timeout"))
  {
    OFCmdSignedInt opt_timeout = 0;
    app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
    opt_acse_timeout = OFstatic_cast(int, opt_timeout);
  }

  if (cmd.findOption("--dimse-timeout"))
  {
    OFCmdSignedInt opt_timeout = 0;
    app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
    opt_dimse_timeout = OFstatic_cast(int, opt_timeout);
    opt_blockMode = DIMSE_NONBLOCKING;
  }

  if (cmd.findOption("--max-pdu")) app.checkValue(cmd.getValueAndCheckMinMax(opt_maxPDU, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
  if (cmd.findOption("--disable-host-lookup")) dcmDisableGethostbyaddr.set(OFTrue);
  if (cmd.findOption("--repeat"))  app.checkValue(cmd.getValueAndCheckMin(opt_repeatCount, 1));
  if (cmd.findOption("--abort"))   opt_abortAssociation = OFTrue;
  if (cmd.findOption("--ignore"))  opt_storageMode = DCMSCU_STORAGE_IGNORE;

  if (cmd.findOption("--output-directory"))
  {
    app.checkValue(cmd.getValue(opt_outputDirectory));
    app.checkConflict("--output-directory", "--ignore", opt_storageMode == DCMSCU_STORAGE_IGNORE);
  }

  cmd.beginOptionBlock();
  if (cmd.findOption("--bit-preserving"))
  {
    app.checkConflict("--bit-preserving", "--ignore", opt_storageMode == DCMSCU_STORAGE_IGNORE);
    opt_storageMode = DCMSCU_STORAGE_BIT_PRESERVING;
  }
  if (cmd.findOption("--normal"))
  {
    app.checkConflict("--normal", "--bit-preserving", opt_storageMode == DCMSCU_STORAGE_BIT_PRESERVING);
    app.checkConflict("--normal", "--ignore", opt_storageMode == DCMSCU_STORAGE_IGNORE);
    opt_storageMode = DCMSCU_STORAGE_DISK;
  }
  cmd.endOptionBlock();

  /* finally parse filenames */
  int paramCount = cmd.getParamCount();
  const char *currentFilename = NULL;
  OFString errormsg;

  for (int i=3; i <= paramCount; i++)
  {
    cmd.getParam(i, currentFilename);
    if (access(currentFilename, R_OK) < 0)
    {
      errormsg = "cannot access file: ";
      errormsg += currentFilename;
      app.printError(errormsg.c_str());
    }
    fileNameList.push_back(currentFilename);
  }

  if (fileNameList.empty() && overrideKeys.empty())
  {
    app.printError("either query file or override keys (or both) must be specified");
  }

  /* print resource identifier */
  OFLOG_DEBUG(getscuLogger, rcsid << OFendl);

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
    OFLOG_WARN(getscuLogger, "no data dictionary loaded, check environment variable: "
      << DCM_DICT_ENVIRONMENT_VARIABLE);
  }

  /* make sure output directory exists and is writeable */

  if (!OFStandard::dirExists(opt_outputDirectory))
  {
    OFLOG_FATAL(getscuLogger, "specified output directory does not exist");
    return 1;
  }
  else if (!OFStandard::isWriteable(opt_outputDirectory))
  {
    OFLOG_FATAL(getscuLogger, "specified output directory is not writeable");
    return 1;
  }

  // Setup SCU
  OFList<OFString> syntaxes;
  prepareTS(opt_get_networkTransferSyntax, syntaxes);
  DcmSCU scu;
  scu.setMaxReceivePDULength(opt_maxPDU);
  scu.setACSETimeout(opt_acse_timeout);
  scu.setDIMSEBlockingMode(opt_blockMode);
  scu.setDIMSETimeout(opt_dimse_timeout);
  scu.setAETitle(opt_ourTitle);
  scu.setPeerHostName(opt_peer);
  scu.setPeerPort(OFstatic_cast(Uint16, opt_port));
  scu.setPeerAETitle(opt_peerTitle);
  scu.setVerbosePCMode(opt_showPresentationContexts);

  // Add presentation contexts for get and find (we do not actually need find...)
  // (only uncompressed)
  scu.addPresentationContext(querySyntax[opt_queryModel], syntaxes);

  // Add storage presentation contexts (long list of storage SOP classes, uncompressed)
  syntaxes.clear();
  prepareTS(opt_store_networkTransferSyntax, syntaxes);
  for (Uint16 i = 0; i < numberOfDcmLongSCUStorageSOPClassUIDs; i++)
  {
    scu.addPresentationContext(dcmLongSCUStorageSOPClassUIDs[i], syntaxes, ASC_SC_ROLE_SCP);
  }

  // Set the storage mode
  scu.setStorageMode(opt_storageMode);
  if (opt_storageMode != DCMSCU_STORAGE_IGNORE)
  {
    scu.setStorageDir(opt_outputDirectory);
  }

  // Initialize network and negotiate association
  OFCondition cond = scu.initNetwork();
  if (cond.bad())
  {
    OFLOG_FATAL(getscuLogger, DimseCondition::dump(temp_str, cond));
    exit(1);
  }
  cond = scu.negotiateAssociation();
  if (cond.bad())
  {
    OFLOG_FATAL(getscuLogger, "No Acceptable Presentation Contexts");
    exit(1);
  }
  cond = EC_Normal;
  T_ASC_PresentationContextID pcid = scu.findPresentationContextID(querySyntax[opt_queryModel], "");
  if (pcid == 0)
  {
    OFLOG_FATAL(getscuLogger, "No adequate Presentation Contexts for sending C-GET");
    exit(1);
  }

  /* Do the real work, i.e. send C-GET requests and receive objects */
  for (Uint16 repeat = 0; repeat < opt_repeatCount; repeat++)
  {
    Uint16 numRuns = 1;
    DcmFileFormat dcmff;
    DcmDataset *dset = dcmff.getDataset();
    OFListConstIterator(OFString) it;
    /* Load first file, if there is one */
    if (!fileNameList.empty())
    {
      numRuns = fileNameList.size();
      it = fileNameList.begin();
      cond = dcmff.loadFile((*it).c_str());
      if (cond.bad())
      {
        OFLOG_FATAL(getscuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
      }
      dset = dcmff.getDataset();
    }
    OFList<RetrieveResponse*> responses;
    /* For all files (or at least one run from override keys) */
    for (Uint16 i = 0; i < numRuns; i++)
    {
      applyOverrideKeys(dset);
      cond = scu.sendCGETRequest(pcid, dset, &responses);
      if (cond.bad())
      {
        exit(1);
      }
      /* Load next file if there is one */
      if (numRuns > 1)
      {
        it++;
        cond = dcmff.loadFile((*it).c_str());
        if (cond.bad())
        {
          OFLOG_FATAL(getscuLogger, DimseCondition::dump(temp_str, cond));
          exit(1);
        }
        dset = dcmff.getDataset();
      }
    }
    if (!responses.empty())
    {
      /* Output final status report */
      OFLOG_INFO(getscuLogger, "Final status report from last C-GET message:");
      (*(--responses.end()))->print();
      /* Delete responses */
      OFListIterator(RetrieveResponse*) iter = responses.begin();
      OFListConstIterator(RetrieveResponse*) last = responses.end();
      while (iter != last)
      {
        delete (*iter);
        iter = responses.erase(iter);
      }
    }
  }

  /* tear down association */
  if (cond == EC_Normal)
  {
    if (opt_abortAssociation)
    {
      OFLOG_INFO(getscuLogger, "Aborting Association");
      scu.closeAssociation(DCMSCU_ABORT_ASSOCIATION);
      exit(0);
    } else
    {
      /* release association */
      OFLOG_INFO(getscuLogger, "Releasing Association");
      scu.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
      exit(0);
    }
  }
  else if (cond == DUL_PEERREQUESTEDRELEASE)
  {
    OFLOG_ERROR(getscuLogger, "Protocol Error: Peer requested release (Aborting)");
    scu.closeAssociation(DCMSCU_ABORT_ASSOCIATION);
    exit(1);
  }
  else if (cond == DUL_PEERABORTEDASSOCIATION)
  {
    OFLOG_INFO(getscuLogger, "Peer Aborted Association");
  }
  else
  {
    OFLOG_ERROR(getscuLogger, "GET SCU Failed: " << DimseCondition::dump(temp_str, cond));
    OFLOG_INFO(getscuLogger, "Aborting Association");
    scu.closeAssociation(DCMSCU_ABORT_ASSOCIATION);
    exit(1);
  }

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif

  return 0;
}

static void applyOverrideKeys(DcmDataset *dataset)
{
  /* replace specific keys by those in overrideKeys */
  OFListConstIterator(OFString) path = overrideKeys.begin();
  OFListConstIterator(OFString) endOfList = overrideKeys.end();
  DcmPathProcessor proc;
  proc.setItemWildcardSupport(OFFalse);
  proc.checkPrivateReservations(OFFalse);
  OFCondition cond;
  while (path != endOfList)
  {
    cond = proc.applyPathWithValue(dataset, *path);
    if (cond.bad())
    {
      OFLOG_ERROR(getscuLogger, "Bad override key/path: " << *path << ": " << cond.text());
    }
    path++;
  }
}

static void prepareTS(E_TransferSyntax ts,
                      OFList<OFString>& syntaxes)
{
  /*
  ** We prefer to use Explicitly encoded transfer syntaxes.
  ** If we are running on a Little Endian machine we prefer
  ** LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
  ** Some SCP implementations will just select the first transfer
  ** syntax they support (this is not part of the standard) so
  ** organise the proposed transfer syntaxes to take advantage
  ** of such behaviour.
  **
  ** The presentation contexts proposed here are only used for
  ** C-FIND and C-MOVE, so there is no need to support compressed
  ** transmission.
  */

  switch (ts)
  {
    case EXS_LittleEndianImplicit:
      /* we only support Little Endian Implicit */
      syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      break;
    case EXS_LittleEndianExplicit:
      /* we prefer Little Endian Explicit */
      syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
    case EXS_BigEndianExplicit:
      /* we prefer Big Endian Explicit */
      syntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
#ifdef WITH_ZLIB
    case EXS_DeflatedLittleEndianExplicit:
      /* we prefer Deflated Little Endian Explicit */
      syntaxes.push_back(UID_DeflatedExplicitVRLittleEndianTransferSyntax);
      syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      syntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
#endif
    default:
      DcmXfer xfer(ts);
      if (xfer.isEncapsulated())
      {
        syntaxes.push_back(xfer.getXferID());
      }
      /* We prefer explicit transfer syntaxes.
       * If we are running on a Little Endian machine we prefer
       * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
       */
      if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
      {
        syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
        syntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      } else
      {
        syntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
        syntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      }
      syntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
  }
}


/*
** CVS Log
** $Log: getscu.cc,v $
** Revision 1.5  2011-09-09 08:58:11  joergr
** Replaced local list of supported storage SOP classes by the global variable
** dcmLongSCUStorageSOPClassUIDs, which is maintained at a central location.
**
** Revision 1.4  2011-09-07 12:40:45  joergr
** Made more clear that option --key also supports path expressions.
**
** Revision 1.3  2011-08-25 15:05:04  joergr
** Changed data structure for Q/R responses from OFVector to OFList. Also fixed
** some possible memory leaks and made the FIND/MOVE/GET code more consistent.
**
** Revision 1.2  2011-08-25 12:51:22  joergr
** Added "verbose-pc" mode that shows the presentation contexts in verbose mode.
**
** Revision 1.1  2011-08-25 09:31:32  onken
** Added C-GET functionality to DcmSCU class and accompanying getscu
** commandline application.
**
*/
