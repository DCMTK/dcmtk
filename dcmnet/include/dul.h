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
** Last Update:		$Author: meichel $, $Date: 2002-11-28 16:57:36 $
** Source File:		$RCSfile: dul.h,v $
** Revision:		$Revision: 1.14 $
** Status:		$State: Exp $
*/


#ifndef DUL_IS_IN
#define DUL_IS_IN 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofglobal.h"
#include "oftypes.h"
#include "extneg.h"

class DcmTransportConnection;
class DcmTransportLayer;

/* Global flag to enable/disable reverse DNS lookup when accepting
 * associations.  If disabled, the numerical IP address instead of the symbolic hostname
 * is stored in the callingPresentationAddress field of the association parameters
 * structure.  Most DICOM applications (except imagectn) don't need the symbolic
 * hostname anyway, and the reverse DNS lookup can cause a long timeout.
 */
extern OFGlobal<OFBool> dcmDisableGethostbyaddr; /* default OFFalse */

/*
 * Global timeout (seconds) for connecting to remote hosts.
 * Default value is -1 which selects infinite timeout, i.e. blocking connect().
 */
extern OFGlobal<Sint32> dcmConnectionTimeout;   /* default -1 */


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
    OFBool useSecureLayer;
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

typedef OFList<char *> DUL_TRANSFERSYNTAXLIST;

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

#define	DUL_AEREQUESTOR		"AE REQUESTOR"
#define	DUL_AEACCEPTOR		"AE ACCEPTOR"
#define	DUL_AEBOTH		"AE BOTH"

/*  These macros define results and reasons for rejecting an association
**  request.  Result is permanent or transient.  There are a number of
**  different reasons for rejecting requests that occur at different layers
 */
#define	DUL_REJ_RSLTPERMANENT		0x01
#define	DUL_REJ_RSLTTRANSIENT		0x02

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


/* Define the function prototypes for this facility.
**
** First set of functions are for establishing the network and associations.
*/

OFCondition
DUL_AcknowledgeAssociationRQ(
  DUL_ASSOCIATIONKEY ** association,
  DUL_ASSOCIATESERVICEPARAMETERS * params, 
  int activatePDUStorage);

OFCondition
DUL_InitializeNetwork(
  const char *mode, 
  void *param,
  int timeout, 
  unsigned long 
  options, 
  DUL_NETWORKKEY ** network);

OFCondition
DUL_ReceiveAssociationRQ(
  DUL_NETWORKKEY ** net,
  DUL_BLOCKOPTIONS blk, 
  int timeout,
  DUL_ASSOCIATESERVICEPARAMETERS * parameters,
  DUL_ASSOCIATIONKEY ** association, 
  int activatePDUStorage);

OFCondition
DUL_RejectAssociationRQ(
  DUL_ASSOCIATIONKEY ** association,
  DUL_ABORTITEMS * params, 
  int activatePDUStorage);
  
OFCondition
DUL_RequestAssociation(
  DUL_NETWORKKEY ** network,
  DUL_ASSOCIATESERVICEPARAMETERS * params,
  DUL_ASSOCIATIONKEY ** association,
  int activatePDUStorage);

/* Define functions for releasing/aborting Associations.
*/
OFCondition DUL_AbortAssociation(DUL_ASSOCIATIONKEY ** association);
OFCondition DUL_DropAssociation(DUL_ASSOCIATIONKEY ** association);
OFCondition DUL_DropNetwork(DUL_NETWORKKEY ** network);
OFCondition DUL_ReleaseAssociation(DUL_ASSOCIATIONKEY ** association);
OFCondition DUL_AcknowledgeRelease(DUL_ASSOCIATIONKEY ** association);

/* Functions for reading/write PDVs inside P DATA PDUs.
*/
OFCondition
DUL_ReadPDVs(DUL_ASSOCIATIONKEY ** association,
	     DUL_PDVLIST * pdvList, DUL_BLOCKOPTIONS block, int timeout);
OFCondition
DUL_WritePDVs(DUL_ASSOCIATIONKEY ** association,
	      DUL_PDVLIST * pdvList);
OFCondition DUL_NextPDV(DUL_ASSOCIATIONKEY ** association, DUL_PDV * pdv);


