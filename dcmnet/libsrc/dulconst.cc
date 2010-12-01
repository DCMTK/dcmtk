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

/*
**          DICOM 93
**        Electronic Radiology Laboratory
**      Mallinckrodt Institute of Radiology
**    Washington University School of Medicine
**
** Module Name(s):  constructAssociatePDU
**      constructAssociateRejectPDU
**      constructReleasePDU
**      constructDataPDU
**      streamAssociatePDU
**      streamRejectReleaseAbortPDU
**      streamDataPDUHead
** Author, Date:  Stephen M. Moore, 14-Apr-1993
** Intent:        This file contains functions for construction of
**                DICOM Upper Layer (DUL) Protocol Data Units (PDUs).
** Last Update:   $Author: joergr $, $Date: 2010-12-01 08:26:36 $
** Revision:      $Revision: 1.28 $
** Status:        $State: Exp $
*/


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/lst.h"
#include "dcmtk/dcmnet/dul.h"
#include "dulstruc.h"
#include "dulpriv.h"
#include "dcmtk/ofstd/ofconsol.h"

static OFCondition
constructSubItem(char *name, unsigned char type,
     DUL_SUBITEM * applicationContext, unsigned long *rtnlen);
static OFCondition
constructPresentationContext(unsigned char associateType,
           unsigned char contextID,
           unsigned char reason, char *abstractSyntax,
     LST_HEAD ** proposedTransferSyntax, char *acceptedTransferSyntax,
     PRV_PRESENTATIONCONTEXTITEM * context, unsigned long *rtnLength);
static OFCondition
constructUserInfo(unsigned char type, DUL_ASSOCIATESERVICEPARAMETERS * params,
      DUL_USERINFO * userInfo, unsigned long *rtnLen);
static OFCondition
constructMaxLength(unsigned long maxPDU, DUL_MAXLENGTH * max,
       unsigned long *rtnLen);
static OFCondition
constructSCUSCPRoles(unsigned char type,
      DUL_ASSOCIATESERVICEPARAMETERS * params, LST_HEAD ** lst,
         unsigned long *rtnLength);
static OFCondition
constructSCUSCPSubItem(char *name, unsigned char type, unsigned char scuRole,
           unsigned char scpRole, PRV_SCUSCPROLE * scuscpItem,
           unsigned long *length);
static OFCondition
streamSubItem(DUL_SUBITEM * item, unsigned char *b,
        unsigned long *len);
static OFCondition
streamPresentationContext(
          LST_HEAD ** presentationContextList, unsigned char *b,
        unsigned long *length);
static OFCondition
streamUserInfo(DUL_USERINFO * userInfo, unsigned char *b,
         unsigned long *length);
static OFCondition
streamMaxLength(DUL_MAXLENGTH * max, unsigned char *b,
    unsigned long *length);
static OFCondition
    streamSCUSCPList(LST_HEAD ** lst, unsigned char *b, unsigned long *length);
static OFCondition
streamSCUSCPRole(PRV_SCUSCPROLE * scuscpRole, unsigned char *b,
     unsigned long *len);
static OFCondition
constructExtNeg(unsigned char type,
    DUL_ASSOCIATESERVICEPARAMETERS * params, SOPClassExtendedNegotiationSubItemList **lst,
    unsigned long *rtnLength);

static OFCondition
streamExtNegList(SOPClassExtendedNegotiationSubItemList *lst, unsigned char *b, unsigned long *length);

static OFCondition
streamExtNeg(SOPClassExtendedNegotiationSubItem* extNeg, unsigned char *b, unsigned long *len);

