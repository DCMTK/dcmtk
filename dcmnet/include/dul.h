/*
          Copyright (C) 1993, 1994, RSNA and Washington University

          The software and supporting documentation for the Radiological
          Society of North America (RSNA) 1993, 1994 Digital Imaging and
          Communications in Medicine (DICOM) Demonstration were developed
          at the
                  Electronic Radiology Laboratory
                  Mallinckrodt Institute of Radiology
                  Washington University School of Medicine
                  510 S. Kingshighway Blvd.
                  St. Louis, MO 63110
          as part of the 1993, 1994 DICOM Central Test Node project for, and
          under contract with, the Radiological Society of North America.

          THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER RSNA NOR
          WASHINGTON UNIVERSITY MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
          PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
          USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
          SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
          THE SOFTWARE IS WITH THE USER.

          Copyright of the software and supporting documentation is
          jointly owned by RSNA and Washington University, and free access
          is hereby granted as a license to use this software, copy this
          software and prepare derivative works based upon this software.
          However, any distribution of this software source code or
          supporting documentation or derivative works (source code and
          supporting documentation) must include the three paragraphs of
          the copyright notice.
*/
/* Copyright marker.  Copyright will be inserted above.  Do not remove */
/*
** @$=@$=@$=
*/
/*
**				DICOM 93
**		    Electronic Radiology Laboratory
**		  Mallinckrodt Institute of Radiology
**		Washington University School of Medicine
**
** Module Name(s):
** Author, Date:	Stephen M. Moore, 14-Apr-1993
** Intent:		This file defines the public structures and constants
**			and the function prototypes for the DUL (DICOM Upper
**			Layer) facility.
** Last Update:		$Author: meichel $, $Date: 1999-04-19 08:39:27 $
** Source File:		$RCSfile: dul.h,v $
** Revision:		$Revision: 1.6 $
** Status:		$State: Exp $
*/


#ifndef DUL_IS_IN
#define DUL_IS_IN 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "extneg.h"

#ifndef DUL_KEYS
#define DUL_KEYS 1
typedef void DUL_NETWORKKEY;
typedef void DUL_ASSOCIATIONKEY;
typedef unsigned char DUL_PRESENTATIONCONTEXTID;
#endif

/*  Define a structure containing fixed length fields that can
 *  be used for requesting or accepting an association.  The
 *  lengths of "titles" and "names" are specified by the DICOM
 *  protocol document.  The fields in the structure are made
 *  longer to allow for zero-terminators.  DICOM doesn't know
 *  about node names, but the DUL_ package needs them.
 */

#define DUL_LEN_TITLE  ((size_t)16)	/* required by DICOM protocol	 */
#define DUL_LEN_NAME   ((size_t)64)	/* required by DICOM protocol	 */
#define DUL_LEN_UID    ((size_t)64)	/* required by DICOM protocol	 */
#define DUL_LEN_NODE  ((size_t)127)	/* should be "big enough"	 */


/* DICOM PDU Types */

#define DUL_TYPEASSOCIATERQ		(unsigned char)0x01
#define DUL_TYPEASSOCIATEAC		(unsigned char)0x02
#define	DUL_TYPEASSOCIATERJ		(unsigned char)0x03
#define	DUL_TYPEDATA			(unsigned char)0x04
#define	DUL_TYPERELEASERQ		(unsigned char)0x05
#define	DUL_TYPERELEASERP		(unsigned char)0x06
#define	DUL_TYPEABORT			(unsigned char)0x07

#define DUL_MAXTYPE			(unsigned char)0x07

