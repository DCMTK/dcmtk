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
**		     Electronic Radiology Laboratory
**		   Mallinckrodt Institute of Radiology
**		Washington University School of Medicine
**
** Module Name(s):	constructAssociatePDU
**			constructAssociateRejectPDU
**			constructReleasePDU
**			constructDataPDU
**			streamAssociatePDU
**			streamRejectReleaseAbortPDU
**			streamDataPDUHead
** Author, Date:	Stephen M. Moore, 14-Apr-1993
** Intent:		This file contains functions for construction of
**			DICOM Upper Layer (DUL) Protocol Data Units (PDUs).
** Last Update:		$Author: hewett $, $Date: 1996-03-26 18:38:46 $
** Source File:		$RCSfile: dulconst.cc,v $
** Revision:		$Revision: 1.1 $
** Status:		$State: Exp $
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif
#include "dicom.h"
#include "cond.h"
#include "lst.h"
#include "dul.h"
#include "dulstruc.h"
#include "dulpriv.h"

static CONDITION
constructSubItem(char *name, unsigned char type,
		 DUL_SUBITEM * applicationContext, unsigned long *rtnlen);
static CONDITION
constructPresentationContext(unsigned char associateType,
			     unsigned char contextID,
			     unsigned char reason, char *abstractSyntax,
	   LST_HEAD ** proposedTransferSyntax, char *acceptedTransferSyntax,
	   PRV_PRESENTATIONCONTEXTITEM * context, unsigned long *rtnLength);
static CONDITION
constructUserInfo(unsigned char type, DUL_ASSOCIATESERVICEPARAMETERS * params,
		  DUL_USERINFO * userInfo, unsigned long *rtnLen);
static CONDITION
constructMaxLength(unsigned long maxPDU, DUL_MAXLENGTH * max,
		   unsigned long *rtnLen);
static CONDITION
constructSCUSCPRoles(unsigned char type,
		  DUL_ASSOCIATESERVICEPARAMETERS * params, LST_HEAD ** list,
		     unsigned long *rtnLength);
static CONDITION
constructSCUSCPSubItem(char *name, unsigned char type, unsigned char scuRole,
		       unsigned char scpRole, PRV_SCUSCPROLE * scuscpItem,
		       unsigned long *length);
static CONDITION
streamSubItem(DUL_SUBITEM * item, unsigned char *b,
	      unsigned long *len);
static CONDITION
streamPresentationContext(
		      LST_HEAD ** presentationContextList, unsigned char *b,
			  unsigned long *length);
static CONDITION
streamUserInfo(DUL_USERINFO * userInfo, unsigned char *b,
	       unsigned long *length);
static CONDITION
streamMaxLength(DUL_MAXLENGTH * max, unsigned char *b,
		unsigned long *length);
static CONDITION
    streamSCUSCPList(LST_HEAD ** list, unsigned char *b, unsigned long *length);
static CONDITION
streamSCUSCPRole(PRV_SCUSCPROLE * scuscpRole, unsigned char *b,
		 unsigned long *len);

static CTNBOOLEAN debug = FALSE;

