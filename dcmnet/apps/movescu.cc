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
 *  Purpose: Query/Retrieve Service Class User (C-MOVE operation)
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dcostrmz.h"   /* for dcmZlibCompressionLevel */

#ifdef WITH_ZLIB
#include <zlib.h>     /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "movescu"

static OFLogger movescuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default application titles */
#define APPLICATIONTITLE        "MOVESCU"
#define PEERAPPLICATIONTITLE    "ANY-SCP"

typedef enum {
     QMPatientRoot = 0,
     QMStudyRoot = 1,
     QMPatientStudyOnly = 2
} QueryModel;

typedef struct {
    const char *findSyntax;
    const char *moveSyntax;
} QuerySyntax;

typedef struct {
    T_ASC_Association *assoc;
    T_ASC_PresentationContextID presId;
} MyCallbackInfo;

OFCmdUnsignedInt  opt_sleepAfter = 0;
OFCmdUnsignedInt  opt_sleepDuring = 0;
OFCmdUnsignedInt  opt_maxPDU = ASC_DEFAULTMAXPDU;
OFBool            opt_useMetaheader = OFTrue;
OFBool            opt_acceptAllXfers = OFFalse;
E_TransferSyntax  opt_in_networkTransferSyntax = EXS_Unknown;
E_TransferSyntax  opt_out_networkTransferSyntax = EXS_Unknown;
E_TransferSyntax  opt_writeTransferSyntax = EXS_Unknown;
E_GrpLenEncoding  opt_groupLength = EGL_recalcGL;
E_EncodingType    opt_sequenceType = EET_ExplicitLength;
E_PaddingEncoding opt_paddingType = EPD_withoutPadding;
OFCmdUnsignedInt  opt_filepad = 0;
OFCmdUnsignedInt  opt_itempad = 0;
OFBool            opt_bitPreserving = OFFalse;
OFBool            opt_ignore = OFFalse;
OFBool            opt_abortDuringStore = OFFalse;
OFBool            opt_abortAfterStore = OFFalse;
OFBool            opt_correctUIDPadding = OFFalse;
OFCmdUnsignedInt  opt_repeatCount = 1;
OFCmdUnsignedInt  opt_retrievePort = 0;
OFBool            opt_abortAssociation = OFFalse;
const char *      opt_moveDestination = NULL;
OFCmdSignedInt    opt_cancelAfterNResponses = -1;
QueryModel        opt_queryModel = QMPatientRoot;
T_DIMSE_BlockingMode opt_blockMode = DIMSE_BLOCKING;
int               opt_dimse_timeout = 0;
int               opt_acse_timeout = 30;
OFBool            opt_ignorePendingDatasets = OFTrue;
OFString          opt_outputDirectory = ".";

#ifdef WITH_ZLIB
OFCmdUnsignedInt  opt_compressionLevel = 0;
#endif

static T_ASC_Network *net = NULL; /* the global DICOM network */
static DcmDataset *overrideKeys = NULL;
static QuerySyntax querySyntax[3] = {
    { UID_FINDPatientRootQueryRetrieveInformationModel,
      UID_MOVEPatientRootQueryRetrieveInformationModel },
    { UID_FINDStudyRootQueryRetrieveInformationModel,
      UID_MOVEStudyRootQueryRetrieveInformationModel },
    { UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel,
      UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel }
};


static void
addOverrideKey(OFConsoleApplication& app, const char *s)
{
    unsigned int g = 0xffff;
    unsigned int e = 0xffff;
    int n = 0;
    OFString dicName, valStr;
    OFString msg;
    char msg2[200];

    // try to parse group and element number
    n = sscanf(s, "%x,%x=", &g, &e);
    OFString toParse = s;
    size_t eqPos = toParse.find('=');
    if (n < 2)  // if at least no tag could be parsed
    {
      // if value is given, extract it (and extrect dictname)
      if (eqPos != OFString_npos)
      {
        dicName = toParse.substr(0,eqPos).c_str();
        valStr = toParse.substr(eqPos+1,toParse.length());
      }
      else // no value given, just dictionary name
        dicName = s; // only dictionary name given (without value)
      // try to lookup in dictionary
      DcmTagKey key(0xffff,0xffff);
      const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
      const DcmDictEntry *dicent = globalDataDict.findEntry(dicName.c_str());
      dcmDataDict.unlock();
      if (dicent!=NULL) {
        // found dictionary name, copy group and element number
        key = dicent->getKey();
        g = key.getGroup();
        e = key.getElement();
      }
      else {
        // not found in dictionary
        msg = "bad key format or dictionary name not found in dictionary: ";
        msg += dicName;
        app.printError(msg.c_str());
      }
    } // tag could be parsed, copy value if it exists
    else
    {
      if (eqPos != OFString_npos)
        valStr = toParse.substr(eqPos+1,toParse.length());
    }
    DcmTag tag(g,e);
    if (tag.error() != EC_Normal) {
        sprintf(msg2, "unknown tag: (%04x,%04x)", g, e);
        app.printError(msg2);
    }
    DcmElement *elem = newDicomElement(tag);
    if (elem == NULL) {
        sprintf(msg2, "cannot create element for tag: (%04x,%04x)", g, e);
        app.printError(msg2);
    }
    if (!valStr.empty()) {
        if (elem->putString(valStr.c_str()).bad())
        {
            sprintf(msg2, "cannot put tag value: (%04x,%04x)=\"", g, e);
            msg = msg2;
            msg += valStr;
            msg += "\"";
            app.printError(msg.c_str());
        }
    }

    if (overrideKeys == NULL) overrideKeys = new DcmDataset;
    if (overrideKeys->insert(elem, OFTrue).bad()) {
        sprintf(msg2, "cannot insert tag: (%04x,%04x)", g, e);
        app.printError(msg2);
    }
}

static OFCondition cmove(T_ASC_Association *assoc, const char *fname);

static OFCondition
addPresentationContext(T_ASC_Parameters *params,
                        T_ASC_PresentationContextID pid,
                        const char *abstractSyntax);

#define SHORTCOL 4
#define LONGCOL 21

