/*
 *
 *  Copyright (C) 1994-2004, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmnet
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Storage Service Class Provider (C-STORE operation)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-02-22 09:40:54 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/apps/storescp.cc,v $
 *  CVS/RCS Revision: $Revision: 1.75 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CCTYPE
#include "ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <direct.h>        /* for _mkdir() */
#endif


#include "dimse.h"
#include "diutil.h"
#include "dcfilefo.h"
#include "dcdebug.h"
#include "dcuid.h"
#include "dcdict.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "ofstd.h"
#include "ofdatime.h"
#include "dcuid.h"         /* for dcmtk version name */
#include "dicom.h"         /* for DICOM_APPLICATION_ACCEPTOR */
#include "dcdeftag.h"      /* for DCM_StudyInstanceUID */
#include "dcostrmz.h"      /* for dcmZlibCompressionLevel */
#include "dcasccfg.h"      /* for class DcmAssociationConfiguration */
#include "dcasccff.h"      /* for class DcmAssociationConfigurationFile */

#ifdef WITH_OPENSSL
#include "tlstrans.h"
#include "tlslayer.h"
#endif

#ifdef WITH_ZLIB
#include <zlib.h>          /* for zlibVersion() */
#endif

#ifdef PRIVATE_STORESCP_DECLARATIONS
PRIVATE_STORESCP_DECLARATIONS
#else
#define OFFIS_CONSOLE_APPLICATION "storescp"
#endif

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v" OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define APPLICATIONTITLE "STORESCP"     /* our application entity title */

#define PATH_PLACEHOLDER "#p"
#define FILENAME_PLACEHOLDER "#f"
#define CALLING_AETITLE_PLACEHOLDER "#a"
#define CALLED_AETITLE_PLACEHOLDER "#c"

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
static void cleanChildren();
static OFCondition acceptUnknownContextsWithPreferredTransferSyntaxes(
         T_ASC_Parameters * params,
         const char* transferSyntaxes[],
         int transferSyntaxCount,
         T_ASC_SC_ROLE acceptedRole = ASC_SC_ROLE_DEFAULT);

OFBool             opt_uniqueFilenames = OFFalse;
OFCmdUnsignedInt   opt_port = 0;
OFBool             opt_refuseAssociation = OFFalse;
OFBool             opt_rejectWithoutImplementationUID = OFFalse;
OFCmdUnsignedInt   opt_sleepAfter = 0;
OFCmdUnsignedInt   opt_sleepDuring = 0;
OFCmdUnsignedInt   opt_maxPDU = ASC_DEFAULTMAXPDU;
OFBool             opt_useMetaheader = OFTrue;
E_TransferSyntax   opt_networkTransferSyntax = EXS_Unknown;
E_TransferSyntax   opt_writeTransferSyntax = EXS_Unknown;
E_GrpLenEncoding   opt_groupLength = EGL_recalcGL;
E_EncodingType     opt_sequenceType = EET_ExplicitLength;
E_PaddingEncoding  opt_paddingType = EPD_withoutPadding;
OFCmdUnsignedInt   opt_filepad = 0;
OFCmdUnsignedInt   opt_itempad = 0;
OFCmdUnsignedInt   opt_compressionLevel = 0;
OFBool             opt_verbose = OFFalse;
OFBool             opt_debug = OFFalse;
OFBool             opt_bitPreserving = OFFalse;
OFBool             opt_ignore = OFFalse;
OFBool             opt_abortDuringStore = OFFalse;
OFBool             opt_abortAfterStore = OFFalse;
OFBool             opt_promiscuous = OFFalse;
OFBool             opt_correctUIDPadding = OFFalse;
OFString           callingaetitle;  // calling AE title will be stored here
OFString           calledaetitle;   // called AE title will be stored here
const char *       opt_respondingaetitle = APPLICATIONTITLE;
static OFBool      opt_secureConnection = OFFalse;    // default: no secure connection
static OFString    opt_outputDirectory(".");          // default: output directory equals "."
static const char *opt_sortConcerningStudies = NULL;  // default: no sorting
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

#ifdef WITH_OPENSSL
static int         opt_keyFileFormat = SSL_FILETYPE_PEM;
static const char *opt_privateKeyFile = NULL;
static const char *opt_certificateFile = NULL;
static const char *opt_passwd = NULL;
static OFString    opt_ciphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
static const char *opt_readSeedFile = NULL;
static const char *opt_writeSeedFile = NULL;
static DcmCertificateVerification opt_certVerification = DCV_requireCertificate;
static const char *opt_dhparam = NULL;
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
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM storage (C-STORE) SCP", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL+SHORTCOL+4);
  cmd.addParam("port", "tcp/ip port number to listen on");

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL+2);
    cmd.addOption("--help",                      "-h",       "print this help text and exit");
    cmd.addOption("--version",                               "print version information and exit", OFTrue /* exclusive */);
    cmd.addOption("--verbose",                   "-v",       "verbose mode, print processing details");
    cmd.addOption("--debug",                     "-d",       "debug mode, print debug information");
    OFString opt0 = "write output-files to (existing) directory p\n(default: ";
    opt0 += opt_outputDirectory;
    opt0 += ")";
    cmd.addOption("--output-directory",          "-od",   1, "[p]ath: string",
                                                             opt0.c_str());

  cmd.addGroup("network options:");
    cmd.addSubGroup("association negotiation profile from configuration file:");
      cmd.addOption("--config-file",            "-xf",    2, "[f]ilename, [p]rofile: string",
                                                             "use profile p from config file f");
    cmd.addSubGroup("preferred network transfer syntaxes (not with --config-file):");
      cmd.addOption("--prefer-uncompr",         "+x=",       "prefer explicit VR local byte order (default)");
      cmd.addOption("--prefer-little",          "+xe",       "prefer explicit VR little endian TS");
      cmd.addOption("--prefer-big",             "+xb",       "prefer explicit VR big endian TS");
      cmd.addOption("--prefer-lossless",        "+xs",       "prefer default JPEG lossless TS");
      cmd.addOption("--prefer-jpeg8",           "+xy",       "prefer default JPEG lossy TS for 8 bit data");
      cmd.addOption("--prefer-jpeg12",          "+xx",       "prefer default JPEG lossy TS for 12 bit data");
      cmd.addOption("--prefer-rle",             "+xr",       "prefer RLE lossless TS");
#ifdef WITH_ZLIB
      cmd.addOption("--prefer-deflated",        "+xd",       "prefer deflated expl. VR little endian TS");
#endif

      cmd.addOption("--implicit",               "+xi",       "accept implicit VR little endian TS only");

#ifdef WITH_TCPWRAPPER
    cmd.addSubGroup("network host access control (tcp wrapper) options:");
      cmd.addOption("--access-full",            "-ac",       "accept connections from any host (default)");
      cmd.addOption("--access-control",         "+ac",       "enforce host access control rules");
#endif

    cmd.addSubGroup("other network options:");
      OFString opt1 = "set my AE title (default: ";
      opt1 += APPLICATIONTITLE;
      opt1 += ")";
      cmd.addOption("--aetitle",                "-aet",  1,  "aetitle: string", opt1.c_str());
      OFString opt3 = "set max receive pdu to n bytes (def.: ";
      sprintf(tempstr, "%ld", OFstatic_cast(long, ASC_DEFAULTMAXPDU));
      opt3 += tempstr;
      opt3 += ")";
      OFString opt4 = "[n]umber of bytes: integer [";
      sprintf(tempstr, "%ld", OFstatic_cast(long, ASC_MINIMUMPDUSIZE));
      opt4 += tempstr;
      opt4 += "..";
      sprintf(tempstr, "%ld", OFstatic_cast(long, ASC_MAXIMUMPDUSIZE));
      opt4 += tempstr;
      opt4 += "]";
      cmd.addOption("--max-pdu",                "-pdu",  1,  opt4.c_str(), opt3.c_str());
      cmd.addOption("--disable-host-lookup",    "-dhl",      "disable hostname lookup");
      cmd.addOption("--refuse",                              "refuse association");
      cmd.addOption("--reject",                              "reject association if no implement. class UID");
      cmd.addOption("--ignore",                              "ignore store data, receive but do not store");
      cmd.addOption("--sleep-after",                     1,  "[s]econds: integer",
                                                             "sleep s seconds after store (default: 0)");
      cmd.addOption("--sleep-during",                    1,  "[s]econds: integer",
                                                             "sleep s seconds during store (default: 0)");
      cmd.addOption("--abort-after",                         "abort association after receipt of C-STORE-RQ\n(but before sending response)");
      cmd.addOption("--abort-during",                        "abort association during receipt of C-STORE-RQ");
      cmd.addOption("--promiscuous",            "-pm",       "promiscuous mode, accept unknown SOP classes\n(not with --config-file)");
      cmd.addOption("--uid-padding",            "-up",       "silently correct space-padded UIDs");

  cmd.addGroup("output options:");
    cmd.addSubGroup("bit preserving mode:");
      cmd.addOption("--normal",                 "-B",        "allow implicit format conversions (default)");
      cmd.addOption("--bit-preserving",         "+B",        "write data exactly as read (not with -ss)");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",             "+F",        "write file format (default)");
      cmd.addOption("--write-dataset",          "-F",        "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax (not with --bit-preserving or compr. transmission):");
      cmd.addOption("--write-xfer-same",        "+t=",       "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",      "+te",       "write with explicit VR little endian TS");
      cmd.addOption("--write-xfer-big",         "+tb",       "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit",    "+ti",       "write with implicit VR little endian TS");
