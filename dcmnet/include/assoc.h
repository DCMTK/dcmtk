/*
**  Copyright (C) 1993/1994, OFFIS, Oldenburg University and CERIUM
**  
**  This software and supporting documentation were
**  developed by 
**  
**    Institut OFFIS
**    Bereich Kommunikationssysteme
**    Westerstr. 10-12
**    26121 Oldenburg, Germany
**    
**    Fachbereich Informatik
**    Abteilung Prozessinformatik
**    Carl von Ossietzky Universitaet Oldenburg 
**    Ammerlaender Heerstr. 114-118
**    26111 Oldenburg, Germany
**    
**    CERIUM
**    Laboratoire SIM
**    Faculte de Medecine
**    2 Avenue du Pr. Leon Bernard
**    35043 Rennes Cedex, France
**  
**  for CEN/TC251/WG4 as a contribution to the Radiological 
**  Society of North America (RSNA) 1993 Digital Imaging and 
**  Communications in Medicine (DICOM) Demonstration.
**  
**  THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER OFFIS,
**  OLDENBURG UNIVERSITY NOR CERIUM MAKE ANY WARRANTY REGARDING 
**  THE SOFTWARE, ITS PERFORMANCE, ITS MERCHANTABILITY OR 
**  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER 
**  DISEASES OR ITS CONFORMITY TO ANY SPECIFICATION.  THE 
**  ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF THE SOFTWARE   
**  IS WITH THE USER. 
**  
**  Copyright of the software and supporting documentation
**  is, unless otherwise stated, jointly owned by OFFIS,
**  Oldenburg University and CERIUM and free access is hereby
**  granted as a license to use this software, copy this
**  software and prepare derivative works based upon this
**  software. However, any distribution of this software
**  source code or supporting documentation or derivative
**  works (source code and supporting documentation) must
**  include the three paragraphs of this copyright notice. 
** 
*/
/*
**
** Author: Andrew Hewett		Created: 03-06-93
** 
** Module: association
**
** Purpose: 
**	This file contains the routines which provide association management
**	for DICOM applications.  It maintains structures which describe
**	active associations and provides access to association specific 
**	informtion.  Also provided are routines for aiding association
**	negotiation (presentation contexts, abstract syntaxes, transfer
**	syntaxes, maximum PDU length, and other extended negotiation).
**
**	This package uses the facilities of the DICOM Upper Layer for
**	receiving/sending association requests/responses.
**
**	Each active association is represented by an T_ASC_Association
**	structure which contains all relevant information.
**
**	Module Prefix: ASC_
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 2001-10-12 10:18:25 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/Attic/assoc.h,v $
** CVS/RCS Revision:	$Revision: 1.17 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
*/


#ifndef ASSOCIATION_H
#define ASSOCIATION_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

/*
** Required Include Files
*/
#include "dicom.h"
#include "lst.h"
#include "dul.h"

/*
** Constant Definitions
*/

/* 
 * There have been reports that smaller PDUs work better in some environments. 
 * Allow a 4K minimum and a 128K maximum. Any further extension requires
 * modifications in the DUL code.
 */
#define ASC_DEFAULTMAXPDU       16384	/* 16K is default if nothing else specified */
#define ASC_MINIMUMPDUSIZE       4096
#define ASC_MAXIMUMPDUSIZE     131072	/* 128K - we only handle this big */


#define DIC_EURO_UIDROOT			OFFIS_UID_ROOT
#define DIC_EURO_IMPLEMENTATIONCLASSUID		OFFIS_IMPLEMENTATION_CLASS_UID
#define DIC_EURO_IMPLEMENTATIONVERSIONNAME	OFFIS_DTK_IMPLEMENTATION_VERSION_NAME

/*
** Type Definitions
*/

/*
** Network initialization structure.
** Is a wrapper for DUL functionality.
*/

typedef enum {
    NET_ACCEPTOR,		/* Provider Only */
    NET_REQUESTOR,		/* User Only */
    NET_ACCEPTORREQUESTOR	/* User and Provider */
} T_ASC_NetworkRole;

typedef struct {
    T_ASC_NetworkRole   role;
    int             	acceptorPort;
    DUL_NETWORKKEY      *network;
} T_ASC_Network;


/*
** Association negotiation parameters.
**
*/


/* not defined anywhere (I think) but a hard limitation for now.
 * DICOM (1998) defines 22 transfer syntaxes, this upper limit
 * should allow for sufficiently many private transfer syntaxes.
 */
#define DICOM_MAXTRANSFERSYNTAXES	50


