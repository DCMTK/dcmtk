/*
 *
 *  Copyright (C) 1996-2023, OFFIS e.V.
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
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

BEGIN_EXTERN_C
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
END_EXTERN_C

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmnet/dicom.h"      /* for DICOM_APPLICATION_REQUESTOR */
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dcmtrans.h"   /* for dcmSocketSend/ReceiveTimeout */
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
#include "dcmtk/dcmtls/tlsopt.h"     /* for DcmTLSOptions */
#include "dcmtk/ofstd/ofstdinc.h"
#include <ctime>


#ifdef ON_THE_FLY_COMPRESSION
#include "dcmtk/dcmjpeg/djdecode.h"  /* for JPEG decoders */
#include "dcmtk/dcmjpeg/djencode.h"  /* for JPEG encoders */
#include "dcmtk/dcmjpls/djdecode.h"  /* for JPEG-LS decoders */
#include "dcmtk/dcmjpls/djencode.h"  /* for JPEG-LS encoders */
#include "dcmtk/dcmdata/dcrledrg.h"  /* for RLE decoder */
#include "dcmtk/dcmdata/dcrleerg.h"  /* for RLE encoder */
#include "dcmtk/dcmjpeg/dipijpeg.h"  /* for dcmimage JPEG plugin */
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
static OFBool opt_renameFile = OFFalse;
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
static OFString patientNamePrefix("OFFIS^TEST_PN_");  // PatientName is PN (maximum 16 chars)
static OFString patientIDPrefix("PID_");              // PatientID is LO (maximum 64 chars)
static OFString studyIDPrefix("SID_");                // StudyID is SH (maximum 16 chars)
static OFString accessionNumberPrefix;                // AccessionNumber is SH (maximum 16 chars)
static const char *opt_configFile = NULL;
static const char *opt_profileName = NULL;
T_DIMSE_BlockingMode opt_blockMode = DIMSE_BLOCKING;
int opt_dimse_timeout = 0;
int opt_acse_timeout = 30;
OFCmdSignedInt opt_socket_timeout = 60;

#ifdef WITH_ZLIB
static OFCmdUnsignedInt opt_compressionLevel = 0;
#endif

// User Identity Negotiation
static T_ASC_UserIdentityNegotiationMode opt_identMode = ASC_USER_IDENTITY_NONE;
static OFString opt_user;
static OFString opt_password;
static OFString opt_identFile;
// Denotes whether we ask for an explicit server response for
// our identity negotiation request and only continue if this
// positive response is provided by the SCP.
static OFBool opt_identResponse = OFFalse;

static OFCondition
addStoragePresentationContexts(T_ASC_Parameters *params, OFList<OFString> &sopClasses);

static OFCondition
cstore(T_ASC_Association *assoc, const OFString &fname);

static OFBool
findSOPClassAndInstanceInFile(
  const char *fname,
  char *sopClass,
  size_t sopClassSize,
  char *sopInstance,
  size_t sopInstanceSize);

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
  DIC_NODENAME peerHost;
  T_ASC_Association *assoc;
  DcmAssociationConfiguration asccfg;  // handler for association configuration profiles
  DcmTLSOptions tlsOptions(NET_REQUESTOR);

  OFStandard::initializeNetwork();