#ifdef WITH_ZLIB
      cmd.addOption("--write-xfer-deflated",    "+td",       "write with deflated expl. VR little endian TS");
#endif
    cmd.addSubGroup("post-1993 value representations (not with --bit-preserving):");
      cmd.addOption("--enable-new-vr",          "+u",        "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",         "-u",        "disable support for new VRs, convert to OB");
    cmd.addSubGroup("group length encoding (not with --bit-preserving):");
      cmd.addOption("--group-length-recalc",    "+g=",       "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create",    "+g",        "always write with group length elements");
      cmd.addOption("--group-length-remove",    "-g",        "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items (not with --bit-preserving):");
      cmd.addOption("--length-explicit",        "+e",        "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",       "-e",        "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset or --bit-preserving):");
      cmd.addOption("--padding-off",            "-p",        "no padding (default)");
      cmd.addOption("--padding-create",         "+p",    2,  "[f]ile-pad [i]tem-pad: integer",
                                                             "align file on multiple of f bytes and items\non multiple of i bytes");
#ifdef WITH_ZLIB
    cmd.addSubGroup("deflate compression level (not with --write-xfer-little/big/implicit):");
      cmd.addOption("--compression-level",      "+cl",   1,  "compression level: 0-9 (default 6)",
                                                             "0=uncompressed, 1=fastest, 9=best compression");
#endif
    cmd.addSubGroup("sorting into subdirectories (not with --bit-preserving):");
      cmd.addOption("--sort-conc-studies",      "-ss",   1,  "[p]refix: string",
                                                             "sort concerning studies into subdirectories\nthat start with prefix p" );
    cmd.addSubGroup("filename generation:");
      cmd.addOption("--default-filenames",      "-uf",       "generate filename from instance UID (default)");
      cmd.addOption("--unique-filenames",       "+uf",       "generate unique filenames");

  cmd.addGroup("event options:", LONGCOL, SHORTCOL+2);
    cmd.addOption(  "--exec-on-reception",      "-xcr",  1,  "[c]ommand: string",
                                                             "execute command c after having received and\nprocessed one C-STORE-Request message" );
    cmd.addOption(  "--exec-on-eostudy",        "-xcs",  1,  "[c]ommand: string (only w/ -ss)",
                                                             "execute command c after having received and\nprocessed all C-STORE-Request messages that\nbelong to one study" );
    cmd.addOption(  "--rename-on-eostudy",      "-rns",      "(only w/ -ss) Having received and processed\nall C-STORE-Request messages that belong to\none study, rename output files according to\na certain pattern" );
    cmd.addOption(  "--eostudy-timeout",        "-tos",  1,  "[t]imeout: integer (only w/ -ss, -xcs or -rns)",
                                                             "specifies a timeout of t seconds for\nend-of-study determination" );

#ifdef WITH_OPENSSL
  cmd.addGroup("transport layer security (TLS) options:");
    cmd.addSubGroup("transport protocol stack options:");
      cmd.addOption("--disable-tls",            "-tls",      "use normal TCP/IP connection (default)");
      cmd.addOption("--enable-tls",             "+tls",  2,  "[p]rivate key file, [c]ertificate file: string",
                                                             "use authenticated secure TLS connection");
    cmd.addSubGroup("private key password options (only with --enable-tls):");
      cmd.addOption("--std-passwd",             "+ps",       "prompt user to type password on stdin (default)");
      cmd.addOption("--use-passwd",             "+pw",   1,  "[p]assword: string ",
                                                             "use specified password");
      cmd.addOption("--null-passwd",            "-pw",       "use empty string as password");
    cmd.addSubGroup("key and certificate file format options:");
      cmd.addOption("--pem-keys",               "-pem",      "read keys and certificates as PEM file (def.)");
      cmd.addOption("--der-keys",               "-der",      "read keys and certificates as DER file");
    cmd.addSubGroup("certification authority options:");
      cmd.addOption("--add-cert-file",         "+cf",    1,  "[c]ertificate filename: string",
                                                             "add certificate file to list of certificates");
      cmd.addOption("--add-cert-dir",          "+cd",    1,  "[c]ertificate directory: string",
                                                             "add certificates in d to list of certificates");
    cmd.addSubGroup("ciphersuite options:");
      cmd.addOption("--cipher",                "+cs",    1,  "[c]iphersuite name: string",
                                                             "add ciphersuite to list of negotiated suites");
      cmd.addOption("--dhparam",               "+dp",    1,  "[f]ilename: string",
                                                             "read DH parameters for DH/DSS ciphersuites");
    cmd.addSubGroup("pseudo random generator options:");
      cmd.addOption("--seed",                  "+rs",    1,  "[f]ilename: string",
                                                             "seed random generator with contents of f");
      cmd.addOption("--write-seed",            "+ws",        "write back modified seed (only with --seed)");
      cmd.addOption("--write-seed-file",       "+wf",    1,  "[f]ilename: string (only with --seed)",
                                                             "write modified seed to file f");
    cmd.addSubGroup("peer authentication options:");
      cmd.addOption("--require-peer-cert",     "-rc",        "verify peer certificate, fail if absent (def.)");
      cmd.addOption("--verify-peer-cert",      "-vc",        "verify peer certificate if present");
      cmd.addOption("--ignore-peer-cert",      "-ic",        "don't verify peer certificate");
#endif

  /* evaluate command line */
  prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
  if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
  {
    /* check exclusive options first */

    if (cmd.getParamCount() == 0)
    {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);          // uses ofConsole.lockCerr()
            CERR << endl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_OPENSSL)
            CERR << " none" << endl;
#else
            CERR << endl;
#endif
#ifdef WITH_ZLIB
            CERR << "- ZLIB, Version " << zlibVersion() << endl;
#endif
#ifdef WITH_OPENSSL
            CERR << "- " << OPENSSL_VERSION_TEXT << endl;
#endif
            return 0;
        }
    }

    /* command line parameters */

    app.checkParam(cmd.getParamAndCheckMinMax(1, opt_port, 1, 65535));

    if (cmd.findOption("--verbose")) opt_verbose=OFTrue;
    if (cmd.findOption("--debug"))
    {
      opt_debug = OFTrue;
      DUL_Debug(OFTrue);
      DIMSE_debug(OFTrue);
      SetDebugLevel(3);
    }
    if (cmd.findOption("--output-directory")) app.checkValue(cmd.getValue(opt_outputDirectory));

    cmd.beginOptionBlock();
    if (cmd.findOption("--prefer-uncompr"))  opt_networkTransferSyntax = EXS_Unknown;
    if (cmd.findOption("--prefer-little"))   opt_networkTransferSyntax = EXS_LittleEndianExplicit;
    if (cmd.findOption("--prefer-big"))      opt_networkTransferSyntax = EXS_BigEndianExplicit;
    if (cmd.findOption("--prefer-lossless")) opt_networkTransferSyntax = EXS_JPEGProcess14SV1TransferSyntax;
    if (cmd.findOption("--prefer-jpeg8"))    opt_networkTransferSyntax = EXS_JPEGProcess1TransferSyntax;
    if (cmd.findOption("--prefer-jpeg12"))   opt_networkTransferSyntax = EXS_JPEGProcess2_4TransferSyntax;
    if (cmd.findOption("--prefer-rle"))      opt_networkTransferSyntax = EXS_RLELossless;
