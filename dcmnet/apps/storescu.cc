/*
 *
 *  Copyright (C) 1994-2000, OFFIS
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
 *  Purpose: Storage Service Class User (C-STORE operation)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-04-14 16:29:27 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/apps/storescu.cc,v $
 *  CVS/RCS Revision: $Revision: 1.33 $
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
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#include <errno.h>
END_EXTERN_C

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "ofstring.h"
#include "dimse.h"
#include "diutil.h"
#include "dcdatset.h"
#include "dcmetinf.h"
#include "dcfilefo.h"
#include "dcdebug.h"
#include "dcuid.h"
#include "dcdict.h"
#include "dcdeftag.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */

#define OFFIS_CONSOLE_APPLICATION "storescu"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* default application titles */
#define APPLICATIONTITLE	"STORESCU"
#define PEERAPPLICATIONTITLE	"ANY-SCP"

static OFBool opt_verbose = OFFalse;
static OFBool opt_showPresentationContexts = OFFalse;
static OFBool opt_debug = OFFalse;
static OFBool opt_abortAssociation = OFFalse;
static OFCmdUnsignedInt opt_maxReceivePDULength = ASC_DEFAULTMAXPDU;
static E_TransferSyntax opt_networkTransferSyntax = EXS_Unknown;

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
static OFString patientNamePrefix("OFFIS^TEST_PN_");   // PatientName is PN (maximum 16 chars)
static OFString patientIDPrefix("PID_"); // PatientID is LO (maximum 64 chars)
static OFString studyIDPrefix("SID_");   // StudyID is SH (maximum 16 chars)
static OFString accessionNumberPrefix;  // AccessionNumber is SH (maximum 16 chars)

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


static CONDITION
addStoragePresentationContexts(T_ASC_Parameters *params, OFList<OFString>& sopClasses);

static CONDITION 
cstore(T_ASC_Association *assoc, const OFString& fname);

#define SHORTCOL 4
#define LONGCOL 18

