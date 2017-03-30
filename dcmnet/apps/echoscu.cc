/*
 *
 *  Copyright (C) 1994-2017, OFFIS e.V.
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
 *  Purpose: Verification Service Class User (C-ECHO operation)
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif

#ifdef PRIVATE_ECHOSCU_DECLARATIONS
PRIVATE_ECHOSCU_DECLARATIONS
#else
#define OFFIS_CONSOLE_APPLICATION "echoscu"
#endif

static OFLogger echoscuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default application titles */
#define APPLICATIONTITLE     "ECHOSCU"
#define PEERAPPLICATIONTITLE "ANY-SCP"

static T_DIMSE_BlockingMode opt_blockMode = DIMSE_BLOCKING;
static int opt_dimse_timeout = 0;

static OFCondition cecho(T_ASC_Association * assoc, unsigned long num_repeat);

/* DICOM standard transfer syntaxes */
static const char* transferSyntaxes[] = {
      UID_LittleEndianImplicitTransferSyntax, /* default xfer syntax first */
      UID_LittleEndianExplicitTransferSyntax,
      UID_BigEndianExplicitTransferSyntax,
      UID_JPEGProcess1TransferSyntax,
      UID_JPEGProcess2_4TransferSyntax,
      UID_JPEGProcess3_5TransferSyntax,
      UID_JPEGProcess6_8TransferSyntax,
      UID_JPEGProcess7_9TransferSyntax,
      UID_JPEGProcess10_12TransferSyntax,
      UID_JPEGProcess11_13TransferSyntax,
      UID_JPEGProcess14TransferSyntax,
      UID_JPEGProcess15TransferSyntax,
      UID_JPEGProcess16_18TransferSyntax,
      UID_JPEGProcess17_19TransferSyntax,
      UID_JPEGProcess20_22TransferSyntax,
      UID_JPEGProcess21_23TransferSyntax,
      UID_JPEGProcess24_26TransferSyntax,
      UID_JPEGProcess25_27TransferSyntax,
      UID_JPEGProcess28TransferSyntax,
      UID_JPEGProcess29TransferSyntax,
      UID_JPEGProcess14SV1TransferSyntax,
      UID_RLELosslessTransferSyntax,
      UID_DeflatedExplicitVRLittleEndianTransferSyntax,
      UID_JPEGLSLosslessTransferSyntax,
      UID_JPEGLSLossyTransferSyntax,
      UID_JPEG2000LosslessOnlyTransferSyntax,
      UID_JPEG2000TransferSyntax,
      UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax,
      UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax,
      UID_MPEG2MainProfileAtMainLevelTransferSyntax,
      UID_MPEG2MainProfileAtHighLevelTransferSyntax,
      UID_MPEG4HighProfileLevel4_1TransferSyntax,
      UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax,
      UID_MPEG4HighProfileLevel4_2_For2DVideoTransferSyntax,
      UID_MPEG4HighProfileLevel4_2_For3DVideoTransferSyntax,
      UID_MPEG4StereoHighProfileLevel4_2TransferSyntax,
      UID_HEVCMainProfileLevel5_1TransferSyntax,
      UID_HEVCMain10ProfileLevel5_1TransferSyntax
};

// ********************************************

#define SHORTCOL 4
#define LONGCOL 19