int
main(int argc, char *argv[])
{
    T_ASC_Parameters *params = NULL;
    const char *opt_peer;
    OFCmdUnsignedInt opt_port = 104;
    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;
    T_ASC_Association *assoc = NULL;
    const char *opt_peerTitle = PEERAPPLICATIONTITLE;
    const char *opt_ourTitle = APPLICATIONTITLE;
    OFList<OFString> fileNameList;

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
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM retrieve (C-MOVE) SCU", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);
  cmd.addParam("peer", "hostname of DICOM peer");
  cmd.addParam("port", "tcp/ip port number of peer");
  cmd.addParam("dcmfile-in", "DICOM query file(s)", OFCmdParam::PM_MultiOptional);

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
   cmd.addOption("--help",                   "-h",      "print this help text and exit", OFCommandLine::AF_Exclusive);
   cmd.addOption("--version",                           "print version information and exit", OFCommandLine::AF_Exclusive);
   OFLog::addOptions(cmd);

  cmd.addGroup("network options:");
    cmd.addSubGroup("override matching keys:");
      cmd.addOption("--key",                 "-k",   1, "[k]ey: gggg,eeee=\"str\" or dict. name=\"str\"",
                                                        "override matching key");
    cmd.addSubGroup("query information model:");
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
      OFString opt5 = "set move destinat. AE title (default: ";
      opt5 += APPLICATIONTITLE;
      opt5 += ")";
      cmd.addOption("--move",                "-aem", 1, "[a]etitle: string", opt5.c_str());
    cmd.addSubGroup("preferred network transfer syntaxes (incoming associations):");
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
      cmd.addOption("--accept-all",          "+xa",     "accept all supported transfer syntaxes");
    cmd.addSubGroup("proposed transmission transfer syntaxes (outgoing associations):");
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
    cmd.addSubGroup("port for incoming network associations:");
      cmd.addOption("--no-port",                        "no port for incoming associations (default)");
      cmd.addOption("--port",                "+P",   1, "[n]umber: integer",
                                                        "port number for incoming associations");
    cmd.addSubGroup("handling of illegal datasets following 'pending' move responses:");
      cmd.addOption("--pending-ignore",      "-pi",     "assume no dataset present (default)");
      cmd.addOption("--pending-read",        "-pr",     "read and ignore dataset");

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
      cmd.addOption("--ignore",                         "ignore store data, receive but do not store");
      cmd.addOption("--cancel",                      1, "[n]umber: integer",
                                                        "cancel after n responses (default: never)");
      cmd.addOption("--uid-padding",         "-up",     "silently correct space-padded UIDs");
  cmd.addGroup("output options:");
    cmd.addSubGroup("general:");
      cmd.addOption("--output-directory",    "-od",  1, "[d]irectory: string (default: \".\")", "write received objects to existing directory d");
    cmd.addSubGroup("bit preserving mode:");
      cmd.addOption("--normal",              "-B",      "allow implicit format conversions (default)");
      cmd.addOption("--bit-preserving",      "+B",      "write data exactly as read");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",          "+F",      "write file format (default)");
      cmd.addOption("--write-dataset",       "-F",      "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax (not with --bit-preserving or compressed transmission):");
      cmd.addOption("--write-xfer-same",     "+t=",     "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",   "+te",     "write with explicit VR little endian TS");
      cmd.addOption("--write-xfer-big",      "+tb",     "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit", "+ti",     "write with implicit VR little endian TS");
#ifdef WITH_ZLIB
      cmd.addOption("--write-xfer-deflated", "+td",     "write with deflated expl. VR little endian TS");
#endif
    cmd.addSubGroup("post-1993 value representations (not with --bit-preserving):");
      cmd.addOption("--enable-new-vr",       "+u",      "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",      "-u",      "disable support for new VRs, convert to OB");
    cmd.addSubGroup("group length encoding (not with --bit-preserving):");
      cmd.addOption("--group-length-recalc", "+g=",     "recalculate group lengths if present (default)");
      cmd.addOption("--group-length-create", "+g",      "always write with group length elements");
      cmd.addOption("--group-length-remove", "-g",      "always write without group length elements");
    cmd.addSubGroup("length encoding in sequences and items (not with --bit-preserving):");
      cmd.addOption("--length-explicit",     "+e",      "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",    "-e",      "write with undefined lengths");
    cmd.addSubGroup("data set trailing padding (not with --write-dataset or --bit-preserving):");
      cmd.addOption("--padding-off",         "-p",      "no padding (default)");
      cmd.addOption("--padding-create",      "+p",   2, "[f]ile-pad [i]tem-pad: integer",
                                                        "align file on multiple of f bytes\nand items on multiple of i bytes");
#ifdef WITH_ZLIB
    cmd.addSubGroup("deflate compression level (only with -xd or --write-xfer-deflated/same):");
      cmd.addOption("--compression-level",   "+cl",  1, "[l]evel: integer (default: 6)",
                                                        "0=uncompressed, 1=fastest, 9=best compression");
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

      if (cmd.findOption("--key", 0, OFCommandLine::FOM_First))
      {
        const char *ovKey = NULL;
        do {
          app.checkValue(cmd.getValue(ovKey));
          addOverrideKey(app, ovKey);
        } while (cmd.findOption("--key", 0, OFCommandLine::FOM_Next));
      }
      cmd.beginOptionBlock();
      if (cmd.findOption("--patient")) opt_queryModel = QMPatientRoot;
      if (cmd.findOption("--study")) opt_queryModel = QMStudyRoot;
      if (cmd.findOption("--psonly")) opt_queryModel = QMPatientStudyOnly;
      cmd.endOptionBlock();

      if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
      if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));
      if (cmd.findOption("--move")) app.checkValue(cmd.getValue(opt_moveDestination));

      cmd.beginOptionBlock();
      if (cmd.findOption("--prefer-uncompr"))
      {
          opt_acceptAllXfers = OFFalse;
          opt_in_networkTransferSyntax = EXS_Unknown;
      }
      if (cmd.findOption("--prefer-little")) opt_in_networkTransferSyntax = EXS_LittleEndianExplicit;
      if (cmd.findOption("--prefer-big")) opt_in_networkTransferSyntax = EXS_BigEndianExplicit;
      if (cmd.findOption("--prefer-lossless")) opt_in_networkTransferSyntax = EXS_JPEGProcess14SV1;
      if (cmd.findOption("--prefer-jpeg8")) opt_in_networkTransferSyntax = EXS_JPEGProcess1;
      if (cmd.findOption("--prefer-jpeg12")) opt_in_networkTransferSyntax = EXS_JPEGProcess2_4;
      if (cmd.findOption("--prefer-j2k-lossless")) opt_in_networkTransferSyntax = EXS_JPEG2000LosslessOnly;
      if (cmd.findOption("--prefer-j2k-lossy")) opt_in_networkTransferSyntax = EXS_JPEG2000;
      if (cmd.findOption("--prefer-jls-lossless")) opt_in_networkTransferSyntax = EXS_JPEGLSLossless;
      if (cmd.findOption("--prefer-jls-lossy")) opt_in_networkTransferSyntax = EXS_JPEGLSLossy;
      if (cmd.findOption("--prefer-mpeg2")) opt_in_networkTransferSyntax = EXS_MPEG2MainProfileAtMainLevel;
      if (cmd.findOption("--prefer-mpeg2-high")) opt_in_networkTransferSyntax = EXS_MPEG2MainProfileAtHighLevel;
      if (cmd.findOption("--prefer-mpeg4")) opt_in_networkTransferSyntax = EXS_MPEG4HighProfileLevel4_1;
      if (cmd.findOption("--prefer-mpeg4-bd")) opt_in_networkTransferSyntax = EXS_MPEG4BDcompatibleHighProfileLevel4_1;
      if (cmd.findOption("--prefer-rle")) opt_in_networkTransferSyntax = EXS_RLELossless;
