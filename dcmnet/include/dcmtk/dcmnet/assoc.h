/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were partly developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  For further copyrights, see the following paragraphs.
 *
 */

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
** Author: Andrew Hewett                Created: 03-06-93
**
** Module: association
**
** Purpose:
**      This file contains the routines which provide association management
**      for DICOM applications.  It maintains structures which describe
**      active associations and provides access to association specific
**      informtion.  Also provided are routines for aiding association
**      negotiation (presentation contexts, abstract syntaxes, transfer
**      syntaxes, maximum PDU length, and other extended negotiation).
**
**      This package uses the facilities of the DICOM Upper Layer for
**      receiving/sending association requests/responses.
**
**      Each active association is represented by an T_ASC_Association
**      structure which contains all relevant information.
**
** Module Prefix: ASC_
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-12-01 08:26:10 $
** CVS/RCS Revision:    $Revision: 1.33 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
*/


#ifndef ASSOCIATION_H
#define ASSOCIATION_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

/*
** Required Include Files
*/
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/lst.h"
#include "dcmtk/dcmnet/dul.h"

/*
** Constant Definitions
*/

/*
 * There have been reports that smaller PDUs work better in some environments.
 * Allow a 4K minimum and a 128K maximum. Any further extension requires
 * modifications in the DUL code.
 */
#define ASC_DEFAULTMAXPDU       16384 /* 16K is default if nothing else specified */
#define ASC_MINIMUMPDUSIZE       4096
#define ASC_MAXIMUMPDUSIZE     131072 /* 128K - we only handle this big */

/*
** Type Definitions
*/

/*
** Network initialization structure.
** Is a wrapper for DUL functionality.
*/

enum T_ASC_NetworkRole
{
    NET_ACCEPTOR,   /* Provider Only */
    NET_REQUESTOR,    /* User Only */
    NET_ACCEPTORREQUESTOR /* User and Provider */
};

struct T_ASC_Network
{
    T_ASC_NetworkRole   role;
    int               acceptorPort;
    DUL_NETWORKKEY      *network;
};


/*
** Association negotiation parameters.
**
*/


/* not defined anywhere (I think) but a hard limitation for now.
 * DICOM (1998) defines 22 transfer syntaxes, this upper limit
 * should allow for sufficiently many private transfer syntaxes.
 */
#define DICOM_MAXTRANSFERSYNTAXES 50


typedef DUL_PRESENTATIONCONTEXTID T_ASC_PresentationContextID;

enum T_ASC_P_ResultReason
{ /* Part 8, pp 45. */
    ASC_P_ACCEPTANCE              = 0,
    ASC_P_USERREJECTION               = 1,
    ASC_P_NOREASON                  = 2,
    ASC_P_ABSTRACTSYNTAXNOTSUPPORTED    = 3,
    ASC_P_TRANSFERSYNTAXESNOTSUPPORTED  = 4,
    ASC_P_NOTYETNEGOTIATED              = 255
};

enum T_ASC_SC_ROLE
{
    ASC_SC_ROLE_NONE,
    ASC_SC_ROLE_DEFAULT,
    ASC_SC_ROLE_SCU,
    ASC_SC_ROLE_SCP,
    ASC_SC_ROLE_SCUSCP
};

struct T_ASC_PresentationContext
{
    T_ASC_PresentationContextID presentationContextID;
    DIC_UI      abstractSyntax;
    unsigned char     transferSyntaxCount;
    DIC_UI    proposedTransferSyntaxes[DICOM_MAXTRANSFERSYNTAXES];
    DIC_UI      acceptedTransferSyntax;
    T_ASC_P_ResultReason  resultReason;
    T_ASC_SC_ROLE     proposedRole;
    T_ASC_SC_ROLE   acceptedRole;
};

enum T_ASC_RejectParametersResult
{
    ASC_RESULT_REJECTEDPERMANENT      = 1,
    ASC_RESULT_REJECTEDTRANSIENT      = 2
};