int
main(int argc, char *argv[])
{
    const char *     opt_peer                = NULL;
    OFCmdUnsignedInt opt_port                = 104;
    const char *     opt_peerTitle           = PEERAPPLICATIONTITLE;
    const char *     opt_ourTitle            = APPLICATIONTITLE;
    OFCmdUnsignedInt opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
    OFCmdUnsignedInt opt_repeatCount         = 1;
    OFBool           opt_abortAssociation    = OFFalse;
    OFCmdUnsignedInt opt_numXferSyntaxes     = 1;
    OFCmdUnsignedInt opt_numPresentationCtx  = 1;
    OFCmdUnsignedInt maxXferSyntaxes         = OFstatic_cast(OFCmdUnsignedInt, (DIM_OF(transferSyntaxes)));
    OFBool           opt_secureConnection    = OFFalse; /* default: no secure connection */
    int opt_acse_timeout = 30;

#ifdef WITH_OPENSSL
    int         opt_keyFileFormat = SSL_FILETYPE_PEM;
    OFBool      opt_doAuthenticate = OFFalse;
    const char *opt_privateKeyFile = NULL;
    const char *opt_certificateFile = NULL;
    const char *opt_passwd = NULL;
#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
    OFString    opt_ciphersuites(TLS1_TXT_RSA_WITH_AES_128_SHA ":" SSL3_TXT_RSA_DES_192_CBC3_SHA);
#else
    OFString    opt_ciphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
#endif
    const char *opt_readSeedFile = NULL;
    const char *opt_writeSeedFile = NULL;
    DcmCertificateVerification opt_certVerification = DCV_requireCertificate;
    const char *opt_dhparam = NULL;
#endif

    T_ASC_Network *net;
    T_ASC_Parameters *params;
    DIC_NODENAME peerHost;
    T_ASC_Association *assoc;
    OFString temp_str;

  OFStandard::initializeNetwork();

  char tempstr[20];
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM verification (C-ECHO) SCU", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  cmd.addParam("peer", "hostname of DICOM peer");
  cmd.addParam("port", "tcp/ip port number of peer");

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
   cmd.addOption("--help",                 "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",                         "print version information and exit", OFCommandLine::AF_Exclusive);
   OFLog::addOptions(cmd);

  cmd.addGroup("network options:");
    cmd.addSubGroup("application entity titles:");
      OFString opt1 = "set my calling AE title (default: ";
      opt1 += APPLICATIONTITLE;
      opt1 += ")";
      cmd.addOption("--aetitle",           "-aet", 1, "[a]etitle: string", opt1.c_str());
      OFString opt2 = "set called AE title of peer (default: ";
      opt2 += PEERAPPLICATIONTITLE;
      opt2 += ")";
      cmd.addOption("--call",              "-aec", 1, "[a]etitle: string", opt2.c_str());
    cmd.addSubGroup("association negotiation debugging:");
      OFString opt5 = "[n]umber: integer (1..";
      sprintf(tempstr, "%ld", OFstatic_cast(long, maxXferSyntaxes));
      opt5 += tempstr;
      opt5 += ")";
      cmd.addOption("--propose-ts",        "-pts", 1, opt5.c_str(), "propose n transfer syntaxes");
      cmd.addOption("--propose-pc",        "-ppc", 1, "[n]umber: integer (1..128)", "propose n presentation contexts");

    cmd.addSubGroup("other network options:");
      cmd.addOption("--timeout",           "-to",  1, "[s]econds: integer (default: unlimited)", "timeout for connection requests");
      cmd.addOption("--acse-timeout",      "-ta",  1, "[s]econds: integer (default: 30)", "timeout for ACSE messages");
      cmd.addOption("--dimse-timeout",     "-td",  1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");

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
      cmd.addOption("--max-pdu",           "-pdu", 1, opt4.c_str(), opt3.c_str());
      cmd.addOption("--repeat",                    1, "[n]umber: integer", "repeat n times");
      cmd.addOption("--abort",                        "abort association instead of releasing it");

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

      if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
      if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));

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
      if (cmd.findOption("--repeat")) app.checkValue(cmd.getValueAndCheckMin(opt_repeatCount, 1));
      if (cmd.findOption("--abort")) opt_abortAssociation=OFTrue;
      if (cmd.findOption("--propose-ts")) app.checkValue(cmd.getValueAndCheckMinMax(opt_numXferSyntaxes, 1, maxXferSyntaxes));
      if (cmd.findOption("--propose-pc")) app.checkValue(cmd.getValueAndCheckMinMax(opt_numPresentationCtx, 1, 128));

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
            OFLOG_FATAL(echoscuLogger, "ciphersuite '" << current << "' is unknown. Known ciphersuites are:");
            unsigned long numSuites = DcmTLSTransportLayer::getNumberOfCipherSuites();
            for (unsigned long cs=0; cs < numSuites; cs++)
            {
              OFLOG_FATAL(echoscuLogger, "    " << DcmTLSTransportLayer::getTLSCipherSuiteName(cs));
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

    /* print resource identifier */
    OFLOG_DEBUG(echoscuLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(echoscuLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* initialize network, i.e. create an instance of T_ASC_Network*. */
    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, opt_acse_timeout, &net);
    if (cond.bad()) {
        OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }

#ifdef WITH_OPENSSL

    DcmTLSTransportLayer *tLayer = NULL;
    if (opt_secureConnection)
    {
      tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, opt_readSeedFile);
      if (tLayer == NULL)
      {
        OFLOG_FATAL(echoscuLogger, "unable to create TLS transport layer");
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
              OFLOG_WARN(echoscuLogger, "unable to load certificate file '" << current << "', ignoring");
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
            OFLOG_WARN(echoscuLogger, "unable to load certificates from directory '" << current << "', ignoring");
          }
        } while (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_Next));
      }

      if (opt_dhparam && ! (tLayer->setTempDHParameters(opt_dhparam)))
      {
        OFLOG_WARN(echoscuLogger, "unable to load temporary DH parameter file '" << opt_dhparam << "', ignoring");
      }

      if (opt_doAuthenticate)
      {
        if (opt_passwd) tLayer->setPrivateKeyPasswd(opt_passwd);

        if (TCS_ok != tLayer->setPrivateKeyFile(opt_privateKeyFile, opt_keyFileFormat))
        {
          OFLOG_ERROR(echoscuLogger, "unable to load private TLS key from '" << opt_privateKeyFile << "'");
          return 1;
        }
        if (TCS_ok != tLayer->setCertificateFile(opt_certificateFile, opt_keyFileFormat))
        {
          OFLOG_ERROR(echoscuLogger, "unable to load certificate from '" << opt_certificateFile << "'");
          return 1;
        }
        if (! tLayer->checkPrivateKeyMatchesCertificate())
        {
          OFLOG_ERROR(echoscuLogger, "private key '" << opt_privateKeyFile << "' and certificate '" << opt_certificateFile << "' do not match");
          return 1;
        }
      }

      if (TCS_ok != tLayer->setCipherSuites(opt_ciphersuites.c_str()))
      {
        OFLOG_ERROR(echoscuLogger, "unable to set selected cipher suites");
        return 1;
      }

      tLayer->setCertificateVerification(opt_certVerification);


      cond = ASC_setTransportLayer(net, tLayer, 0);
      if (cond.bad())
      {
          OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
          return 1;
      }
    }