/* constructAssociatePDU
**
** Purpose:
**  Construct an Association PDU and initialize it.
**
** Parameter Dictionary:
**  params    Service parameters describing the Association
**  type    The PDU type
**  pdu   The PDU that is to be initialized.
**
** Return Values:
**
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/

OFCondition
constructAssociatePDU(DUL_ASSOCIATESERVICEPARAMETERS * params,
          unsigned char type, PRV_ASSOCIATEPDU * pdu)
{
    unsigned long
        itemLength;   /* Holds length of an item computed by a
         * lower level routine. */
    DUL_PRESENTATIONCONTEXT
  * presentationCtx;  /* Pointer to loop through presentation
         * context items */
    PRV_PRESENTATIONCONTEXTITEM
  * contextItem;    /* A presentation context item which is
         * constructed for each presentation context
         * requested by caller */

    pdu->type = type;
    pdu->rsv1 = 0;
    pdu->length = 0;
    pdu->protocol = DUL_PROTOCOL;
    pdu->rsv2[0] = pdu->rsv2[1] = 0;
    pdu->length += 2 + 2; /* Protocol + reserved area */
    pdu->presentationContextList = LST_Create();
    if (pdu->presentationContextList == NULL) return EC_MemoryExhausted;
    pdu->userInfo.SCUSCPRoleList = LST_Create();
    if (pdu->userInfo.SCUSCPRoleList == NULL) return EC_MemoryExhausted;

    if (strlen(params->calledAPTitle) < 1 || strlen(params->calledAPTitle) > 16)
        return makeDcmnetCondition(DULC_ILLEGALSERVICEPARAMETER, OF_error, "Illegal service parameter: Called AP Title");
    (void) strcpy(pdu->calledAPTitle, params->calledAPTitle);
    pdu->length += 16;

    if (strlen(params->callingAPTitle) < 1 || strlen(params->callingAPTitle) > 16)
        return makeDcmnetCondition(DULC_ILLEGALSERVICEPARAMETER, OF_error, "Illegal service parameter: Calling AP Title");
    (void) strcpy(pdu->callingAPTitle, params->callingAPTitle);
    pdu->length += 16;

    (void) memset(pdu->rsv3, 0, 32);
    pdu->length += 32;

    OFCondition cond = constructSubItem(params->applicationContextName,
   DUL_TYPEAPPLICATIONCONTEXT, &pdu->applicationContext, &itemLength);
    if (cond.bad())
  return cond;
    pdu->length += itemLength;

    cond = EC_Normal;
    if (type == DUL_TYPEASSOCIATERQ) {
        DCMNET_DEBUG("Constructing Associate RQ PDU");
  presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Head(&params->requestedPresentationContext);
  (void) LST_Position(&params->requestedPresentationContext,
          (LST_NODE*)presentationCtx);
  while (presentationCtx != NULL && cond.good()) {
      contextItem = (PRV_PRESENTATIONCONTEXTITEM *)
    malloc(sizeof(PRV_PRESENTATIONCONTEXTITEM));
      if (contextItem == NULL) return EC_MemoryExhausted;

      cond = constructPresentationContext(type,
             presentationCtx->presentationContextID,
            presentationCtx->result,
              presentationCtx->abstractSyntax,
           &presentationCtx->proposedTransferSyntax, NULL,
            contextItem, &itemLength);
            OFCondition cond2 = LST_Enqueue(&pdu->presentationContextList, (LST_NODE*)contextItem);
            if (cond2.bad()) return cond2;

      pdu->length += itemLength;
      presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Next(&params->requestedPresentationContext);
  }
    } else {
        DCMNET_DEBUG("Constructing Associate AC PDU");
  if (params->acceptedPresentationContext != NULL) {
      presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Head(&params->acceptedPresentationContext);
      if (presentationCtx != NULL)
    (void) LST_Position(&params->acceptedPresentationContext,
            (LST_NODE*)presentationCtx);
      while (presentationCtx != NULL && cond.good()) {
    contextItem = (PRV_PRESENTATIONCONTEXTITEM *)
        malloc(sizeof(*contextItem));
    if (contextItem == NULL) return EC_MemoryExhausted;

    cond = constructPresentationContext(type,
             presentationCtx->presentationContextID,
                presentationCtx->result,
              presentationCtx->abstractSyntax,
            NULL, presentationCtx->acceptedTransferSyntax,
              contextItem, &itemLength);

                OFCondition cond2 = LST_Enqueue(&pdu->presentationContextList, (LST_NODE*)contextItem);
                if (cond2.bad()) return cond2;

    pdu->length += itemLength;
    presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Next(&params->acceptedPresentationContext);
      }
  }
    }

    if (cond.bad())
  return cond;

    cond = constructUserInfo(type, params, &(pdu->userInfo), &itemLength);
    if (cond.bad())
  return cond;
    pdu->length += itemLength;

    return EC_Normal;
}


/* constructAssociateRejectPDU
**
** Purpose:
**  Construct an Associate Reject PDU and fill up the reason for
**  rejection, the source and result.
**
** Parameter Dictionary:
**  result    Result of rejection
**  source    Whether service user or provider
**  reason    Reason for rejection
**  pdu   The PDU that is to be initialized.
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/

OFCondition
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

    return EC_Normal;
}


/* constructReleaseRQPDU
**
** Purpose:
**  Construct a Release Request PDU and initialize it.
**
** Parameter Dictionary:
**  pdu   The PDU to be initialized as a Release Request PDU
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
OFCondition
constructReleaseRQPDU(DUL_REJECTRELEASEABORTPDU * pdu, unsigned long mode)
{
    pdu->type = DUL_TYPERELEASERQ;
    pdu->rsv1 = 0;
    pdu->length = 4;
    pdu->rsv2 = 0;
    pdu->result = 0;
    pdu->source = 0;
    pdu->reason = 0;
    if (mode)
    {
      pdu->rsv2   = (unsigned char)(mode >> 24);
      pdu->result = (unsigned char)(mode >> 16);
      pdu->source = (unsigned char)(mode >> 8);
      pdu->reason = (unsigned char)(mode);
    }

    return EC_Normal;
}


/* constructReleaseRPPDU
**
** Purpose:
**  Construct a Release Response PDU
**
** Parameter Dictionary:
**  pdu The PDU to be initialized as a Release Response PDU
**
** Return Values:
**
** Notes:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
OFCondition
constructReleaseRPPDU(DUL_REJECTRELEASEABORTPDU * pdu)
{
    pdu->type = DUL_TYPERELEASERP;
    pdu->rsv1 = 0;
    pdu->length = 4;
    pdu->rsv2 = 0;
    pdu->result = 0;
    pdu->source = 0;
    pdu->reason = 0;

    return EC_Normal;
}


/* constructAbortPDU
**
** Purpose:
**  Construct a ABORT PDU
**
** Parameter Dictionary:
**  src   Originator of the ABORT PDU (service user/provider)
**  reason    Reason for sending the ABORT PDU
**  pdu   The PDU that is to be initialized as an ABORT PDU
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
OFCondition
constructAbortPDU(unsigned char src, unsigned char reason,
      DUL_REJECTRELEASEABORTPDU * pdu,
      unsigned long mode)
{
    pdu->type = DUL_TYPEABORT;
    pdu->rsv1 = 0;
    pdu->length = 4;
    pdu->rsv2 = 0;
    pdu->result = 0;
    pdu->source = src;
    pdu->reason = reason;
    if (mode)
    {
      pdu->rsv1   = (unsigned char)(mode >> 24);
      pdu->rsv2   = (unsigned char)(mode >> 16);
      pdu->result = (unsigned char)(mode >> 8);
      pdu->reason = (unsigned char)(mode);
      pdu->source = 0;
    }
    return EC_Normal;
}


/* constructDataPDU
**
** Purpose:
**  Construct a data PDU
**
** Parameter Dictionary:
**  buf   Buffer holding the data that is to be included in the
**      PDU
**  length    Length of the data
**  type    PDU type
**  presentationContextID
**      ID of the presentation context (unique value that
**      distinguishes the context)
**  last    A boolean value to indicate whether or not this PDU
**      is the last one in the stream
**  pdu   THe PDU that is to be constructed.
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/

OFCondition
constructDataPDU(void *buf, unsigned long length,
    DUL_DATAPDV type, DUL_PRESENTATIONCONTEXTID presentationContextID,
     OFBool last, DUL_DATAPDU * pdu)
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
    return EC_Normal;
}


/* streamAssociatePDU
**
** Purpose:
**  Convert the Associate PDU into a stream format (suitable for
**  transmission over the network)
**
** Parameter Dictionary:
**  assoc   The PDU that is to be converted
**  b   The PDU to be sent over the network
**  maxLength Maximum length allowed for the stream
**  rtnLength Actual length of the constructed PDU
**
** Return Values:
**
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/

OFCondition
streamAssociatePDU(PRV_ASSOCIATEPDU * assoc, unsigned char *b,
       unsigned long /*maxLength*/, unsigned long *rtnLen)
{
    unsigned long
        subLength;

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

    OFCondition cond = streamSubItem(&assoc->applicationContext, b, &subLength);
    if (cond.bad())
  return cond;

    b += subLength;
    *rtnLen += subLength;
    cond = streamPresentationContext(&assoc->presentationContextList,
             b, &subLength);

    if (cond.bad())
  return cond;

    b += subLength;
    *rtnLen += subLength;
    cond = streamUserInfo(&assoc->userInfo, b, &subLength);
    if (cond.bad())
  return cond;

    b += subLength;
    *rtnLen += subLength;

    return EC_Normal;
}