#ifdef WITH_ZLIB
      if (cmd.findOption("--prefer-deflated")) opt_in_networkTransferSyntax = EXS_DeflatedLittleEndianExplicit;
#endif
      if (cmd.findOption("--implicit")) opt_in_networkTransferSyntax = EXS_LittleEndianImplicit;
      if (cmd.findOption("--accept-all"))
      {
          opt_acceptAllXfers = OFTrue;
          opt_in_networkTransferSyntax = EXS_Unknown;
      }
      cmd.endOptionBlock();
      if (opt_in_networkTransferSyntax != EXS_Unknown) opt_acceptAllXfers = OFFalse;

      cmd.beginOptionBlock();
      if (cmd.findOption("--propose-uncompr")) opt_out_networkTransferSyntax = EXS_Unknown;
      if (cmd.findOption("--propose-little")) opt_out_networkTransferSyntax = EXS_LittleEndianExplicit;
      if (cmd.findOption("--propose-big")) opt_out_networkTransferSyntax = EXS_BigEndianExplicit;
      if (cmd.findOption("--propose-implicit")) opt_out_networkTransferSyntax = EXS_LittleEndianImplicit;
#ifdef WITH_ZLIB
      if (cmd.findOption("--propose-deflated")) opt_out_networkTransferSyntax = EXS_DeflatedLittleEndianExplicit;
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

      cmd.beginOptionBlock();
      if (cmd.findOption("--port"))    app.checkValue(cmd.getValueAndCheckMinMax(opt_retrievePort, 1, 65535));
      if (cmd.findOption("--no-port")) { /* do nothing */ }
      cmd.endOptionBlock();

      cmd.beginOptionBlock();
      if (cmd.findOption("--pending-ignore")) opt_ignorePendingDatasets = OFTrue;
      if (cmd.findOption("--pending-read")) opt_ignorePendingDatasets = OFFalse;
      cmd.endOptionBlock();

      if (cmd.findOption("--max-pdu")) app.checkValue(cmd.getValueAndCheckMinMax(opt_maxPDU, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
      if (cmd.findOption("--disable-host-lookup")) dcmDisableGethostbyaddr.set(OFTrue);
      if (cmd.findOption("--repeat"))  app.checkValue(cmd.getValueAndCheckMin(opt_repeatCount, 1));
      if (cmd.findOption("--abort"))   opt_abortAssociation = OFTrue;
      if (cmd.findOption("--ignore"))  opt_ignore = OFTrue;
      if (cmd.findOption("--cancel"))  app.checkValue(cmd.getValueAndCheckMin(opt_cancelAfterNResponses, 0));
      if (cmd.findOption("--uid-padding")) opt_correctUIDPadding = OFTrue;

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
        app.checkConflict("--write-xfer-little", "--prefer-lossless", opt_in_networkTransferSyntax == EXS_JPEGProcess14SV1);
        app.checkConflict("--write-xfer-little", "--prefer-jpeg8", opt_in_networkTransferSyntax == EXS_JPEGProcess1);
        app.checkConflict("--write-xfer-little", "--prefer-jpeg12", opt_in_networkTransferSyntax == EXS_JPEGProcess2_4);
        app.checkConflict("--write-xfer-little", "--prefer-j2k-lossless", opt_in_networkTransferSyntax == EXS_JPEG2000LosslessOnly);
        app.checkConflict("--write-xfer-little", "--prefer-j2k-lossy", opt_in_networkTransferSyntax == EXS_JPEG2000);
        app.checkConflict("--write-xfer-little", "--prefer-jls-lossless", opt_in_networkTransferSyntax == EXS_JPEGLSLossless);
        app.checkConflict("--write-xfer-little", "--prefer-jls-lossy", opt_in_networkTransferSyntax == EXS_JPEGLSLossy);
        app.checkConflict("--write-xfer-little", "--prefer-mpeg2", opt_in_networkTransferSyntax == EXS_MPEG2MainProfileAtMainLevel);
        app.checkConflict("--write-xfer-little", "--prefer-mpeg2-high", opt_in_networkTransferSyntax == EXS_MPEG2MainProfileAtHighLevel);
        app.checkConflict("--write-xfer-little", "--prefer-mpeg4", opt_in_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_1);
        app.checkConflict("--write-xfer-little", "--prefer-mpeg4-bd", opt_in_networkTransferSyntax == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
        app.checkConflict("--write-xfer-little", "--prefer-rle", opt_in_networkTransferSyntax == EXS_RLELossless);
        // we don't have to check a conflict for --prefer-deflated because we can always convert that to uncompressed.
        opt_writeTransferSyntax = EXS_LittleEndianExplicit;
      }
      if (cmd.findOption("--write-xfer-big"))
      {
        app.checkConflict("--write-xfer-big", "--accept-all", opt_acceptAllXfers);
        app.checkConflict("--write-xfer-big", "--bit-preserving", opt_bitPreserving);
        app.checkConflict("--write-xfer-big", "--prefer-lossless", opt_in_networkTransferSyntax == EXS_JPEGProcess14SV1);
        app.checkConflict("--write-xfer-big", "--prefer-jpeg8", opt_in_networkTransferSyntax == EXS_JPEGProcess1);
        app.checkConflict("--write-xfer-big", "--prefer-jpeg12", opt_in_networkTransferSyntax == EXS_JPEGProcess2_4);
        app.checkConflict("--write-xfer-big", "--prefer-j2k-lossy", opt_in_networkTransferSyntax == EXS_JPEG2000);
        app.checkConflict("--write-xfer-big", "--prefer-j2k-lossless", opt_in_networkTransferSyntax == EXS_JPEG2000LosslessOnly);
        app.checkConflict("--write-xfer-big", "--prefer-jls-lossless", opt_in_networkTransferSyntax == EXS_JPEGLSLossless);
        app.checkConflict("--write-xfer-big", "--prefer-jls-lossy", opt_in_networkTransferSyntax == EXS_JPEGLSLossy);
        app.checkConflict("--write-xfer-big", "--prefer-mpeg2", opt_in_networkTransferSyntax == EXS_MPEG2MainProfileAtMainLevel);
        app.checkConflict("--write-xfer-big", "--prefer-mpeg2-high", opt_in_networkTransferSyntax == EXS_MPEG2MainProfileAtHighLevel);
        app.checkConflict("--write-xfer-big", "--prefer-mpeg4", opt_in_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_1);
        app.checkConflict("--write-xfer-big", "--prefer-mpeg4-bd", opt_in_networkTransferSyntax == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
        app.checkConflict("--write-xfer-big", "--prefer-rle", opt_in_networkTransferSyntax == EXS_RLELossless);
        // we don't have to check a conflict for --prefer-deflated because we can always convert that to uncompressed.
        opt_writeTransferSyntax = EXS_BigEndianExplicit;
      }
      if (cmd.findOption("--write-xfer-implicit"))
      {
        app.checkConflict("--write-xfer-implicit", "--accept-all", opt_acceptAllXfers);
        app.checkConflict("--write-xfer-implicit", "--bit-preserving", opt_bitPreserving);
        app.checkConflict("--write-xfer-implicit", "--prefer-lossless", opt_in_networkTransferSyntax == EXS_JPEGProcess14SV1);
        app.checkConflict("--write-xfer-implicit", "--prefer-jpeg8", opt_in_networkTransferSyntax == EXS_JPEGProcess1);
        app.checkConflict("--write-xfer-implicit", "--prefer-jpeg12", opt_in_networkTransferSyntax == EXS_JPEGProcess2_4);
        app.checkConflict("--write-xfer-implicit", "--prefer-j2k-lossy", opt_in_networkTransferSyntax == EXS_JPEG2000);
        app.checkConflict("--write-xfer-implicit", "--prefer-j2k-lossless", opt_in_networkTransferSyntax == EXS_JPEG2000LosslessOnly);
        app.checkConflict("--write-xfer-implicit", "--prefer-jls-lossless", opt_in_networkTransferSyntax == EXS_JPEGLSLossless);
        app.checkConflict("--write-xfer-implicit", "--prefer-jls-lossy", opt_in_networkTransferSyntax == EXS_JPEGLSLossy);
        app.checkConflict("--write-xfer-implicit", "--prefer-mpeg2", opt_in_networkTransferSyntax == EXS_MPEG2MainProfileAtMainLevel);
        app.checkConflict("--write-xfer-implicit", "--prefer-mpeg2-high", opt_in_networkTransferSyntax == EXS_MPEG2MainProfileAtHighLevel);
        app.checkConflict("--write-xfer-implicit", "--prefer-mpeg4", opt_in_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_1);
        app.checkConflict("--write-xfer-implicit", "--prefer-mpeg4-bd", opt_in_networkTransferSyntax == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
        app.checkConflict("--write-xfer-implicit", "--prefer-rle", opt_in_networkTransferSyntax == EXS_RLELossless);
        // we don't have to check a conflict for --prefer-deflated because we can always convert that to uncompressed.
        opt_writeTransferSyntax = EXS_LittleEndianImplicit;
      }
#ifdef WITH_ZLIB
      if (cmd.findOption("--write-xfer-deflated"))
      {
        app.checkConflict("--write-xfer-deflated", "--accept-all", opt_acceptAllXfers);
        app.checkConflict("--write-xfer-deflated", "--bit-preserving", opt_bitPreserving);
        app.checkConflict("--write-xfer-deflated", "--prefer-lossless", opt_in_networkTransferSyntax == EXS_JPEGProcess14SV1);
        app.checkConflict("--write-xfer-deflated", "--prefer-jpeg8", opt_in_networkTransferSyntax == EXS_JPEGProcess1);
        app.checkConflict("--write-xfer-deflated", "--prefer-jpeg12", opt_in_networkTransferSyntax == EXS_JPEGProcess2_4);
        app.checkConflict("--write-xfer-deflated", "--prefer-j2k-lossless", opt_in_networkTransferSyntax == EXS_JPEG2000LosslessOnly);
        app.checkConflict("--write-xfer-deflated", "--prefer-j2k-lossy", opt_in_networkTransferSyntax == EXS_JPEG2000);
        app.checkConflict("--write-xfer-deflated", "--prefer-jls-lossless", opt_in_networkTransferSyntax == EXS_JPEGLSLossless);
        app.checkConflict("--write-xfer-deflated", "--prefer-jls-lossy", opt_in_networkTransferSyntax == EXS_JPEGLSLossy);
        app.checkConflict("--write-xfer-deflated", "--prefer-mpeg2", opt_in_networkTransferSyntax == EXS_MPEG2MainProfileAtMainLevel);
        app.checkConflict("--write-xfer-deflated", "--prefer-mpeg2-high", opt_in_networkTransferSyntax == EXS_MPEG2MainProfileAtHighLevel);
        app.checkConflict("--write-xfer-deflated", "--prefer-mpeg4", opt_in_networkTransferSyntax == EXS_MPEG4HighProfileLevel4_1);
        app.checkConflict("--write-xfer-deflated", "--prefer-mpeg4-bd", opt_in_networkTransferSyntax == EXS_MPEG4BDcompatibleHighProfileLevel4_1);
        app.checkConflict("--write-xfer-deflated", "--prefer-rle", opt_in_networkTransferSyntax == EXS_RLELossless);
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
        app.checkDependence("--compression-level", "--propose-deflated, --write-xfer-deflated or --write-xfer-same",
          (opt_out_networkTransferSyntax == EXS_DeflatedLittleEndianExplicit) ||
          (opt_writeTransferSyntax == EXS_DeflatedLittleEndianExplicit) ||
          (opt_writeTransferSyntax == EXS_Unknown));
        app.checkValue(cmd.getValueAndCheckMinMax(opt_compressionLevel, 0, 9));
        dcmZlibCompressionLevel.set(OFstatic_cast(int, opt_compressionLevel));
      }
#endif

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

      if ((fileNameList.empty()) && (overrideKeys == NULL))
      {
        app.printError("either query file or override keys (or both) must be specified");
      }
    }

    /* print resource identifier */
    OFLOG_DEBUG(movescuLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(movescuLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* make sure output directory exists and is writeable */
    if (opt_retrievePort > 0)
    {
        if (!OFStandard::dirExists(opt_outputDirectory))
        {
          OFLOG_FATAL(movescuLogger, "specified output directory does not exist");
          return 1;
        }
        else if (!OFStandard::isWriteable(opt_outputDirectory))
        {
          OFLOG_FATAL(movescuLogger, "specified output directory is not writeable");
          return 1;
        }
    }

#ifndef DISABLE_PORT_PERMISSION_CHECK
#ifdef HAVE_GETEUID
    /* if retrieve port is privileged we must be as well */
    if ((opt_retrievePort > 0) && (opt_retrievePort < 1024)) {
        if (geteuid() != 0)
        {
          OFLOG_FATAL(movescuLogger, "cannot listen on port " << opt_retrievePort << ", insufficient privileges");
          return 1;
        }
    }
#endif
#endif

    /* network for move request and responses */
    T_ASC_NetworkRole role = (opt_retrievePort > 0) ? NET_ACCEPTORREQUESTOR : NET_REQUESTOR;
    OFCondition cond = ASC_initializeNetwork(role, OFstatic_cast(int, opt_retrievePort), opt_acse_timeout, &net);
    if (cond.bad())
    {
        OFLOG_FATAL(movescuLogger, "cannot create network: " << DimseCondition::dump(temp_str, cond));
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

    /* set up main association */
    cond = ASC_createAssociationParameters(&params, opt_maxPDU);
    if (cond.bad()) {
        OFLOG_FATAL(movescuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }
    ASC_setAPTitles(params, opt_ourTitle, opt_peerTitle, NULL);

    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", opt_peer, OFstatic_cast(int, opt_port));
    ASC_setPresentationAddresses(params, localHost, peerHost);

    /*
     * We also add a presentation context for the corresponding
     * find sop class.
     */
    cond = addPresentationContext(params, 1,
        querySyntax[opt_queryModel].findSyntax);

    cond = addPresentationContext(params, 3,
        querySyntax[opt_queryModel].moveSyntax);
    if (cond.bad()) {
        OFLOG_FATAL(movescuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }

    OFLOG_DEBUG(movescuLogger, "Request Parameters:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_RQ));

    /* create association */
    OFLOG_INFO(movescuLogger, "Requesting Association");
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED) {
            T_ASC_RejectParameters rej;

            ASC_getRejectParameters(params, &rej);
            OFLOG_FATAL(movescuLogger, "Association Rejected:");
            OFLOG_FATAL(movescuLogger, ASC_printRejectParameters(temp_str, &rej));
            exit(1);
        } else {
            OFLOG_FATAL(movescuLogger, "Association Request Failed:");
            OFLOG_FATAL(movescuLogger, DimseCondition::dump(temp_str, cond));
            exit(1);
        }
    }
    /* what has been accepted/refused ? */
    OFLOG_DEBUG(movescuLogger, "Association Parameters Negotiated:" << OFendl << ASC_dumpParameters(temp_str, params, ASC_ASSOC_AC));

    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        OFLOG_FATAL(movescuLogger, "No Acceptable Presentation Contexts");
        exit(1);
    }

    OFLOG_INFO(movescuLogger, "Association Accepted (Max Send PDV: " << assoc->sendPDVLength << ")");

    /* do the real work */
    cond = EC_Normal;
    if (fileNameList.empty())
    {
      /* no files provided on command line */
      cond = cmove(assoc, NULL);
    } else {
      OFListIterator(OFString) iter = fileNameList.begin();
      OFListIterator(OFString) enditer = fileNameList.end();
      while ((iter != enditer) && cond.good())
      {
          cond = cmove(assoc, (*iter).c_str());
          ++iter;
      }
    }

    /* tear down association */
    if (cond == EC_Normal)
    {
        if (opt_abortAssociation) {
            OFLOG_INFO(movescuLogger, "Aborting Association");
            cond = ASC_abortAssociation(assoc);
            if (cond.bad()) {
                OFLOG_FATAL(movescuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
                exit(1);
            }
        } else {
            /* release association */
            OFLOG_INFO(movescuLogger, "Releasing Association");
            cond = ASC_releaseAssociation(assoc);
            if (cond.bad())
            {
                OFLOG_FATAL(movescuLogger, "Association Release Failed:");
                OFLOG_FATAL(movescuLogger, DimseCondition::dump(temp_str, cond));
                exit(1);
            }
        }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        OFLOG_ERROR(movescuLogger, "Protocol Error: Peer requested release (Aborting)");
        OFLOG_INFO(movescuLogger, "Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            OFLOG_FATAL(movescuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
            exit(1);
        }
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
        OFLOG_INFO(movescuLogger, "Peer Aborted Association");
    }
    else
    {
        OFLOG_ERROR(movescuLogger, "Move SCU Failed: " << DimseCondition::dump(temp_str, cond));
        OFLOG_INFO(movescuLogger, "Aborting Association");
        cond = ASC_abortAssociation(assoc);
        if (cond.bad()) {
            OFLOG_FATAL(movescuLogger, "Association Abort Failed: " << DimseCondition::dump(temp_str, cond));
            exit(1);
        }
    }

    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad()) {
        OFLOG_FATAL(movescuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }
    cond = ASC_dropNetwork(&net);
    if (cond.bad()) {
        OFLOG_FATAL(movescuLogger, DimseCondition::dump(temp_str, cond));
        exit(1);
    }

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

    return 0;
}


static OFCondition
addPresentationContext(T_ASC_Parameters *params,
                        T_ASC_PresentationContextID pid,
                        const char *abstractSyntax)
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

    const char *transferSyntaxes[] = { NULL, NULL, NULL, NULL };
    int numTransferSyntaxes = 0;

    switch (opt_out_networkTransferSyntax) {
    case EXS_LittleEndianImplicit:
        /* we only support Little Endian Implicit */
        transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
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
#ifdef WITH_ZLIB
    case EXS_DeflatedLittleEndianExplicit:
        /* we prefer Deflated Little Endian Explicit */
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
        } else {
            transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    }

    return ASC_addPresentationContext(
        params, pid, abstractSyntax,
        transferSyntaxes, numTransferSyntaxes);
}

static OFCondition
acceptSubAssoc(T_ASC_Network *aNet, T_ASC_Association **assoc)
{
    const char *knownAbstractSyntaxes[] = {
        UID_VerificationSOPClass
    };
    const char *transferSyntaxes[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    int numTransferSyntaxes;
    OFString temp_str;

    OFCondition cond = ASC_receiveAssociation(aNet, assoc, opt_maxPDU);
    if (cond.good())
    {
      OFLOG_INFO(movescuLogger, "Sub-Association Received");
      OFLOG_DEBUG(movescuLogger, "Parameters:" << OFendl << ASC_dumpParameters(temp_str, (*assoc)->params, ASC_ASSOC_RQ));

      switch (opt_in_networkTransferSyntax)
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
            } else {
              transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
              transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            }
            transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 3;
          }
          break;

        }

        /* accept the Verification SOP Class if presented */
        cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
            (*assoc)->params,
            knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes),
            transferSyntaxes, numTransferSyntaxes);

        if (cond.good())
        {
            /* the array of Storage SOP Class UIDs comes from dcuid.h */
            cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
                (*assoc)->params,
                dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs,
                transferSyntaxes, numTransferSyntaxes);
        }
    }
    if (cond.good())
        cond = ASC_acknowledgeAssociation(*assoc);
    if (cond.good())
    {
        OFLOG_INFO(movescuLogger, "Sub-Association Acknowledged (Max Send PDV: " << (*assoc)->sendPDVLength << ")");
        if (ASC_countAcceptedPresentationContexts((*assoc)->params) == 0)
            OFLOG_INFO(movescuLogger, "    (but no valid presentation contexts)");
        /* dump the presentation contexts which have been accepted/refused */
        OFLOG_DEBUG(movescuLogger, ASC_dumpParameters(temp_str, (*assoc)->params, ASC_ASSOC_AC));
    } else {
        OFLOG_ERROR(movescuLogger, DimseCondition::dump(temp_str, cond));
        ASC_dropAssociation(*assoc);
        ASC_destroyAssociation(assoc);
    }
    return cond;
}