/* Miscellaneous functions.
*/
const char *DUL_Message(OFCondition cond);
void DUL_Debug(OFBool flag);
OFCondition
DUL_AssociationParameter(DUL_ASSOCIATIONKEY ** association,
			 DUL_ASSOCIATION_PARAMETER param, DUL_DATA_TYPE type,
			 void *address, size_t length);
OFCondition
DUL_MakePresentationCtx(DUL_PRESENTATIONCONTEXT ** ctx,
		     DUL_SC_ROLE proposedSCRole, DUL_SC_ROLE acceptedSCRole,
DUL_PRESENTATIONCONTEXTID ctxID, unsigned char reason, const char *abstractSyntax,
			const char *transferSyntax,...);
void DUL_DumpParams(DUL_ASSOCIATESERVICEPARAMETERS * params);
void DUL_DumpConnectionParameters(DUL_ASSOCIATIONKEY *association, ostream& outstream);

OFCondition DUL_ClearServiceParameters(DUL_ASSOCIATESERVICEPARAMETERS * params);
void DUL_DefaultServiceParameters(DUL_ASSOCIATESERVICEPARAMETERS * params);
void dumpExtNegList(SOPClassExtendedNegotiationSubItemList& list);

/*
** Additional functions (from dulextra.cc) needed to support 
** selecting amongst several concurrent associations.
** Andrew Hewett, Institute OFFIS, Oldenburg, Germany.
*/

OFBool 
DUL_dataWaiting(DUL_ASSOCIATIONKEY * callerAssociation, int timeout);
int 
DUL_networkSocket(DUL_NETWORKKEY * callerNet);
OFBool 
DUL_associationWaiting(DUL_NETWORKKEY * callerNet, int timeout);

/*
 * functions allowing to retrieve raw A-ASSOCIATE PDUs from the DUL layer
 */
void DUL_activateAssociatePDUStorage(DUL_ASSOCIATIONKEY *dulassoc);
void DUL_returnAssociatePDUStorage(DUL_ASSOCIATIONKEY *dulassoc, void *& pdu, unsigned long& pdusize);

/* get pointer to transport connection from opaque association pointer */
DcmTransportConnection *DUL_getTransportConnection(DUL_ASSOCIATIONKEY * callerAssociation);

/* change transport layer */
OFCondition DUL_setTransportLayer(DUL_NETWORKKEY *callerNetworkKey, DcmTransportLayer *newLayer, int takeoverOwnership);

/*
 * function allowing to retrieve the peer certificate from the DUL layer
 */
unsigned long DUL_getPeerCertificateLength(DUL_ASSOCIATIONKEY *dulassoc);
unsigned long DUL_getPeerCertificate(DUL_ASSOCIATIONKEY *dulassoc, void *buf, unsigned long bufLen);

/*
** END extra functions
*/


#endif

/*
** CVS Log
** $Log: dul.h,v $
** Revision 1.14  2002-11-28 16:57:36  meichel
** Added global flag dcmConnectionTimeout that defines a timeout for
**   outgoing association requests in the DICOM upper layer.
**
** Revision 1.13  2001/11/27 09:54:33  wilkens
** Updated storescp. 6 new options (--output-directory, --sort-conc-studies,
** --exec-on-reception, --exec-on-eostudy, --rename-on-eostudy, and
** --eostudy-timeout) implemented (requirements from GO-Kard).
**
** Revision 1.12  2001/10/12 10:18:27  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.11  2001/09/26 12:28:59  meichel
** Implemented changes in dcmnet required by the adaptation of dcmdata
**   to class OFCondition.  Removed some unused code.
**
** Revision 1.10  2001/06/01 11:02:02  meichel
** Implemented global flag and command line option to disable reverse
**   DNS hostname lookup using gethostbyaddr when accepting associations.
**
** Revision 1.9  2000/10/10 12:06:54  meichel
** Updated transport layer error codes and routines for printing
**   connection parameters.
**
** Revision 1.8  2000/08/10 14:50:54  meichel
** Added initial OpenSSL support.
**
** Revision 1.7  2000/06/07 08:57:22  meichel
** dcmnet ACSE routines now allow to retrieve a binary copy of the A-ASSOCIATE
**   RQ/AC/RJ PDUs, e.g. for logging purposes.
**
** Revision 1.6  1999/04/19 08:39:27  meichel
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