typedef DUL_PRESENTATIONCONTEXTID T_ASC_PresentationContextID;

typedef enum { /* Part 8, pp 45. */
    ASC_P_ACCEPTANCE             	= 0,
    ASC_P_USERREJECTION             	= 1,
    ASC_P_NOREASON                 	= 2,
    ASC_P_ABSTRACTSYNTAXNOTSUPPORTED    = 3,
    ASC_P_TRANSFERSYNTAXESNOTSUPPORTED  = 4,
    ASC_P_NOTYETNEGOTIATED            	= 255
} T_ASC_P_ResultReason; 

typedef enum {
    ASC_SC_ROLE_DEFAULT,
    ASC_SC_ROLE_SCU,
    ASC_SC_ROLE_SCP,
    ASC_SC_ROLE_SCUSCP
} T_ASC_SC_ROLE;

typedef struct {
    T_ASC_PresentationContextID presentationContextID;
    DIC_UI 			abstractSyntax;
    unsigned char 		transferSyntaxCount;
    DIC_UI 		proposedTransferSyntaxes[DICOM_MAXTRANSFERSYNTAXES];
    DIC_UI			acceptedTransferSyntax;
    T_ASC_P_ResultReason 	resultReason;
    T_ASC_SC_ROLE 		proposedRole;
    T_ASC_SC_ROLE		acceptedRole;
} T_ASC_PresentationContext;

typedef enum { 
    ASC_RESULT_REJECTEDPERMANENT			= 1, 
    ASC_RESULT_REJECTEDTRANSIENT			= 2
} T_ASC_RejectParametersResult;

typedef enum {
    ASC_SOURCE_SERVICEUSER                          = 1,
    ASC_SOURCE_SERVICEPROVIDER_ACSE_RELATED         = 2,
    ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED = 3
} T_ASC_RejectParametersSource;

typedef enum { /* the following values are coded by DUL */
    /* Service User reasons */
    ASC_REASON_SU_NOREASON                          = 0x0101,
    ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED        = 0x0102,
    ASC_REASON_SU_CALLINGAETITLENOTRECOGNIZED       = 0x0103,
    ASC_REASON_SU_CALLEDAETITLENOTRECOGNIZED        = 0x0107,
    /* Service Provider ACSE Related reasons */
    ASC_REASON_SP_ACSE_NOREASON                     = 0x0201,
    ASC_REASON_SP_ACSE_PROTOCOLVERSIONNOTSUPPORTED  = 0x0202,
    /* Service Provider Presentation Related reasons */
    ASC_REASON_SP_PRES_TEMPORARYCONGESTION          = 0x0301,
    ASC_REASON_SP_PRES_LOCALLIMITEXCEEDED           = 0x0302
} T_ASC_RejectParametersReason;

typedef struct {
    T_ASC_RejectParametersResult result;
    T_ASC_RejectParametersSource source;
    T_ASC_RejectParametersReason reason;
} T_ASC_RejectParameters;


typedef struct {
    DIC_UI ourImplementationClassUID;
    DIC_SH ourImplementationVersionName;
    DIC_UI theirImplementationClassUID;
    DIC_SH theirImplementationVersionName;

    DUL_ASSOCIATESERVICEPARAMETERS DULparams;
    /*
     * DICOM Upper Layer service parameters.  They should only be
     * accessed via functions defined below. 
     */

    long ourMaxPDUReceiveSize;		/* we say what we can receive */
    long theirMaxPDUReceiveSize;	/* they say what we can send */

} T_ASC_Parameters;

/*
** Association structure containing all association specific
** information. 
*/
typedef struct {
    DUL_ASSOCIATIONKEY *DULassociation;
    T_ASC_Parameters *params;

    unsigned short nextMsgID;	/* should be incremented by user */

    unsigned long sendPDVLength;	/* max length of PDV to send out */
    unsigned char *sendPDVBuffer;	/* buffer of size sendPDVLength */

} T_ASC_Association;


/*
** Public Function Prototypes
*/

/* 
 * Network creation/destroy wrappers.
 * The T_ASC_Network structure will be allocated/freed by
 * these routines.
 */

OFCondition 
ASC_initializeNetwork(
    T_ASC_NetworkRole role,
    int acceptorPort,
    int timeout,
    T_ASC_Network ** network);

OFCondition 
ASC_dropNetwork(T_ASC_Network ** network);

/*
 * Building Association parameters
 */

OFCondition
ASC_createAssociationParameters(
    T_ASC_Parameters ** params,
    long maxReceivePDUSize);