#endif

    /* initialize association parameters, i.e. create an instance of T_ASC_Parameters*. */
    cond = ASC_createAssociationParameters(&params, opt_maxReceivePDULength);
    if (cond.bad()) {
        OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }

#ifdef PRIVATE_ECHOSCU_CODE
    PRIVATE_ECHOSCU_CODE
#endif

    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "STORESCU" and "ANY-SCP". */
    ASC_setAPTitles(params, opt_ourTitle, opt_peerTitle, NULL);

    /* Set the transport layer type (type of network connection) in the params */
    /* structure. The default is an insecure connection; where OpenSSL is  */
    /* available the user is able to request an encrypted,secure connection. */
    cond = ASC_setTransportLayerType(params, opt_secureConnection);
    if (cond.bad()) {
        OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
        return 1;
    }

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    sprintf(peerHost, "%s:%d", opt_peer, OFstatic_cast(int, opt_port));
    ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), peerHost);

    /* Set the presentation contexts which will be negotiated */
    /* when the network connection will be established */
    int presentationContextID = 1; /* odd byte value 1, 3, 5, .. 255 */
    for (unsigned long ii=0; ii<opt_numPresentationCtx; ii++)
    {
        cond = ASC_addPresentationContext(params, presentationContextID, UID_VerificationSOPClass,
                 transferSyntaxes, OFstatic_cast(int, opt_numXferSyntaxes));
        presentationContextID += 2;
        if (cond.bad())
        {
            OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
            exit(1);
        }
    }

    /* dump presentation contexts if required */
    OFLOG_DEBUG(echoscuLogger, "Request Parameters:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ));

    /* create association, i.e. try to establish a network connection to another */
    /* DICOM application. This call creates an instance of T_ASC_Association*. */
    OFLOG_INFO(echoscuLogger, "Requesting Association");
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED)
        {
            T_ASC_RejectParameters rej;

            ASC_getRejectParameters(params, &rej);
            OFLOG_FATAL(echoscuLogger, "Association Rejected:" << OFendl << ASC_printRejectParameters(temp_str, &rej));
            exit(1);
        } else {
            OFLOG_FATAL(echoscuLogger, "Association Request Failed: " << DimseCondition::dump(temp_str, cond));
            exit(1);
        }
    }

    /* dump the presentation contexts which have been accepted/refused */
    OFLOG_DEBUG(echoscuLogger, "Association Parameters Negotiated:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC));

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        OFLOG_FATAL(echoscuLogger, "No Acceptable Presentation Contexts");
        exit(1);
    }

    /* dump general information concerning the establishment of the network connection if required */
    OFLOG_INFO(echoscuLogger, "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")");

    /* do the real work, i.e. send a number of C-ECHO-RQ messages to the DICOM application */
    /* this application is connected with and handle corresponding C-ECHO-RSP messages. */
    cond = cecho(assoc, opt_repeatCount);

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
        if (opt_abortAssociation) {
            OFLOG_INFO(echoscuLogger, "Aborting Association");
            cond = ASC_abortAssociation(assoc);
            if (cond.bad())
            {
                OFLOG_FATAL(echoscuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
                exit(1);
            }
        } else {
            /* release association */
            OFLOG_INFO(echoscuLogger, "Releasing Association");
            cond = ASC_releaseAssociation(assoc);
            if (cond.bad())
            {
                OFLOG_FATAL(echoscuLogger, "Association Release Failed: " << DimseCondition::dump(temp_str, cond));
                exit(1);
            }
        }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        OFLOG_FATAL(echoscuLogger, "Protocol Error: Peer requested release (Aborting)");
        OFLOG_INFO(echoscuLogger, "Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            OFLOG_FATAL(echoscuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
            exit(1);
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        OFLOG_INFO(echoscuLogger, "Peer Aborted Association");
    }
    else
    {
        OFLOG_ERROR(echoscuLogger, "Echo SCU Failed: " << DimseCondition::dump(temp_str, cond));
        OFLOG_INFO(echoscuLogger, "Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            OFLOG_FATAL(echoscuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
            exit(1);
        }
    }

    /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
    /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad()) {
        OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }

    /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
    /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
    cond = ASC_dropNetwork(&net);
    if (cond.bad()) {
        OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }

    OFStandard::shutdownNetwork();

