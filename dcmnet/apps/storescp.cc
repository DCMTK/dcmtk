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
 *  Purpose: Storage Service Class Provider (C-STORE operation)
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CCTYPE
#define INCLUDE_CSIGNAL
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>       /* needed on Solaris for O_RDONLY */
#endif
#ifdef HAVE_SIGNAL_H
// On Solaris with Sun Workshop 11, <signal.h> declares signal() but <csignal> does not
#include <signal.h>
#endif
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <direct.h>      /* for _mkdir() */
#endif

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/dcmnet/dicom.h"         /* for DICOM_APPLICATION_ACCEPTOR */
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dcasccfg.h"      /* for class DcmAssociationConfiguration */
#include "dcmtk/dcmnet/dcasccff.h"      /* for class DcmAssociationConfigurationFile */
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcuid.h"        /* for dcmtk version name */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcostrmz.h"     /* for dcmZlibCompressionLevel */

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

// we assume that the inetd super server is available on all non-Windows systems
#ifndef _WIN32
#define INETD_AVAILABLE
#endif


#ifdef PRIVATE_STORESCP_DECLARATIONS
PRIVATE_STORESCP_DECLARATIONS
#else
#define OFFIS_CONSOLE_APPLICATION "storescp"
#endif

static OFLogger storescpLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v" OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define APPLICATIONTITLE "STORESCP"     /* our application entity title */

#define PATH_PLACEHOLDER "#p"
#define FILENAME_PLACEHOLDER "#f"
#define CALLING_AETITLE_PLACEHOLDER "#a"
#define CALLED_AETITLE_PLACEHOLDER "#c"
#define CALLING_PRESENTATION_ADDRESS_PLACEHOLDER "#r"

static OFCondition processCommands(T_ASC_Association *assoc);
static OFCondition acceptAssociation(T_ASC_Network *net, DcmAssociationConfiguration& asccfg);
static OFCondition echoSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID);
static OFCondition storeSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID);
static void executeOnReception();
static void executeEndOfStudyEvents();
static void executeOnEndOfStudy();
static void renameOnEndOfStudy();
static OFString replaceChars( const OFString &srcstr, const OFString &pattern, const OFString &substitute );
static void executeCommand( const OFString &cmd );
static void cleanChildren(pid_t pid, OFBool synch);
static OFCondition acceptUnknownContextsWithPreferredTransferSyntaxes(
         T_ASC_Parameters * params,
         const char* transferSyntaxes[],
         int transferSyntaxCount,
         T_ASC_SC_ROLE acceptedRole = ASC_SC_ROLE_DEFAULT);

/* sort study mode */
enum E_SortStudyMode
{
    ESM_None,
    ESM_Timestamp,
    ESM_StudyInstanceUID,
    ESM_PatientName
};

OFBool             opt_showPresentationContexts = OFFalse;
OFBool             opt_uniqueFilenames = OFFalse;
OFString           opt_fileNameExtension;
OFBool             opt_timeNames = OFFalse;
int                timeNameCounter = -1;   // "serial number" to differentiate between files with same timestamp
OFCmdUnsignedInt   opt_port = 0;
OFBool             opt_refuseAssociation = OFFalse;
OFBool             opt_rejectWithoutImplementationUID = OFFalse;
OFCmdUnsignedInt   opt_sleepAfter = 0;
OFCmdUnsignedInt   opt_sleepDuring = 0;
OFCmdUnsignedInt   opt_maxPDU = ASC_DEFAULTMAXPDU;
OFBool             opt_useMetaheader = OFTrue;
OFBool             opt_acceptAllXfers = OFFalse;
E_TransferSyntax   opt_networkTransferSyntax = EXS_Unknown;
E_TransferSyntax   opt_writeTransferSyntax = EXS_Unknown;
E_GrpLenEncoding   opt_groupLength = EGL_recalcGL;
E_EncodingType     opt_sequenceType = EET_ExplicitLength;
E_PaddingEncoding  opt_paddingType = EPD_withoutPadding;
OFCmdUnsignedInt   opt_filepad = 0;
OFCmdUnsignedInt   opt_itempad = 0;
OFCmdUnsignedInt   opt_compressionLevel = 0;
OFBool             opt_bitPreserving = OFFalse;
OFBool             opt_ignore = OFFalse;
OFBool             opt_abortDuringStore = OFFalse;
OFBool             opt_abortAfterStore = OFFalse;
OFBool             opt_promiscuous = OFFalse;
OFBool             opt_correctUIDPadding = OFFalse;
OFBool             opt_inetd_mode = OFFalse;
OFString           callingAETitle;                    // calling application entity title will be stored here
OFString           lastCallingAETitle;
OFString           calledAETitle;                     // called application entity title will be stored here
OFString           lastCalledAETitle;
OFString           callingPresentationAddress;        // remote hostname or IP address will be stored here
OFString           lastCallingPresentationAddress;
const char *       opt_respondingAETitle = APPLICATIONTITLE;
static OFBool      opt_secureConnection = OFFalse;    // default: no secure connection
static OFString    opt_outputDirectory = ".";         // default: output directory equals "."
E_SortStudyMode    opt_sortStudyMode = ESM_None;      // default: no sorting
static const char *opt_sortStudyDirPrefix = NULL;     // default: no directory prefix
OFString           lastStudyInstanceUID;
OFString           subdirectoryPathAndName;
OFList<OFString>   outputFileNameArray;
static const char *opt_execOnReception = NULL;        // default: don't execute anything on reception
static const char *opt_execOnEndOfStudy = NULL;       // default: don't execute anything on end of study

OFString           lastStudySubdirectoryPathAndName;
static OFBool      opt_renameOnEndOfStudy = OFFalse;  // default: don't rename any files on end of study
static long        opt_endOfStudyTimeout = -1;        // default: no end of study timeout
static OFBool      endOfStudyThroughTimeoutEvent = OFFalse;
static const char *opt_configFile = NULL;
static const char *opt_profileName = NULL;
T_DIMSE_BlockingMode opt_blockMode = DIMSE_BLOCKING;
int                opt_dimse_timeout = 0;
int                opt_acse_timeout = 30;

#if defined(HAVE_FORK) || defined(_WIN32)
OFBool             opt_forkMode = OFFalse;
#endif

OFBool             opt_forkedChild = OFFalse;
OFBool             opt_execSync = OFFalse;            // default: execute in background

#ifdef WITH_OPENSSL
static int         opt_keyFileFormat = SSL_FILETYPE_PEM;
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


#ifdef HAVE_WAITPID
/** signal handler for SIGCHLD signals that immediately cleans up
 *  terminated children.
 */
#ifdef SIGNAL_HANDLER_WITH_ELLIPSE
extern "C" void sigChildHandler(...)
#else
extern "C" void sigChildHandler(int)
#endif
{
  int status = 0;
  waitpid( -1, &status, WNOHANG );
  signal(SIGCHLD, sigChildHandler);
}
#endif


#define SHORTCOL 4
#define LONGCOL 21

