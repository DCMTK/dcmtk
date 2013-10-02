/*
 *
 *  Copyright (C) 1994-2013, OFFIS e.V.
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

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dfindscu.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcostrmz.h"   /* for dcmZlibCompressionLevel */

#ifdef WITH_ZLIB
#include <zlib.h>     /* for zlibVersion() */
#endif

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#define OFFIS_CONSOLE_APPLICATION "findscu"

static OFLogger findscuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default application titles */
#define APPLICATIONTITLE        "FINDSCU"
#define PEERAPPLICATIONTITLE    "ANY-SCP"

#define SHORTCOL 4
#define LONGCOL 19

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
    OFBool                opt_extractResponsesToFile = OFFalse;
    OFString              opt_outputDirectory = ".";
    OFCmdUnsignedInt      opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
    E_TransferSyntax      opt_networkTransferSyntax = EXS_Unknown;
    const char *          opt_ourTitle = APPLICATIONTITLE;
    const char *          opt_peer;
    const char *          opt_peerTitle = PEERAPPLICATIONTITLE;
    OFCmdUnsignedInt      opt_port = 104;
    OFCmdUnsignedInt      opt_repeatCount = 1;
    OFBool                opt_secureConnection = OFFalse; /* default: no secure connection */
    OFList<OFString>      overrideKeys;

#ifdef WITH_ZLIB
    OFCmdUnsignedInt      opt_compressionLevel = 0;
#endif

#ifdef WITH_OPENSSL
    const char *          opt_certificateFile = NULL;
    OFBool                opt_doAuthenticate = OFFalse;
    int                   opt_keyFileFormat = SSL_FILETYPE_PEM;
    const char *          opt_passwd = NULL;
    const char *          opt_privateKeyFile = NULL;
#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
    OFString              opt_ciphersuites(TLS1_TXT_RSA_WITH_AES_128_SHA ":" SSL3_TXT_RSA_DES_192_CBC3_SHA);
#else
    OFString              opt_ciphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
#endif
    const char *          opt_dhparam = NULL;
    const char *          opt_readSeedFile = NULL;
    const char *          opt_writeSeedFile = NULL;
    DcmCertificateVerification  opt_certVerification = DCV_requireCertificate;
#endif

    /*
    ** Don't let dcmdata remove trailing blank padding or perform other
    ** manipulations.  We want to see the real data.
    */
    dcmEnableAutomaticInputDataCorrection.set(OFFalse);

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
   cmd.addOption("--help",                 "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",                         "print version information and exit", OFCommandLine::AF_Exclusive);
   OFLog::addOptions(cmd);

  cmd.addGroup("network options:");
    cmd.addSubGroup("override matching keys:");
      cmd.addOption("--key",               "-k",   1, "[k]ey: gggg,eeee=\"str\", path or dict. name=\"str\"",
                                                      "override matching key");
    cmd.addSubGroup("query information model:");
      cmd.addOption("--worklist",          "-W",      "use modality worklist information model (default)");
      cmd.addOption("--patient",           "-P",      "use patient root information model");
      cmd.addOption("--study",             "-S",      "use study root information model");
      cmd.addOption("--psonly",            "-O",      "use patient/study only information model");
    cmd.addSubGroup("application entity titles:");
      OFString opt1 = "set my calling AE title (default: ";
      opt1 += APPLICATIONTITLE;
      opt1 += ")";
      cmd.addOption("--aetitle",           "-aet", 1, "[a]etitle: string", opt1.c_str());
      OFString opt2 = "set called AE title of peer (default: ";
      opt2 += PEERAPPLICATIONTITLE;
      opt2 += ")";
      cmd.addOption("--call",              "-aec", 1, "[a]etitle: string", opt2.c_str());
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",     "+u",      "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",    "-u",      "disable support for new VRs, convert to OB");
    cmd.addSubGroup("proposed transmission transfer syntaxes:");
      cmd.addOption("--propose-uncompr",   "-x=",     "propose all uncompressed TS, explicit VR\nwith local byte ordering first (default)");
      cmd.addOption("--propose-little",    "-xe",     "propose all uncompressed TS, explicit VR\nlittle endian first");
      cmd.addOption("--propose-big",       "-xb",     "propose all uncompressed TS, explicit VR\nbig endian first");
