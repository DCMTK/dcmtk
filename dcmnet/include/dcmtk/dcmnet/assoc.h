/*
 *
 *  Copyright (C) 1994-2021, OFFIS e.V.
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
**      information.  Also provided are routines for aiding association
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
    NET_ACCEPTOR,           /* Provider Only */
    NET_REQUESTOR,          /* User Only */
    NET_ACCEPTORREQUESTOR   /* User and Provider */
};

struct DCMTK_DCMNET_EXPORT T_ASC_Network
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

struct DCMTK_DCMNET_EXPORT T_ASC_PresentationContext
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

struct DCMTK_DCMNET_EXPORT T_ASC_RejectParameters
{
    T_ASC_RejectParametersResult result;
    T_ASC_RejectParametersSource source;
    T_ASC_RejectParametersReason reason;
};


struct DCMTK_DCMNET_EXPORT T_ASC_Parameters
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
struct DCMTK_DCMNET_EXPORT T_ASC_Association
{
    DUL_ASSOCIATIONKEY *DULassociation;
    T_ASC_Parameters *params;

    unsigned short nextMsgID;     /* should be incremented by user */
    unsigned long sendPDVLength;  /* max length of PDV to send out */
    unsigned char *sendPDVBuffer; /* buffer of size sendPDVLength */
};

/*
** Public Function Prototypes
*/

/*
 * Network creation/destroy wrappers.
 * The T_ASC_Network structure will be allocated/freed by
 * these routines.
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
DCMTK_DCMNET_EXPORT OFCondition ASC_initializeNetwork(
    T_ASC_NetworkRole role,
    int acceptorPort,
    int timeout,
    T_ASC_Network ** network,
    unsigned long options = 0);

/** network instance destruction function (destructor)
 *  @param network T_ASC_Network will be freed by this routine
 *  @return EC_Normal if successful, an error code otherwise
 */
DCMTK_DCMNET_EXPORT OFCondition ASC_dropNetwork(T_ASC_Network ** network);

/*
 * Building Association parameters
 */

/* create association parameters and initialize with default values */
DCMTK_DCMNET_EXPORT OFCondition
ASC_createAssociationParameters(
    T_ASC_Parameters ** params,
    long maxReceivePDUSize);

 /*
  * Free an association parameters structure and embedded information.
  * You do not usually need to do this since the parameters structure will
  * be noted in the association structure and automatically freed when an
  * association terminates.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_destroyAssociationParameters(
    T_ASC_Parameters ** params);

/* set transport layer type in association parameters */
DCMTK_DCMNET_EXPORT OFCondition
ASC_setTransportLayerType(
    T_ASC_Parameters * params,
    OFBool useSecureLayer);

 /*
  * Copies the provided Application Titles in the association parameters.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_setAPTitles(
    T_ASC_Parameters * params,
    const char* callingAPTitle,
    const char* calledAPTitle,
    const char* respondingAPTitle);

 /*
  * Copies the Application Titles stored in the association parameters
  * into the supplied string variables.  You must provide storage to copy
  * into.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_getAPTitles(
    T_ASC_Parameters * params,
    char* callingAPTitle,
    size_t callingAPTitleSize,
    char* calledAPTitle,
    size_t calledAPTitleSize,
    char* respondingAPTitle,
    size_t respondingAPTitleSize);

 /*
  * Copies the Application Context Name stored in the association parameters
  * into the supplied string variable.  You must provide storage to copy
  * into.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_getApplicationContextName(
    T_ASC_Parameters * params,
    char* applicationContextName,
    size_t applicationContextNameSize);

 /*
  * Copies the provided Presentation Addresses into the association
  * parameters.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_setPresentationAddresses(
    T_ASC_Parameters * params,
    const char* callingPresentationAddress,
    const char* calledPresentationAddress);

/*
  * Copies the Presentation Addresses stored in the association parameters
  * into the supplied string variables.  You must provide storage to copy
  * into.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_getPresentationAddresses(
    T_ASC_Parameters * params,
    char* callingPresentationAddress,
    size_t callingPresentationAddressSize,
    char* calledPresentationAddress,
    size_t calledPresentationAddressSize);

 /*
  * Copies the Rejection Parameters stored in the association parameters into
  * the supplied structure.  You must provide storage to copy into.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_getRejectParameters(
    T_ASC_Parameters * params,
    T_ASC_RejectParameters * rejectParameters);

DCMTK_DCMNET_EXPORT OFString&
ASC_printRejectParameters(
    OFString& str,
    const T_ASC_RejectParameters *rej);

 /*
  * Adds a presentation context entry to the presentation context list.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_addPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    const char* abstractSyntax,
    const char* transferSyntaxList[],
    int transferSyntaxListCount,
    T_ASC_SC_ROLE proposedRole = ASC_SC_ROLE_DEFAULT);

 /*
  * Returns the number of presentation contexts contained in the presentation
  * context list.
  */
