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
** Test program for Retrieve (DIMSE C-MOVE operation)
**
** Author: 	Andrew Hewett
**		Kuratorium OFFIS e.V., Oldenburg, Germany
** Created:	03/96
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1997-05-29 15:52:57 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/apps/movescu.cc,v $
** CVS/RCS Revision:	$Revision: 1.14 $
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
#include "dcuid.h"    /* for dcmtk version name */

static char rcsid[] = "$dcmtk: movescu v"
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
    char *findSyntax;
    char *moveSyntax;
} QuerySyntax;

typedef struct {
    T_ASC_Association *assoc;
    T_ASC_PresentationContextID presId;
} MyCallbackInfo;


T_ASC_Network *net = NULL; /* the global DICOM network */

static char *progname = NULL;
static BOOLEAN verbose = FALSE;
static BOOLEAN debug = FALSE;
static BOOLEAN abortAssociation = FALSE;
static int maxReceivePDULength = ASC_DEFAULTMAXPDU;
static int repeatCount = 1;
static int cancelAfterNResponses = -1;
static int retrievePort = 104;
static BOOLEAN ignoreStoreData = FALSE;
static char *moveDestination = NULL;
static QueryModel queryModel = QMPatientRoot;

static QuerySyntax querySyntax[3] = {
    { UID_FINDPatientRootQueryRetrieveInformationModel, 
      UID_MOVEPatientRootQueryRetrieveInformationModel },
    { UID_FINDStudyRootQueryRetrieveInformationModel, 
      UID_MOVEStudyRootQueryRetrieveInformationModel },
    { UID_FINDPatientStudyOnlyQueryRetrieveInformationModel, 
      UID_MOVEPatientStudyOnlyQueryRetrieveInformationModel }
};

static DcmDataset *overrideKeys = NULL;


static void
shortusage()
{
    fprintf(stderr, "\
usage: %s  [-k key][-P|-S|-O][-r n][-p portnum][-m moveDestination][-i]\n\
           [-u][-v][-d][-a][-C n][-b n][-t ourAETitle][-c theirAETitle]\n\
	   peer port queryfile ...\n",
	progname);
}

static void
fullusage()
{
    fprintf(stderr, "%s\n\n", rcsid);
    shortusage();
    fprintf(stderr, "\
parameters:\n\
    peer        hostname of dicom peer\n\
    port        tcp/ip port number of peer\n\
    queryfile   query keys file (dicom file)\n\
options:\n\
    -u          disable generation of unknown VR (UN)\n\
    -k key      override matching key (gggg,eeee=\"string\")\n\
    -P		use patient root query model (default)\n\
    -S		use study root query model\n\
    -O		use patient/study only query model\n\
    -r n	repeat n times\n\
    -p portnum	tcp/ip port number for receiving move sub-operations\n\
    -m title	move destination AE title (default is me: %s)\n\
    -v		verbose mode\n\
    -d		debug mode\n\
    -a		abort association\n\
    -i		ignore retreived images\n\
    -C n	cancel after n responses (default: never)\n\
    -b n	set max receive pdu to n bytes (default: %d)\n\
    -t title	my calling AE title (default: %s)\n\
    -c title	called AE title of peer (default: %s)\n",
    APPLICATIONTITLE, maxReceivePDULength, APPLICATIONTITLE, 
    PEERAPPLICATIONTITLE);
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

static CONDITION cmove(T_ASC_Association *assoc, const char *fname);

static CONDITION
addPresentationContext(T_ASC_Parameters *params, 
			T_ASC_PresentationContextID pid,
			const char* abstractSyntax);

int
main(int argc, char *argv[])
{
    CONDITION cond;
    T_ASC_Parameters *params = NULL;
    char *peer = NULL;
    int port;
    DIC_NODENAME localHost;
    DIC_NODENAME peerHost;
    int i, j;
    T_ASC_Association *assoc = NULL;
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

    prepareCmdLineArgs(argc, argv, "movescu");

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
	case 'P':
	    queryModel = QMPatientRoot;
	    break;
	case 'S':
	    queryModel = QMStudyRoot;
	    break;
	case 'O':
	    queryModel = QMPatientStudyOnly;
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
	case 'i':
	    ignoreStoreData = TRUE;
	    break;
	case 'r':
	    if (((i + 1) < argc) && (argv[i + 1][0] != '-') &&
		(sscanf(argv[i + 1], "%d", &repeatCount) == 1)) {
		i++;		/* repeat count parsed */
	    } else {
		repeatCount = 1;
	    }
	    break;
	case 'p':
	    if (((i + 1) < argc) && (argv[i + 1][0] != '-') &&
		(sscanf(argv[i + 1], "%d", &retrievePort) == 1)) {
		i++;		/* retrieve port parsed */
	    } else {
		usage();
	    }
	    break;
        case 'k':
	    if ((i + 1) < argc) {
		addOverrideKey(argv[++i]);
	    } else {
		usage();
	    }
	    break;
	case 'C':
	    if (((i + 1) < argc) && (argv[i + 1][0] != '-') &&
		(sscanf(argv[i + 1], "%d", &cancelAfterNResponses) == 1)) {
		i++;		/* cancelAfter count parsed */
	    } else {
		cancelAfterNResponses = -1;
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
	case 'm':
	    if (i++ < argc)
		moveDestination = argv[i];
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
    if (!dcmDataDict.isDictionaryLoaded()) {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
		DCM_DICT_ENVIRONMENT_VARIABLE);
    }


    /* if retrieve port is privileged we must be as well */
    if (retrievePort < 1024) {
        if (geteuid() != 0) {
	    errmsg("cannot listen on port %d, insufficient privileges", port);
	    usage();
	}
    }

    /* network for move request and responses */
    cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, retrievePort, 
				 1000, &net);
    if (!SUCCESS(cond)) {
	errmsg("cannot create network:");
	COND_DumpConditions();
	exit(1);
    }

    /* return to normal uid so that we can't do too much damage in case
     * things go very wrong.   Only works if the program is setuid root,
     * and run by another user.  Running as root user may be
     * potentially disasterous if this program screws up badly.
     */
    setuid(getuid());

    /* set up main association */
    cond = ASC_createAssociationParameters(&params, maxReceivePDULength);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }
    ASC_setAPTitles(params, ourTitle, peerTitle, NULL);

    gethostname(localHost, sizeof(localHost) - 1);
    sprintf(peerHost, "%s:%d", peer, port);
    ASC_setPresentationAddresses(params, localHost, peerHost);

    /*
     * We also add a presentation context for the corresponding
     * find sop class.
     */
    cond = addPresentationContext(params, 1, 
        querySyntax[queryModel].findSyntax);

    cond = addPresentationContext(params, 3, 
        querySyntax[queryModel].moveSyntax);
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
        cond = cmove(assoc, argv[j]);
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
addPresentationContext(T_ASC_Parameters *params, 
			T_ASC_PresentationContextID pid,
			const char* abstractSyntax)
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
	params, pid, abstractSyntax,
	transferSyntaxes, DIM_OF(transferSyntaxes));

    return cond;
}

