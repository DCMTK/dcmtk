/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Purpose: Storage Service Class User (C-STORE operation)
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 10:42:44 $
 *  CVS/RCS Revision: $Revision: 1.98 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmnet/dicom.h"      /* for DICOM_APPLICATION_REQUESTOR */
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dcasccfg.h"   /* for class DcmAssociationConfiguration */
#include "dcmtk/dcmnet/dcasccff.h"   /* for class DcmAssociationConfigurationFile */
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcuid.h"     /* for dcmtk version name */
#include "dcmtk/dcmdata/dcostrmz.h"  /* for dcmZlibCompressionLevel */

#ifdef ON_THE_FLY_COMPRESSION
#include "dcmtk/dcmjpeg/djdecode.h"  /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/djencode.h"  /* for dcmjpeg encoders */
#include "dcmtk/dcmdata/dcrledrg.h"  /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmdata/dcrleerg.h"  /* for DcmRLEEncoderRegistration */
#include "dcmtk/dcmjpeg/dipijpeg.h"  /* for dcmimage JPEG plugin */
#endif

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#ifdef WITH_ZLIB
#include <zlib.h>          /* for zlibVersion() */
#endif

#if defined (HAVE_WINDOWS_H) || defined(HAVE_FNMATCH_H)
#define PATTERN_MATCHING_AVAILABLE
#endif

#define OFFIS_CONSOLE_APPLICATION "storescu"

static OFLogger storescuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default application titles */
#define APPLICATIONTITLE     "STORESCU"
#define PEERAPPLICATIONTITLE "ANY-SCP"

static OFBool opt_showPresentationContexts = OFFalse;
static OFBool opt_abortAssociation = OFFalse;
static OFCmdUnsignedInt opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
static OFCmdUnsignedInt opt_maxSendPDULength = 0;
static E_TransferSyntax opt_networkTransferSyntax = EXS_Unknown;
static E_FileReadMode opt_readMode = ERM_autoDetect;

static OFBool opt_scanDir = OFFalse;
static OFBool opt_recurse = OFFalse;
static const char *opt_scanPattern = "";

static OFBool opt_haltOnUnsuccessfulStore = OFTrue;
static OFBool unsuccessfulStoreEncountered = OFFalse;
static int lastStatusCode = STATUS_Success;

static OFBool opt_proposeOnlyRequiredPresentationContexts = OFFalse;
static OFBool opt_combineProposedTransferSyntaxes = OFFalse;

static OFCmdUnsignedInt opt_repeatCount = 1;
static OFCmdUnsignedInt opt_inventPatientCount = 25;
static OFCmdUnsignedInt opt_inventStudyCount = 50;
static OFCmdUnsignedInt opt_inventSeriesCount = 100;
static OFBool opt_inventSOPInstanceInformation = OFFalse;
static OFBool opt_correctUIDPadding = OFFalse;
static OFString patientNamePrefix("OFFIS^TEST_PN_");   // PatientName is PN (maximum 16 chars)
static OFString patientIDPrefix("PID_"); // PatientID is LO (maximum 64 chars)
static OFString studyIDPrefix("SID_");   // StudyID is SH (maximum 16 chars)
static OFString accessionNumberPrefix;   // AccessionNumber is SH (maximum 16 chars)
static OFBool opt_secureConnection = OFFalse; /* default: no secure connection */
static const char *opt_configFile = NULL;
static const char *opt_profileName = NULL;
T_DIMSE_BlockingMode opt_blockMode = DIMSE_BLOCKING;
int opt_dimse_timeout = 0;
int opt_acse_timeout = 30;

#ifdef WITH_ZLIB
static OFCmdUnsignedInt opt_compressionLevel = 0;
#endif

#ifdef WITH_OPENSSL
static int         opt_keyFileFormat = SSL_FILETYPE_PEM;
static OFBool      opt_doAuthenticate = OFFalse;
static const char *opt_privateKeyFile = NULL;
static const char *opt_certificateFile = NULL;
static const char *opt_passwd = NULL;
#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
static OFString    opt_ciphersuites(TLS1_TXT_RSA_WITH_AES_128_SHA ":" SSL3_TXT_RSA_DES_192_CBC3_SHA);
#else
static OFString    opt_ciphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
#endif
static const char *opt_readSeedFile = NULL;
static const char *opt_writeSeedFile = NULL;
static DcmCertificateVerification opt_certVerification = DCV_requireCertificate;
static const char *opt_dhparam = NULL;
#endif

// User Identity Negotiation
static T_ASC_UserIdentityNegotiationMode opt_identMode = ASC_USER_IDENTITY_NONE;
static OFString opt_user;
static OFString opt_password;
static OFString opt_identFile;
static OFBool opt_identResponse = OFFalse;

static OFCondition
addStoragePresentationContexts(T_ASC_Parameters *params, OFList<OFString> &sopClasses);

static OFCondition
cstore(T_ASC_Association *assoc, const OFString &fname);

static OFBool
findSOPClassAndInstanceInFile(const char *fname, char *sopClass, char *sopInstance);

static OFCondition
configureUserIdentityRequest(T_ASC_Parameters *params);

static OFCondition
checkUserIdentityResponse(T_ASC_Parameters *params);

/* helper macro for converting stream output to a string */
#define CONVERT_TO_STRING(output, string) \
    optStream.str(""); \
    optStream.clear(); \
    optStream << output << OFStringStream_ends; \
    OFSTRINGSTREAM_GETOFSTRING(optStream, string)

#define SHORTCOL 4
#define LONGCOL 19