OFCondition 
ASC_destroyAssociationParameters(
    T_ASC_Parameters ** params);

/* set transport layer type in association parameters */
OFCondition
ASC_setTransportLayerType(
    T_ASC_Parameters * params,
    OFBool useSecureLayer);

OFCondition 
ASC_setAPTitles(
    T_ASC_Parameters * params,
    const char* callingAPTitle,
    const char* calledAPTitle,
    const char* respondingAPTitle);

OFCondition 
ASC_getAPTitles(
    T_ASC_Parameters * params,
    char* callingAPTitle,
    char* calledAPTitle,
    char* respondingAPTitle);

OFCondition 
ASC_getApplicationContextName(
    T_ASC_Parameters * params,
    char* applicationContextName);

OFCondition 
ASC_setPresentationAddresses(
    T_ASC_Parameters * params,
    const char* callingPresentationAddress,
    const char* calledPresentationAddress);

OFCondition 
ASC_getPresentationAddresses(
    T_ASC_Parameters * params,
    char* callingPresentationAddress,
    char* calledPresentationAddress);

OFCondition 
ASC_getRejectParameters(
    T_ASC_Parameters * params,
    T_ASC_RejectParameters * rejectParameters);

void 
ASC_printRejectParameters(
    FILE *f, 
    T_ASC_RejectParameters *rej);

void 
ASC_printRejectParameters(
    ostream &out, 
    T_ASC_RejectParameters *rej);

OFCondition 
ASC_addPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    const char* abstractSyntax,
    const char* transferSyntaxList[],
    int transferSyntaxListCount,
    T_ASC_SC_ROLE proposedRole = ASC_SC_ROLE_DEFAULT);

int 
ASC_countPresentationContexts(
    T_ASC_Parameters * params);

int 
ASC_countAcceptedPresentationContexts(
    T_ASC_Parameters * params);

OFCondition 
ASC_getPresentationContext(
    T_ASC_Parameters * params,
    int listPosition,
    T_ASC_PresentationContext * presentationContext);

OFCondition 
ASC_acceptPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    const char* transferSyntax,
    T_ASC_SC_ROLE acceptedRole = ASC_SC_ROLE_DEFAULT);

OFCondition
ASC_acceptContextsWithPreferredTransferSyntaxes(
    T_ASC_Parameters * params,
    const char* abstractSyntaxes[], int abstractSyntaxCount,
    const char* transferSyntaxes[], int transferSyntaxCount,
    T_ASC_SC_ROLE acceptedRole = ASC_SC_ROLE_DEFAULT);

OFCondition
ASC_acceptContextsWithTransferSyntax(
    T_ASC_Parameters * params,
    const char* transferSyntax, int abstractSyntaxCount,
    const char* abstractSyntaxes[],
    T_ASC_SC_ROLE acceptedRole = ASC_SC_ROLE_DEFAULT);

OFCondition 
ASC_refusePresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    T_ASC_P_ResultReason resultReason);

OFCondition
ASC_findAcceptedPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    T_ASC_PresentationContext * presentationContext);

T_ASC_PresentationContextID
ASC_findAcceptedPresentationContextID(
    T_ASC_Association *assoc,
    const char* abstractSyntax);

T_ASC_PresentationContextID
ASC_findAcceptedPresentationContextID(
    T_ASC_Association *assoc,
    const char* abstractSyntax,
    const char * transferSyntax);

/* extended negotiation */
void ASC_getRequestedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList** extNegList);
void ASC_getAcceptedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList** extNegList);
void ASC_setRequestedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList* extNegList);
void ASC_setAcceptedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList* extNegList);

/* get peer certificate from open association */
unsigned long ASC_getPeerCertificateLength(T_ASC_Association *assoc);
unsigned long ASC_getPeerCertificate(T_ASC_Association *assoc, void *buf, unsigned long bufLen);

/* set new transport layer object */
OFCondition
ASC_setTransportLayer(T_ASC_Network *network, DcmTransportLayer *newLayer, int takeoverOwnership);

void 
ASC_dumpParameters(T_ASC_Parameters * params, ostream& outstream);

void 
ASC_dumpPresentationContext(T_ASC_PresentationContext * presentationContext, ostream& outstream);

void
ASC_dumpConnectionParameters(T_ASC_Association *association, ostream& outstream);

/*
 * Association Inquiries
 */

OFBool
ASC_associationWaiting(T_ASC_Network * network, int timeout);

OFBool
ASC_dataWaiting(T_ASC_Association * association, int timeout);