int main(int argc, char *argv[])
{
  T_ASC_Network *net;
  DcmAssociationConfiguration asccfg;

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
  OFString temp_str;
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, "DICOM storage (C-STORE) SCP", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  cmd.addParam("port", "tcp/ip port number to listen on", OFCmdParam::PM_Optional);

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--help",                     "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
    cmd.addOption("--version",                             "print version information and exit", OFCommandLine::AF_Exclusive);
    OFLog::addOptions(cmd);
    cmd.addOption("--verbose-pc",               "+v",      "show presentation contexts in verbose mode");

#if defined(HAVE_FORK) || defined(_WIN32)
  cmd.addGroup("multi-process options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--single-process",                      "single process mode (default)");
    cmd.addOption("--fork",                                "fork child process for each association");
#ifdef _WIN32
    cmd.addOption("--forked-child",                        "process is forked child, internal use only", OFCommandLine::AF_Internal);
#endif
#endif

  cmd.addGroup("network options:");
    cmd.addSubGroup("association negotiation profile from configuration file:");
      cmd.addOption("--config-file",            "-xf",  2, "[f]ilename, [p]rofile: string",
                                                           "use profile p from config file f");
    cmd.addSubGroup("preferred network transfer syntaxes (not with --config-file):");
      cmd.addOption("--prefer-uncompr",         "+x=",     "prefer explicit VR local byte order (default)");
      cmd.addOption("--prefer-little",          "+xe",     "prefer explicit VR little endian TS");
      cmd.addOption("--prefer-big",             "+xb",     "prefer explicit VR big endian TS");
      cmd.addOption("--prefer-lossless",        "+xs",     "prefer default JPEG lossless TS");
      cmd.addOption("--prefer-jpeg8",           "+xy",     "prefer default JPEG lossy TS for 8 bit data");
      cmd.addOption("--prefer-jpeg12",          "+xx",     "prefer default JPEG lossy TS for 12 bit data");
      cmd.addOption("--prefer-j2k-lossless",    "+xv",     "prefer JPEG 2000 lossless TS");
      cmd.addOption("--prefer-j2k-lossy",       "+xw",     "prefer JPEG 2000 lossy TS");
      cmd.addOption("--prefer-jls-lossless",    "+xt",     "prefer JPEG-LS lossless TS");
      cmd.addOption("--prefer-jls-lossy",       "+xu",     "prefer JPEG-LS lossy TS");
      cmd.addOption("--prefer-mpeg2",           "+xm",     "prefer MPEG2 Main Profile @ Main Level TS");
      cmd.addOption("--prefer-mpeg2-high",      "+xh",     "prefer MPEG2 Main Profile @ High Level TS");
      cmd.addOption("--prefer-mpeg4",           "+xn",     "prefer MPEG4 AVC/H.264 HP / Level 4.1 TS");
      cmd.addOption("--prefer-mpeg4-bd",        "+xl",     "prefer MPEG4 AVC/H.264 BD-compatible TS");
      cmd.addOption("--prefer-rle",             "+xr",     "prefer RLE lossless TS");
#ifdef WITH_ZLIB
      cmd.addOption("--prefer-deflated",        "+xd",     "prefer deflated expl. VR little endian TS");
#endif
      cmd.addOption("--implicit",               "+xi",     "accept implicit VR little endian TS only");
      cmd.addOption("--accept-all",             "+xa",     "accept all supported transfer syntaxes");

#ifdef WITH_TCPWRAPPER
    cmd.addSubGroup("network host access control (tcp wrapper):");
      cmd.addOption("--access-full",            "-ac",     "accept connections from any host (default)");
      cmd.addOption("--access-control",         "+ac",     "enforce host access control rules");
#endif

    cmd.addSubGroup("other network options:");
#ifdef INETD_AVAILABLE
      // this option is only offered on Posix platforms
      cmd.addOption("--inetd",                  "-id",     "run from inetd super server (not with --fork)");
#endif

      cmd.addOption("--acse-timeout",           "-ta",  1, "[s]econds: integer (default: 30)", "timeout for ACSE messages");
      cmd.addOption("--dimse-timeout",          "-td",  1, "[s]econds: integer (default: unlimited)", "timeout for DIMSE messages");

      OFString opt1 = "set my AE title (default: ";
      opt1 += APPLICATIONTITLE;
      opt1 += ")";
      cmd.addOption("--aetitle",                "-aet", 1, "[a]etitle: string", opt1.c_str());
      OFString opt3 = "set max receive pdu to n bytes (def.: ";
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
      cmd.addOption("--max-pdu",                "-pdu", 1, opt4.c_str(), opt3.c_str());
      cmd.addOption("--disable-host-lookup",    "-dhl",    "disable hostname lookup");
      cmd.addOption("--refuse",                            "refuse association");
      cmd.addOption("--reject",                            "reject association if no implement. class UID");
      cmd.addOption("--ignore",                            "ignore store data, receive but do not store");
      cmd.addOption("--sleep-after",                    1, "[s]econds: integer",
                                                           "sleep s seconds after store (default: 0)");
      cmd.addOption("--sleep-during",                   1, "[s]econds: integer",
                                                           "sleep s seconds during store (default: 0)");
      cmd.addOption("--abort-after",                       "abort association after receipt of C-STORE-RQ\n(but before sending response)");
      cmd.addOption("--abort-during",                      "abort association during receipt of C-STORE-RQ");
      cmd.addOption("--promiscuous",            "-pm",     "promiscuous mode, accept unknown SOP classes\n(not with --config-file)");
      cmd.addOption("--uid-padding",            "-up",     "silently correct space-padded UIDs");

#ifdef WITH_OPENSSL
  cmd.addGroup("transport layer security (TLS) options:");
    cmd.addSubGroup("transport protocol stack:");
      cmd.addOption("--disable-tls",            "-tls",    "use normal TCP/IP connection (default)");
      cmd.addOption("--enable-tls",             "+tls", 2, "[p]rivate key file, [c]ertificate file: string",
                                                           "use authenticated secure TLS connection");
    cmd.addSubGroup("private key password (only with --enable-tls):");
      cmd.addOption("--std-passwd",             "+ps",     "prompt user to type password on stdin (default)");
      cmd.addOption("--use-passwd",             "+pw",  1, "[p]assword: string",
                                                           "use specified password");
      cmd.addOption("--null-passwd",            "-pw",     "use empty string as password");
    cmd.addSubGroup("key and certificate file format:");
      cmd.addOption("--pem-keys",               "-pem",    "read keys and certificates as PEM file (def.)");
      cmd.addOption("--der-keys",               "-der",    "read keys and certificates as DER file");
    cmd.addSubGroup("certification authority:");
      cmd.addOption("--add-cert-file",          "+cf",  1, "[c]ertificate filename: string",
                                                           "add certificate file to list of certificates", OFCommandLine::AF_NoWarning);
      cmd.addOption("--add-cert-dir",           "+cd",  1, "[c]ertificate directory: string",
                                                           "add certificates in d to list of certificates", OFCommandLine::AF_NoWarning);
    cmd.addSubGroup("ciphersuite:");
      cmd.addOption("--cipher",                 "+cs",  1, "[c]iphersuite name: string",
                                                           "add ciphersuite to list of negotiated suites");
      cmd.addOption("--dhparam",                "+dp",  1, "[f]ilename: string",
                                                           "read DH parameters for DH/DSS ciphersuites");
    cmd.addSubGroup("pseudo random generator:");
      cmd.addOption("--seed",                   "+rs",  1, "[f]ilename: string",
                                                           "seed random generator with contents of f");
      cmd.addOption("--write-seed",             "+ws",     "write back modified seed (only with --seed)");
      cmd.addOption("--write-seed-file",        "+wf",  1, "[f]ilename: string (only with --seed)",
                                                           "write modified seed to file f");
    cmd.addSubGroup("peer authentication");
      cmd.addOption("--require-peer-cert",      "-rc",     "verify peer certificate, fail if absent (def.)");
      cmd.addOption("--verify-peer-cert",       "-vc",     "verify peer certificate if present");
      cmd.addOption("--ignore-peer-cert",       "-ic",     "don't verify peer certificate");
#endif

  cmd.addGroup("output options:");
    cmd.addSubGroup("general:");
      cmd.addOption("--output-directory",       "-od",  1, "[d]irectory: string (default: \".\")", "write received objects to existing directory d");
    cmd.addSubGroup("bit preserving mode:");
      cmd.addOption("--normal",                 "-B",      "allow implicit format conversions (default)");
      cmd.addOption("--bit-preserving",         "+B",      "write data exactly as read");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",             "+F",      "write file format (default)");
      cmd.addOption("--write-dataset",          "-F",      "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax (not with --bit-preserving or compressed transmission):");
      cmd.addOption("--write-xfer-same",        "+t=",     "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",      "+te",     "write with explicit VR little endian TS");
      cmd.addOption("--write-xfer-big",         "+tb",     "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit",    "+ti",     "write with implicit VR little endian TS");
#ifdef WITH_ZLIB
      cmd.addOption("--write-xfer-deflated",    "+td",     "write with deflated expl. VR little endian TS");
#endif
    cmd.addSubGroup("post-1993 value representations (not with --bit-preserving):");
      cmd.addOption("--enable-new-vr",          "+u",      "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",         "-u",      "disable support for new VRs, convert to OB");
    cmd.addSubGroup("group length encoding (not with --bit-preserving):");
      cmd.addOption("--group-length-recalc",    "+g=",     "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create",    "+g",      "always write with group length elements");
      cmd.addOption("--group-length-remove",    "-g",      "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items (not with --bit-preserving):");
      cmd.addOption("--length-explicit",        "+e",      "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",       "-e",      "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset or --bit-preserving):");
      cmd.addOption("--padding-off",            "-p",      "no padding (default)");
      cmd.addOption("--padding-create",         "+p",   2, "[f]ile-pad [i]tem-pad: integer",
                                                           "align file on multiple of f bytes and items\non multiple of i bytes");
#ifdef WITH_ZLIB
    cmd.addSubGroup("deflate compression level (only with --write-xfer-deflated/same):");
      cmd.addOption("--compression-level",      "+cl",  1, "[l]evel: integer (default: 6)",
                                                           "0=uncompressed, 1=fastest, 9=best compression");
#endif
    cmd.addSubGroup("sorting into subdirectories (not with --bit-preserving):");
      cmd.addOption("--sort-conc-studies",      "-ss",  1, "[p]refix: string",
                                                           "sort studies using prefix p and a timestamp");
      cmd.addOption("--sort-on-study-uid",      "-su",  1, "[p]refix: string",
                                                           "sort studies using prefix p and the Study\nInstance UID");
      cmd.addOption("--sort-on-patientname",    "-sp",     "sort studies using the Patient's Name and\na timestamp");

    cmd.addSubGroup("filename generation:");
      cmd.addOption("--default-filenames",      "-uf",     "generate filename from instance UID (default)");
      cmd.addOption("--unique-filenames",       "+uf",     "generate unique filenames");
      cmd.addOption("--timenames",              "-tn",     "generate filename from creation time");
      cmd.addOption("--filename-extension",     "-fe",  1, "[e]xtension: string",
                                                           "append e to all filenames");

  cmd.addGroup("event options:", LONGCOL, SHORTCOL + 2);
    cmd.addOption("--exec-on-reception",        "-xcr", 1, "[c]ommand: string",
                                                           "execute command c after having received and\nprocessed one C-STORE-RQ message");
    cmd.addOption("--exec-on-eostudy",          "-xcs", 1, "[c]ommand: string",
                                                           "execute command c after having received and\nprocessed all C-STORE-RQ messages that belong\nto one study");
    cmd.addOption("--rename-on-eostudy",        "-rns",    "having received and processed all C-STORE-RQ\nmessages that belong to one study, rename\noutput files according to certain pattern");
    cmd.addOption("--eostudy-timeout",          "-tos", 1, "[t]imeout: integer",
                                                           "specifies a timeout of t seconds for\nend-of-study determination");
    cmd.addOption("--exec-sync",                "-xs",     "execute command synchronously in foreground");

  /* evaluate command line */
  prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
  if (app.parseCommandLine(cmd, argc, argv))
  {
    /* print help text and exit */
    if (cmd.getArgCount() == 0)
      app.printUsage();

    /* check exclusive options first */
    if (cmd.hasExclusiveOption())
    {
      if (cmd.findOption("--version"))
      {
        app.printHeader(OFTrue /*print host identifier*/);
        COUT << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_OPENSSL) && !defined(WITH_TCPWRAPPER)
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
#ifdef WITH_TCPWRAPPER
        COUT << "- LIBWRAP" << OFendl;
#endif
        return 0;
      }
    }

#ifdef INETD_AVAILABLE
    if (cmd.findOption("--inetd"))
    {
      opt_inetd_mode = OFTrue;
      opt_forkMode = OFFalse;

      // duplicate stdin, which is the socket passed by inetd
      int inetd_fd = dup(0);
      if (inetd_fd < 0) exit(99);

      close(0); // close stdin
      close(1); // close stdout
      close(2); // close stderr

      // open new file descriptor for stdin
      int fd = open("/dev/null", O_RDONLY);
      if (fd != 0) exit(99);

      // create new file descriptor for stdout
      fd = open("/dev/null", O_WRONLY);
      if (fd != 1) exit(99);

      // create new file descriptor for stderr
      fd = open("/dev/null", O_WRONLY);
      if (fd != 2) exit(99);

      dcmExternalSocketHandle.set(inetd_fd);

      // the port number is not really used. Set to non-privileged port number
      // to avoid failing the privilege test.
      opt_port = 1024;
    }
#endif

#if defined(HAVE_FORK) || defined(_WIN32)
    cmd.beginOptionBlock();
    if (cmd.findOption("--single-process"))
      opt_forkMode = OFFalse;
    if (cmd.findOption("--fork"))
    {
      app.checkConflict("--inetd", "--fork", opt_inetd_mode);
      opt_forkMode = OFTrue;
    }
    cmd.endOptionBlock();
#ifdef _WIN32
    if (cmd.findOption("--forked-child")) opt_forkedChild = OFTrue;
#endif
#endif

    if (opt_inetd_mode)
    {
      // port number is not required in inetd mode
      if (cmd.getParamCount() > 0)
        OFLOG_WARN(storescpLogger, "Parameter port not required in inetd mode");
    } else {
      // omitting the port number is only allowed in inetd mode
      if (cmd.getParamCount() == 0)
        app.printError("Missing parameter port");
      else
        app.checkParam(cmd.getParamAndCheckMinMax(1, opt_port, 1, 65535));
    }

    OFLog::configureFromCommandLine(cmd, app);
    if (cmd.findOption("--verbose-pc"))
    {
      app.checkDependence("--verbose-pc", "verbose mode", storescpLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL));
      opt_showPresentationContexts = OFTrue;
    }

    cmd.beginOptionBlock();
    if (cmd.findOption("--prefer-uncompr"))
    {
      opt_acceptAllXfers = OFFalse;
      opt_networkTransferSyntax = EXS_Unknown;
    }
    if (cmd.findOption("--prefer-little")) opt_networkTransferSyntax = EXS_LittleEndianExplicit;
    if (cmd.findOption("--prefer-big")) opt_networkTransferSyntax = EXS_BigEndianExplicit;
    if (cmd.findOption("--prefer-lossless")) opt_networkTransferSyntax = EXS_JPEGProcess14SV1;
    if (cmd.findOption("--prefer-jpeg8")) opt_networkTransferSyntax = EXS_JPEGProcess1;
    if (cmd.findOption("--prefer-jpeg12")) opt_networkTransferSyntax = EXS_JPEGProcess2_4;
    if (cmd.findOption("--prefer-j2k-lossless")) opt_networkTransferSyntax = EXS_JPEG2000LosslessOnly;
    if (cmd.findOption("--prefer-j2k-lossy")) opt_networkTransferSyntax = EXS_JPEG2000;
    if (cmd.findOption("--prefer-jls-lossless")) opt_networkTransferSyntax = EXS_JPEGLSLossless;
    if (cmd.findOption("--prefer-jls-lossy")) opt_networkTransferSyntax = EXS_JPEGLSLossy;
    if (cmd.findOption("--prefer-mpeg2")) opt_networkTransferSyntax = EXS_MPEG2MainProfileAtMainLevel;
    if (cmd.findOption("--prefer-mpeg2-high")) opt_networkTransferSyntax = EXS_MPEG2MainProfileAtHighLevel;
    if (cmd.findOption("--prefer-mpeg4")) opt_networkTransferSyntax = EXS_MPEG4HighProfileLevel4_1;
    if (cmd.findOption("--prefer-mpeg4-bd")) opt_networkTransferSyntax = EXS_MPEG4BDcompatibleHighProfileLevel4_1;
    if (cmd.findOption("--prefer-rle")) opt_networkTransferSyntax = EXS_RLELossless;
#ifdef WITH_ZLIB
    if (cmd.findOption("--prefer-deflated")) opt_networkTransferSyntax = EXS_DeflatedLittleEndianExplicit;