/* streamRejectReleaseAbortPDU
**
** Purpose:
**  Construct a Reject Release Abort PDU in the stream format
**
** Parameter Dictionary:
**  assoc   The PDU that is to be converted
**  b   The PDU to be sent over the network
**  maxLength Maximum length allowed for the stream
**  rtnLength Actual length of the constructed PDU
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/

OFCondition
streamRejectReleaseAbortPDU(DUL_REJECTRELEASEABORTPDU * pdu,
     unsigned char *b, unsigned long maxLength, unsigned long *rtnLen)
{
    if (maxLength < 2 + 2 + 4)
    {
      return makeDcmnetCondition(DULC_CODINGERROR, OF_error, "Coding Error in DUL routine: buffer too short in streamRejectReleaseAbortPDU");
    }

    *b++ = pdu->type;
    *b++ = pdu->rsv1;
    COPY_LONG_BIG(pdu->length, b);
    b += 4;
    *b++ = pdu->rsv2;
    *b++ = pdu->result;
    *b++ = pdu->source;
    *b++ = pdu->reason;

    *rtnLen = 2 + 4 + 4;

    return EC_Normal;
}


/* streamDataPDUHead
**
** Purpose:
**  Construct a Data PDU in the stream format
**
** Parameter Dictionary:
**  assoc   The PDU that is to be converted
**  b   The PDU to be sent over the network
**  maxLength Maximum length allowed for the stream
**  rtnLength Actual length of the constructed PDU
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/

OFCondition
streamDataPDUHead(DUL_DATAPDU * pdu, unsigned char *buf,
      unsigned long maxLength, unsigned long *rtnLen)
{
    unsigned long l;

#ifdef PDV_TEST
    if (maxLength < 18)
    {
      return makeDcmnetCondition(DULC_CODINGERROR, OF_error, "Coding Error in DUL routine: buffer too short in streamDataPDUHead");
    }

#else
    if (maxLength < 12)
    {
      return makeDcmnetCondition(DULC_CODINGERROR, OF_error, "Coding Error in DUL routine: buffer too short in streamDataPDUHead");
    }

#endif

    /* write PDU type field information to buffer */
    *buf++ = pdu->type;
    /* append PDU reserved field information to buffer */
    *buf++ = pdu->rsv1;
    /* append PDU length field information to buffer */
    l = pdu->length;
#ifdef PDV_TEST
    l += 6;
#endif

    COPY_LONG_BIG(l, buf);
    buf += 4;

#ifdef PDV_TEST
    l = 2;      /* Insert a false, 0-length PDV (2 byte hdr) */
    COPY_LONG_BIG(l, buf);
    buf += 4;
    *buf++ = pdu->presentationDataValue.presentationContextID;
    *buf++ = pdu->presentationDataValue.messageControlHeader & (~0x2);
#endif

    /* append PDV length field information to buffer */
    COPY_LONG_BIG(pdu->presentationDataValue.length, buf);
    buf += 4;
    /* append PDV presentation context ID field information to buffer */
    *buf++ = pdu->presentationDataValue.presentationContextID;
    /* append PDV message control header field information to buffer */
    *buf++ = pdu->presentationDataValue.messageControlHeader;

    /* set rtnLen (actual length of constructed PDU) */
    *rtnLen = 12;
#ifdef PDV_TEST
    *rtnLen = 18;
#endif

    return EC_Normal;
}


/*    ======================================================
 *  Private functions are defined below.
 */


