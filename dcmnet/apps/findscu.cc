/*
**
**  Copyright (C) 1996, OFFIS
**
**  This software and supporting documentation were developed by
**
**    Kuratorium OFFIS e.V.
**    Forschungsbereich 2: Kommunikationssysteme
**    Escherweg 2
**    D-26121 Oldenburg, Germany
**
**  for CEN/TC251/WG4 as a contribution to the Computer Assisted Radiology
**  (CAR) 1996 DICOM Demonstration.
**
**  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
**  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
**  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
**  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
**  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
**
**  Copyright of the software  and  supporting  documentation  is,  unless
**  otherwise stated, owned by OFFIS, and free access is hereby granted as
**  a license to  use  this  software,  copy  this  software  and  prepare
**  derivative works based upon this software.  However, any  distribution
**  of this software source code or supporting documentation or derivative
**  works  (source code and  supporting documentation)  must  include  the
**  three paragraphs of this copyright notice.
**
*/

/*
** Test program for Query (DIMSE C-FIND operation)
**
** Author: 	Andrew Hewett
**		Kuratorium OFFIS e.V., Oldenburg, Germany
** Created:	03/96
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-04-18 08:40:11 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/apps/findscu.cc,v $
** CVS/RCS Revision:	$Revision: 1.10 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/


#include "osconfig.h" /* make sure OS specific configuration is included first */

BEGIN_EXTERN_C
#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
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

#include "dimse.h"
#include "diutil.h"
#include "dcfilefo.h"
#include "dcdebug.h"
#include "dcuid.h"
#include "dcdict.h"
#include "cmdlnarg.h"


/* default application titles */
#define APPLICATIONTITLE	"FINDSCU"
#define PEERAPPLICATIONTITLE	"ANY-SCP"


static char *progname = NULL;
static BOOLEAN verbose = FALSE;
static BOOLEAN debug = FALSE;
static BOOLEAN abortAssociation = FALSE;
static int maxReceivePDULength = ASC_DEFAULTMAXPDU;
static int repeatCount = 1;

static char *abstractSyntax = UID_FINDModalityWorklistInformationModel;

static DcmDataset *overrideKeys = NULL;


static void
shortusage()
{
    fprintf(stderr, 
"usage: %s [-k key][-u][-r n][-v][-d][-a][-b n][-P][-S][-O][-W]\n\
	[-t ourAETitle][-c theirAETitle]\n\
          peer port [file ...]\n",
	progname);
}

static void
fullusage()
{
    shortusage();
    fprintf(stderr,
"parameters:\n\
    peer	hostname of dicom peer\n\
    port	tcp/ip port number of peer\n\
    file	query data set\n\
options:\n\
    -u          disable generation of unknown VR (UN)\n\
    -k key      override matching key (gggg,eeee=\"string\")\n\
    -r n	repeat n times\n\
    -v		verbose mode\n\
    -d		debug mode\n\
    -a		abort association\n\
    -b n	set max receive pdu to n bytes (default: %d)\n\
    -P		use Patient Root Q/R Information Model\n\
    -S		use Study Root Q/R Information Model\n\
    -O		use Patient/Study Only Root Q/R Information Model\n\
    -W		use Modality Worklist Information Model (default)\n\
    -t title	my calling AE title (default: %s)\n\
    -c title	called AE title of peer (default: %s)\n",
    maxReceivePDULength, APPLICATIONTITLE, PEERAPPLICATIONTITLE);
    exit(1);
}

static void 
usage()
{
    shortusage();
    exit(1);
}

static void 
errmsg(const char *msg,...)
{
    va_list args;

    fprintf(stderr, "%s: ", progname);
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}