typedef struct {
    char applicationContextName[DUL_LEN_NAME + 1];
    char callingAPTitle[DUL_LEN_TITLE + 1];
    char calledAPTitle[DUL_LEN_TITLE + 1];
    char respondingAPTitle[DUL_LEN_TITLE + 1];
    unsigned long maxPDU;
    unsigned short result;
    unsigned short resultSource;
    unsigned short diagnostic;
    char callingPresentationAddress[64];
    char calledPresentationAddress[64];
    LST_HEAD *requestedPresentationContext;
    LST_HEAD *acceptedPresentationContext;
    unsigned short maximumOperationsInvoked;
    unsigned short maximumOperationsPerformed;
    char callingImplementationClassUID[DICOM_UI_LENGTH + 1];
    char callingImplementationVersionName[16 + 1];
    char calledImplementationClassUID[DICOM_UI_LENGTH + 1];
    char calledImplementationVersionName[16 + 1];
    unsigned long peerMaxPDU;
    SOPClassExtendedNegotiationSubItemList *requestedExtNegList;
    SOPClassExtendedNegotiationSubItemList *acceptedExtNegList;
}   DUL_ASSOCIATESERVICEPARAMETERS;

typedef enum {
    DUL_SC_ROLE_DEFAULT,
    DUL_SC_ROLE_SCU,
    DUL_SC_ROLE_SCP,
    DUL_SC_ROLE_SCUSCP
}   DUL_SC_ROLE;

#define	DUL_PRESENTATION_ACCEPT			0
#define	DUL_PRESENTATION_REJECT_USER		1
#define	DUL_PRESENTATION_REJECT_NOREASON	2
#define	DUL_PRESENTATION_REJECT_ABSTRACT_SYNTAX	3
#define	DUL_PRESENTATION_REJECT_TRANSFER_SYNTAX	4

typedef struct {
    void *reserved[2];
    DUL_PRESENTATIONCONTEXTID presentationContextID;
    char abstractSyntax[DUL_LEN_UID + 1];
    LST_HEAD *proposedTransferSyntax;
    char acceptedTransferSyntax[DUL_LEN_UID + 1];
    unsigned char result;
    DUL_SC_ROLE proposedSCRole;
    DUL_SC_ROLE acceptedSCRole;
}   DUL_PRESENTATIONCONTEXT;

typedef struct {
    void *reserved[2];
    char transferSyntax[DUL_LEN_UID + 1];
}   DUL_TRANSFERSYNTAX;

typedef struct dul_abortitems {
    unsigned char result;
    unsigned char source;
    unsigned char reason;
}   DUL_ABORTITEMS;

typedef enum {
    DUL_COMMANDPDV,		/* A command PDV inside a data PDU */
    DUL_DATASETPDV
}				/* A data set PDV inside a data PDU */
    DUL_DATAPDV;

typedef enum {
    DUL_BLOCK,			/* Block on an operation (read, request) */
    DUL_NOBLOCK
}				/* Or return immediately if nothing avail */
    DUL_BLOCKOPTIONS;

typedef struct {
    unsigned long fragmentLength;
    unsigned char presentationContextID;
    DUL_DATAPDV pdvType;
    OFBool lastPDV;
    void *data;
}   DUL_PDV;

typedef struct {
    unsigned long count;
    void *scratch;
    unsigned long scratchLength;
    DUL_ABORTITEMS abort;
    DUL_PDV *pdv;
}   DUL_PDVLIST;

/*  Define the bits that go in the options field for InitializeNetwork
**
**  The low two bits define the byte order of messages at the DICOM
**  level.  This does not define the order of user data inside of a
**  DICOM PDU.
*/

#define	DUL_ORDERMASK		0x03	/* The bottom two bits */
#define	DUL_ORDERLITTLEENDIAN	0x01
#define	DUL_ORDERBIGENDIAN	0x02

#define	DUL_DOMAINMASK		0x04
#define	DUL_FULLDOMAINNAME	0x04

#define	DUL_NETWORK_TCP		"NETWORK TCP"
#define	DUL_NETWORK_OSI		"NETWORK OSI"

#define	DUL_AEREQUESTOR		"AE REQUESTOR"
#define	DUL_AEACCEPTOR		"AE ACCEPTOR"
#define	DUL_AEBOTH		"AE BOTH"

/*  These macros define results and reasons for rejecting an association
**  request.  Result is permanent or transient.  There are a number of
**  different reasons for rejecting requests that occur at different layers
 */