/* constructSubItem
**
** Purpose:
**  Construct the subitem part of the PDU
**
** Parameter Dictionary:
**  name    Data that goes in the subitem PDU
**  type    Type of the subitem PDU
**  subItem   The subitem PDU that is to be constructed
**  rtnLength Actual length of the constructed subitem (returned
**      to caller)
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
static OFCondition
constructSubItem(char *name, unsigned char type,
     DUL_SUBITEM * subItem, unsigned long *rtnLength)
{
    if (strlen(name) < 1 || strlen(name) > 64)
    {
        char buf[1024];
        sprintf(buf,"Illegal service parameter: %s", name);
        return makeDcmnetCondition(DULC_ILLEGALSERVICEPARAMETER, OF_error, buf);
    }
    subItem->type = type;
    subItem->rsv1 = 0;
    subItem->length = (unsigned short) strlen(name);
    (void) strcpy(subItem->data, name);

    *rtnLength = subItem->length + 4;
    return EC_Normal;
}


/* constructPresentationContext
**
** Purpose:
**  Construct the presentation context part of the PDU
**
** Parameter Dictionary:
**  associateType   Type of the parent Associate PDU
**  contextID   Unique value identifying this presentation
**        context
**  reason      Reason to be included in the result field
**  abstractSyntax    Used to build teh abstract syntax list
**  proposedTransferSyntax  Proposed transfer characteristics (to be
**        negotiated)
**  acceptedTransferSyntax  Accepted transfer characteristics (after
**        negotiation)
**  context     The pointer to the actual presentation
**        context being constructed
**  rtnLength   Actual length of the entire context being
**        constructed.
**
**
** Return Values:
**
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
static OFCondition
constructPresentationContext(unsigned char associateType,
           unsigned char contextID,
           unsigned char reason, char *abstractSyntax,
     LST_HEAD ** proposedTransferSyntax, char *acceptedTransferSyntax,
         PRV_PRESENTATIONCONTEXTITEM * context, unsigned long *rtnLen)
{
    OFCondition cond = EC_Normal;
    unsigned long
        length;
    DUL_SUBITEM
  * subItem;    /* Subitem pointer created for transfer
         * syntax items */
    DUL_TRANSFERSYNTAX
  * transfer;   /* Pointer to loop through list of transfer
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
  if (cond.bad())
      return cond;
  context->length += (unsigned short) length;
  *rtnLen += length;
    } else
  context->abstractSyntax.length = 0;

    context->transferSyntaxList = LST_Create();
    if (context->transferSyntaxList == NULL) return EC_MemoryExhausted;

    if (associateType == DUL_TYPEASSOCIATERQ) {
  transfer = (DUL_TRANSFERSYNTAX*)LST_Head(proposedTransferSyntax);
  if (transfer == NULL) return DUL_LISTERROR;
  (void) LST_Position(proposedTransferSyntax, (LST_NODE*)transfer);

  while (transfer != NULL) {
      subItem = (DUL_SUBITEM *) malloc(sizeof(DUL_SUBITEM));
      if (subItem == NULL) return EC_MemoryExhausted;

      cond = constructSubItem(transfer->transferSyntax,
          DUL_TYPETRANSFERSYNTAX, subItem, &length);
      if (cond.bad()) return cond;

            OFCondition cond2 = LST_Enqueue(&context->transferSyntaxList, (LST_NODE*)subItem);
            if (cond2.bad()) return cond2;

      context->length += (unsigned short) length;
      *rtnLen += length;
      transfer = (DUL_TRANSFERSYNTAX*)LST_Next(proposedTransferSyntax);
  }
    } else {
  subItem = (DUL_SUBITEM *) malloc(sizeof(*subItem));
  if (subItem == NULL) return EC_MemoryExhausted;
  cond = constructSubItem(acceptedTransferSyntax,
        DUL_TYPETRANSFERSYNTAX, subItem, &length);
  if (cond.bad()) {
    free(subItem);
    return cond;
  }

        OFCondition cond2 = LST_Enqueue(&context->transferSyntaxList, (LST_NODE*)subItem);
        if (cond2.bad()) return cond2;

  context->length += (unsigned short) length;
  *rtnLen += length;
    }

    return EC_Normal;
}


/* constructUserInfo
**
** Purpose:
**  Construct the User Info part of the Associate PDU. This function
**  is for calculating the total length of all user item information. It does
**  not yet write the item content to buffer or network. This happens later on
**  in function streamUserInfo().
**
** Parameter Dictionary:
**  type    Type of the User info part
**  params    Service parameters describing the Association
**  userInfo  The user info part that is to be constructed
**  rtnLength Actual length of the constructed part, returned to
**      the caller.
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
static OFCondition
constructUserInfo(unsigned char type, DUL_ASSOCIATESERVICEPARAMETERS * params,
      DUL_USERINFO * userInfo, unsigned long *rtnLen)
{
    // the order in which the user info sub-items are constructed in this
    // function is not significant. The final transmission order is determined
    // by streamUserInfo().

    unsigned long length;
    // needed to detect overflow of total user item lengths (max 65535 bytes).
    // in the end this is copied to userInfo->length
    unsigned long totalUserInfoLength = 0;

    userInfo->type = DUL_TYPEUSERINFO;
    userInfo->rsv1 = 0;
    userInfo->length = 0;
    *rtnLen = 4;

    // construct user info sub-item 51H: maximum length
    OFCondition cond = constructMaxLength(params->maxPDU, &userInfo->maxLength, &length);
    if (cond.bad()) return cond;
    totalUserInfoLength += length;
    *rtnLen += length;

    // construct user info sub-item 52H: implementation class UID
    if (type == DUL_TYPEASSOCIATERQ)
        cond = constructSubItem(params->callingImplementationClassUID,
          DUL_TYPEIMPLEMENTATIONCLASSUID, &userInfo->implementationClassUID, &length);
    else
        cond = constructSubItem(params->calledImplementationClassUID,
            DUL_TYPEIMPLEMENTATIONCLASSUID, &userInfo->implementationClassUID, &length);
    if (cond.bad()) return cond;
    totalUserInfoLength += length;
    *rtnLen += length;

    // user info sub-item 53H (async operations) is not yet implemented!

    // construct user info sub-item 55H: implementation version name
    if (type == DUL_TYPEASSOCIATERQ) {
        if (strlen(params->callingImplementationVersionName) != 0) {
            cond = constructSubItem(params->callingImplementationVersionName,
                DUL_TYPEIMPLEMENTATIONVERSIONNAME,
                &userInfo->implementationVersionName,
                &length);
            if (cond.bad()) return cond;
            totalUserInfoLength += length;
            *rtnLen += length;
        }
     } else {
        if (strlen(params->calledImplementationVersionName) != 0) {
            cond = constructSubItem(params->calledImplementationVersionName,
              DUL_TYPEIMPLEMENTATIONVERSIONNAME,
              &userInfo->implementationVersionName,
              &length);
            if (cond.bad()) return cond;
            totalUserInfoLength += length;
            *rtnLen += length;
        }
    }

    // construct one or more user info sub-items 54H: SCP/SCU role selection
    cond = constructSCUSCPRoles(type, params, &userInfo->SCUSCPRoleList, &length);
    if (cond.bad()) return cond;
    totalUserInfoLength += length;
    *rtnLen += length;

    // construct one or more user info sub-items 56H: extended negotiation
    cond = constructExtNeg(type, params, &userInfo->extNegList, &length);
    if (cond.bad()) return cond;
    totalUserInfoLength += length;
    *rtnLen += length;

    // construct user info sub-item 58H: user identification negotiation
    if (params->reqUserIdentNeg && (type == DUL_TYPEASSOCIATERQ))
    {
      cond = params->reqUserIdentNeg->streamedLength(length);
      if (cond.bad()) return cond;
      userInfo->usrIdent = new UserIdentityNegotiationSubItemRQ();
        *(OFstatic_cast(UserIdentityNegotiationSubItemRQ*,userInfo->usrIdent)) = *(OFstatic_cast(UserIdentityNegotiationSubItemRQ*, params->reqUserIdentNeg));
      totalUserInfoLength += length;
      *rtnLen += length;
    }
    else if (params->ackUserIdentNeg && (type == DUL_TYPEASSOCIATEAC))
    {
      cond = params->ackUserIdentNeg->streamedLength(length);
      if (cond.bad()) return cond;
      userInfo->usrIdent = new UserIdentityNegotiationSubItemAC();
      *(OFstatic_cast(UserIdentityNegotiationSubItemAC*,userInfo->usrIdent)) = *(OFstatic_cast(UserIdentityNegotiationSubItemAC*, params->ackUserIdentNeg));
      totalUserInfoLength += length;
      *rtnLen += length;
    }
    if (totalUserInfoLength > 65535)
    {
      char errbuf[500];
      sprintf(errbuf, "Total length of user items (%lu bytes) exceeds upper limit of 65535 bytes", totalUserInfoLength);
      return makeDcmnetCondition(ASCC_CODINGERROR, OF_error, errbuf);
    }
    else // now casting to unsigned short should be safe
      userInfo->length = OFstatic_cast(unsigned short, totalUserInfoLength);
    return EC_Normal;
}


/* constructMaxLength
**
** Purpose:
**  Construct the Max Length part of the PDU
**
** Parameter Dictionary:
**  maxPDU    Length of the Max PDU
**  max   The Max PDU that is to be constructed
**  rtnLength Length of the PDU constructed.
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/

static OFCondition
constructMaxLength(unsigned long maxPDU, DUL_MAXLENGTH * max,
       unsigned long *rtnLen)
{
    unsigned long compatMode = dcmEnableBackwardCompatibility.get();
    max->type = DUL_TYPEMAXLENGTH;
    max->rsv1 = 0;
    max->length = 4;
    if (compatMode & 0x8000) max->maxLength = DUL_DULCOMPAT | DUL_DIMSECOMPAT | compatMode;
    else max->maxLength = maxPDU;
    *rtnLen = 8;

    return EC_Normal;
}


/* constructSCUSCPRoles
**
** Purpose:
**  Construct the SCU-SCP Role list part of the PDU
**
** Parameter Dictionary:
**  type    Type of the parent PDU
**  params    Service parameters describing the Association
**  list    The SCU_SCP role list that is to be constructed
**  rtnLength Length of the list created.
**
** Return Values:
**
**
** Notes:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
static OFCondition
constructSCUSCPRoles(unsigned char type,
      DUL_ASSOCIATESERVICEPARAMETERS * params, LST_HEAD ** lst,
         unsigned long *rtnLength)
{
  /* Pointer to loop through presentation ctx */
  DUL_PRESENTATIONCONTEXT* presentationCtx;
  PRV_SCUSCPROLE* scuscpItem;
  unsigned char scuRole = 0, scpRole = 0;
  unsigned long length;

  *rtnLength = 0;
  OFCondition cond = EC_Normal;
  if (type == DUL_TYPEASSOCIATERQ)
  {
    presentationCtx = params->requestedPresentationContext != NULL ?
      (DUL_PRESENTATIONCONTEXT*)LST_Head(&params->requestedPresentationContext) :
      (DUL_PRESENTATIONCONTEXT*)NULL;

    if (presentationCtx != NULL)
      (void) LST_Position(&params->requestedPresentationContext, (LST_NODE*)presentationCtx);

    while (presentationCtx != NULL) {
      if (presentationCtx->proposedSCRole != DUL_SC_ROLE_DEFAULT) {
        scuscpItem = (PRV_SCUSCPROLE*)malloc(sizeof(PRV_SCUSCPROLE));
        if (scuscpItem == NULL) return EC_MemoryExhausted;
        if (presentationCtx->proposedSCRole == DUL_SC_ROLE_SCU) {
          scuRole = 1;
          scpRole = 0;
        } else if (presentationCtx->proposedSCRole == DUL_SC_ROLE_SCP) {
          scuRole = 0;
          scpRole = 1;
        } else {
          scuRole = scpRole = 1;
        }
        cond = constructSCUSCPSubItem(presentationCtx->abstractSyntax,
          DUL_TYPESCUSCPROLE, scuRole, scpRole, scuscpItem, &length);
        if (cond.bad())
          return cond;
        *rtnLength += length;
        cond = LST_Enqueue(lst, (LST_NODE*)scuscpItem);
        if (cond.bad()) return cond;
      }
      presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Next(&params->requestedPresentationContext);
    }
  } else {  // type != DUL_TYPEASSOCIATERQ
    presentationCtx = params->acceptedPresentationContext != NULL ?
      (DUL_PRESENTATIONCONTEXT*)LST_Head(&params->acceptedPresentationContext) :
      (DUL_PRESENTATIONCONTEXT*)NULL;

    if (presentationCtx != NULL)
      (void) LST_Position(&params->acceptedPresentationContext, (LST_NODE*)presentationCtx);

    while (presentationCtx != NULL) {
      // check that the default behavior does not apply
      if ((presentationCtx->proposedSCRole != DUL_SC_ROLE_DEFAULT) &&
          (presentationCtx->acceptedSCRole != DUL_SC_ROLE_DEFAULT)) {
        scuscpItem = (PRV_SCUSCPROLE*)malloc(sizeof(*scuscpItem));
        if (scuscpItem == NULL) return EC_MemoryExhausted;
        if (presentationCtx->acceptedSCRole == DUL_SC_ROLE_SCU) {
          // only accept SCU role for the requester if proposed, see PS 3.7
          scuRole = (presentationCtx->proposedSCRole != DUL_SC_ROLE_SCP) ? 1 : 0;
          scpRole = 0;
        } else if (presentationCtx->acceptedSCRole == DUL_SC_ROLE_SCP) {
          scuRole = 0;
          // only accept SCP role for the requester if proposed, see PS 3.7
          scpRole = (presentationCtx->proposedSCRole != DUL_SC_ROLE_SCU) ? 1 : 0;
        } else {
          // only accept roles for the requester if proposed, see PS 3.7
          scuRole = (presentationCtx->proposedSCRole != DUL_SC_ROLE_SCP) ? 1 : 0;
          scpRole = (presentationCtx->proposedSCRole != DUL_SC_ROLE_SCU) ? 1 : 0;
        }
        // neither SCU nor SCP role accepted
        if ((scuRole == 0) && (scpRole == 0)) {
            presentationCtx->acceptedSCRole = DUL_SC_ROLE_NONE;
            DCMNET_WARN("setting accepted SCP/SCU role to NONE, i.e. both role fields are 0 in SCP/SCU role selection sub-item");
        }
        cond = constructSCUSCPSubItem(presentationCtx->abstractSyntax,
          DUL_TYPESCUSCPROLE, scuRole, scpRole, scuscpItem, &length);
        if (cond.bad()) {
          free(scuscpItem);
          return cond;
        }
        *rtnLength += length;
        cond = LST_Enqueue(lst, (LST_NODE*)scuscpItem);
        if (cond.bad()) return cond;
      }
      presentationCtx = (DUL_PRESENTATIONCONTEXT*)LST_Next(&params->acceptedPresentationContext);
    }
  }
  return EC_Normal;
}