#endif
    if (cmd.findOption("--implicit")) opt_networkTransferSyntax = EXS_LittleEndianImplicit;
    if (cmd.findOption("--accept-all"))
    {
      opt_acceptAllXfers = OFTrue;
      opt_networkTransferSyntax = EXS_Unknown;
    }
    cmd.endOptionBlock();
    if (opt_networkTransferSyntax != EXS_Unknown) opt_acceptAllXfers = OFFalse;

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

    if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_respondingAETitle));
    if (cmd.findOption("--max-pdu")) app.checkValue(cmd.getValueAndCheckMinMax(opt_maxPDU, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
    if (cmd.findOption("--disable-host-lookup")) dcmDisableGethostbyaddr.set(OFTrue);
    if (cmd.findOption("--refuse")) opt_refuseAssociation = OFTrue;
    if (cmd.findOption("--reject")) opt_rejectWithoutImplementationUID = OFTrue;
    if (cmd.findOption("--ignore")) opt_ignore = OFTrue;
    if (cmd.findOption("--sleep-after")) app.checkValue(cmd.getValueAndCheckMin(opt_sleepAfter, 0));
    if (cmd.findOption("--sleep-during")) app.checkValue(cmd.getValueAndCheckMin(opt_sleepDuring, 0));
    if (cmd.findOption("--abort-after")) opt_abortAfterStore = OFTrue;
    if (cmd.findOption("--abort-during")) opt_abortDuringStore = OFTrue;
    if (cmd.findOption("--promiscuous")) opt_promiscuous = OFTrue;
    if (cmd.findOption("--uid-padding")) opt_correctUIDPadding = OFTrue;

    if (cmd.findOption("--config-file"))
    {
      // check conflicts with other command line options
      app.checkConflict("--config-file", "--prefer-little", opt_networkTransferSyntax == EXS_LittleEndianExplicit);
      app.checkConflict("--config-file", "--prefer-big", opt_networkTransferSyntax == EXS_BigEndianExplicit);
      app.checkConflict("--config-file", "--prefer-lossless", opt_networkTransferSyntax == EXS_JPEGProcess14SV1);
      app.checkConflict("--config-file", "--prefer-jpeg8", opt_networkTransferSyntax == EXS_JPEGProcess1);
      app.checkConflict("--config-file", "--prefer-jpeg12", opt_networkTransferSyntax == EXS_JPEGProcess2_4);
      app.checkConflict("--config-file", "--prefer-j2k-lossless", opt_networkTransferSyntax == EXS_JPEG2000LosslessOnly);
      app.checkConflict("--config-file", "--prefer-j2k-lossy", opt_networkTransferSyntax == EXS_JPEG2000);
      app.checkConflict("--config-file", "--prefer-jls-lossless", opt_networkTransferSyntax == EXS_JPEGLSLossless);
      app.checkConflict("--config-file", "--prefer-jls-lossy", opt_networkTransferSyntax == EXS_JPEGLSLossy);
      app.checkConflict("--config-file", "--prefer-mpeg2", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtMainLevel);
      app.checkConflict("--config-file", "--prefer-mpeg2-high", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtHighLevel);
      app.checkConflict("--config-file", "--prefer-mpeg4", opt_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_1);
      app.checkConflict("--config-file", "--prefer-mpeg4-bd", opt_networkTransferSyntax == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
      app.checkConflict("--config-file", "--prefer-rle", opt_networkTransferSyntax == EXS_RLELossless);
#ifdef WITH_ZLIB
      app.checkConflict("--config-file", "--prefer-deflated", opt_networkTransferSyntax == EXS_DeflatedLittleEndianExplicit);
#endif
      app.checkConflict("--config-file", "--implicit", opt_networkTransferSyntax == EXS_LittleEndianImplicit);
      app.checkConflict("--config-file", "--accept-all", opt_acceptAllXfers);
      app.checkConflict("--config-file", "--promiscuous", opt_promiscuous);

      app.checkValue(cmd.getValue(opt_configFile));
      app.checkValue(cmd.getValue(opt_profileName));

      // read configuration file
      OFCondition cond = DcmAssociationConfigurationFile::initialize(asccfg, opt_configFile);
      if (cond.bad())
      {
        OFLOG_FATAL(storescpLogger, "cannot read config file: " << cond.text());
        return 1;
      }

      /* perform name mangling for config file key */
      OFString sprofile;
      const unsigned char *c = OFreinterpret_cast(const unsigned char *, opt_profileName);
      while (*c)
      {
        if (! isspace(*c)) sprofile += OFstatic_cast(char, toupper(*c));
        ++c;
      }

      if (!asccfg.isKnownProfile(sprofile.c_str()))
      {
        OFLOG_FATAL(storescpLogger, "unknown configuration profile name: " << sprofile);
        return 1;
      }

      if (!asccfg.isValidSCPProfile(sprofile.c_str()))
      {
        OFLOG_FATAL(storescpLogger, "profile '" << sprofile << "' is not valid for SCP use, duplicate abstract syntaxes found");
        return 1;
      }

    }

#ifdef WITH_TCPWRAPPER
    cmd.beginOptionBlock();
    if (cmd.findOption("--access-full")) dcmTCPWrapperDaemonName.set(NULL);
    if (cmd.findOption("--access-control")) dcmTCPWrapperDaemonName.set(OFFIS_CONSOLE_APPLICATION);
    cmd.endOptionBlock();
#endif

    if (cmd.findOption("--output-directory")) app.checkValue(cmd.getValue(opt_outputDirectory));

    cmd.beginOptionBlock();
    if (cmd.findOption("--normal")) opt_bitPreserving = OFFalse;
    if (cmd.findOption("--bit-preserving")) opt_bitPreserving = OFTrue;
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--write-file")) opt_useMetaheader = OFTrue;
    if (cmd.findOption("--write-dataset")) opt_useMetaheader = OFFalse;
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--write-xfer-same")) opt_writeTransferSyntax = EXS_Unknown;
    if (cmd.findOption("--write-xfer-little"))
    {
      app.checkConflict("--write-xfer-little", "--accept-all", opt_acceptAllXfers);
      app.checkConflict("--write-xfer-little", "--bit-preserving", opt_bitPreserving);
      app.checkConflict("--write-xfer-little", "--prefer-lossless", opt_networkTransferSyntax == EXS_JPEGProcess14SV1);
      app.checkConflict("--write-xfer-little", "--prefer-jpeg8", opt_networkTransferSyntax == EXS_JPEGProcess1);
      app.checkConflict("--write-xfer-little", "--prefer-jpeg12", opt_networkTransferSyntax == EXS_JPEGProcess2_4);
      app.checkConflict("--write-xfer-little", "--prefer-j2k-lossless", opt_networkTransferSyntax == EXS_JPEG2000LosslessOnly);
      app.checkConflict("--write-xfer-little", "--prefer-j2k-lossy", opt_networkTransferSyntax == EXS_JPEG2000);
      app.checkConflict("--write-xfer-little", "--prefer-jls-lossless", opt_networkTransferSyntax == EXS_JPEGLSLossless);
      app.checkConflict("--write-xfer-little", "--prefer-jls-lossy", opt_networkTransferSyntax == EXS_JPEGLSLossy);
      app.checkConflict("--write-xfer-little", "--prefer-mpeg2", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtMainLevel);
      app.checkConflict("--write-xfer-little", "--prefer-mpeg2-high", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtHighLevel);
      app.checkConflict("--write-xfer-little", "--prefer-mpeg4", opt_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_1);
      app.checkConflict("--write-xfer-little", "--prefer-mpeg4-bd", opt_networkTransferSyntax == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
      app.checkConflict("--write-xfer-little", "--prefer-rle", opt_networkTransferSyntax == EXS_RLELossless);
      // we don't have to check a conflict for --prefer-deflated because we can always convert that to uncompressed.
      opt_writeTransferSyntax = EXS_LittleEndianExplicit;
    }
    if (cmd.findOption("--write-xfer-big"))
    {
      app.checkConflict("--write-xfer-big", "--accept-all", opt_acceptAllXfers);
      app.checkConflict("--write-xfer-big", "--bit-preserving", opt_bitPreserving);
      app.checkConflict("--write-xfer-big", "--prefer-lossless", opt_networkTransferSyntax == EXS_JPEGProcess14SV1);
      app.checkConflict("--write-xfer-big", "--prefer-jpeg8", opt_networkTransferSyntax == EXS_JPEGProcess1);
      app.checkConflict("--write-xfer-big", "--prefer-jpeg12", opt_networkTransferSyntax == EXS_JPEGProcess2_4);
      app.checkConflict("--write-xfer-big", "--prefer-j2k-lossless", opt_networkTransferSyntax == EXS_JPEG2000LosslessOnly);
      app.checkConflict("--write-xfer-big", "--prefer-j2k-lossy", opt_networkTransferSyntax == EXS_JPEG2000);
      app.checkConflict("--write-xfer-big", "--prefer-jls-lossless", opt_networkTransferSyntax == EXS_JPEGLSLossless);
      app.checkConflict("--write-xfer-big", "--prefer-jls-lossy", opt_networkTransferSyntax == EXS_JPEGLSLossy);
      app.checkConflict("--write-xfer-big", "--prefer-mpeg2", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtMainLevel);
      app.checkConflict("--write-xfer-big", "--prefer-mpeg2-high", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtHighLevel);
      app.checkConflict("--write-xfer-big", "--prefer-mpeg4", opt_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_1);
      app.checkConflict("--write-xfer-big", "--prefer-mpeg4-bd", opt_networkTransferSyntax == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
      app.checkConflict("--write-xfer-big", "--prefer-rle", opt_networkTransferSyntax == EXS_RLELossless);
      // we don't have to check a conflict for --prefer-deflated because we can always convert that to uncompressed.
      opt_writeTransferSyntax = EXS_BigEndianExplicit;
    }
    if (cmd.findOption("--write-xfer-implicit"))
    {
      app.checkConflict("--write-xfer-implicit", "--accept-all", opt_acceptAllXfers);
      app.checkConflict("--write-xfer-implicit", "--bit-preserving", opt_bitPreserving);
      app.checkConflict("--write-xfer-implicit", "--prefer-lossless", opt_networkTransferSyntax == EXS_JPEGProcess14SV1);
      app.checkConflict("--write-xfer-implicit", "--prefer-jpeg8", opt_networkTransferSyntax == EXS_JPEGProcess1);
      app.checkConflict("--write-xfer-implicit", "--prefer-jpeg12", opt_networkTransferSyntax == EXS_JPEGProcess2_4);
      app.checkConflict("--write-xfer-implicit", "--prefer-j2k-lossless", opt_networkTransferSyntax == EXS_JPEG2000LosslessOnly);
      app.checkConflict("--write-xfer-implicit", "--prefer-j2k-lossy", opt_networkTransferSyntax == EXS_JPEG2000);
      app.checkConflict("--write-xfer-implicit", "--prefer-jls-lossless", opt_networkTransferSyntax == EXS_JPEGLSLossless);
      app.checkConflict("--write-xfer-implicit", "--prefer-jls-lossy", opt_networkTransferSyntax == EXS_JPEGLSLossy);
      app.checkConflict("--write-xfer-implicit", "--prefer-mpeg2", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtMainLevel);
      app.checkConflict("--write-xfer-implicit", "--prefer-mpeg2-high", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtHighLevel);
      app.checkConflict("--write-xfer-implicit", "--prefer-mpeg4", opt_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_1);
      app.checkConflict("--write-xfer-implicit", "--prefer-mpeg4-bd", opt_networkTransferSyntax == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
      app.checkConflict("--write-xfer-implicit", "--prefer-rle", opt_networkTransferSyntax == EXS_RLELossless);
      // we don't have to check a conflict for --prefer-deflated because we can always convert that to uncompressed.
      opt_writeTransferSyntax = EXS_LittleEndianImplicit;
    }
#ifdef WITH_ZLIB
    if (cmd.findOption("--write-xfer-deflated"))
    {
      app.checkConflict("--write-xfer-deflated", "--accept-all", opt_acceptAllXfers);
      app.checkConflict("--write-xfer-deflated", "--bit-preserving", opt_bitPreserving);
      app.checkConflict("--write-xfer-deflated", "--prefer-lossless", opt_networkTransferSyntax == EXS_JPEGProcess14SV1);
      app.checkConflict("--write-xfer-deflated", "--prefer-jpeg8", opt_networkTransferSyntax == EXS_JPEGProcess1);
      app.checkConflict("--write-xfer-deflated", "--prefer-jpeg12", opt_networkTransferSyntax == EXS_JPEGProcess2_4);
      app.checkConflict("--write-xfer-deflated", "--prefer-j2k-lossless", opt_networkTransferSyntax == EXS_JPEG2000LosslessOnly);
      app.checkConflict("--write-xfer-deflated", "--prefer-j2k-lossy", opt_networkTransferSyntax == EXS_JPEG2000);
      app.checkConflict("--write-xfer-deflated", "--prefer-jls-lossless", opt_networkTransferSyntax == EXS_JPEGLSLossless);
      app.checkConflict("--write-xfer-deflated", "--prefer-jls-lossy", opt_networkTransferSyntax == EXS_JPEGLSLossy);
      app.checkConflict("--write-xfer-deflated", "--prefer-mpeg2", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtMainLevel);
      app.checkConflict("--write-xfer-deflated", "--prefer-mpeg2-high", opt_networkTransferSyntax == EXS_MPEG2MainProfileAtHighLevel);
      app.checkConflict("--write-xfer-deflated", "--prefer-mpeg4", opt_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_1);
      app.checkConflict("--write-xfer-deflated", "--prefer-mpeg4-bd", opt_networkTransferSyntax == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
      app.checkConflict("--write-xfer-deflated", "--prefer-rle", opt_networkTransferSyntax == EXS_RLELossless);
      opt_writeTransferSyntax = EXS_DeflatedLittleEndianExplicit;
    }
#endif
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--enable-new-vr"))
    {
      app.checkConflict("--enable-new-vr", "--bit-preserving", opt_bitPreserving);
      dcmEnableUnknownVRGeneration.set(OFTrue);
      dcmEnableUnlimitedTextVRGeneration.set(OFTrue);
      dcmEnableOtherFloatStringVRGeneration.set(OFTrue);
      dcmEnableOtherDoubleStringVRGeneration.set(OFTrue);
    }
    if (cmd.findOption("--disable-new-vr"))
    {
      app.checkConflict("--disable-new-vr", "--bit-preserving", opt_bitPreserving);
      dcmEnableUnknownVRGeneration.set(OFFalse);
      dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
      dcmEnableOtherFloatStringVRGeneration.set(OFFalse);
      dcmEnableOtherDoubleStringVRGeneration.set(OFFalse);
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--group-length-recalc"))
    {
      app.checkConflict("--group-length-recalc", "--bit-preserving", opt_bitPreserving);
      opt_groupLength = EGL_recalcGL;
    }
    if (cmd.findOption("--group-length-create"))
    {
      app.checkConflict("--group-length-create", "--bit-preserving", opt_bitPreserving);
      opt_groupLength = EGL_withGL;
    }
    if (cmd.findOption("--group-length-remove"))
    {
      app.checkConflict("--group-length-remove", "--bit-preserving", opt_bitPreserving);
      opt_groupLength = EGL_withoutGL;
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--length-explicit"))
    {
      app.checkConflict("--length-explicit", "--bit-preserving", opt_bitPreserving);
      opt_sequenceType = EET_ExplicitLength;
    }
    if (cmd.findOption("--length-undefined"))
    {
      app.checkConflict("--length-undefined", "--bit-preserving", opt_bitPreserving);
      opt_sequenceType = EET_UndefinedLength;
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--padding-off")) opt_paddingType = EPD_withoutPadding;
    if (cmd.findOption("--padding-create"))
    {
      app.checkConflict("--padding-create", "--write-dataset", !opt_useMetaheader);
      app.checkConflict("--padding-create", "--bit-preserving", opt_bitPreserving);
      app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
      app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
      opt_paddingType = EPD_withPadding;
    }
    cmd.endOptionBlock();

#ifdef WITH_ZLIB
    if (cmd.findOption("--compression-level"))
    {
      app.checkDependence("--compression-level", "--write-xfer-deflated or --write-xfer-same",
        (opt_writeTransferSyntax == EXS_DeflatedLittleEndianExplicit) || (opt_writeTransferSyntax == EXS_Unknown));
      app.checkValue(cmd.getValueAndCheckMinMax(opt_compressionLevel, 0, 9));
      dcmZlibCompressionLevel.set(OFstatic_cast(int, opt_compressionLevel));
    }
#endif

    cmd.beginOptionBlock();
    if (cmd.findOption("--sort-conc-studies"))
    {
      app.checkConflict("--sort-conc-studies", "--bit-preserving", opt_bitPreserving);
      app.checkValue(cmd.getValue(opt_sortStudyDirPrefix));
      opt_sortStudyMode = ESM_Timestamp;
    }
    if (cmd.findOption("--sort-on-study-uid"))
    {
      app.checkConflict("--sort-on-study-uid", "--bit-preserving", opt_bitPreserving);
      app.checkValue(cmd.getValue(opt_sortStudyDirPrefix));
      opt_sortStudyMode = ESM_StudyInstanceUID;
    }
    if (cmd.findOption("--sort-on-patientname"))
    {
      app.checkConflict("--sort-on-patientname", "--bit-preserving", opt_bitPreserving);
      opt_sortStudyDirPrefix = NULL;
      opt_sortStudyMode = ESM_PatientName;
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--default-filenames")) opt_uniqueFilenames = OFFalse;
    if (cmd.findOption("--unique-filenames")) opt_uniqueFilenames = OFTrue;
    cmd.endOptionBlock();

    if (cmd.findOption("--timenames")) opt_timeNames = OFTrue;
    if (cmd.findOption("--filename-extension"))
      app.checkValue(cmd.getValue(opt_fileNameExtension));
    if (cmd.findOption("--timenames"))
      app.checkConflict("--timenames", "--unique-filenames", opt_uniqueFilenames);

    if (cmd.findOption("--exec-on-reception")) app.checkValue(cmd.getValue(opt_execOnReception));

    if (cmd.findOption("--exec-on-eostudy"))
    {
      app.checkConflict("--exec-on-eostudy", "--fork", opt_forkMode);
      app.checkConflict("--exec-on-eostudy", "--inetd", opt_inetd_mode);
      app.checkDependence("--exec-on-eostudy", "--sort-conc-studies, --sort-on-study-uid or --sort-on-patientname", opt_sortStudyMode != ESM_None );
      app.checkValue(cmd.getValue(opt_execOnEndOfStudy));
    }

    if (cmd.findOption("--rename-on-eostudy"))
    {
      app.checkConflict("--rename-on-eostudy", "--fork", opt_forkMode);
      app.checkConflict("--rename-on-eostudy", "--inetd", opt_inetd_mode);
      app.checkDependence("--rename-on-eostudy", "--sort-conc-studies, --sort-on-study-uid or --sort-on-patientname", opt_sortStudyMode != ESM_None );
      opt_renameOnEndOfStudy = OFTrue;
    }

    if (cmd.findOption("--eostudy-timeout"))
    {
      app.checkDependence("--eostudy-timeout", "--sort-conc-studies, --sort-on-study-uid, --sort-on-patientname, --exec-on-eostudy or --rename-on-eostudy",
        (opt_sortStudyMode != ESM_None) || (opt_execOnEndOfStudy != NULL) || opt_renameOnEndOfStudy);
      app.checkValue(cmd.getValueAndCheckMin(opt_endOfStudyTimeout, 0));
    }

    if (cmd.findOption("--exec-sync")) opt_execSync = OFTrue;
  }

  /* print resource identifier */
  OFLOG_DEBUG(storescpLogger, rcsid << OFendl);

#ifdef WITH_OPENSSL

  cmd.beginOptionBlock();
  if (cmd.findOption("--disable-tls")) opt_secureConnection = OFFalse;
  if (cmd.findOption("--enable-tls"))
  {
    opt_secureConnection = OFTrue;
    app.checkValue(cmd.getValue(opt_privateKeyFile));
    app.checkValue(cmd.getValue(opt_certificateFile));
  }
  cmd.endOptionBlock();

  cmd.beginOptionBlock();
  if (cmd.findOption("--std-passwd"))
  {
    app.checkDependence("--std-passwd", "--enable-tls", opt_secureConnection);
    opt_passwd = NULL;
  }
  if (cmd.findOption("--use-passwd"))
  {
    app.checkDependence("--use-passwd", "--enable-tls", opt_secureConnection);
    app.checkValue(cmd.getValue(opt_passwd));
  }
  if (cmd.findOption("--null-passwd"))
  {
    app.checkDependence("--null-passwd", "--enable-tls", opt_secureConnection);
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
        OFLOG_FATAL(storescpLogger, "ciphersuite '" << current << "' is unknown, known ciphersuites are:");
        unsigned long numSuites = DcmTLSTransportLayer::getNumberOfCipherSuites();
        for (unsigned long cs = 0; cs < numSuites; cs++)
        {
          OFLOG_FATAL(storescpLogger, "    " << DcmTLSTransportLayer::getTLSCipherSuiteName(cs));
        }
        return 1;
      }
      else
      {
        if (!opt_ciphersuites.empty()) opt_ciphersuites += ":";
        opt_ciphersuites += currentOpenSSL;
      }
    } while (cmd.findOption("--cipher", 0, OFCommandLine::FOM_Next));
  }

#endif

#ifndef DISABLE_PORT_PERMISSION_CHECK
#ifdef HAVE_GETEUID
  /* if port is privileged we must be as well */
  if (opt_port < 1024)
  {
    if (geteuid() != 0)
    {
      OFLOG_FATAL(storescpLogger, "cannot listen on port " << opt_port << ", insufficient privileges");
      return 1;
    }
  }
#endif
#endif

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
    OFLOG_WARN(storescpLogger, "no data dictionary loaded, check environment variable: "
        << DCM_DICT_ENVIRONMENT_VARIABLE);
  }

  /* if the output directory does not equal "." (default directory) */
  if (opt_outputDirectory != ".")
  {
    /* if there is a path separator at the end of the path, get rid of it */
    OFStandard::normalizeDirName(opt_outputDirectory, opt_outputDirectory);

    /* check if the specified directory exists and if it is a directory.
     * If the output directory is invalid, dump an error message and terminate execution.
     */
    if (!OFStandard::dirExists(opt_outputDirectory))
    {
      OFLOG_FATAL(storescpLogger, "specified output directory does not exist");
      return 1;
    }
  }

  /* check if the output directory is writeable */
  if (!OFStandard::isWriteable(opt_outputDirectory))
  {
    OFLOG_FATAL(storescpLogger, "specified output directory is not writeable");
    return 1;
  }

#ifdef HAVE_FORK
  if (opt_forkMode)
    DUL_requestForkOnTransportConnectionReceipt(argc, argv);
#elif defined(_WIN32)
  if (opt_forkedChild)
  {
    // child process
    DUL_markProcessAsForkedChild();

    char buf[256];
    DWORD bytesRead = 0;
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);

    // read socket handle number from stdin, i.e. the anonymous pipe
    // to which our parent process has written the handle number.
    if (ReadFile(hStdIn, buf, sizeof(buf), &bytesRead, NULL))
    {
      // make sure buffer is zero terminated
      buf[bytesRead] = '\0';
      dcmExternalSocketHandle.set(atoi(buf));
    }
    else
    {
      OFLOG_ERROR(storescpLogger, "cannot read socket handle: " << GetLastError());
      return 1;
    }
  }
  else
  {
    // parent process
    if (opt_forkMode)
      DUL_requestForkOnTransportConnectionReceipt(argc, argv);
  }
#endif

  /* initialize network, i.e. create an instance of T_ASC_Network*. */
  OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, OFstatic_cast(int, opt_port), opt_acse_timeout, &net);
  if (cond.bad())
  {
    OFLOG_ERROR(storescpLogger, "cannot create network: " << DimseCondition::dump(temp_str, cond));
    return 1;
  }

#ifdef HAVE_GETUID
  /* return to normal uid so that we can't do too much damage in case
   * things go very wrong.   Only does something if the program is setuid
   * root, and run by another user.  Running as root user may be
   * potentially disastrous if this program screws up badly.
   */
  setuid(getuid());
#endif

#ifdef WITH_OPENSSL
  DcmTLSTransportLayer *tLayer = NULL;
  if (opt_secureConnection)
  {
    tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_ACCEPTOR, opt_readSeedFile);
    if (tLayer == NULL)
    {
      OFLOG_FATAL(storescpLogger, "unable to create TLS transport layer");
      return 1;
    }

    if (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_First))
    {
      do
      {
        app.checkValue(cmd.getValue(current));
        if (TCS_ok != tLayer->addTrustedCertificateFile(current, opt_keyFileFormat))
        {
          OFLOG_WARN(storescpLogger, "unable to load certificate file '" << current << "', ignoring");
        }
      } while (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_Next));
    }

    if (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_First))
    {
      do
      {
        app.checkValue(cmd.getValue(current));
        if (TCS_ok != tLayer->addTrustedCertificateDir(current, opt_keyFileFormat))
        {
          OFLOG_WARN(storescpLogger, "unable to load certificates from directory '" << current << "', ignoring");
        }
      } while (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_Next));
    }

    if (opt_dhparam && !(tLayer->setTempDHParameters(opt_dhparam)))
    {
      OFLOG_WARN(storescpLogger, "unable to load temporary DH parameter file '" << opt_dhparam << "', ignoring");
    }

    if (opt_passwd) tLayer->setPrivateKeyPasswd(opt_passwd);

    if (TCS_ok != tLayer->setPrivateKeyFile(opt_privateKeyFile, opt_keyFileFormat))
    {
      OFLOG_WARN(storescpLogger, "unable to load private TLS key from '" << opt_privateKeyFile << "'");
      return 1;
    }
    if (TCS_ok != tLayer->setCertificateFile(opt_certificateFile, opt_keyFileFormat))
    {
      OFLOG_WARN(storescpLogger, "unable to load certificate from '" << opt_certificateFile << "'");
      return 1;
    }
    if (! tLayer->checkPrivateKeyMatchesCertificate())
    {
      OFLOG_WARN(storescpLogger, "private key '" << opt_privateKeyFile << "' and certificate '" << opt_certificateFile << "' do not match");
      return 1;
    }

    if (TCS_ok != tLayer->setCipherSuites(opt_ciphersuites.c_str()))
    {
      OFLOG_WARN(storescpLogger, "unable to set selected cipher suites");
      return 1;
    }

    tLayer->setCertificateVerification(opt_certVerification);

    cond = ASC_setTransportLayer(net, tLayer, 0);
    if (cond.bad())
    {
      OFLOG_ERROR(storescpLogger, DimseCondition::dump(temp_str, cond));
      return 1;
    }
  }
