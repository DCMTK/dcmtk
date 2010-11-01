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
 *  Purpose: Verification Service Class User (C-ECHO operation)
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 10:42:44 $
 *  CVS/RCS Revision: $Revision: 1.53 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
      UID_JPEGLSLosslessTransferSyntax,
      UID_JPEGLSLossyTransferSyntax,
      UID_DeflatedExplicitVRLittleEndianTransferSyntax,
      UID_JPEG2000LosslessOnlyTransferSyntax,
      UID_JPEG2000TransferSyntax,
      UID_MPEG2MainProfileAtMainLevelTransferSyntax,
      UID_MPEG2MainProfileAtHighLevelTransferSyntax,
      UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax,
      UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax
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
    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;
    T_ASC_Association *assoc;
    OFString temp_str;

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
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
            if (opt_ciphersuites.length() > 0) opt_ciphersuites += ":";
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

    /* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
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
    /* strucutre. The default is an insecure connection; where OpenSSL is  */
    /* available the user is able to request an encrypted,secure connection. */
    cond = ASC_setTransportLayerType(params, opt_secureConnection);
    if (cond.bad()) {
        OFLOG_FATAL(echoscuLogger, DimseCondition::dump(temp_str, cond));
        return 1;
    }

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", opt_peer, OFstatic_cast(int, opt_port));
    ASC_setPresentationAddresses(params, localHost, peerHost);

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
    OFLOG_INFO(echoscuLogger, "Sending Echo Request: MsgID " << msgId);

    /* send C-ECHO-RQ and handle response */
    OFCondition cond = DIMSE_echoUser(assoc, msgId, opt_blockMode, opt_dimse_timeout, &status, &statusDetail);

    /* depending on if a response was received, dump some information */
    if (cond.good()) {
        OFLOG_INFO(echoscuLogger, "Received Echo Response (Status: " << DU_cstoreStatusString(status) << ")");
    } else {
        OFString temp_str;
        OFLOG_ERROR(echoscuLogger, "Echo Failed: " << DimseCondition::dump(temp_str, cond));
    }

    /* check for status detail information, there should never be any */
    if (statusDetail != NULL) {
        OFLOG_INFO(echoscuLogger, "Status Detail (should never be any):" << OFendl << DcmObject::PrintHelper(*statusDetail));
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

    /* as long as no error occured and the counter does not equal 0 */
    /* send an C-ECHO-RQ and handle the response */
    while (cond.good() && n--) cond = echoSCU(assoc);

    return cond;
}