int main(int argc, char *argv[])
{
  OFOStringStream optStream;

  const char *opt_peer = NULL;
  OFCmdUnsignedInt opt_port = 104;
  const char *opt_peerTitle = PEERAPPLICATIONTITLE;
  const char *opt_ourTitle = APPLICATIONTITLE;

  OFList<OFString> fileNameList;       // list of files to transfer to SCP
  OFList<OFString> sopClassUIDList;    // the list of SOP classes
  OFList<OFString> sopInstanceUIDList; // the list of SOP instances

  T_ASC_Network *net;
  T_ASC_Parameters *params;
  DIC_NODENAME localHost;
  DIC_NODENAME peerHost;
  T_ASC_Association *assoc;
  DcmAssociationConfiguration asccfg;  // handler for association configuration profiles

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

  OFString temp_str;
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM storage (C-STORE) SCU", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  cmd.addParam("peer", "hostname of DICOM peer");
  cmd.addParam("port", "tcp/ip port number of peer");
  cmd.addParam("dcmfile-in", "DICOM file or directory to be transmitted", OFCmdParam::PM_MultiMandatory);

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
   cmd.addOption("--help",                    "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",                            "print version information and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--verbose-pc",              "+v",      "show presentation contexts in verbose mode");
   OFLog::addOptions(cmd);

  cmd.addGroup("input options:");
    cmd.addSubGroup("input file format:");
      cmd.addOption("--read-file",            "+f",      "read file format or data set (default)");
      cmd.addOption("--read-file-only",       "+fo",     "read file format only");
      cmd.addOption("--read-dataset",         "-f",      "read data set without file meta information");
    cmd.addSubGroup("input files:");
      cmd.addOption("--scan-directories",     "+sd",     "scan directories for input files (dcmfile-in)");
#ifdef PATTERN_MATCHING_AVAILABLE
      cmd.addOption("--scan-pattern",         "+sp",  1, "[p]attern: string (only with --scan-directories)",
                                                         "pattern for filename matching (wildcards)");
#endif
      cmd.addOption("--no-recurse",           "-r",      "do not recurse within directories (default)");
      cmd.addOption("--recurse",              "+r",      "recurse within specified directories");
  cmd.addGroup("network options:");
    cmd.addSubGroup("application entity titles:");
      cmd.addOption("--aetitle",              "-aet", 1, "[a]etitle: string", "set my calling AE title (default: " APPLICATIONTITLE ")");
      cmd.addOption("--call",                 "-aec", 1, "[a]etitle: string", "set called AE title of peer (default: " PEERAPPLICATIONTITLE ")");
    cmd.addSubGroup("association negotiation profile from configuration file:");
      cmd.addOption("--config-file",          "-xf",  2, "[f]ilename, [p]rofile: string",
                                                         "use profile p from config file f");
    cmd.addSubGroup("proposed transmission transfer syntaxes (not with --config-file):");
      cmd.addOption("--propose-uncompr",      "-x=",     "propose all uncompressed TS, explicit VR\nwith local byte ordering first (default)");
      cmd.addOption("--propose-little",       "-xe",     "propose all uncompressed TS, explicit VR\nlittle endian first");
      cmd.addOption("--propose-big",          "-xb",     "propose all uncompressed TS, explicit VR\nbig endian first");
      cmd.addOption("--propose-implicit",     "-xi",     "propose implicit VR little endian TS only");
      cmd.addOption("--propose-lossless",     "-xs",     "propose default JPEG lossless TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-jpeg8",        "-xy",     "propose default JPEG lossy TS for 8 bit data\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-jpeg12",       "-xx",     "propose default JPEG lossy TS for 12 bit data\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-j2k-lossless", "-xv",     "propose JPEG 2000 lossless TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-j2k-lossy",    "-xw",     "propose JPEG 2000 lossy TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-jls-lossless", "-xt",     "propose JPEG-LS lossless TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-jls-lossy",    "-xu",     "propose JPEG-LS lossy TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-mpeg2",        "-xm",     "propose MPEG2 Main Profile @ Main Level TS only");
      cmd.addOption("--propose-mpeg2-high",   "-xh",     "propose MPEG2 Main Profile @ High Level TS only");
      cmd.addOption("--propose-rle",          "-xr",     "propose RLE lossless TS\nand all uncompressed transfer syntaxes");
#ifdef WITH_ZLIB
      cmd.addOption("--propose-deflated",     "-xd",     "propose deflated expl. VR little endian TS\nand all uncompressed transfer syntaxes");
#endif
      cmd.addOption("--required",             "-R",      "propose only required presentation contexts\n(default: propose all supported)");
      cmd.addOption("--combine",              "+C",      "combine proposed transfer syntaxes\n(default: separate pres. context for each TS)");
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",        "+u",      "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",       "-u",      "disable support for new VRs, convert to OB");
#ifdef WITH_ZLIB
    cmd.addSubGroup("deflate compression level (only with --propose-deflated or --config-file):");
      cmd.addOption("--compression-level",    "+cl",  1, "[l]evel: integer (default: 6)",
                                                         "0=uncompressed, 1=fastest, 9=best compression");
#endif
    cmd.addSubGroup("user identity negotiation:");
      cmd.addOption("--user",                 "-usr", 1, "[u]ser name: string",
                                                         "authenticate using user name u");
      cmd.addOption("--password",             "-pwd", 1, "[p]assword: string (only with --user)",
                                                         "authenticate using password p");
      cmd.addOption("--empty-password",       "-epw",    "send empty password (only with --user)");
      cmd.addOption("--kerberos",             "-kt",  1, "[f]ilename: string",
                                                         "read kerberos ticket from file f");
      cmd.addOption("--saml",                         1, "[f]ilename: string",
                                                         "read SAML request from file f");
      cmd.addOption("--pos-response",         "-rsp",    "expect positive response");
    cmd.addSubGroup("other network options:");
      cmd.addOption("--timeout",              "-to",  1, "[s]econds: integer (default: unlimited)", "timeout for connection requests");
      CONVERT_TO_STRING("[s]econds: integer (default: " << opt_acse_timeout << ")", optString1);
      cmd.addOption("--acse-timeout",         "-ta",  1, optString1.c_str(), "timeout for ACSE messages");
      cmd.addOption("--dimse-timeout",        "-td",  1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");
      CONVERT_TO_STRING("[n]umber of bytes: integer (" << ASC_MINIMUMPDUSIZE << ".." << ASC_MAXIMUMPDUSIZE << ")", optString2);
      CONVERT_TO_STRING("set max receive pdu to n bytes (default: " << opt_maxReceivePDULength << ")", optString3);
      cmd.addOption("--max-pdu",              "-pdu", 1, optString2.c_str(), optString3.c_str());
      cmd.addOption("--max-send-pdu",                 1, optString2.c_str(), "restrict max send pdu to n bytes");
      cmd.addOption("--repeat",                       1, "[n]umber: integer", "repeat n times");
      cmd.addOption("--abort",                           "abort association instead of releasing it");
      cmd.addOption("--no-halt",              "-nh",     "do not halt if unsuccessful store encountered\n(default: do halt)");
      cmd.addOption("--uid-padding",          "-up",     "silently correct space-padded UIDs");

      cmd.addOption("--invent-instance",      "+II",     "invent a new SOP instance UID for every image\nsent");
      CONVERT_TO_STRING("invent a new series UID after n images" << OFendl << "have been sent (default: " << opt_inventSeriesCount << ")", optString4);
      cmd.addOption("--invent-series",        "+IR",  1, "[n]umber: integer (implies --invent-instance)", optString4.c_str());
      CONVERT_TO_STRING("invent a new study UID after n series" << OFendl << "have been sent (default: " << opt_inventStudyCount << ")", optString5);
      cmd.addOption("--invent-study",         "+IS",  1, "[n]umber: integer (implies --invent-instance)", optString5.c_str());
      CONVERT_TO_STRING("invent a new patient ID and name after n studies" << OFendl << "have been sent (default: " << opt_inventPatientCount << ")", optString6);
      cmd.addOption("--invent-patient",       "+IP",  1, "[n]umber: integer (implies --invent-instance)", optString6.c_str());

#ifdef WITH_OPENSSL
  cmd.addGroup("transport layer security (TLS) options:");
    cmd.addSubGroup("transport protocol stack:");
      cmd.addOption("--disable-tls",          "-tls",    "use normal TCP/IP connection (default)");
      cmd.addOption("--enable-tls",           "+tls", 2, "[p]rivate key file, [c]ertificate file: string",
                                                         "use authenticated secure TLS connection");
      cmd.addOption("--anonymous-tls",        "+tla",    "use secure TLS connection without certificate");
    cmd.addSubGroup("private key password (only with --enable-tls):");
      cmd.addOption("--std-passwd",           "+ps",     "prompt user to type password on stdin (default)");
      cmd.addOption("--use-passwd",           "+pw",  1, "[p]assword: string ",
                                                         "use specified password");
      cmd.addOption("--null-passwd",          "-pw",     "use empty string as password");
    cmd.addSubGroup("key and certificate file format:");
      cmd.addOption("--pem-keys",             "-pem",    "read keys and certificates as PEM file (default)");
      cmd.addOption("--der-keys",             "-der",    "read keys and certificates as DER file");
    cmd.addSubGroup("certification authority:");
      cmd.addOption("--add-cert-file",        "+cf",  1, "[c]ertificate filename: string",
                                                         "add certificate file to list of certificates", OFCommandLine::AF_NoWarning);
      cmd.addOption("--add-cert-dir",         "+cd",  1, "[c]ertificate directory: string",
                                                         "add certificates in d to list of certificates", OFCommandLine::AF_NoWarning);
    cmd.addSubGroup("ciphersuite:");
      cmd.addOption("--cipher",               "+cs",  1, "[c]iphersuite name: string",
                                                         "add ciphersuite to list of negotiated suites");
      cmd.addOption("--dhparam",              "+dp",  1, "[f]ilename: string",
                                                         "read DH parameters for DH/DSS ciphersuites");
    cmd.addSubGroup("pseudo random generator:");
      cmd.addOption("--seed",                 "+rs",  1, "[f]ilename: string",
                                                         "seed random generator with contents of f");
      cmd.addOption("--write-seed",           "+ws",     "write back modified seed (only with --seed)");
      cmd.addOption("--write-seed-file",      "+wf",  1, "[f]ilename: string (only with --seed)",
                                                         "write modified seed to file f");
    cmd.addSubGroup("peer authentication:");
      cmd.addOption("--require-peer-cert",    "-rc",     "verify peer certificate, fail if absent (default)");
      cmd.addOption("--verify-peer-cert",     "-vc",     "verify peer certificate if present");
      cmd.addOption("--ignore-peer-cert",     "-ic",     "don't verify peer certificate");
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
#if !defined(WITH_ZLIB) && !(ON_THE_FLY_COMPRESSION) && !defined(WITH_OPENSSL)
          COUT << " none" << OFendl;
#else
          COUT << OFendl;
#endif
#ifdef WITH_ZLIB
          COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef ON_THE_FLY_COMPRESSION
          COUT << "- " << DiJPEGPlugin::getLibraryVersionString() << OFendl;
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
      if (cmd.findOption("--verbose-pc"))
      {
        app.checkDependence("--verbose-pc", "verbose mode", storescuLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL));
        opt_showPresentationContexts = OFTrue;
      }

      cmd.beginOptionBlock();
      if (cmd.findOption("--read-file")) opt_readMode = ERM_autoDetect;
      if (cmd.findOption("--read-file-only")) opt_readMode = ERM_fileOnly;
      if (cmd.findOption("--read-dataset")) opt_readMode = ERM_dataset;
      cmd.endOptionBlock();

      if (cmd.findOption("--scan-directories")) opt_scanDir = OFTrue;
#ifdef PATTERN_MATCHING_AVAILABLE
      if (cmd.findOption("--scan-pattern"))
      {
        app.checkDependence("--scan-pattern", "--scan-directories", opt_scanDir);
        app.checkValue(cmd.getValue(opt_scanPattern));
      }
#endif
      cmd.beginOptionBlock();
      if (cmd.findOption("--no-recurse")) opt_recurse = OFFalse;
      if (cmd.findOption("--recurse"))
      {
        app.checkDependence("--recurse", "--scan-directories", opt_scanDir);
        opt_recurse = OFTrue;
      }
      cmd.endOptionBlock();

      if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
      if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));

      cmd.beginOptionBlock();
      if (cmd.findOption("--propose-uncompr")) opt_networkTransferSyntax = EXS_Unknown;
      if (cmd.findOption("--propose-little")) opt_networkTransferSyntax = EXS_LittleEndianExplicit;
      if (cmd.findOption("--propose-big")) opt_networkTransferSyntax = EXS_BigEndianExplicit;
      if (cmd.findOption("--propose-implicit")) opt_networkTransferSyntax = EXS_LittleEndianImplicit;
      if (cmd.findOption("--propose-lossless")) opt_networkTransferSyntax = EXS_JPEGProcess14SV1TransferSyntax;
      if (cmd.findOption("--propose-jpeg8")) opt_networkTransferSyntax = EXS_JPEGProcess1TransferSyntax;
      if (cmd.findOption("--propose-jpeg12")) opt_networkTransferSyntax = EXS_JPEGProcess2_4TransferSyntax;
      if (cmd.findOption("--propose-j2k-lossless")) opt_networkTransferSyntax = EXS_JPEG2000LosslessOnly;
      if (cmd.findOption("--propose-j2k-lossy")) opt_networkTransferSyntax = EXS_JPEG2000;
      if (cmd.findOption("--propose-jls-lossless")) opt_networkTransferSyntax = EXS_JPEGLSLossless;
      if (cmd.findOption("--propose-jls-lossy")) opt_networkTransferSyntax = EXS_JPEGLSLossy;
      if (cmd.findOption("--propose-mpeg2")) opt_networkTransferSyntax = EXS_MPEG2MainProfileAtMainLevel;
      if (cmd.findOption("--propose-mpeg2-high")) opt_networkTransferSyntax = EXS_MPEG2MainProfileAtHighLevel;
      if (cmd.findOption("--propose-rle")) opt_networkTransferSyntax = EXS_RLELossless;
#ifdef WITH_ZLIB
      if (cmd.findOption("--propose-deflated")) opt_networkTransferSyntax = EXS_DeflatedLittleEndianExplicit;