static OFCondition echoSCP(
    T_ASC_Association *assoc,
    T_DIMSE_Message *msg,
    T_ASC_PresentationContextID presID)
{
    OFString temp_str;
    // assign the actual information of the C-Echo-RQ command to a local variable
    T_DIMSE_C_EchoRQ *req = &msg->msg.CEchoRQ;
    if (movescuLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        OFLOG_INFO(movescuLogger, "Received Echo Request");
        OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(temp_str, *req, DIMSE_INCOMING, NULL, presID));
    } else {
        OFLOG_INFO(movescuLogger, "Received Echo Request (MsgID " << req->MessageID << ")");
    }

    /* the echo succeeded !! */
    OFCondition cond = DIMSE_sendEchoResponse(assoc, presID, req, STATUS_Success, NULL);
    if (cond.bad())
    {
        OFLOG_ERROR(movescuLogger, "Echo SCP Failed: " << DimseCondition::dump(temp_str, cond));
    }
    return cond;
}

struct StoreCallbackData
{
    char *imageFileName;
    DcmFileFormat *dcmff;
    T_ASC_Association *assoc;
};

static void
storeSCPCallback(
    /* in */
    void *callbackData,
    T_DIMSE_StoreProgress *progress,    /* progress state */
    T_DIMSE_C_StoreRQ *req,             /* original store request */
    char *imageFileName, DcmDataset **imageDataSet, /* being received into */
    /* out */
    T_DIMSE_C_StoreRSP *rsp,            /* final store response */
    DcmDataset **statusDetail)
{
    DIC_UI sopClass;
    DIC_UI sopInstance;

    if ((opt_abortDuringStore && progress->state != DIMSE_StoreBegin) ||
        (opt_abortAfterStore && progress->state == DIMSE_StoreEnd)) {
        OFLOG_INFO(movescuLogger, "ABORT initiated (due to command line options)");
        ASC_abortAssociation(OFstatic_cast(StoreCallbackData*, callbackData)->assoc);
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
        return;
    }

    if (opt_sleepDuring > 0)
    {
      OFStandard::sleep(OFstatic_cast(unsigned int, opt_sleepDuring));
    }

    // dump some information if required (depending on the progress state)
    // We can't use oflog for the pdu output, but we use a special logger for
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

    if (progress->state == DIMSE_StoreEnd)
    {
       *statusDetail = NULL;    /* no status detail */

       /* could save the image somewhere else, put it in database, etc */
       /*
        * An appropriate status code is already set in the resp structure, it need not be success.
        * For example, if the caller has already detected an out of resources problem then the
        * status will reflect this.  The callback function is still called to allow cleanup.
        */
       // rsp->DimseStatus = STATUS_Success;

       if ((imageDataSet != NULL) && (*imageDataSet != NULL) && !opt_bitPreserving && !opt_ignore)
       {
         StoreCallbackData *cbdata = OFstatic_cast(StoreCallbackData*, callbackData);
         /* create full path name for the output file */
         OFString ofname;
         OFStandard::combineDirAndFilename(ofname, opt_outputDirectory, cbdata->imageFileName, OFTrue /* allowEmptyDirName */);

         E_TransferSyntax xfer = opt_writeTransferSyntax;
         if (xfer == EXS_Unknown) xfer = (*imageDataSet)->getOriginalXfer();

         OFCondition cond = cbdata->dcmff->saveFile(ofname.c_str(), xfer, opt_sequenceType, opt_groupLength,
           opt_paddingType, OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad),
           (opt_useMetaheader) ? EWM_fileformat : EWM_dataset);
         if (cond.bad())
         {
           OFLOG_ERROR(movescuLogger, "cannot write DICOM file: " << ofname);
           rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
         }

        /* should really check the image to make sure it is consistent,
         * that its sopClass and sopInstance correspond with those in
         * the request.
         */
        if ((rsp->DimseStatus == STATUS_Success) && !opt_ignore)
        {
          /* which SOP class and SOP instance ? */
          if (!DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance, opt_correctUIDPadding))
          {
             OFLOG_FATAL(movescuLogger, "bad DICOM file: " << imageFileName);
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
    }
}

static OFCondition storeSCP(
  T_ASC_Association *assoc,
  T_DIMSE_Message *msg,
  T_ASC_PresentationContextID presID)
{
    OFCondition cond = EC_Normal;
    T_DIMSE_C_StoreRQ *req;
    char imageFileName[2048];

    req = &msg->msg.CStoreRQ;

    if (opt_ignore)
    {
#ifdef _WIN32
        tmpnam(imageFileName);
#else
        strcpy(imageFileName, NULL_DEVICE_NAME);
#endif
    } else {
        sprintf(imageFileName, "%s.%s",
            dcmSOPClassUIDToModality(req->AffectedSOPClassUID),
            req->AffectedSOPInstanceUID);
    }

    OFString temp_str;
    if (movescuLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        OFLOG_INFO(movescuLogger, "Received Store Request");
        OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(temp_str, *req, DIMSE_INCOMING, NULL, presID));
    } else {
        OFLOG_INFO(movescuLogger, "Received Store Request (MsgID " << req->MessageID << ", "
            << dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "OT") << ")");
    }

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

    DcmDataset *dset = dcmff.getDataset();

    if (opt_bitPreserving)
    {
      cond = DIMSE_storeProvider(assoc, presID, req, imageFileName, opt_useMetaheader,
        NULL, storeSCPCallback, OFreinterpret_cast(void*, &callbackData), opt_blockMode, opt_dimse_timeout);
    } else {
      cond = DIMSE_storeProvider(assoc, presID, req, NULL, opt_useMetaheader,
        &dset, storeSCPCallback, OFreinterpret_cast(void*, &callbackData), opt_blockMode, opt_dimse_timeout);
    }

    if (cond.bad())
    {
      OFLOG_ERROR(movescuLogger, "Store SCP Failed: " << DimseCondition::dump(temp_str, cond));
      /* remove file */
      if (!opt_ignore)
      {
        if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) unlink(imageFileName);
      }