OFBool
ASC_selectReadableAssociation(
    T_ASC_Association* assocs[], 
    int assocCount, int timeout);

/*
 * Association Messages
 */

OFCondition 
ASC_requestAssociation(
    T_ASC_Network * network,
    T_ASC_Parameters * params,	/* params will be saved
				 * in the association
				 * structure */
    T_ASC_Association ** association,
    void **associatePDU=NULL,
    unsigned long *associatePDUlength=NULL);

OFCondition 
ASC_receiveAssociation(
    T_ASC_Network * network,
    T_ASC_Association ** association,
    long maxReceivePDUSize,
    void **associatePDU=NULL,
    unsigned long *associatePDUlength=NULL,
    OFBool useSecureLayer=OFFalse);

OFCondition
ASC_acknowledgeAssociation(
    T_ASC_Association * assoc,
    void **associatePDU=NULL,
    unsigned long *associatePDUlength=NULL);

OFCondition
ASC_rejectAssociation(
    T_ASC_Association * association,
    T_ASC_RejectParameters * rejectParameters,
    void **associatePDU=NULL,
    unsigned long *associatePDUlength=NULL);

OFCondition 
ASC_releaseAssociation(T_ASC_Association * association);

OFCondition 
ASC_acknowledgeRelease(T_ASC_Association * association);

OFCondition 
ASC_abortAssociation(T_ASC_Association * association);

OFCondition 
ASC_dropSCPAssociation(T_ASC_Association * association);

OFCondition 
ASC_dropAssociation(T_ASC_Association * association);

OFCondition 
ASC_destroyAssociation(T_ASC_Association ** association);


#endif

/*
** CVS Log
** $Log: assoc.h,v $
** Revision 1.17  2001-10-12 10:18:25  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.16  2001/09/28 13:23:46  joergr
** Added forward declaration of "ostream" to keep gcc 3.0 quiet.
**
** Revision 1.15  2000/10/10 12:06:05  meichel
** Added version of function ASC_printRejectParameters that takes
**   an ostream& instead of a FILE*
**
** Revision 1.14  2000/08/10 14:50:52  meichel
** Added initial OpenSSL support.
**
** Revision 1.13  2000/06/07 13:56:20  meichel
** Output stream now passed as mandatory parameter to ASC_dumpParameters.
**
** Revision 1.12  2000/06/07 08:57:21  meichel
** dcmnet ACSE routines now allow to retrieve a binary copy of the A-ASSOCIATE
**   RQ/AC/RJ PDUs, e.g. for logging purposes.
**
** Revision 1.11  2000/05/30 13:07:41  joergr
** Added a condition value to report the external request for a shutdown of
** the application (used for imagectn).
**
** Revision 1.10  1999/09/06 13:28:31  meichel
** Enhanced max receive PDU range to 4-128K.
**
** Revision 1.9  1999/04/26 17:20:55  meichel
** Added new "transfer syntax aware" variant of the dcmnet function
**   ASC_findAcceptedPresentationContextID. This variant tries to find an
**   accepted presentation context that matches both abstract and transfer syntax.
**
** Revision 1.8  1999/04/21 13:01:23  meichel
** Increased max. number of transfer syntaxes that
**   can be managed in an A-ASSOCIATE packet from 25 to 50.
**
** Revision 1.7  1999/04/19 08:39:27  meichel
** Added experimental support for extended SOP class negotiation.
**
** Revision 1.6  1997/08/05 07:38:08  andreas
** Corrected error in DUL finite state machine
** SCPs shall close sockets after the SCU have closed the socket in
** a normal association release. Therfore, an ARTIM timer is described
** in DICOM part 8 that is not implemented correctly in the
** DUL. Since the whole DUL finite state machine is affected, we
** decided to solve the proble outside the fsm. Now it is necessary to call the
** ASC_DropSCPAssociation() after the calling ASC_acknowledgeRelease().
**
** Revision 1.5  1997/07/21 08:40:08  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.4  1997/05/05 13:05:53  meichel
** Removed unused constant DICOM_MAXPRESENTATIONCONTEXTS.
**
** Revision 1.3  1997/05/05 10:30:08  meichel
** Fixed bugs related to association negotiation in the DICOM upper layer module.
** Added application tests/assctest.cc to examine handling of large A-ASSOCIATE
** PDUs. See CHANGES file for details.
**
** Revision 1.2  1996/04/25 16:06:27  hewett
** Replaced declarations of DIC_UL with unsigned long.
**
** Revision 1.1.1.1  1996/03/26 18:38:44  hewett
** Initial Release.
**
**
*/