DCMTK_DCMNET_EXPORT int
ASC_countPresentationContexts(
    T_ASC_Parameters * params);

DCMTK_DCMNET_EXPORT int
ASC_countAcceptedPresentationContexts(
    T_ASC_Parameters * params);

 /*
  * You must supply the memory for presentationContext, the values stored in
  * the presentation context list position indicated will be copied into the
  * memory structure.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_getPresentationContext(
    T_ASC_Parameters * params,
    int listPosition,
    T_ASC_PresentationContext * presentationContext);

 /*
  * The presentation context will be marked as accepted and the provided
  * transfer syntax name chosen.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_acceptPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    const char* transferSyntax,
    T_ASC_SC_ROLE acceptedRole = ASC_SC_ROLE_DEFAULT,
    const OFBool alwaysAcceptDefaultRole = OFFalse);

DCMTK_DCMNET_EXPORT OFCondition
ASC_acceptContextsWithPreferredTransferSyntaxes(
    T_ASC_Parameters * params,
    const char* abstractSyntaxes[], int abstractSyntaxCount,
    const char* transferSyntaxes[], int transferSyntaxCount,
    T_ASC_SC_ROLE acceptedRole = ASC_SC_ROLE_DEFAULT);

/*
  * Any proposed presentation contexts which are found abstractSyntaxes[]
  * which also have proposed a transfer syntax of transferSyntax, will be
  * accepted.  Any presentation contexts already marked as accepted will be
  * left alone but any remaining presentation contexts will be refused.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_acceptContextsWithTransferSyntax(
    T_ASC_Parameters * params,
    const char* transferSyntax, int abstractSyntaxCount,
    const char* abstractSyntaxes[],
    T_ASC_SC_ROLE acceptedRole = ASC_SC_ROLE_DEFAULT);

 /*
  * The presentation context will be marked as refused.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_refusePresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    T_ASC_P_ResultReason resultReason);

 /*
  * ASC_findAcceptedPresentationContext: You must supply the memory for
  * presentationContext, the values stored in the accepted presentation
  * context list with given ID will be copied into the memory structure.
  * Returns EC_Normal if found, or ASC_BADPRESENTATIONCONTEXTID if not
  * found.
  */
DCMTK_DCMNET_EXPORT OFCondition
ASC_findAcceptedPresentationContext(
    T_ASC_Parameters * params,
    T_ASC_PresentationContextID presentationContextID,
    T_ASC_PresentationContext * presentationContext);

/* ASC_findAcceptedPresentationContextID:
 * Searches in the accepted presentation context list for the given
 * abstract syntax.  If found returns its PresentationContextID, otherwise
 * returns 0 (which is not a valid ID).
 */
DCMTK_DCMNET_EXPORT T_ASC_PresentationContextID
ASC_findAcceptedPresentationContextID(
    T_ASC_Association *assoc,
    const char* abstractSyntax);