static void
addOverrideKey(char* s)
{
    unsigned int g = 0xffff;
    unsigned int e = 0xffff;
    int n = 0;
    char val[1024];

    val[0] = '\0';
    n = sscanf(s, "%x,%x=%s", &g, &e, val);
    
    if (n < 2) {
	errmsg("bad key format: %s", s);
	usage(); /* does not return */
    }

    DcmTag tag(g,e);
    if (tag.error() != EC_Normal) {
	errmsg("unknown tag: (%04x,%04x)", g, e);
	usage();
    }
    DcmElement *elem = newDicomElement(tag);
    if (elem == NULL) {
	errmsg("cannot create element for tag: (%04x,%04x)", g, e);
	usage();
    }
    if (strlen(val) > 0) {
	elem->putString(val);
	if (elem->error() != EC_Normal) {
	    errmsg("cannot put tag value: (%04x,%04x)=\"%s\"", g, e, val);
	    usage();
	}
    }

    if (overrideKeys == NULL) {
	overrideKeys = new DcmDataset;
    }

    overrideKeys->insert(elem, TRUE);
    if (overrideKeys->error() != EC_Normal) {
	errmsg("cannot insert tag: (%04x,%04x)", g, e);
	usage();
    }
}

static CONDITION
addPresentationContexts(T_ASC_Parameters *params);

static CONDITION 
cfind(T_ASC_Association *assoc, const char* fname);


