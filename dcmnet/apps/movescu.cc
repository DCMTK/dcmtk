/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Purpose: Query/Retrieve Service Class User (C-MOVE operation)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-08-20 12:21:21 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/apps/movescu.cc,v $
 *  CVS/RCS Revision: $Revision: 1.41 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */                        

#include "osconfig.h" /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

BEGIN_EXTERN_C
#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#include <errno.h>
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "dicom.h"
#include "dimse.h"
#include "diutil.h"
#include "dcfilefo.h"
#include "dcdebug.h"
#include "dcuid.h"
#include "dcdict.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "movescu"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default application titles */
#define APPLICATIONTITLE	"MOVESCU"
#define PEERAPPLICATIONTITLE	"ANY-SCP"

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
E_TransferSyntax  opt_networkTransferSyntax = EXS_Unknown;
E_TransferSyntax  opt_writeTransferSyntax = EXS_Unknown;
E_GrpLenEncoding  opt_groupLength = EGL_recalcGL;
E_EncodingType    opt_sequenceType = EET_ExplicitLength;
E_PaddingEncoding opt_paddingType = EPD_withoutPadding;
OFCmdUnsignedInt  opt_filepad = 0;
OFCmdUnsignedInt  opt_itempad = 0; 
OFBool            opt_verbose = OFFalse;
OFBool            opt_debug = OFFalse;
OFBool            opt_bitPreserving = OFFalse;
OFBool            opt_ignore = OFFalse;
OFBool            opt_abortDuringStore = OFFalse;
OFBool            opt_abortAfterStore = OFFalse;
OFCmdUnsignedInt  opt_repeatCount = 1; 
OFCmdUnsignedInt  opt_retrievePort = 104; 
E_TransferSyntax  opt_in_networkTransferSyntax = EXS_Unknown;
E_TransferSyntax  opt_out_networkTransferSyntax = EXS_Unknown;
OFBool            opt_abortAssociation = OFFalse;
const char *      opt_moveDestination = NULL;
OFCmdSignedInt    opt_cancelAfterNResponses = -1;
QueryModel        opt_queryModel = QMPatientRoot;

static T_ASC_Network *net = NULL; /* the global DICOM network */
static DcmDataset *overrideKeys = NULL;
static QuerySyntax querySyntax[3] = {
    { UID_FINDPatientRootQueryRetrieveInformationModel, 
      UID_MOVEPatientRootQueryRetrieveInformationModel },
    { UID_FINDStudyRootQueryRetrieveInformationModel, 
      UID_MOVEStudyRootQueryRetrieveInformationModel },
    { UID_FINDPatientStudyOnlyQueryRetrieveInformationModel, 
      UID_MOVEPatientStudyOnlyQueryRetrieveInformationModel }
};


static void 
errmsg(const char *msg,...)
{
    va_list args;

    fprintf(stderr, "%s: ", OFFIS_CONSOLE_APPLICATION);
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}