#endif

#ifdef HAVE_WAITPID
  // register signal handler
  signal(SIGCHLD, sigChildHandler);
#endif

  while (cond.good())
  {
    /* receive an association and acknowledge or reject it. If the association was */
    /* acknowledged, offer corresponding services and invoke one or more if required. */
    cond = acceptAssociation(net, asccfg);

    /* remove zombie child processes */
    cleanChildren(-1, OFFalse);
#ifdef WITH_OPENSSL
    /* since storescp is usually terminated with SIGTERM or the like,
     * we write back an updated random seed after every association handled.
     */
    if (tLayer && opt_writeSeedFile)
    {
      if (tLayer->canWriteRandomSeed())
      {
        if (!tLayer->writeRandomSeed(opt_writeSeedFile))
          OFLOG_WARN(storescpLogger, "cannot write random seed file '" << opt_writeSeedFile << "', ignoring");
      }
      else
      {
        OFLOG_WARN(storescpLogger, "cannot write random seed, ignoring");
      }
    }
#endif
    // if running in inetd mode, we always terminate after one association
    if (opt_inetd_mode) break;

    // if running in multi-process mode, always terminate child after one association
    if (DUL_processIsForkedChild()) break;
  }

  /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
  /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
  cond = ASC_dropNetwork(&net);
  if (cond.bad())
  {
    OFLOG_ERROR(storescpLogger, DimseCondition::dump(temp_str, cond));
    return 1;
  }

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif

#ifdef WITH_OPENSSL
  delete tLayer;
#endif

  return 0;
}