enum T_ASC_RejectParametersSource
{
    ASC_SOURCE_SERVICEUSER                          = 1,
    ASC_SOURCE_SERVICEPROVIDER_ACSE_RELATED         = 2,
    ASC_SOURCE_SERVICEPROVIDER_PRESENTATION_RELATED = 3
};

enum T_ASC_RejectParametersReason
{ /* the following values are coded by DUL */
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
};

struct T_ASC_RejectParameters
{
    T_ASC_RejectParametersResult result;
    T_ASC_RejectParametersSource source;
    T_ASC_RejectParametersReason reason;
};


struct T_ASC_Parameters
{
    DIC_UI ourImplementationClassUID;
    DIC_SH ourImplementationVersionName;
    DIC_UI theirImplementationClassUID;
    DIC_SH theirImplementationVersionName;
    DUL_ModeCallback *modeCallback;

    DUL_ASSOCIATESERVICEPARAMETERS DULparams;
    /*
     * DICOM Upper Layer service parameters.  They should only be
     * accessed via functions defined below.
     */

    long ourMaxPDUReceiveSize;    /* we say what we can receive */
    long theirMaxPDUReceiveSize;  /* they say what we can send */

};

/*
** Association structure containing all association specific
** information.
*/
struct T_ASC_Association
{
    DUL_ASSOCIATIONKEY *DULassociation;
    T_ASC_Parameters *params;

    unsigned short nextMsgID;         /* should be incremented by user */
    unsigned long sendPDVLength;  /* max length of PDV to send out */
    unsigned char *sendPDVBuffer; /* buffer of size sendPDVLength */
};

/*
** Public Function Prototypes
*/

/** network instance creation function (constructor)
 *  @param role association acceptor, requestor or both
 *  @param acceptorPort acceptor port for incoming connections.
 *    For association requestors, zero should be passed here.
 *  @param timeout timeout for network operations, in seconds
 *  @param network T_ASC_Network will be allocated and returned in this parameter
 *  @param options network options. Only DUL_FULLDOMAINNAME is currently defined
 *    as a possible option.
 *  @return EC_Normal if successful, an error code otherwise
 */
OFCondition ASC_initializeNetwork(
    T_ASC_NetworkRole role,
    int acceptorPort,
    int timeout,
    T_ASC_Network ** network,
    unsigned long options = 0);

/** network instance destruction function (destructor)
 *  @param network T_ASC_Network will be freed by this routine
 *  @return EC_Normal if successful, an error code otherwise
 */
OFCondition ASC_dropNetwork(T_ASC_Network ** network);

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