static CONDITION
acceptSubAssoc(T_ASC_Network * net, T_ASC_Association ** assoc)
{
    CONDITION cond = ASC_NORMAL;
    const char* knownAbstractSyntaxes[] = {
	UID_VerificationSOPClass
    };
    const char* transferSyntaxes[] = { 
	NULL, NULL, UID_LittleEndianImplicitTransferSyntax };

    cond = ASC_receiveAssociation(net, assoc, maxReceivePDULength);
    if (SUCCESS(cond)) {

	/* 
	** We prefer to accept Explicitly encoded transfer syntaxes.
	** If we are running on a Little Endian machine we prefer 
	** LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
	*/

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

        /* accept the Verification SOP Class if presented */
        cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
	    (*assoc)->params, 
	    knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes),
	    transferSyntaxes, DIM_OF(transferSyntaxes));
        
        if (SUCCESS(cond)) {
            /* the array of Storage SOP Class UIDs comes from dcuid.h */
            cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
	        (*assoc)->params, 
	        dcmStorageSOPClassUIDs, numberOfDcmStorageSOPClassUIDs,
	        transferSyntaxes, DIM_OF(transferSyntaxes));
        }
    }
    if (SUCCESS(cond)) {
	cond = ASC_acknowledgeAssociation(*assoc);
    }
    if (cond != ASC_NORMAL) {
        ASC_dropAssociation(*assoc);
        ASC_destroyAssociation(assoc);
    }
    return cond;
}

static CONDITION
echoSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg,
	T_ASC_PresentationContextID presID)
{
    CONDITION cond;

    if (verbose) {
        printf("Received ");
        DIMSE_printCEchoRQ(stdout, &msg->msg.CEchoRQ);
    }

    /* the echo succeeded !! */
    cond = DIMSE_sendEchoResponse(assoc, presID, &msg->msg.CEchoRQ, 
        STATUS_Success, NULL);
    
    if (!SUCCESS(cond)) {
        errmsg("Echo SCP Failed:");
	COND_DumpConditions();
    }
    return cond;

}


struct StoreCallbackData {
    char* imageFileName;
    DcmFileFormat* dcmff;
};