static void
addOverrideKey(OFConsoleApplication& app, const char* s)
{
    unsigned int g = 0xffff;
    unsigned int e = 0xffff;
    int n = 0;
    char val[1024];
    OFString msg;
    char msg2[200];

    val[0] = '\0';
    n = sscanf(s, "%x,%x=%s", &g, &e, val);
    
    if (n < 2) {
      msg = "bad key format: ";
      msg += s;
      app.printError(msg.c_str());
    }

    const char* spos = s;
    char ccc;
    do
    {
      ccc = *spos;
      if (ccc == '=') break;
      if (ccc == 0) { spos = NULL; break; }
      spos++;
    } while(1);

    if (spos && *(spos+1)) {
        strcpy(val, spos+1);
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
    if (strlen(val) > 0) {
	elem->putString(val);
	if (elem->error() != EC_Normal)
	{
            sprintf(msg2, "cannot put tag value: (%04x,%04x)=\"", g, e);
            msg = msg2;
            msg += val;
            msg += "\"";
            app.printError(msg.c_str());
	}
    }

    if (overrideKeys == NULL) overrideKeys = new DcmDataset;
    overrideKeys->insert(elem, OFTrue);
    if (overrideKeys->error() != EC_Normal) {
        sprintf(msg2, "cannot insert tag: (%04x,%04x)", g, e);
        app.printError(msg2);
    }
}

static OFCondition cmove(T_ASC_Association *assoc, const char *fname);

static OFCondition
addPresentationContext(T_ASC_Parameters *params, 
			T_ASC_PresentationContextID pid,
			const char* abstractSyntax);

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

    SetDebugLevel((0));	/* stop dcmdata debugging messages */

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
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM retrieve (C-MOVE) SCU", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL+SHORTCOL+4);
  cmd.addParam("peer", "hostname of DICOM peer");
  cmd.addParam("port", "tcp/ip port number of peer");
  cmd.addParam("dcmfile_in", "DICOM query file(s)", OFCmdParam::PM_MultiOptional);

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL+2);
   cmd.addOption("--help",                      "-h",        "print this help text and exit");
   cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
   cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");
  cmd.addGroup("network options:");
    cmd.addSubGroup("override matching keys:");
      cmd.addOption("--key",              "-k",   1,   "key: gggg,eeee=\"string\"", "override matching key");
    cmd.addSubGroup("query information model:");
      cmd.addOption("--patient",          "-P",        "use patient root information model (default)");
      cmd.addOption("--study",            "-S",        "use study root information model");
      cmd.addOption("--psonly",           "-O",        "use patient/study only information model");
    cmd.addSubGroup("application entity titles:");
      OFString opt1 = "set my calling AE title (default: ";
      opt1 += APPLICATIONTITLE;
      opt1 += ")";
      cmd.addOption("--aetitle",                "-aet",   1, "aetitle: string", opt1.c_str());
      OFString opt2 = "set called AE title of peer (default: ";
      opt2 += PEERAPPLICATIONTITLE;
      opt2 += ")";
      cmd.addOption("--call",                   "-aec",   1, "aetitle: string", opt2.c_str());
      OFString opt5 = "set move destinat. AE title (default: ";
      opt5 += APPLICATIONTITLE;
      opt5 += ")";
      cmd.addOption("--move",                   "-aem",   1, "aetitle: string", opt5.c_str());
    cmd.addSubGroup("preferred network transfer syntaxes (incoming associations):");
      cmd.addOption("--prefer-uncompr",         "+x=",       "prefer explicit VR local byte order (default)");
      cmd.addOption("--prefer-little",          "+xe",       "prefer explicit VR little endian TS");
      cmd.addOption("--prefer-big",             "+xb",       "prefer explicit VR big endian TS");
      cmd.addOption("--prefer-lossless",        "+xs",       "prefer default JPEG lossless TS");
      cmd.addOption("--prefer-jpeg8",           "+xy",       "prefer default JPEG lossy TS for 8 bit data");
      cmd.addOption("--prefer-jpeg12",          "+xx",       "prefer default JPEG lossy TS for 12 bit data");
      cmd.addOption("--prefer-rle",             "+xr",       "prefer RLE lossless TS");
      cmd.addOption("--implicit",               "+xi",       "accept implicit VR little endian TS only");
    cmd.addSubGroup("proposed transmission transfer syntaxes (outgoing associations):");
      cmd.addOption("--propose-uncompr",        "-x=",       "propose all uncompressed TS, explicit VR\nwith local byte ordering first (default)");
      cmd.addOption("--propose-little",         "-xe",       "propose all uncompressed TS, explicit VR\nlittle endian first");
      cmd.addOption("--propose-big",            "-xb",       "propose all uncompressed TS, explicit VR\nbig endian first");
      cmd.addOption("--propose-implicit",       "-xi",       "propose implicit VR little endian TS only");
    cmd.addSubGroup("other network options:");
      OFString opt6 = "[n]umber: integer (default: ";
      sprintf(tempstr, "%ld", (long)opt_retrievePort);
      opt6 += tempstr;
      opt6 += ")";
      cmd.addOption("--port",                   "+P",    1,  opt6.c_str(), "port number for incoming associations");
      OFString opt3 = "set max receive pdu to n bytes (default: ";
      sprintf(tempstr, "%ld", (long)ASC_DEFAULTMAXPDU);
      opt3 += tempstr;
      opt3 += ")";
      OFString opt4 = "[n]umber of bytes: integer [";
      sprintf(tempstr, "%ld", (long)ASC_MINIMUMPDUSIZE);
      opt4 += tempstr;
      opt4 += "..";
      sprintf(tempstr, "%ld", (long)ASC_MAXIMUMPDUSIZE);
      opt4 += tempstr;
      opt4 += "]";
      cmd.addOption("--max-pdu",                "-pdu",   1,  opt4.c_str(), opt3.c_str());
      cmd.addOption("--disable-host-lookup",    "-dhl",      "disable hostname lookup");
      cmd.addOption("--repeat",                           1,  "[n]umber: integer", "repeat n times");
      cmd.addOption("--abort",                                "abort association instead of releasing it");
      cmd.addOption("--ignore",                               "ignore store data, receive but do not store");
      cmd.addOption("--cancel",                           1,  "[n]umber: integer", "cancel after n responses (default: never)");
  cmd.addGroup("output options:");
    cmd.addSubGroup("bit preserving mode:");
      cmd.addOption("--normal",                 "-B",        "allow implicit format conversions (default)");
      cmd.addOption("--bit-preserving",         "+B",        "write data exactly as read");
    cmd.addSubGroup("output file format:");
      cmd.addOption("--write-file",             "+F",        "write file format (default)");
      cmd.addOption("--write-dataset",          "-F",        "write data set without file meta information");
    cmd.addSubGroup("output transfer syntax (not with --bit-preserving or compressed transmission):");
      cmd.addOption("--write-xfer-same",        "+t=",       "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",      "+te",       "write with explicit VR little endian TS");
      cmd.addOption("--write-xfer-big",         "+tb",       "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit",    "+ti",       "write with implicit VR little endian TS");
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
      cmd.addOption("--padding-create",         "+p",    2,  "[f]ile-pad [i]tem-pad: integer", "align file on multiple of f bytes\nand items on multiple of i bytes");

    /* evaluate command line */                           
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      /* check for --help first */
      if (cmd.findOption("--help")) app.printUsage();

      cmd.getParam(1, opt_peer);
      app.checkParam(cmd.getParamAndCheckMinMax(2, opt_port, 1, 65535));

      if (cmd.findOption("--verbose")) opt_verbose=OFTrue;
      if (cmd.findOption("--debug")) 
      {
      	opt_debug = OFTrue;
        DUL_Debug(OFTrue);
        DIMSE_debug(OFTrue);
      	SetDebugLevel(3);
      }
      
      if (cmd.findOption("--key", 0, OFCommandLine::FOM_First))
      {
      	const char *ovKey = NULL;
      	do {
      	  app.checkValue(cmd.getValue(ovKey));
      	  addOverrideKey(app, ovKey);
      	} while (cmd.findOption("--key", 0, OFCommandLine::FOM_Next));
      }      
      cmd.beginOptionBlock();
      if (cmd.findOption("--patient"))  opt_queryModel = QMPatientRoot;
      if (cmd.findOption("--study"))    opt_queryModel = QMStudyRoot;
      if (cmd.findOption("--psonly"))   opt_queryModel = QMPatientStudyOnly;
      cmd.endOptionBlock();
      if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
      if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));
      if (cmd.findOption("--move")) app.checkValue(cmd.getValue(opt_moveDestination));
      cmd.beginOptionBlock();
      if (cmd.findOption("--prefer-uncompr"))  opt_in_networkTransferSyntax = EXS_Unknown;
      if (cmd.findOption("--prefer-little"))   opt_in_networkTransferSyntax = EXS_LittleEndianExplicit;
      if (cmd.findOption("--prefer-big"))      opt_in_networkTransferSyntax = EXS_BigEndianExplicit;
      if (cmd.findOption("--prefer-lossless")) opt_in_networkTransferSyntax = EXS_JPEGProcess14SV1TransferSyntax;
      if (cmd.findOption("--prefer-jpeg8"))    opt_in_networkTransferSyntax = EXS_JPEGProcess1TransferSyntax;
      if (cmd.findOption("--prefer-jpeg12"))   opt_in_networkTransferSyntax = EXS_JPEGProcess2_4TransferSyntax;
      if (cmd.findOption("--prefer-rle"))      opt_in_networkTransferSyntax = EXS_RLELossless;
      if (cmd.findOption("--implicit"))        opt_in_networkTransferSyntax = EXS_LittleEndianImplicit;
      cmd.endOptionBlock();
      cmd.beginOptionBlock();
      if (cmd.findOption("--propose-uncompr"))  opt_out_networkTransferSyntax = EXS_Unknown;
      if (cmd.findOption("--propose-little"))   opt_out_networkTransferSyntax = EXS_LittleEndianExplicit;
      if (cmd.findOption("--propose-big"))      opt_out_networkTransferSyntax = EXS_BigEndianExplicit;
      if (cmd.findOption("--propose-implicit")) opt_out_networkTransferSyntax = EXS_LittleEndianImplicit;
      cmd.endOptionBlock();

      if (cmd.findOption("--port"))    app.checkValue(cmd.getValueAndCheckMinMax(opt_retrievePort, 1, 65535));
      if (cmd.findOption("--max-pdu")) app.checkValue(cmd.getValueAndCheckMinMax(opt_maxPDU, ASC_MINIMUMPDUSIZE, ASC_MAXIMUMPDUSIZE));
      if (cmd.findOption("--disable-host-lookup")) dcmDisableGethostbyaddr.set(OFTrue);
      if (cmd.findOption("--repeat"))  app.checkValue(cmd.getValueAndCheckMin(opt_repeatCount, 1));
      if (cmd.findOption("--abort"))   opt_abortAssociation = OFTrue;
      if (cmd.findOption("--ignore"))  opt_ignore = OFTrue;
      if (cmd.findOption("--cancel"))  app.checkValue(cmd.getValueAndCheckMin(opt_cancelAfterNResponses, 0));
      
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

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
	  DCM_DICT_ENVIRONMENT_VARIABLE);
  }