/* constructAssociatePDU
**
** Purpose:
**	Construct an Association PDU and initialize it.
**
** Parameter Dictionary:
**	params		Service parameters describing the Association
**	type		The PDU type
**	pdu		The PDU that is to be initialized.
**
** Return Values:
**
**	DUL_LISTCREATEFAILED
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**	DUL_ILLEGALSERVICEPARAMETER
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

CONDITION
constructAssociatePDU(DUL_ASSOCIATESERVICEPARAMETERS * params,
		      unsigned char type, PRV_ASSOCIATEPDU * pdu)
{
    CONDITION
	cond;			/* Variable used to examine condition
				 * returned by lower level routines. */
    unsigned long
        itemLength;		/* Holds length of an item computed by a
				 * lower level routine. */
    DUL_PRESENTATIONCONTEXT
	* presentationCtx;	/* Pointer to loop through presentation
				 * context items */
    PRV_PRESENTATIONCONTEXTITEM
	* contextItem;		/* A presentation context item which is
				 * constructed for each presentation context
				 * requested by caller */

    pdu->type = type;
    pdu->rsv1 = 0;
    pdu->length = 0;
    pdu->protocol = DUL_PROTOCOL;
    pdu->rsv2[0] = pdu->rsv2[1] = 0;
    pdu->length += 2 + 2;	/* Protocol + reserved area */
    pdu->presentationContextList = LST_Create();
    if (pdu->presentationContextList == NULL)
	return COND_PushCondition(DUL_LISTCREATEFAILED,
		DUL_Message(DUL_LISTCREATEFAILED), "constructAssociatePDU");
    pdu->userInfo.SCUSCPRoleList = LST_Create();
    if (pdu->userInfo.SCUSCPRoleList == NULL)
	return COND_PushCondition(DUL_LISTCREATEFAILED,
		DUL_Message(DUL_LISTCREATEFAILED), "constructAssociatePDU");


    if (strlen(params->calledAPTitle) < 1 || strlen(params->calledAPTitle) > 16)
	return COND_PushCondition(DUL_ILLEGALSERVICEPARAMETER,
	       DUL_Message(DUL_ILLEGALSERVICEPARAMETER), "Called AP Title");
    (void) strcpy(pdu->calledAPTitle, params->calledAPTitle);
    pdu->length += 16;

    if (strlen(params->callingAPTitle) < 1 || strlen(params->callingAPTitle) > 16)
	return COND_PushCondition(DUL_ILLEGALSERVICEPARAMETER,
	      DUL_Message(DUL_ILLEGALSERVICEPARAMETER), "Calling AP Title");
    (void) strcpy(pdu->callingAPTitle, params->callingAPTitle);
    pdu->length += 16;

    (void) memset(pdu->rsv3, 0, 32);
    pdu->length += 32;

    cond = constructSubItem(params->applicationContextName,
	 DUL_TYPEAPPLICATIONCONTEXT, &pdu->applicationContext, &itemLength);
    if (cond != DUL_NORMAL)
	return cond;
    pdu->length += itemLength;

    cond = DUL_NORMAL;
    if (type == DUL_TYPEASSOCIATERQ) {
	if (debug)
	    fprintf(DEBUG_DEVICE, "Constructing Associate RQ PDU\n");
	presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Head(&params->requestedPresentationContext);
	(void) LST_Position(&params->requestedPresentationContext,
			    (LST_NODE*)presentationCtx);
	while (presentationCtx != NULL && cond == DUL_NORMAL) {
	    contextItem = (PRV_PRESENTATIONCONTEXTITEM *)
		malloc(sizeof(*contextItem));
	    if (contextItem == NULL)
		return COND_PushCondition(DUL_MALLOCERROR,
		      DUL_Message(DUL_MALLOCERROR), "constructAssociatePDU",
					  sizeof(*contextItem));

	    cond = constructPresentationContext(type,
				     presentationCtx->presentationContextID,
						presentationCtx->result,
					    presentationCtx->abstractSyntax,
			     &presentationCtx->proposedTransferSyntax, NULL,
						contextItem, &itemLength);

	    if (LST_Enqueue(&pdu->presentationContextList, (LST_NODE*)contextItem) !=
		LST_NORMAL)
		return COND_PushCondition(DUL_LISTERROR,
		       DUL_Message(DUL_LISTERROR), "constructAssociatePDU");

	    pdu->length += itemLength;
	    presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Next(&params->requestedPresentationContext);
	}
    } else {
	if (debug)
	    fprintf(DEBUG_DEVICE, "Constructing Associate AC PDU\n");
	if (params->acceptedPresentationContext != NULL) {
	    presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Head(&params->acceptedPresentationContext);
	    if (presentationCtx != NULL)
		(void) LST_Position(&params->acceptedPresentationContext,
				    (LST_NODE*)presentationCtx);
	    while (presentationCtx != NULL && cond == DUL_NORMAL) {
		contextItem = (PRV_PRESENTATIONCONTEXTITEM *)
		    malloc(sizeof(*contextItem));
		if (contextItem == NULL)
		    return COND_PushCondition(DUL_MALLOCERROR,
		      DUL_Message(DUL_MALLOCERROR), "constructAssociatePDU",
					      sizeof(*contextItem));

		cond = constructPresentationContext(type,
				     presentationCtx->presentationContextID,
						    presentationCtx->result,
					    presentationCtx->abstractSyntax,
			      NULL, presentationCtx->acceptedTransferSyntax,
						  contextItem, &itemLength);

		if (LST_Enqueue(&pdu->presentationContextList, (LST_NODE*)contextItem) !=
		    LST_NORMAL)
		    return COND_PushCondition(DUL_LISTERROR,
		       DUL_Message(DUL_LISTERROR), "constructAssociatePDU");

		pdu->length += itemLength;
		presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Next(&params->acceptedPresentationContext);
	    }
	}
    }

    if (cond != DUL_NORMAL)
	return cond;

    cond = constructUserInfo(type, params, &(pdu->userInfo), &itemLength);
    if (cond != DUL_NORMAL)
	return cond;
    pdu->length += itemLength;

    return DUL_NORMAL;
}