static void storeSCPCallback(
    /* in */
    void *callbackData, 
    T_DIMSE_StoreProgress *progress,	/* progress state */
    T_DIMSE_C_StoreRQ *req,		/* original store request */
    char *imageFileName, DcmDataset **imageDataSet, /* being received into */
    /* out */
    T_DIMSE_C_StoreRSP *rsp, 		/* final store response */
    DcmDataset **statusDetail)
{
    DIC_UI sopClass;
    DIC_UI sopInstance;
    
    if (verbose) {
        switch (progress->state) {
	case DIMSE_StoreBegin:	
	    printf("RECV:"); break;
	case DIMSE_StoreEnd:
	    printf("\n"); break;
	default:
	    putchar('.'); break;
	}
        fflush(stdout);
    }
    if (progress->state == DIMSE_StoreEnd) {
	*statusDetail = NULL;	/* no status detail */

	/* could save the image somewhere else, put it in database, etc */
	rsp->Status = STATUS_Success;

	if ((imageDataSet)&&(*imageDataSet))
	{
	    StoreCallbackData *cbdata = (StoreCallbackData*) callbackData;
            const char* fileName = cbdata->imageFileName;

	    DcmFileStream outf( fileName, DCM_WriteMode );
	    if ( outf.Fail() ) {
		errmsg("Cannot write image file: %s", fileName);
		rsp->Status = STATUS_STORE_Refused_OutOfResources;
	    } else {

		E_TransferSyntax xfer = EXS_Unknown;
		E_EncodingType enctype = EET_ExplicitLength;
		E_GrpLenEncoding ogltype = EGL_recalcGL;

		if (xfer == EXS_Unknown) {
		    /* use the same as the input */
		    xfer = (*imageDataSet)->getOriginalXfer();
		}

		DcmFileFormat *ff = cbdata->dcmff;

		ff->transferInit();
		ff->write( outf, xfer, enctype, ogltype );
		ff->transferEnd();

		if (ff->error() != EC_Normal) {
		    errmsg("Cannot write image file: %s", fileName);
		    rsp->Status = STATUS_STORE_Refused_OutOfResources;
		}
	    }
	}

	/* should really check the image to make sure it is consistent,
	 * that its sopClass and sopInstance correspond with those in
	 * the request.
	 */
	 if ((rsp->Status == STATUS_Success)&&(!ignoreStoreData)) {
             /* which SOP class and SOP instance ? */
             if (! DU_findSOPClassAndInstanceInDataSet(*imageDataSet, 
						       sopClass, sopInstance))
	     {
	        errmsg("Bad image file: %s", imageFileName);
	        rsp->Status = STATUS_STORE_Error_CannotUnderstand;
             } else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0) {
	         rsp->Status = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
             } else if (strcmp(sopInstance, 
			       req->AffectedSOPInstanceUID) != 0) {
	         rsp->Status = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
	     }
	}
    }
}

static CONDITION
storeSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg,
	T_ASC_PresentationContextID presID)
{
    CONDITION cond;
    T_DIMSE_C_StoreRQ *req;
    char imageFileName[1024];

    req = &msg->msg.CStoreRQ;

    if (ignoreStoreData) {
	strcpy(imageFileName, "/dev/null");
    } else {
	sprintf(imageFileName, "%s.%s", 
	    DU_sopClassToModality(req->AffectedSOPClassUID),
	    req->AffectedSOPInstanceUID);
    }

    if (verbose) {
        printf("Received ");
        DIMSE_printCStoreRQ(stdout, req);
    }

    StoreCallbackData callbackData;
    callbackData.imageFileName = imageFileName;
    DcmFileFormat dcmff;
    callbackData.dcmff = &dcmff;

    DcmDataset *dset = dcmff.getDataset();

    cond = DIMSE_storeProvider(assoc, presID, req, (char *)NULL, 1, 
			       &dset, storeSCPCallback, (void*)&callbackData,
			       DIMSE_BLOCKING, 0);
    if (!SUCCESS(cond)) {
        errmsg("Store SCP Failed:");
	COND_DumpConditions();
        /* remove file */
        if (!ignoreStoreData) {
	    unlink(imageFileName);
        }
    }

    return cond;
}

