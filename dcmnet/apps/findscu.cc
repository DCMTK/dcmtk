/*
 *
 *  Copyright (C) 1994-2018, OFFIS e.V.
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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Query/Retrieve Service Class User (C-FIND operation)
 *
 */

#include "dcmtk/config/osconfig.h"      /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/dfindscu.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcostrmz.h"     /* for dcmZlibCompressionLevel */
#include "dcmtk/dcmtls/tlsopt.h"        /* for DcmTLSOptions */

#ifdef WITH_ZLIB
#include <zlib.h>                       /* for zlibVersion() */
#endif
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
#include "dcmtk/ofstd/ofchrenc.h"       /* for OFCharacterEncoding */
#endif

#define OFFIS_CONSOLE_APPLICATION "findscu"

static OFLogger findscuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default application titles */
#define APPLICATIONTITLE        "FINDSCU"
#define PEERAPPLICATIONTITLE    "ANY-SCP"

#define SHORTCOL 4
#define LONGCOL 20

int main(int argc, char *argv[])
{
    OFList<OFString>      fileNameList;
    OFBool                opt_abortAssociation = OFFalse;
    const char *          opt_abstractSyntax = UID_FINDModalityWorklistInformationModel;
    int                   opt_acse_timeout = 30;
    T_DIMSE_BlockingMode  opt_blockMode = DIMSE_BLOCKING;
    OFCmdSignedInt        opt_cancelAfterNResponses = -1;
    int                   opt_dimse_timeout = 0;
    int                   opt_outputResponsesToLogger = 0;
    DcmFindSCUExtractMode opt_extractResponses = FEM_none;
    OFString              opt_extractXMLFilename;
    OFString              opt_outputDirectory = ".";
    OFCmdUnsignedInt      opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
    E_TransferSyntax      opt_networkTransferSyntax = EXS_Unknown;
    const char *          opt_ourTitle = APPLICATIONTITLE;
    const char *          opt_peer;
    const char *          opt_peerTitle = PEERAPPLICATIONTITLE;
    OFCmdUnsignedInt      opt_port = 104;
    OFCmdUnsignedInt      opt_repeatCount = 1;
    OFList<OFString>      overrideKeys;
    DcmTLSOptions         tlsOptions(NET_REQUESTOR);

#ifdef WITH_ZLIB
    OFCmdUnsignedInt      opt_compressionLevel = 0;
#endif

    /*
    ** By default. don't let "dcmdata" remove trailing padding or
    ** perform other manipulations. We want to see the real data.
    */
    OFBool                opt_automaticDataCorrection = OFFalse;

  OFStandard::initializeNetwork();
#ifdef WITH_OPENSSL
  DcmTLSTransportLayer::initializeOpenSSL();
#endif

  char tempstr[20];
  OFString temp_str;
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM query (C-FIND) SCU", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  cmd.addParam("peer", "hostname of DICOM peer");
  cmd.addParam("port", "tcp/ip port number of peer");
  cmd.addParam("dcmfile-in", "DICOM query file(s)", OFCmdParam::PM_MultiOptional);

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
   cmd.addOption("--help",                  "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",                          "print version information and exit", OFCommandLine::AF_Exclusive);
   OFLog::addOptions(cmd);

  cmd.addGroup("network options:");
    cmd.addSubGroup("override matching keys:");
      cmd.addOption("--key",                "-k",   1, "[k]ey: gggg,eeee=\"str\", path or dict. name=\"str\"",
                                                       "override matching key");
    cmd.addSubGroup("query information model:");
      cmd.addOption("--worklist",           "-W",      "use modality worklist information model (def.)");
      cmd.addOption("--patient",            "-P",      "use patient root information model");
      cmd.addOption("--study",              "-S",      "use study root information model");
      cmd.addOption("--psonly",             "-O",      "use patient/study only information model");
    cmd.addSubGroup("application entity titles:");
      OFString opt1 = "set my calling AE title (default: ";
      opt1 += APPLICATIONTITLE;
      opt1 += ")";
      cmd.addOption("--aetitle",            "-aet", 1, "[a]etitle: string", opt1.c_str());
      OFString opt2 = "set called AE title of peer (default: ";
      opt2 += PEERAPPLICATIONTITLE;
      opt2 += ")";
      cmd.addOption("--call",               "-aec", 1, "[a]etitle: string", opt2.c_str());
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",      "+u",      "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",     "-u",      "disable support for new VRs, convert to OB");
    cmd.addSubGroup("proposed transmission transfer syntaxes:");
      cmd.addOption("--propose-uncompr",    "-x=",     "propose all uncompressed TS, explicit VR\nwith local byte ordering first (default)");
      cmd.addOption("--propose-little",     "-xe",     "propose all uncompressed TS, explicit VR\nlittle endian first");
      cmd.addOption("--propose-big",        "-xb",     "propose all uncompressed TS, explicit VR\nbig endian first");
#ifdef WITH_ZLIB
      cmd.addOption("--propose-deflated",   "-xd",     "propose deflated explicit VR little endian TS\nand all uncompressed transfer syntaxes");
#endif
      cmd.addOption("--propose-implicit",   "-xi",     "propose implicit VR little endian TS only");
#ifdef WITH_ZLIB
    cmd.addSubGroup("deflate compression level (only with --propose-deflated):");
      cmd.addOption("--compression-level",  "+cl",  1, "[l]evel: integer (default: 6)",
                                                       "0=uncompressed, 1=fastest, 9=best compression");
#endif
    cmd.addSubGroup("other network options:");
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
      cmd.addOption("--timeout",            "-to",  1, "[s]econds: integer (default: unlimited)", "timeout for connection requests");
      cmd.addOption("--acse-timeout",       "-ta",  1, "[s]econds: integer (default: 30)", "timeout for ACSE messages");
      cmd.addOption("--dimse-timeout",      "-td",  1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");
      cmd.addOption("--max-pdu",            "-pdu", 1, opt4.c_str(), opt3.c_str());
      cmd.addOption("--repeat",                     1, "[n]umber: integer", "repeat n times");
      cmd.addOption("--abort",                         "abort association instead of releasing it");
      cmd.addOption("--cancel",                     1, "[n]umber: integer",
                                                       "cancel after n responses (default: never)");

  // add TLS specific command line options if (and only if) we are compiling with OpenSSL
  tlsOptions.addTLSCommandlineOptions(cmd);

  cmd.addGroup("output options:");
    cmd.addSubGroup("general:");
      cmd.addOption("--output-directory",   "-od",  1, "[d]irectory: string (default: \".\")",
                                                       "write output files to existing directory d");
    cmd.addSubGroup("automatic data correction:");
      cmd.addOption("--enable-correction",  "+dc",     "enable automatic data correction");
      cmd.addOption("--disable-correction", "-dc",     "disable automatic data correction (default)");
    cmd.addSubGroup("C-FIND responses:");
      cmd.addOption("--show-responses",     "+sr",     "always output responses to the logger");
      cmd.addOption("--hide-responses",     "-sr",     "do not output responses to the logger");
      cmd.addOption("--extract",            "-X",      "extract responses to DICOM file (rsp0001.dcm...)");
      cmd.addOption("--extract-xml",        "-Xx",     "extract responses to XML file (rsp0001.xml...)");
      cmd.addOption("--extract-xml-single", "-Xs",  1, "[f]ilename: string",
                                                       "extract all responses to given XML file f");

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
#if !defined(WITH_ZLIB) && !defined(WITH_OPENSSL) && !defined(DCMTK_ENABLE_CHARSET_CONVERSION)
          COUT << " none" << OFendl;
#else
          COUT << OFendl;
#endif
#ifdef WITH_ZLIB
          COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
          // print OpenSSL version if (and only if) we are compiling with OpenSSL
          tlsOptions.printLibraryVersion();
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
          COUT << "- " << OFCharacterEncoding::getLibraryVersionString() << OFendl;
#endif
          return 0;
        }

        // check if the command line contains the --list-ciphers option
        if (tlsOptions.listOfCiphersRequested(cmd))
        {
            tlsOptions.printSupportedCiphersuites(app, COUT);
            return 0;
        }
      }

      /* command line parameters */
      cmd.getParam(1, opt_peer);
      app.checkParam(cmd.getParamAndCheckMinMax(2, opt_port, 1, 65535));

      OFLog::configureFromCommandLine(cmd, app);

      if (cmd.findOption("--key", 0, OFCommandLine::FOM_FirstFromLeft))
      {
        const char *ovKey = NULL;
        do {
          app.checkValue(cmd.getValue(ovKey));
          overrideKeys.push_back(ovKey);
        } while (cmd.findOption("--key", 0, OFCommandLine::FOM_NextFromLeft));
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--worklist")) opt_abstractSyntax = UID_FINDModalityWorklistInformationModel;
      if (cmd.findOption("--patient"))  opt_abstractSyntax = UID_FINDPatientRootQueryRetrieveInformationModel;
      if (cmd.findOption("--study"))    opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;
      if (cmd.findOption("--psonly"))   opt_abstractSyntax = UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel;
      cmd.endOptionBlock();

      if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
      if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));

      cmd.beginOptionBlock();
      if (cmd.findOption("--propose-uncompr"))  opt_networkTransferSyntax = EXS_Unknown;
      if (cmd.findOption("--propose-little"))   opt_networkTransferSyntax = EXS_LittleEndianExplicit;
      if (cmd.findOption("--propose-big"))      opt_networkTransferSyntax = EXS_BigEndianExplicit;
      if (cmd.findOption("--propose-implicit")) opt_networkTransferSyntax = EXS_LittleEndianImplicit;