/* transfer syntax aware version of T_ASC_PresentationContextID.
 * Tries to find a presentation context that matches the characteristics
 * of the given DICOM dataset best
 * - if possible finds a presentation context with matching TS
 * - then tries to find an explicit VR uncompressed TS presentation ctx
 * - then tries to find an implicit VR uncompressed TS presentation ctx
 * - finally accepts each matching presentation ctx independent of TS.
 * Returns 0 if no appropriate presentation context could be found at all.
 */
DCMTK_DCMNET_EXPORT T_ASC_PresentationContextID
ASC_findAcceptedPresentationContextID(
    T_ASC_Association *assoc,
    const char* abstractSyntax,
    const char * transferSyntax);

/* extended negotiation */
DCMTK_DCMNET_EXPORT void ASC_getRequestedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList** extNegList);
DCMTK_DCMNET_EXPORT void ASC_getAcceptedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList** extNegList);
DCMTK_DCMNET_EXPORT void ASC_setRequestedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList* extNegList);
DCMTK_DCMNET_EXPORT void ASC_setAcceptedExtNegList(T_ASC_Parameters* params, SOPClassExtendedNegotiationSubItemList* extNegList);

/* user identity negotiation */

/* function that returns user identity request structure from association
 * parameters.
 * @param params - [in] The parameters to read from
 * @param usrIdentAC - [out] The result pointer to user identity request
 */
DCMTK_DCMNET_EXPORT void ASC_getUserIdentRQ(T_ASC_Parameters* params, UserIdentityNegotiationSubItemRQ** usrIdentRQ);

/* function that returns user identity response structure from association
 * parameters. Note: For accessing the User Identity response value,
 *  it is more convenient to use the function ASC_getCopyOfIdentResponse().
 * @param params - [in] The parameters to read from
 * @param usrIdentAC - [out] The result pointer to user identity response
 * @return none
 */
DCMTK_DCMNET_EXPORT void ASC_getUserIdentAC(T_ASC_Parameters* params, UserIdentityNegotiationSubItemAC** usrIdentAC);

/** Sets User/Password authentication for User Identity Negotiation
 *  request.
 *  @param params - [in/out] The association parameters to be filled
 *  @param userName - [in]  The username to send (in UTF-8)
 *  @password - [in] Password in UTF-8
 *  @return EC_Normal if user identity could be set, error otherwise
 */
DCMTK_DCMNET_EXPORT OFCondition
ASC_setIdentRQUserPassword(
    T_ASC_Parameters * params,
    const OFString& userName,
    const OFString& password,
    const OFBool requestRsp = OFTrue);

/** Sets User authentication (no password) for User Identity Negotiation
 *  request.
 *  @param params - [in/out] The association parameters to be filled
 *  @param userName - [in]  The username to send (in UTF-8)
 *  @return EC_Normal if user identity could be set, error otherwise
 */
DCMTK_DCMNET_EXPORT OFCondition
ASC_setIdentRQUserOnly(
    T_ASC_Parameters * params,
    const OFString& userName,
    const OFBool requestRsp = OFTrue);

/** Sets Kerberos authentication for User Identity Negotiation request.
 *  @param params     - [in/out] The association parameters to be filled
 *  @param kerbTicket - [in]  The kerberos ticket to send (will be copied)
 *  @param length     - [in] Length of kerberos ticket
 *  @return EC_Normal if kerberos ticket could be set, error otherwise
 */
DCMTK_DCMNET_EXPORT OFCondition
ASC_setIdentRQKerberos(
    T_ASC_Parameters * params,
    const char* kerbTicket,
    const Uint16 length,
    const OFBool requestRsp = OFTrue);

/** Sets SAML authentication for User Identity Negotiation request.
 *  @param params - [in/out] The association parameters to be filled
 *  @param saml   - [in]  The SAML information to send (will be copied)
 *  @param length - [in] Length of SAML information
 *  @return EC_Normal if SAML info could be set, error otherwise
 */