#ifdef _WIN32
    } else if (opt_ignore) {
        if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) unlink(imageFileName); // delete the temporary file
#endif
    }

    if (opt_sleepAfter > 0)
    {
      OFStandard::sleep(OFstatic_cast(unsigned int, opt_sleepDuring));
    }
    return cond;
}

static OFCondition
subOpSCP(T_ASC_Association **subAssoc)
{
    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presID;

    if (!ASC_dataWaiting(*subAssoc, 0)) /* just in case */
        return DIMSE_NODATAAVAILABLE;

    OFCondition cond = DIMSE_receiveCommand(*subAssoc, opt_blockMode, opt_dimse_timeout, &presID, &msg, NULL);

    if (cond == EC_Normal) {
      switch (msg.CommandField)
      {
        case DIMSE_C_ECHO_RQ:
          // process C-ECHO-Request
          cond = echoSCP(*subAssoc, &msg, presID);
          break;
        case DIMSE_C_STORE_RQ:
          // process C-STORE-Request
          cond = storeSCP(*subAssoc, &msg, presID);
          break;
        default:
          OFString tempStr;
          // we cannot handle this kind of message
          cond = DIMSE_BADCOMMANDTYPE;
          OFLOG_ERROR(movescuLogger, "Expected C-ECHO or C-STORE request but received DIMSE command 0x"
              << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(4)
              << OFstatic_cast(unsigned, msg.CommandField));
          OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(tempStr, msg, DIMSE_INCOMING, NULL, presID));
          break;
      }
    }
    /* clean up on association termination */
    if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        cond = ASC_acknowledgeRelease(*subAssoc);
        ASC_dropSCPAssociation(*subAssoc);
        ASC_destroyAssociation(subAssoc);
        return cond;
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
    }
    else if (cond != EC_Normal)
    {
        OFString temp_str;
        OFLOG_ERROR(movescuLogger, "DIMSE failure (aborting sub-association): " << DimseCondition::dump(temp_str, cond));
        /* some kind of error so abort the association */
        cond = ASC_abortAssociation(*subAssoc);
    }

    if (cond != EC_Normal)
    {
        ASC_dropAssociation(*subAssoc);
        ASC_destroyAssociation(subAssoc);
    }
    return cond;
}

