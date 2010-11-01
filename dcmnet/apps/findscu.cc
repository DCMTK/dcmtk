/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 10:42:44 $
 *  CVS/RCS Revision: $Revision: 1.62 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dfindscu.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcdict.h"

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
    OFBool                opt_extractResponsesToFile = OFFalse;
    OFCmdUnsignedInt      opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
    E_TransferSyntax      opt_networkTransferSyntax = EXS_Unknown;
    const char *          opt_ourTitle = APPLICATIONTITLE;
    const char *          opt_peer;
    const char *          opt_peerTitle = PEERAPPLICATIONTITLE;
    OFCmdUnsignedInt      opt_port = 104;
    OFCmdUnsignedInt      opt_repeatCount = 1;
    OFBool                opt_secureConnection = OFFalse; /* default: no secure connection */
    OFList<OFString>      overrideKeys;

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
    ** Don't let dcmdata remove tailing blank padding or perform other
    ** maipulations.  We want to see the real data.
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
      cmd.addOption("--propose-implicit",  "-xi",     "propose implicit VR little endian TS only");
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
      cmd.addOption("--extract",           "-X",      "extract responses to file (rsp0001.dcm, ...)");

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

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
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
      if (cmd.findOption("--propose-uncompr"))  opt_networkTransferSyntax = EXS_Unknown;
      if (cmd.findOption("--propose-little"))   opt_networkTransferSyntax = EXS_LittleEndianExplicit;
      if (cmd.findOption("--propose-big"))      opt_networkTransferSyntax = EXS_BigEndianExplicit;
      if (cmd.findOption("--propose-implicit")) opt_networkTransferSyntax = EXS_LittleEndianImplicit;
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--worklist")) opt_abstractSyntax = UID_FINDModalityWorklistInformationModel;
      if (cmd.findOption("--patient"))  opt_abstractSyntax = UID_FINDPatientRootQueryRetrieveInformationModel;
      if (cmd.findOption("--study"))    opt_abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;
      if (cmd.findOption("--psonly"))   opt_abstractSyntax = UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel;
      cmd.endOptionBlock();

      if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
      if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));

      cmd.beginOptionBlock();
      if (cmd.findOption("--enable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFTrue);
        dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
      }
      if (cmd.findOption("--disable-new-vr"))
      {
        dcmEnableUnknownVRGeneration.set(OFFalse);
        dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
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
            if (opt_ciphersuites.length() > 0) opt_ciphersuites += ":";
            opt_ciphersuites += currentOpenSSL;
          }
        } while (cmd.findOption("--cipher", 0, OFCommandLine::FOM_Next));
      }