static OFCondition acceptAssociation(T_ASC_Network *net, DcmAssociationConfiguration& asccfg)
{
  char buf[BUFSIZ];
  T_ASC_Association *assoc;
  OFCondition cond;
  OFString sprofile;
  OFString temp_str;

#ifdef PRIVATE_STORESCP_VARIABLES
  PRIVATE_STORESCP_VARIABLES
#endif

  const char* knownAbstractSyntaxes[] =
  {
    UID_VerificationSOPClass
  };

  const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
  int numTransferSyntaxes = 0;

  // try to receive an association. Here we either want to use blocking or
  // non-blocking, depending on if the option --eostudy-timeout is set.
  if( opt_endOfStudyTimeout == -1 )
    cond = ASC_receiveAssociation(net, &assoc, opt_maxPDU, NULL, NULL, opt_secureConnection);
  else
    cond = ASC_receiveAssociation(net, &assoc, opt_maxPDU, NULL, NULL, opt_secureConnection, DUL_NOBLOCK, OFstatic_cast(int, opt_endOfStudyTimeout));

  if (cond.code() == DULC_FORKEDCHILD)
  {
    // OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
    goto cleanup;
  }

  // if some kind of error occurred, take care of it
  if (cond.bad())
  {
    // check what kind of error occurred. If no association was
    // received, check if certain other conditions are met
    if( cond == DUL_NOASSOCIATIONREQUEST )
    {
      // If in addition to the fact that no association was received also option --eostudy-timeout is set
      // and if at the same time there is still a study which is considered to be open (i.e. we were actually
      // expecting to receive more objects that belong to this study) (this is the case if lastStudyInstanceUID
      // does not equal NULL), we have to consider that all objects for the current study have been received.
      // In such an "end-of-study" case, we might have to execute certain optional functions which were specified
      // by the user through command line options passed to storescp.
      if( opt_endOfStudyTimeout != -1 && !lastStudyInstanceUID.empty() )
      {
        // indicate that the end-of-study-event occurred through a timeout event.
        // This knowledge will be necessary in function renameOnEndOFStudy().
        endOfStudyThroughTimeoutEvent = OFTrue;

        // before we actually execute those optional functions, we need to determine the path and name
        // of the subdirectory into which the DICOM files for the last study were written.
        lastStudySubdirectoryPathAndName = subdirectoryPathAndName;

        // now we can finally handle end-of-study events which might have to be executed
        executeEndOfStudyEvents();

        // also, we need to clear lastStudyInstanceUID to indicate
        // that the last study is not considered to be open any more.
        lastStudyInstanceUID.clear();

        // also, we need to clear subdirectoryPathAndName
        subdirectoryPathAndName.clear();

        // reset the endOfStudyThroughTimeoutEvent variable.
        endOfStudyThroughTimeoutEvent = OFFalse;
      }
    }
    // If something else was wrong we might have to dump an error message.
    else
    {
      OFLOG_ERROR(storescpLogger, "Receiving Association failed: " << DimseCondition::dump(temp_str, cond));
    }

    // no matter what kind of error occurred, we need to do a cleanup
    goto cleanup;
  }

#if defined(HAVE_FORK) || defined(_WIN32)
  if (opt_forkMode)
    OFLOG_INFO(storescpLogger, "Association Received in " << (DUL_processIsForkedChild() ? "child" : "parent")
        << " process (pid: " << OFStandard::getProcessID() << ")");
  else
#endif
  OFLOG_INFO(storescpLogger, "Association Received");

  /* dump presentation contexts if required */
  if (opt_showPresentationContexts)
    OFLOG_INFO(storescpLogger, "Parameters:" << OFendl << ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_RQ));
  else
    OFLOG_DEBUG(storescpLogger, "Parameters:" << OFendl << ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_RQ));

  if (opt_refuseAssociation)
  {
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_NOREASON
    };

    OFLOG_INFO(storescpLogger, "Refusing Association (forced via command line)");
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      OFLOG_ERROR(storescpLogger, "Association Reject Failed: " << DimseCondition::dump(temp_str, cond));
    }
    goto cleanup;
  }

  switch (opt_networkTransferSyntax)
  {
    case EXS_LittleEndianImplicit:
      /* we only support Little Endian Implicit */
      transferSyntaxes[0] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 1;
      break;
    case EXS_LittleEndianExplicit:
      /* we prefer Little Endian Explicit */
      transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
    case EXS_BigEndianExplicit:
      /* we prefer Big Endian Explicit */
      transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
    case EXS_JPEGProcess14SV1:
      /* we prefer JPEGLossless:Hierarchical-1stOrderPrediction (default lossless) */
      transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGProcess1:
      /* we prefer JPEGBaseline (default lossy for 8 bit images) */
      transferSyntaxes[0] = UID_JPEGProcess1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGProcess2_4:
      /* we prefer JPEGExtended (default lossy for 12 bit images) */
      transferSyntaxes[0] = UID_JPEGProcess2_4TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEG2000LosslessOnly:
      /* we prefer JPEG2000 Lossless */
      transferSyntaxes[0] = UID_JPEG2000LosslessOnlyTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEG2000:
      /* we prefer JPEG2000 Lossy */
      transferSyntaxes[0] = UID_JPEG2000TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGLSLossless:
      /* we prefer JPEG-LS Lossless */
      transferSyntaxes[0] = UID_JPEGLSLosslessTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGLSLossy:
      /* we prefer JPEG-LS Lossy */
      transferSyntaxes[0] = UID_JPEGLSLossyTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_MPEG2MainProfileAtMainLevel:
      /* we prefer MPEG2 MP@ML */
      transferSyntaxes[0] = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_MPEG2MainProfileAtHighLevel:
      /* we prefer MPEG2 MP@HL */
      transferSyntaxes[0] = UID_MPEG2MainProfileAtHighLevelTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_MPEG4HighProfileLevel4_1:
      /* we prefer MPEG4 HP/L4.1 */
      transferSyntaxes[0] = UID_MPEG4HighProfileLevel4_1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_MPEG4BDcompatibleHighProfileLevel4_1:
      /* we prefer MPEG4 BD HP/L4.1 */
      transferSyntaxes[0] = UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_RLELossless:
      /* we prefer RLE Lossless */
      transferSyntaxes[0] = UID_RLELosslessTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
#ifdef WITH_ZLIB
    case EXS_DeflatedLittleEndianExplicit:
      /* we prefer Deflated Explicit VR Little Endian */
      transferSyntaxes[0] = UID_DeflatedExplicitVRLittleEndianTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
#endif
    default:
      if (opt_acceptAllXfers)
      {
        /* we accept all supported transfer syntaxes
         * (similar to "AnyTransferSyntax" in "storescp.cfg")
         */
        transferSyntaxes[0] = UID_JPEG2000TransferSyntax;
        transferSyntaxes[1] = UID_JPEG2000LosslessOnlyTransferSyntax;
        transferSyntaxes[2] = UID_JPEGProcess2_4TransferSyntax;
        transferSyntaxes[3] = UID_JPEGProcess1TransferSyntax;
        transferSyntaxes[4] = UID_JPEGProcess14SV1TransferSyntax;
        transferSyntaxes[5] = UID_JPEGLSLossyTransferSyntax;
        transferSyntaxes[6] = UID_JPEGLSLosslessTransferSyntax;
        transferSyntaxes[7] = UID_RLELosslessTransferSyntax;
        transferSyntaxes[8] = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
        transferSyntaxes[9] = UID_MPEG2MainProfileAtHighLevelTransferSyntax;
        transferSyntaxes[10] = UID_MPEG4HighProfileLevel4_1TransferSyntax;
        transferSyntaxes[11] = UID_MPEG4BDcompatibleHighProfileLevel4_1TransferSyntax;
        transferSyntaxes[12] = UID_DeflatedExplicitVRLittleEndianTransferSyntax;
        if (gLocalByteOrder == EBO_LittleEndian)
        {
          transferSyntaxes[13] = UID_LittleEndianExplicitTransferSyntax;
          transferSyntaxes[14] = UID_BigEndianExplicitTransferSyntax;
        } else {
          transferSyntaxes[13] = UID_BigEndianExplicitTransferSyntax;
          transferSyntaxes[14] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[15] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 16;
      } else {
        /* We prefer explicit transfer syntaxes.
         * If we are running on a Little Endian machine we prefer
         * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
         */
        if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
        {
          transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        }
        else
        {
          transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
          transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
      }
      break;
  }

  if (opt_profileName)
  {
    /* perform name mangling for config file key */
    const unsigned char *c = OFreinterpret_cast(const unsigned char *, opt_profileName);
    while (*c)
    {
      if (!isspace(*c)) sprofile += OFstatic_cast(char, toupper(*c));
      ++c;
    }

    /* set presentation contexts as defined in config file */
    cond = asccfg.evaluateAssociationParameters(sprofile.c_str(), *assoc);
    if (cond.bad())
    {
      OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
      goto cleanup;
    }
  }
  else
  {
    /* accept the Verification SOP Class if presented */
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes( assoc->params, knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes), transferSyntaxes, numTransferSyntaxes);
    if (cond.bad())
    {
      OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
      goto cleanup;
    }

    /* the array of Storage SOP Class UIDs comes from dcuid.h */
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes( assoc->params, dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs, transferSyntaxes, numTransferSyntaxes);
    if (cond.bad())
    {
      OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
      goto cleanup;
    }

    if (opt_promiscuous)
    {
      /* accept everything not known not to be a storage SOP class */
      cond = acceptUnknownContextsWithPreferredTransferSyntaxes(
        assoc->params, transferSyntaxes, numTransferSyntaxes);
      if (cond.bad())
      {
        OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
        goto cleanup;
      }
    }
  }

  /* set our app title */
  ASC_setAPTitles(assoc->params, NULL, NULL, opt_respondingAETitle);

  /* acknowledge or reject this association */
  cond = ASC_getApplicationContextName(assoc->params, buf);
  if ((cond.bad()) || strcmp(buf, UID_StandardApplicationContext) != 0)
  {
    /* reject: the application context name is not supported */
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
    };

    OFLOG_INFO(storescpLogger, "Association Rejected: Bad Application Context Name: " << buf);
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
    }
    goto cleanup;

  }
  else if (opt_rejectWithoutImplementationUID && strlen(assoc->params->theirImplementationClassUID) == 0)
  {
    /* reject: the no implementation Class UID provided */
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_NOREASON
    };

    OFLOG_INFO(storescpLogger, "Association Rejected: No Implementation Class UID provided");
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      OFLOG_DEBUG(storescpLogger, DimseCondition::dump(temp_str, cond));
    }
    goto cleanup;
  }
  else
  {
#ifdef PRIVATE_STORESCP_CODE
    PRIVATE_STORESCP_CODE
#endif
    cond = ASC_acknowledgeAssociation(assoc);
    if (cond.bad())
    {
      OFLOG_ERROR(storescpLogger, DimseCondition::dump(temp_str, cond));
      goto cleanup;
    }
    OFLOG_INFO(storescpLogger, "Association Acknowledged (Max Send PDV: " << assoc->sendPDVLength << ")");
    if (ASC_countAcceptedPresentationContexts(assoc->params) == 0)
      OFLOG_INFO(storescpLogger, "    (but no valid presentation contexts)");
    /* dump the presentation contexts which have been accepted/refused */
    if (opt_showPresentationContexts)
      OFLOG_INFO(storescpLogger, ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_AC));
    else
      OFLOG_DEBUG(storescpLogger, ASC_dumpParameters(temp_str, assoc->params, ASC_ASSOC_AC));
  }

#ifdef BUGGY_IMPLEMENTATION_CLASS_UID_PREFIX
  /* active the dcmPeerRequiresExactUIDCopy workaround code
   * (see comments in dimse.h) for a implementation class UID
   * prefix known to exhibit the buggy behaviour.
   */
  if (0 == strncmp(assoc->params->theirImplementationClassUID,
      BUGGY_IMPLEMENTATION_CLASS_UID_PREFIX,
      strlen(BUGGY_IMPLEMENTATION_CLASS_UID_PREFIX)))
  {
    dcmEnableAutomaticInputDataCorrection.set(OFFalse);
    dcmPeerRequiresExactUIDCopy.set(OFTrue);
  }
#endif

  // store previous values for later use
  lastCallingAETitle = callingAETitle;
  lastCalledAETitle = calledAETitle;
  lastCallingPresentationAddress = callingPresentationAddress;
  // store calling and called aetitle in global variables to enable
  // the --exec options using them. Enclose in quotation marks because
  // aetitles may contain space characters.
  DIC_AE callingTitle;
  DIC_AE calledTitle;
  if (ASC_getAPTitles(assoc->params, callingTitle, calledTitle, NULL).good())
  {
    callingAETitle = "\"";
    callingAETitle += OFSTRING_GUARD(callingTitle);
    callingAETitle += "\"";
    calledAETitle = "\"";
    calledAETitle += OFSTRING_GUARD(calledTitle);
    calledAETitle += "\"";
  }
  else
  {
    // should never happen
    callingAETitle.clear();
    calledAETitle.clear();
  }
  // store calling presentation address (i.e. remote hostname)
  callingPresentationAddress = OFSTRING_GUARD(assoc->params->DULparams.callingPresentationAddress);

  /* now do the real work, i.e. receive DIMSE commands over the network connection */
  /* which was established and handle these commands correspondingly. In case of */
  /* storescp only C-ECHO-RQ and C-STORE-RQ commands can be processed. */
  cond = processCommands(assoc);

  if (cond == DUL_PEERREQUESTEDRELEASE)
  {
    OFLOG_INFO(storescpLogger, "Association Release");
    cond = ASC_acknowledgeRelease(assoc);
  }
  else if (cond == DUL_PEERABORTEDASSOCIATION)
  {
    OFLOG_INFO(storescpLogger, "Association Aborted");
  }
  else
  {
    OFLOG_ERROR(storescpLogger, "DIMSE failure (aborting association): " << DimseCondition::dump(temp_str, cond));
    /* some kind of error so abort the association */
    cond = ASC_abortAssociation(assoc);
  }

cleanup:

  if (cond.code() == DULC_FORKEDCHILD) return cond;

  cond = ASC_dropSCPAssociation(assoc);
  if (cond.bad())
  {
    OFLOG_FATAL(storescpLogger, DimseCondition::dump(temp_str, cond));
    exit(1);
  }
  cond = ASC_destroyAssociation(&assoc);
  if (cond.bad())
  {
    OFLOG_FATAL(storescpLogger, DimseCondition::dump(temp_str, cond));
    exit(1);
  }

  return cond;
}