#ifdef WITH_OPENSSL
  DcmTLSTransportLayer::initializeOpenSSL();
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
   OFLog::addOptions(cmd);
   cmd.addOption("--verbose-pc",              "+v",      "show presentation contexts in verbose mode");

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
      cmd.addOption("--no-rename",            "-rn",     "do not rename processed files (default)");
      cmd.addOption("--rename",               "+rn",     "append .done/.bad to processed files");
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
      cmd.addOption("--propose-mpeg2",        "-xm",     "propose MPEG2 Main Profile @ Main Level TS");
      cmd.addOption("--propose-mpeg2-high",   "-xh",     "propose MPEG2 Main Profile @ High Level TS");
      cmd.addOption("--propose-mpeg4",        "-xn",     "propose MPEG4 AVC/H.264 HP / Level 4.1 TS");
      cmd.addOption("--propose-mpeg4-bd",     "-xl",     "propose MPEG4 AVC/H.264 BD-compatible TS");
      cmd.addOption("--propose-mpeg4-2-2d",   "-x2",     "propose MPEG4 AVC/H.264 HP / Level 4.2 TS (2D)");
      cmd.addOption("--propose-mpeg4-2-3d",   "-x3",     "propose MPEG4 AVC/H.264 HP / Level 4.2 TS (3D)");
      cmd.addOption("--propose-mpeg4-2-st",   "-xo",     "propose MPEG4 AVC/H.264 Stereo / Level 4.2 TS");
      cmd.addOption("--propose-hevc",         "-x4",     "propose HEVC/H.265 Main Profile / Level 5.1 TS");
      cmd.addOption("--propose-hevc10",       "-x5",     "propose HEVC/H.265 Main 10 Profile / L5.1 TS");
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
      cmd.addOption("--jwt",                          1, "[f]ilename: string",
                                                         "read JWT data from file f");
      cmd.addOption("--pos-response",         "-rsp",    "expect positive response");
    cmd.addSubGroup("other network options:");
      cmd.addOption("--timeout",              "-to",  1, "[s]econds: integer (default: unlimited)", "timeout for connection requests");
      CONVERT_TO_STRING("[s]econds: integer (default: " << opt_socket_timeout << ")", optString1);
      cmd.addOption("--socket-timeout",       "-ts",  1, optString1.c_str(), "timeout for network socket (0 for none)");
      CONVERT_TO_STRING("[s]econds: integer (default: " << opt_acse_timeout << ")", optString2);
      cmd.addOption("--acse-timeout",         "-ta",  1, optString2.c_str(), "timeout for ACSE messages");
      cmd.addOption("--dimse-timeout",        "-td",  1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");
      CONVERT_TO_STRING("[n]umber of bytes: integer (" << ASC_MINIMUMPDUSIZE << ".." << ASC_MAXIMUMPDUSIZE << ")", optString3);
      CONVERT_TO_STRING("set max receive pdu to n bytes (default: " << opt_maxReceivePDULength << ")", optString4);
      cmd.addOption("--max-pdu",              "-pdu", 1, optString3.c_str(), optString4.c_str());
      cmd.addOption("--max-send-pdu",                 1, optString3.c_str(), "restrict max send pdu to n bytes");
      cmd.addOption("--repeat",                       1, "[n]umber: integer", "repeat n times");
      cmd.addOption("--abort",                           "abort association instead of releasing it");
      cmd.addOption("--no-halt",              "-nh",     "do not halt if unsuccessful store encountered\n(default: do halt)");
      cmd.addOption("--uid-padding",          "-up",     "silently correct space-padded UIDs");

      cmd.addOption("--invent-instance",      "+II",     "invent a new SOP instance UID for every image\nsent");
      CONVERT_TO_STRING("invent a new series UID after n images" << OFendl << "have been sent (default: " << opt_inventSeriesCount << ")", optString5);
      cmd.addOption("--invent-series",        "+IR",  1, "[n]umber: integer (implies --invent-instance)", optString5.c_str());
      CONVERT_TO_STRING("invent a new study UID after n series" << OFendl << "have been sent (default: " << opt_inventStudyCount << ")", optString6);
      cmd.addOption("--invent-study",         "+IS",  1, "[n]umber: integer (implies --invent-instance)", optString6.c_str());
      CONVERT_TO_STRING("invent a new patient ID and name after n studies" << OFendl << "have been sent (default: " << opt_inventPatientCount << ")", optString7);
      cmd.addOption("--invent-patient",       "+IP",  1, "[n]umber: integer (implies --invent-instance)", optString7.c_str());

    // add TLS specific command line options if (and only if) we are compiling with OpenSSL
    tlsOptions.addTLSCommandlineOptions(cmd);

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
          COUT << "- " << DJLSDecoderRegistration::getLibraryVersionString() << OFendl;
#endif
          // print OpenSSL version if (and only if) we are compiling with OpenSSL
          tlsOptions.printLibraryVersion();
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

      cmd.beginOptionBlock();
      if (cmd.findOption("--no-rename")) opt_renameFile = OFFalse;
      if (cmd.findOption("--rename")) opt_renameFile = OFTrue;
      cmd.endOptionBlock();

      if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
      if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));

      cmd.beginOptionBlock();
      if (cmd.findOption("--propose-uncompr")) opt_networkTransferSyntax = EXS_Unknown;
      if (cmd.findOption("--propose-little")) opt_networkTransferSyntax = EXS_LittleEndianExplicit;
      if (cmd.findOption("--propose-big")) opt_networkTransferSyntax = EXS_BigEndianExplicit;
      if (cmd.findOption("--propose-implicit")) opt_networkTransferSyntax = EXS_LittleEndianImplicit;
      if (cmd.findOption("--propose-lossless")) opt_networkTransferSyntax = EXS_JPEGProcess14SV1;
      if (cmd.findOption("--propose-jpeg8")) opt_networkTransferSyntax = EXS_JPEGProcess1;
      if (cmd.findOption("--propose-jpeg12")) opt_networkTransferSyntax = EXS_JPEGProcess2_4;
      if (cmd.findOption("--propose-j2k-lossless")) opt_networkTransferSyntax = EXS_JPEG2000LosslessOnly;
      if (cmd.findOption("--propose-j2k-lossy")) opt_networkTransferSyntax = EXS_JPEG2000;
      if (cmd.findOption("--propose-jls-lossless")) opt_networkTransferSyntax = EXS_JPEGLSLossless;
      if (cmd.findOption("--propose-jls-lossy")) opt_networkTransferSyntax = EXS_JPEGLSLossy;
      if (cmd.findOption("--propose-mpeg2")) opt_networkTransferSyntax = EXS_MPEG2MainProfileAtMainLevel;
      if (cmd.findOption("--propose-mpeg2-high")) opt_networkTransferSyntax = EXS_MPEG2MainProfileAtHighLevel;
      if (cmd.findOption("--propose-mpeg4")) opt_networkTransferSyntax = EXS_MPEG4HighProfileLevel4_1;
      if (cmd.findOption("--propose-mpeg4-bd")) opt_networkTransferSyntax = EXS_MPEG4BDcompatibleHighProfileLevel4_1;
      if (cmd.findOption("--propose-mpeg4-2-2d")) opt_networkTransferSyntax = EXS_MPEG4HighProfileLevel4_2_For2DVideo;
      if (cmd.findOption("--propose-mpeg4-2-3d")) opt_networkTransferSyntax = EXS_MPEG4HighProfileLevel4_2_For3DVideo;
      if (cmd.findOption("--propose-mpeg4-2-st")) opt_networkTransferSyntax = EXS_MPEG4StereoHighProfileLevel4_2;
      if (cmd.findOption("--propose-hevc")) opt_networkTransferSyntax = EXS_HEVCMainProfileLevel5_1;
      if (cmd.findOption("--propose-hevc10")) opt_networkTransferSyntax = EXS_HEVCMain10ProfileLevel5_1;
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
        app.checkConflict("--config-file", "--propose-lossless", opt_networkTransferSyntax == EXS_JPEGProcess14SV1);
        app.checkConflict("--config-file", "--propose-jpeg8", opt_networkTransferSyntax == EXS_JPEGProcess1);
        app.checkConflict("--config-file", "--propose-jpeg12", opt_networkTransferSyntax == EXS_JPEGProcess2_4);
        app.checkConflict("--config-file", "--propose-j2k-lossless", opt_networkTransferSyntax == EXS_JPEG2000LosslessOnly);
        app.checkConflict("--config-file", "--propose-j2k-lossy", opt_networkTransferSyntax == EXS_JPEG2000);
        app.checkConflict("--config-file", "--propose-jls-lossless", opt_networkTransferSyntax == EXS_JPEGLSLossless);
        app.checkConflict("--config-file", "--propose-jls-lossy", opt_networkTransferSyntax == EXS_JPEGLSLossy);
        app.checkConflict("--config-file", "--propose-mpeg2", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtMainLevel);
        app.checkConflict("--config-file", "--propose-mpeg2-high", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtHighLevel);
        app.checkConflict("--config-file", "--propose-mpeg4", opt_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_1);
        app.checkConflict("--config-file", "--propose-mpeg4-bd", opt_networkTransferSyntax == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
        app.checkConflict("--config-file", "--propose-mpeg4-2-2d", opt_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_2_For2DVideo);
        app.checkConflict("--config-file", "--propose-mpeg4-2-3d", opt_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_2_For3DVideo);
        app.checkConflict("--config-file", "--propose-mpeg4-2-st", opt_networkTransferSyntax == EXS_MPEG4StereoHighProfileLevel4_2);
        app.checkConflict("--config-file", "--propose-hevc", opt_networkTransferSyntax == EXS_HEVCMainProfileLevel5_1);
        app.checkConflict("--config-file", "--propose-hevc10", opt_networkTransferSyntax == EXS_HEVCMain10ProfileLevel5_1);
        app.checkConflict("--config-file", "--propose-rle", opt_networkTransferSyntax == EXS_RLELossless);
#ifdef WITH_ZLIB
        app.checkConflict("--config-file", "--propose-deflated", opt_networkTransferSyntax == EXS_DeflatedLittleEndianExplicit);
#endif
        app.checkConflict("--config-file", "--required", opt_proposeOnlyRequiredPresentationContexts);
        app.checkConflict("--config-file", "--combine", opt_combineProposedTransferSyntaxes);

        app.checkValue(cmd.getValue(opt_configFile));
        app.checkValue(cmd.getValue(opt_profileName));

        // read configuration file. The profile name is checked later.
        OFCondition cond = DcmAssociationConfigurationFile::initialize(asccfg, opt_configFile, OFTrue);
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
      if (cmd.findOption("--enable-new-vr")) dcmEnableGenerationOfNewVRs();
      if (cmd.findOption("--disable-new-vr")) dcmDisableGenerationOfNewVRs();
      cmd.endOptionBlock();

      if (cmd.findOption("--timeout"))
      {
        OFCmdSignedInt opt_timeout = 0;
        app.checkValue(cmd.getValueAndCheckMin(opt_timeout, 1));
        dcmConnectionTimeout.set(OFstatic_cast(Sint32, opt_timeout));
      }

      if (cmd.findOption("--socket-timeout"))
        app.checkValue(cmd.getValueAndCheckMin(opt_socket_timeout, -1));
      // always set the timeout values since the global default might be different
      dcmSocketSendTimeout.set(OFstatic_cast(Sint32, opt_socket_timeout));
      dcmSocketReceiveTimeout.set(OFstatic_cast(Sint32, opt_socket_timeout));

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

      // evaluate (most of) the TLS command line options (if we are compiling with OpenSSL)
      tlsOptions.parseArguments(app, cmd);

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
      if (cmd.findOption("--jwt"))
      {
        app.checkValue(cmd.getValue(opt_identFile));
        opt_identMode = ASC_USER_IDENTITY_JWT;
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
        opt_password = "";
        opt_identMode = ASC_USER_IDENTITY_USER_PASSWORD;
      }
      cmd.endOptionBlock();
      if (cmd.findOption("--pos-response"))
      {
         app.checkDependence("--pos-response", "--user, --kerberos, --saml or --jwt", opt_identMode != ASC_USER_IDENTITY_NONE);
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
          if (!findSOPClassAndInstanceInFile(currentFilename, sopClassUID, sizeof(sopClassUID), sopInstanceUID, sizeof(sopInstanceUID)))
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
          else if (!dcmIsaStorageSOPClassUID(sopClassUID, ESSC_All))
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

    // register JPEG-LS decompression codecs
    DJLSDecoderRegistration::registerCodecs();

    // register JPEG-LS compression codecs
    DJLSEncoderRegistration::registerCodecs();

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

    /* initialize association parameters, i.e. create an instance of T_ASC_Parameters*. */
    cond = ASC_createAssociationParameters(&params, opt_maxReceivePDULength, dcmConnectionTimeout.get());
    if (cond.bad()) {
      OFLOG_FATAL(storescuLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }

    /* create a secure transport layer if requested and OpenSSL is available */
    cond = tlsOptions.createTransportLayer(net, params, app, cmd);
    if (cond.bad()) {
        OFLOG_FATAL(storescuLogger, DimseCondition::dump(temp_str, cond));
        return 1;
    }

    /* sets this application's title and the called application's title in the params */
    /* structure. The default values to be set here are "STORESCU" and "ANY-SCP". */
    ASC_setAPTitles(params, opt_ourTitle, opt_peerTitle, NULL);

    /* Figure out the presentation addresses and copy the */
    /* corresponding values into the association parameters.*/
    sprintf(peerHost, "%s:%d", opt_peer, OFstatic_cast(int, opt_port));
    ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), peerHost);

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
      }
      return 1;
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

    OFStandard::shutdownNetwork();

    cond = tlsOptions.writeRandomSeed();
    if (cond.bad()) {
        // failure to write back the random seed is a warning, not an error
        OFLOG_WARN(storescuLogger, DimseCondition::dump(temp_str, cond));
    }

    int exitCode = 0;
    if (opt_haltOnUnsuccessfulStore && unsuccessfulStoreEncountered) {
      if (lastStatusCode == STATUS_Success) {
        // there must have been some kind of general network error
        exitCode = 10;
      } else {
        exitCode = (lastStatusCode >> 8); // only the least significant byte is relevant as exit code
      }
    }