/* constructExtNeg
**
** Purpose:
**  Construct the extended negotiation part of the PDU
**
** Notes:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
static OFCondition
constructExtNeg(unsigned char type,
    DUL_ASSOCIATESERVICEPARAMETERS * params, SOPClassExtendedNegotiationSubItemList **lst,
    unsigned long *rtnLength)
{
    unsigned long length;
    *rtnLength = 0;

    if (type == DUL_TYPEASSOCIATERQ && params->requestedExtNegList != NULL) {
        *lst = new SOPClassExtendedNegotiationSubItemList;
        if (*lst == NULL) return EC_MemoryExhausted;
        appendList(*(params->requestedExtNegList), **lst);
    } else if (type == DUL_TYPEASSOCIATEAC && params->acceptedExtNegList != NULL) {
        *lst = new SOPClassExtendedNegotiationSubItemList;
        if (*lst == NULL)  return EC_MemoryExhausted;
        appendList(*(params->acceptedExtNegList), **lst);
    }

    if (*lst != NULL) {
        OFListIterator(SOPClassExtendedNegotiationSubItem*) i = (*lst)->begin();
        while (i != (*lst)->end()) {
            SOPClassExtendedNegotiationSubItem* extNeg = *i;
            extNeg->itemType = 0x56;
            // recompute the length fields
            extNeg->sopClassUIDLength = extNeg->sopClassUID.length();
            extNeg->itemLength = 2 + extNeg->sopClassUIDLength + extNeg->serviceClassAppInfoLength;
            length = 4 + extNeg->itemLength;
            *rtnLength += length;

            ++i;
        }
    }

    return EC_Normal;
}

/* constructSCUSCPSubItem
**
** Purpose:
**  Construct a SCU-SCP subitem part in the parent PDU
**
** Parameter Dictionary:
**  name    SOP Class UID
**  type    Type of the SCU-SCP subitem
**  scuRole   Role played by the SCU
**  scpRole   Role played by the SCP
**  scuscpItem  The subitem to be constructed
**  length    Length of the subitm that is constructed
**
** Return Values:
**
** Notes:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
static OFCondition
constructSCUSCPSubItem(char *name, unsigned char type, unsigned char scuRole,
           unsigned char scpRole, PRV_SCUSCPROLE * scuscpItem,
           unsigned long *length)
{
    if (strlen(name) < 1 || strlen(name) > 64)
    {
        char buf[1024];
        sprintf(buf,"Illegal service parameter: %s", name);
        return makeDcmnetCondition(DULC_ILLEGALSERVICEPARAMETER, OF_error, buf);
    }

    scuscpItem->type = type;
    scuscpItem->rsv1 = 0;
    scuscpItem->SCURole = scuRole;
    scuscpItem->SCPRole = scpRole;
    scuscpItem->length = (unsigned short) (strlen(name) + 2 + 2);
    (void) strcpy(scuscpItem->SOPClassUID, name);

    *length = scuscpItem->length + 4;
    return EC_Normal;
}

/* streamSubItem
**
** Purpose:
**  Convert the subitem into a stream format
**
** Parameter Dictionary:
**  item    The subitem that is to be converted
**  b   The stream version of the subitem
**  len   Length of the stream format
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/

static OFCondition
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

    return EC_Normal;
}

/* streamPresentationContext
**
** Purpose:
**  Convert the presentation context list into stream format
**
** Parameter Dictionary:
**  presentationContextList   The presentation context list that
**          is to be converted to stream format
**  b       The stream version (output)
**  length        Length of the stream version
**
** Return Values:
**
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/

static OFCondition
streamPresentationContext(LST_HEAD ** presentationContextList,
        unsigned char *b, unsigned long *length)
{
    PRV_PRESENTATIONCONTEXTITEM
    * presentation;
    DUL_SUBITEM
  * transfer;
    OFCondition cond = EC_Normal;
    unsigned long
        subLength;

    *length = 0;

    presentation = (PRV_PRESENTATIONCONTEXTITEM*)LST_Head(presentationContextList);
    if (presentation == NULL) return DUL_LISTERROR;
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
      if (cond.bad())
    return cond;
      b += subLength;
      *length += subLength;
  }
  transfer = (DUL_SUBITEM*)LST_Head(&presentation->transferSyntaxList);
  if (transfer == NULL) return DUL_LISTERROR;
  (void) LST_Position(&presentation->transferSyntaxList, (LST_NODE*)transfer);
  while (transfer != NULL) {
      if (transfer->length != 0) {
    cond = streamSubItem(transfer, b, &subLength);
    if (cond.bad())
        return cond;
    b += subLength;
    *length += subLength;
      }
      transfer = (DUL_SUBITEM*)LST_Next(&presentation->transferSyntaxList);
  }
  presentation = (PRV_PRESENTATIONCONTEXTITEM*)LST_Next(presentationContextList);
    }
    return EC_Normal;
}

/* streamUserInfo
**
** Purpose:
**  Convert the stream user info part of the PDU into stream format
**
** Parameter Dictionary:
**  userInfo  The user info structure to be converted to stream
**      format
**  b   The stream version (output)
**  length    Length of the stream version
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/

static OFCondition
streamUserInfo(DUL_USERINFO * userInfo, unsigned char *b,
         unsigned long *length)
{
    unsigned long
        subLength;

    *length = 0;
    *b++ = userInfo->type;
    *b++ = userInfo->rsv1;
    COPY_SHORT_BIG(userInfo->length, b);
    b += 2;
    *length += 4;

    // stream user info sub-item 51H: maximum length
    OFCondition cond = streamMaxLength(&userInfo->maxLength, b, &subLength);
    if (cond.bad())
  return cond;
    b += subLength;
    *length += subLength;

    // stream user info sub-item 52H: implementation class UID
    cond = streamSubItem(&userInfo->implementationClassUID, b, &subLength);
    if (cond.bad())
  return cond;
    b += subLength;
    *length += subLength;

    // user info sub-item 53H (async operations) is not yet implemented!

#ifdef OLD_USER_INFO_SUB_ITEM_ORDER
    /* prior DCMTK releases did not encode user information sub items
     * in ascending order, i.e. they sent 55H followed by 54H and 56H.
     * This behaviour has been "legalized" by DICOM CP 280 but is known
     * to create problems with some other toolkits.
     * Should be re-activated for testing purposes only.
     */

    // stream user info sub-item 55H: implementation version name
    if (userInfo->implementationVersionName.length != 0) {
  cond = streamSubItem(&userInfo->implementationVersionName, b, &subLength);
  if (cond.bad())
      return cond;
  b += subLength;
  *length += subLength;
    }
