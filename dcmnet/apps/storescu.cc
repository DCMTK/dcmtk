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
** Test program for Storage Service Class (C-STORE operation)
**
** Author: 	Andrew Hewett
**		Kuratorium OFFIS e.V., Oldenburg, Germany
**
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-09-27 08:24:30 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/apps/storescu.cc,v $
** CVS/RCS Revision:	$Revision: 1.6 $
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


#define PATHSEPARATOR PATH_SEPARATOR /* via osconfig.h */


/* default application titles */
#define APPLICATIONTITLE	"STORESCU"
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
    fprintf(stderr, "\
usage: %s [-r n][-v][-d][-a][-b n][-t ourAETitle][-c theirAETitle]\n\
          peer port file ...\n",
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
    file	dicom image data set\n\
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


static CONDITION
addAllStoragePresentationContexts(T_ASC_Parameters *params);

static CONDITION 
cstore(T_ASC_Association *assoc, const char* fname);


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

    prepareCmdLineArgs(argc, argv, "storescu");

    /* strip any leading path from program name */
    if ((progname = (char*)strrchr(argv[0], PATHSEPARATOR)) != NULL) {
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

    if (argc - i < 2) {
	errmsg("port number and filename(s) missing");
	usage();
    }

    /* peer to call */
    peer = argv[i];
    i++;

    /* get port number to call */
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

    cond = addAllStoragePresentationContexts(params);
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
    for (j=i; j<argc && cond==DIMSE_NORMAL; j++) {
        cond = cstore(assoc, argv[j]);
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
    return 0;
}

static CONDITION
addAllStoragePresentationContexts(T_ASC_Parameters *params)
{
    CONDITION cond = ASC_NORMAL;
    int i;
    int pid = 1;

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

    /* the array of Storage SOP Class UIDs comes from dcuid.h */
    for (i=0; i<numberOfDcmStorageSOPClassUIDs && SUCCESS(cond); i++) {
	cond = ASC_addPresentationContext(
	    params, pid, dcmStorageSOPClassUIDs[i],
	    transferSyntaxes, DIM_OF(transferSyntaxes));
	pid += 2;	/* only odd presentation context id's */
    }

    return cond;
}

static void
progressCallback(void * /*callbackData*/, 
    T_DIMSE_StoreProgress *progress,
    T_DIMSE_C_StoreRQ * /*req*/)
{
    if (verbose) {
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

    /* which SOP class and SOP instance ? */
    if (!DU_findSOPClassAndInstanceInDataSet(dcmff.getDataset(), 
	sopClass, sopInstance)) {
	errmsg("No SOP Class & Instance UIDs in file: %s", fname);
	return DIMSE_BADDATA;
    }

    /* which presentation context should be used */
    presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    if (presId == 0) {
	errmsg("No presentation context for: (%s) %s", 
	    DU_sopClassToModality(sopClass), sopClass);
	return DIMSE_NOVALIDPRESENTATIONCONTEXTID;
    }

    bzero((char*)&req, sizeof(req));
    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, sopClass);
    strcpy(req.AffectedSOPInstanceUID, sopInstance);
    req.DataSetType = DIMSE_DATASET_PRESENT;
    req.Priority = DIMSE_PRIORITY_LOW;

    if (verbose) {
	printf("Store SCU RQ: MsgID %d, (%s)\n", 
	    msgId, DU_sopClassToModality(sopClass));
    }

    cond = DIMSE_storeUser(assoc, presId, &req,
        NULL, dcmff.getDataset(), progressCallback, NULL, 
	DIMSE_BLOCKING, 0, 
	&rsp, &statusDetail);
	
	
    if (cond == DIMSE_NORMAL) {
        if (verbose) {
	    DIMSE_printCStoreRSP(stdout, &rsp);
        }
    } else {
        errmsg("Store Failed, file: %s:", fname);
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
cstore(T_ASC_Association * assoc, const char *fname)
{
    CONDITION cond = DIMSE_NORMAL;
    int n = repeatCount;

    while (cond == DIMSE_NORMAL && n--) {
	cond = storeSCU(assoc, fname);
    }
    return cond;
}

/*
** CVS Log
** $Log: storescu.cc,v $
** Revision 1.6  1996-09-27 08:24:30  hewett
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
**
*/