static void
subOpCallback(void * /*subOpCallbackData*/ ,
        T_ASC_Network *aNet, T_ASC_Association **subAssoc)
{

    if (aNet == NULL) return;   /* help no net ! */

    if (*subAssoc == NULL) {
        /* negotiate association */
        acceptSubAssoc(aNet, subAssoc);
    } else {
        /* be a service class provider */
        subOpSCP(subAssoc);
    }
}

static void
moveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request,
    int responseCount, T_DIMSE_C_MoveRSP *response)
{
    OFCondition cond = EC_Normal;
    MyCallbackInfo *myCallbackData;
    OFString temp_str;

    myCallbackData = OFstatic_cast(MyCallbackInfo*, callbackData);

    if (movescuLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL)) {
        OFLOG_INFO(movescuLogger, "Received Move Response " << responseCount);
        OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(temp_str, *response, DIMSE_INCOMING));
    } else {
        OFLOG_INFO(movescuLogger, "Received Move Response " << responseCount << " (" << DU_cmoveStatusString(response->DimseStatus) << ")");
    }

    /* should we send a cancel back ?? */
    if (opt_cancelAfterNResponses == responseCount) {
        OFLOG_INFO(movescuLogger, "Sending Cancel Request (MsgID " << request->MessageID
            << ", PresID " << OFstatic_cast(unsigned int, myCallbackData->presId) << ")");
        cond = DIMSE_sendCancelRequest(myCallbackData->assoc,
            myCallbackData->presId, request->MessageID);
        if (cond != EC_Normal) {
            OFLOG_ERROR(movescuLogger, "Cancel Request Failed: " << DimseCondition::dump(temp_str, cond));
        }
    }
}