#ifdef WITH_ZLIB
    if (cmd.findOption("--prefer-deflated")) opt_networkTransferSyntax = EXS_DeflatedLittleEndianExplicit;
#endif
    if (cmd.findOption("--implicit"))        opt_networkTransferSyntax = EXS_LittleEndianImplicit;
    cmd.endOptionBlock();

    if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_respondingaetitle));
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
      app.checkValue(cmd.getValue(opt_configFile));
      app.checkValue(cmd.getValue(opt_profileName));

      // check conflicts with other command line options
      app.checkConflict("--config-file", "--prefer-little", (opt_networkTransferSyntax == EXS_LittleEndianExplicit));
      app.checkConflict("--config-file", "--prefer-big", (opt_networkTransferSyntax == EXS_BigEndianExplicit));
      app.checkConflict("--config-file", "--prefer-lossless", (opt_networkTransferSyntax == EXS_JPEGProcess14SV1TransferSyntax));
      app.checkConflict("--config-file", "--prefer-jpeg8", (opt_networkTransferSyntax == EXS_JPEGProcess1TransferSyntax));
      app.checkConflict("--config-file", "--prefer-jpeg12", (opt_networkTransferSyntax == EXS_JPEGProcess2_4TransferSyntax));
      app.checkConflict("--config-file", "--prefer-rle", (opt_networkTransferSyntax == EXS_RLELossless));
#ifdef WITH_ZLIB
      app.checkConflict("--config-file", "--prefer-deflated", (opt_networkTransferSyntax == EXS_DeflatedLittleEndianExplicit));
#endif
      app.checkConflict("--config-file", "--implicit", (opt_networkTransferSyntax == EXS_LittleEndianImplicit));
      app.checkConflict("--config-file", "--promiscuous", opt_promiscuous);

      // read configuration file
      OFCondition cond = DcmAssociationConfigurationFile::initialize(asccfg, opt_configFile);
      if (cond.bad())
      {
        CERR << "error reading config file: "
             << cond.text() << endl;
        return 1;
      }

      /* perform name mangling for config file key */
      OFString sprofile;
      const char *c = opt_profileName;
      while (*c)
      {
        if (! isspace(*c)) sprofile += OFstatic_cast(char, toupper(*c));
        ++c;
      }

      if (!asccfg.isKnownProfile(sprofile.c_str()))
      {
        CERR << "unknown configuration profile name: "
             << sprofile << endl;
        return 1;
      }

      if (!asccfg.isValidSCPProfile(sprofile.c_str()))
      {
        CERR << "profile '"
             << sprofile
             << "' is not valid for SCP use, duplicate abstract syntaxes found."
             << endl;
        return 1;
      }

    }

#ifdef WITH_TCPWRAPPER
    cmd.beginOptionBlock();
    if (cmd.findOption("--access-full")) dcmTCPWrapperDaemonName.set(NULL);
    if (cmd.findOption("--access-control")) dcmTCPWrapperDaemonName.set(OFFIS_CONSOLE_APPLICATION);
    cmd.endOptionBlock();
#endif

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
      app.checkConflict("--write-xfer-little", "--bit-preserving", opt_bitPreserving);
      app.checkConflict("--write-xfer-little", "--prefer-lossless", opt_networkTransferSyntax==EXS_JPEGProcess14SV1TransferSyntax);
      app.checkConflict("--write-xfer-little", "--prefer-jpeg8", opt_networkTransferSyntax==EXS_JPEGProcess1TransferSyntax);
      app.checkConflict("--write-xfer-little", "--prefer-jpeg12", opt_networkTransferSyntax==EXS_JPEGProcess2_4TransferSyntax);
      app.checkConflict("--write-xfer-little", "--prefer-rle", opt_networkTransferSyntax==EXS_RLELossless);
      opt_writeTransferSyntax = EXS_LittleEndianExplicit;
    }
    if (cmd.findOption("--write-xfer-big"))
    {
      app.checkConflict("--write-xfer-big", "--bit-preserving", opt_bitPreserving);
      app.checkConflict("--write-xfer-big", "--prefer-lossless", opt_networkTransferSyntax==EXS_JPEGProcess14SV1TransferSyntax);
      app.checkConflict("--write-xfer-big", "--prefer-jpeg8", opt_networkTransferSyntax==EXS_JPEGProcess1TransferSyntax);
      app.checkConflict("--write-xfer-big", "--prefer-jpeg12", opt_networkTransferSyntax==EXS_JPEGProcess2_4TransferSyntax);
      app.checkConflict("--write-xfer-big", "--prefer-rle", opt_networkTransferSyntax==EXS_RLELossless);
      opt_writeTransferSyntax = EXS_BigEndianExplicit;
    }
    if (cmd.findOption("--write-xfer-implicit"))
    {
      app.checkConflict("--write-xfer-implicit", "--bit-preserving", opt_bitPreserving);
      app.checkConflict("--write-xfer-implicit", "--prefer-lossless", opt_networkTransferSyntax==EXS_JPEGProcess14SV1TransferSyntax);
      app.checkConflict("--write-xfer-implicit", "--prefer-jpeg8", opt_networkTransferSyntax==EXS_JPEGProcess1TransferSyntax);
      app.checkConflict("--write-xfer-implicit", "--prefer-jpeg12", opt_networkTransferSyntax==EXS_JPEGProcess2_4TransferSyntax);
      app.checkConflict("--write-xfer-implicit", "--prefer-rle", opt_networkTransferSyntax==EXS_RLELossless);
      opt_writeTransferSyntax = EXS_LittleEndianImplicit;
    }
#ifdef WITH_ZLIB
    if (cmd.findOption("--write-xfer-deflated"))
    {
      app.checkConflict("--write-xfer-deflated", "--bit-preserving", opt_bitPreserving);
      app.checkConflict("--write-xfer-deflated", "--prefer-lossless", opt_networkTransferSyntax==EXS_JPEGProcess14SV1TransferSyntax);
      app.checkConflict("--write-xfer-deflated", "--prefer-jpeg8", opt_networkTransferSyntax==EXS_JPEGProcess1TransferSyntax);
      app.checkConflict("--write-xfer-deflated", "--prefer-jpeg12", opt_networkTransferSyntax==EXS_JPEGProcess2_4TransferSyntax);
      app.checkConflict("--write-xfer-deflated", "--prefer-rle", opt_networkTransferSyntax==EXS_RLELossless);
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
    }
    if (cmd.findOption("--disable-new-vr"))
    {
      app.checkConflict("--disable-new-vr", "--bit-preserving", opt_bitPreserving);
      dcmEnableUnknownVRGeneration.set(OFFalse);
      dcmEnableUnlimitedTextVRGeneration.set(OFFalse);
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
      app.checkConflict("--padding-create", "--write-dataset", ! opt_useMetaheader);
      app.checkConflict("--padding-create", "--bit-preserving", opt_bitPreserving);
      app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
      app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
      opt_paddingType = EPD_withPadding;
    }
    cmd.endOptionBlock();

#ifdef WITH_ZLIB
    cmd.beginOptionBlock();
    if (cmd.findOption("--compression-level"))
    {
        if (opt_writeTransferSyntax != EXS_DeflatedLittleEndianExplicit && opt_writeTransferSyntax != EXS_Unknown)
          app.printError("--compression-level only allowed with --write-xfer-deflated or --write-xfer-same");
        app.checkValue(cmd.getValueAndCheckMinMax(opt_compressionLevel, 0, 9));
        dcmZlibCompressionLevel.set(OFstatic_cast(int, opt_compressionLevel));
    }
    cmd.endOptionBlock();