int
main(int argc, char *argv[])
{
    const char *opt_peer = NULL;
    OFCmdUnsignedInt opt_port = 104;
    const char *opt_peerTitle = PEERAPPLICATIONTITLE;
    const char *opt_ourTitle = APPLICATIONTITLE;

    OFList<OFString> fileNameList;
    OFList<OFString> sopClassUIDList; // the list of sop classes
    OFList<OFString> sopInstanceUIDList; // the list of sop instances

    CONDITION cond;
    T_ASC_Network *net;
    T_ASC_Parameters *params;
    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;
    T_ASC_Association *assoc;

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
  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "DICOM storage (C-STORE) SCU", rcsid);
  OFCommandLine cmd;

  cmd.setParamColumn(LONGCOL+SHORTCOL+4);
  cmd.addParam("peer", "hostname of DICOM peer");
  cmd.addParam("port", "tcp/ip port number of peer");
  cmd.addParam("dcmfile_in", "DICOM file(s) to be transmitted", OFCmdParam::PM_MultiMandatory);

  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.addGroup("general options:", LONGCOL, SHORTCOL+2);
   cmd.addOption("--help",                      "-h",        "print this help text and exit");
   cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
   cmd.addOption("--verbose-pc",                "+v",        "verbose mode and show presentation contexts");
   cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");
  cmd.addGroup("network options:");
    cmd.addSubGroup("application entity titles:");
      OFString opt1 = "set my calling AE title (default: ";
      opt1 += APPLICATIONTITLE;
      opt1 += ")";
      cmd.addOption("--aetitle",                "-aet",   1, "aetitle: string", opt1.c_str());
      OFString opt2 = "set called AE title of peer (default: ";
      opt2 += PEERAPPLICATIONTITLE;
      opt2 += ")";
      cmd.addOption("--call",                   "-aec",   1, "aetitle: string", opt2.c_str());
    cmd.addSubGroup("proposed transmission transfer syntaxes:");
      cmd.addOption("--propose-uncompr",        "-x=",       "propose all uncompressed TS, explicit VR\nwith local byte ordering first (default)");
      cmd.addOption("--propose-little",         "-xe",       "propose all uncompressed TS, explicit VR\nlittle endian first");
      cmd.addOption("--propose-big",            "-xb",       "propose all uncompressed TS, explicit VR\nbig endian first");
      cmd.addOption("--propose-implicit",       "-xi",       "propose implicit VR little endian TS only");
      cmd.addOption("--propose-lossless",       "-xs",       "propose default JPEG lossless TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-jpeg8",          "-xy",       "propose default JPEG lossy TS for 8 bit data\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-jpeg12",         "-xx",       "propose default JPEG lossy TS for 12 bit data\nand all uncompressed transfer syntaxes");
      cmd.addOption("--propose-rle",            "-xr",       "propose RLE lossless TS\nand all uncompressed transfer syntaxes");
      cmd.addOption("--required",               "-R",        "propose only required presentation contexts\n(default: propose all supported)");
      cmd.addOption("--combine",                "+C",        "combine proposed transfer syntaxes\n(default: separate pres. context for each TS)");
    cmd.addSubGroup("post-1993 value representations:");
      cmd.addOption("--enable-new-vr",          "+u",        "enable support for new VRs (UN/UT) (default)");
      cmd.addOption("--disable-new-vr",         "-u",        "disable support for new VRs, convert to OB");
    cmd.addSubGroup("other network options:");
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
      cmd.addOption("--repeat",                           1,  "[n]umber: integer", "repeat n times");
      cmd.addOption("--abort",                                "abort association instead of releasing it");
      cmd.addOption("--no-halt",                              "do not halt if unsuccessful store encountered\n(default: do halt)");
      cmd.addOption("--invent-instance",        "+II",        "invent a new SOP instance UID for every image sent");
      OFString opt5 = "invent a new series UID after n images\nhave been sent (default: ";
      sprintf(tempstr, "%ld", (long)opt_inventSeriesCount);
      opt5 += tempstr;
      opt5 += ")";
      cmd.addOption("--invent-series",          "+IR",   1,  "[n]umber: integer (implies --invent-instance)", opt5.c_str());
      OFString opt6 = "invent a new study UID after n series\nhave been sent (default: ";
      sprintf(tempstr, "%ld", (long)opt_inventStudyCount);
      opt6 += tempstr;
      opt6 += ")";
      cmd.addOption("--invent-study",           "+IS",   1,  "[n]umber: integer (implies --invent-instance)", opt6.c_str());
      OFString opt7 = "invent a new patient ID and name after n studies\nhave been sent (default: ";
      sprintf(tempstr, "%ld", (long)opt_inventPatientCount);
      opt7 += tempstr;
      opt7 += ")";
      cmd.addOption("--invent-patient",         "+IP",   1,  "[n]umber: integer (implies --invent-instance)", opt7.c_str());

    /* evaluate command line */                           
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
    {
      /* check for --help first */
      if (cmd.findOption("--help")) app.printUsage();

      cmd.getParam(1, opt_peer);
      app.checkParam(cmd.getParam(2, opt_port, 1, (OFCmdUnsignedInt)65535));

      if (cmd.findOption("--verbose")) opt_verbose=OFTrue;
      if (cmd.findOption("--verbose-pc")) 
      {
      	opt_verbose=OFTrue;
      	opt_showPresentationContexts=OFTrue;
      }
      if (cmd.findOption("--debug")) 
      {
      	opt_debug = OFTrue;
        DUL_Debug(OFTrue);
        DIMSE_debug(OFTrue);
      	SetDebugLevel(3);
      }
      if (cmd.findOption("--aetitle")) app.checkValue(cmd.getValue(opt_ourTitle));
      if (cmd.findOption("--call")) app.checkValue(cmd.getValue(opt_peerTitle));

      cmd.beginOptionBlock();
      if (cmd.findOption("--propose-uncompr"))  opt_networkTransferSyntax = EXS_Unknown;
      if (cmd.findOption("--propose-little"))   opt_networkTransferSyntax = EXS_LittleEndianExplicit;
      if (cmd.findOption("--propose-big"))      opt_networkTransferSyntax = EXS_BigEndianExplicit;
      if (cmd.findOption("--propose-implicit")) opt_networkTransferSyntax = EXS_LittleEndianImplicit;
      if (cmd.findOption("--propose-lossless")) opt_networkTransferSyntax = EXS_JPEGProcess14SV1TransferSyntax;
      if (cmd.findOption("--propose-jpeg8"))    opt_networkTransferSyntax = EXS_JPEGProcess1TransferSyntax;
      if (cmd.findOption("--propose-jpeg12"))   opt_networkTransferSyntax = EXS_JPEGProcess2_4TransferSyntax;
      if (cmd.findOption("--propose-rle"))      opt_networkTransferSyntax = EXS_RLELossless;
      cmd.endOptionBlock();

      if (cmd.findOption("--required")) opt_proposeOnlyRequiredPresentationContexts = OFTrue;
      if (cmd.findOption("--combine")) opt_combineProposedTransferSyntaxes = OFTrue;

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

      if (cmd.findOption("--max-pdu")) app.checkValue(cmd.getValue(opt_maxReceivePDULength, ASC_MINIMUMPDUSIZE, (OFCmdUnsignedInt)ASC_MAXIMUMPDUSIZE));
      if (cmd.findOption("--repeat"))  app.checkValue(cmd.getValue(opt_repeatCount, (OFCmdUnsignedInt)1));
      if (cmd.findOption("--abort"))   opt_abortAssociation = OFTrue;
      if (cmd.findOption("--no-halt")) opt_haltOnUnsuccessfulStore = OFFalse;
      if (cmd.findOption("--invent-instance")) opt_inventSOPInstanceInformation = OFTrue;
      if (cmd.findOption("--invent-series")) 
      {
      	opt_inventSOPInstanceInformation = OFTrue;
      	app.checkValue(cmd.getValue(opt_inventSeriesCount, (OFCmdUnsignedInt)1));
      }
      if (cmd.findOption("--invent-study")) 
      {
      	opt_inventSOPInstanceInformation = OFTrue;
      	app.checkValue(cmd.getValue(opt_inventStudyCount, (OFCmdUnsignedInt)1));
      }
      if (cmd.findOption("--invent-patient")) 
      {
      	opt_inventSOPInstanceInformation = OFTrue;
      	app.checkValue(cmd.getValue(opt_inventPatientCount, (OFCmdUnsignedInt)1));
      }
      
      /* finally parse filenames */
      int paramCount = cmd.getParamCount();
      const char *currentFilename = NULL;
      OFString errormsg;
      char sopClassUID[128];
      char sopInstanceUID[128];

      for (int i=3; i <= paramCount; i++)
      {
      	cmd.getParam(i, currentFilename);
        if (access(currentFilename, R_OK) < 0)
        {
          errormsg = "cannot access file: ";
          errormsg += currentFilename;
          app.printError(errormsg.c_str());
        }
        if (opt_proposeOnlyRequiredPresentationContexts)
        {
            if (!DU_findSOPClassAndInstanceInFile(currentFilename, sopClassUID, sopInstanceUID))
            {
              errormsg = "missing SOP class (or instance) in file: ";
              errormsg += currentFilename;
              app.printError(errormsg.c_str());
            } 
            else if (!dcmIsaStorageSOPClassUID(sopClassUID))
            {
              errormsg = "unknown storage sop class in file: ";
              errormsg += currentFilename;
              errormsg += ": ";
              errormsg += sopClassUID;
              app.printError(errormsg.c_str());
            }
            else
            {
              sopClassUIDList.push_back(sopClassUID);
              sopInstanceUIDList.push_back(sopInstanceUID);
            }
        }
        fileNameList.push_back(currentFilename);
      }
   }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded()) {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
		DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 1000, &net);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }
    cond = ASC_createAssociationParameters(&params, opt_maxReceivePDULength);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }
    ASC_setAPTitles(params, opt_ourTitle, opt_peerTitle, NULL);

    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", opt_peer, (int)opt_port);
    ASC_setPresentationAddresses(params, localHost, peerHost);

    cond = addStoragePresentationContexts(params, sopClassUIDList);        
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }
    if (opt_showPresentationContexts || opt_debug) {
	printf("Request Parameters:\n");
	ASC_dumpParameters(params);
    }

    /* create association */
    if (opt_verbose)
	printf("Requesting Association\n");
    cond = ASC_requestAssociation(net, params, &assoc);
    if (cond != ASC_NORMAL) {
	if (cond == ASC_ASSOCIATIONREJECTED) {
	    T_ASC_RejectParameters rej;

	    ASC_getRejectParameters(params, &rej);
	    errmsg("Association Rejected:");
	    ASC_printRejectParameters(stderr, &rej);
	    exit(1);
	} else {
	    errmsg("Association Request Failed:");
	    COND_DumpConditions();
	    exit(1);
	}
    }
    /* what has been accepted/refused ? */
    if (opt_showPresentationContexts || opt_debug) {
	printf("Association Parameters Negotiated:\n");
	ASC_dumpParameters(params);
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
    cond = DIMSE_NORMAL;
    OFListIterator(OFString) iter = fileNameList.begin();
    OFListIterator(OFString) enditer = fileNameList.end();
    
    while ((iter != enditer) && (cond == DIMSE_NORMAL))
    {
        cond = cstore(assoc, *iter);
        ++iter;
    }

    /* tear down association */
    switch (cond) {
    case DIMSE_NORMAL:
	if (opt_abortAssociation) {
	    if (opt_verbose)
		printf("Aborting Association\n");
	    cond = ASC_abortAssociation(assoc);
	    if (!SUCCESS(cond)) {
		errmsg("Association Abort Failed:");
		COND_DumpConditions();
		exit(1);
	    }
	} else {
	    /* release association */
	    if (opt_verbose)
		printf("Releasing Association\n");
	    cond = ASC_releaseAssociation(assoc);
	    if (cond != ASC_NORMAL && cond != ASC_RELEASECONFIRMED) {
		errmsg("Association Release Failed:");
		COND_DumpConditions();
		exit(1);
	    }
	}
	break;
    case DIMSE_PEERREQUESTEDRELEASE:
	errmsg("Protocol Error: peer requested release (Aborting)");
	if (opt_verbose)
	    printf("Aborting Association\n");
	cond = ASC_abortAssociation(assoc);
	if (!SUCCESS(cond)) {
	    errmsg("Association Abort Failed:");
	    COND_DumpConditions();
	    exit(1);
	}
	break;
    case DIMSE_PEERABORTEDASSOCIATION:
	if (opt_verbose) printf("Peer Aborted Association\n");
	break;
    default:
	errmsg("SCU Failed:");
	COND_DumpConditions();
	if (opt_verbose)
	    printf("Aborting Association\n");
	cond = ASC_abortAssociation(assoc);
	if (!SUCCESS(cond)) {
	    errmsg("Association Abort Failed:");
	    COND_DumpConditions();
	    exit(1);
	}
	break;
    }

    cond = ASC_destroyAssociation(&assoc);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }
    cond = ASC_dropNetwork(&net);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }
    if (opt_debug) {
	/* are there any conditions sitting on the condition stack? */
	char buf[BUFSIZ];
	CONDITION c;

	if (COND_TopCondition(&c, buf, BUFSIZ) != COND_NORMAL) {
	    fprintf(stderr, "CONDITIONS Remaining\n");
	    COND_DumpConditions();
	}
    }