/* constructAssociateRejectPDU
**
** Purpose:
**	Construct an Associate Reject PDU and fill up the reason for
**	rejection, the source and result.
**
** Parameter Dictionary:
**	result		Result of rejection
**	source		Whether service user or provider
**	reason		Reason for rejection
**	pdu		The PDU that is to be initialized.
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

CONDITION
constructAssociateRejectPDU(unsigned char result,
			    unsigned char source, unsigned char reason,
			    DUL_REJECTRELEASEABORTPDU * pdu)
{
    pdu->type = DUL_TYPEASSOCIATERJ;
    pdu->rsv1 = 0;
    pdu->length = 4;
    pdu->rsv2 = 0;
    pdu->result = result;
    pdu->source = source;
    pdu->reason = reason;

    return DUL_NORMAL;
}


/* constructReleaseRQPDU
**
** Purpose:
**	Construct a Release Request PDU and initialize it.
**
** Parameter Dictionary:
**	pdu		The PDU to be initialized as a Release Request PDU
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
CONDITION
constructReleaseRQPDU(DUL_REJECTRELEASEABORTPDU * pdu)
{
    pdu->type = DUL_TYPERELEASERQ;
    pdu->rsv1 = 0;
    pdu->length = 4;
    pdu->rsv2 = 0;
    pdu->result = 0;
    pdu->source = 0;
    pdu->reason = 0;

    return DUL_NORMAL;
}


/* constructReleaseRPPDU
**
** Purpose:
**	Construct a Release Response PDU
**
** Parameter Dictionary:
**	pdu	The PDU to be initialized as a Release Response PDU
**
** Return Values:
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
CONDITION
constructReleaseRPPDU(DUL_REJECTRELEASEABORTPDU * pdu)
{
    pdu->type = DUL_TYPERELEASERP;
    pdu->rsv1 = 0;
    pdu->length = 4;
    pdu->rsv2 = 0;
    pdu->result = 0;
    pdu->source = 0;
    pdu->reason = 0;

    return DUL_NORMAL;
}


/* constructAbortPDU
**
** Purpose:
**	Construct a ABORT PDU
**
** Parameter Dictionary:
**	src		Originator of the ABORT PDU (service user/provider)
**	reason		Reason for sending the ABORT PDU
**	pdu		The PDU that is to be initialized as an ABORT PDU
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
CONDITION
constructAbortPDU(unsigned char src, unsigned char reason,
		  DUL_REJECTRELEASEABORTPDU * pdu)
{
    pdu->type = DUL_TYPEABORT;
    pdu->rsv1 = 0;
    pdu->length = 4;
    pdu->rsv2 = 0;
    pdu->result = 0;
    pdu->source = src;
    pdu->reason = reason;

    return DUL_NORMAL;
}


/* constructDataPDU
**
** Purpose:
**	Construct a data PDU
**
** Parameter Dictionary:
**	buf		Buffer holding the data that is to be included in the
**			PDU
**	length		Length of the data
**	type		PDU type
**	presentationContextID
**			ID of the presentation context (unique value that
**			distinguishes the context)
**	last		A boolean value to indicate whether or not this PDU
**			is the last one in the stream
**	pdu		THe PDU that is to be constructed.
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

CONDITION
constructDataPDU(void *buf, unsigned long length,
	  DUL_DATAPDV type, DUL_PRESENTATIONCONTEXTID presentationContextID,
		 CTNBOOLEAN last, DUL_DATAPDU * pdu)
{
    unsigned char
        u;

    pdu->type = DUL_TYPEDATA;
    pdu->rsv1 = 0;
    pdu->length = length + 6;

    pdu->presentationDataValue.length = 2 + length;
    u = 0;
    switch (type) {
    case DUL_DATASETPDV:
	break;
    case DUL_COMMANDPDV:
	u |= 1;
	break;
    default:
	break;
    }
    if (last)
	u |= 2;

    pdu->presentationDataValue.presentationContextID = presentationContextID;
    pdu->presentationDataValue.messageControlHeader = u;
    pdu->presentationDataValue.data = buf;
    return DUL_NORMAL;
}


/* streamAssociatePDU
**
** Purpose:
**	Convert the Associate PDU into a stream format (suitable for
**	transmission over the network)
**
** Parameter Dictionary:
**	assoc		The PDU that is to be converted
**	b		The PDU to be sent over the network
**	maxLength	Maximum length allowed for the stream
**	rtnLength	Actual length of the constructed PDU
**
** Return Values:
**
**	DUL_LISTERROR
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

CONDITION
streamAssociatePDU(PRV_ASSOCIATEPDU * assoc, unsigned char *b,
		   unsigned long /*maxLength*/, unsigned long *rtnLen)
{
    unsigned long
        subLength;
    CONDITION
	cond;

    *b++ = assoc->type;
    *b++ = assoc->rsv1;
    COPY_LONG_BIG(assoc->length, b);
    b += 4;
    COPY_SHORT_BIG(assoc->protocol, b);
    b += 2;
    *b++ = assoc->rsv2[0];
    *b++ = assoc->rsv2[1];
    (void) memset(b, ' ', 32);
    (void) strncpy((char *) b, assoc->calledAPTitle, strlen(assoc->calledAPTitle));
    b += 16;
    (void) strncpy((char *) b, assoc->callingAPTitle,
		   strlen(assoc->callingAPTitle));
    b += 16;
    (void) memset(b, 0, 32);
    b += 32;

    *rtnLen = 1 + 1 + 4 + 2 + 2 + 16 + 16 + 32;

    cond = streamSubItem(&assoc->applicationContext, b, &subLength);
    if (cond != DUL_NORMAL)
	return cond;

    b += subLength;
    *rtnLen += subLength;
    cond = streamPresentationContext(&assoc->presentationContextList,
				     b, &subLength);

    if (cond != DUL_NORMAL)
	return cond;

    b += subLength;
    *rtnLen += subLength;
    cond = streamUserInfo(&assoc->userInfo, b, &subLength);
    if (cond != DUL_NORMAL)
	return cond;

    b += subLength;
    *rtnLen += subLength;

    return DUL_NORMAL;
}