#endif

    }

    /* print resource identifier */
    OFLOG_DEBUG(findscuLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(findscuLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
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
      &fileNameList);

    if (cond.bad()) OFLOG_ERROR(findscuLogger, DimseCondition::dump(temp_str, cond));

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

/*
** CVS Log
** $Log: findscu.cc,v $
** Revision 1.62  2010-11-01 10:42:44  uli
** Fixed some compiler warnings reported by gcc with additional flags.
**
** Revision 1.61  2010-10-14 13:13:42  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.60  2010-09-24 13:30:30  joergr
** Compared names of SOP Class UIDs with 2009 edition of the DICOM standard. The
** resulting name changes are mainly caused by the fact that the corresponding
** SOP Class is now retired.
**
** Revision 1.59  2009-12-01 11:26:19  joergr
** Use helper function checkDependence() where appropriate.
** Fixed issue with syntax usage (e.g. layout and formatting).
** Sightly modified log messages.
**
** Revision 1.58  2009-11-18 11:53:58  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.57  2009-07-08 16:14:08  onken
** Added support for specifying tag paths as override keys.
**
** Revision 1.56  2009-06-04 10:16:24  joergr
** Added new flag that can be used to avoid wrong warning messages (in debug
** mode) that an option has possibly never been checked.
**
** Revision 1.55  2009-04-24 12:26:05  joergr
** Fixed minor inconsistencies regarding layout/formatting in syntax usage.
**
** Revision 1.54  2009-04-21 14:09:22  joergr
** Fixed minor inconsistencies in manpage / syntax usage.
**
** Revision 1.53  2008-09-25 16:00:58  joergr
** Added support for printing the expanded command line arguments.
** Always output the resource identifier of the command line tool in debug mode.
**
** Revision 1.52  2008-07-10 10:59:27  joergr
** Fixed layout of the usage output (--help).
**
** Revision 1.51  2007/02/19 13:13:26  meichel
** Refactored findscu code into class DcmFindSCU, which is now part of the dcmnet
**   library, and a short command line tool that only evaluates command line
**   parameters and then makes use of this class. This facilitates re-use of the
**   findscu code in other applications.
**
** Revision 1.50  2006/08/15 16:04:28  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.49  2006/07/27 14:20:53  joergr
** Changed parameter "exclusive" of method addOption() from type OFBool into an
** integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
** Option "--help" is no longer an exclusive option by default.
** Made naming conventions for command line parameters more consistent, e.g.
** used "dcmfile-in", "dcmfile-out" and "bitmap-out".
**
** Revision 1.48  2006/01/17 15:38:50  onken
** Fixed "--key" option, which was broken when using the optional assignment ("=")
** operation inside the option value
**
** Revision 1.47  2005/12/08 15:44:19  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.46  2005/11/23 16:10:23  meichel
** Added support for AES ciphersuites in TLS module. All TLS-enabled
**   tools now support the "AES TLS Secure Transport Connection Profile".
**
** Revision 1.45  2005/11/17 13:45:16  meichel
** Added command line options for DIMSE and ACSE timeouts
**
** Revision 1.44  2005/11/16 14:58:07  meichel
** Set association timeout in ASC_initializeNetwork to 30 seconds. This improves
**   the responsiveness of the tools if the peer blocks during assoc negotiation.
**
** Revision 1.43  2005/11/14 09:06:50  onken
** Added data dictionary name support for "--key" option
**
** Revision 1.42  2005/11/03 17:39:41  meichel
** Added transfer syntax selection options to findscu.
**
** Revision 1.41  2004/02/27 12:51:51  meichel
** Added --cancel option to findscu, similar to the option available in movescu.
**
** Revision 1.40  2004/02/26 17:17:43  meichel
** Fixed minor memory leak in findscu
**
** Revision 1.39  2003/03/14 15:57:34  meichel
** Enabled TLS network connections in findscu
**
** Revision 1.38  2002/11/29 09:15:49  meichel
** Introduced new command line option --timeout for controlling the
**   connection request timeout.
**
** Revision 1.37  2002/11/27 13:04:30  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.36  2002/11/26 08:43:20  meichel
** Replaced all includes for "zlib.h" with <zlib.h>
**   to avoid inclusion of zlib.h in the makefile dependencies.
**
** Revision 1.35  2002/09/23 17:53:46  joergr
** Added new command line option "--version" which prints the name and version
** number of external libraries used (incl. preparation for future support of
** 'config.guess' host identifiers).
**
** Revision 1.34  2002/08/20 12:21:21  meichel
** Adapted code to new loadFile and saveFile methods, thus removing direct
**   use of the DICOM stream classes.
**
** Revision 1.33  2001/11/09 15:56:23  joergr
** Renamed some of the getValue/getParam methods to avoid ambiguities reported
** by certain compilers.
**
** Revision 1.32  2001/11/01 14:38:58  wilkens
** Added lots of comments.
**
** Revision 1.31  2001/10/12 10:18:20  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.30  2001/09/26 12:28:54  meichel
** Implemented changes in dcmnet required by the adaptation of dcmdata
**   to class OFCondition.  Removed some unused code.
**
** Revision 1.29  2001/06/01 15:50:01  meichel
** Updated copyright header
**
** Revision 1.28  2000/06/07 13:56:16  meichel
** Output stream now passed as mandatory parameter to ASC_dumpParameters.
**
** Revision 1.27  2000/04/14 16:29:26  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.26  2000/03/08 16:43:15  meichel
** Updated copyright header.
**
** Revision 1.25  2000/03/03 14:11:11  meichel
** iImplemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.24  2000/02/29 11:49:49  meichel
** Removed support for VS value representation. This was proposed in CP 101
**   but never became part of the standard.
**
** Revision 1.23  2000/02/23 15:12:19  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.22  2000/02/01 10:24:02  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.21  1999/04/30 16:40:21  meichel
** Minor code purifications to keep Sun CC 2.0.1 quiet
**
** Revision 1.20  1999/04/29 10:02:45  meichel
** Adapted findscu to new command line option scheme
**
** Revision 1.19  1999/04/19 08:43:53  meichel
** Added new option to findscu which allows to extract all
**   C-FIND-RSP messages to file as received over network.
**
** Revision 1.18  1999/03/29 11:19:53  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.17  1998/08/10 08:53:34  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.16  1998/01/14 14:35:53  hewett
** Modified existing -u command line option to also disable generation
** of UT and VS (previously just disabled generation of UN).
**
** Revision 1.15  1997/08/05 07:46:20  andreas
** - Change needed version number of WINSOCK to 1.1
**   to support WINDOWS 95
**
** Revision 1.14  1997/07/21 08:37:02  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.13  1997/05/30 07:33:21  meichel
** Added space characters around comments and simplified
** some inlining code (needed for SunCC 2.0.1).
**
** Revision 1.12  1997/05/29 15:52:56  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.11  1997/05/22 13:29:58  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.10  1997/04/18 08:40:11  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.9  1997/03/27 16:11:26  hewett
** Added command line switches allowing generation of UN to
** be disabled (it is enabled by default).
**
** Revision 1.8  1996/12/16 15:13:59  hewett
** Added bugfix for WINSOCK support.  The required WINSOCK version
** number was being incorrectly set to version 0.1.  The fixed
** WINSOCK initialisation now uses the MAKEWORD macro to correctly
** set the required version number. This bugfix was contributed
** by Dr. Yongjian Bao of Innomed GmbH, Germany.
**
** Revision 1.7  1996/09/27 14:05:03  hewett
** Added calls to initialise WINSOCK library for Win32 environment.  Only
** compiled in if HAVE_WINSOCK_H
**
** Revision 1.6  1996/09/27 08:24:28  hewett
** System header files now enclosed with BEGIN_EXTERN_C/END_EXTERN_C
**
** Revision 1.5  1996/09/24 16:21:49  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.4  1996/08/29 16:24:07  hewett
** Added copyright header.
**
** Revision 1.3  1996/06/20 07:23:40  hewett
** Corrected typo in printf.
**
** Revision 1.2  1996/04/25 16:19:17  hewett
** Added char* parameter casts to bzero() calls.
**
** Revision 1.1  1996/04/22 09:58:16  hewett
** Initial release.
**
**
*/