#define	DUL_REJ_RSLTPERMANENT		0x01
#define	DUL_REJ_RSLTTRANSIENT		0x02

/*  These should have been removed a long time ago.
#define	DUL_REJ_SU_NOREASON		0x0101
#define	DUL_REJ_SU_UNSUP_APP_CTX	0x0102
#define	DUL_REJ_SU_UNRECOG_CALLINGAP	0x0103
#define	DUL_REJ_SU_UNRECOG_CALLEDAP	0x0107

#define	DUL_REJ_SPACSE_NOREASON		0x0201
#define	DUL_REJ_SPACSE_UNSUP_PROTOCOL	0x0202

#define	DUL_REJ_SPPRES_CONGESTION	0x0301
#define	DUL_REJ_SPPRES_LIMITEXCEEDED	0x0302
*/

/*  These macros define parameters used to construct an ABORT PDU.
**  These include the source of the abort (SCU or SCP) and the
**  reason for the abort.
*/

#define	DUL_SCU_INITIATED_ABORT	0x00
#define	DUL_SCP_INITIATED_ABORT	0x02

#define DUL_ABORTSERVICEUSER		0x00
#define DUL_ABORTSERVICEPROVIDER	0x02


#define	DUL_ABORTNOREASON		0x00
#define	DUL_ABORTUNRECOGNIZEDPDU	0x01
#define	DUL_ABORTUNEXPECTEDPDU		0x02
#define	DUL_ABORTUNRECOGNIZEDPDUPARAM	0x04
#define	DUL_ABORTUNEXPECTEDPDUPARAM	0x05
#define	DUL_ABORTINVALIDPDUPARAM	0x06

/*  These macros define parameters used to construct a REJECT PDU.
**  These include the source of the reject (DICOM UL service-user,
**  (DICOM UL service-provider) and the reason for the reject.
*/
#define	DUL_REJECT_PERMANENT		0x01
#define	DUL_REJECT_TRANSIENT		0x02

#define	DUL_ULSU_REJECT			0x01
#define	DUL_ULSP_ACSE_REJECT		0x02
#define	DUL_ULSP_PRESENTATION_REJECT	0x03

#define	DUL_ULSU_REJ_NOREASON		0x01
#define	DUL_ULSU_REJ_UNSUP_APP_CTX_NAME	0x02
#define	DUL_ULSU_REJ_UNREC_CALLING_TITLE 0x03
#define	DUL_ULSU_REJ_UNREC_CALLED_TITLE	0x07

#define	DUL_ULSP_ACSE_REJ_NOREASON	0x01
#define	DUL_ULSP_ACSE_UNSUP_PROTOCOL	0x02

#define	DUL_ULSP_PRES_REJ_TEMP_CONGEST	0x01
#define	DUL_ULSP_PRES_REJ_LIMIT		0x02

#define	DUL_ABORTNOREASON		0x00
#define	DUL_ABORTUNRECOGNIZEDPDU	0x01
#define	DUL_ABORTUNEXPECTEDPDU		0x02
#define	DUL_ABORTUNRECOGNIZEDPDUPARAM	0x04
#define	DUL_ABORTUNEXPECTEDPDUPARAM	0x05
#define	DUL_ABORTINVALIDPDUPARAM	0x06

/* Define a set of constants and types that let the user get information
** about the Association
*/

typedef enum {
    DUL_K_INTEGER,		/* An integer type */
    DUL_K_STRING		/* A string type */
}   DUL_DATA_TYPE;
typedef enum {
    DUL_K_MAX_PDV_XMIT
}   DUL_ASSOCIATION_PARAMETER;

#define	DUL_TIMEOUT	180


/*
	PDU Callback types for snooping stuff
*/

#define CALLBACK_ITOA_PDU	1
#define CALLBACK_ATOI_PDU	2


/* Define the function prototypes for this facility.
**
** First set of functions are for establishing the network and associations.
*/

CONDITION
DUL_AcknowledgeAssociationRQ(DUL_ASSOCIATIONKEY ** association,
			     DUL_ASSOCIATESERVICEPARAMETERS * params);
