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
** Simple Service Class Provider Example Program
**
** Author: 	Andrew Hewett
**		Kuratorium OFFIS e.V., Oldenburg, Germany
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1997-03-27 16:11:27 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/apps/storescp.cc,v $
** CVS/RCS Revision:	$Revision: 1.7 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

BEGIN_EXTERN_C
#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
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

#define PATHSEPARATOR PATH_SEPARATOR	/* via osconfig.h" */


#define APPLICATIONTITLE	"STORESCP"

static char* progname = NULL;
BOOLEAN verbose = FALSE;
BOOLEAN debug = FALSE;
BOOLEAN rejectWhenNoImplementationClassUID = FALSE;
BOOLEAN refuse = FALSE;
static int maxReceivePDULength = ASC_DEFAULTMAXPDU;
BOOLEAN ignoreStoreData = FALSE;
static int sleepAfterStore = 0;
static int sleepDuringStore = 0;

static void
shortusage()
{
    fprintf(stderr, 
	    "usage: %s [-u][-v][-d][-r][-i][-s n][-S n][-b n] port\n", 
	    progname);
}

static void
fullusage()
{
    shortusage();
    fprintf(stderr, "\
parameters:\n\
    port	tcp/ip port number to listen on\n\
options:\n\
    -u          disable generation of unknown VR (UN)\n\
    -v		verbose mode\n\
    -d		debug mode\n\
    -r		refuse association\n\
    -i		ignore store data, receive but do not save to disk\n\
    -s n	sleep n seconds after store (default 0)\n\
    -b n	set max receive pdu to n bytes (default: %d)\n",
    maxReceivePDULength);
    exit(1);
}

static void 
usage()
{
    shortusage();
    exit(1);
}

static void errmsg(const char* msg, ...)
{
    va_list args;
    
    fprintf(stderr, "%s: ", progname);
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
}

static CONDITION processCommands(T_ASC_Association *assoc);
static CONDITION acceptAssociation(T_ASC_Network *net);
static CONDITION echoSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg,
	T_ASC_PresentationContextID presID);
static CONDITION storeSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg,
	T_ASC_PresentationContextID presID);
	

int 
main(int argc, char *argv[])
{
    CONDITION cond;
    T_ASC_Network *net;
    int i;
    int port;

#ifdef HAVE_GUSI_H
    /* needed for Macintosh */
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.0 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 0 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

    prepareCmdLineArgs(argc, argv, "storescp");

    /* strip any leading path from program name */
    if ((progname = (char *)strrchr(argv[0], PATHSEPARATOR)) != NULL) {
	progname++;
    } else {
	progname = argv[0];
    }

    if (argc < 2) {
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
	case 'r':
	    refuse = TRUE;
	    break;
	case 'i':
	    ignoreStoreData = TRUE;
	    break;
	case 's':
	    if (((i + 1) < argc) &&
		(sscanf(argv[i + 1], "%d", &sleepAfterStore) == 1)) {
		i++;		/* seconds parsed */
	    } else {
		usage();
	    }
	    break;
	case 'S':
	    if (((i + 1) < argc) &&
		(sscanf(argv[i + 1], "%d", &sleepDuringStore) == 1)) {
		i++;		/* seconds parsed */
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
	default:
	    usage();
	}
    }

    if (i >= argc)
	usage();

    /* get port number to listen on */
    if (sscanf(argv[i], "%d", &port) != 1) {
	errmsg("bad port number: %s", argv[i]);
	usage();
    }
    DUL_Debug(debug);
    DIMSE_debug(debug);
    SetDebugLevel(((debug)?3:0));	/* dcmdata debugging */

#ifdef HAVE_GETEUID
    /* if port is privileged we must be as well */
    if (port < 1024) {
        if (geteuid() != 0) {
	    errmsg("cannot listen on port %d, insufficient privileges", port);
	    usage();
	}
    }
#endif

    /* make sure data dictionary is loaded */
    if (dcmDataDict.numberOfEntries() == 0) {
	fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
		DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    cond = ASC_initializeNetwork(NET_ACCEPTOR, port, 1000, &net);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }

#ifdef HAVE_GETUID
    /* return to normal uid so that we can't do too much damage in case
     * things go very wrong.   Only does someting if the program is setuid 
     * root, and run by another user.  Running as root user may be
     * potentially disasterous if this program screws up badly.
     */
    setuid(getuid());
#endif

    while (SUCCESS(cond)) {

	cond = acceptAssociation(net);

    }

    cond = ASC_dropNetwork(&net);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }

#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif

    return 0;
}

static CONDITION 
acceptAssociation(T_ASC_Network * net)
{
    CONDITION cond;
    char buf[BUFSIZ];
    T_ASC_Association *assoc;

    const char* knownAbstractSyntaxes[] = {
	UID_VerificationSOPClass,
    };
    const char* transferSyntaxes[] = { 
	NULL, NULL, UID_LittleEndianImplicitTransferSyntax };

    cond = ASC_receiveAssociation(net, &assoc, maxReceivePDULength);
    if (!SUCCESS(cond)) {
	if (verbose)
	    COND_DumpConditions();

	goto cleanup;
    }
    if (verbose) {
	printf("Association Received\n");
    }
    if (debug) {
	printf("Parameters:\n");
	ASC_dumpParameters(assoc->params);
    }

    if (refuse) {
	T_ASC_RejectParameters rej = {
		ASC_RESULT_REJECTEDPERMANENT,
		ASC_SOURCE_SERVICEUSER,
		ASC_REASON_SU_NOREASON
	};

	if (verbose) {
	    printf("Refusing Association (forced via command line)\n");
	}
	cond = ASC_rejectAssociation(assoc, &rej);
	if (cond != ASC_NORMAL) {
	    printf("Association Reject Failed:\n");
	    COND_DumpConditions();
	}
	goto cleanup;

    }

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
	assoc->params, 
	knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes),
	transferSyntaxes, DIM_OF(transferSyntaxes));
    if (!SUCCESS(cond)) {
	if (verbose)
	    COND_DumpConditions();

	goto cleanup;
    }

    /* the array of Storage SOP Class UIDs comes from dcuid.h */
    cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
	assoc->params, 
	dcmStorageSOPClassUIDs, numberOfDcmStorageSOPClassUIDs,
	transferSyntaxes, DIM_OF(transferSyntaxes));
    if (!SUCCESS(cond)) {
	if (verbose)
	    COND_DumpConditions();

	goto cleanup;
    }

    /* set our app title */
    ASC_setAPTitles(assoc->params, NULL, NULL, APPLICATIONTITLE);

    /* acknowledge or reject this association */

    if ((cond = ASC_getApplicationContextName(assoc->params, buf) != 
	ASC_NORMAL) || strcmp(buf, UID_StandardApplicationContext) != 0) {
	/* reject: the application context name is not supported */
	T_ASC_RejectParameters rej = {
	    ASC_RESULT_REJECTEDPERMANENT,
	    ASC_SOURCE_SERVICEUSER,
	    ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
	};

	if (verbose) {
	    printf("Association Rejected: bad appliciation context name: %s\n", buf);
	}
	cond = ASC_rejectAssociation(assoc, &rej);
	if (cond != ASC_NORMAL) {
	    if (verbose)
		COND_DumpConditions();
	}
	goto cleanup;

    } else if (rejectWhenNoImplementationClassUID && 
	strlen(assoc->params->theirImplementationClassUID) == 0) {
	/* reject: the no implementation Class UID provided */
	T_ASC_RejectParameters rej = {
	    ASC_RESULT_REJECTEDPERMANENT,
	    ASC_SOURCE_SERVICEUSER,
	    ASC_REASON_SU_NOREASON
	};

	if (verbose) {
	    printf("Association Rejected: No implementation Class UID provided\n");
	}
	cond = ASC_rejectAssociation(assoc, &rej);
	if (cond != ASC_NORMAL) {
	    if (verbose)
		COND_DumpConditions();
	}
	goto cleanup;

    } else {
	cond = ASC_acknowledgeAssociation(assoc);
	if (cond != ASC_NORMAL) {
	    COND_DumpConditions();

	    goto cleanup;
	}
	if (verbose) {
	    printf("Association Acknowledged (Max Send PDV: %lu)\n",
		assoc->sendPDVLength);
	    if (ASC_countAcceptedPresentationContexts(assoc->params) == 0)
		printf("    (but no valid presentation contexts)\n");
	    if (debug) ASC_dumpParameters(assoc->params);
	}
    }


    /* now do the real work */
    cond = processCommands(assoc);

    if (cond == DIMSE_PEERREQUESTEDRELEASE) {
	COND_PopCondition(FALSE);	/* pop only the peer requested
					 * release condition from the stack */
	if (verbose) printf("Association Release\n");
	cond = ASC_acknowledgeRelease(assoc);
    } else if (cond == DIMSE_PEERABORTEDASSOCIATION) {
	COND_PopCondition(FALSE);	/* pop DIMSE abort */
	COND_PopCondition(FALSE);	/* pop DUL abort */
	if (verbose) printf("Association Aborted\n");
    } else {
	errmsg("DIMSE Failure (aborting association):\n");
	COND_DumpConditions();
	/* some kind of error so abort the association */
	cond = ASC_abortAssociation(assoc);
    }