static CONDITION
subOpSCP(T_ASC_Association **subAssoc)
{
    CONDITION cond;
    T_DIMSE_Message     msg;
    T_ASC_PresentationContextID presID;

    if (!ASC_dataWaiting(*subAssoc, 0))	/* just in case */
	return DIMSE_NODATAAVAILABLE;

    cond = DIMSE_receiveCommand(*subAssoc, DIMSE_BLOCKING, 0, &presID,
	    &msg, NULL);

    if (cond == DIMSE_NORMAL) {
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
    if (cond == DIMSE_PEERREQUESTEDRELEASE) {
        /* pop only the peer requested release condition from the stack */
	COND_PopCondition(FALSE);	
	cond = ASC_acknowledgeRelease(*subAssoc);
    } else if (cond == DIMSE_PEERABORTEDASSOCIATION) {
	COND_PopCondition(FALSE);	/* pop DIMSE abort */
	COND_PopCondition(FALSE);	/* pop DUL abort */
    } else if (cond != DIMSE_NORMAL) {
	errmsg("DIMSE Failure (aborting sub-association):\n");
	COND_DumpConditions();
        /* some kind of error so abort the association */
	cond = ASC_abortAssociation(*subAssoc);
    }
    if (cond != DIMSE_NORMAL) {
        ASC_dropAssociation(*subAssoc);
        ASC_destroyAssociation(subAssoc);
    }
    return cond;
}

static void
subOpCallback(void */*subOpCallbackData*/, 
	T_ASC_Network *net, T_ASC_Association **subAssoc)
{

    if (net == NULL) return;	/* help no net ! */

    if (*subAssoc == NULL) {
        /* negotiate association */
	acceptSubAssoc(net, subAssoc);
    } else {
        /* be a service class provider */
	subOpSCP(subAssoc);
    }
}

static void 
moveCallback(void *callbackData, T_DIMSE_C_MoveRQ *request, 
    int responseCount, T_DIMSE_C_MoveRSP *response)
{
    CONDITION cond;
    MyCallbackInfo *myCallbackData;

    myCallbackData = (MyCallbackInfo*)callbackData;

    if (verbose) {
        printf("Move Response %d:\n", responseCount);
	DIMSE_printCMoveRSP(stdout, response);
    }
    /* should we send a cancel back ?? */
    if (cancelAfterNResponses == responseCount) {
	if (verbose) {
	    printf("Sending Cancel RQ, MsgId: %d, PresId: %d\n",
	        request->MessageID, myCallbackData->presId);
	}
        cond = DIMSE_sendCancelRequest(myCallbackData->assoc,
	    request->MessageID, myCallbackData->presId);
        if (cond != DIMSE_NORMAL) {
	    errmsg("Cancel RQ Failed:");
	    COND_DumpConditions();
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
    int elemCount = keys.card();
    for (int i=0; i<elemCount; i++) {
	DcmElement *elem = keys.remove((unsigned long)0);

	dset->insert(elem, TRUE);
    }
}


static  CONDITION
moveSCU(T_ASC_Association * assoc, const char *fname)
{
    CONDITION           cond;
    T_ASC_PresentationContextID presId;
    T_DIMSE_C_MoveRQ	req;
    T_DIMSE_C_MoveRSP   rsp;
    DIC_US      	msgId = assoc->nextMsgID++;
    DcmDataset		*rspIds = NULL;
    char                *sopClass;
    DcmDataset		*statusDetail = NULL;
    MyCallbackInfo	callbackData;


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


    sopClass = querySyntax[queryModel].moveSyntax;
    
    /* which presentation context should be used */
    presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    if (presId == 0) {
	return COND_PushCondition(DIMSE_NOVALIDPRESENTATIONCONTEXTID, 
	    "No Presentation Context for: %s", sopClass);
    }

    if (verbose) {
	printf("Find SCU RQ: MsgID %d\n", msgId);
	printf("Request:\n");
	dcmff.getDataset()->print();
	printf("--------\n");
    }
    
    callbackData.assoc = assoc;
    callbackData.presId = presId;

    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, sopClass);
    req.Priority = DIMSE_PRIORITY_MEDIUM;
    req.DataSetType = DIMSE_DATASET_PRESENT;
    if (moveDestination == NULL) {
        /* set the destination to be me */
        ASC_getAPTitles(assoc->params, req.MoveDestination, 
	    NULL, NULL);
    } else {
	strcpy(req.MoveDestination, moveDestination);
    }

    cond = DIMSE_moveUser(assoc, presId, &req, dcmff.getDataset(),
        moveCallback, &callbackData, DIMSE_BLOCKING, 0, 
	net, subOpCallback, NULL,
	&rsp, &statusDetail, &rspIds);
        
    if (cond == DIMSE_NORMAL) {
        if (verbose) {
	    DIMSE_printCMoveRSP(stdout, &rsp); 
	    if (rspIds != NULL) {
	        printf("Response Identifiers:\n");
		rspIds->print();
	    }
        }
    } else {
        errmsg("Move Failed:");
	COND_DumpConditions();
    }
    if (statusDetail != NULL) {
        printf("  Status Detail:\n");
	statusDetail->print();
	delete statusDetail;
    }

    if (rspIds != NULL) delete rspIds;
    
    return cond;
}


static CONDITION
cmove(T_ASC_Association * assoc, const char *fname)
{
    CONDITION cond = DIMSE_NORMAL;
    int n = repeatCount;

    while (cond == DIMSE_NORMAL && n--) {
	cond = moveSCU(assoc, fname);
    }
    return cond;
}

/*
** CVS Log
**
** $Log: movescu.cc,v $
** Revision 1.14  1997-05-29 15:52:57  meichel
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