CONDITION
DUL_InitializeNetwork(const char *type, const char *mode, void *param,
	     int timeout, unsigned long options, DUL_NETWORKKEY ** network);
CONDITION
DUL_ReceiveAssociationRQ(DUL_NETWORKKEY ** net,
	  DUL_BLOCKOPTIONS blk, DUL_ASSOCIATESERVICEPARAMETERS * parameters,
			 DUL_ASSOCIATIONKEY ** association);
CONDITION
DUL_RejectAssociationRQ(DUL_ASSOCIATIONKEY ** association,
			DUL_ABORTITEMS * params);
CONDITION
DUL_RequestAssociation(DUL_NETWORKKEY ** network,
		       DUL_ASSOCIATESERVICEPARAMETERS * params,
		       DUL_ASSOCIATIONKEY ** association);

/* Define functions for releasing/aborting Associations.
*/
CONDITION DUL_AbortAssociation(DUL_ASSOCIATIONKEY ** association);
CONDITION DUL_DropAssociation(DUL_ASSOCIATIONKEY ** association);
CONDITION DUL_DropNetwork(DUL_NETWORKKEY ** network);
CONDITION DUL_ReleaseAssociation(DUL_ASSOCIATIONKEY ** association);
CONDITION DUL_AcknowledgeRelease(DUL_ASSOCIATIONKEY ** association);

/* Functions for reading/write PDVs inside P DATA PDUs.
*/
CONDITION
DUL_ReadPDVs(DUL_ASSOCIATIONKEY ** association,
	     DUL_PDVLIST * pdvList, DUL_BLOCKOPTIONS block, int timeout);
CONDITION
DUL_WritePDVs(DUL_ASSOCIATIONKEY ** association,
	      DUL_PDVLIST * pdvList);
CONDITION DUL_NextPDV(DUL_ASSOCIATIONKEY ** association, DUL_PDV * pdv);

/*
	Functions for snooping
*/
#ifdef SNOOP
CONDITION
DUL_FileSnoop(char *itoa_file, char *atoi_file, char *initiator, char *acceptor);
CONDITION
DUL_NetworkSnoop(char *device, int ppa, char *initiator,
		 char *acceptor, int port, int bufsize, int associations);
CONDITION
DUL_RegPDUCall(void (*callback) (), int callbackType, void *ctx);
#endif

/* Miscellaneous functions.
*/
const char *DUL_Message(CONDITION cond);
void DUL_Debug(OFBool flag);
CONDITION
DUL_AssociationParameter(DUL_ASSOCIATIONKEY ** association,
			 DUL_ASSOCIATION_PARAMETER param, DUL_DATA_TYPE type,
			 void *address, size_t length);
CONDITION
DUL_MakePresentationCtx(DUL_PRESENTATIONCONTEXT ** ctx,
		     DUL_SC_ROLE proposedSCRole, DUL_SC_ROLE acceptedSCRole,
DUL_PRESENTATIONCONTEXTID ctxID, unsigned char reason, const char *abstractSyntax,
			const char *transferSyntax,...);
void DUL_DumpParams(DUL_ASSOCIATESERVICEPARAMETERS * params);
CONDITION DUL_ClearServiceParameters(DUL_ASSOCIATESERVICEPARAMETERS * params);
void DUL_DefaultServiceParameters(DUL_ASSOCIATESERVICEPARAMETERS * params);
void DUL_Blog(OFBool flag);
void dumpExtNegList(SOPClassExtendedNegotiationSubItemList& list);

/*
** Additional functions (from dulextra.cc) needed to support 
** selecting amongst several concurrent associations.
** Andrew Hewett, Institute OFFIS, Oldenburg, Germany.
*/

int
DUL_associationSocket(DUL_ASSOCIATIONKEY * callerAssociation);
OFBool 
DUL_dataWaiting(DUL_ASSOCIATIONKEY * callerAssociation, int timeout);
int 
DUL_networkSocket(DUL_NETWORKKEY * callerNet);
OFBool 
DUL_associationWaiting(DUL_NETWORKKEY * callerNet, int timeout);