#endif

    if (cmd.findOption("--sort-conc-studies"))
    {
      app.checkConflict("--sort-conc-studies", "--bit-preserving", opt_bitPreserving);
      app.checkValue(cmd.getValue(opt_sortConcerningStudies));
    }

    cmd.beginOptionBlock();
    if (cmd.findOption("--default-filenames")) opt_uniqueFilenames = OFFalse;
    if (cmd.findOption("--unique-filenames")) opt_uniqueFilenames = OFTrue;
    cmd.endOptionBlock();

    if (cmd.findOption("--exec-on-reception")) app.checkValue(cmd.getValue(opt_execOnReception));

    if (cmd.findOption("--exec-on-eostudy"))
    {
      app.checkDependence("--exec-on-eostudy", "--sort-conc-studies", opt_sortConcerningStudies != NULL );
      app.checkValue(cmd.getValue(opt_execOnEndOfStudy));
    }

    if (cmd.findOption("--rename-on-eostudy"))
    {
      app.checkDependence("--rename-on-eostudy", "--sort-conc-studies", opt_sortConcerningStudies != NULL );
      opt_renameOnEndOfStudy = OFTrue;
    }

    if (cmd.findOption("--eostudy-timeout"))
    {
      if( opt_sortConcerningStudies == NULL && opt_execOnEndOfStudy == NULL && opt_renameOnEndOfStudy == OFFalse )
        app.printError("--eostudy-timeout only in combination with --sort-conc-studies, --exec-on-eostudy or --rename-on-eostudy");
      app.checkValue(cmd.getValueAndCheckMin(opt_endOfStudyTimeout, 0));
    }
  }

#ifdef WITH_OPENSSL

#ifdef DEBUG
  /* prevent command line code from moaning that --add-cert-dir and --add-cert-file have not been checked */
  if (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_First)) /* nothing */ ;
  if (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_First)) /* nothing */ ;
#endif

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
    if (! opt_secureConnection) app.printError("--std-passwd only with --enable-tls");
    opt_passwd = NULL;
  }
  if (cmd.findOption("--use-passwd"))
  {
    if (! opt_secureConnection) app.printError("--use-passwd only with --enable-tls");
    app.checkValue(cmd.getValue(opt_passwd));
  }
  if (cmd.findOption("--null-passwd"))
  {
    if (! opt_secureConnection) app.printError("--null-passwd only with --enable-tls");
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
    if (opt_readSeedFile == NULL) app.printError("--write-seed only with --seed");
    opt_writeSeedFile = opt_readSeedFile;
  }
  if (cmd.findOption("--write-seed-file"))
  {
    if (opt_readSeedFile == NULL) app.printError("--write-seed-file only with --seed");
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
        CERR << "ciphersuite '" << current << "' is unknown. Known ciphersuites are:" << endl;
        unsigned long numSuites = DcmTLSTransportLayer::getNumberOfCipherSuites();
        for (unsigned long cs=0; cs < numSuites; cs++)
        {
          CERR << "    " << DcmTLSTransportLayer::getTLSCipherSuiteName(cs) << endl;
        }
        return 1;
      }
      else
      {
        if (opt_ciphersuites.length() > 0) opt_ciphersuites += ":";
        opt_ciphersuites += currentOpenSSL;
      }
    } while (cmd.findOption("--cipher", 0, OFCommandLine::FOM_Next));
  }

#endif


#ifdef HAVE_GETEUID
  /* if port is privileged we must be as well */
  if (opt_port < 1024)
  {
    if (geteuid() != 0)
    {
      fprintf(stderr, "storescp: cannot listen on port %lu, insufficient privileges\n", opt_port);
      return 1;
    }
  }
#endif

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
    fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n", DCM_DICT_ENVIRONMENT_VARIABLE);
  }

  /* if the output directory does not equal "." (default directory) */
  if( opt_outputDirectory.compare(".") != 0 )
  {
    /* if there is a path separator at the end of the path, get rid of it */
    if( opt_outputDirectory[opt_outputDirectory.length()-1] == PATH_SEPARATOR )
    {
      opt_outputDirectory.erase(opt_outputDirectory.length()-1, 1);
    }

    /* check if the specified directory exists and if it is a directory.
     * If the output directory is invalid, dump an error message and terminate execution.
     */
    if( !OFStandard::dirExists(opt_outputDirectory) )
    {
      CERR << "Error: invalid output directory encountered ('" << opt_outputDirectory << "')" << endl;
      return 1;
    }
  }

  /* initialize network, i.e. create an instance of T_ASC_Network*. */
  OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, OFstatic_cast(int, opt_port), 1000, &net);
  if (cond.bad())
  {
    DimseCondition::dump(cond);
    return 1;
  }

#ifdef HAVE_GETUID
  /* return to normal uid so that we can't do too much damage in case
   * things go very wrong.   Only does someting if the program is setuid
   * root, and run by another user.  Running as root user may be
   * potentially disasterous if this program screws up badly.
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
      app.printError("unable to create TLS transport layer");
    }

    if (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_First))
    {
      do
      {
        app.checkValue(cmd.getValue(current));
        if (TCS_ok != tLayer->addTrustedCertificateFile(current, opt_keyFileFormat))
        {
          CERR << "warning unable to load certificate file '" << current << "', ignoring" << endl;
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
          CERR << "warning unable to certificates from directory '" << current << "', ignoring" << endl;
        }
      } while (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_Next));
    }

    if (opt_dhparam && ! (tLayer->setTempDHParameters(opt_dhparam)))
    {
      CERR << "warning unable to load temporary DH parameter file '" << opt_dhparam << "', ignoring" << endl;
    }

    if (opt_passwd) tLayer->setPrivateKeyPasswd(opt_passwd);

    if (TCS_ok != tLayer->setPrivateKeyFile(opt_privateKeyFile, opt_keyFileFormat))
    {
      CERR << "unable to load private TLS key from '" << opt_privateKeyFile << "'" << endl;
      return 1;
    }
    if (TCS_ok != tLayer->setCertificateFile(opt_certificateFile, opt_keyFileFormat))
    {
      CERR << "unable to load certificate from '" << opt_certificateFile << "'" << endl;
      return 1;
    }
    if (! tLayer->checkPrivateKeyMatchesCertificate())
    {
      CERR << "private key '" << opt_privateKeyFile << "' and certificate '" << opt_certificateFile << "' do not match" << endl;
      return 1;
    }

    if (TCS_ok != tLayer->setCipherSuites(opt_ciphersuites.c_str()))
    {
      CERR << "unable to set selected cipher suites" << endl;
      return 1;
    }

    tLayer->setCertificateVerification(opt_certVerification);


    cond = ASC_setTransportLayer(net, tLayer, 0);
    if (cond.bad())
    {
        DimseCondition::dump(cond);
        return 1;
    }
  }

#endif

  while (cond.good())
  {
    /* receive an association and acknowledge or reject it. If the association was */
    /* acknowledged, offer corresponding services and invoke one or more if required. */
    cond = acceptAssociation(net, asccfg);

    /* remove zombie child processes */
    cleanChildren();
#ifdef WITH_OPENSSL
    /* since storescp is usually terminated with SIGTERM or the like,
     * we write back an updated random seed after every association handled.
     */
    if (tLayer && opt_writeSeedFile)
    {
      if (tLayer->canWriteRandomSeed())
      {
        if (!tLayer->writeRandomSeed(opt_writeSeedFile))
        {
          CERR << "Error while writing random seed file '" << opt_writeSeedFile << "', ignoring." << endl;
        }
      }
      else
      {
        CERR << "Warning: cannot write random seed, ignoring." << endl;
      }
    }