#ifdef HAVE_GETEUID
    /* if retrieve port is privileged we must be as well */
    if (opt_retrievePort < 1024) {
        if (geteuid() != 0) {
	    errmsg("cannot listen on port %d, insufficient privileges", opt_retrievePort);
            return 1;
	}
    }
#endif

    /* network for move request and responses */
    OFCondition cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, (int)opt_retrievePort, 
				 1000, &net);
    if (cond.bad())
    {
	errmsg("cannot create network:");
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

    /* set up main association */
    cond = ASC_createAssociationParameters(&params, opt_maxPDU);
    if (cond.bad()) {
	DimseCondition::dump(cond);
	exit(1);
    }
    ASC_setAPTitles(params, opt_ourTitle, opt_peerTitle, NULL);

    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", opt_peer, (int)opt_port);
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
	DimseCondition::dump(cond);
	exit(1);
    }
    if (opt_debug) {
	printf("Request Parameters:\n");
	ASC_dumpParameters(params, COUT);
    }

    /* create association */
    if (opt_verbose)
	printf("Requesting Association\n");
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond.bad()) {
	if (cond == DUL_ASSOCIATIONREJECTED) {
	    T_ASC_RejectParameters rej;

	    ASC_getRejectParameters(params, &rej);
	    errmsg("Association Rejected:");
	    ASC_printRejectParameters(stderr, &rej);
	    exit(1);
	} else {
	    errmsg("Association Request Failed:");
	    DimseCondition::dump(cond);
	    exit(1);
	}
    }
    /* what has been accepted/refused ? */
    if (opt_debug) {
	printf("Association Parameters Negotiated:\n");
	ASC_dumpParameters(params, COUT);
    }

    if (ASC_countAcceptedPresentationContexts(params) == 0) {
	errmsg("No Acceptable Presentation Contexts");
	exit(1);
    }

    if (opt_verbose) {
	printf("Association Accepted (Max Send PDV: %lu)\n",
		assoc->sendPDVLength);
    }

    /* do the real work */
    cond = EC_Normal;
    if (fileNameList.empty())
    {
	/* no files provided on command line */
	cond = cmove(assoc, NULL);
    } else {
      OFListIterator(OFString) iter = fileNameList.begin();
      OFListIterator(OFString) enditer = fileNameList.end();    
      while ((iter != enditer) && (cond == EC_Normal)) // compare with EC_Normal since DUL_PEERREQUESTEDRELEASE is also good()
      {
          cond = cmove(assoc, (*iter).c_str());
          ++iter;
      }    	
    }

    /* tear down association */
    if (cond == EC_Normal)
    {
	if (opt_abortAssociation) {
	    if (opt_verbose)
		printf("Aborting Association\n");
	    cond = ASC_abortAssociation(assoc);
	    if (cond.bad()) {
		errmsg("Association Abort Failed:");
		DimseCondition::dump(cond);
		exit(1);
	    }
	} else {
	    /* release association */
	    if (opt_verbose)
		printf("Releasing Association\n");
	    cond = ASC_releaseAssociation(assoc);
	    if (cond.bad())
	    {
		errmsg("Association Release Failed:");
		DimseCondition::dump(cond);
		exit(1);
	    }
	}
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE)
    {
	errmsg("Protocol Error: peer requested release (Aborting)");
	if (opt_verbose)
	    printf("Aborting Association\n");
	cond = ASC_abortAssociation(assoc);
	if (cond.bad()) {
	    errmsg("Association Abort Failed:");
	    DimseCondition::dump(cond);
	    exit(1);
	}
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
	if (opt_verbose) printf("Peer Aborted Association\n");
    }
    else 
    {
	errmsg("SCU Failed:");
	DimseCondition::dump(cond);
	if (opt_verbose)
	    printf("Aborting Association\n");
	cond = ASC_abortAssociation(assoc);
	if (cond.bad()) {
	    errmsg("Association Abort Failed:");
	    DimseCondition::dump(cond);
	    exit(1);
	}
    }

    cond = ASC_destroyAssociation(&assoc);
    if (cond.bad()) {
	DimseCondition::dump(cond);
	exit(1);
    }
    cond = ASC_dropNetwork(&net);
    if (cond.bad()) {
	DimseCondition::dump(cond);
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
			const char* abstractSyntax)
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

    const char* transferSyntaxes[] = { NULL, NULL, NULL };
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
acceptSubAssoc(T_ASC_Network * aNet, T_ASC_Association ** assoc)
{
    const char* knownAbstractSyntaxes[] = {
	UID_VerificationSOPClass
    };
    const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL };
    int numTransferSyntaxes;

    OFCondition cond = ASC_receiveAssociation(aNet, assoc, opt_maxPDU);
    if (cond.good())
    {
      switch (opt_in_networkTransferSyntax)
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
          transferSyntaxes[0] = UID_RLELossless;
          transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
          transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
          transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
          numTransferSyntaxes = 4;
          break;
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
	        dcmStorageSOPClassUIDs, numberOfDcmStorageSOPClassUIDs,
	        transferSyntaxes, numTransferSyntaxes);
        }
    }
    if (cond.good()) cond = ASC_acknowledgeAssociation(*assoc);
    if (cond.bad()) {
        ASC_dropAssociation(*assoc);
        ASC_destroyAssociation(assoc);
    }
    return cond;
}