#ifdef WITH_ZLIB
      if (cmd.findOption("--propose-deflated")) opt_networkTransferSyntax = EXS_DeflatedLittleEndianExplicit;
#endif
      cmd.endOptionBlock();

#ifdef WITH_ZLIB
      if (cmd.findOption("--compression-level"))
      {
        app.checkDependence("--compression-level", "--propose-deflated",
          (opt_networkTransferSyntax == EXS_DeflatedLittleEndianExplicit));
        app.checkValue(cmd.getValueAndCheckMinMax(opt_compressionLevel, 0, 9));
        dcmZlibCompressionLevel.set(OFstatic_cast(int, opt_compressionLevel));
      }
#endif

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr")) dcmEnableGenerationOfNewVRs();
      if (cmd.findOption("--disable-new-vr")) dcmDisableGenerationOfNewVRs();
      cmd.endOptionBlock();

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

      if (cmd.findOption("--max-pdu")) app.checkValue(cmd.getValueAndCheckMinMax(opt_maxReceivePDULength, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
      if (cmd.findOption("--repeat"))  app.checkValue(cmd.getValueAndCheckMin(opt_repeatCount, 1));
      if (cmd.findOption("--abort"))   opt_abortAssociation = OFTrue;
      if (cmd.findOption("--cancel"))  app.checkValue(cmd.getValueAndCheckMin(opt_cancelAfterNResponses, 0));

      if (cmd.findOption("--output-directory")) app.checkValue(cmd.getValue(opt_outputDirectory));

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-correction")) opt_automaticDataCorrection = OFTrue;
      if (cmd.findOption("--disable-correction")) opt_automaticDataCorrection = OFFalse;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--show-responses")) opt_outputResponsesToLogger = 1;
      if (cmd.findOption("--hide-responses")) opt_outputResponsesToLogger = 2;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--extract")) opt_extractResponses = FEM_dicomFile;
      if (cmd.findOption("--extract-xml")) opt_extractResponses = FEM_xmlFile;
      if (cmd.findOption("--extract-xml-single"))
      {
          opt_extractResponses = FEM_singleXMLFile;
          app.checkValue(cmd.getValue(opt_extractXMLFilename));
      }
      cmd.endOptionBlock();

      /* finally parse filenames */
      int paramCount = cmd.getParamCount();
      const char *currentFilename = NULL;
      OFString errormsg;

      for (int i = 3; i <= paramCount; i++)
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

      // evaluate (most of) the TLS command line options (if we are compiling with OpenSSL)
      tlsOptions.parseArguments(app, cmd);
    }

    if (opt_outputResponsesToLogger == 0)
    {
      // default configuration for the C-FIND response logger
      if (!cmd.findOption("--log-config"))
      {
        if (cmd.findOption("--extract") || cmd.findOption("--extract-xml") || cmd.findOption("--extract-xml-single"))
        {
          OFLog::getLogger(DCMNET_LOGGER_NAME ".responses").setLogLevel(OFLogger::OFF_LOG_LEVEL);
        }
        else if (!cmd.findOption("--quiet") && !cmd.findOption("--verbose") && !cmd.findOption("--debug") && !cmd.findOption("--log-level"))
        {
          OFLog::getLogger(DCMNET_LOGGER_NAME ".responses").setLogLevel(OFLogger::INFO_LOG_LEVEL);
        }
      }
    }
    else if (opt_outputResponsesToLogger == 1)
    {
      // always show C-FIND responses
      OFLog::getLogger(DCMNET_LOGGER_NAME ".responses").setLogLevel(OFLogger::INFO_LOG_LEVEL);
    }
    else if (opt_outputResponsesToLogger == 2)
    {
      // never show C-FIND responses
      OFLog::getLogger(DCMNET_LOGGER_NAME ".responses").setLogLevel(OFLogger::OFF_LOG_LEVEL);
    }

    /* print resource identifier */
    OFLOG_DEBUG(findscuLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
      OFLOG_WARN(findscuLogger, "no data dictionary loaded, check environment variable: "
        << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* make sure that output directory can be used (if needed) */
    if (opt_extractResponses != FEM_none)
    {
      if (!OFStandard::dirExists(opt_outputDirectory))
      {
        OFLOG_FATAL(findscuLogger, "specified output directory does not exist");
        return 1;
      }
      else if (!OFStandard::isWriteable(opt_outputDirectory))
      {
        OFLOG_FATAL(findscuLogger, "specified output directory is not writeable");
        return 1;
      }
    }

    // enabled or disable removal of trailing padding
    dcmEnableAutomaticInputDataCorrection.set(opt_automaticDataCorrection);

    // declare findSCU handler and initialize network
    DcmFindSCU findscu;
    OFCondition cond = findscu.initializeNetwork(opt_acse_timeout);
    if (cond.bad()) {
      OFLOG_ERROR(findscuLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }

#ifdef WITH_OPENSSL
    if (tlsOptions.secureConnectionRequested())
    {
       /* create a secure transport layer */
       cond = tlsOptions.createTransportLayer(NULL, NULL, app, cmd);
       if (cond.bad())
       {
           OFLOG_FATAL(findscuLogger, DimseCondition::dump(temp_str, cond));
           return 1;
       }

       /* activate secure transport layer */
       cond = findscu.setTransportLayer(tlsOptions.getTransportLayer());
       if (cond.bad())
       {
           OFLOG_ERROR(findscuLogger, DimseCondition::dump(temp_str, cond));
           return 1;
       }
    }
#endif

    // do the main work: negotiate network association, perform C-FIND transaction,
    // process results, and finally tear down the association.
    cond = findscu.performQuery(
      opt_peer,
      opt_port,
      opt_ourTitle,
      opt_peerTitle,
      opt_abstractSyntax,
      opt_networkTransferSyntax,
      opt_blockMode,
      opt_dimse_timeout,
      opt_maxReceivePDULength,
      tlsOptions.secureConnectionRequested(),
      opt_abortAssociation,
      opt_repeatCount,
      opt_extractResponses,
      opt_cancelAfterNResponses,
      &overrideKeys,
      NULL, /* we want to use the default callback */
      &fileNameList,
      opt_outputDirectory.c_str(),
      opt_extractXMLFilename.c_str());

    // destroy network structure
    cond = findscu.dropNetwork();
    if (cond.bad()) OFLOG_ERROR(findscuLogger, DimseCondition::dump(temp_str, cond));

    OFStandard::shutdownNetwork();

    cond = tlsOptions.writeRandomSeed();
    if (cond.bad()) {
        // failure to write back the random seed is a warning, not an error
        OFLOG_WARN(findscuLogger, DimseCondition::dump(temp_str, cond));
    }

    return 0;
}