OFString&
ASC_printRejectParameters(
    OFString& str,
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

/* user identity negotiation */

/* function that returns user identity request structure from association
 * parameters.
 * @param params - [in] The parameters to read from
 * @param usrIdentAC - [out] The result pointer to user identity request
 */
void ASC_getUserIdentRQ(T_ASC_Parameters* params, UserIdentityNegotiationSubItemRQ** usrIdentRQ);

/* function that returns user identity response structure from association
 * parameters. Note: For accessing the User Identity response value,
 *  it is more convenient to use the function ASC_getCopyOfIdentResponse().
 * @param params - [in] The parameters to read from
 * @param usrIdentAC - [out] The result pointer to user identity response
 * @return none
 */
void ASC_getUserIdentAC(T_ASC_Parameters* params, UserIdentityNegotiationSubItemAC** usrIdentAC);

/** Sets User/Password authentication for User Identity Negotiation
 *  request.
 *  @param params - [in/out] The association parameters to be filled
 *  @param userName - [in]  The username to send (in UTF-8)
 *  @password - [in] Password in UTF-8
 *  @return EC_Normal if user identity could be set, error otherwise
 */
OFCondition
ASC_setIdentRQUserPassword(
    T_ASC_Parameters * params,
    const OFString& userName,
    const OFString& password,
    const OFBool& requestRsp = OFTrue);

/** Sets User authentication (no password) for User Identity Negotiation
 *  request.
 *  @param params - [in/out] The association parameters to be filled
 *  @param userName - [in]  The username to send (in UTF-8)
 *  @return EC_Normal if user identity could be set, error otherwise
 */
OFCondition
ASC_setIdentRQUserOnly(
    T_ASC_Parameters * params,
    const OFString& userName,
    const OFBool& requestRsp = OFTrue);

/** Sets Kerberos authentication for User Identity Negotiation request.
 *  @param params     - [in/out] The association parameters to be filled
 *  @param kerbTicket - [in]  The kerberos ticket to send (will be copied)
 *  @param length     - [in] Length of kerberos ticket
 *  @return EC_Normal if kerberos ticket could be set, error otherwise
 */
OFCondition
ASC_setIdentRQKerberos(
    T_ASC_Parameters * params,
    const char* kerbTicket,
    const Uint16& length,
    const OFBool& requestRsp = OFTrue);

/** Sets SAML authentication for User Identity Negotiation request.
 *  @param params - [in/out] The association parameters to be filled
 *  @param saml   - [in]  The SAML information to send (will be copied)
 *  @param length - [in] Length of SAML information
 *  @return EC_Normal if SAML info could be set, error otherwise
 */
OFCondition
ASC_setIdentRQSaml(
    T_ASC_Parameters * params,
    const char* saml,
    const Uint16& length,
    const OFBool& requestRsp = OFTrue);


/** Acknowledges a User Identity Negotiation request.
 *  @param params - [in/out] The association parameters to be filled
 *  @param response  - [in]  The response to return (Kerberos or SAML) (will be copied)
 *  @param length - [in] Length of response
 *  @return EC_Normal if response could be set, error otherwise
 */
OFCondition ASC_setIdentAC(
    T_ASC_Parameters * params,
    const char* response,
    const Uint16& length );

/** Returns a copy of the User Identity Negotiation response value.
 *  CAUTION: The returned buffer (copy of orginal data) must be freed by the
 *  caller!
 *  @param params - [in]  The association parameters to get response from
 *  @param buffer - [out] The buffer to write to. Memory is allocated inside
 *                        function, so the returned buffer memory must be freed
 *                        by the caller. If there is no response or there was
 *                        was a problem, NULL is returned here.
 *  @param length - [out] Length of returned buffer. If there is a problem or no
 *                        response at all, this is set to 0.
 *  @return none
 */
void
ASC_getCopyOfIdentResponse(T_ASC_Parameters * params,
                           void*& buffer,
                           unsigned short& bufferLen);

/* TLS/SSL */

/* get peer certificate from open association */
unsigned long ASC_getPeerCertificateLength(T_ASC_Association *assoc);
unsigned long ASC_getPeerCertificate(T_ASC_Association *assoc, void *buf, unsigned long bufLen);

/* set new transport layer object */
OFCondition
ASC_setTransportLayer(T_ASC_Network *network, DcmTransportLayer *newLayer, int takeoverOwnership);

enum ASC_associateType
{
    ASC_ASSOC_RQ,
    ASC_ASSOC_AC,
    ASC_ASSOC_RJ
};

OFString&
ASC_dumpParameters(OFString& str, T_ASC_Parameters * param, ASC_associateType dir);

OFString&
ASC_dumpConnectionParameters(OFString& str, T_ASC_Association *association);

void ASC_activateCallback(T_ASC_Parameters *params, DUL_ModeCallback *cb);

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
    T_ASC_Parameters * params,  /* params will be saved
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
    OFBool useSecureLayer=OFFalse,
    DUL_BLOCKOPTIONS block=DUL_BLOCK,
    int timeout=0);

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
ASC_dropSCPAssociation(T_ASC_Association * association, int timeout = DUL_TIMEOUT);

OFCondition
ASC_dropAssociation(T_ASC_Association * association);

OFCondition
ASC_destroyAssociation(T_ASC_Association ** association);

/// @deprecated Please use OFString& ASC_printRejectParameters(OFString&, T_ASC_RejectParameters*) instead.
void
ASC_printRejectParameters(
    FILE *f,
    T_ASC_RejectParameters *rej);

/// @deprecated Please use OFString& ASC_printRejectParameters(OFString&, T_ASC_RejectParameters*) instead.
void
ASC_printRejectParameters(
    STD_NAMESPACE ostream& out,
    T_ASC_RejectParameters *rej);

/**
 * @deprecated Please use OFString& ASC_dumpParameters(OFString&, T_ASC_Parameters *,
 *             ASC_associateType) instead.
 */
void
ASC_dumpParameters(T_ASC_Parameters * params, STD_NAMESPACE ostream& outstream);

/// @deprecated You should dump the complete T_ASC_Parameters with ASC_dumpParameters() instead.
void
ASC_dumpPresentationContext(T_ASC_PresentationContext * presentationContext, STD_NAMESPACE ostream& outstream);

/**
 * @deprecated Please use OFString& ASC_dumpParameters(OFString&, T_ASC_Parameters*,
 *             ASC_associateType) instead.
 */
void
ASC_dumpConnectionParameters(T_ASC_Association *association, STD_NAMESPACE ostream& outstream);


#endif

/*
** CVS Log
** $Log: assoc.h,v $
** Revision 1.33  2010-12-01 08:26:10  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.32  2010-10-14 13:17:21  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.31  2010-10-01 12:25:29  uli
** Fixed most compiler warnings in remaining modules.
**
** Revision 1.30  2010-08-26 09:26:41  joergr
** Fixed incorrect behavior of association acceptors during SCP/SCU role
** selection negotiation.
**
** Revision 1.29  2009-11-18 11:53:58  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.28  2008-10-07 09:07:47  onken
** Added code for accessing user identity from the server's side.
**
** Revision 1.27  2008-04-17 15:28:33  onken
** Reworked and extended User Identity Negotiation code.
**
** Revision 1.26  2007-09-07 08:49:12  onken
** Added basic support for Extended Negotiation of User Identity.
**
** Revision 1.25  2006/08/15 16:04:29  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.24  2005/12/08 16:02:06  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.23  2004/07/15 08:10:45  meichel
** Added optional timeout parameter to ASC_dropSCPAssociation().
**
** Revision 1.22  2004/04/07 10:22:09  meichel
** Added optional parameter to ASC_initializeNetwork that allows to pass
**   the DUL_FULLDOMAINNAME option to the DUL layer
**
** Revision 1.21  2004/02/25 12:31:15  meichel
** Added global option flag for compatibility with very old DCMTK releases in the
**   DICOM upper layer and ACSE code. Default is automatic handling, which should
**   work in most cases.
**
** Revision 1.20  2003/06/10 13:42:07  meichel
** Replaced unnamed C style structs by C++ declarations
**
** Revision 1.19  2002/07/10 11:43:55  meichel
** Replaced dcmnet specific definitions for implementation class UID and
**   version name by the constants defined in dcmdata.
**
** Revision 1.18  2001/11/27 09:54:33  wilkens
** Updated storescp. 6 new options (--output-directory, --sort-conc-studies,
** --exec-on-reception, --exec-on-eostudy, --rename-on-eostudy, and
** --eostudy-timeout) implemented (requirements from GO-Kard).
**
** Revision 1.17  2001/10/12 10:18:25  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.16  2001/09/28 13:23:46  joergr
** Added forward declaration of "ostream" to keep gcc 3.0 quiet.
**
** Revision 1.15  2000/10/10 12:06:05  meichel
** Added version of function ASC_printRejectParameters that takes
**   an STD_NAMESPACE ostream& instead of a FILE*
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