#endif

  }

  /* drop the network, i.e. free memory of T_ASC_Network* structure. This call */
  /* is the counterpart of ASC_initializeNetwork(...) which was called above. */
  cond = ASC_dropNetwork(&net);
  if (cond.bad())
  {
    DimseCondition::dump(cond);
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

#ifdef PRIVATE_STORESCP_VARIABLES
  PRIVATE_STORESCP_VARIABLES
#endif

  const char* knownAbstractSyntaxes[] =
  {
    UID_VerificationSOPClass
  };

  const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL };
  int numTransferSyntaxes = 0;

  // try to receive an association. Here we either want to use blocking or
  // non-blocking, depending on if the option --eostudy-timeout is set.
  if( opt_endOfStudyTimeout == -1 )
    cond = ASC_receiveAssociation(net, &assoc, opt_maxPDU, NULL, NULL, opt_secureConnection);
  else
    cond = ASC_receiveAssociation(net, &assoc, opt_maxPDU, NULL, NULL, opt_secureConnection, DUL_NOBLOCK, OFstatic_cast(int, opt_endOfStudyTimeout));

  // if some kind of error occured, take care of it
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
      if( opt_endOfStudyTimeout != -1 && ! lastStudyInstanceUID.empty() )
      {
        // indicate that the end-of-study-event occured through a timeout event.
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
      if( opt_verbose ) DimseCondition::dump(cond);
    }

    // no matter what kind of error occurred, we need to do a cleanup
    goto cleanup;
  }

  if (opt_verbose) printf("Association Received\n");

  if (opt_debug)
  {
    printf("Parameters:\n");
    ASC_dumpParameters(assoc->params, COUT);
  }

  if (opt_refuseAssociation)
  {
    T_ASC_RejectParameters rej =
    {
      ASC_RESULT_REJECTEDPERMANENT,
      ASC_SOURCE_SERVICEUSER,
      ASC_REASON_SU_NOREASON
    };

    if (opt_verbose) printf("Refusing Association (forced via command line)\n");
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      printf("Association Reject Failed:\n");
      DimseCondition::dump(cond);
    }
    goto cleanup;
  }

  switch (opt_networkTransferSyntax)
  {
    case EXS_LittleEndianImplicit:
      /* we only support Little Endian Implicit */
      transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 1;
      break;
    case EXS_LittleEndianExplicit:
      /* we prefer Little Endian Explicit */
      transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[2]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
    case EXS_BigEndianExplicit:
      /* we prefer Big Endian Explicit */
      transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2]  = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
      break;
    case EXS_JPEGProcess14SV1TransferSyntax:
      /* we prefer JPEGLossless:Hierarchical-1stOrderPrediction (default lossless) */
      transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGProcess1TransferSyntax:
      /* we prefer JPEGBaseline (default lossy for 8 bit images) */
      transferSyntaxes[0] = UID_JPEGProcess1TransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 4;
      break;
    case EXS_JPEGProcess2_4TransferSyntax:
      /* we prefer JPEGExtended (default lossy for 12 bit images) */
      transferSyntaxes[0] = UID_JPEGProcess2_4TransferSyntax;
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
      break;
  }

  if (opt_profileName)
  {
    /* perform name mangling for config file key */
    OFString sprofile;
    const char *c = opt_profileName;
    while (*c)
    {
      if (! isspace(*c)) sprofile += OFstatic_cast(char, toupper(*c));
      ++c;
    }

    /* set presentation contexts as defined in config file */
    cond = asccfg.evaluateAssociationParameters(sprofile.c_str(), *assoc);
    if (cond.bad())
    {
      if (opt_verbose) DimseCondition::dump(cond);
      goto cleanup;
    }
  }
  else
  {
    /* accept the Verification SOP Class if presented */
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes( assoc->params, knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes), transferSyntaxes, numTransferSyntaxes);
    if (cond.bad())
    {
      if (opt_verbose) DimseCondition::dump(cond);
      goto cleanup;
    }

    /* the array of Storage SOP Class UIDs comes from dcuid.h */
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes( assoc->params, dcmStorageSOPClassUIDs, numberOfDcmStorageSOPClassUIDs, transferSyntaxes, numTransferSyntaxes);
    if (cond.bad())
    {
      if (opt_verbose) DimseCondition::dump(cond);
      goto cleanup;
    }

    if (opt_promiscuous)
    {
      /* accept everything not known not to be a storage SOP class */
      cond = acceptUnknownContextsWithPreferredTransferSyntaxes(
        assoc->params, transferSyntaxes, numTransferSyntaxes);
      if (cond.bad())
      {
        if (opt_verbose) DimseCondition::dump(cond);
        goto cleanup;
      }
    }
  }

  /* set our app title */
  ASC_setAPTitles(assoc->params, NULL, NULL, opt_respondingaetitle);

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

    if (opt_verbose) printf("Association Rejected: bad application context name: %s\n", buf);
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      if (opt_verbose) DimseCondition::dump(cond);
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

    if (opt_verbose) printf("Association Rejected: No Implementation Class UID provided\n");
    cond = ASC_rejectAssociation(assoc, &rej);
    if (cond.bad())
    {
      if (opt_verbose) DimseCondition::dump(cond);
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
      DimseCondition::dump(cond);
      goto cleanup;
    }
    if (opt_verbose)
    {
      printf("Association Acknowledged (Max Send PDV: %lu)\n", assoc->sendPDVLength);
      if (ASC_countAcceptedPresentationContexts(assoc->params) == 0)
        printf("    (but no valid presentation contexts)\n");
      if (opt_debug) ASC_dumpParameters(assoc->params, COUT);
    }
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

  // store calling and called aetitle in global variables to enable
  // the --exec options using them. Enclose in quotation marks because
  // aetitles may contain space characters.
  DIC_AE callingTitle;
  DIC_AE calledTitle;
  if (ASC_getAPTitles(assoc->params, callingTitle, calledTitle, NULL).good())
  {
    callingaetitle = "\"";
    callingaetitle += callingTitle;
    callingaetitle += "\"";
    calledaetitle = "\"";
    calledaetitle += calledTitle;
    calledaetitle += "\"";
  }
  else
  {
    // should never happen
    callingaetitle.clear();
    calledaetitle.clear();
  }

  /* now do the real work, i.e. receive DIMSE commmands over the network connection */
  /* which was established and handle these commands correspondingly. In case of */
  /* storscp only C-ECHO-RQ and C-STORE-RQ commands can be processed. */
  cond = processCommands(assoc);

  if (cond == DUL_PEERREQUESTEDRELEASE)
  {
    if (opt_verbose) printf("Association Release\n");
    cond = ASC_acknowledgeRelease(assoc);
  }
  else if (cond == DUL_PEERABORTEDASSOCIATION)
  {
    if (opt_verbose) printf("Association Aborted\n");
  }
  else
  {
    fprintf(stderr, "storescp: DIMSE Failure (aborting association)\n");
    /* some kind of error so abort the association */
    cond = ASC_abortAssociation(assoc);
  }

cleanup:
  cond = ASC_dropSCPAssociation(assoc);
  if (cond.bad())
  {
    DimseCondition::dump(cond);
    exit(1);
  }
  cond = ASC_destroyAssociation(&assoc);
  if (cond.bad())
  {
    DimseCondition::dump(cond);
    exit(1);
  }

  return cond;
}