static OFCondition echoSCP(
  T_ASC_Association * assoc,
  T_DIMSE_Message * msg,
  T_ASC_PresentationContextID presID)
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

    if ((opt_abortDuringStore > 0 && progress->state != DIMSE_StoreBegin) ||
        (opt_abortAfterStore > 0 && progress->state == DIMSE_StoreEnd)) {
        if (opt_verbose) {
            printf("ABORT initiated (due to command line options)\n");
        }
        ASC_abortAssociation(((StoreCallbackData*) callbackData)->assoc);
        rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
        return;
    }

    if (opt_sleepDuring > 0)
    {
      sleep((unsigned int)opt_sleepDuring);
    }
    
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

       if ((imageDataSet)&&(*imageDataSet)&&(!opt_bitPreserving)&&(!opt_ignore))
       {
         StoreCallbackData *cbdata = (StoreCallbackData*) callbackData;
         const char* fileName = cbdata->imageFileName;

         DcmFileStream outf(fileName, DCM_WriteMode);
         if (outf.Fail())
         {
           fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName);
           rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
         } else {
           E_TransferSyntax xfer = opt_writeTransferSyntax;
           if (xfer == EXS_Unknown) xfer = (*imageDataSet)->getOriginalXfer();

           if (opt_useMetaheader)
           {
             /* write as fileformat */
             cbdata->dcmff->transferInit();
             cbdata->dcmff->write(outf, xfer, opt_sequenceType, opt_groupLength,
               opt_paddingType, (Uint32)opt_filepad, (Uint32)opt_itempad);
             cbdata->dcmff->transferEnd();
             if (cbdata->dcmff->error() != EC_Normal)
             {
               fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName);
               rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
             }
           } else {
             /* write as dataset */
             (*imageDataSet)->transferInit();
             (*imageDataSet)->write(outf, xfer, opt_sequenceType, opt_groupLength,
               opt_paddingType, (Uint32)opt_filepad, (Uint32)opt_itempad);
             (*imageDataSet)->transferEnd();
             if ((*imageDataSet)->error() != EC_Normal)
             {
               fprintf(stderr, "storescp: Cannot write image file: %s\n", fileName);
               rsp->DimseStatus = STATUS_STORE_Refused_OutOfResources;
             }
           }
         }
      
        /* should really check the image to make sure it is consistent,
         * that its sopClass and sopInstance correspond with those in
         * the request.
         */
        if ((rsp->DimseStatus == STATUS_Success)&&(!opt_ignore))
        {
          /* which SOP class and SOP instance ? */
          if (! DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance))
          {
             fprintf(stderr, "storescp: Bad image file: %s\n", imageFileName);
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
    return;
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

    if (opt_verbose)
    {
      printf("Received ");
      DIMSE_printCStoreRQ(stdout, req);
    }

    StoreCallbackData callbackData;
    callbackData.assoc = assoc;
    callbackData.imageFileName = imageFileName;
    DcmFileFormat dcmff;
    callbackData.dcmff = &dcmff;

    DcmDataset *dset = dcmff.getDataset();

    if (opt_bitPreserving)
    {
      cond = DIMSE_storeProvider(assoc, presID, req, imageFileName, opt_useMetaheader,
        NULL, storeSCPCallback, (void*)&callbackData, DIMSE_BLOCKING, 0);
    } else {
      cond = DIMSE_storeProvider(assoc, presID, req, (char *)NULL, opt_useMetaheader,
        &dset, storeSCPCallback, (void*)&callbackData, DIMSE_BLOCKING, 0);
    }

    if (cond.bad())
    {
      fprintf(stderr, "storescp: Store SCP Failed:\n");
      DimseCondition::dump(cond);
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
      sleep((unsigned int)opt_sleepAfter);
    }
    return cond;
}