#endif

    // stream one or more user info sub-items 54H: SCP/SCU role selection
    if (LST_Count(&userInfo->SCUSCPRoleList) != 0) {
  cond = streamSCUSCPList(&userInfo->SCUSCPRoleList, b, &subLength);
  if (cond.bad())
      return cond;
  b += subLength;
  *length += subLength;
    }

#ifndef OLD_USER_INFO_SUB_ITEM_ORDER
    // stream user info sub-item 55H: implementation version name
    if (userInfo->implementationVersionName.length != 0) {
  cond = streamSubItem(&userInfo->implementationVersionName, b, &subLength);
  if (cond.bad())
      return cond;
  b += subLength;
  *length += subLength;
    }
#endif

    // stream one or more user info sub-items 56H: extended negotiation
    if (userInfo->extNegList != NULL) {
  cond = streamExtNegList(userInfo->extNegList, b, &subLength);
  if (cond.bad())
      return cond;
  b += subLength;
  *length += subLength;
    }

  // stream user info sub-item 58H: user identity negotiation
  if (userInfo->usrIdent != NULL) {
    cond = userInfo->usrIdent->stream(b, subLength /*out*/);
    if (cond.bad())
      return cond;
    b += subLength;
    *length += subLength;
  }

    return EC_Normal;
}