DCMTK_DCMNET_EXPORT OFCondition
ASC_setIdentRQSaml(
    T_ASC_Parameters * params,
    const char* saml,
    const Uint16 length,
    const OFBool requestRsp = OFTrue);

/** Sets JSON Web Token (JWT) authentication for User Identity Negotiation
 *  request.
 *  @param params - [in/out] The association parameters to be filled
 *  @param jwt    - [in]  The JWT information to send (will be copied)
 *  @param length - [in] Length of JWT information
 *  @return EC_Normal if JWT info could be set, error otherwise
 */
DCMTK_DCMNET_EXPORT OFCondition
ASC_setIdentRQJwt(
    T_ASC_Parameters * params,
    const char* jwt,
    const Uint16 length,
    const OFBool requestRsp = OFTrue);

/** Acknowledges a User Identity Negotiation request.
 *  @param params - [in/out] The association parameters to be filled
 *  @param response  - [in]  The response to return (Kerberos or SAML) (will be copied)
 *  @param length - [in] Length of response
 *  @return EC_Normal if response could be set, error otherwise
 */
DCMTK_DCMNET_EXPORT OFCondition ASC_setIdentAC(
    T_ASC_Parameters * params,
    const char* response,
    const Uint16 length);

/** Returns a copy of the User Identity Negotiation response value.
 *  CAUTION: The returned buffer (copy of original data) must be freed by the
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
DCMTK_DCMNET_EXPORT void
ASC_getCopyOfIdentResponse(T_ASC_Parameters * params,
                           char*& buffer,
                           unsigned short& bufferLen);

/* TLS/SSL */

/* get peer certificate from open association */
DCMTK_DCMNET_EXPORT unsigned long ASC_getPeerCertificateLength(T_ASC_Association *assoc);
DCMTK_DCMNET_EXPORT unsigned long ASC_getPeerCertificate(T_ASC_Association *assoc, void *buf, unsigned long bufLen);

/* set new transport layer object */
DCMTK_DCMNET_EXPORT OFCondition
ASC_setTransportLayer(T_ASC_Network *network, DcmTransportLayer *newLayer, int takeoverOwnership);

enum ASC_associateType
{
    ASC_ASSOC_RQ,
    ASC_ASSOC_AC,
    ASC_ASSOC_RJ
};

DCMTK_DCMNET_EXPORT OFString&
ASC_dumpParameters(OFString& str, T_ASC_Parameters * param, ASC_associateType dir);

DCMTK_DCMNET_EXPORT OFString&
ASC_dumpConnectionParameters(OFString& str, T_ASC_Association *association);

DCMTK_DCMNET_EXPORT void ASC_activateCallback(T_ASC_Parameters *params, DUL_ModeCallback *cb);

/*
 * Association Inquiries
 */

DCMTK_DCMNET_EXPORT OFBool
ASC_associationWaiting(T_ASC_Network * network, int timeout);

DCMTK_DCMNET_EXPORT OFBool
ASC_dataWaiting(T_ASC_Association * association, int timeout);

DCMTK_DCMNET_EXPORT OFBool
ASC_selectReadableAssociation(
    T_ASC_Association* assocs[],
    int assocCount, int timeout);

/*
 * Association Messages
 */

DCMTK_DCMNET_EXPORT OFCondition
ASC_requestAssociation(
    T_ASC_Network * network,
    T_ASC_Parameters * params,  /* params will be saved in the association structure */
    T_ASC_Association ** association,
    void **associatePDU=NULL,
    unsigned long *associatePDUlength=NULL,
    DUL_BLOCKOPTIONS block=DUL_BLOCK,
    int timeout=0);