static OFCondition
subOpSCP(T_ASC_Association **subAssoc)
{
    T_DIMSE_Message     msg;
    T_ASC_PresentationContextID presID;

    if (!ASC_dataWaiting(*subAssoc, 0))	/* just in case */
	return DIMSE_NODATAAVAILABLE;

    OFCondition cond = DIMSE_receiveCommand(*subAssoc, DIMSE_BLOCKING, 0, &presID,
	    &msg, NULL);

    if (cond == EC_Normal) {
	switch (msg.CommandField) {
	case DIMSE_C_STORE_RQ:
	    cond = storeSCP(*subAssoc, &msg, presID);
	    break;
	case DIMSE_C_ECHO_RQ:
	    cond = echoSCP(*subAssoc, &msg, presID);
	    break;
	default:
	    cond = DIMSE_BADCOMMANDTYPE;
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
	errmsg("DIMSE Failure (aborting sub-association):\n");
	DimseCondition::dump(cond);
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

    if (aNet == NULL) return;	/* help no net ! */

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

    myCallbackData = (MyCallbackInfo*)callbackData;

    if (opt_verbose) {
        printf("Move Response %d: ", responseCount);
	DIMSE_printCMoveRSP(stdout, response);
    }
    /* should we send a cancel back ?? */
    if (opt_cancelAfterNResponses == responseCount) {
	if (opt_verbose) {
	    printf("Sending Cancel RQ, MsgId: %d, PresId: %d\n",
	        request->MessageID, myCallbackData->presId);
	}
        cond = DIMSE_sendCancelRequest(myCallbackData->assoc,
	    myCallbackData->presId, request->MessageID);
        if (cond != EC_Normal) {
	    errmsg("Cancel RQ Failed:");
	    DimseCondition::dump(cond);
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
    for (unsigned long i=0; i<elemCount; i++) {
	DcmElement *elem = keys.remove((unsigned long)0);

	dset->insert(elem, OFTrue);
    }
}


static  OFCondition
moveSCU(T_ASC_Association * assoc, const char *fname)
{
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_MoveRQ	req;
    T_DIMSE_C_MoveRSP   rsp;
    DIC_US      	msgId = assoc->nextMsgID++;
    DcmDataset		*rspIds = NULL;
    const char          *sopClass;
    DcmDataset		*statusDetail = NULL;
    MyCallbackInfo	callbackData;

    if (opt_verbose) {
	printf("================================\n");
	if (fname) printf("Sending query file: %s\n", fname); else printf("Sending query\n");
    }

    DcmFileFormat dcmff;

    if (fname != NULL) {
	if (dcmff.loadFile(fname).bad()) {
	    errmsg("Bad DICOM file: %s: %s", fname, dcmff.error().text());
	    return DIMSE_BADDATA;
	}
    }

    /* replace specific keys by those in overrideKeys */
    substituteOverrideKeys(dcmff.getDataset());

    sopClass = querySyntax[opt_queryModel].moveSyntax;
    
    /* which presentation context should be used */
    presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    if (presId == 0) return DIMSE_NOVALIDPRESENTATIONCONTEXTID;

    if (opt_verbose) {
	printf("Move SCU RQ: MsgID %d\n", msgId);
	printf("Request:\n");
	dcmff.getDataset()->print(COUT);
    }
    
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

    OFCondition cond = DIMSE_moveUser(assoc, presId, &req, dcmff.getDataset(),
        moveCallback, &callbackData, DIMSE_BLOCKING, 0, 
	net, subOpCallback, NULL,
	&rsp, &statusDetail, &rspIds);
        
    if (cond == EC_Normal) {
        if (opt_verbose) {
	    DIMSE_printCMoveRSP(stdout, &rsp); 
	    if (rspIds != NULL) {
	        printf("Response Identifiers:\n");
		rspIds->print(COUT);
	    }
        }
    } else {
        errmsg("Move Failed:");
	DimseCondition::dump(cond);
    }
    if (statusDetail != NULL) {
        printf("  Status Detail:\n");
	statusDetail->print(COUT);
	delete statusDetail;
    }

    if (rspIds != NULL) delete rspIds;
    
    return cond;
}


static OFCondition
cmove(T_ASC_Association * assoc, const char *fname)
{
    OFCondition cond = EC_Normal;
    int n = (int)opt_repeatCount;

    while (cond.good() && n--)
    {
	cond = moveSCU(assoc, fname);
    }
    return cond;
}

/*
** CVS Log
**
** $Log: movescu.cc,v $
** Revision 1.41  2002-08-20 12:21:21  meichel
** Adapted code to new loadFile and saveFile methods, thus removing direct
**   use of the DICOM stream classes.
**
** Revision 1.40  2002/04/11 12:45:50  joergr
** Adapted layout of command line help.
**
** Revision 1.39  2001/11/09 15:56:24  joergr
** Renamed some of the getValue/getParam methods to avoid ambiguities reported
** by certain compilers.
**
** Revision 1.38  2001/10/12 10:18:21  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.37  2001/09/26 12:28:55  meichel
** Implemented changes in dcmnet required by the adaptation of dcmdata
**   to class OFCondition.  Removed some unused code.
**
** Revision 1.36  2001/06/01 15:50:02  meichel
** Updated copyright header
**
** Revision 1.35  2001/06/01 11:01:56  meichel
** Implemented global flag and command line option to disable reverse
**   DNS hostname lookup using gethostbyaddr when accepting associations.
**
** Revision 1.34  2000/11/10 18:07:42  meichel
** Mixed up strcmp and strcpy - oops.
**
** Revision 1.33  2000/11/10 16:25:03  meichel
** Fixed problem with DIMSE routines which attempted to delete /dev/null
**   under certain circumstances, which could lead to disastrous results if
**   tools were run with root permissions (what they shouldn't).
**
** Revision 1.32  2000/06/07 13:56:17  meichel
** Output stream now passed as mandatory parameter to ASC_dumpParameters.
**
** Revision 1.31  2000/04/14 16:29:26  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.30  2000/03/08 16:43:16  meichel
** Updated copyright header.
**
** Revision 1.29  2000/02/29 11:49:49  meichel
** Removed support for VS value representation. This was proposed in CP 101
**   but never became part of the standard.
**
** Revision 1.28  2000/02/23 15:12:20  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.27  2000/02/03 11:50:08  meichel
** Moved UID related functions from dcmnet (diutil.h) to dcmdata (dcuid.h)
**   where they belong. Renamed access functions to dcmSOPClassUIDToModality
**   and dcmGuessModalityBytes.
**
** Revision 1.26  2000/02/02 15:17:28  meichel
** Replaced some #if statements by more robust #ifdef
**
** Revision 1.25  2000/02/01 10:24:02  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.24  1999/04/30 16:40:22  meichel
** Minor code purifications to keep Sun CC 2.0.1 quiet
**
** Revision 1.23  1999/04/29 12:01:02  meichel
** Adapted movescu to new command line option scheme.
**   Added support for transmission of compressed images.
**
** Revision 1.22  1999/03/29 11:19:54  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.21  1998/08/10 08:53:35  meichel
** renamed member variable in DIMSE structures from "Status" to
**   "DimseStatus". This is required if dcmnet is used together with
**   <X11/Xlib.h> where Status is #define'd as int.
**
** Revision 1.20  1998/02/06 15:07:28  meichel
** Removed many minor problems (name clashes, unreached code)
**   reported by Sun CC4 with "+w" or Sun CC2.
**
** Revision 1.19  1998/01/14 14:35:54  hewett
** Modified existing -u command line option to also disable generation
** of UT and VS (previously just disabled generation of UN).
**
** Revision 1.18  1997/08/05 07:36:20  andreas
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
** Revision 1.17  1997/07/21 08:37:03  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.16  1997/06/26 12:53:08  andreas
** - Include tests for changing of user IDs and the using of fork
**   in code since Windows NT/95 do not support this
** - Corrected error interchanged parameters in a call
**
** Revision 1.15  1997/05/30 07:33:22  meichel
** Added space characters around comments and simplified
** some inlining code (needed for SunCC 2.0.1).
**
** Revision 1.14  1997/05/29 15:52:57  meichel
** Added constant for dcmtk release date in dcuid.h.
** All dcmtk applications now contain a version string
** which is displayed with the command line options ("usage" message)
** and which can be queried in the binary with the "ident" command.
**
** Revision 1.13  1997/05/23 10:44:19  meichel
** Major rewrite of storescp application. See CHANGES for details.
** Changes to interfaces of some DIMSE functions.
**
** Revision 1.12  1997/05/22 13:29:59  hewett
** Modified the test for presence of a data dictionary to use the
** method DcmDataDictionary::isDictionaryLoaded().
**
** Revision 1.11  1997/05/16 08:31:33  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
**
** Revision 1.10  1997/04/18 08:40:14  andreas
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
** Revision 1.8  1997/01/08 12:19:34  hewett
** The Storage SCP code now will accept any presentation context for
** a Storage SOP Class based on the table of Storage SOP Classes
** exported in dcuid.h
**
** Revision 1.7  1997/01/08 10:46:45  hewett
** Changes default AE title to MOVESCU and cleaned up option summary.
**
** Revision 1.6  1996/12/16 15:14:00  hewett
** Added bugfix for WINSOCK support.  The required WINSOCK version
** number was being incorrectly set to version 0.1.  The fixed
** WINSOCK initialisation now uses the MAKEWORD macro to correctly
** set the required version number. This bugfix was contributed
** by Dr. Yongjian Bao of Innomed GmbH, Germany.
**
**
*/