/* streamMaxLength
**
** Purpose:
**  Convert the Max Length structure into stream format
**
** Parameter Dictionary:
**  max   Max Length structure to be converted to
**      stream format
**  b   The stream version (output)
**  length    Length of the stream version
**
** Return Values:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
static OFCondition
streamMaxLength(DUL_MAXLENGTH * max, unsigned char *b,
    unsigned long *length)
{

    *b++ = max->type;
    *b++ = max->rsv1;
    COPY_SHORT_BIG(max->length, b);
    b += 2;
    COPY_LONG_BIG(max->maxLength, b);

    *length = 8;
    return EC_Normal;
}

/* streamSCUSCPList
**
** Purpose:
**  Convert the SCU-SCP list into stream format
**
** Parameter Dictionary:
**  list    SCU-SCP list
**  b   The stream version (output)
**  length    Length of the stream version
**
** Return Values:
**
** Notes:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
static OFCondition
streamSCUSCPList(LST_HEAD ** lst, unsigned char *b, unsigned long *length)
{
    PRV_SCUSCPROLE
    * scuscpRole;
    OFCondition cond = EC_Normal;
    unsigned long
        localLength;

    *length = 0;
    scuscpRole = (PRV_SCUSCPROLE*)LST_Head(lst);
    if (scuscpRole != NULL)
  (void) LST_Position(lst, (LST_NODE*)scuscpRole);
    while (scuscpRole != NULL) {
  localLength = 0;
  cond = streamSCUSCPRole(scuscpRole, b, &localLength);
  if (cond.bad())
      return cond;
  *length += localLength;
  b += localLength;
  scuscpRole = (PRV_SCUSCPROLE*)LST_Next(lst);
    }
    return EC_Normal;
}

/* streamSCUSCPRole
**
** Purpose:
**  Convert the SCU-SCP role list into stream format
**
** Parameter Dictionary:
**  scuscpRole  SCU-SCP role list that is to be converted to
**      stream format
**  b   The stream version (output)
**  length    Length of the stream version
**
** Return Values:
**
** Notes:
**
** Algorithm:
**  Description of the algorithm (optional) and any other notes.
*/
static OFCondition
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

    return EC_Normal;
}