#ifdef HAVE_WINSOCK_H
    WSACleanup();
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

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif    
    return exitCode;
}


static OFBool
isaListMember(OFList<OFString>& list, OFString& s)
{
    OFListIterator(OFString) cur = list.begin();
    OFListIterator(OFString) end = list.end();

    OFBool found = OFFalse;

    while (cur != end && !found) {

        found = (s == *cur);
        
        ++cur;
    }

    return found;
}

static CONDITION
addPresentationContext(T_ASC_Parameters *params, 
    int presentationContextId, const OFString& abstractSyntax,
    const OFString& transferSyntax, 
    T_ASC_SC_ROLE proposedRole = ASC_SC_ROLE_DEFAULT)
{
    CONDITION cond = ASC_NORMAL;

    const char* c_p = transferSyntax.c_str();
    cond = ASC_addPresentationContext(params, presentationContextId, 
        abstractSyntax.c_str(), &c_p, 1, proposedRole);
    return cond;
}

static CONDITION
addPresentationContext(T_ASC_Parameters *params, 
    int presentationContextId, const OFString& abstractSyntax,
    const OFList<OFString>& transferSyntaxList, 
    T_ASC_SC_ROLE proposedRole = ASC_SC_ROLE_DEFAULT)
{
    CONDITION cond = ASC_NORMAL;

    // create an array of supported/possible transfer syntaxes
    const char** transferSyntaxes = new const char*[transferSyntaxList.size()];
    int transferSyntaxCount = 0;
    OFListIterator(OFString) s_cur = transferSyntaxList.begin();
    OFListIterator(OFString) s_end = transferSyntaxList.end();
    while (s_cur != s_end) {
        transferSyntaxes[transferSyntaxCount++] = (*s_cur).c_str();
        ++s_cur;
    }

    cond = ASC_addPresentationContext(params, presentationContextId, 
        abstractSyntax.c_str(), transferSyntaxes, transferSyntaxCount, proposedRole);

    delete[] transferSyntaxes;
    return cond;
}