static void
substituteOverrideKeys(DcmDataset *dset)
{
    if (overrideKeys == NULL) {
        return; /* nothing to do */
    }

    /* copy the override keys */
    DcmDataset keys(*overrideKeys);

    /* put the override keys into dset replacing existing tags */
    unsigned long elemCount = keys.card();
    for (unsigned long i = 0; i < elemCount; i++) {
        DcmElement *elem = keys.remove(OFstatic_cast(unsigned long, 0));

        dset->insert(elem, OFTrue);
    }
}


static  OFCondition
moveSCU(T_ASC_Association *assoc, const char *fname)
{
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_MoveRQ    req;
    T_DIMSE_C_MoveRSP   rsp;
    DIC_US              msgId = assoc->nextMsgID++;
    DcmDataset          *rspIds = NULL;
    const char          *sopClass;
    DcmDataset          *statusDetail = NULL;
    MyCallbackInfo      callbackData;
    OFString            temp_str;

    DcmFileFormat dcmff;

    if (fname != NULL) {
        if (dcmff.loadFile(fname).bad()) {
            OFLOG_ERROR(movescuLogger, "bad DICOM file: " << fname << ": " << dcmff.error().text());
            return DIMSE_BADDATA;
        }
    }

    /* replace specific keys by those in overrideKeys */
    substituteOverrideKeys(dcmff.getDataset());

    sopClass = querySyntax[opt_queryModel].moveSyntax;

    /* which presentation context should be used */
    presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    if (presId == 0) return DIMSE_NOVALIDPRESENTATIONCONTEXTID;

    callbackData.assoc = assoc;
    callbackData.presId = presId;

    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, sopClass);
    req.Priority = DIMSE_PRIORITY_MEDIUM;
    req.DataSetType = DIMSE_DATASET_PRESENT;
    if (opt_moveDestination == NULL) {
        /* set the destination to be me */
        ASC_getAPTitles(assoc->params, req.MoveDestination,
            NULL, NULL);
    } else {
        strcpy(req.MoveDestination, opt_moveDestination);
    }

    if (movescuLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        OFLOG_INFO(movescuLogger, "Sending Move Request");
        OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(temp_str, req, DIMSE_OUTGOING, NULL, presId));
    } else {
        OFLOG_INFO(movescuLogger, "Sending Move Request (MsgID " << msgId << ")");
    }
    OFLOG_INFO(movescuLogger, "Request Identifiers:" << OFendl << DcmObject::PrintHelper(*dcmff.getDataset()));

    OFCondition cond = DIMSE_moveUser(assoc, presId, &req, dcmff.getDataset(),
        moveCallback, &callbackData, opt_blockMode, opt_dimse_timeout, net, subOpCallback,
        NULL, &rsp, &statusDetail, &rspIds, opt_ignorePendingDatasets);

    if (cond == EC_Normal) {
        if (movescuLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL)) {
            OFLOG_INFO(movescuLogger, "Received Final Move Response");
            OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(temp_str, rsp, DIMSE_INCOMING));
            if (rspIds != NULL) {
                OFLOG_DEBUG(movescuLogger, "Response Identifiers:" << OFendl << DcmObject::PrintHelper(*rspIds));
            }
        } else {
            OFLOG_INFO(movescuLogger, "Received Final Move Response (" << DU_cmoveStatusString(rsp.DimseStatus) << ")");
        }
    } else {
        OFLOG_ERROR(movescuLogger, "Move Request Failed: " << DimseCondition::dump(temp_str, cond));
    }
    if (statusDetail != NULL) {
        OFLOG_DEBUG(movescuLogger, "Status Detail:" << OFendl << DcmObject::PrintHelper(*statusDetail));
        delete statusDetail;
    }

    if (rspIds != NULL) delete rspIds;

    return cond;
}


static OFCondition
cmove(T_ASC_Association *assoc, const char *fname)
{
    OFCondition cond = EC_Normal;
    int n = OFstatic_cast(int, opt_repeatCount);
    while (cond.good() && n--)
        cond = moveSCU(assoc, fname);
    return cond;
}