static OFCondition
streamExtNegList(SOPClassExtendedNegotiationSubItemList *lst, unsigned char *b, unsigned long *length)
{
  OFCondition cond = EC_Normal;
  unsigned long localLength;

  *length = 0;

  if (lst == NULL)
    return EC_Normal;

  OFListIterator(SOPClassExtendedNegotiationSubItem*) i = lst->begin();
  while (i != lst->end()) {
    localLength = 0;
    cond = streamExtNeg(*i, b, &localLength);
    if (cond.bad())
      return cond;
    *length += localLength;
    b += localLength;
    ++i;
  }
  return EC_Normal;
}

static OFCondition
streamExtNeg(SOPClassExtendedNegotiationSubItem* extNeg, unsigned char *b, unsigned long *len)
{

    if (extNeg == NULL)
        return EC_Normal;

    extNeg->itemType = 0x56;
    // recompute the length fields
    extNeg->sopClassUIDLength = extNeg->sopClassUID.length();
    extNeg->itemLength = 2 + extNeg->sopClassUIDLength + extNeg->serviceClassAppInfoLength;

    *b++ = extNeg->itemType;
    *b++ = extNeg->reserved1;
    COPY_SHORT_BIG(extNeg->itemLength, b);
    b += 2;

    COPY_SHORT_BIG(extNeg->sopClassUIDLength, b);
    b += 2;

    (void) memcpy(b, extNeg->sopClassUID.c_str(), extNeg->sopClassUIDLength);
    b += extNeg->sopClassUIDLength;

    (void) memcpy(b, extNeg->serviceClassAppInfo, extNeg->serviceClassAppInfoLength);
    b += extNeg->serviceClassAppInfoLength;

    *len = 4 + extNeg->itemLength;

    return EC_Normal;
}

/*
** CVS Log
** $Log: dulconst.cc,v $
** Revision 1.28  2010-12-01 08:26:36  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.27  2010-10-14 13:14:28  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.26  2010-09-08 07:31:29  uli
** Fix wrong use of logical invert. A bit-wise invert is meant here.
**
** Revision 1.25  2010-08-26 09:25:10  joergr
** Fixed incorrect behavior of association acceptors during SCP/SCU role
** selection negotiation.
**
** Revision 1.24  2010-08-24 09:21:29  uli
** Fixed a NULL pointer dereference if ASC_acknowledgeAssociation() was called
** without a previous ASC_acceptContextsWithPreferredTransferSyntaxes().
**
** Revision 1.23  2010-03-05 08:37:23  uli
** Fixed possible memory leak in case of error during construction
** of sub-items. This was found with cppcheck.
**
** Revision 1.22  2009-12-16 16:56:52  onken
** Fixed bug in scu/scp role negotation.
**
** Revision 1.21  2009-11-18 11:53:59  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.20  2008-04-17 15:27:35  onken
** Reworked and extended User Identity Negotiation code.
**
** Revision 1.19  2007-09-28 13:15:45  onken
** Fixed crash of network code when processing association requests without any
** offered presentation contexts. Minor changes in source formatting.
**
** Revision 1.18  2007/09/07 08:49:29  onken
** Added basic support for Extended Negotiation of User Identity.
**
** Revision 1.17  2006/08/15 16:04:29  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.16  2005/12/08 15:44:49  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.15  2004/06/16 12:51:34  meichel
** Fixed minor bug in DUL backward compatibility code
**
** Revision 1.14  2004/02/25 12:31:17  meichel
** Added global option flag for compatibility with very old DCMTK releases in the
**   DICOM upper layer and ACSE code. Default is automatic handling, which should
**   work in most cases.
**
** Revision 1.13  2003/06/02 16:44:11  meichel
** Renamed local variables to avoid name clashes with STL
**
** Revision 1.12  2002/11/27 13:04:44  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.11  2002/11/26 15:37:02  meichel
** Changed DUL code to always send A-ASSOCIATE user information sub-items
**   in ascending order.
**
** Revision 1.10  2001/11/01 13:49:09  wilkens
** Added lots of comments.
**
** Revision 1.9  2001/10/12 10:18:38  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.8  2000/03/03 14:11:23  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.7  2000/02/23 15:12:45  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.6  2000/02/01 10:24:12  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.5  1999/04/19 08:38:58  meichel
** Added experimental support for extended SOP class negotiation.
**
** Revision 1.4  1997/07/21 08:47:21  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.3  1997/07/04 09:24:54  meichel
** Simplified some sizeof() constructs to avoid compiler warnings
**   on the IBM xlC compiler (AIX 3.x).
**
** Revision 1.2  1996/06/20 07:35:49  hewett
** Removed inclusion of system header already included by dcompat.h
** and made sure that dcompat.h is always included (via dicom.h).
**
** Revision 1.1.1.1  1996/03/26 18:38:46  hewett
** Initial Release.
**
**
*/