cleanup:
    cond = ASC_dropAssociation(assoc);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }
    cond = ASC_destroyAssociation(&assoc);
    if (!SUCCESS(cond)) {
	COND_DumpConditions();
	exit(1);
    }

    if (verbose) {
	/* are there any conditions sitting on the condition stack? */
	char buf[BUFSIZ];
	CONDITION c;

	if (COND_TopCondition(&c, buf, BUFSIZ) != COND_NORMAL) {
	    fprintf(stderr, "CONDITIONS Remaining\n");
	    COND_DumpConditions();
	}
    }
    return cond;
}


static CONDITION
processCommands(T_ASC_Association * assoc)
{
    CONDITION cond = DIMSE_NORMAL;
    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presID;
    DcmDataset *statusDetail = NULL;

    while (cond == DIMSE_NORMAL) {
	
	cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &presID,
		&msg, &statusDetail);

	if (statusDetail != NULL) {
	    printf("Extra Status Detail: \n");
	    statusDetail->print();
	    delete statusDetail;
	}

	/* did peer release, abort, or do we have a valid message ? */
	switch (cond) {
	case DIMSE_NORMAL:
	    /* process command */
	    switch (msg.CommandField) {
	    case DIMSE_C_ECHO_RQ:
		cond = echoSCP(assoc, &msg, presID);
		break;
	    case DIMSE_C_STORE_RQ:
		cond = storeSCP(assoc, &msg, presID);
		break;
	    default:
		/* we cannot handle this kind of message */
		cond = DIMSE_BADCOMMANDTYPE;
		errmsg("Cannot handle command: 0x%x\n", 
			(unsigned)msg.CommandField);
	    }
	    break;
	case DIMSE_PEERREQUESTEDRELEASE:
	case DIMSE_PEERABORTEDASSOCIATION:
	    /* association gone */
	    break;
	default:
	    break;
	}
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

static void 
storeSCPCallback(
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

    if (sleepDuringStore > 0) {
        sleep(sleepDuringStore);
    }    
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
		E_GrpLenEncoding ogltype = EGL_withGL;

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
    char imageFileName[2048];

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

    cond = DIMSE_storeProvider(assoc, presID, req, (char *)NULL,
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

    if (sleepAfterStore > 0) {
        sleep(sleepAfterStore);
    }

    return cond;
}

/*
** CVS Log
** $Log: storescp.cc,v $
** Revision 1.7  1997-03-27 16:11:27  hewett
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
