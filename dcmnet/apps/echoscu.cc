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
** Test program for Verification Service Class (C-ECHO operation)
**
** Author: 	Andrew Hewett
**		Kuratorium OFFIS e.V., Oldenburg, Germany
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-09-24 16:21:48 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/apps/echoscu.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#include "dimse.h"
#include "diutil.h"
#include "dcfilefo.h"
#include "dcdebug.h"
#include "dcdict.h"
#include "dcuid.h"
#include "cmdlnarg.h"

#define PATHSEPARATOR PATH_SEPARATOR	/* via osconfig.h" */

/* default application titles */
#define APPLICATIONTITLE	"ECHOSCU"
#define PEERAPPLICATIONTITLE	"ANY-SCP"

static char *progname = NULL;
static BOOLEAN verbose = FALSE;
static BOOLEAN debug = FALSE;
static BOOLEAN abortAssociation = FALSE;
static int maxReceivePDULength = ASC_DEFAULTMAXPDU;
static int repeatCount = 1;

static void
shortusage()
{
    fprintf(stderr, "usage: %s [-r n][-v][-d][-a][-b n][-t ourAETitle]"
	    "[-c theirAETitle] peer port\n",
	    progname);
}

static void
fullusage()
{
    shortusage();
    fprintf(stderr, "\
parameters:\n\
    peer	hostname of dicom peer\n\
    port	tcp/ip port number of peer\n\
options:\n\
    -r n	repeat n times\n\
    -v		verbose mode\n\
    -d		debug mode\n\
    -a		abort association\n\
    -b n	set max receive pdu to n bytes (default: %d)\n\
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

static CONDITION cecho(T_ASC_Association *assoc);


// ********************************************

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
    int i;
    T_ASC_Association *assoc;
    char *peerTitle = PEERAPPLICATIONTITLE;
    char *ourTitle = APPLICATIONTITLE;

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

    prepareCmdLineArgs(argc, argv, "echoscu");

    /* strip any leading path from program name */
    if ((progname = (char*)strrchr(argv[0], PATHSEPARATOR)) != NULL) {
	progname++;
    } else {
	progname = argv[0];
    }

    if (argc < 3) {
	fullusage();
    }
    /* parse program arguments */
    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
	switch (argv[i][1]) {
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

    if (argc - i < 2)
	usage();

    /* peer to call */
    peer = argv[i];
    i++;

    /* get port number to call */
    if (sscanf(argv[i], "%d", &port) != 1) {
	errmsg("bad port number: %s", argv[i]);
	usage();
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

    const char* transferSyntaxes[] = {UID_LittleEndianImplicitTransferSyntax};

    cond = ASC_addPresentationContext(
	params,
	1, UID_VerificationSOPClass,
	transferSyntaxes, DIM_OF(transferSyntaxes));

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
    cond = cecho(assoc);

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
    return 0;
}

static CONDITION 
echoSCU(T_ASC_Association * assoc)
{
    CONDITION cond;
    DIC_US msgId = assoc->nextMsgID++;
    DIC_US status;
    DcmDataset *statusDetail = NULL;

    if (verbose) {
	printf("Echo [%d], ", msgId);
	fflush(stdout);
    }

    cond = DIMSE_echoUser(assoc, msgId, DIMSE_BLOCKING, 0,
    	&status, &statusDetail);

    if (cond == DIMSE_NORMAL) {
        if (verbose) {
	    printf("Complete [Status: %s]\n",
	        DU_cstoreStatusString(status));
        }
    } else {
        errmsg("Failed:");
	COND_DumpConditions();
    }
    if (statusDetail != NULL) {
        printf("  Status Detail (should never be any):\n");
	statusDetail->print();
        delete statusDetail;
    }
    return cond;
}

static CONDITION
cecho(T_ASC_Association * assoc)
{
    CONDITION cond = DIMSE_NORMAL;
    int n = repeatCount;

    while (cond == DIMSE_NORMAL && n--) {
	cond = echoSCU(assoc);
    }
    return cond;
}

/*
** CVS Log
** $Log: echoscu.cc,v $
** Revision 1.4  1996-09-24 16:21:48  hewett
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
**
*/