/* streamRejectReleaseAbortPDU
**
** Purpose:
**	Construct a Reject Release Abort PDU in the stream format
**
** Parameter Dictionary:
**	assoc		The PDU that is to be converted
**	b		The PDU to be sent over the network
**	maxLength	Maximum length allowed for the stream
**	rtnLength	Actual length of the constructed PDU
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

CONDITION
streamRejectReleaseAbortPDU(DUL_REJECTRELEASEABORTPDU * pdu,
	   unsigned char *b, unsigned long maxLength, unsigned long *rtnLen)
{
    if (maxLength < 2 + 2 + 4)
	return COND_PushCondition(DUL_CODINGERROR,
				  DUL_Message(DUL_CODINGERROR),
			 "buffer too short in streamRejectReleaseAbortPDU");

    *b++ = pdu->type;
    *b++ = pdu->rsv1;
    COPY_LONG_BIG(pdu->length, b);
    b += 4;
    *b++ = pdu->rsv2;
    *b++ = pdu->result;
    *b++ = pdu->source;
    *b++ = pdu->reason;

    *rtnLen = 2 + 4 + 4;

    return DUL_NORMAL;
}


/* streamDataPDUHead
**
** Purpose:
**	Construct a Data PDU in the stream format
**
** Parameter Dictionary:
**	assoc		The PDU that is to be converted
**	b		The PDU to be sent over the network
**	maxLength	Maximum length allowed for the stream
**	rtnLength	Actual length of the constructed PDU
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

CONDITION
streamDataPDUHead(DUL_DATAPDU * pdu, unsigned char *buf,
		  unsigned long maxLength, unsigned long *rtnLen)
{
    unsigned long l;

#ifdef PDV_TEST
    if (maxLength < 18)
	return COND_PushCondition(DUL_CODINGERROR,
				  DUL_Message(DUL_CODINGERROR),
				  "buffer too short in streamDataPDUHead");
#else
    if (maxLength < 12)
	return COND_PushCondition(DUL_CODINGERROR,
				  DUL_Message(DUL_CODINGERROR),
				  "buffer too short in streamDataPDUHead");

#endif

    *buf++ = pdu->type;
    *buf++ = pdu->rsv1;
    l = pdu->length;
#ifdef PDV_TEST
    l += 6;
#endif

    COPY_LONG_BIG(l, buf);
    buf += 4;

#ifdef PDV_TEST
    l = 2;			/* Insert a false, 0-length PDV (2 byte hdr) */
    COPY_LONG_BIG(l, buf);
    buf += 4;
    *buf++ = pdu->presentationDataValue.presentationContextID;
    *buf++ = pdu->presentationDataValue.messageControlHeader & (!0x2);
#endif

    COPY_LONG_BIG(pdu->presentationDataValue.length, buf);
    buf += 4;
    *buf++ = pdu->presentationDataValue.presentationContextID;
    *buf++ = pdu->presentationDataValue.messageControlHeader;

    *rtnLen = 12;
#ifdef PDV_TEST
    *rtnLen = 18;
#endif

    return DUL_NORMAL;
}


/* constructDebug
**
** Purpose:
**	To enable/disable the debugging facility
**
** Parameter Dictionary:
**	flag		Set/Unset the debugging facility.
**
** Return Values:
**	None
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
void
constructDebug(CTNBOOLEAN flag)
{
    debug = flag;
}

/*    ======================================================
 *  Private functions are defined below.
 */