static OFCondition
processCommands(T_ASC_Association * assoc)
    /*
     * This function receives DIMSE commands over the network connection
     * and handles these commands correspondingly. Note that in case of
     * storescp only C-ECHO-RQ and C-STORE-RQ commands can be processed.
     *
     * Parameters:
     *   assoc - [in] The association (network connection to another DICOM application).
     */
{
  OFCondition cond = EC_Normal;
  T_DIMSE_Message msg;
  T_ASC_PresentationContextID presID = 0;
  DcmDataset *statusDetail = NULL;

  // start a loop to be able to receive more than one DIMSE command
  while( cond == EC_Normal || cond == DIMSE_NODATAAVAILABLE || cond == DIMSE_OUTOFRESOURCES )
  {
    // receive a DIMSE command over the network
    if( opt_endOfStudyTimeout == -1 )
      cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &presID, &msg, &statusDetail);
    else
      cond = DIMSE_receiveCommand(assoc, DIMSE_NONBLOCKING, OFstatic_cast(int, opt_endOfStudyTimeout), &presID, &msg, &statusDetail);

    // check what kind of error occurred. If no data was
    // received, check if certain other conditions are met
    if( cond == DIMSE_NODATAAVAILABLE )
    {
      // If in addition to the fact that no data was received also option --eostudy-timeout is set and
      // if at the same time there is still a study which is considered to be open (i.e. we were actually
      // expecting to receive more objects that belong to this study) (this is the case if lastStudyInstanceUID
      // does not equal NULL), we have to consider that all objects for the current study have been received.
      // In such an "end-of-study" case, we might have to execute certain optional functions which were specified
      // by the user through command line options passed to storescp.
      if( opt_endOfStudyTimeout != -1 && !lastStudyInstanceUID.empty() )
      {
        // indicate that the end-of-study-event occurred through a timeout event.
        // This knowledge will be necessary in function renameOnEndOFStudy().
        endOfStudyThroughTimeoutEvent = OFTrue;

        // before we actually execute those optional functions, we need to determine the path and name
        // of the subdirectory into which the DICOM files for the last study were written.
        lastStudySubdirectoryPathAndName = subdirectoryPathAndName;

        // now we can finally handle end-of-study events which might have to be executed
        executeEndOfStudyEvents();

        // also, we need to clear lastStudyInstanceUID to indicate
        // that the last study is not considered to be open any more.
        lastStudyInstanceUID.clear();

        // also, we need to clear subdirectoryPathAndName
        subdirectoryPathAndName.clear();

        // reset the endOfStudyThroughTimeoutEvent variable.
        endOfStudyThroughTimeoutEvent = OFFalse;
      }
    }

    // if the command which was received has extra status
    // detail information, dump this information
    if (statusDetail != NULL)
    {
      OFLOG_DEBUG(storescpLogger, "Status Detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
      delete statusDetail;
    }

    // check if peer did release or abort, or if we have a valid message
    if (cond == EC_Normal)
    {
      // in case we received a valid message, process this command
      // note that storescp can only process a C-ECHO-RQ and a C-STORE-RQ
      switch (msg.CommandField)
      {
        case DIMSE_C_ECHO_RQ:
          // process C-ECHO-Request
          cond = echoSCP(assoc, &msg, presID);
          break;
        case DIMSE_C_STORE_RQ:
          // process C-STORE-Request
          cond = storeSCP(assoc, &msg, presID);
          break;
        default:
          OFString tempStr;
          // we cannot handle this kind of message
          cond = DIMSE_BADCOMMANDTYPE;
          OFLOG_ERROR(storescpLogger, "Expected C-ECHO or C-STORE request but received DIMSE command 0x"
               << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
               << OFstatic_cast(unsigned, msg.CommandField));
          OFLOG_DEBUG(storescpLogger, DIMSE_dumpMessage(tempStr, msg, DIMSE_INCOMING, NULL, presID));
          break;
      }
    }
  }
  return cond;
}


static OFCondition echoSCP( T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID)
{
  OFString temp_str;
  // assign the actual information of the C-Echo-RQ command to a local variable
  T_DIMSE_C_EchoRQ *req = &msg->msg.CEchoRQ;
  if (storescpLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    OFLOG_INFO(storescpLogger, "Received Echo Request");
    OFLOG_DEBUG(storescpLogger, DIMSE_dumpMessage(temp_str, *req, DIMSE_INCOMING, NULL, presID));
  } else {
    OFLOG_INFO(storescpLogger, "Received Echo Request (MsgID " << req->MessageID << ")");
  }

  /* the echo succeeded !! */
  OFCondition cond = DIMSE_sendEchoResponse(assoc, presID, req, STATUS_Success, NULL);
  if (cond.bad())
  {
    OFLOG_ERROR(storescpLogger, "Echo SCP Failed: " << DimseCondition::dump(temp_str, cond));
  }
  return cond;
}

// substitute non-ASCII characters with ASCII "equivalents"
static void mapCharacterAndAppendToString(Uint8 c,
                                          OFString &output)
{
    static const char *latin1_table[] =
    {
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 0-15
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 16-31
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 32-47
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "_", "_", "_", "_", "_", "_", // Codes 48-63
        "_", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", // Codes 64-79
        "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "_", "_", "_", "_", "_", // Codes 80-95
        "_", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", // Codes 96-111
        "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "_", "_", "_", "_", "_", // Codes 112-127
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 128-143
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 144-159
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 160-175
        "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", "_", // Codes 176-191
        "A", "A", "A", "A", "Ae","A", "A", "C", "E", "E", "E", "E", "I", "I", "I", "I", // Codes 192-107
        "D", "N", "O", "O", "O", "O", "Oe","_", "O", "U", "U", "U", "Ue","Y", "_", "ss",// Codes 108-123
        "a", "a", "a", "a", "ae","a", "a", "c", "e", "e", "e", "e", "i", "i", "i", "i", // Codes 124-141
        "d", "n", "o", "o", "o", "o", "oe","_", "o", "u", "u", "u", "ue","y", "_", "y"  // Codes 142-157
    };
    output += latin1_table[c];
}

struct StoreCallbackData
{
  char* imageFileName;
  DcmFileFormat* dcmff;
  T_ASC_Association* assoc;
};


static void
storeSCPCallback(
    void *callbackData,
    T_DIMSE_StoreProgress *progress,
    T_DIMSE_C_StoreRQ *req,
    char * /*imageFileName*/, DcmDataset **imageDataSet,
    T_DIMSE_C_StoreRSP *rsp,
    DcmDataset **statusDetail)
    /*
     * This function.is used to indicate progress when storescp receives instance data over the
     * network. On the final call to this function (identified by progress->state == DIMSE_StoreEnd)
     * this function will store the data set which was received over the network to a file.
     * Earlier calls to this function will simply cause some information to be dumped to stdout.
     *
     * Parameters:
     *   callbackData  - [in] data for this callback function
     *   progress      - [in] The state of progress. (identifies if this is the initial or final call
     *                   to this function, or a call in between these two calls.
     *   req           - [in] The original store request message.
     *   imageFileName - [in] The path to and name of the file the information shall be written to.
     *   imageDataSet  - [in] The data set which shall be stored in the image file
     *   rsp           - [inout] the C-STORE-RSP message (will be sent after the call to this function)
     *   statusDetail  - [inout] This variable can be used to capture detailed information with regard to
     *                   the status information which is captured in the status element (0000,0900). Note
     *                   that this function does specify any such information, the pointer will be set to NULL.
     */
{
  DIC_UI sopClass;
  DIC_UI sopInstance;

  // determine if the association shall be aborted
  if( (opt_abortDuringStore && progress->state != DIMSE_StoreBegin) ||
      (opt_abortAfterStore && progress->state == DIMSE_StoreEnd) )
  {
    OFLOG_INFO(storescpLogger, "ABORT initiated (due to command line options)");
    ASC_abortAssociation((OFstatic_cast(StoreCallbackData*, callbackData))->assoc);
    rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
    return;
  }

  // if opt_sleepAfter is set, the user requires that the application shall
  // sleep a certain amount of seconds after having received one PDU.
  if (opt_sleepDuring > 0)
  {
    OFStandard::sleep(OFstatic_cast(unsigned int, opt_sleepDuring));
  }

  // dump some information if required (depending on the progress state)
  // We can't use oflog for the PDU output, but we use a special logger for
  // generating this output. If it is set to level "INFO" we generate the
  // output, if it's set to "DEBUG" then we'll assume that there is debug output
  // generated for each PDU elsewhere.
  OFLogger progressLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION ".progress");
  if (progressLogger.getChainedLogLevel() == OFLogger::INFO_LOG_LEVEL)
  {
    switch (progress->state)
    {
      case DIMSE_StoreBegin:
        COUT << "RECV: ";
        break;
      case DIMSE_StoreEnd:
        COUT << OFendl;
        break;
      default:
        COUT << '.';
        break;
    }
    COUT.flush();
  }

  // if this is the final call of this function, save the data which was received to a file
  // (note that we could also save the image somewhere else, put it in database, etc.)
  if (progress->state == DIMSE_StoreEnd)
  {
    OFString tmpStr;

    // do not send status detail information
    *statusDetail = NULL;

    // remember callback data
    StoreCallbackData *cbdata = OFstatic_cast(StoreCallbackData *, callbackData);

    // Concerning the following line: an appropriate status code is already set in the resp structure,
    // it need not be success. For example, if the caller has already detected an out of resources problem
    // then the status will reflect this.  The callback function is still called to allow cleanup.
    //rsp->DimseStatus = STATUS_Success;

    // we want to write the received information to a file only if this information
    // is present and the options opt_bitPreserving and opt_ignore are not set.
    if ((imageDataSet != NULL) && (*imageDataSet != NULL) && !opt_bitPreserving && !opt_ignore)
    {
      OFString fileName;

      // in case one of the --sort-xxx options is set, we need to perform some particular steps to
      // determine the actual name of the output file
      if (opt_sortStudyMode != ESM_None)
      {
        // determine the study instance UID in the (current) DICOM object that has just been received
        OFString currentStudyInstanceUID;
        if ((*imageDataSet)->findAndGetOFString(DCM_StudyInstanceUID, currentStudyInstanceUID).bad() || currentStudyInstanceUID.empty())
        {
          OFLOG_ERROR(storescpLogger, "element StudyInstanceUID " << DCM_StudyInstanceUID << " absent or empty in data set");
          rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
          return;
        }

        // if --sort-on-patientname is active, we need to extract the
        // patient's name (format: last_name^first_name)
        OFString currentPatientName;
        if (opt_sortStudyMode == ESM_PatientName)
        {
          OFString tmpName;
          if ((*imageDataSet)->findAndGetOFString(DCM_PatientName, tmpName).bad() || tmpName.empty())
          {
            // default if patient name is missing or empty
            tmpName = "ANONYMOUS";
            OFLOG_WARN(storescpLogger, "element PatientName " << DCM_PatientName << " absent or empty in data set, using '"
                 << tmpName << "' instead");
          }

          /* substitute non-ASCII characters in patient name to ASCII "equivalent" */
          const size_t length = tmpName.length();
          for (size_t i = 0; i < length; i++)
            mapCharacterAndAppendToString(tmpName[i], currentPatientName);
        }

        // if this is the first DICOM object that was received or if the study instance UID in the
        // current DICOM object does not equal the last object's study instance UID we need to create
        // a new subdirectory in which the current DICOM object will be stored
        if (lastStudyInstanceUID.empty() || (lastStudyInstanceUID != currentStudyInstanceUID))
        {
          // if lastStudyInstanceUID is non-empty, we have just completed receiving all objects for one
          // study. In such a case, we need to set a certain indicator variable (lastStudySubdirectoryPathAndName),
          // so that we know that executeOnEndOfStudy() might have to be executed later. In detail, this indicator
          // variable will contain the path and name of the last study's subdirectory, so that we can still remember
          // this directory, when we execute executeOnEndOfStudy(). The memory that is allocated for this variable
          // here will be freed after the execution of executeOnEndOfStudy().
          if (!lastStudyInstanceUID.empty())
            lastStudySubdirectoryPathAndName = subdirectoryPathAndName;

          // create the new lastStudyInstanceUID value according to the value in the current DICOM object
          lastStudyInstanceUID = currentStudyInstanceUID;

          // get the current time (needed for subdirectory name)
          OFDateTime dateTime;
          dateTime.setCurrentDateTime();

          // create a name for the new subdirectory.
          char timestamp[32];
          sprintf(timestamp, "%04u%02u%02u_%02u%02u%02u%03u",
            dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
            dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond());

          OFString subdirectoryName;
          switch (opt_sortStudyMode)
          {
            case ESM_Timestamp:
              // pattern: "[prefix]_[YYYYMMDD]_[HHMMSSMMM]"
              subdirectoryName = opt_sortStudyDirPrefix;
              if (!subdirectoryName.empty())
                subdirectoryName += '_';
              subdirectoryName += timestamp;
              break;
            case ESM_StudyInstanceUID:
              // pattern: "[prefix]_[Study Instance UID]"
              subdirectoryName = opt_sortStudyDirPrefix;
              if (!subdirectoryName.empty())
                subdirectoryName += '_';
              subdirectoryName += currentStudyInstanceUID;
              break;
            case ESM_PatientName:
              // pattern: "[Patient's Name]_[YYYYMMDD]_[HHMMSSMMM]"
              subdirectoryName = currentPatientName;
              subdirectoryName += '_';
              subdirectoryName += timestamp;
              break;
            case ESM_None:
              break;
          }

          // create subdirectoryPathAndName (string with full path to new subdirectory)
          OFStandard::combineDirAndFilename(subdirectoryPathAndName, OFStandard::getDirNameFromPath(tmpStr, cbdata->imageFileName), subdirectoryName);

          // check if the subdirectory already exists
          // if it already exists dump a warning
          if( OFStandard::dirExists(subdirectoryPathAndName) )
            OFLOG_WARN(storescpLogger, "subdirectory for study already exists: " << subdirectoryPathAndName);
          else
          {
            // if it does not exist create it
            OFLOG_INFO(storescpLogger, "creating new subdirectory for study: " << subdirectoryPathAndName);
#ifdef HAVE_WINDOWS_H
            if( _mkdir( subdirectoryPathAndName.c_str() ) == -1 )
#else
            if( mkdir( subdirectoryPathAndName.c_str(), S_IRWXU | S_IRWXG | S_IRWXO ) == -1 )
#endif
            {
              OFLOG_ERROR(storescpLogger, "could not create subdirectory for study: " << subdirectoryPathAndName);
              rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
              return;
            }
            // all objects of a study have been received, so a new subdirectory is started.
            // ->timename counter can be reset, because the next filename can't cause a duplicate.
            // if no reset would be done, files of a new study (->new directory) would start with a counter in filename
            if (opt_timeNames)
              timeNameCounter = -1;
          }
        }

        // integrate subdirectory name into file name (note that cbdata->imageFileName currently contains both
        // path and file name; however, the path refers to the output directory captured in opt_outputDirectory)
        OFStandard::combineDirAndFilename(fileName, subdirectoryPathAndName, OFStandard::getFilenameFromPath(tmpStr, cbdata->imageFileName));

        // update global variable outputFileNameArray
        // (might be used in executeOnReception() and renameOnEndOfStudy)
        outputFileNameArray.push_back(tmpStr);
      }
      // if no --sort-xxx option is set, the determination of the output file name is simple
      else
      {
        fileName = cbdata->imageFileName;

        // update global variables outputFileNameArray
        // (might be used in executeOnReception() and renameOnEndOfStudy)
        outputFileNameArray.push_back(OFStandard::getFilenameFromPath(tmpStr, fileName));
      }

      // determine the transfer syntax which shall be used to write the information to the file
      E_TransferSyntax xfer = opt_writeTransferSyntax;
      if (xfer == EXS_Unknown) xfer = (*imageDataSet)->getOriginalXfer();

      // store file either with meta header or as pure dataset
      OFLOG_INFO(storescpLogger, "storing DICOM file: " << fileName);
      if (OFStandard::fileExists(fileName))
      {
        OFLOG_WARN(storescpLogger, "DICOM file already exists, overwriting: " << fileName);
      }
      OFCondition cond = cbdata->dcmff->saveFile(fileName.c_str(), xfer, opt_sequenceType, opt_groupLength,
          opt_paddingType, OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad),
          (opt_useMetaheader) ? EWM_fileformat : EWM_dataset);
      if (cond.bad())
      {
        OFLOG_ERROR(storescpLogger, "cannot write DICOM file: " << fileName << ": " << cond.text());
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
      }

      // check the image to make sure it is consistent, i.e. that its sopClass and sopInstance correspond
      // to those mentioned in the request. If not, set the status in the response message variable.
      if ((rsp->DimseStatus == STATUS_Success)&&(!opt_ignore))
      {
        // which SOP class and SOP instance ?
        if (!DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance, opt_correctUIDPadding))
        {
           OFLOG_ERROR(storescpLogger, "bad DICOM file: " << fileName);
           rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
        }
        else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0)
        {
          rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
        }
        else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0)
        {
          rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
        }
      }
    }

    // in case opt_bitPreserving is set, do some other things
    if( opt_bitPreserving )
    {
      // we need to set outputFileNameArray and outputFileNameArrayCnt to be
      // able to perform the placeholder substitution in executeOnReception()
      outputFileNameArray.push_back(OFStandard::getFilenameFromPath(tmpStr, cbdata->imageFileName));
    }
  }
}