#endif
      cmd.endOptionBlock();

      if (cmd.findOption("--required")) opt_proposeOnlyRequiredPresentationContexts = OFTrue;
      if (cmd.findOption("--combine")) opt_combineProposedTransferSyntaxes = OFTrue;

      if (cmd.findOption("--config-file"))
      {
        // check conflicts with other command line options
        app.checkConflict("--config-file", "--propose-little", opt_networkTransferSyntax == EXS_LittleEndianExplicit);
        app.checkConflict("--config-file", "--propose-big", opt_networkTransferSyntax == EXS_BigEndianExplicit);
        app.checkConflict("--config-file", "--propose-implicit", opt_networkTransferSyntax == EXS_LittleEndianImplicit);
        app.checkConflict("--config-file", "--propose-lossless", opt_networkTransferSyntax == EXS_JPEGProcess14SV1TransferSyntax);
        app.checkConflict("--config-file", "--propose-jpeg8", opt_networkTransferSyntax == EXS_JPEGProcess1TransferSyntax);
        app.checkConflict("--config-file", "--propose-jpeg12", opt_networkTransferSyntax == EXS_JPEGProcess2_4TransferSyntax);
        app.checkConflict("--config-file", "--propose-j2k-lossless", opt_networkTransferSyntax == EXS_JPEG2000LosslessOnly);
        app.checkConflict("--config-file", "--propose-j2k-lossy", opt_networkTransferSyntax == EXS_JPEG2000);
        app.checkConflict("--config-file", "--propose-jls-lossless", opt_networkTransferSyntax == EXS_JPEGLSLossless);
        app.checkConflict("--config-file", "--propose-jls-lossy", opt_networkTransferSyntax == EXS_JPEGLSLossy);
        app.checkConflict("--config-file", "--propose-mpeg2", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtMainLevel);
        app.checkConflict("--config-file", "--propose-mpeg2-high", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtHighLevel);
        app.checkConflict("--config-file", "--propose-rle", opt_networkTransferSyntax == EXS_RLELossless);
#ifdef WITH_ZLIB
        app.checkConflict("--config-file", "--propose-deflated", opt_networkTransferSyntax == EXS_DeflatedLittleEndianExplicit);
#endif
        app.checkConflict("--config-file", "--required", opt_proposeOnlyRequiredPresentationContexts);
        app.checkConflict("--config-file", "--combine", opt_combineProposedTransferSyntaxes);

        app.checkValue(cmd.getValue(opt_configFile));
        app.checkValue(cmd.getValue(opt_profileName));

        // read configuration file. The profile name is checked later.
        OFCondition cond = DcmAssociationConfigurationFile::initialize(asccfg, opt_configFile);
        if (cond.bad())
        {
          OFLOG_ERROR(storescuLogger, "reading config file: " << cond.text());
          return 1;
        }
      }

#ifdef WITH_ZLIB
      if (cmd.findOption("--compression-level"))
      {
        app.checkDependence("--compression-level", "--propose-deflated or --config-file",
          (opt_networkTransferSyntax == EXS_DeflatedLittleEndianExplicit) || (opt_configFile != NULL));
        app.checkValue(cmd.getValueAndCheckMinMax(opt_compressionLevel, 0, 9));
        dcmZlibCompressionLevel.set(OFstatic_cast(int, opt_compressionLevel));
      }
#endif

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

      if (cmd.findOption("--max-pdu"))
        app.checkValue(cmd.getValueAndCheckMinMax(opt_maxReceivePDULength, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));

      if (cmd.findOption("--max-send-pdu"))
      {
        app.checkValue(cmd.getValueAndCheckMinMax(opt_maxSendPDULength, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
        dcmMaxOutgoingPDUSize.set(OFstatic_cast(Uint32, opt_maxSendPDULength));
      }

      if (cmd.findOption("--repeat"))  app.checkValue(cmd.getValueAndCheckMin(opt_repeatCount, 1));
      if (cmd.findOption("--abort"))   opt_abortAssociation = OFTrue;
      if (cmd.findOption("--no-halt")) opt_haltOnUnsuccessfulStore = OFFalse;
      if (cmd.findOption("--uid-padding")) opt_correctUIDPadding = OFTrue;

      if (cmd.findOption("--invent-instance")) opt_inventSOPInstanceInformation = OFTrue;
      if (cmd.findOption("--invent-series"))
      {
        opt_inventSOPInstanceInformation = OFTrue;
        app.checkValue(cmd.getValueAndCheckMin(opt_inventSeriesCount, 1));
      }
      if (cmd.findOption("--invent-study"))
      {
        opt_inventSOPInstanceInformation = OFTrue;
        app.checkValue(cmd.getValueAndCheckMin(opt_inventStudyCount, 1));
      }
      if (cmd.findOption("--invent-patient"))
      {
        opt_inventSOPInstanceInformation = OFTrue;
        app.checkValue(cmd.getValueAndCheckMin(opt_inventPatientCount, 1));
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
            OFLOG_ERROR(storescuLogger, "ciphersuite '" << current << "' is unknown. Known ciphersuites are:");
            unsigned long numSuites = DcmTLSTransportLayer::getNumberOfCipherSuites();
            for (unsigned long cs = 0; cs < numSuites; cs++)
              OFLOG_ERROR(storescuLogger, "    " << DcmTLSTransportLayer::getTLSCipherSuiteName(cs));
            return 1;
          } else {
            if (opt_ciphersuites.length() > 0) opt_ciphersuites += ":";
            opt_ciphersuites += currentOpenSSL;
          }
        } while (cmd.findOption("--cipher", 0, OFCommandLine::FOM_Next));
      }

#endif

      // User Identity Negotiation
      cmd.beginOptionBlock();
      if (cmd.findOption("--user"))
      {
        app.checkValue(cmd.getValue(opt_user));
        opt_identMode = ASC_USER_IDENTITY_USER;
      }
      if (cmd.findOption("--kerberos"))
      {
        app.checkValue(cmd.getValue(opt_identFile));
        opt_identMode = ASC_USER_IDENTITY_KERBEROS;
      }
      if (cmd.findOption("--saml"))
      {
        app.checkValue(cmd.getValue(opt_identFile));
        opt_identMode = ASC_USER_IDENTITY_SAML;
      }
      cmd.endOptionBlock();
      cmd.beginOptionBlock();
      if (cmd.findOption("--password"))
      {
        app.checkDependence("--password", "--user", opt_identMode == ASC_USER_IDENTITY_USER);
        app.checkValue(cmd.getValue(opt_password));
        opt_identMode = ASC_USER_IDENTITY_USER_PASSWORD;
      }
      if (cmd.findOption("--empty-password"))
      {
        app.checkDependence("--empty-password", "--user", opt_identMode == ASC_USER_IDENTITY_USER);
        opt_password= "";
        opt_identMode = ASC_USER_IDENTITY_USER_PASSWORD;
      }
      cmd.endOptionBlock();
      if (cmd.findOption("--pos-response"))
      {
         app.checkDependence("--pos-response", "--user, --kerberos or --saml", opt_identMode != ASC_USER_IDENTITY_NONE);
         opt_identResponse = OFTrue;
      }
   }

    /* print resource identifier */
    OFLOG_DEBUG(storescuLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
      OFLOG_WARN(storescuLogger, "no data dictionary loaded, check environment variable: "
          << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* finally, create list of input files */
    const char *paramString = NULL;
    const int paramCount = cmd.getParamCount();
    OFList<OFString> inputFiles;
    if (opt_scanDir)
      OFLOG_INFO(storescuLogger, "determining input files ...");
    /* iterate over all input filenames/directories */
    for (int i = 3; i <= paramCount; i++)
    {
      cmd.getParam(i, paramString);
      /* search directory recursively (if required) */
      if (OFStandard::dirExists(paramString))
      {
        if (opt_scanDir)
          OFStandard::searchDirectoryRecursively(paramString, inputFiles, opt_scanPattern, "" /*dirPrefix*/, opt_recurse);
        else
          OFLOG_WARN(storescuLogger, "ignoring directory because option --scan-directories is not set: " << paramString);
      } else
        inputFiles.push_back(paramString);
    }
    /* check whether there are any input files at all */
    if (inputFiles.empty())
    {
      OFLOG_FATAL(storescuLogger, "no input files to be sent");
      exit(1);
    }

    /* check input files */
    OFString errormsg;
    DcmFileFormat dfile;
    char sopClassUID[128];
    char sopInstanceUID[128];
    OFBool ignoreName;
    const char *currentFilename = NULL;
    OFListIterator(OFString) if_iter = inputFiles.begin();
    OFListIterator(OFString) if_last = inputFiles.end();
    OFLOG_INFO(storescuLogger, "checking input files ...");
    /* iterate over all input filenames */
    while (if_iter != if_last)
    {
      ignoreName = OFFalse;
      currentFilename = (*if_iter).c_str();
      if (OFStandard::fileExists(currentFilename))
      {
        if (opt_proposeOnlyRequiredPresentationContexts)
        {
          if (!findSOPClassAndInstanceInFile(currentFilename, sopClassUID, sopInstanceUID))
          {
            ignoreName = OFTrue;
            errormsg = "missing SOP class (or instance) in file: ";
            errormsg += currentFilename;
            if (opt_haltOnUnsuccessfulStore)
            {
              OFLOG_FATAL(storescuLogger, errormsg);
              exit(1);
            }
            else
              OFLOG_WARN(storescuLogger, errormsg << ", ignoring file");
          }
          else if (!dcmIsaStorageSOPClassUID(sopClassUID))
          {
            ignoreName = OFTrue;
            errormsg = "unknown storage SOP class in file: ";
            errormsg += currentFilename;
            errormsg += ": ";
            errormsg += sopClassUID;
            if (opt_haltOnUnsuccessfulStore)
            {
              OFLOG_FATAL(storescuLogger, errormsg);
              exit(1);
            }
            else
              OFLOG_WARN(storescuLogger, errormsg << ", ignoring file");
          }
          else
          {
            sopClassUIDList.push_back(sopClassUID);
            sopInstanceUIDList.push_back(sopInstanceUID);
          }
        }
        if (!ignoreName) fileNameList.push_back(currentFilename);
      }
      else
      {
        errormsg = "cannot access file: ";
        errormsg += currentFilename;
        if (opt_haltOnUnsuccessfulStore)
        {
          OFLOG_FATAL(storescuLogger, errormsg);
          exit(1);
        }
        else
          OFLOG_WARN(storescuLogger, errormsg << ", ignoring file");
      }
      ++if_iter;
    }

#ifdef ON_THE_FLY_COMPRESSION
    // register global JPEG decompression codecs
    DJDecoderRegistration::registerCodecs();

    // register global JPEG compression codecs
    DJEncoderRegistration::registerCodecs();

    // register RLE compression codec
    DcmRLEEncoderRegistration::registerCodecs();

    // register RLE decompression codec
    DcmRLEDecoderRegistration::registerCodecs();
#endif

    /* initialize network, i.e. create an instance of T_ASC_Network*. */
    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, opt_acse_timeout, &net);
    if (cond.bad()) {
      OFLOG_FATAL(storescuLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }

#ifdef WITH_OPENSSL

    DcmTLSTransportLayer *tLayer = NULL;
    if (opt_secureConnection)
    {
      tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, opt_readSeedFile);
      if (tLayer == NULL)
      {
        OFLOG_FATAL(storescuLogger, "unable to create TLS transport layer");
        exit(1);
      }

      if (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          if (TCS_ok != tLayer->addTrustedCertificateFile(current, opt_keyFileFormat))
            OFLOG_WARN(storescuLogger, "unable to load certificate file '" << current << "', ignoring");
        } while (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_Next));
      }

      if (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          if (TCS_ok != tLayer->addTrustedCertificateDir(current, opt_keyFileFormat))
            OFLOG_WARN(storescuLogger, "unable to load certificates from directory '" << current << "', ignoring");
        } while (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_Next));
      }

      if (opt_dhparam && !(tLayer->setTempDHParameters(opt_dhparam)))
        OFLOG_WARN(storescuLogger, "unable to load temporary DH parameter file '" << opt_dhparam << "', ignoring");

      if (opt_doAuthenticate)
      {
        if (opt_passwd) tLayer->setPrivateKeyPasswd(opt_passwd);

        if (TCS_ok != tLayer->setPrivateKeyFile(opt_privateKeyFile, opt_keyFileFormat))
        {
          OFLOG_ERROR(storescuLogger, "unable to load private TLS key from '" << opt_privateKeyFile << "'");
          return 1;
        }
        if (TCS_ok != tLayer->setCertificateFile(opt_certificateFile, opt_keyFileFormat))
        {
          OFLOG_ERROR(storescuLogger, "unable to load certificate from '" << opt_certificateFile << "'");
          return 1;
        }
        if (! tLayer->checkPrivateKeyMatchesCertificate())
        {
          OFLOG_ERROR(storescuLogger, "private key '" << opt_privateKeyFile << "' and certificate '" << opt_certificateFile << "' do not match");
          return 1;
        }
      }

      if (TCS_ok != tLayer->setCipherSuites(opt_ciphersuites.c_str()))
      {
        OFLOG_ERROR(storescuLogger, "unable to set selected cipher suites");
        return 1;
      }

      tLayer->setCertificateVerification(opt_certVerification);


      cond = ASC_setTransportLayer(net, tLayer, 0);
      if (cond.bad())
      {
        OFLOG_FATAL(storescuLogger, DimseCondition::dump(temp_str, cond));
        return 1;
      }
    }