#ifdef WITH_ZLIB
      cmd.addOption("--propose-deflated",  "-xd",     "propose deflated explicit VR little endian TS\nand all uncompressed transfer syntaxes");
#endif
      cmd.addOption("--propose-implicit",  "-xi",     "propose implicit VR little endian TS only");
#ifdef WITH_ZLIB
    cmd.addSubGroup("deflate compression level (only with --propose-deflated):");
      cmd.addOption("--compression-level", "+cl",  1, "[l]evel: integer (default: 6)",
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
      cmd.addOption("--timeout",           "-to",  1, "[s]econds: integer (default: unlimited)", "timeout for connection requests");
      cmd.addOption("--acse-timeout",      "-ta",  1, "[s]econds: integer (default: 30)", "timeout for ACSE messages");
      cmd.addOption("--dimse-timeout",     "-td",  1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");
      cmd.addOption("--max-pdu",           "-pdu", 1, opt4.c_str(), opt3.c_str());
      cmd.addOption("--repeat",                    1, "[n]umber: integer", "repeat n times");
      cmd.addOption("--abort",                        "abort association instead of releasing it");
      cmd.addOption("--cancel",                    1, "[n]umber: integer",
                                                      "cancel after n responses (default: never)");

#ifdef WITH_OPENSSL
  cmd.addGroup("transport layer security (TLS) options:");
    cmd.addSubGroup("transport protocol stack:");
      cmd.addOption("--disable-tls",       "-tls",    "use normal TCP/IP connection (default)");
      cmd.addOption("--enable-tls",        "+tls", 2, "[p]rivate key file, [c]ertificate file: string",
                                                      "use authenticated secure TLS connection");
      cmd.addOption("--anonymous-tls",     "+tla",    "use secure TLS connection without certificate");
    cmd.addSubGroup("private key password (only with --enable-tls):");
      cmd.addOption("--std-passwd",        "+ps",     "prompt user to type password on stdin (default)");
      cmd.addOption("--use-passwd",        "+pw",  1, "[p]assword: string ",
                                                      "use specified password");
      cmd.addOption("--null-passwd",       "-pw",     "use empty string as password");
    cmd.addSubGroup("key and certificate file format:");
      cmd.addOption("--pem-keys",          "-pem",    "read keys and certificates as PEM file (default)");
      cmd.addOption("--der-keys",          "-der",    "read keys and certificates as DER file");
    cmd.addSubGroup("certification authority:");
      cmd.addOption("--add-cert-file",     "+cf",  1, "[c]ertificate filename: string",
                                                      "add certificate file to list of certificates", OFCommandLine::AF_NoWarning);
      cmd.addOption("--add-cert-dir",      "+cd",  1, "[c]ertificate directory: string",
                                                      "add certificates in d to list of certificates", OFCommandLine::AF_NoWarning);
    cmd.addSubGroup("ciphersuite:");
      cmd.addOption("--cipher",            "+cs",  1, "[c]iphersuite name: string",
                                                      "add ciphersuite to list of negotiated suites");
      cmd.addOption("--dhparam",           "+dp",  1, "[f]ilename: string",
                                                      "read DH parameters for DH/DSS ciphersuites");
    cmd.addSubGroup("pseudo random generator:");
      cmd.addOption("--seed",              "+rs",  1, "[f]ilename: string",
                                                      "seed random generator with contents of f");
      cmd.addOption("--write-seed",        "+ws",     "write back modified seed (only with --seed)");
      cmd.addOption("--write-seed-file",   "+wf",  1, "[f]ilename: string (only with --seed)",
                                                      "write modified seed to file f");
    cmd.addSubGroup("peer authentication:");
      cmd.addOption("--require-peer-cert", "-rc",     "verify peer certificate, fail if absent (default)");
      cmd.addOption("--verify-peer-cert",  "-vc",     "verify peer certificate if present");
      cmd.addOption("--ignore-peer-cert",  "-ic",     "don't verify peer certificate");
#endif

  cmd.addGroup("output options:");
    cmd.addSubGroup("general:");
      cmd.addOption("--output-directory",  "-od",  1, "[d]irectory: string (default: \".\")", "write output files to existing directory d");
    cmd.addSubGroup("C-FIND responses:");
      cmd.addOption("--show-responses",    "+sr",     "always output responses to the logger");
      cmd.addOption("--hide-responses",    "-sr",     "do not output responses to the logger");
      cmd.addOption("--extract",           "-X",      "extract responses to file (rsp0001.dcm, ...)");

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
          COUT << "- " << OPENSSL_VERSION_TEXT << OFendl;
#endif
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
      if (cmd.findOption("--enable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFTrue);
        dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
        dcmEnableOtherFloatStringVRGeneration.set(OFTrue);
        dcmEnableOtherDoubleStringVRGeneration.set(OFTrue);
      }
      if (cmd.findOption("--disable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
        dcmEnableOtherFloatStringVRGeneration.set(OFFalse);
        dcmEnableOtherDoubleStringVRGeneration.set(OFFalse);
      }
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
      if (cmd.findOption("--show-responses")) opt_outputResponsesToLogger = 1;
      if (cmd.findOption("--hide-responses")) opt_outputResponsesToLogger = 2;
      cmd.endOptionBlock();

      if (cmd.findOption("--extract")) opt_extractResponsesToFile = OFTrue;
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

#ifdef WITH_OPENSSL

      cmd.beginOptionBlock();
      if (cmd.findOption("--disable-tls")) opt_secureConnection = OFFalse;
      if (cmd.findOption("--enable-tls"))
      {
        opt_secureConnection = OFTrue;
        opt_doAuthenticate = OFTrue;
        app.checkValue(cmd.getValue(opt_privateKeyFile));
        app.checkValue(cmd.getValue(opt_certificateFile));
      }
      if (cmd.findOption("--anonymous-tls"))
      {
        opt_secureConnection = OFTrue;
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--std-passwd"))
      {
        app.checkDependence("--std-passwd", "--enable-tls", opt_doAuthenticate);
        opt_passwd = NULL;
      }
      if (cmd.findOption("--use-passwd"))
      {
        app.checkDependence("--use-passwd", "--enable-tls", opt_doAuthenticate);
        app.checkValue(cmd.getValue(opt_passwd));
      }
      if (cmd.findOption("--null-passwd"))
      {
        app.checkDependence("--null-passwd", "--enable-tls", opt_doAuthenticate);
        opt_passwd = "";
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--pem-keys")) opt_keyFileFormat = SSL_FILETYPE_PEM;
      if (cmd.findOption("--der-keys")) opt_keyFileFormat = SSL_FILETYPE_ASN1;
      cmd.endOptionBlock();

      if (cmd.findOption("--dhparam"))
      {
        app.checkValue(cmd.getValue(opt_dhparam));
      }

      if (cmd.findOption("--seed"))
      {
        app.checkValue(cmd.getValue(opt_readSeedFile));
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--write-seed"))
      {
        app.checkDependence("--write-seed", "--seed", opt_readSeedFile != NULL);
        opt_writeSeedFile = opt_readSeedFile;
      }
      if (cmd.findOption("--write-seed-file"))
      {
        app.checkDependence("--write-seed-file", "--seed", opt_readSeedFile != NULL);
        app.checkValue(cmd.getValue(opt_writeSeedFile));
      }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--require-peer-cert")) opt_certVerification = DCV_requireCertificate;
      if (cmd.findOption("--verify-peer-cert"))  opt_certVerification = DCV_checkCertificate;
      if (cmd.findOption("--ignore-peer-cert"))  opt_certVerification = DCV_ignoreCertificate;
      cmd.endOptionBlock();

      const char *current = NULL;
      const char *currentOpenSSL;
      if (cmd.findOption("--cipher", 0, OFCommandLine::FOM_First))
      {
        opt_ciphersuites.clear();
        do
        {
          app.checkValue(cmd.getValue(current));
          if (NULL == (currentOpenSSL = DcmTLSTransportLayer::findOpenSSLCipherSuiteName(current)))
          {
            OFLOG_FATAL(findscuLogger, "ciphersuite '" << current << "' is unknown. Known ciphersuites are:");
            unsigned long numSuites = DcmTLSTransportLayer::getNumberOfCipherSuites();
            for (unsigned long cs=0; cs < numSuites; cs++)
            {
              OFLOG_FATAL(findscuLogger, "    " << DcmTLSTransportLayer::getTLSCipherSuiteName(cs));
            }
            return 1;
          } else {
            if (!opt_ciphersuites.empty()) opt_ciphersuites += ":";
            opt_ciphersuites += currentOpenSSL;
          }
        } while (cmd.findOption("--cipher", 0, OFCommandLine::FOM_Next));
      }

#endif

    }

    if (opt_outputResponsesToLogger == 0)
    {
      // default configuration for the C-FIND response logger
      if (!cmd.findOption("--log-config"))
      {
        if (cmd.findOption("--extract"))
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
    if (opt_extractResponsesToFile)
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

    // declare findSCU handler and initialize network
    DcmFindSCU findscu;
    OFCondition cond = findscu.initializeNetwork(opt_acse_timeout);
    if (cond.bad()) {
      OFLOG_ERROR(findscuLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }

#ifdef WITH_OPENSSL

    DcmTLSTransportLayer *tLayer = NULL;
    if (opt_secureConnection)
    {
      tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, opt_readSeedFile);
      if (tLayer == NULL)
      {
        OFLOG_FATAL(findscuLogger, "unable to create TLS transport layer");
        return 1;
      }

      if (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          if (TCS_ok != tLayer->addTrustedCertificateFile(current, opt_keyFileFormat))
          {
            OFLOG_WARN(findscuLogger, "unable to load certificate file '" << current << "', ignoring");
          }
        } while (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_Next));
      }

      if (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          if (TCS_ok != tLayer->addTrustedCertificateDir(current, opt_keyFileFormat))
          {
            OFLOG_WARN(findscuLogger, "unable to load certificates from directory '" << current << "', ignoring");
          }
        } while (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_Next));
      }

      if (opt_dhparam && ! (tLayer->setTempDHParameters(opt_dhparam)))
      {
        OFLOG_WARN(findscuLogger, "unable to load temporary DH parameter file '" << opt_dhparam << "', ignoring");
      }

      if (opt_doAuthenticate)
      {
        if (opt_passwd) tLayer->setPrivateKeyPasswd(opt_passwd);

        if (TCS_ok != tLayer->setPrivateKeyFile(opt_privateKeyFile, opt_keyFileFormat))
        {
          OFLOG_FATAL(findscuLogger, "unable to load private TLS key from '" << opt_privateKeyFile << "'");
          return 1;
        }
        if (TCS_ok != tLayer->setCertificateFile(opt_certificateFile, opt_keyFileFormat))
        {
          OFLOG_FATAL(findscuLogger, "unable to load certificate from '" << opt_certificateFile << "'");
          return 1;
        }
        if (! tLayer->checkPrivateKeyMatchesCertificate())
        {
          OFLOG_FATAL(findscuLogger, "private key '" << opt_privateKeyFile << "' and certificate '" << opt_certificateFile << "' do not match");
          return 1;
        }
      }

      if (TCS_ok != tLayer->setCipherSuites(opt_ciphersuites.c_str()))
      {
        OFLOG_FATAL(findscuLogger, "unable to set selected cipher suites");
        return 1;
      }

      tLayer->setCertificateVerification(opt_certVerification);

      cond = findscu.setTransportLayer(tLayer);
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
      opt_secureConnection,
      opt_abortAssociation,
      opt_repeatCount,
      opt_extractResponsesToFile,
      opt_cancelAfterNResponses,
      &overrideKeys,
      NULL, /* we want to use the default callback */
      &fileNameList,
      opt_outputDirectory.c_str());

    // destroy network structure
    cond = findscu.dropNetwork();
    if (cond.bad()) OFLOG_ERROR(findscuLogger, DimseCondition::dump(temp_str, cond));

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

#ifdef WITH_OPENSSL
    if (tLayer && opt_writeSeedFile)
    {
      if (tLayer->canWriteRandomSeed())
      {
        if (!tLayer->writeRandomSeed(opt_writeSeedFile))
        {
          OFLOG_ERROR(findscuLogger, "cannot write random seed file '" << opt_writeSeedFile << "', ignoring");
        }
      } else {
        OFLOG_ERROR(findscuLogger, "cannot write random seed, ignoring");
      }
    }
    delete tLayer;

#endif

    return 0;
}