static OFCondition storeSCP(
  T_ASC_Association *assoc,
  T_DIMSE_Message *msg,
  T_ASC_PresentationContextID presID)
    /*
     * This function processes a DIMSE C-STORE-RQ command that was
     * received over the network connection.
     *
     * Parameters:
     *   assoc  - [in] The association (network connection to another DICOM application).
     *   msg    - [in] The DIMSE C-STORE-RQ message that was received.
     *   presID - [in] The ID of the presentation context which was specified in the PDV which contained
     *                 the DIMSE command.
     */
{
  OFCondition cond = EC_Normal;
  T_DIMSE_C_StoreRQ *req;
  char imageFileName[2048];

  // assign the actual information of the C-STORE-RQ command to a local variable
  req = &msg->msg.CStoreRQ;

  // if opt_ignore is set, the user requires that the data shall be received but not
  // stored. in this case, we want to create a corresponding temporary filename for
  // a file in which the data shall be stored temporarily. If this is not the case,
  // create a real filename (consisting of path and filename) for a real file.
  if (opt_ignore)
  {
#ifdef _WIN32
    tmpnam(imageFileName);
#else
    strcpy(imageFileName, NULL_DEVICE_NAME);
#endif
  }
  else
  {
    // 3 possibilities: create unique filenames (fn), create timestamp fn, create fn from SOP Instance UIDs
    if (opt_uniqueFilenames)
    {
      // create unique filename by generating a temporary UID and using ".X." as an infix
      char buf[70];
      dcmGenerateUniqueIdentifier(buf);
      sprintf(imageFileName, "%s%c%s.X.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"),
        buf, opt_fileNameExtension.c_str());
    }
    else if (opt_timeNames)
    {
      // create a name for the new file. pattern: "[YYYYMMDDHHMMSSMMM]_[NUMBER].MODALITY[EXTENSION]" (use current datetime)
      // get the current time (needed for file name)
      OFDateTime dateTime;
      dateTime.setCurrentDateTime();
      // used to hold prospective filename
      char cmpFileName[2048];
      // next if/else block generates prospective filename, that is compared to last written filename
      if (timeNameCounter == -1)
      {
        // timeNameCounter not set -> last written filename has to be without "serial number"
        sprintf(cmpFileName, "%04u%02u%02u%02u%02u%02u%03u.%s%s",
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
      }
      else
      {
        // counter was active before, so generate filename with "serial number" for comparison
        sprintf(cmpFileName, "%04u%02u%02u%02u%02u%02u%03u_%04u.%s%s", // millisecond version
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          timeNameCounter, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
      }
      if ( (outputFileNameArray.size()!=0) && (outputFileNameArray.back() == cmpFileName) )
      {
        // if this is not the first run and the prospective filename is equal to the last written filename
        // generate one with a serial number (incremented by 1)
        timeNameCounter++;
        sprintf(imageFileName, "%s%c%04u%02u%02u%02u%02u%02u%03u_%04u.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, // millisecond version
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          timeNameCounter, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
      }
      else
      {
        // first run or filenames are different: create filename without serial number
        sprintf(imageFileName, "%s%c%04u%02u%02u%02u%02u%02u%03u.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, // millisecond version
          dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
          dateTime.getTime().getHour(), dateTime.getTime().getMinute(),dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond(),
          dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"), opt_fileNameExtension.c_str());
        // reset counter, because timestamp and therefore filename has changed
        timeNameCounter = -1;
      }
    }
    else
    {
      // don't create new UID, use the study instance UID as found in object
      sprintf(imageFileName, "%s%c%s.%s%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "UNKNOWN"),
        req->AffectedSOPInstanceUID, opt_fileNameExtension.c_str());
    }
  }

  // dump some information if required
  OFString str;
  if (storescpLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
  {
    OFLOG_INFO(storescpLogger, "Received Store Request");
    OFLOG_DEBUG(storescpLogger, DIMSE_dumpMessage(str, *req, DIMSE_INCOMING, NULL, presID));
  } else {
    OFLOG_INFO(storescpLogger, "Received Store Request (MsgID " << req->MessageID << ", "
      << dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "OT") << ")");
  }

  // initialize some variables
  StoreCallbackData callbackData;
  callbackData.assoc = assoc;
  callbackData.imageFileName = imageFileName;
  DcmFileFormat dcmff;
  callbackData.dcmff = &dcmff;

  // store SourceApplicationEntityTitle in metaheader
  if (assoc && assoc->params)
  {
    const char *aet = assoc->params->DULparams.callingAPTitle;
    if (aet) dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
  }

  // define an address where the information which will be received over the network will be stored
  DcmDataset *dset = dcmff.getDataset();

  // if opt_bitPreserving is set, the user requires that the data shall be
  // written exactly as it was received. Depending on this option, function
  // DIMSE_storeProvider must be called with certain parameters.
  if (opt_bitPreserving)
  {
    cond = DIMSE_storeProvider(assoc, presID, req, imageFileName, opt_useMetaheader, NULL,
      storeSCPCallback, &callbackData, opt_blockMode, opt_dimse_timeout);
  }
  else
  {
    cond = DIMSE_storeProvider(assoc, presID, req, NULL, opt_useMetaheader, &dset,
      storeSCPCallback, &callbackData, opt_blockMode, opt_dimse_timeout);
  }

  // if some error occurred, dump corresponding information and remove the outfile if necessary
  if (cond.bad())
  {
    OFString temp_str;
    OFLOG_ERROR(storescpLogger, "Store SCP Failed: " << DimseCondition::dump(temp_str, cond));
    // remove file
    if (!opt_ignore)
    {
      if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0)
        OFStandard::deleteFile(imageFileName);
    }
  }
#ifdef _WIN32
  else if (opt_ignore)
  {
    if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0)
      OFStandard::deleteFile(imageFileName); // delete the temporary file
  }
#endif

  // if everything was successful so far and option --exec-on-reception is set,
  // we want to execute a certain command which was passed to the application
  if( cond.good() && opt_execOnReception != NULL )
    executeOnReception();

  // if everything was successful so far, go ahead and handle possible end-of-study events
  if( cond.good() )
    executeEndOfStudyEvents();

  // if opt_sleepAfter is set, the user requires that the application shall
  // sleep a certain amount of seconds after storing the instance data.
  if (opt_sleepAfter > 0)
  {
    OFStandard::sleep(OFstatic_cast(unsigned int, opt_sleepAfter));
  }

  // return return value
  return cond;
}


static void executeEndOfStudyEvents()
    /*
     * This function deals with the execution of end-of-study-events. In detail,
     * events that need to take place are specified by the user through certain
     * command line options. The options that define these end-of-study-events
     * are "--rename-on-eostudy" and "--exec-on-eostudy".
     *
     * Parameters:
     *   none.
     */
{
  // if option --rename-on-eostudy is set and variable lastStudySubdirectoryPathAndName
  // does not equal NULL (i.e. we received all objects that belong to one study, or - in
  // other words - it is the end of one study) we want to rename the output files that
  // belong to the last study. (Note that these files are captured in outputFileNameArray)
  if( opt_renameOnEndOfStudy && !lastStudySubdirectoryPathAndName.empty() )
    renameOnEndOfStudy();

  // if option --exec-on-eostudy is set and variable lastStudySubdirectoryPathAndName does
  // not equal NULL (i.e. we received all objects that belong to one study, or - in other
  // words - it is the end of one study) we want to execute a certain command which was
  // passed to the application
  if( opt_execOnEndOfStudy != NULL && !lastStudySubdirectoryPathAndName.empty() )
    executeOnEndOfStudy();

  lastStudySubdirectoryPathAndName.clear();
}


static void executeOnReception()
    /*
     * This function deals with the execution of the command line which was passed
     * to option --exec-on-reception of the storescp. This command line is captured
     * in opt_execOnReception. Note that the command line can contain the placeholders
     * PATH_PLACEHOLDER and FILENAME_PLACEHOLDER which need to be substituted before the command line is actually
     * executed. PATH_PLACEHOLDER will be substituted by the path to the output directory into which
     * the last file was written; FILENAME_PLACEHOLDER will be substituted by the filename of the last
     * file which was written.
     *
     * Parameters:
     *   none.
     */
{
  OFString cmd = opt_execOnReception;

  // in case a file was actually written
  if( !opt_ignore )
  {
    // perform substitution for placeholder #p (depending on presence of any --sort-xxx option)
    OFString dir = (opt_sortStudyMode == ESM_None) ? opt_outputDirectory : subdirectoryPathAndName;
    cmd = replaceChars( cmd, OFString(PATH_PLACEHOLDER), dir );

    // perform substitution for placeholder #f; note that outputFileNameArray.back()
    // always contains the name of the file (without path) which was written last.
    OFString outputFileName = outputFileNameArray.back();
    cmd = replaceChars( cmd, OFString(FILENAME_PLACEHOLDER), outputFileName );
  }

  // perform substitution for placeholder #a
  cmd = replaceChars( cmd, OFString(CALLING_AETITLE_PLACEHOLDER), callingAETitle );

  // perform substitution for placeholder #c
  cmd = replaceChars( cmd, OFString(CALLED_AETITLE_PLACEHOLDER), calledAETitle );

  // perform substitution for placeholder #r
  cmd = replaceChars( cmd, OFString(CALLING_PRESENTATION_ADDRESS_PLACEHOLDER), callingPresentationAddress );

  // Execute command in a new process
  executeCommand( cmd );
}


static void renameOnEndOfStudy()
    /*
     * This function deals with renaming the last study's output files. In detail, these file's
     * current filenames will be changed to a filename that corresponds to the pattern [modality-
     * prefix][consecutive-numbering]. The current filenames of all files that belong to the study
     * are captured in outputFileNameArray. The new filenames will be calculated within this
     * function: The [modality-prefix] will be taken from the old filename,
     * [consecutive-numbering] is a consecutively numbered, 6 digit number which will be calculated
     * starting from 000001.
     *
     * Parameters:
     *   none.
     */
{
  int counter = 1;

  OFListIterator(OFString) first = outputFileNameArray.begin();
  OFListIterator(OFString) last = outputFileNameArray.end();

  // before we deal with all the filenames which are included in the array, we need to distinguish
  // two different cases: If endOfStudyThroughTimeoutEvent is not true, the last filename in the array
  // refers to a file that belongs to a new study of which the first object was just received. (In this
  // case there are at least two filenames in the array). Then, this last filename is - at the end of the
  // following loop - not supposed to be deleted from the array. If endOfStudyThroughTimeoutEvent is true,
  // all filenames that are captured in the array, refer to files that belong to the same study. Hence,
  // all of these files shall be renamed and all of the filenames within the array shall be deleted.
  if( !endOfStudyThroughTimeoutEvent ) --last;

  // rename all files that belong to the last study
  while (first != last)
  {
    // determine the new file name: The first two characters of the old file name make up the [modality-prefix].
    // The value for [consecutive-numbering] will be determined using the counter variable.
    char modalityId[3];
    char newFileName[9];
    if (opt_timeNames)
    {
      // modality prefix are the first 2 characters after serial number (if present)
      size_t serialPos = (*first).find("_");
      if (serialPos != OFString_npos)
      {
        //serial present: copy modality prefix (skip serial: 1 digit "_" + 4 digits serial + 1 digit ".")
        OFStandard::strlcpy( modalityId, (*first).substr(serialPos+6, 2).c_str(), 3 );
      }
      else
      {
        //serial not present, copy starts directly after first "." (skip 17 for timestamp, one for ".")
        OFStandard::strlcpy( modalityId, (*first).substr(18, 2).c_str(), 3 );
      }
    }
    else
    {
      OFStandard::strlcpy( modalityId, (*first).c_str(), 3 );
    }
    sprintf( newFileName, "%s%06d", modalityId, counter );

    // create two strings containing path and file name for
    // the current filename and the future filename
    OFString oldPathAndFileName;
    oldPathAndFileName = lastStudySubdirectoryPathAndName;
    oldPathAndFileName += PATH_SEPARATOR;
    oldPathAndFileName += *first;

    OFString newPathAndFileName;
    newPathAndFileName = lastStudySubdirectoryPathAndName;
    newPathAndFileName += PATH_SEPARATOR;
    newPathAndFileName += newFileName;

    // rename file
    if( rename( oldPathAndFileName.c_str(), newPathAndFileName.c_str() ) != 0 )
      OFLOG_WARN(storescpLogger, "cannot rename file '" << oldPathAndFileName << "' to '" << newPathAndFileName << "'");

    // remove entry from list
    first = outputFileNameArray.erase(first);

    // increase counter
    counter++;
  }
}


static void executeOnEndOfStudy()
    /*
     * This function deals with the execution of the command line which was passed
     * to option --exec-on-eostudy of the storescp. This command line is captured
     * in opt_execOnEndOfStudy. Note that the command line can contain the placeholder
     * PATH_PLACEHOLDER which needs to be substituted before the command line is actually executed.
     * In detail, PATH_PLACEHOLDER will be substituted by the path to the output directory into which
     * the files of the last study were written.
     *
     * Parameters:
     *   none.
     */
{
  OFString cmd = opt_execOnEndOfStudy;

  // perform substitution for placeholder #p; #p will be substituted by lastStudySubdirectoryPathAndName
  cmd = replaceChars( cmd, OFString(PATH_PLACEHOLDER), lastStudySubdirectoryPathAndName );

  // perform substitution for placeholder #a
  cmd = replaceChars( cmd, OFString(CALLING_AETITLE_PLACEHOLDER), (endOfStudyThroughTimeoutEvent) ? callingAETitle : lastCallingAETitle );

  // perform substitution for placeholder #c
  cmd = replaceChars( cmd, OFString(CALLED_AETITLE_PLACEHOLDER), (endOfStudyThroughTimeoutEvent) ? calledAETitle : lastCalledAETitle );

  // perform substitution for placeholder #r
  cmd = replaceChars( cmd, OFString(CALLING_PRESENTATION_ADDRESS_PLACEHOLDER), (endOfStudyThroughTimeoutEvent) ? callingPresentationAddress : lastCallingPresentationAddress );

  // Execute command in a new process
  executeCommand( cmd );
}


static OFString replaceChars( const OFString &srcstr, const OFString &pattern, const OFString &substitute )
    /*
     * This function replaces all occurrences of pattern in srcstr with substitute and returns
     * the result as a new OFString variable. Note that srcstr itself will not be changed,
     *
     * Parameters:
     *   srcstr     - [in] The source string.
     *   pattern    - [in] The pattern string which shall be substituted.
     *   substitute - [in] The substitute for pattern in srcstr.
     */
{
  OFString result = srcstr;
  size_t pos = 0;

  while( pos != OFString_npos )
  {
    pos = result.find( pattern, pos );

    if( pos != OFString_npos )
    {
      result.replace( pos, pattern.size(), substitute );
      pos += substitute.size();
    }
  }

  return( result );
}


static void executeCommand( const OFString &cmd )
    /*
     * This function executes the given command line. The execution will be
     * performed in a new process which can be run in the background
     * so that it does not slow down the execution of storescp.
     *
     * Parameters:
     *   cmd - [in] The command which shall be executed.
     */
{
#ifdef HAVE_FORK
  pid_t pid = fork();
  if( pid < 0 )     // in case fork failed, dump an error message
    OFLOG_ERROR(storescpLogger, "cannot execute command '" << cmd << "' (fork failed)");
  else if (pid > 0)
  {
    /* we are the parent process */
    /* remove pending zombie child processes */
    cleanChildren(pid, opt_execSync);
  }
  else // in case we are the child process, execute the command etc.
  {
    // execute command through execl will terminate the child process.
    // Since we only have a single command string and not a list of arguments,
    // we 'emulate' a call to system() by passing the command to /bin/sh
    // which hopefully exists on all Posix systems.

    if (execl( "/bin/sh", "/bin/sh", "-c", cmd.c_str(), NULL ) < 0)
      OFLOG_ERROR(storescpLogger, "cannot execute /bin/sh");

    // if execl succeeds, this part will not get executed.
    // if execl fails, there is not much we can do except bailing out.
    abort();
  }
#else
  PROCESS_INFORMATION procinfo;
  STARTUPINFO sinfo;
  OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
  sinfo.cb = sizeof(sinfo);

  // execute command (Attention: Do not pass DETACHED_PROCESS as sixth argument to the below
  // called function because in such a case the execution of batch-files is not going to work.)
  if( !CreateProcess(NULL, OFconst_cast(char *, cmd.c_str()), NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo) )
    OFLOG_ERROR(storescpLogger, "cannot execute command '" << cmd << "'");

  if (opt_execSync)
  {
    // Wait until child process exits (makes execution synchronous)
    WaitForSingleObject(procinfo.hProcess, INFINITE);
  }

  // Close process and thread handles to avoid resource leak
  CloseHandle(procinfo.hProcess);
  CloseHandle(procinfo.hThread);
#endif
}


static void cleanChildren(pid_t pid, OFBool synch)
  /*
   * This function removes child processes that have terminated,
   * i.e. converted to zombies. Should be called now and then.
   */
{
#ifdef HAVE_WAITPID
  int stat_loc;
#elif HAVE_WAIT3
  struct rusage rusage;
#if defined(__NeXT__)
  /* some systems need a union wait as argument to wait3 */
  union wait status;
#else
  int        status;
#endif
#endif

#if defined(HAVE_WAITPID) || defined(HAVE_WAIT3)
  int child = 1;
  int options = synch ? 0 : WNOHANG;
  while (child > 0)
  {
#ifdef HAVE_WAITPID
    child = OFstatic_cast(int, waitpid(pid, &stat_loc, options));
#elif defined(HAVE_WAIT3)
    child = wait3(&status, options, &rusage);
#endif
    if (child < 0)
    {
      if (errno != ECHILD)
      {
        char buf[256];
        OFLOG_WARN(storescpLogger, "wait for child failed: " << OFStandard::strerror(errno, buf, sizeof(buf)));
      }
    }

    if (synch) child = -1; // break out of loop
  }
#endif
}


static
DUL_PRESENTATIONCONTEXT *
findPresentationContextID(LST_HEAD * head,
                          T_ASC_PresentationContextID presentationContextID)
{
  DUL_PRESENTATIONCONTEXT *pc;
  LST_HEAD **l;
  OFBool found = OFFalse;

  if (head == NULL)
    return NULL;

  l = &head;
  if (*l == NULL)
    return NULL;

  pc = OFstatic_cast(DUL_PRESENTATIONCONTEXT *, LST_Head(l));
  (void)LST_Position(l, OFstatic_cast(LST_NODE *, pc));

  while (pc && !found) {
    if (pc->presentationContextID == presentationContextID) {
      found = OFTrue;
    } else {
      pc = OFstatic_cast(DUL_PRESENTATIONCONTEXT *, LST_Next(l));
    }
  }
  return pc;
}


/** accept all presentation contexts for unknown SOP classes,
 *  i.e. UIDs appearing in the list of abstract syntaxes
 *  where no corresponding name is defined in the UID dictionary.
 *  @param params pointer to association parameters structure
 *  @param transferSyntax transfer syntax to accept
 *  @param acceptedRole SCU/SCP role to accept
 */
static OFCondition acceptUnknownContextsWithTransferSyntax(
  T_ASC_Parameters * params,
  const char* transferSyntax,
  T_ASC_SC_ROLE acceptedRole)
{
  OFCondition cond = EC_Normal;
  int n, i, k;
  DUL_PRESENTATIONCONTEXT *dpc;
  T_ASC_PresentationContext pc;
  OFBool accepted = OFFalse;
  OFBool abstractOK = OFFalse;

  n = ASC_countPresentationContexts(params);
  for (i = 0; i < n; i++)
  {
    cond = ASC_getPresentationContext(params, i, &pc);
    if (cond.bad()) return cond;
    abstractOK = OFFalse;
    accepted = OFFalse;

    if (dcmFindNameOfUID(pc.abstractSyntax) == NULL)
    {
      abstractOK = OFTrue;

      /* check the transfer syntax */
      for (k = 0; (k < OFstatic_cast(int, pc.transferSyntaxCount)) && !accepted; k++)
      {
        if (strcmp(pc.proposedTransferSyntaxes[k], transferSyntax) == 0)
        {
          accepted = OFTrue;
        }
      }
    }

    if (accepted)
    {
      cond = ASC_acceptPresentationContext(
        params, pc.presentationContextID,
        transferSyntax, acceptedRole);
      if (cond.bad()) return cond;
    } else {
      T_ASC_P_ResultReason reason;

      /* do not refuse if already accepted */
      dpc = findPresentationContextID(params->DULparams.acceptedPresentationContext,
                                      pc.presentationContextID);
      if ((dpc == NULL) || ((dpc != NULL) && (dpc->result != ASC_P_ACCEPTANCE)))
      {

        if (abstractOK) {
          reason = ASC_P_TRANSFERSYNTAXESNOTSUPPORTED;
        } else {
          reason = ASC_P_ABSTRACTSYNTAXNOTSUPPORTED;
        }
        /*
         * If previously this presentation context was refused
         * because of bad transfer syntax let it stay that way.
         */
        if ((dpc != NULL) && (dpc->result == ASC_P_TRANSFERSYNTAXESNOTSUPPORTED))
          reason = ASC_P_TRANSFERSYNTAXESNOTSUPPORTED;

        cond = ASC_refusePresentationContext(params, pc.presentationContextID, reason);
        if (cond.bad()) return cond;
      }
    }
  }
  return EC_Normal;
}


/** accept all presentation contexts for unknown SOP classes,
 *  i.e. UIDs appearing in the list of abstract syntaxes
 *  where no corresponding name is defined in the UID dictionary.
 *  This method is passed a list of "preferred" transfer syntaxes.
 *  @param params pointer to association parameters structure
 *  @param transferSyntax transfer syntax to accept
 *  @param acceptedRole SCU/SCP role to accept
 */
static OFCondition acceptUnknownContextsWithPreferredTransferSyntaxes(
  T_ASC_Parameters * params,
  const char* transferSyntaxes[], int transferSyntaxCount,
  T_ASC_SC_ROLE acceptedRole)
{
  OFCondition cond = EC_Normal;
  /*
  ** Accept in the order "least wanted" to "most wanted" transfer
  ** syntax.  Accepting a transfer syntax will override previously
  ** accepted transfer syntaxes.
  */
  for (int i = transferSyntaxCount - 1; i >= 0; i--)
  {
    cond = acceptUnknownContextsWithTransferSyntax(params, transferSyntaxes[i], acceptedRole);
    if (cond.bad()) return cond;
  }
  return cond;
}