/* constructSubItem
**
** Purpose:
**	Construct the subitem part of the PDU
**
** Parameter Dictionary:
**	name		Data that goes in the subitem PDU
**	type		Type of the subitem PDU
**	subItem		The subitem PDU that is to be constructed
**	rtnLength	Actual length of the constructed subitem (returned
**			to caller)
**
** Return Values:
**	DUL_ILLEGALSERVICEPARAMETER
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
constructSubItem(char *name, unsigned char type,
		 DUL_SUBITEM * subItem, unsigned long *rtnLength)
{
    if (strlen(name) < 1 || strlen(name) > 64)
	return COND_PushCondition(DUL_ILLEGALSERVICEPARAMETER,
			    DUL_Message(DUL_ILLEGALSERVICEPARAMETER), name);

    subItem->type = type;
    subItem->rsv1 = 0;
    subItem->length = (unsigned short) strlen(name);
    (void) strcpy(subItem->data, name);

    *rtnLength = subItem->length + 4;
    return DUL_NORMAL;
}


/* constructPresentationContext
**
** Purpose:
**	Construct the presentation context part of the PDU
**
** Parameter Dictionary:
**	associateType		Type of the parent Associate PDU
**	contextID		Unique value identifying this presentation
**				context
**	reason			Reason to be included in the result field
**	abstractSyntax		Used to build teh abstract syntax list
**	proposedTransferSyntax	Proposed transfer characteristics (to be
**				negotiated)
**	acceptedTransferSyntax	Accepted transfer characteristics (after
**				negotiation)
**	context			The pointer to the actual presentation
**				context being constructed
**	rtnLength		Actual length of the entire context being
**				constructed.
**
**
** Return Values:
**
**	DUL_LISTCREATEFAILED
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
constructPresentationContext(unsigned char associateType,
			     unsigned char contextID,
			     unsigned char reason, char *abstractSyntax,
	   LST_HEAD ** proposedTransferSyntax, char *acceptedTransferSyntax,
	       PRV_PRESENTATIONCONTEXTITEM * context, unsigned long *rtnLen)
{
    CONDITION
    cond;
    unsigned long
        length;
    DUL_SUBITEM
	* subItem;		/* Subitem pointer created for transfer
				 * syntax items */
    DUL_TRANSFERSYNTAX
	* transfer;		/* Pointer to loop through list of transfer
				 * syntaxes */

    *rtnLen = 0;

    if (associateType == DUL_TYPEASSOCIATERQ)
	context->type = DUL_TYPEPRESENTATIONCONTEXTRQ;
    else
	context->type = DUL_TYPEPRESENTATIONCONTEXTAC;

    context->rsv1 = 0;
    context->length = 0;
    context->contextID = contextID;
    context->rsv2 = 0;
    context->result = reason;
    context->rsv3 = 0;
    context->length += 4;
    *rtnLen += 8;

    if (associateType == DUL_TYPEASSOCIATERQ) {
	cond = constructSubItem(abstractSyntax, DUL_TYPEABSTRACTSYNTAX,
				&(context->abstractSyntax), &length);
	if (cond != DUL_NORMAL)
	    return cond;
	context->length += (unsigned short) length;
	*rtnLen += length;
    } else
	context->abstractSyntax.length = 0;

    context->transferSyntaxList = LST_Create();
    if (context == NULL)
	return COND_PushCondition(DUL_LISTCREATEFAILED,
	 DUL_Message(DUL_LISTCREATEFAILED), "constructPresentationContext");

    if (associateType == DUL_TYPEASSOCIATERQ) {
	transfer = (DUL_TRANSFERSYNTAX*)LST_Head(proposedTransferSyntax);
	if (transfer == NULL)
	    return COND_PushCondition(DUL_LISTERROR,
		DUL_Message(DUL_LISTERROR), "constructPresentationContext");
	(void) LST_Position(proposedTransferSyntax, (LST_NODE*)transfer);

	while (transfer != NULL) {
	    subItem = (DUL_SUBITEM *) malloc(sizeof(*subItem));
	    if (subItem == NULL)
		return COND_PushCondition(DUL_MALLOCERROR,
		DUL_Message(DUL_MALLOCERROR), "constructPresentationContext",
					  sizeof(*subItem));

	    cond = constructSubItem(transfer->transferSyntax,
				  DUL_TYPETRANSFERSYNTAX, subItem, &length);
	    if (cond != DUL_NORMAL)
		return cond;

	    if (LST_Enqueue(&context->transferSyntaxList, (LST_NODE*)subItem) != LST_NORMAL)
		return COND_PushCondition(DUL_LISTERROR,
		DUL_Message(DUL_LISTERROR), "constructPresentationContext");
	    context->length += (unsigned short) length;
	    *rtnLen += length;
	    transfer = (DUL_TRANSFERSYNTAX*)LST_Next(proposedTransferSyntax);
	}
    } else {
	subItem = (DUL_SUBITEM *) malloc(sizeof(*subItem));
	if (subItem == NULL)
	    return COND_PushCondition(DUL_MALLOCERROR,
	       DUL_Message(DUL_MALLOCERROR), "constructPresentationContext",
				      sizeof(*subItem));
	cond = constructSubItem(acceptedTransferSyntax,
				DUL_TYPETRANSFERSYNTAX, subItem, &length);
	if (cond != DUL_NORMAL)
	    return cond;
	if (LST_Enqueue(&context->transferSyntaxList, (LST_NODE*)subItem) != LST_NORMAL)
	    return COND_PushCondition(DUL_LISTERROR,
		DUL_Message(DUL_LISTERROR), "constructPresentationContext");
	context->length += (unsigned short) length;
	*rtnLen += length;
    }

    return DUL_NORMAL;
}