int
main(int argc, char *argv[])
{
    CONDITION cond;
    T_ASC_Network *net;
    T_ASC_Parameters *params;
    char *peer;
    int port = 104;
    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;
    int i, j;
    T_ASC_Association *assoc;
    char *peerTitle = PEERAPPLICATIONTITLE;
    char *ourTitle = APPLICATIONTITLE;

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.0 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 0 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

    prepareCmdLineArgs(argc, argv, "findscu");

    SetDebugLevel((0));	/* stop dcmdata debugging messages */

    /* strip any leading path from program name */
    if ((progname = (char*)strrchr(argv[0], PATH_SEPARATOR)) != NULL) {
	progname++;
    } else {
	progname = argv[0];
    }

    if (argc < 4) {
	fullusage();
    }
    /* parse program arguments */
    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
	switch (argv[i][1]) {
	case 'u':
	    dcmEnableUnknownVRGeneration = TRUE;
	    break;
	case 'v':
	    verbose = TRUE;
	    break;
	case 'd':
	    debug = TRUE;
	    verbose = TRUE;
	    break;
	case 'a':
	    abortAssociation = TRUE;
	    break;
	case 'r':
	    if (((i + 1) < argc) && (argv[i + 1][0] != '-') &&
		(sscanf(argv[i + 1], "%d", &repeatCount) == 1)) {
		i++;		/* repeat count parsed */
	    } else {
		repeatCount = 1;
	    }
	    break;
        case 'k':
	    if ((i + 1) < argc) {
		addOverrideKey(argv[++i]);
	    } else {
		usage();
	    }
	    break;
	case 'b':
	    if (((i + 1) < argc) && 
		(sscanf(argv[i + 1], "%d", &maxReceivePDULength) == 1)) {
		i++;		/* Maximum Receive PDU Length parsed */
		if (maxReceivePDULength < ASC_MINIMUMPDUSIZE) {
		    errmsg("Maximum receive PDU length (%d) too small",
			maxReceivePDULength);
		    usage();
		} else if (maxReceivePDULength > ASC_MAXIMUMPDUSIZE) {
		    errmsg("Maximum receive PDU length (%d) too big",
			maxReceivePDULength);
		    usage();
		}
	    } else {
		usage();
	    }
	    break;
	case 'P':
	    abstractSyntax = UID_FINDPatientRootQueryRetrieveInformationModel;
	    break;
	case 'S':
	    abstractSyntax = UID_FINDStudyRootQueryRetrieveInformationModel;
	    break;
	case 'O':
	    abstractSyntax = UID_FINDPatientStudyOnlyQueryRetrieveInformationModel;
	    break;
	case 'W':
	    abstractSyntax = UID_FINDModalityWorklistInformationModel;
	    break;
	case 'c':
	    if (i++ < argc)
		peerTitle = argv[i];
	    else
		usage();
	    break;
	case 't':
	    if (i++ < argc)
		ourTitle = argv[i];
	    else
		usage();
	    break;
	default:
	    usage();
	}
    }

    if (argc - i < 2) {
	errmsg("port number and filename(s) missing");
	usage();
    }

    /* peer to call */
    peer = argv[i];
    i++;

    /* get port number call */
    if (sscanf(argv[i], "%d", &port) != 1) {
	errmsg("bad port number: %s", argv[i]);
	usage();
    }
    i++;

    for (j=i; j<argc; j++) {
	if (access(argv[j], R_OK) < 0) {
	    errmsg("cannot access file: %s", argv[j]);
	    usage();
	}
    }

    DUL_Debug(debug);
    DIMSE_debug(debug);
    SetDebugLevel(((debug)?3:0));	/* dcmdata debugging */

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
		DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 1000, &net);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }
    cond = ASC_createAssociationParameters(&params, maxReceivePDULength);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }
    ASC_setAPTitles(params, ourTitle, peerTitle, NULL);

    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", peer, port);
    ASC_setPresentationAddresses(params, localHost, peerHost);

    cond = addPresentationContexts(params);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }
    if (debug) {
	printf("Request Parameters:\n");
	ASC_dumpParameters(params);
    }

    /* create association */
    if (verbose)
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
    if (debug) {
	printf("Association Parameters Negotiated:\n");
	ASC_dumpParameters(params);
    }

    if (ASC_countAcceptedPresentationContexts(params) == 0) {
	errmsg("No Acceptable Presentation Contexts");
	exit(1);
    }

    if (verbose) {
	printf("Association Accepted (Max Send PDV: %lu)\n",
		assoc->sendPDVLength);
    }

    /* do the real work */
    cond = DIMSE_NORMAL;
    if (i == argc) {
	/* no files provided on command line */
	cond = cfind(assoc, NULL);
    } else {
	for (j=i; j<argc && cond==DIMSE_NORMAL; j++) {
	    cond = cfind(assoc, argv[j]);
	}
    }

    /* tear down association */
    switch (cond) {
    case DIMSE_NORMAL:
	if (abortAssociation) {
	    if (verbose)
		printf("Aborting Association\n");
	    cond = ASC_abortAssociation(assoc);
	    if (!SUCCESS(cond)) {
		errmsg("Association Abort Failed:");
		COND_DumpConditions();
		exit(1);
	    }
	} else {
	    /* release association */
	    if (verbose)
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
	if (verbose)
	    printf("Aborting Association\n");
	cond = ASC_abortAssociation(assoc);
	if (!SUCCESS(cond)) {
	    errmsg("Association Abort Failed:");
	    COND_DumpConditions();
	    exit(1);
	}
	break;
    case DIMSE_PEERABORTEDASSOCIATION:
	if (verbose) printf("Peer Aborted Association\n");
	break;
    default:
	errmsg("SCU Failed:");
	COND_DumpConditions();
	if (verbose)
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
    if (debug) {
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

    return 0;
}

static CONDITION
addPresentationContexts(T_ASC_Parameters *params)
{
    CONDITION cond = ASC_NORMAL;

    /* 
    ** We prefer to accept Explicitly encoded transfer syntaxes.
    ** If we are running on a Little Endian machine we prefer 
    ** LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
    ** Some SCP implementations will just select the first transfer
    ** syntax they support (this is not part of the standard) so
    ** organise the proposed transfer syntaxes to take advantage
    ** of such behaviour.
    */

    const char* transferSyntaxes[] = { 
	NULL, NULL, UID_LittleEndianImplicitTransferSyntax };

    /* gLocalByteOrder is defined in dcxfer.h */
    if (gLocalByteOrder == EBO_LittleEndian) {
	/* we are on a little endian machine */
	transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
	transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
    } else {
	/* we are on a big endian machine */
	transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
	transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
    }

    cond = ASC_addPresentationContext(
	params, 1, abstractSyntax,
	transferSyntaxes, DIM_OF(transferSyntaxes));

    return cond;
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
    int elemCount = keys.card();
    for (int i=0; i<elemCount; i++) {
	DcmElement *elem = keys.remove((unsigned long)0);

	dset->insert(elem, TRUE);
    }
}

static void
progressCallback(
	/* in */
	void */*callbackData*/, 
	T_DIMSE_C_FindRQ */*request*/, 	/* original find request */
	int responseCount, 
	T_DIMSE_C_FindRSP */*response*/,	/* pending response received */
	DcmDataset *responseIdentifiers /* pending response identifiers */
	)
{
    printf("RESPONSE: %d\n", responseCount);
    responseIdentifiers->print();
    printf("--------\n");
}

static CONDITION 
findSCU(T_ASC_Association * assoc, const char *fname)
{
    CONDITION cond;
    DIC_US msgId = assoc->nextMsgID++;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_FindRQ req;
    T_DIMSE_C_FindRSP rsp;
    DcmDataset *statusDetail = NULL;

    DcmFileFormat dcmff;

    if (fname != NULL) {
	DcmFileStream inf(fname, DCM_ReadMode);
	if ( inf.Fail() ) {
	    errmsg("Cannot open file: %s: %s", fname, strerror(errno));
	    return DIMSE_BADDATA;
	}

	dcmff.transferInit();
	dcmff.read(inf, EXS_Unknown);
	dcmff.transferEnd();

	if (dcmff.error() != EC_Normal) {
	    errmsg("Bad DICOM file: %s: %s", fname, 
		   dcmErrorConditionToString(dcmff.error()));
	    return DIMSE_BADDATA;
	}
    }

    /* replace specific keys by those in overrideKeys */
    substituteOverrideKeys(dcmff.getDataset());

    /* which presentation context should be used */
    presId = ASC_findAcceptedPresentationContextID(
	assoc, abstractSyntax);
    if (presId == 0) {
	errmsg("No presentation context"); 
	return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    bzero((char*)&req, sizeof(req));
    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, abstractSyntax);
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

    if (verbose) {
	printf("Find SCU RQ: MsgID %d\n", msgId);
	printf("Request:\n");
	dcmff.getDataset()->print();
	printf("--------\n");
    }

    cond = DIMSE_findUser(assoc, presId, &req, dcmff.getDataset(), 
			  progressCallback, NULL, 
			  DIMSE_BLOCKING, 0, 
			  &rsp, &statusDetail);
	
	
    if (cond == DIMSE_NORMAL) {
        if (verbose) {
	    DIMSE_printCFindRSP(stdout, &rsp);
        } else {
	    if (rsp.Status != STATUS_Success) {
		printf("Response: %s\n", DU_cfindStatusString(rsp.Status));
	    }
	}
    } else {
	if (fname) {
	    errmsg("Find Failed, file: %s:", fname);
	} else {
	    errmsg("Find Failed, query keys:");
	    dcmff.getDataset()->print();
	}
	COND_DumpConditions();
    }
    if (statusDetail != NULL) {
        printf("  Status Detail:\n");
	statusDetail->print();
	delete statusDetail;
    }
    return cond;
}


static CONDITION
cfind(T_ASC_Association * assoc, const char *fname)
{
    CONDITION cond = DIMSE_NORMAL;
    int n = repeatCount;

    while (cond == DIMSE_NORMAL && n--) {
	cond = findSCU(assoc, fname);
    }
    return cond;
}

/*
** CVS Log
** $Log: findscu.cc,v $
** Revision 1.10  1997-04-18 08:40:11  andreas
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   E_Condition get(Uint16 & value, const unsigned long pos);
**   becomes
**   E_Condition getUint16(Uint16 & value, const unsigned long pos);
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