DCMTK_DCMNET_EXPORT OFCondition
ASC_receiveAssociation(
    T_ASC_Network * network,
    T_ASC_Association ** association,
    long maxReceivePDUSize,
    void **associatePDU=NULL,
    unsigned long *associatePDUlength=NULL,
    OFBool useSecureLayer=OFFalse,
    DUL_BLOCKOPTIONS block=DUL_BLOCK,
    int timeout=0);

DCMTK_DCMNET_EXPORT OFCondition
ASC_acknowledgeAssociation(
    T_ASC_Association * assoc,
    void **associatePDU=NULL,
    unsigned long *associatePDUlength=NULL);

DCMTK_DCMNET_EXPORT OFCondition
ASC_rejectAssociation(
    T_ASC_Association * association,
    const T_ASC_RejectParameters * rejectParameters,
    void **associatePDU=NULL,
    unsigned long *associatePDUlength=NULL);

DCMTK_DCMNET_EXPORT OFCondition
ASC_releaseAssociation(T_ASC_Association * association);

DCMTK_DCMNET_EXPORT OFCondition
ASC_acknowledgeRelease(T_ASC_Association * association);

DCMTK_DCMNET_EXPORT OFCondition
ASC_abortAssociation(T_ASC_Association * association);

DCMTK_DCMNET_EXPORT OFCondition
ASC_dropSCPAssociation(T_ASC_Association * association, int timeout = DUL_TIMEOUT);

DCMTK_DCMNET_EXPORT OFCondition
ASC_dropAssociation(T_ASC_Association * association);

DCMTK_DCMNET_EXPORT OFCondition
ASC_closeTransportConnection(T_ASC_Association * association);

DCMTK_DCMNET_EXPORT OFCondition
ASC_destroyAssociation(T_ASC_Association ** association);

/// @deprecated Please use OFString& ASC_printRejectParameters(OFString&, T_ASC_RejectParameters*) instead.
DCMTK_DCMNET_EXPORT void
ASC_printRejectParameters(
    FILE *f,
    const T_ASC_RejectParameters *rej);

/// @deprecated Please use OFString& ASC_printRejectParameters(OFString&, T_ASC_RejectParameters*) instead.
DCMTK_DCMNET_EXPORT void
ASC_printRejectParameters(
    STD_NAMESPACE ostream& out,
    const T_ASC_RejectParameters *rej);

 /*
  * Write parameters in textual form to stdout (debugging aid)
  */
/**
 * @deprecated Please use OFString& ASC_dumpParameters(OFString&, T_ASC_Parameters *,
 *             ASC_associateType) instead.
 */
DCMTK_DCMNET_EXPORT void
ASC_dumpParameters(T_ASC_Parameters * params, STD_NAMESPACE ostream& outstream);

 /*
  * Write presentation context structure in textual form to stdout.
  * (debugging aid)
  */
/// @deprecated You should dump the complete T_ASC_Parameters with ASC_dumpParameters() instead.
DCMTK_DCMNET_EXPORT void
ASC_dumpPresentationContext(T_ASC_PresentationContext * presentationContext, STD_NAMESPACE ostream& outstream);

/**
 * @deprecated Please use OFString& ASC_dumpParameters(OFString&, T_ASC_Parameters*,
 *             ASC_associateType) instead.
 */
DCMTK_DCMNET_EXPORT void
ASC_dumpConnectionParameters(T_ASC_Association *association, STD_NAMESPACE ostream& outstream);

/** Converts given ASC role to string (e.g. for printing)
 *  @param  role The role to convert
    @return The role as a string
 */
DCMTK_DCMNET_EXPORT const char*
ASC_role2String(const T_ASC_SC_ROLE role);

/** Converts given ASC role to DUL role
 *  @param  role The role to convert
    @return The role as DUL role
 */
DCMTK_DCMNET_EXPORT DUL_SC_ROLE
ascRole2dulRole(const T_ASC_SC_ROLE role);

DCMTK_DCMNET_EXPORT void
destroyDULParamPresentationContextList(LST_HEAD ** lst);
#endif