static OFCondition
processCommands(T_ASC_Association * assoc)
    /*
     * This function receives DIMSE commmands over the network connection
     * and handles these commands correspondingly. Note that in case of
     * storscp only C-ECHO-RQ and C-STORE-RQ commands can be processed.
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
      if( opt_endOfStudyTimeout != -1 && ! lastStudyInstanceUID.empty() )
      {
        // indicate that the end-of-study-event occured through a timeout event.
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
      printf("Extra Status Detail: \n");
      statusDetail->print(COUT);
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
          // we cannot handle this kind of message
          cond = DIMSE_BADCOMMANDTYPE;
          fprintf(stderr, "storescp: Cannot handle command: 0x%x\n", OFstatic_cast(unsigned, msg.CommandField));
          break;
      }
    }
  }
  return cond;
}



static OFCondition echoSCP( T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID)
{
  if (opt_verbose)
  {
    printf("Received ");
    DIMSE_printCEchoRQ(stdout, &msg->msg.CEchoRQ);
  }

  /* the echo succeeded !! */
  OFCondition cond = DIMSE_sendEchoResponse(assoc, presID, &msg->msg.CEchoRQ, STATUS_Success, NULL);
  if (cond.bad())
  {
    fprintf(stderr, "storescp: Echo SCP Failed:\n");
    DimseCondition::dump(cond);
  }
  return cond;
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
    if (opt_verbose)
      printf("ABORT initiated (due to command line options)\n");
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
  if (opt_verbose)
  {
    switch (progress->state)
    {
      case DIMSE_StoreBegin:
        printf("RECV:");
        break;
      case DIMSE_StoreEnd:
        printf("\n");
        break;
      default:
        putchar('.');
        break;
    }
    fflush(stdout);
  }

  // if this is the final call of this function, save the data which was received to a file
  // (note that we could also save the image somewhere else, put it in database, etc.)
  if (progress->state == DIMSE_StoreEnd)
  {
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
    if ((imageDataSet)&&(*imageDataSet)&&(!opt_bitPreserving)&&(!opt_ignore))
    {
      OFString fileName;

      // in case option --sort-conc-studies is set, we need to perform some particular
      // steps to determine the actual name of the output file
      if( opt_sortConcerningStudies != NULL )
      {
        // determine the study instance UID in the (current) DICOM object that has just been received
        // note that if findAndGetString says the resulting study instance UID equals NULL, the study
        // instance UID in the (current) DICOM object is an empty string; in general: no matter what
        // happened, we want to create a new string that will contain a corresponding value for the
        // current study instance UID
        const char *tmpstr1 = NULL;
        OFString currentStudyInstanceUID;
        DcmTagKey studyInstanceUIDTagKey( DCM_StudyInstanceUID );
        OFCondition ec = (*imageDataSet)->findAndGetString( studyInstanceUIDTagKey, tmpstr1, OFFalse );
        if( ec != EC_Normal )
        {
          fprintf(stderr, "storescp: No study instance UID found in data set.\n");
          rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
          return;
        }
        if (tmpstr1) currentStudyInstanceUID = tmpstr1;

        // if this is the first DICOM object that was received or if the study instance UID in the
        // current DICOM object does not equal the last object's study instance UID we need to create
        // a new subdirectory in which the current DICOM object will be stored
        if( lastStudyInstanceUID.empty() || lastStudyInstanceUID != currentStudyInstanceUID)
        {
          // if lastStudyInstanceUID is non-empty, we have just completed receiving all objects for one
          // study. In such a case, we need to set a certain indicator variable (lastStudySubdirectoryPathAndName),
          // so that we know that executeOnEndOfStudy() might have to be executed later. In detail, this indicator
          // variable will contain the path and name of the last study's subdirectory, so that we can still remember
          // this directory, when we execute executeOnEndOfStudy(). The memory that is allocated for this variable
          // here will be freed after the execution of executeOnEndOfStudy(). 
          if( ! lastStudyInstanceUID.empty() )
          {
            lastStudySubdirectoryPathAndName = subdirectoryPathAndName;
          }

          // create the new lastStudyInstanceUID value according to the value in the current DICOM object
          lastStudyInstanceUID = currentStudyInstanceUID;

          // get the current time (needed for subdirectory name)
          OFDateTime dateTime;
          dateTime.setCurrentDateTime();
          // create a name for the new subdirectory. pattern: "[opt_sortConcerningStudies]_[YYYYMMDD]_[HHMMSSMMM]" (use current datetime)
          char buf[32];
          sprintf(buf, "_%04u%02u%02u_%02u%02u%02u%03u", 
            dateTime.getDate().getYear(), dateTime.getDate().getMonth(), dateTime.getDate().getDay(),
            dateTime.getTime().getHour(), dateTime.getTime().getMinute(), dateTime.getTime().getIntSecond(), dateTime.getTime().getMilliSecond());          
          OFString subdirectoryName = opt_sortConcerningStudies;
          subdirectoryName += buf;

          // create subdirectoryPathAndName (string with full path to new subdirectory)
          subdirectoryPathAndName = cbdata->imageFileName;
          size_t position = subdirectoryPathAndName.rfind(PATH_SEPARATOR);
          if (position != OFString_npos) subdirectoryPathAndName.erase(position+1);
          subdirectoryPathAndName += subdirectoryName;

          // check if the subdirectory is already existent
          // if it is already existent dump a warning
          if( OFStandard::dirExists(subdirectoryPathAndName) )
          {
            fprintf(stderr, "storescp: Warning: Subdirectory for studies already existent. (%s)\n", subdirectoryPathAndName.c_str() );
          }

          // if it is not existent create it
#ifdef HAVE_WINDOWS_H
          if( _mkdir( subdirectoryPathAndName.c_str() ) == -1 )
#else
          if( mkdir( subdirectoryPathAndName.c_str(), S_IRWXU | S_IRWXG | S_IRWXO ) == -1 )
#endif
          {
            fprintf(stderr, "storescp: Could not create subdirectory %s.\n", subdirectoryPathAndName.c_str() );
            rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
            return;
          }
        }

        // integrate subdirectory name into file name (note that cbdata->imageFileName currently contains both
        // path and file name; however, the path refers to the output directory captured in opt_outputDirectory)
        char *tmpstr5 = strrchr( cbdata->imageFileName, PATH_SEPARATOR );
        fileName = subdirectoryPathAndName;
        fileName += tmpstr5;

        // update global variable outputFileNameArray 
        // (might be used in executeOnReception() and renameOnEndOfStudy)
        outputFileNameArray.push_back(++tmpstr5);
      }
      // if option --sort-conc-studies is not set, the determination of the output file name is simple
      else
      {
        fileName = cbdata->imageFileName;

        // update global variables outputFileNameArray 
        // (might be used in executeOnReception() and renameOnEndOfStudy)
        const char *tmpstr6 = strrchr( fileName.c_str(), PATH_SEPARATOR );
        outputFileNameArray.push_back(++tmpstr6);
      }

      // determine the transfer syntax which shall be used to write the information to the file
      E_TransferSyntax xfer = opt_writeTransferSyntax;
      if (xfer == EXS_Unknown) xfer = (*imageDataSet)->getOriginalXfer();

      // store file either with meta header or as pure dataset
      OFCondition cond = cbdata->dcmff->saveFile(fileName.c_str(), xfer, opt_sequenceType, opt_groupLength,
          opt_paddingType, OFstatic_cast(Uint32, opt_filepad),
          OFstatic_cast(Uint32, opt_itempad), !opt_useMetaheader);
      if (cond.bad())
      {
        fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName.c_str());
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
      }

      // check the image to make sure it is consistent, i.e. that its sopClass and sopInstance correspond
      // to those mentioned in the request. If not, set the status in the response message variable.
      if ((rsp->DimseStatus == STATUS_Success)&&(!opt_ignore))
      {
        // which SOP class and SOP instance ?
        if (! DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance, opt_correctUIDPadding))
        {
           fprintf(stderr, "storescp: Bad image file: %s\n", fileName.c_str());
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
      char *tmpstr7 = strrchr( cbdata->imageFileName, PATH_SEPARATOR );
      outputFileNameArray.push_back(++tmpstr7);
    }
  }

  // return
  return;
}