static CONDITION
addStoragePresentationContexts(T_ASC_Parameters *params, OFList<OFString>& sopClasses)
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
    fallbackSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
    fallbackSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
    fallbackSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
    // Remove the preferred syntax from the fallback list
    fallbackSyntaxes.remove(preferredTransferSyntax);
    // If little endian implicit is preferred then we don't need any fallback syntaxes
    // because it is the default transfer syntax and all applications must support it.
    if (opt_networkTransferSyntax == EXS_LittleEndianImplicit) {
        fallbackSyntaxes.clear();
    }

    // created a list of transfer syntaxes combined from the preferred and fallback syntaxes
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
        // add all the known storage sop classes to the list
        // the array of Storage SOP Class UIDs comes from dcuid.h
        for (int i=0; i<numberOfDcmStorageSOPClassUIDs; i++) {
            sopClasses.push_back(dcmStorageSOPClassUIDs[i]);
        }
    }

    // thin out the sop classes to remove any duplicates.
    OFList<OFString> sops;
    s_cur = sopClasses.begin();
    s_end = sopClasses.end();
    while (s_cur != s_end) {
        if (!isaListMember(sops, *s_cur)) {
            sops.push_back(*s_cur);
        }
        ++s_cur;
    }

    // add a presentations context for each sop class / transfer syntax pair
    CONDITION cond = ASC_NORMAL;
    int pid = 1; // presentation context id
    s_cur = sops.begin();
    s_end = sops.end();
    while (s_cur != s_end && SUCCESS(cond)) {

        if (pid > 255) {
    	    errmsg("Too many presentation contexts");
	    return ASC_BADPRESENTATIONCONTEXTID;
        }

        if (opt_combineProposedTransferSyntaxes) {
            cond = addPresentationContext(params, pid, *s_cur, combinedSyntaxes);
	    pid += 2;	/* only odd presentation context id's */
        } else {

            // sop class with preferred transfer syntax
            cond = addPresentationContext(params, pid, *s_cur, preferredTransferSyntax);
	    pid += 2;	/* only odd presentation context id's */

            if (fallbackSyntaxes.size() > 0) {
                if (pid > 255) {
                    errmsg("Too many presentation contexts");
	            return ASC_BADPRESENTATIONCONTEXTID;
                }

                // sop class with fallback transfer syntax
                cond = addPresentationContext(params, pid, *s_cur, fallbackSyntaxes);
	        pid += 2;	/* only odd presentation context id's */
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
    return (int)t;
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
    OFString uid = dcmGenerateUniqueIdentifer(uidbuf, prefix.c_str());
    return uid;
}

static OFBool
updateStringAttributeValue(DcmItem* dataset, const DcmTagKey& key, OFString& value)
{
    DcmStack stack;
    DcmTag tag(key);

    E_Condition cond = EC_Normal;
    cond = dataset->search(key, stack, ESM_fromHere, OFFalse);
    if (cond != EC_Normal) {
	CERR << "error: updateStringAttributeValue: cannot find: " << tag.getTagName() 
	     << " " << key << ": "
	     << dcmErrorConditionToString(cond) << endl;
        return OFFalse;
    }

    DcmElement* elem = (DcmElement*) stack.top();
    
    DcmVR vr(elem->ident());
    if (elem->getLength() > vr.getMaxValueLength()) {
	CERR << "error: updateStringAttributeValue: INTERNAL ERROR: " << tag.getTagName() 
	     << " " << key << ": value too large (max "
	    << vr.getMaxValueLength() << ") for " << vr.getVRName() << " value: " << value << endl;
        return OFFalse;
    }
    
    cond = elem->putOFStringArray(value);
    if (cond != EC_Normal) {
	CERR << "error: updateStringAttributeValue: cannot put string in attribute: " << tag.getTagName() 
	     << " " << key << ": "
	     << dcmErrorConditionToString(cond) << endl;
        return OFFalse;
    }

    return OFTrue;
}

static void
replaceSOPInstanceInformation(DcmDataset* dataset)
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

    if (seriesInstanceUID.length() == 0) seriesInstanceUID=makeUID(SITE_SERIES_UID_ROOT, (int)seriesCounter);
    if (seriesNumber.length() == 0) seriesNumber = intToString((int)seriesCounter);
    if (studyInstanceUID.length() == 0) studyInstanceUID = makeUID(SITE_STUDY_UID_ROOT, (int)studyCounter);
    if (studyID.length() == 0) studyID = studyIDPrefix + intToString((int)secondsSince1970()) + intToString((int)studyCounter);
    if (accessionNumber.length() == 0) accessionNumber = accessionNumberPrefix + intToString(secondsSince1970()) + intToString((int)studyCounter);
    if (patientID.length() == 0) patientID = patientIDPrefix + intToString(secondsSince1970()) + intToString((int)patientCounter);
    if (patientName.length() == 0) patientName = patientNamePrefix + intToString(secondsSince1970()) + intToString((int)patientCounter);

    if (imageCounter >= opt_inventSeriesCount) {
        imageCounter = 0;
        seriesCounter++;
        seriesInstanceUID = makeUID(SITE_SERIES_UID_ROOT, (int)seriesCounter);
        seriesNumber = intToString((int)seriesCounter);
    }
    if (seriesCounter >= opt_inventStudyCount) {
        seriesCounter = 0;
        studyCounter++;
        studyInstanceUID = makeUID(SITE_STUDY_UID_ROOT, (int)studyCounter);
        studyID = studyIDPrefix + intToString(secondsSince1970()) + intToString((int)studyCounter);
        accessionNumber = accessionNumberPrefix + intToString(secondsSince1970()) + intToString((int)studyCounter);
    }
    if (studyCounter >= opt_inventPatientCount) {
        // we create as many patients as necessary */
        studyCounter = 0;
        patientCounter++;
        patientID = patientIDPrefix + intToString(secondsSince1970()) + intToString((int)patientCounter);
        patientName = patientNamePrefix + intToString(secondsSince1970()) + intToString((int)patientCounter);
    }

    OFString sopInstanceUID = makeUID(SITE_INSTANCE_UID_ROOT, (int)imageCounter);
    OFString imageNumber = intToString((int)imageCounter);

    if (opt_verbose) {
        COUT << "Inventing Identifying Information (" << 
            "pa" << patientCounter << ", st" << studyCounter << 
            ", se" << seriesCounter << ", im" << imageCounter << "): " << endl;
        COUT << "  PatientName=" << patientName << endl;
        COUT << "  PatientID=" << patientID << endl;
        COUT << "  StudyInstanceUID=" << studyInstanceUID << endl;
        COUT << "  StudyID=" << studyID << endl;
        COUT << "  SeriesInstanceUID=" << seriesInstanceUID << endl;
        COUT << "  SeriesNumber=" << seriesNumber << endl;
        COUT << "  SOPInstanceUID=" << sopInstanceUID << endl;
        COUT << "  ImageNumber=" << imageNumber << endl;
    }

    updateStringAttributeValue(dataset, DCM_PatientsName, patientName);
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
    T_DIMSE_C_StoreRQ * /*req*/)
{
    if (opt_verbose) {
        switch (progress->state) {
	case DIMSE_StoreBegin:	
	    printf("XMIT:"); break;
	case DIMSE_StoreEnd:
	    printf("\n"); break;
	default:
	    putchar('.'); break;
	}
        fflush(stdout);
    }
}

static CONDITION 
storeSCU(T_ASC_Association * assoc, const char *fname)
{
    CONDITION cond;
    DIC_US msgId = assoc->nextMsgID++;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_StoreRQ req;
    T_DIMSE_C_StoreRSP rsp;
    DIC_UI sopClass;
    DIC_UI sopInstance;
    DcmDataset *statusDetail = NULL;

    unsuccessfulStoreEncountered = OFTrue; // assumption 

    if (opt_verbose) {
        printf("--------------------------\n");
        printf("Sending file: %s\n", fname);
    }

    DcmFileStream inf(fname, DCM_ReadMode);
    if ( inf.Fail() ) {
	errmsg("Cannot open file: %s: %s", fname, strerror(errno));
	return DIMSE_BADDATA;
    }

    DcmFileFormat dcmff;
    dcmff.transferInit();
    dcmff.read(inf, EXS_Unknown);
    dcmff.transferEnd();

    if (dcmff.error() != EC_Normal) {
	errmsg("Bad DICOM file: %s: %s", fname, 
	       dcmErrorConditionToString(dcmff.error()));
	return DIMSE_BADDATA;
    }

    if (opt_inventSOPInstanceInformation) {
        replaceSOPInstanceInformation(dcmff.getDataset());
    }

    /* which SOP class and SOP instance ? */
    if (!DU_findSOPClassAndInstanceInDataSet(dcmff.getDataset(), 
	sopClass, sopInstance)) {
	errmsg("No SOP Class & Instance UIDs in file: %s", fname);
	return DIMSE_BADDATA;
    }

    /* which presentation context should be used */
    DcmXfer filexfer(dcmff.getDataset()->getOriginalXfer());
    if (filexfer.getXfer() != EXS_Unknown) presId = ASC_findAcceptedPresentationContextID(assoc, sopClass, filexfer.getXferID());
    else presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    if (presId == 0) {
        const char *modalityName = dcmSOPClassUIDToModality(sopClass);
        if (!modalityName) modalityName = dcmFindNameOfUID(sopClass);
        if (!modalityName) modalityName = "unknown SOP class";
	errmsg("No presentation context for: (%s) %s", modalityName, sopClass);
	return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    if (opt_verbose) {
        DcmXfer fileTransfer(dcmff.getDataset()->getOriginalXfer());
        T_ASC_PresentationContext pc;
        ASC_findAcceptedPresentationContext(assoc->params, presId, &pc);
        DcmXfer netTransfer(pc.acceptedTransferSyntax);
        printf("Transfer: %s -> %s\n", 
            dcmFindNameOfUID(fileTransfer.getXferID()), dcmFindNameOfUID(netTransfer.getXferID()));
    }

    bzero((char*)&req, sizeof(req));
    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, sopClass);
    strcpy(req.AffectedSOPInstanceUID, sopInstance);
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

    if (opt_verbose) {
	printf("Store SCU RQ: MsgID %d, (%s)\n", msgId, dcmSOPClassUIDToModality(sopClass));
    }

    cond = DIMSE_storeUser(assoc, presId, &req,
        NULL, dcmff.getDataset(), progressCallback, NULL, 
	DIMSE_BLOCKING, 0, 
	&rsp, &statusDetail);

    /*
     * If store command completed normally, with a status 
     * of success or some warning then the image was accepted.
     */
    if (cond == DIMSE_NORMAL && (rsp.DimseStatus == STATUS_Success || DICOM_WARNING_STATUS(rsp.DimseStatus))) {
        unsuccessfulStoreEncountered = OFFalse;
    }

    lastStatusCode = rsp.DimseStatus;
		
    if (cond == DIMSE_NORMAL) {
        if (opt_verbose) {
	    DIMSE_printCStoreRSP(stdout, &rsp);
        }
    } else {
        errmsg("Store Failed, file: %s:", fname);
	COND_DumpConditions();
    }
    if (statusDetail != NULL) {
        printf("  Status Detail:\n");
	statusDetail->print(COUT);
	delete statusDetail;
    }
    return cond;
}


static CONDITION
cstore(T_ASC_Association * assoc, const OFString& fname)
{
    CONDITION cond = DIMSE_NORMAL;
    int n = (int)opt_repeatCount;

    while ((cond == DIMSE_NORMAL) && n-- && !(opt_haltOnUnsuccessfulStore && unsuccessfulStoreEncountered)) {
	cond = storeSCU(assoc, fname.c_str());
    }
    return cond;
}

/*
** CVS Log
** $Log: storescu.cc,v $
** Revision 1.33  2000-04-14 16:29:27  meichel
** Removed default value from output stream passed to print() method.
**   Required for use in multi-thread environments.
**
** Revision 1.32  2000/03/08 16:43:17  meichel
** Updated copyright header.
**
** Revision 1.31  2000/03/03 14:11:12  meichel
** iImplemented library support for redirecting error messages into memory
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