#ifdef ON_THE_FLY_COMPRESSION
    // deregister JPEG codecs
    DJDecoderRegistration::cleanup();
    DJEncoderRegistration::cleanup();

    // deregister JPEG-LS codecs
    DJLSDecoderRegistration::cleanup();
    DJLSEncoderRegistration::cleanup();

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
  OFCondition cond = ASC_addPresentationContext(params, OFstatic_cast(T_ASC_PresentationContextID, presentationContextId),
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

  OFCondition cond = ASC_addPresentationContext(params, OFstatic_cast(T_ASC_PresentationContextID, presentationContextId),
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
   * This preferred transfer syntax will be proposed in one
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
  // - If MPEG2, MPEG4 or HEVC is preferred, we don't want to propose any fallback solution
  //   because this is not required and we cannot decompress the movie anyway.
  if ((opt_networkTransferSyntax != EXS_LittleEndianImplicit) &&
      (opt_networkTransferSyntax != EXS_MPEG2MainProfileAtMainLevel) &&
      (opt_networkTransferSyntax != EXS_MPEG2MainProfileAtHighLevel) &&
      (opt_networkTransferSyntax != EXS_MPEG4HighProfileLevel4_1) &&
      (opt_networkTransferSyntax != EXS_MPEG4BDcompatibleHighProfileLevel4_1) &&
      (opt_networkTransferSyntax != EXS_MPEG4HighProfileLevel4_2_For2DVideo) &&
      (opt_networkTransferSyntax != EXS_MPEG4HighProfileLevel4_2_For3DVideo) &&
      (opt_networkTransferSyntax != EXS_MPEG4StereoHighProfileLevel4_2) &&
      (opt_networkTransferSyntax != EXS_HEVCMainProfileLevel5_1) &&
      (opt_networkTransferSyntax != EXS_HEVCMain10ProfileLevel5_1))
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

  if (seriesInstanceUID.empty()) seriesInstanceUID=makeUID(SITE_SERIES_UID_ROOT, OFstatic_cast(int, seriesCounter));
  if (seriesNumber.empty()) seriesNumber = intToString(OFstatic_cast(int, seriesCounter));
  if (studyInstanceUID.empty()) studyInstanceUID = makeUID(SITE_STUDY_UID_ROOT, OFstatic_cast(int, studyCounter));
  if (studyID.empty()) studyID = studyIDPrefix + intToString(OFstatic_cast(int, secondsSince1970())) + intToString(OFstatic_cast(int, studyCounter));
  if (accessionNumber.empty()) accessionNumber = accessionNumberPrefix + intToString(secondsSince1970()) + intToString(OFstatic_cast(int, studyCounter));
  if (patientID.empty()) patientID = patientIDPrefix + intToString(secondsSince1970()) + intToString(OFstatic_cast(int, patientCounter));
  if (patientName.empty()) patientName = patientNamePrefix + intToString(secondsSince1970()) + intToString(OFstatic_cast(int, patientCounter));

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
    OFLOG_DEBUG(storescuLogger, DIMSE_dumpMessage(str, *req, DIMSE_OUTGOING /*, NULL, presID */));
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

static void
renameFile(const char *fname, const char *fext)
{
  if (!opt_renameFile) return;
  OFString fnewname(fname);
  fnewname += fext;
  if (OFStandard::renameFile(fname, fnewname))
    OFLOG_DEBUG(storescuLogger, "renamed file '" << fname << "' to '" << fnewname << "'");
  else
    OFLOG_WARN(storescuLogger, "cannot rename file '" << fname << "' to '" << fnewname << "'");
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

  /* figure out if an error occurred while the file was read */
  if (cond.bad())
  {
    OFLOG_ERROR(storescuLogger, "Bad DICOM file: " << fname << ": " << cond.text());
    renameFile(fname, ".bad");
    return cond;
  }

  /* if required, invent new SOP instance information for the current data set (user option) */
  if (opt_inventSOPInstanceInformation) {
    replaceSOPInstanceInformation(dcmff.getDataset());
  }

  /* figure out which SOP class and SOP instance is encapsulated in the file */
  if (!DU_findSOPClassAndInstanceInDataSet(dcmff.getDataset(),
    sopClass, sizeof(sopClass), sopInstance, sizeof(sopInstance), opt_correctUIDPadding))
  {
    OFLOG_ERROR(storescuLogger, "No SOP Class or Instance UID in file: " << fname);
    renameFile(fname, ".bad");
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

  if (filexfer != EXS_Unknown)
    presID = ASC_findAcceptedPresentationContextID(assoc, sopClass, filexfer.getXferID());
  else
    presID = ASC_findAcceptedPresentationContextID(assoc, sopClass);
  if (presID == 0)
  {
    if (storescuLogger.isEnabledFor(OFLogger::ERROR_LOG_LEVEL))
    {
      const char *modalityName = dcmSOPClassUIDToModality(sopClass);
      if (!modalityName) modalityName = dcmFindNameOfUID(sopClass, "unknown SOP class");
      OFLOG_ERROR(storescuLogger, "No presentation context for: (" << modalityName << ") " << sopClass);
    }
    renameFile(fname, ".bad");
    return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
  }

  T_ASC_PresentationContext pc;
  ASC_findAcceptedPresentationContext(assoc->params, presID, &pc);
  DcmXfer netTransfer(pc.acceptedTransferSyntax);

  /* if required, dump general information concerning transfer syntaxes */
  if ((netTransfer != dcmff.getDataset()->getOriginalXfer()) &&
    storescuLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
  {
    DcmXfer origTransfer(dcmff.getDataset()->getOriginalXfer());
    OFLOG_INFO(storescuLogger, "Converting transfer syntax: " << origTransfer.getXferName()
      << " -> " << netTransfer.getXferName());
  }

#ifdef ON_THE_FLY_COMPRESSION
  cond = dcmff.getDataset()->chooseRepresentation(netTransfer.getXfer(), NULL);
  if (cond.bad())
  {
    OFLOG_ERROR(storescuLogger, "No conversion to transfer syntax " << netTransfer.getXferName() << " possible!");
    renameFile(fname, ".bad");
    return cond;
  }
#endif

  /* prepare the transmission of data */
  memset(OFreinterpret_cast(char *, &req), 0, sizeof(req));
  req.MessageID = msgId;
  OFStandard::strlcpy(req.AffectedSOPClassUID, sopClass, sizeof(req.AffectedSOPClassUID));
  OFStandard::strlcpy(req.AffectedSOPInstanceUID, sopInstance, sizeof(req.AffectedSOPInstanceUID));
  req.DataSetType = DIMSE_DATASET_PRESENT;
  req.Priority = DIMSE_PRIORITY_MEDIUM;

  /* if required, dump some more general information */
  OFLOG_INFO(storescuLogger, "Sending Store Request (MsgID " << msgId << ", "
    << dcmSOPClassUIDToModality(sopClass, "OT") << ")");
  /* the following is needed, because the presentation context ID is not shown otherwise */
  if (storescuLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    const char *sopClassName = dcmFindKeywordOfUID(sopClass, sopClass /*defaultValue*/);
    const char *netXferName = dcmFindKeywordOfUID(netTransfer.getXferID(), netTransfer.getXferID() /*defaultValue*/);
    OFLOG_DEBUG(storescuLogger, "  using Presentation Context ID " << OFstatic_cast(int, presID)
      << " = " << sopClassName << " with " << netXferName << " transfer syntax");
  }

  /* finally conduct transmission of data */
  cond = DIMSE_storeUser(assoc, presID, &req,
    NULL, dcmff.getDataset(), progressCallback, NULL,
    opt_blockMode, opt_dimse_timeout,
    &rsp, &statusDetail, NULL, OFstatic_cast(long, OFStandard::getFileSize(fname)));

  /*
   * If store command completed normally, with a status
   * of success or some warning then the image was accepted.
   */
  if (cond == EC_Normal && (rsp.DimseStatus == STATUS_Success || DICOM_WARNING_STATUS(rsp.DimseStatus)))
  {
    unsuccessfulStoreEncountered = OFFalse;
    renameFile(fname, ".done");
  }
  else
  {
    renameFile(fname, ".bad");
  }

  /* remember the response's status for later transmissions of data */
  lastStatusCode = rsp.DimseStatus;

  /* dump some more general information */
  if (cond == EC_Normal)
  {
    OFString temp_str;
    if (storescuLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
      OFLOG_INFO(storescuLogger, "Received Store Response");
      OFLOG_DEBUG(storescuLogger, DIMSE_dumpMessage(temp_str, rsp, DIMSE_INCOMING, NULL, presID));
    } else {
      OFLOG_INFO(storescuLogger, "Received Store Response (" << DU_cstoreStatusString(rsp.DimseStatus) << ")");
    }
  }
  else
  {
    OFString temp_str;
    OFLOG_ERROR(storescuLogger, "Store Failed, file: " << fname << ":" << OFendl << DimseCondition::dump(temp_str, cond));
  }

  /* dump status detail information if there is some */
  if (statusDetail != NULL) {
    OFLOG_DEBUG(storescuLogger, "Status Detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
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

  /* as long as no error occurred and the counter does not equal 0 */
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
  size_t sopClassSize,
  char *sopInstance,
  size_t sopInstanceSize)
{
    DcmFileFormat ff;
    if (!ff.loadFile(fname, EXS_Unknown, EGL_noChange, DCM_MaxReadLength, opt_readMode).good())
        return OFFalse;

    /* look in the meta-header first */
    OFBool found = DU_findSOPClassAndInstanceInDataSet(ff.getMetaInfo(), sopClass, sopClassSize, sopInstance, sopInstanceSize, opt_correctUIDPadding);

    if (!found)
      found = DU_findSOPClassAndInstanceInDataSet(ff.getDataset(), sopClass, sopClassSize, sopInstance, sopInstanceSize, opt_correctUIDPadding);

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
    case ASC_USER_IDENTITY_JWT:
    {
      OFFile identFile;
      if (!identFile.fopen(opt_identFile.c_str(), "rb"))
      {
        OFString openerror;
        identFile.getLastErrorString(openerror);
        OFLOG_ERROR(storescuLogger, "Unable to open Kerberos, SAML or JWT file: " << openerror);
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
        OFLOG_INFO(storescuLogger, "Kerberos, SAML or JWT file is larger than 65535 bytes, bytes after that position are ignored");
        filesize = 65535;
      }

      char *buf = new char[OFstatic_cast(unsigned int, filesize)];
      size_t bytesRead = identFile.fread(buf, 1, OFstatic_cast(size_t, filesize));
      identFile.fclose();
      if (bytesRead == 0)
      {
        OFLOG_ERROR(storescuLogger, "Unable to read Kerberos, SAML or JWT info from file: File empty?");
        delete[] buf;
        return EC_IllegalCall;
      }
      // Casting to Uint16 should be safe since it is checked above that file
      // size does not exceed 65535 bytes.
      if (opt_identMode == ASC_USER_IDENTITY_KERBEROS)
        cond = ASC_setIdentRQKerberos(params, buf, OFstatic_cast(Uint16,bytesRead), opt_identResponse);
      else if (opt_identMode == ASC_USER_IDENTITY_SAML)
        cond = ASC_setIdentRQSaml(params, buf, OFstatic_cast(Uint16,bytesRead), opt_identResponse);
      else // JWT
        cond = ASC_setIdentRQJwt(params, buf, OFstatic_cast(Uint16,bytesRead), opt_identResponse);
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