/*
** END extra functions
*/

/*  Now define the fixed values for conditions returned by this
**  package.  Note that FAC_DUL is used to generate these
**  conditions.  This should be defined in some global include
**  file so that we can keep all of the facilities straight.
*/

#define	DUL_NORMAL	FORM_COND(FAC_DUL, SEV_SUCC, 1)
#define	DUL_NETWORKINITIALIZED	FORM_COND(FAC_DUL, SEV_ERROR, 2)
#define	DUL_KEYCREATEFAILURE	FORM_COND(FAC_DUL, SEV_ERROR, 3)
#define	DUL_UNSUPPORTEDNETWORK	FORM_COND(FAC_DUL, SEV_ERROR, 4)
#define	DUL_UNRECOGNIZEDAE	FORM_COND(FAC_DUL, SEV_ERROR, 5)
#define	DUL_TCPINITERROR	FORM_COND(FAC_DUL, SEV_ERROR, 6)
#define	DUL_NULLKEY		FORM_COND(FAC_DUL, SEV_ERROR, 7)
#define	DUL_ILLEGALKEY		FORM_COND(FAC_DUL, SEV_ERROR, 8)
#define	DUL_ILLEGALACCEPT	FORM_COND(FAC_DUL, SEV_ERROR, 9)
#define	DUL_ILLEGALREQUEST	FORM_COND(FAC_DUL, SEV_ERROR, 10)
#define	DUL_UNEXPECTEDPDU	FORM_COND(FAC_DUL, SEV_ERROR, 11)
#define	DUL_UNKNOWNREMOTENODE	FORM_COND(FAC_DUL, SEV_ERROR, 12)
#define	DUL_UNKNOWNHOST		FORM_COND(FAC_DUL, SEV_ERROR, 13)
#define	DUL_MALLOCERROR		FORM_COND(FAC_DUL, SEV_ERROR, 14)
#define	DUL_TCPIOERROR		FORM_COND(FAC_DUL, SEV_ERROR, 15)
#define	DUL_NOASSOCIATIONREQUEST	FORM_COND(FAC_DUL, SEV_WARN,  16)
#define	DUL_UNRECOGNIZEDPDUTYPE	FORM_COND(FAC_DUL, SEV_ERROR, 17)
#define	DUL_PEERABORTEDASSOCIATION	FORM_COND(FAC_DUL, SEV_ERROR, 19)
#define	DUL_PEERDROPPEDASSOCIATION	FORM_COND(FAC_DUL, SEV_ERROR, 20)
#define	DUL_WRONGASSOCIATIONSTATE	FORM_COND(FAC_DUL, SEV_ERROR, 21)
#define	DUL_CODINGERROR		FORM_COND(FAC_DUL, SEV_ERROR, 22)
#define	DUL_ILLEGALSERVICEPARAMETER	FORM_COND(FAC_DUL, SEV_ERROR, 23)
#define	DUL_REQUESTASSOCIATIONFAILED	FORM_COND(FAC_DUL, SEV_ERROR, 24)
#define	DUL_APABORT		FORM_COND(FAC_DUL, SEV_ERROR, 25)
#define DUL_ASSOCIATIONREJECTED	FORM_COND(FAC_DUL, SEV_ERROR, 26)
#define	DUL_ILLEGALREJECTRESULT	FORM_COND(FAC_DUL, SEV_ERROR, 27)
#define	DUL_ILLEGALREJECTREASON	FORM_COND(FAC_DUL, SEV_ERROR, 28)
#define	DUL_RELEASECONFIRMED	FORM_COND(FAC_DUL, SEV_SUCC,  29)
#define	DUL_PEERREQUESTEDRELEASE	FORM_COND(FAC_DUL, SEV_SUCC,  30)
#define DUL_PDATAPDUARRIVED	FORM_COND(FAC_DUL, SEV_SUCC,  31)
#define	DUL_READTIMEOUT		FORM_COND(FAC_DUL, SEV_WARN,  32)
#define DUL_NETWORKCLOSED	FORM_COND(FAC_DUL, SEV_ERROR, 33)
#define DUL_ILLEGALPDU		FORM_COND(FAC_DUL, SEV_ERROR, 34)
#define	DUL_FSMERROR		FORM_COND(FAC_DUL, SEV_FATAL, 35)
#define	DUL_WRONGDATATYPE	FORM_COND(FAC_DUL, SEV_ERROR, 36)
#define	DUL_INSUFFICIENTBUFFERLENGTH	FORM_COND(FAC_DUL, SEV_ERROR, 37)
#define	DUL_INCORRECTBUFFERLENGTH	FORM_COND(FAC_DUL, SEV_ERROR, 38)
#define	DUL_ASSOCIATIONPARAMETERFAILED	FORM_COND(FAC_DUL, SEV_ERROR, 39)
#define	DUL_NOPDVS			FORM_COND(FAC_DUL, SEV_ERROR, 40)
#define DUL_ILLEGALPARAMETER	FORM_COND(FAC_DUL, SEV_ERROR, 41)
#define	DUL_LISTERROR		FORM_COND(FAC_DUL, SEV_ERROR, 42)
#define	DUL_ILLEGALPDULENGTH	FORM_COND(FAC_DUL, SEV_FATAL, 43)
#define	DUL_LISTCREATEFAILED	FORM_COND(FAC_DUL, SEV_ERROR, 44)
#define	DUL_UNSUPPORTEDPEERPROTOCOL	FORM_COND(FAC_DUL, SEV_ERROR, 45)
#define	DUL_PEERILLEGALXFERSYNTAXCOUNT	FORM_COND(FAC_DUL, SEV_ERROR, 46)
#define	DUL_PCTRANSLATIONFAILURE	FORM_COND(FAC_DUL, SEV_ERROR, 47)
#define DUL_SNPFILEOPEN			FORM_COND(FAC_DUL, SEV_ERROR, 48)
#define DUL_SNPFILEREAD 		FORM_COND(FAC_DUL, SEV_ERROR, 49)
#define DUL_SNPCALLBACKUSE		FORM_COND(FAC_DUL, SEV_ERROR, 50)
#define DUL_SNPCALLBACKREG		FORM_COND(FAC_DUL, SEV_ERROR, 51)
#define DUL_SNPINIT 			FORM_COND(FAC_DUL, SEV_ERROR, 52)
#define DUL_SNPPREMATUREEOF 		FORM_COND(FAC_DUL, SEV_ERROR, 53)
#define DUL_SNPSTART			FORM_COND(FAC_DUL, SEV_ERROR, 54)
#define DUL_SNPSTOP 			FORM_COND(FAC_DUL, SEV_ERROR, 55)
#define DUL_SNPTERMINATE 		FORM_COND(FAC_DUL, SEV_ERROR, 56)
#define DUL_SNPNOTALLASSOC		FORM_COND(FAC_DUL, SEV_ERROR, 57)
#define DUL_SNPBADSTATE			FORM_COND(FAC_DUL, SEV_ERROR, 58)
#define DUL_SNPBADASSOCSTATE		FORM_COND(FAC_DUL, SEV_ERROR, 59)
#define DUL_SNPUNIMPLEMENTED		FORM_COND(FAC_DUL, SEV_ERROR, 60)

#endif

/*
** CVS Log
** $Log: dul.h,v $
** Revision 1.6  1999-04-19 08:39:27  meichel
** Added experimental support for extended SOP class negotiation.
**
** Revision 1.5  1999/03/29 11:19:59  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.4  1998/06/29 12:14:27  meichel
** Removed some name clashes (e.g. local variable with same
**   name as class member) to improve maintainability.
**   Applied some code purifications proposed by the gcc 2.8.1 -Weffc++ option.
**
** Revision 1.3  1997/07/21 08:40:11  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.2  1997/01/13 15:53:02  hewett
** Added missing function prototypes (required for CodeWarrior 10).
**
** Revision 1.1.1.1  1996/03/26 18:38:44  hewett
** Initial Release.
**
**
*/