#ifdef WITH_OPENSSL
    if (tLayer && opt_writeSeedFile)
    {
      if (tLayer->canWriteRandomSeed())
      {
        if (!tLayer->writeRandomSeed(opt_writeSeedFile))
        {
          OFLOG_ERROR(echoscuLogger, "cannot write random seed file '" << opt_writeSeedFile << "', ignoring");
        }
      } else {
        OFLOG_ERROR(echoscuLogger, "cannot write random seed, ignoring");
      }
    }
    delete tLayer;
#endif

    return 0;
}

static OFCondition
echoSCU(T_ASC_Association * assoc)
    /*
     * This function will send a C-ECHO-RQ over the network to another DICOM application
     * and handle the response.
     *
     * Parameters:
     *   assoc - [in] The association (network connection to another DICOM application).
     */
{
    DIC_US msgId = assoc->nextMsgID++;
    DIC_US status;
    DcmDataset *statusDetail = NULL;

    /* dump information if required */
    OFLOG_INFO(echoscuLogger, "Sending Echo Request (MsgID " << msgId << ")");

    /* send C-ECHO-RQ and handle response */
    OFCondition cond = DIMSE_echoUser(assoc, msgId, opt_blockMode, opt_dimse_timeout, &status, &statusDetail);

    /* depending on if a response was received, dump some information */
    if (cond.good()) {
        OFLOG_INFO(echoscuLogger, "Received Echo Response (" << DU_cechoStatusString(status) << ")");
    } else {
        OFString temp_str;
        OFLOG_ERROR(echoscuLogger, "Echo Failed: " << DimseCondition::dump(temp_str, cond));
    }

    /* check for status detail information, there should never be any */
    if (statusDetail != NULL) {
        OFLOG_DEBUG(echoscuLogger, "Status Detail (should never be any):" << OFendl << DcmObject::PrintHelper(*statusDetail));
        delete statusDetail;
    }

    /* return result value */
    return cond;
}

static OFCondition
cecho(T_ASC_Association * assoc, unsigned long num_repeat)
    /*
     * This function will send num_repeat C-ECHO-RQ messages to the DICOM application
     * this application is connected with and handle corresponding C-ECHO-RSP messages.
     *
     * Parameters:
     *   assoc      - [in] The association (network connection to another DICOM application).
     *   num_repeat - [in] The amount of C-ECHO-RQ messages which shall be sent.
     */
{
    OFCondition cond = EC_Normal;
    unsigned long n = num_repeat;

    /* as long as no error occurred and the counter does not equal 0 */
    /* send an C-ECHO-RQ and handle the response */
    while (cond.good() && n--) cond = echoSCU(assoc);

    return cond;
}