static OFCondition storeSCP(
  T_ASC_Association *assoc,
  T_DIMSE_Message *msg,
  T_ASC_PresentationContextID presID)
    /*
     * This function processes a DIMSE C-STORE-RQ commmand that was
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
    if (opt_uniqueFilenames)
    {
      // create unique filename by generating a temporary UID and using ".X." as an infix
      char buf[70];
      dcmGenerateUniqueIdentifier(buf);
      sprintf(imageFileName, "%s%c%s.X.%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, dcmSOPClassUIDToModality(req->AffectedSOPClassUID), buf);
    }
    else
    {
      sprintf(imageFileName, "%s%c%s.%s", opt_outputDirectory.c_str(), PATH_SEPARATOR, dcmSOPClassUIDToModality(req->AffectedSOPClassUID), req->AffectedSOPInstanceUID);
    }
  }

  // dump some information if required
  if (opt_verbose)
  {
    printf("Received ");
    DIMSE_printCStoreRQ(stdout, req);
  }

  // intialize some variables
  StoreCallbackData callbackData;
  callbackData.assoc = assoc;
  callbackData.imageFileName = imageFileName;
  DcmFileFormat dcmff;
  callbackData.dcmff = &dcmff;

  // define an address where the information which will be received over the network will be stored
  DcmDataset *dset = dcmff.getDataset();

  // if opt_bitPreserving is set, the user requires that the data shall be
  // written exactly as it was received. Depending on this option, function
  // DIMSE_storeProvider must be called with certain parameters.
  if (opt_bitPreserving)
  {
      cond = DIMSE_storeProvider(assoc, presID, req, imageFileName, opt_useMetaheader, NULL,
          storeSCPCallback, &callbackData, DIMSE_BLOCKING, 0);
  }
  else
  {
    cond = DIMSE_storeProvider(assoc, presID, req, NULL, opt_useMetaheader, &dset,
        storeSCPCallback, &callbackData, DIMSE_BLOCKING, 0);
  }

  // if some error occured, dump corresponding information and remove the outfile if necessary
  if (cond.bad())
  {
    fprintf(stderr, "storescp: Store SCP Failed:\n");
    DimseCondition::dump(cond);
    // remove file
    if (!opt_ignore)
    {
      if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) unlink(imageFileName);
    }
  }
#ifdef _WIN32
  else if (opt_ignore)
  {
    if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) unlink(imageFileName); // delete the temporary file
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
     * events that need to take place are sepcified by the user through certain
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
  if( opt_renameOnEndOfStudy && ! lastStudySubdirectoryPathAndName.empty() )
    renameOnEndOfStudy();

  // if option --exec-on-eostudy is set and variable lastStudySubdirectoryPathAndName does
  // not equal NULL (i.e. we received all objects that belong to one study, or - in other
  // words - it is the end of one study) we want to execute a certain command which was
  // passed to the application
  if( opt_execOnEndOfStudy != NULL && ! lastStudySubdirectoryPathAndName.empty() )
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
    // perform substitution for placeholder #p; note that
    //  - in case option --sort-conc-studies is set, #p will be substituted by subdirectoryPathAndName
    //  - and in case option --sort-conc-studies is not set, #p will be substituted by opt_outputDirectory
    OFString dir = (opt_sortConcerningStudies == NULL) ? OFString(opt_outputDirectory) : subdirectoryPathAndName;
    cmd = replaceChars( cmd, OFString(PATH_PLACEHOLDER), dir );

    // perform substitution for placeholder #f; note that outputFileNameArray.back()
    // always contains the name of the file (without path) which was written last.
    OFString outputFileName = outputFileNameArray.back();
    cmd = replaceChars( cmd, OFString(FILENAME_PLACEHOLDER), outputFileName );
  }

  // perform substitution for placeholder #a
  cmd = replaceChars( cmd, OFString(CALLING_AETITLE_PLACEHOLDER), callingaetitle );

  // perform substitution for placeholder #c
  cmd = replaceChars( cmd, OFString(CALLED_AETITLE_PLACEHOLDER), calledaetitle );

  // Execute command in a new process
  executeCommand( cmd );
}



static void renameOnEndOfStudy()
    /*
     * This function deals with renaming the last study's output files. In detail, these file's
     * current filenames will be changed to a filename that corresponds to the pattern [modality-
     * prefix][consecutive-numbering]. The current filenames of all files that belong to the study
     * are captured in outputFileNameArray. The new filenames will be calculated whithin this
     * function: The [modality-prefix] will be taken from the old filename (first two characters),
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
  // foolowing loop - not supposed to be deleted from the array. If endOfStudyThroughTimeoutEvent is true,
  // all filenames that are captured in the array, refer to files that belong to the same study. Hence,
  // all of these files shall be renamed and all of the filenames within the array shall be deleted.
  if( ! endOfStudyThroughTimeoutEvent ) --last;

  // rename all files that belong to the last study
  while (first != last)
  {
    // determine the new file name: The first two characters of the old file name make up the [modality-prefix].
    // The value for [consecutive-numbering] will be determined using the counter variable.
    char modalityId[3];
    char newFileName[9];
    OFStandard::strlcpy( modalityId, (*first).c_str(), 3 );
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
      fprintf( stderr, "storescp: Cannot rename file '%s' to '%s'.\n", oldPathAndFileName.c_str(), newPathAndFileName.c_str() );

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
  cmd = replaceChars( cmd, OFString(CALLING_AETITLE_PLACEHOLDER), callingaetitle );

  // perform substitution for placeholder #c
  cmd = replaceChars( cmd, OFString(CALLED_AETITLE_PLACEHOLDER), calledaetitle );

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
     * This function executes the given command line. Note that the execution will be
     * performed in a new process so that it does not slow down the execution of storescp.
     *
     * Parameters:
     *   cmd - [in] The command which shall be executed.
     */
{
#ifdef HAVE_FORK
  pid_t pid = fork();
  if( pid < 0 )     // in case fork failed, dump an error message
    fprintf( stderr, "storescp: Cannot execute command '%s' (fork failed).\n", cmd.c_str() );
  else if (pid > 0)
  {
    /* we are the parent process */
    /* remove pending zombie child processes */
    cleanChildren();
  }
  else // in case we are the child process, execute the command etc.
  {
    // execute command through execl will terminate the child process.
    // Since we only have a single command string and not a list of arguments,
    // we 'emulate' a call to system() by passing the command to /bin/sh
    // which hopefully exists on all Posix systems.

    if (execl( "/bin/sh", "/bin/sh", "-c", cmd.c_str(), NULL ) < 0)
      fprintf( stderr, "storescp: Cannot execute /bin/sh.\n" );

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
    fprintf( stderr, "storescp: Error while executing command '%s'.\n" , cmd.c_str() );

#endif
}

static void cleanChildren()
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
    int options = WNOHANG;
    while (child > 0)
    {
#ifdef HAVE_WAITPID
        child = OFstatic_cast(int, waitpid(-1, &stat_loc, options));
#elif defined(HAVE_WAIT3)
        child = wait3(&status, options, &rusage);
#endif
        if (child < 0)
        {
           if (errno != ECHILD) CERR << "wait for child failed: " << strerror(errno) << endl;
        }
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

/** accept all presenstation contexts for unknown SOP classes,
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
            dpc = findPresentationContextID(
                              params->DULparams.acceptedPresentationContext,
                                            pc.presentationContextID);
            if ((dpc == NULL) ||
                ((dpc != NULL) && (dpc->result != ASC_P_ACCEPTANCE)))
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
                if ((dpc != NULL) &&
                    (dpc->result == ASC_P_TRANSFERSYNTAXESNOTSUPPORTED))
                    reason = ASC_P_TRANSFERSYNTAXESNOTSUPPORTED;

                cond = ASC_refusePresentationContext(params,
                                              pc.presentationContextID,
                                              reason);
                if (cond.bad()) return cond;
            }
        }
    }
    return EC_Normal;
}


/** accept all presenstation contexts for unknown SOP classes,
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
    for (int i=transferSyntaxCount-1; i>=0; i--)
    {
        cond = acceptUnknownContextsWithTransferSyntax(params, transferSyntaxes[i], acceptedRole);
        if (cond.bad()) return cond;
    }
    return cond;
}


/*
** CVS Log
** $Log: storescp.cc,v $
** Revision 1.75  2005-02-22 09:40:54  meichel
** Fixed two bugs in "bit-preserving" Store SCP code. Errors while creating or
**   writing the DICOM file (e.g. file system full) now result in a DIMSE error
**   response (out of resources) being sent back to the SCU.
**
** Revision 1.74  2004/08/03 16:46:00  meichel
** Minor changes for platforms on which strchr/strrchr return a const pointer.
**
** Revision 1.73  2004/04/07 16:58:55  meichel
** Added OFconst_cast, required on Win32
**
** Revision 1.72  2004/04/07 10:01:16  meichel
** Removed call to system() and execl to /bin/false which does not exist
**   on some BSD platforms.
**
** Revision 1.71  2004/04/07 09:42:34  meichel
** Updated sorting and command execution code in storescp to use OFString
**   and OFList. This will hopefully fix the remaining memory leaks.
**
** Revision 1.70  2004/04/06 18:11:24  joergr
** Added missing suffix "TransferSyntax" to some transfer syntax constants.
**
** Revision 1.69  2004/02/25 12:18:06  meichel
** Added a few dummy macros allowing for future private extensions
**
** Revision 1.68  2004/02/13 14:17:39  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.67  2004/02/12 14:05:17  wilkens
** Corrected bug in storescp that options "-xcr" and "+B" (or "--ignore") could
** not be used together. Now they can.
**
** Revision 1.66  2003/08/14 10:58:47  meichel
** Added check if association configuration profile is valid for SCP use
**
** Revision 1.65  2003/08/11 18:31:15  joergr
** Included "ctype" header file required for gcc 3.2.3 on Debian Linux.
**
** Revision 1.64  2003/06/11 15:46:24  meichel
** Added support for configuration file based association negotiation
**   profiles
**
** Revision 1.63  2003/06/10 14:17:35  meichel
** Added option to create unique filenames, even if receiving the same
**   SOP instance multiple times. Exec options now allow to pass the calling
**   and called aetitle on the command line.
**
** Revision 1.62  2003/06/10 14:05:57  meichel
** Added support for TCP wrappers based host access control
**
** Revision 1.61  2003/06/06 09:44:40  meichel
** Added static sleep function in class OFStandard. This replaces the various
**   calls to sleep(), Sleep() and usleep() throughout the toolkit.
**
** Revision 1.60  2002/11/27 13:04:31  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.59  2002/11/26 08:43:21  meichel
** Replaced all includes for "zlib.h" with <zlib.h>
**   to avoid inclusion of zlib.h in the makefile dependencies.
**
** Revision 1.58  2002/11/25 18:00:19  meichel
** Converted compile time option to leniently handle space padded UIDs
**   in the Storage Service Class into command line / config file option.
**
** Revision 1.57  2002/09/23 17:53:47  joergr
** Added new command line option "--version" which prints the name and version
** number of external libraries used (incl. preparation for future support of
** 'config.guess' host identifiers).
**
** Revision 1.56  2002/09/10 16:04:33  meichel
** Added experimental "promiscuous" mode to storescp. In this mode,
**   activated by the --promiscuous command line option, all presentation
**   contexts not known not to be Storage SOP Classes are accepted.
**
** Revision 1.55  2002/09/02 15:35:57  meichel
** Added --prefer-deflated, --write-xfer-deflated and --compression-level
**   options to storescp
**
** Revision 1.54  2002/08/21 10:18:27  meichel
** Adapted code to new loadFile and saveFile methods, thus removing direct
**   use of the DICOM stream classes.
**
** Revision 1.53  2002/08/02 10:59:56  meichel
** Execute options in storescp now clean up zombie child processes
**   as they should.
**
** Revision 1.52  2002/04/19 10:46:11  joergr
** Added new helper routines to get the milli and micro seconds part as well as
** the integral value of seconds.
**
** Revision 1.51  2002/04/11 12:47:33  joergr
** Replaced direct call of system routines by new standard date and time
** functions.
** Use the new standard file system routines like fileExists() etc.
**
** Revision 1.50  2001/12/19 09:59:43  meichel
** Added prototype declaration for gettimeofday() for systems like Ultrix
**   where the function is known but no prototype present in the system headers.
**
** Revision 1.49  2001/12/14 12:18:06  wilkens
** Fixed a bug in storescp that prevented the application from working correctly
** under Unix.
**
** Revision 1.48  2001/12/14 09:51:51  joergr
** Modified use of time routines to keep gcc on Mac OS X happy.
**
** Revision 1.47  2001/12/11 15:12:01  joergr
** Fixed warning reported by cygwin gcc compiler.
**
** Revision 1.46  2001/12/11 14:11:49  joergr
** Replaced occurrences of strncpy by more secure strlcpy (see ofstd.h).
** Added type cast to keep old Sun compilers quiet.
** Modified description of command line option -tos.
**
** Revision 1.45  2001/12/06 14:11:11  joergr
** Made description and layout of command line options more consistent.
**
** Revision 1.44  2001/11/30 10:01:34  wilkens
** Changed description of command line options.
**
** Revision 1.43  2001/11/28 14:23:33  wilkens
** Fixed a problem with storescp. Execution of batch-files through new options
** --exec-on-reception and --exec-on-eostudy is now possible also for optimized
** executable.
**
** Revision 1.41  2001/11/09 15:56:25  joergr
** Renamed some of the getValue/getParam methods to avoid ambiguities reported
** by certain compilers.
**
** Revision 1.40  2001/11/01 14:39:01  wilkens
** Added lots of comments.
**
** Revision 1.39  2001/10/12 10:18:21  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.38  2001/09/28 13:21:41  joergr
** Replaced "cerr" by "CERR".
**
** Revision 1.37  2001/06/01 15:50:02  meichel
** Updated copyright header
**
** Revision 1.36  2001/06/01 11:01:57  meichel
** Implemented global flag and command line option to disable reverse
**   DNS hostname lookup using gethostbyaddr when accepting associations.
**
** Revision 1.35  2000/12/15 13:28:14  meichel
** Global flag to enable/disable workaround code for some buggy Store SCUs
**   in DIMSE_storeProvider().  If enabled, an illegal space-padding in the
**   Affected SOP Instance UID field of the C-STORE-RQ message is retained
**   in the corresponding C-STORE-RSP message.
**
** Revision 1.34  2000/11/10 18:07:43  meichel
** Mixed up strcmp and strcpy - oops.
**
** Revision 1.33  2000/11/10 16:25:04  meichel
** Fixed problem with DIMSE routines which attempted to delete /dev/null
**   under certain circumstances, which could lead to disastrous results if
**   tools were run with root permissions (what they shouldn't).
**
** Revision 1.32  2000/08/10 14:50:49  meichel
** Added initial OpenSSL support.
**
** Revision 1.31  2000/06/07 13:56:18  meichel
** Output stream now passed as mandatory parameter to ASC_dumpParameters.
**
** Revision 1.30  2000/04/14 16:29:27  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.29  2000/03/08 16:43:16  meichel
** Updated copyright header.
**
** Revision 1.28  2000/02/29 11:49:50  meichel
** Removed support for VS value representation. This was proposed in CP 101
**   but never became part of the standard.
**
** Revision 1.27  2000/02/23 15:12:21  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.26  2000/02/03 11:50:09  meichel
** Moved UID related functions from dcmnet (diutil.h) to dcmdata (dcuid.h)
**   where they belong. Renamed access functions to dcmSOPClassUIDToModality
**   and dcmGuessModalityBytes.
**
** Revision 1.25  2000/02/01 10:24:03  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.24  1999/10/07 17:26:19  meichel
** Corrected typo in storescup error message
**
** Revision 1.23  1999/04/29 10:02:54  meichel
** Adapted findscu to new command line option scheme
**
** Revision 1.22  1999/04/28 08:29:07  meichel
** Adapted storescp to new command line option scheme.
**   Added support for transmission of compressed images.
**
** Revision 1.21  1999/04/27 17:24:40  meichel
** Adapted storescp to new command line option scheme.
**   Added support for transmission of compressed images.
**
** Revision 1.20  1999/03/29 11:19:55  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.19  1998/08/10 08:53:36  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.18  1998/02/06 15:07:30  meichel
** Removed many minor problems (name clashes, unreached code)
**   reported by Sun CC4 with "+w" or Sun CC2.
**
** Revision 1.17  1998/01/14 14:35:55  hewett
** Modified existing -u command line option to also disable generation
** of UT and VS (previously just disabled generation of UN).
**
** Revision 1.16  1997/09/12 13:21:50  meichel
** Command line option '-h' in storescp now works correctly.
**
** Revision 1.15  1997/08/05 07:36:21  andreas
** - Corrected error in DUL finite state machine
**   SCPs shall close sockets after the SCU have closed the socket in
**   a normal association release. Therfore, an ARTIM timer is described
**   in DICOM part 8 that is not implemented correctly in the
**   DUL. Since the whole DUL finite state machine is affected, we
**   decided to solve the proble outside the fsm. Now it is necessary to call the
**   ASC_DropSCPAssociation() after the calling ASC_acknowledgeRelease().
** - Change needed version number of WINSOCK to 1.1
**   to support WINDOWS 95
**
** Revision 1.14  1997/07/24 13:10:55  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.13  1997/07/21 08:37:04  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.12  1997/05/29 15:52:57  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.11  1997/05/27 15:44:53  meichel
** Corrected typo in storescp help texts.
**
** Revision 1.10  1997/05/23 10:44:20  meichel
** Major rewrite of storescp application. See CHANGES for details.
** Changes to interfaces of some DIMSE functions.
**
** Revision 1.9  1997/05/22 13:29:59  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.8  1997/05/16 08:31:34  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.7  1997/03/27 16:11:27  hewett
** Added command line switches allowing generation of UN to
** be disabled (it is enabled by default).
**
** Revision 1.6  1996/12/16 15:14:00  hewett
** Added bugfix for WINSOCK support.  The required WINSOCK version
** number was being incorrectly set to version 0.1.  The fixed
** WINSOCK initialisation now uses the MAKEWORD macro to correctly
** set the required version number. This bugfix was contributed
** by Dr. Yongjian Bao of Innomed GmbH, Germany.
**
** Revision 1.5  1996/09/27 14:05:05  hewett
** Added calls to initialise WINSOCK library for Win32 environment.  Only
** compiled in if HAVE_WINSOCK_H
**
** Revision 1.4  1996/09/27 08:24:30  hewett
** System header files now enclosed with BEGIN_EXTERN_C/END_EXTERN_C
**
** Revision 1.3  1996/09/24 16:20:32  hewett
** Now uses global table of Storage SOP Class UIDs (from dcuid.h).
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.2  1996/09/03 11:39:03  hewett
** Added copyright information.
**
** Revision 1.1.1.1  1996/03/26 18:38:44  hewett
** Initial Release.
**
**
*/