/* constructUserInfo
**
** Purpose:
**	Construct the USer Info part of the Associate PDU
**
** Parameter Dictionary:
**	type		Type of the User info part
**	params		Service parameters describing the Association
**	userInfo	The user info part that is to be constructed
**	rtnLength	Actual length of the constructed part, returned to
**			the caller.
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
constructUserInfo(unsigned char type, DUL_ASSOCIATESERVICEPARAMETERS * params,
		  DUL_USERINFO * userInfo, unsigned long *rtnLen)
{
    CONDITION
    cond;
    unsigned long
        length;

    userInfo->type = DUL_TYPEUSERINFO;
    userInfo->rsv1 = 0;
    userInfo->length = 0;
    *rtnLen = 4;

    cond = constructMaxLength(params->maxPDU, &userInfo->maxLength, &length);
    if (cond != DUL_NORMAL)
	return cond;
    userInfo->length += (unsigned short) length;
    *rtnLen += length;

    if (type == DUL_TYPEASSOCIATERQ)
	cond = constructSubItem(params->callingImplementationClassUID,
	  DUL_TYPEIMPLEMENTATIONCLASSUID, &userInfo->implementationClassUID,
				&length);
    else
	cond = constructSubItem(params->calledImplementationClassUID,
	  DUL_TYPEIMPLEMENTATIONCLASSUID, &userInfo->implementationClassUID,
				&length);
    if (cond != DUL_NORMAL)
	return cond;
    userInfo->length += (unsigned short) length;
    *rtnLen += length;

    if (type == DUL_TYPEASSOCIATERQ) {
	if (strlen(params->callingImplementationVersionName) != 0) {
	    cond = constructSubItem(params->callingImplementationVersionName,
				    DUL_TYPEIMPLEMENTATIONVERSIONNAME,
				    &userInfo->implementationVersionName,
				    &length);
	    if (cond != DUL_NORMAL)
		return cond;
	    userInfo->length += (unsigned short) length;
	    *rtnLen += length;
	}
    } else {
	if (strlen(params->calledImplementationVersionName) != 0) {
	    cond = constructSubItem(params->calledImplementationVersionName,
				    DUL_TYPEIMPLEMENTATIONVERSIONNAME,
				    &userInfo->implementationVersionName,
				    &length);
	    if (cond != DUL_NORMAL)
		return cond;
	    userInfo->length += (unsigned short) length;
	    *rtnLen += length;
	}
    }
    cond = constructSCUSCPRoles(type, params, &userInfo->SCUSCPRoleList,
				&length);
    if (cond != DUL_NORMAL)
	return cond;
    userInfo->length += (unsigned short) length;
    *rtnLen += length;
    return DUL_NORMAL;
}


/* constructMaxLength
**
** Purpose:
**	Construct the Max Length part of the PDU
**
** Parameter Dictionary:
**	maxPDU		Length of the Max PDU
**	max		The Max PDU that is to be constructed
**	rtnLength	Length of the PDU constructed.
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
constructMaxLength(unsigned long maxPDU, DUL_MAXLENGTH * max,
		   unsigned long *rtnLen)
{
    max->type = DUL_TYPEMAXLENGTH;
    max->rsv1 = 0;
    max->length = 4;
    max->maxLength = maxPDU;
    *rtnLen = 8;

    return DUL_NORMAL;
}


/* constructSCUSCPRoles
**
** Purpose:
**	Construct the SCU-SCP Role list part of the PDU
**
** Parameter Dictionary:
**	type		Type of the parent PDU
**	params		Service parameters describing the Association
**	list		The SCU_SCP role list that is to be constructed
**	rtnLength	Length of the list created.
**
** Return Values:
**
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
constructSCUSCPRoles(unsigned char type,
		  DUL_ASSOCIATESERVICEPARAMETERS * params, LST_HEAD ** list,
		     unsigned long *rtnLength)
{
    CONDITION
    cond;
    DUL_PRESENTATIONCONTEXT
	* presentationCtx;	/* Pointer to loop through presentation ctx */
    PRV_SCUSCPROLE
	* scuscpItem;
    unsigned char
        scuRole = 0,
        scpRole = 0;
    unsigned long
        length;

    *rtnLength = 0;
    cond = DUL_NORMAL;
    if (type == DUL_TYPEASSOCIATERQ) {
	presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Head(&params->requestedPresentationContext);
	if (presentationCtx != NULL)
	    (void) LST_Position(&params->requestedPresentationContext,
				(LST_NODE*)presentationCtx);
	while (presentationCtx != NULL) {
	    if (presentationCtx->proposedSCRole != DUL_SC_ROLE_DEFAULT) {
		scuscpItem = (PRV_SCUSCPROLE*)malloc(sizeof(*scuscpItem));
		if (scuscpItem == NULL)
		    return COND_PushCondition(DUL_MALLOCERROR,
		       DUL_Message(DUL_MALLOCERROR), "constructSCUSCPRoles",
					      sizeof(*scuscpItem));
		if (presentationCtx->proposedSCRole == DUL_SC_ROLE_SCU) {
		    scuRole = 1;
		} else if (presentationCtx->proposedSCRole == DUL_SC_ROLE_SCP) {
		    scpRole = 1;
		} else {
		    scuRole = scpRole = 1;
		}
		cond = constructSCUSCPSubItem(presentationCtx->abstractSyntax,
				       DUL_TYPESCUSCPROLE, scuRole, scpRole,
					      scuscpItem, &length);
		if (cond != DUL_NORMAL)
		    return cond;
		*rtnLength += length;
		cond = LST_Enqueue(list, (LST_NODE*)scuscpItem);
		if (cond != LST_NORMAL)
		    return COND_PushCondition(DUL_LISTERROR,
			DUL_Message(DUL_LISTERROR), "constructSCUSCPRoles");
	    }
	    presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Next(&params->requestedPresentationContext);
	}
    } else {
	presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Head(&params->acceptedPresentationContext);
	if (presentationCtx != NULL)
	    (void) LST_Position(&params->acceptedPresentationContext,
				(LST_NODE*)presentationCtx);
	while (presentationCtx != NULL) {
	    if (presentationCtx->acceptedSCRole != DUL_SC_ROLE_DEFAULT) {
		scuscpItem = (PRV_SCUSCPROLE*)malloc(sizeof(*scuscpItem));
		if (scuscpItem == NULL)
		    return COND_PushCondition(DUL_MALLOCERROR,
		       DUL_Message(DUL_MALLOCERROR), "constructSCUSCPRoles",
					      sizeof(*scuscpItem));
		if (presentationCtx->acceptedSCRole == DUL_SC_ROLE_SCU) {
		    scuRole = 1;
		} else if (presentationCtx->acceptedSCRole == DUL_SC_ROLE_SCP) {
		    scpRole = 1;
		} else {
		    scuRole = scpRole = 1;
		}
		cond = constructSCUSCPSubItem(presentationCtx->abstractSyntax,
				       DUL_TYPESCUSCPROLE, scuRole, scpRole,
					      scuscpItem, &length);
		if (cond != DUL_NORMAL)
		    return cond;
		*rtnLength += length;
		cond = LST_Enqueue(list, (LST_NODE*)scuscpItem);
		if (cond != LST_NORMAL)
		    return COND_PushCondition(DUL_LISTERROR,
			DUL_Message(DUL_LISTERROR), "constructSCUSCPRoles");
	    }
	    presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Next(&params->acceptedPresentationContext);
	}
    }
    return DUL_NORMAL;
}