#endif

    /* initialize asscociation parameters, i.e. create an instance of T_ASC_Parameters*. */
    cond = ASC_createAssociationParameters(&params, opt_maxReceivePDULength);
    if (cond.bad()) {
      OFLOG_FATAL(storescuLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }
    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "STORESCU" and "ANY-SCP". */
    ASC_setAPTitles(params, opt_ourTitle, opt_peerTitle, NULL);

    /* Set the transport layer type (type of network connection) in the params */
    /* strucutre. The default is an insecure connection; where OpenSSL is  */
    /* available the user is able to request an encrypted,secure connection. */
    cond = ASC_setTransportLayerType(params, opt_secureConnection);
    if (cond.bad()) {
      OFLOG_FATAL(storescuLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", opt_peer, OFstatic_cast(int, opt_port));
    ASC_setPresentationAddresses(params, localHost, peerHost);

    /* Configure User Identity Negotiation*/
    if (opt_identMode != ASC_USER_IDENTITY_NONE)
    {
      cond = configureUserIdentityRequest(params);
      if (cond.bad())
        return 1;
    }

    if (opt_profileName)
    {
      /* perform name mangling for config file key */
      OFString sprofile;
      const unsigned char *c = OFreinterpret_cast(const unsigned char *, opt_profileName);
      while (*c)
      {
        if (!isspace(*c)) sprofile += OFstatic_cast(char, toupper(*c));
        ++c;
      }

      /* set presentation contexts as defined in config file */
      cond = asccfg.setAssociationParameters(sprofile.c_str(), *params);
    }
    else
    {
      /* Set the presentation contexts which will be negotiated */
      /* when the network connection will be established */
      cond = addStoragePresentationContexts(params, sopClassUIDList);
    }

    if (cond.bad()) {
      OFLOG_FATAL(storescuLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }

    /* dump presentation contexts if required */
    if (opt_showPresentationContexts)
      OFLOG_INFO(storescuLogger, "Request Parameters:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ));
    else
      OFLOG_DEBUG(storescuLogger, "Request Parameters:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ));

    /* create association, i.e. try to establish a network connection to another */
    /* DICOM application. This call creates an instance of T_ASC_Association*. */
    OFLOG_INFO(storescuLogger, "Requesting Association");
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) {
      if (cond == DUL_ASSOCIATIONREJECTED) {
        T_ASC_RejectParameters rej;

        ASC_getRejectParameters(params, &rej);
        OFLOG_FATAL(storescuLogger, "Association Rejected:" << OFendl << ASC_printRejectParameters(temp_str, &rej));
        return 1;
      } else {
        OFLOG_FATAL(storescuLogger, "Association Request Failed: " << DimseCondition::dump(temp_str, cond));
        return 1;
      }
    }

    /* dump the connection parameters if in debug mode*/
    OFLOG_DEBUG(storescuLogger, ASC_dumpConnectionParameters(temp_str, assoc));

    /* dump the presentation contexts which have been accepted/refused */
    if (opt_showPresentationContexts)
      OFLOG_INFO(storescuLogger, "Association Parameters Negotiated:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC));
    else
      OFLOG_DEBUG(storescuLogger, "Association Parameters Negotiated:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC));

    /* count the presentation contexts which have been accepted by the SCP */
    /* If there are none, finish the execution */
    if (ASC_countAcceptedPresentationContexts(params) == 0) {
      OFLOG_FATAL(storescuLogger, "No Acceptable Presentation Contexts");
      return 1;
    }

    /* check user authentication response (if applicable) */
    cond = checkUserIdentityResponse(params);
    if (cond.bad())
    {
      OFLOG_FATAL(storescuLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }
    /* dump general information concerning the establishment of the network connection if required */
    OFLOG_INFO(storescuLogger, "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")");

    /* do the real work, i.e. for all files which were specified in the */
    /* command line, transmit the encapsulated DICOM objects to the SCP. */
    cond = EC_Normal;
    OFListIterator(OFString) iter = fileNameList.begin();
    OFListIterator(OFString) enditer = fileNameList.end();

    while ((iter != enditer) && cond.good())
    {
      cond = cstore(assoc, *iter);
      ++iter;
    }

    /* tear down association, i.e. terminate network connection to SCP */
    if (cond == EC_Normal)
    {
      if (opt_abortAssociation) {
        OFLOG_INFO(storescuLogger, "Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
          OFLOG_ERROR(storescuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
          return 1;
        }
      } else {
        /* release association */
        OFLOG_INFO(storescuLogger, "Releasing Association");
        cond = ASC_releaseAssociation(assoc);
        if (cond.bad())
        {
          OFLOG_ERROR(storescuLogger, "Association Release Failed: " << DimseCondition::dump(temp_str, cond));
          return 1;
        }
      }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
      OFLOG_ERROR(storescuLogger, "Protocol Error: Peer requested release (Aborting)");
      OFLOG_INFO(storescuLogger, "Aborting Association");
      cond = ASC_abortAssociation(assoc);
      if (cond.bad()) {
        OFLOG_ERROR(storescuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
        return 1;
      }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
      OFLOG_INFO(storescuLogger, "Peer Aborted Association");
    }
    else
    {
      OFLOG_ERROR(storescuLogger, "Store SCU Failed: " << DimseCondition::dump(temp_str, cond));
      OFLOG_INFO(storescuLogger, "Aborting Association");
      cond = ASC_abortAssociation(assoc);
      if (cond.bad()) {
        OFLOG_ERROR(storescuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
        return 1;
      }
    }

    /* destroy the association, i.e. free memory of T_ASC_Association* structure. This */
    /* call is the counterpart of ASC_requestAssociation(...) which was called above. */
    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad()) {
      OFLOG_FATAL(storescuLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }
    /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
    /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
    cond = ASC_dropNetwork(&net);
    if (cond.bad()) {
      OFLOG_FATAL(storescuLogger, DimseCondition::dump(temp_str, cond));
      return 1;
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
          OFLOG_WARN(storescuLogger, "cannot write random seed file '" << opt_writeSeedFile << "', ignoring");
      } else
        OFLOG_WARN(storescuLogger, "cannot write random seed, ignoring");
    }
    delete tLayer;
#endif

    int exitCode = 0;
    if (opt_haltOnUnsuccessfulStore && unsuccessfulStoreEncountered) {
      if (lastStatusCode == STATUS_Success) {
        // there must have been some kind of general network error
        exitCode = 0xff;
      } else {
        exitCode = (lastStatusCode >> 8); // only the least significant byte is relevant as exit code
      }
    }

#ifdef ON_THE_FLY_COMPRESSION
    // deregister JPEG codecs
    DJDecoderRegistration::cleanup();
    DJEncoderRegistration::cleanup();

    // deregister RLE codecs
    DcmRLEDecoderRegistration::cleanup();
    DcmRLEEncoderRegistration::cleanup();
#endif

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif
    return exitCode;
}


static OFBool
isaListMember(OFList<OFString> &lst, OFString &s)
{
  OFListIterator(OFString) cur = lst.begin();
  OFListIterator(OFString) end = lst.end();

  OFBool found = OFFalse;
  while (cur != end && !found) {
    found = (s == *cur);
    ++cur;
  }

  return found;
}

static OFCondition
addPresentationContext(T_ASC_Parameters *params,
  int presentationContextId,
  const OFString &abstractSyntax,
  const OFString &transferSyntax,
  T_ASC_SC_ROLE proposedRole = ASC_SC_ROLE_DEFAULT)
{
  const char *c_p = transferSyntax.c_str();
  OFCondition cond = ASC_addPresentationContext(params, presentationContextId,
    abstractSyntax.c_str(), &c_p, 1, proposedRole);
  return cond;
}

static OFCondition
addPresentationContext(T_ASC_Parameters *params,
  int presentationContextId,
  const OFString &abstractSyntax,
  const OFList<OFString> &transferSyntaxList,
  T_ASC_SC_ROLE proposedRole = ASC_SC_ROLE_DEFAULT)
{
  // create an array of supported/possible transfer syntaxes
  const char **transferSyntaxes = new const char*[transferSyntaxList.size()];
  int transferSyntaxCount = 0;
  OFListConstIterator(OFString) s_cur = transferSyntaxList.begin();
  OFListConstIterator(OFString) s_end = transferSyntaxList.end();
  while (s_cur != s_end) {
    transferSyntaxes[transferSyntaxCount++] = (*s_cur).c_str();
    ++s_cur;
  }

  OFCondition cond = ASC_addPresentationContext(params, presentationContextId,
    abstractSyntax.c_str(), transferSyntaxes, transferSyntaxCount, proposedRole);

  delete[] transferSyntaxes;
  return cond;
}

static OFCondition
addStoragePresentationContexts(T_ASC_Parameters *params,
  OFList<OFString> &sopClasses)
{
  /*
   * Each SOP Class will be proposed in two presentation contexts (unless
   * the opt_combineProposedTransferSyntaxes global variable is true).
   * The command line specified a preferred transfer syntax to use.
   * This prefered transfer syntax will be proposed in one
   * presentation context and a set of alternative (fallback) transfer
   * syntaxes will be proposed in a different presentation context.
   *
   * Generally, we prefer to use Explicitly encoded transfer syntaxes
   * and if running on a Little Endian machine we prefer
   * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
   * Some SCP implementations will just select the first transfer
   * syntax they support (this is not part of the standard) so
   * organise the proposed transfer syntaxes to take advantage
   * of such behaviour.
   */

  // Which transfer syntax was preferred on the command line
  OFString preferredTransferSyntax;
  if (opt_networkTransferSyntax == EXS_Unknown) {
    /* gLocalByteOrder is defined in dcxfer.h */
    if (gLocalByteOrder == EBO_LittleEndian) {
      /* we are on a little endian machine */
      preferredTransferSyntax = UID_LittleEndianExplicitTransferSyntax;
    } else {
      /* we are on a big endian machine */
      preferredTransferSyntax = UID_BigEndianExplicitTransferSyntax;
    }
  } else {
    DcmXfer xfer(opt_networkTransferSyntax);
    preferredTransferSyntax = xfer.getXferID();
  }

  OFListIterator(OFString) s_cur;
  OFListIterator(OFString) s_end;

  OFList<OFString> fallbackSyntaxes;
  // - If little endian implicit is preferred, we don't need any fallback syntaxes
  //   because it is the default transfer syntax and all applications must support it.
  // - If MPEG2 MP@ML/HL is preferred, we don't want to propose any fallback solution
  //   because this is not required and we cannot decompress the movie anyway.
  if ((opt_networkTransferSyntax != EXS_LittleEndianImplicit) &&
      (opt_networkTransferSyntax != EXS_MPEG2MainProfileAtMainLevel) &&
      (opt_networkTransferSyntax != EXS_MPEG2MainProfileAtHighLevel))
  {
    fallbackSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
    fallbackSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
    fallbackSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
    // Remove the preferred syntax from the fallback list
    fallbackSyntaxes.remove(preferredTransferSyntax);
  }

  // create a list of transfer syntaxes combined from the preferred and fallback syntaxes
  OFList<OFString> combinedSyntaxes;
  s_cur = fallbackSyntaxes.begin();
  s_end = fallbackSyntaxes.end();
  combinedSyntaxes.push_back(preferredTransferSyntax);
  while (s_cur != s_end)
  {
    if (!isaListMember(combinedSyntaxes, *s_cur)) combinedSyntaxes.push_back(*s_cur);
    ++s_cur;
  }

  if (!opt_proposeOnlyRequiredPresentationContexts) {
    // add the (short list of) known storage SOP classes to the list
    // the array of Storage SOP Class UIDs comes from dcuid.h
    for (int i = 0; i < numberOfDcmShortSCUStorageSOPClassUIDs; i++)
      sopClasses.push_back(dcmShortSCUStorageSOPClassUIDs[i]);
  }

  // thin out the SOP classes to remove any duplicates
  OFList<OFString> sops;
  s_cur = sopClasses.begin();
  s_end = sopClasses.end();
  while (s_cur != s_end) {
    if (!isaListMember(sops, *s_cur)) {
      sops.push_back(*s_cur);
    }
    ++s_cur;
  }

  // add a presentations context for each SOP class / transfer syntax pair
  OFCondition cond = EC_Normal;
  int pid = 1; // presentation context id
  s_cur = sops.begin();
  s_end = sops.end();
  while (s_cur != s_end && cond.good()) {

    if (pid > 255) {
      OFLOG_ERROR(storescuLogger, "Too many presentation contexts");
      return ASC_BADPRESENTATIONCONTEXTID;
    }

    if (opt_combineProposedTransferSyntaxes) {
      cond = addPresentationContext(params, pid, *s_cur, combinedSyntaxes);
      pid += 2;   /* only odd presentation context id's */
    } else {

      // SOP class with preferred transfer syntax
      cond = addPresentationContext(params, pid, *s_cur, preferredTransferSyntax);
      pid += 2;   /* only odd presentation context id's */

      if (fallbackSyntaxes.size() > 0) {
        if (pid > 255) {
          OFLOG_ERROR(storescuLogger, "Too many presentation contexts");
          return ASC_BADPRESENTATIONCONTEXTID;
        }

        // SOP class with fallback transfer syntax
        cond = addPresentationContext(params, pid, *s_cur, fallbackSyntaxes);
        pid += 2; /* only odd presentation context id's */
      }
    }
    ++s_cur;
  }

  return cond;
}

static int
secondsSince1970()
{
  time_t t = time(NULL);
  return OFstatic_cast(int, t);
}

static OFString
intToString(int i)
{
  char numbuf[32];
  sprintf(numbuf, "%d", i);
  return numbuf;
}

static OFString
makeUID(OFString basePrefix, int counter)
{
  OFString prefix = basePrefix + "." + intToString(counter);
  char uidbuf[65];
  OFString uid = dcmGenerateUniqueIdentifier(uidbuf, prefix.c_str());
  return uid;
}

static OFBool
updateStringAttributeValue(DcmItem *dataset, const DcmTagKey &key, OFString &value)
{
  DcmStack stack;
  DcmTag tag(key);

  OFCondition cond = EC_Normal;
  cond = dataset->search(key, stack, ESM_fromHere, OFFalse);
  if (cond != EC_Normal) {
    OFLOG_ERROR(storescuLogger, "updateStringAttributeValue: cannot find: " << tag.getTagName()
         << " " << key << ": " << cond.text());
    return OFFalse;
  }

  DcmElement *elem = OFstatic_cast(DcmElement *, stack.top());

  DcmVR vr(elem->ident());
  if (elem->getLength() > vr.getMaxValueLength()) {
    OFLOG_ERROR(storescuLogger, "updateStringAttributeValue: INTERNAL ERROR: " << tag.getTagName()
         << " " << key << ": value too large (max " << vr.getMaxValueLength()
         << ") for " << vr.getVRName() << " value: " << value);
    return OFFalse;
  }

  cond = elem->putOFStringArray(value);
  if (cond != EC_Normal) {
    OFLOG_ERROR(storescuLogger, "updateStringAttributeValue: cannot put string in attribute: " << tag.getTagName()
         << " " << key << ": " << cond.text());
    return OFFalse;
  }

  return OFTrue;
}

static void
replaceSOPInstanceInformation(DcmDataset *dataset)
{
  static OFCmdUnsignedInt patientCounter = 0;
  static OFCmdUnsignedInt studyCounter = 0;
  static OFCmdUnsignedInt seriesCounter = 0;
  static OFCmdUnsignedInt imageCounter = 0;
  static OFString seriesInstanceUID;
  static OFString seriesNumber;
  static OFString studyInstanceUID;
  static OFString studyID;
  static OFString accessionNumber;
  static OFString patientID;
  static OFString patientName;

  if (seriesInstanceUID.length() == 0) seriesInstanceUID=makeUID(SITE_SERIES_UID_ROOT, OFstatic_cast(int, seriesCounter));
  if (seriesNumber.length() == 0) seriesNumber = intToString(OFstatic_cast(int, seriesCounter));
  if (studyInstanceUID.length() == 0) studyInstanceUID = makeUID(SITE_STUDY_UID_ROOT, OFstatic_cast(int, studyCounter));
  if (studyID.length() == 0) studyID = studyIDPrefix + intToString(OFstatic_cast(int, secondsSince1970())) + intToString(OFstatic_cast(int, studyCounter));
  if (accessionNumber.length() == 0) accessionNumber = accessionNumberPrefix + intToString(secondsSince1970()) + intToString(OFstatic_cast(int, studyCounter));
  if (patientID.length() == 0) patientID = patientIDPrefix + intToString(secondsSince1970()) + intToString(OFstatic_cast(int, patientCounter));
  if (patientName.length() == 0) patientName = patientNamePrefix + intToString(secondsSince1970()) + intToString(OFstatic_cast(int, patientCounter));

  if (imageCounter >= opt_inventSeriesCount) {
    imageCounter = 0;
    seriesCounter++;
    seriesInstanceUID = makeUID(SITE_SERIES_UID_ROOT, OFstatic_cast(int, seriesCounter));
    seriesNumber = intToString(OFstatic_cast(int, seriesCounter));
  }
  if (seriesCounter >= opt_inventStudyCount) {
    seriesCounter = 0;
    studyCounter++;
    studyInstanceUID = makeUID(SITE_STUDY_UID_ROOT, OFstatic_cast(int, studyCounter));
    studyID = studyIDPrefix + intToString(secondsSince1970()) + intToString(OFstatic_cast(int, studyCounter));
    accessionNumber = accessionNumberPrefix + intToString(secondsSince1970()) + intToString(OFstatic_cast(int, studyCounter));
  }
  if (studyCounter >= opt_inventPatientCount) {
    // we create as many patients as necessary */
    studyCounter = 0;
    patientCounter++;
    patientID = patientIDPrefix + intToString(secondsSince1970()) + intToString(OFstatic_cast(int, patientCounter));
    patientName = patientNamePrefix + intToString(secondsSince1970()) + intToString(OFstatic_cast(int, patientCounter));
  }

  OFString sopInstanceUID = makeUID(SITE_INSTANCE_UID_ROOT, OFstatic_cast(int, imageCounter));
  OFString imageNumber = intToString(OFstatic_cast(int, imageCounter));

  OFLOG_INFO(storescuLogger, "Inventing Identifying Information ("
         << "pa" << patientCounter << ", st" << studyCounter
         << ", se" << seriesCounter << ", im" << imageCounter << "):");
  OFLOG_INFO(storescuLogger, "  PatientName=" << patientName);
  OFLOG_INFO(storescuLogger, "  PatientID=" << patientID);
  OFLOG_INFO(storescuLogger, "  StudyInstanceUID=" << studyInstanceUID);
  OFLOG_INFO(storescuLogger, "  StudyID=" << studyID);
  OFLOG_INFO(storescuLogger, "  SeriesInstanceUID=" << seriesInstanceUID);
  OFLOG_INFO(storescuLogger, "  SeriesNumber=" << seriesNumber);
  OFLOG_INFO(storescuLogger, "  SOPInstanceUID=" << sopInstanceUID);
  OFLOG_INFO(storescuLogger, "  ImageNumber=" << imageNumber);

  updateStringAttributeValue(dataset, DCM_PatientName, patientName);
  updateStringAttributeValue(dataset, DCM_PatientID, patientID);
  updateStringAttributeValue(dataset, DCM_StudyInstanceUID, studyInstanceUID);
  updateStringAttributeValue(dataset, DCM_StudyID, studyID);
  updateStringAttributeValue(dataset, DCM_SeriesInstanceUID, seriesInstanceUID);
  updateStringAttributeValue(dataset, DCM_SeriesNumber, seriesNumber);
  updateStringAttributeValue(dataset, DCM_SOPInstanceUID, sopInstanceUID);
  updateStringAttributeValue(dataset, DCM_InstanceNumber, imageNumber);

  imageCounter++;
}

static void
progressCallback(void * /*callbackData*/,
  T_DIMSE_StoreProgress *progress,
  T_DIMSE_C_StoreRQ * req)
{
  if (progress->state == DIMSE_StoreBegin)
  {
    OFString str;
    OFLOG_DEBUG(storescuLogger, DIMSE_dumpMessage(str, *req, DIMSE_OUTGOING));
  }

  // We can't use oflog for the pdu output, but we use a special logger for
  // generating this output. If it is set to level "INFO" we generate the
  // output, if it's set to "DEBUG" then we'll assume that there is debug output
  // generated for each PDU elsewhere.
  OFLogger progressLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION ".progress");
  if (progressLogger.getChainedLogLevel() == OFLogger::INFO_LOG_LEVEL) {
    switch (progress->state) {
      case DIMSE_StoreBegin:
        COUT << "XMIT: "; break;
      case DIMSE_StoreEnd:
        COUT << OFendl; break;
      default:
        COUT << "."; break;
    }
    COUT.flush();
  }
}

static OFCondition
storeSCU(T_ASC_Association *assoc, const char *fname)
  /*
   * This function will read all the information from the given file,
   * figure out a corresponding presentation context which will be used
   * to transmit the information over the network to the SCP, and it
   * will finally initiate the transmission of all data to the SCP.
   *
   * Parameters:
   *   assoc - [in] The association (network connection to another DICOM application).
   *   fname - [in] Name of the file which shall be processed.
   */
{
  DIC_US msgId = assoc->nextMsgID++;
  T_ASC_PresentationContextID presID;
  T_DIMSE_C_StoreRQ req;
  T_DIMSE_C_StoreRSP rsp;
  DIC_UI sopClass;
  DIC_UI sopInstance;
  DcmDataset *statusDetail = NULL;

  unsuccessfulStoreEncountered = OFTrue; // assumption

  OFLOG_INFO(storescuLogger, "Sending file: " << fname);

  /* read information from file. After the call to DcmFileFormat::loadFile(...) the information */
  /* which is encapsulated in the file will be available through the DcmFileFormat object. */
  /* In detail, it will be available through calls to DcmFileFormat::getMetaInfo() (for */
  /* meta header information) and DcmFileFormat::getDataset() (for data set information). */
  DcmFileFormat dcmff;
  OFCondition cond = dcmff.loadFile(fname, EXS_Unknown, EGL_noChange, DCM_MaxReadLength, opt_readMode);

  /* figure out if an error occured while the file was read*/
  if (cond.bad()) {
    OFLOG_ERROR(storescuLogger, "Bad DICOM file: " << fname << ": " << cond.text());
    return cond;
  }

  /* if required, invent new SOP instance information for the current data set (user option) */
  if (opt_inventSOPInstanceInformation) {
    replaceSOPInstanceInformation(dcmff.getDataset());
  }

  /* figure out which SOP class and SOP instance is encapsulated in the file */
  if (!DU_findSOPClassAndInstanceInDataSet(dcmff.getDataset(),
    sopClass, sopInstance, opt_correctUIDPadding)) {
      OFLOG_ERROR(storescuLogger, "No SOP Class or Instance UID in file: " << fname);
      return DIMSE_BADDATA;
  }

  /* figure out which of the accepted presentation contexts should be used */
  DcmXfer filexfer(dcmff.getDataset()->getOriginalXfer());

  /* special case: if the file uses an unencapsulated transfer syntax (uncompressed
   * or deflated explicit VR) and we prefer deflated explicit VR, then try
   * to find a presentation context for deflated explicit VR first.
   */
  if (filexfer.isNotEncapsulated() &&
    opt_networkTransferSyntax == EXS_DeflatedLittleEndianExplicit)
  {
    filexfer = EXS_DeflatedLittleEndianExplicit;
  }

  if (filexfer.getXfer() != EXS_Unknown) presID = ASC_findAcceptedPresentationContextID(assoc, sopClass, filexfer.getXferID());
  else presID = ASC_findAcceptedPresentationContextID(assoc, sopClass);
  if (presID == 0) {
    const char *modalityName = dcmSOPClassUIDToModality(sopClass);
    if (!modalityName) modalityName = dcmFindNameOfUID(sopClass);
    if (!modalityName) modalityName = "unknown SOP class";
    OFLOG_ERROR(storescuLogger, "No presentation context for: (" << modalityName << ") " << sopClass);
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  T_ASC_PresentationContext pc;
  ASC_findAcceptedPresentationContext(assoc->params, presID, &pc);
  DcmXfer netTransfer(pc.acceptedTransferSyntax);

  /* if required, dump general information concerning transfer syntaxes */
  if (storescuLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL)) {
    DcmXfer fileTransfer(dcmff.getDataset()->getOriginalXfer());
    OFLOG_INFO(storescuLogger, "Transfer Syntax: " << dcmFindNameOfUID(fileTransfer.getXferID(), "")
        << " -> " << dcmFindNameOfUID(netTransfer.getXferID(), ""));
  }

#ifdef ON_THE_FLY_COMPRESSION
  dcmff.getDataset()->chooseRepresentation(netTransfer.getXfer(), NULL);
#endif

  /* prepare the transmission of data */
  bzero(OFreinterpret_cast(char *, &req), sizeof(req));
  req.MessageID = msgId;
  strcpy(req.AffectedSOPClassUID, sopClass);
  strcpy(req.AffectedSOPInstanceUID, sopInstance);
  req.DataSetType = DIMSE_DATASET_PRESENT;
  req.Priority = DIMSE_PRIORITY_LOW;

  /* if required, dump some more general information */
  OFLOG_INFO(storescuLogger, "Sending Store Request: MsgID " << msgId << ", ("
    << dcmSOPClassUIDToModality(sopClass, "OT") << ")");

  /* finally conduct transmission of data */
  cond = DIMSE_storeUser(assoc, presID, &req,
    NULL, dcmff.getDataset(), progressCallback, NULL,
    opt_blockMode, opt_dimse_timeout,
    &rsp, &statusDetail, NULL, OFStandard::getFileSize(fname));

  /*
   * If store command completed normally, with a status
   * of success or some warning then the image was accepted.
   */
  if (cond == EC_Normal && (rsp.DimseStatus == STATUS_Success || DICOM_WARNING_STATUS(rsp.DimseStatus))) {
    unsuccessfulStoreEncountered = OFFalse;
  }

  /* remember the response's status for later transmissions of data */
  lastStatusCode = rsp.DimseStatus;

  /* dump some more general information */
  if (cond == EC_Normal)
  {
    OFString str;
    OFLOG_INFO(storescuLogger, "Received Store Response");
    OFLOG_DEBUG(storescuLogger, DIMSE_dumpMessage(str, rsp, DIMSE_INCOMING, NULL, presID));
  }
  else
  {
    OFString temp_str;
    OFLOG_ERROR(storescuLogger, "Store Failed, file: " << fname << ":" << OFendl << DimseCondition::dump(temp_str, cond));
  }

  /* dump status detail information if there is some */
  if (statusDetail != NULL) {
    OFLOG_WARN(storescuLogger, "Status Detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
    delete statusDetail;
  }
  /* return */
  return cond;
}


static OFCondition
cstore(T_ASC_Association *assoc, const OFString &fname)
  /*
   * This function will process the given file as often as is specified by opt_repeatCount.
   * "Process" in this case means "read file, send C-STORE-RQ, receive C-STORE-RSP".
   *
   * Parameters:
   *   assoc - [in] The association (network connection to another DICOM application).
   *   fname - [in] Name of the file which shall be processed.
   */
{
  OFCondition cond = EC_Normal;

  /* opt_repeatCount specifies how many times a certain file shall be processed */
  int n = OFstatic_cast(int, opt_repeatCount);

  /* as long as no error occured and the counter does not equal 0 */
  while ((cond.good()) && n-- && !(opt_haltOnUnsuccessfulStore && unsuccessfulStoreEncountered))
  {
    /* process file (read file, send C-STORE-RQ, receive C-STORE-RSP) */
    cond = storeSCU(assoc, fname.c_str());
  }

  // we don't want to return an error code if --no-halt was specified.
  if (!opt_haltOnUnsuccessfulStore)
  {
    cond = EC_Normal;
  }

  /* return result value */
  return cond;
}


static OFBool
findSOPClassAndInstanceInFile(
  const char *fname,
  char *sopClass,
  char *sopInstance)
{
    DcmFileFormat ff;
    if (!ff.loadFile(fname, EXS_Unknown, EGL_noChange, DCM_MaxReadLength, opt_readMode).good())
        return OFFalse;

    /* look in the meta-header first */
    OFBool found = DU_findSOPClassAndInstanceInDataSet(ff.getMetaInfo(), sopClass, sopInstance, opt_correctUIDPadding);

    if (!found)
      found = DU_findSOPClassAndInstanceInDataSet(ff.getDataset(), sopClass, sopInstance, opt_correctUIDPadding);

    return found;
}


static OFCondition
configureUserIdentityRequest(T_ASC_Parameters *params)
{
  OFCondition cond = EC_Normal;
  switch (opt_identMode)
  {
    case ASC_USER_IDENTITY_USER:
    {
      cond = ASC_setIdentRQUserOnly(params, opt_user, opt_identResponse);
      return cond;
    }
    case ASC_USER_IDENTITY_USER_PASSWORD:
    {
      cond = ASC_setIdentRQUserPassword(params, opt_user, opt_password, opt_identResponse);
      return cond;
    }
    case ASC_USER_IDENTITY_KERBEROS:
    case ASC_USER_IDENTITY_SAML:
    {
      OFFile identFile;
      if (!identFile.fopen(opt_identFile.c_str(), "rb"))
      {
        OFString openerror;
        identFile.getLastErrorString(openerror);
        OFLOG_ERROR(storescuLogger, "Unable to open Kerberos or SAML file: " << openerror);
        return EC_IllegalCall;
      }
      // determine file size
      offile_off_t result = identFile.fseek(0, SEEK_END);
      if (result != 0)
        return EC_IllegalParameter;
      offile_off_t filesize = identFile.ftell();
      identFile.rewind();
      if (filesize > 65535)
      {
        OFLOG_INFO(storescuLogger, "Kerberos or SAML file is larger than 65535 bytes, bytes after that position are ignored");
        filesize = 65535;
      }

      char *buf = new char[OFstatic_cast(unsigned int, filesize)];
      size_t bytesRead = identFile.fread(buf, 1, OFstatic_cast(size_t, filesize));
      identFile.fclose();
      if (bytesRead == 0)
      {
        OFLOG_ERROR(storescuLogger, "Unable to read Kerberos or SAML info from file: File empty?");
        delete[] buf;
        return EC_IllegalCall;
      }
      if (opt_identMode == ASC_USER_IDENTITY_KERBEROS)
        cond = ASC_setIdentRQKerberos(params, buf, bytesRead, opt_identResponse);
      else
        cond = ASC_setIdentRQSaml(params, buf, bytesRead, opt_identResponse);
      delete[] buf;
      break;
    }
    default:
    {
      cond = EC_IllegalCall;
    }
  }
  if (cond.bad())
  {
    OFString temp_str;
    OFLOG_FATAL(storescuLogger, DimseCondition::dump(temp_str, cond));
  }
  return cond;
}

static OFCondition
checkUserIdentityResponse(T_ASC_Parameters *params)
{
  if (params == NULL)
    return ASC_NULLKEY;

  /* So far it is only checked whether a requested, positive response was
     actually received */

  // In case we sent no user identity request, there are no checks at all
  if ((opt_identMode == ASC_USER_IDENTITY_NONE) || (!opt_identResponse))
    return EC_Normal;

  // If positive response was requested, we expect a corresponding response
  if ((opt_identMode == ASC_USER_IDENTITY_USER) || (opt_identMode == ASC_USER_IDENTITY_USER_PASSWORD))
  {
    UserIdentityNegotiationSubItemAC *rsp = params->DULparams.ackUserIdentNeg;
    if (rsp == NULL)
    {
      OFLOG_ERROR(storescuLogger, "User Identity Negotiation failed: Positive response requested but none received");
      return ASC_USERIDENTIFICATIONFAILED;
    }
  }
  return EC_Normal;
}

/*
** CVS Log
** $Log: storescu.cc,v $
** Revision 1.98  2010-11-01 10:42:44  uli
** Fixed some compiler warnings reported by gcc with additional flags.
**
** Revision 1.97  2010-10-20 07:41:35  uli
** Made sure isalpha() & friends are only called with valid arguments.
**
** Revision 1.96  2010-10-14 13:13:42  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.95  2010-09-02 12:12:48  joergr
** Added support for "MPEG2 Main Profile @ High Level" transfer syntax.
**
** Revision 1.94  2010-08-10 11:59:31  uli
** Fixed some cases where dcmFindNameOfUID() returning NULL could cause crashes.
**
** Revision 1.93  2010-08-09 13:19:32  joergr
** Updated data dictionary to 2009 edition of the DICOM standard. From now on,
** the official "keyword" is used for the attribute name which results in a
** number of minor changes (e.g. "PatientsName" is now called "PatientName").
**
** Revision 1.92  2010-07-21 12:54:05  joergr
** Made sure that no NULL pointer is passed to the OFString constructor. This
** occurred when option --scan-directories was used without --scan-pattern.
**
** Revision 1.91  2010-06-09 20:00:46  joergr
** Fixed wrong option text introduced with last commit.
**
** Revision 1.90  2010-06-09 16:03:36  joergr
** Used new approach on how to output variable information to the syntax usage.
** Moved check on input files behind the line where the resource identifier is
** printed.
**
** Revision 1.89  2010-05-21 11:47:52  uli
** Replaced DU_fileSize() with OFStandard::getFileSize().
**
** Revision 1.88  2010-05-18 16:10:25  joergr
** Replaced '\n' by OFendl in log messages.
**
** Revision 1.87  2010-02-04 13:54:13  joergr
** Fixed issue with experimental macro ON_THE_FLY_COMPRESSION.
**
** Revision 1.86  2009-12-02 16:13:23  joergr
** Make sure that dcmSOPClassUIDToModality() never returns NULL when passed to
** the log stream in order to avoid an application crash.
**
** Revision 1.85  2009-12-01 10:16:07  joergr
** Sightly modified log messages.
**
** Revision 1.84  2009-12-01 09:52:08  joergr
** Sightly modified log messages.
**
** Revision 1.83  2009-11-18 11:53:58  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.82  2009-08-04 10:08:42  joergr
** Added output of Presentation Context ID of the C-STORE message in debug mode.
**
** Revision 1.81  2009-07-13 09:44:18  onken
** Removed misleading comment about dcmnet DIMSE return code and changed
** corresponding OFCondition check from EC_Normal to .good().
**
** Revision 1.80  2009-06-04 10:16:24  joergr
** Added new flag that can be used to avoid wrong warning messages (in debug
** mode) that an option has possibly never been checked.
**
** Revision 1.79  2009-04-24 12:26:05  joergr
** Fixed minor inconsistencies regarding layout/formatting in syntax usage.
**
** Revision 1.78  2009-04-21 14:09:22  joergr
** Fixed minor inconsistencies in manpage / syntax usage.
**
** Revision 1.77  2009-02-06 16:41:00  joergr
** Added support for JPEG-LS and MPEG2 transfer syntaxes.
** Fixed minor inconsistencies with regard to transfer syntaxes.
** Call OFConsoleApplication::checkDependence() where appropriate.
** Reworked checking of command line options for user identify negotiation.
**
** Revision 1.76  2008-11-03 15:44:26  joergr
** Removed "option block" encapsulation from option --compression-level.
**
** Revision 1.75  2008-11-03 15:29:03  joergr
** Made documentation of --compression-level more consistent with other options.
**
** Revision 1.74  2008-09-25 16:00:58  joergr
** Added support for printing the expanded command line arguments.
** Always output the resource identifier of the command line tool in debug mode.
**
** Revision 1.73  2008-04-18 14:09:30  onken
** *** empty log message ***
**
** Revision 1.72  2008-04-18 09:19:18  onken
** *** empty log message ***
**
** Revision 1.71  2008-04-18 08:54:14  onken
** Renamed commandline option from -pw to --pass. Minor commandline option
** formatting changes.
**
** Revision 1.70  2008-04-17 15:30:21  onken
** Added command line options for User Identity Negotiation.
**
** Revision 1.69  2007-10-01 16:23:44  joergr
** Renamed command line option --pattern (+p) to --scan-pattern (+sp).
**
** Revision 1.68  2007/03/12 14:29:05  joergr
** Added support for searching directories recursively for DICOM files.
** Added support for common "input file format" options.
** Consistently use COUT and CERR instead of stdout and stderr.
**
** Revision 1.67  2006/08/15 16:04:28  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.66  2006/07/27 14:23:32  joergr
** Changed parameter "exclusive" of method addOption() from type OFBool into an
** integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
** Option "--help" is no longer an exclusive option by default.
** Made naming conventions for command line parameters more consistent, e.g.
** used "dcmfile-in", "dcmfile-out" and "bitmap-out".
**
** Revision 1.65  2006/02/03 10:35:53  joergr
** Fixed inconsistent source code layout.
**
** Revision 1.64  2005/12/08 15:44:22  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.63  2005/11/23 16:10:23  meichel
** Added support for AES ciphersuites in TLS module. All TLS-enabled
**   tools now support the "AES TLS Secure Transport Connection Profile".
**
** Revision 1.62  2005/11/17 13:45:16  meichel
** Added command line options for DIMSE and ACSE timeouts
**
** Revision 1.61  2005/11/16 14:58:07  meichel
** Set association timeout in ASC_initializeNetwork to 30 seconds. This improves
**   the responsiveness of the tools if the peer blocks during assoc negotiation.
**
** Revision 1.60  2005/11/11 16:09:01  onken
** Added options for JPEG2000 support (lossy and lossless)
**
** Revision 1.59  2005/10/25 08:55:43  meichel
** Updated list of UIDs and added support for new transfer syntaxes
**   and storage SOP classes.
**
** Revision 1.58  2004/01/21 10:18:39  meichel
** StoreSCU with --no-halt option now also continues if errors other than a
**   missing presentation context occur, e.g. attempt to load non-DICOM file.
**
** Revision 1.57  2003/08/11 18:31:15  joergr
** Included "ctype" header file required for gcc 3.2.3 on Debian Linux.
**
** Revision 1.56  2003/07/03 15:43:37  meichel
** Adapted for use of OFListConstIterator, needed for compiling with HAVE_STL.
**
** Revision 1.55  2003/06/11 15:47:10  meichel
** Extended documentation, fixed typos
**
** Revision 1.54  2003/06/11 13:02:52  meichel
** Added support for configuration file based association negotiation
**   profiles
**
** Revision 1.53  2003/06/02 16:44:10  meichel
** Renamed local variables to avoid name clashes with STL
**
** Revision 1.52  2002/11/29 09:15:51  meichel
** Introduced new command line option --timeout for controlling the
**   connection request timeout.
**
** Revision 1.51  2002/11/27 13:04:32  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.50  2002/11/26 08:43:22  meichel
** Replaced all includes for "zlib.h" with <zlib.h>
**   to avoid inclusion of zlib.h in the makefile dependencies.
**
** Revision 1.49  2002/11/25 18:00:20  meichel
** Converted compile time option to leniently handle space padded UIDs
**   in the Storage Service Class into command line / config file option.
**
** Revision 1.48  2002/09/23 17:53:48  joergr
** Added new command line option "--version" which prints the name and version
** number of external libraries used (incl. preparation for future support of
** 'config.guess' host identifiers).
**
** Revision 1.47  2002/09/10 16:02:06  meichel
** Added --max-send-pdu option that allows to restrict the size of
**   outgoing P-DATA PDUs
**
** Revision 1.46  2002/08/29 16:02:19  meichel
** Added --propose-deflated and --compression-level options to storescu
**
** Revision 1.45  2002/08/20 12:21:22  meichel
** Adapted code to new loadFile and saveFile methods, thus removing direct
**   use of the DICOM stream classes.
**
** Revision 1.44  2002/01/08 10:27:29  joergr
** Corrected spelling of function dcmGenerateUniqueIdentifier().
**
** Revision 1.43  2001/12/06 14:11:13  joergr
** Made description and layout of command line options more consistent.
**
** Revision 1.42  2001/11/09 15:56:25  joergr
** Renamed some of the getValue/getParam methods to avoid ambiguities reported
** by certain compilers.
**
** Revision 1.41  2001/11/01 14:38:59  wilkens
** Added lots of comments.
**
** Revision 1.40  2001/10/12 10:18:22  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.39  2001/09/28 13:21:42  joergr
** Replaced "cerr" by "CERR".
**
** Revision 1.38  2001/09/26 12:28:56  meichel
** Implemented changes in dcmnet required by the adaptation of dcmdata
**   to class OFCondition.  Removed some unused code.
**
** Revision 1.37  2001/06/01 15:50:03  meichel
** Updated copyright header
**
** Revision 1.36  2000/08/10 14:50:49  meichel
** Added initial OpenSSL support.
**
** Revision 1.35  2000/06/07 13:56:18  meichel
** Output stream now passed as mandatory parameter to ASC_dumpParameters.
**
** Revision 1.34  2000/06/07 08:58:10  meichel
** added optional paramter to DIMSE_storeUser that enables precise file size
**   information inside the store user callback.
**
** Revision 1.33  2000/04/14 16:29:27  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.32  2000/03/08 16:43:17  meichel
** Updated copyright header.
**
** Revision 1.31  2000/03/03 14:11:12  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.30  2000/02/29 11:49:50  meichel
** Removed support for VS value representation. This was proposed in CP 101
**   but never became part of the standard.
**
** Revision 1.29  2000/02/23 15:12:22  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.28  2000/02/03 11:50:09  meichel
** Moved UID related functions from dcmnet (diutil.h) to dcmdata (dcuid.h)
**   where they belong. Renamed access functions to dcmSOPClassUIDToModality
**   and dcmGuessModalityBytes.
**
** Revision 1.27  2000/02/02 14:41:19  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.26  2000/02/01 10:24:03  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.25  1999/07/14 12:03:35  meichel
** Updated data dictionary for supplement 29, 39, 33_lb, CP packet 4 and 5.
**   Corrected dcmtk applications for changes in attribute name constants.
**
** Revision 1.24  1999/04/30 16:40:23  meichel
** Minor code purifications to keep Sun CC 2.0.1 quiet
**
** Revision 1.23  1999/04/27 17:24:24  meichel
** Updated storescu and storescp for minor changes is command line class.
**
** Revision 1.22  1999/04/27 12:27:00  meichel
** Adapted storescu to new command line option scheme. Added support for
**   transmission of compressed images and on-the-fly creation of new UIDs.
**
** Revision 1.21  1999/03/29 11:19:56  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.20  1998/10/20 08:20:22  meichel
** Closed some memory leaks in dcmdata and dcmnet libraries.
**
** Revision 1.19  1998/04/02 14:46:46  joergr
** Corrected bug in command line parser of storescu.
**
** Revision 1.18  1998/01/14 14:35:17  hewett
** Added command line option (-ti) to restrict association negotiation
** to only propose the default Little Endian Implicit VR Transfer Syntax.
** Modified existing -u command line option to also disable generation
** of UT and VS (previously just disabled generation of UN).
**
** Revision 1.17  1997/08/06 12:23:01  andreas
** Change needed version number of WINSOCK to 1.1 to support WINDOWS 95
**
** Revision 1.16  1997/07/21 08:37:04  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.15  1997/05/29 15:52:58  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.14  1997/05/28 12:03:26  meichel
** Corrected spacing in usage message.
**
** Revision 1.13  1997/05/23 10:43:04  meichel
** Fixed a bug in storescu application: -u option did not work correctly.
**
** Revision 1.12  1997/05/22 13:30:00  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.11  1997/05/20 10:17:06  meichel
** *** empty log message ***
**
** Revision 1.10  1997/05/20 10:00:24  meichel
** Fixed bug in storescu which caused a segmentation fault when transmission
** of a non-image IOD (i.e. visit IOD) was attempted.
**
** Revision 1.9  1997/03/27 16:11:28  hewett
** Added command line switches allowing generation of UN to
** be disabled (it is enabled by default).
**
** Revision 1.8  1996/12/16 15:14:01  hewett
** Added bugfix for WINSOCK support.  The required WINSOCK version
** number was being incorrectly set to version 0.1.  The fixed
** WINSOCK initialisation now uses the MAKEWORD macro to correctly
** set the required version number. This bugfix was contributed
** by Dr. Yongjian Bao of Innomed GmbH, Germany.
**
** Revision 1.7  1996/09/27 14:05:06  hewett
** Added calls to initialise WINSOCK library for Win32 environment.  Only
** compiled in if HAVE_WINSOCK_H
**
** Revision 1.6  1996/09/27 08:24:30  hewett
** System header files now enclosed with BEGIN_EXTERN_C/END_EXTERN_C
**
** Revision 1.5  1996/09/24 16:21:17  hewett
** Now uses global table of Storage SOP Class UIDs (from dcuid.h).
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.4  1996/09/03 11:39:03  hewett
** Added copyright information.
**
** Revision 1.3  1996/04/25 16:19:18  hewett
** Added char* parameter casts to bzero() calls.
**
** Revision 1.2  1996/04/22 09:56:20  hewett
** Delete unused application conditions.
**
** Revision 1.1.1.1  1996/03/26 18:38:44  hewett
** Initial Release.
**
*/