/*
** CVS Log
** $Log: echoscu.cc,v $
** Revision 1.53  2010-11-01 10:42:44  uli
** Fixed some compiler warnings reported by gcc with additional flags.
**
** Revision 1.52  2010-10-14 13:13:42  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.51  2010-09-09 08:34:09  joergr
** Added support for "MPEG2 Main Profile @ High Level" transfer syntax.
**
** Revision 1.50  2010-05-18 16:10:25  joergr
** Replaced '\n' by OFendl in log messages.
**
** Revision 1.49  2009-12-01 10:59:38  joergr
** Use helper function checkDependence() where appropriate.
** Sightly modified log messages.
**
** Revision 1.48  2009-11-18 11:53:58  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.47  2009-09-04 14:36:42  joergr
** Output all --version information to COUT (and not to CERR).
**
** Revision 1.46  2009-07-13 09:44:18  onken
** Removed misleading comment about dcmnet DIMSE return code and changed
** corresponding OFCondition check from EC_Normal to .good().
**
** Revision 1.45  2009-06-04 10:16:24  joergr
** Added new flag that can be used to avoid wrong warning messages (in debug
** mode) that an option has possibly never been checked.
**
** Revision 1.44  2009-04-24 12:26:05  joergr
** Fixed minor inconsistencies regarding layout/formatting in syntax usage.
**
** Revision 1.43  2009-04-21 14:09:22  joergr
** Fixed minor inconsistencies in manpage / syntax usage.
**
** Revision 1.42  2008-09-25 16:00:58  joergr
** Added support for printing the expanded command line arguments.
** Always output the resource identifier of the command line tool in debug mode.
**
** Revision 1.41  2006/08/15 16:04:28  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.40  2006/07/27 14:20:08  joergr
** Changed parameter "exclusive" of method addOption() from type OFBool into an
** integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
** Option "--help" is no longer an exclusive option by default.
**
** Revision 1.39  2005/12/08 15:44:18  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.38  2005/11/23 16:10:23  meichel
** Added support for AES ciphersuites in TLS module. All TLS-enabled
**   tools now support the "AES TLS Secure Transport Connection Profile".
**
** Revision 1.37  2005/11/17 13:45:16  meichel
** Added command line options for DIMSE and ACSE timeouts
**
** Revision 1.36  2005/11/16 14:58:07  meichel
** Set association timeout in ASC_initializeNetwork to 30 seconds. This improves
**   the responsiveness of the tools if the peer blocks during assoc negotiation.
**
** Revision 1.35  2005/10/25 08:55:43  meichel
** Updated list of UIDs and added support for new transfer syntaxes
**   and storage SOP classes.
**
** Revision 1.34  2004/04/06 18:12:26  joergr
** Added missing suffix "TransferSyntax" to some transfer syntax constants.
** Added six more transfer syntaxes to the --propose-ts option.
**
** Revision 1.33  2004/02/25 12:18:06  meichel
** Added a few dummy macros allowing for future private extensions
**
** Revision 1.32  2002/12/13 15:42:43  meichel
** Enabled TLS network connections in echoscu
**
** Revision 1.31  2002/11/29 09:15:49  meichel
** Introduced new command line option --timeout for controlling the
**   connection request timeout.
**
** Revision 1.30  2002/11/28 16:58:35  meichel
** Introduced new command line option --timeout for controlling the
**   connection request timeout.
**
** Revision 1.29  2002/11/27 13:04:29  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.28  2002/11/26 08:43:19  meichel
** Replaced all includes for "zlib.h" with <zlib.h>
**   to avoid inclusion of zlib.h in the makefile dependencies.
**
** Revision 1.27  2002/09/23 17:53:46  joergr
** Added new command line option "--version" which prints the name and version
** number of external libraries used (incl. preparation for future support of
** 'config.guess' host identifiers).
**
** Revision 1.26  2001/11/09 15:56:22  joergr
** Renamed some of the getValue/getParam methods to avoid ambiguities reported
** by certain compilers.
**
** Revision 1.25  2001/11/06 14:19:21  wilkens
** Added more comments.
**
** Revision 1.24  2001/11/01 14:38:57  wilkens
** Added lots of comments.
**
** Revision 1.23  2001/10/12 10:18:20  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.22  2001/06/01 15:50:01  meichel
** Updated copyright header
**
** Revision 1.21  2000/06/07 13:56:16  meichel
** Output stream now passed as mandatory parameter to ASC_dumpParameters.
**
** Revision 1.20  2000/04/14 16:29:25  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.19  2000/03/08 16:43:15  meichel
** Updated copyright header.
**
** Revision 1.18  2000/02/23 15:12:18  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.17  2000/02/01 10:24:01  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.16  1999/05/03 14:06:29  joergr
** Minor code purifications to keep Sun CC 2.0.1 quiet.
**
** Revision 1.15  1999/04/27 17:24:25  meichel
** Updated storescu and storescp for minor changes is command line class.
**
** Revision 1.14  1999/04/27 12:25:42  meichel
** Updated echoscu copyright header
**
** Revision 1.13  1999/04/21 15:54:22  meichel
** Adapted echoscu command line options to new scheme.  Merged assctest
**   functionality into echoscu (--propose-ts and --propose-pc).
**
** Revision 1.12  1999/03/29 11:19:52  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.11  1997/08/05 07:46:19  andreas
** - Change needed version number of WINSOCK to 1.1
**   to support WINDOWS 95
**
** Revision 1.10  1997/07/21 08:37:02  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.9  1997/05/29 15:52:55  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.8  1997/05/22 13:29:57  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.7  1996/12/16 15:13:58  hewett
** Added bugfix for WINSOCK support.  The required WINSOCK version
** number was being incorrectly set to version 0.1.  The fixed
** WINSOCK initialisation now uses the MAKEWORD macro to correctly
** set the required version number. This bugfix was contributed
** by Dr. Yongjian Bao of Innomed GmbH, Germany.
**
** Revision 1.6  1996/09/27 14:05:02  hewett
** Added calls to initialise WINSOCK library for Win32 environment.  Only
** compiled in if HAVE_WINSOCK_H
**
** Revision 1.5  1996/09/27 08:24:28  hewett
** System header files now enclosed with BEGIN_EXTERN_C/END_EXTERN_C
**
** Revision 1.4  1996/09/24 16:21:48  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.3  1996/09/03 11:39:01  hewett
** Added copyright information.
**
** Revision 1.2  1996/04/22 09:57:58  hewett
** Initialised port variable to 104.
**
** Revision 1.1.1.1  1996/03/26 18:38:44  hewett
** Initial Release.
**
*/