/* constructSCUSCPSubItem
**
** Purpose:
**	Construct a SCU-SCP subitem part in the parent PDU
**
** Parameter Dictionary:
**	name		SOP Class UID
**	type		Type of the SCU-SCP subitem
**	scuRole		Role played by the SCU
**	scpRole		Role played by the SCP
**	scuscpItem	The subitem to be constructed
**	length		Length of the subitm that is constructed
**
** Return Values:
**	DUL_ILLEGALSERVICEPARAMETER
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
constructSCUSCPSubItem(char *name, unsigned char type, unsigned char scuRole,
		       unsigned char scpRole, PRV_SCUSCPROLE * scuscpItem,
		       unsigned long *length)
{
    if (strlen(name) < 1 || strlen(name) > 64)
	return COND_PushCondition(DUL_ILLEGALSERVICEPARAMETER,
			    DUL_Message(DUL_ILLEGALSERVICEPARAMETER), name);

    scuscpItem->type = type;
    scuscpItem->rsv1 = 0;
    scuscpItem->SCURole = scuRole;
    scuscpItem->SCPRole = scpRole;
    scuscpItem->length = (unsigned short) (strlen(name) + 2 + 2);
    (void) strcpy(scuscpItem->SOPClassUID, name);

    *length = scuscpItem->length + 4;
    return DUL_NORMAL;
}

/* streamSubItem
**
** Purpose:
**	Convert the subitem into a stream format
**
** Parameter Dictionary:
**	item		The subitem that is to be converted
**	b		The stream version of the subitem
**	len		Length of the stream format
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
streamSubItem(DUL_SUBITEM * item, unsigned char *b,
	      unsigned long *len)
{
    unsigned short
        length;

    length = item->length;

    *b++ = item->type;
    *b++ = item->rsv1;
    COPY_SHORT_BIG(length, b);
    b += 2;
    (void) memcpy(b, item->data, length);
    *len = 4 + length;

    return DUL_NORMAL;
}

/* streamPresentationContext
**
** Purpose:
**	Convert the presentation context list into stream format
**
** Parameter Dictionary:
**	presentationContextList		The presentation context list that
**					is to be converted to stream format
**	b				The stream version (output)
**	length				Length of the stream version
**
** Return Values:
**
**	DUL_LISTERROR
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
streamPresentationContext(LST_HEAD ** presentationContextList,
			  unsigned char *b, unsigned long *length)
{
    PRV_PRESENTATIONCONTEXTITEM
    * presentation;
    DUL_SUBITEM
	* transfer;
    CONDITION
	cond;
    unsigned long
        subLength;

    *length = 0;

    presentation = (PRV_PRESENTATIONCONTEXTITEM*)LST_Head(presentationContextList);
    if (presentation == NULL)
	return COND_PushCondition(DUL_LISTERROR,
		   DUL_Message(DUL_LISTERROR), "streamPresentationContext");
    (void) LST_Position(presentationContextList, (LST_NODE*)presentation);

    while (presentation != NULL) {
	*b++ = presentation->type;
	*b++ = presentation->rsv1;

	COPY_SHORT_BIG(presentation->length, b);
	b += 2;
	*b++ = presentation->contextID;
	*b++ = presentation->rsv2;
	*b++ = presentation->result;
	*b++ = presentation->rsv3;
	*length += 8;

	if (presentation->abstractSyntax.length != 0) {
	    cond = streamSubItem(&presentation->abstractSyntax, b, &subLength);
	    if (cond != DUL_NORMAL)
		return cond;
	    b += subLength;
	    *length += subLength;
	}
	transfer = (DUL_SUBITEM*)LST_Head(&presentation->transferSyntaxList);
	if (transfer == NULL)
	    return COND_PushCondition(DUL_LISTERROR,
		   DUL_Message(DUL_LISTERROR), "streamPresentationContext");
	(void) LST_Position(&presentation->transferSyntaxList, (LST_NODE*)transfer);
	while (transfer != NULL) {
	    if (transfer->length != 0) {
		cond = streamSubItem(transfer, b, &subLength);
		if (cond != DUL_NORMAL)
		    return cond;
		b += subLength;
		*length += subLength;
	    }
	    transfer = (DUL_SUBITEM*)LST_Next(&presentation->transferSyntaxList);
	}
	presentation = (PRV_PRESENTATIONCONTEXTITEM*)LST_Next(presentationContextList);
    }
    return DUL_NORMAL;
}

/* streamUserInfo
**
** Purpose:
**	Convert the stream user info part of the PDU into stream format
**
** Parameter Dictionary:
**	userInfo	The user info structure to be converted to stream
**			format
**	b		The stream version (output)
**	length		Length of the stream version
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
streamUserInfo(DUL_USERINFO * userInfo, unsigned char *b,
	       unsigned long *length)
{
    CONDITION
    cond;
    unsigned long
        subLength;

    *length = 0;
    *b++ = userInfo->type;
    *b++ = userInfo->rsv1;
    COPY_SHORT_BIG(userInfo->length, b);
    b += 2;
    *length += 4;

    cond = streamMaxLength(&userInfo->maxLength, b, &subLength);
    if (cond != DUL_NORMAL)
	return cond;
    b += subLength;
    *length += subLength;

    cond = streamSubItem(&userInfo->implementationClassUID, b, &subLength);
    if (cond != DUL_NORMAL)
	return cond;
    b += subLength;
    *length += subLength;

    if (userInfo->implementationVersionName.length != 0) {
	cond = streamSubItem(&userInfo->implementationVersionName, b, &subLength);
	if (cond != DUL_NORMAL)
	    return cond;
	b += subLength;
	*length += subLength;
    }
    if (LST_Count(&userInfo->SCUSCPRoleList) != 0) {
	cond = streamSCUSCPList(&userInfo->SCUSCPRoleList, b, &subLength);
	if (cond != DUL_NORMAL)
	    return cond;
	b += subLength;
	*length += subLength;
    }
    return DUL_NORMAL;
}

/* streamMaxLength
**
** Purpose:
**	Convert the Max Length structure into stream format
**
** Parameter Dictionary:
**	max		Max Length structure to be converted to
**			stream format
**	b		The stream version (output)
**	length		Length of the stream version
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
streamMaxLength(DUL_MAXLENGTH * max, unsigned char *b,
		unsigned long *length)
{

    *b++ = max->type;
    *b++ = max->rsv1;
    COPY_SHORT_BIG(max->length, b);
    b += 2;
    COPY_LONG_BIG(max->maxLength, b);

    *length = 8;
    return DUL_NORMAL;
}

/* streamSCUSCPList
**
** Purpose:
**	Convert the SCU-SCP list into stream format
**
** Parameter Dictionary:
**	list		SCU-SCP list
**	b		The stream version (output)
**	length		Length of the stream version
**
** Return Values:
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
streamSCUSCPList(LST_HEAD ** list, unsigned char *b, unsigned long *length)
{
    PRV_SCUSCPROLE
    * scuscpRole;
    CONDITION
	cond;
    unsigned long
        localLength;

    *length = 0;
    scuscpRole = (PRV_SCUSCPROLE*)LST_Head(list);
    if (scuscpRole != NULL)
	(void) LST_Position(list, (LST_NODE*)scuscpRole);
    while (scuscpRole != NULL) {
	localLength = 0;
	cond = streamSCUSCPRole(scuscpRole, b, &localLength);
	if (cond != DUL_NORMAL)
	    return cond;
	*length += localLength;
	b += localLength;
	scuscpRole = (PRV_SCUSCPROLE*)LST_Next(list);
    }
    return DUL_NORMAL;
}

/* streamSCUSCPRole
**
** Purpose:
**	Convert the SCU-SCP role list into stream format
**
** Parameter Dictionary:
**	scuscpRole	SCU-SCP role list that is to be converted to
**			stream format
**	b		The stream version (output)
**	length		Length of the stream version
**
** Return Values:
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/
static CONDITION
streamSCUSCPRole(PRV_SCUSCPROLE * scuscpRole, unsigned char *b,
		 unsigned long *len)
{
    unsigned short
        length;

    length = scuscpRole->length;

    *b++ = scuscpRole->type;
    *b++ = scuscpRole->rsv1;
    COPY_SHORT_BIG(length, b);
    b += 2;

    length = strlen(scuscpRole->SOPClassUID);
    COPY_SHORT_BIG(length, b);
    b += 2;

    (void) memcpy(b, scuscpRole->SOPClassUID, length);
    b += length;
    *b++ = scuscpRole->SCURole;
    *b++ = scuscpRole->SCPRole;
    *len = 4 + scuscpRole->length;

    return DUL_NORMAL;
}

/*
** CVS Log
** $Log: dulconst.cc,v $
** Revision 1.1  1996-03-26 18:38:46  hewett
** Initial revision
**
**
*/
