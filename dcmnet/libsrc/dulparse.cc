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
** Module Name(s):  parseAssociate
**                  parseDebug
** Author, Date:    Stephen M. Moore, 15-Apr-93
** Intent:          This file contains functions for parsing Dicom
**                  Upper Layer (DUL) Protocol Data Units (PDUs)
**                  into logical in-memory structures.
** Last Update:     $Author: joergr $, $Date: 2010-12-01 08:26:36 $
** Revision:        $Revision: 1.33 $
** Status:          $State: Exp $
*/


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/dcmnet/lst.h"
#include "dcmtk/dcmnet/dul.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dulstruc.h"
#include "dulpriv.h"
#include "dcmtk/ofstd/ofconsol.h"

static OFCondition
parseSubItem(DUL_SUBITEM * subItem, unsigned char *buf,
             unsigned long *itemLength, unsigned long availData);
static OFCondition
parsePresentationContext(unsigned char type,
                         PRV_PRESENTATIONCONTEXTITEM * context,
                         unsigned char *buf, unsigned long *itemLength,
                         unsigned long availData);
static OFCondition
parseUserInfo(DUL_USERINFO * userInfo,
              unsigned char *buf, unsigned long *itemLength,
              unsigned char typeRQorAC, unsigned long availData);
static OFCondition
parseMaxPDU(DUL_MAXLENGTH * max, unsigned char *buf,
            unsigned long *itemLength, unsigned long availData);
static OFCondition
    parseDummy(unsigned char *buf, unsigned long *itemLength,
            unsigned long availData);
static OFCondition
parseSCUSCPRole(PRV_SCUSCPROLE * role, unsigned char *buf,
                unsigned long *length, unsigned long availData);
static void trim_trailing_spaces(char *s);

static OFCondition
parseExtNeg(SOPClassExtendedNegotiationSubItem* extNeg, unsigned char *buf,
            unsigned long *length, unsigned long availData);

static OFCondition
makeLengthError(const char *pdu, unsigned long bufSize, unsigned long minSize = 0,
        unsigned long length = 0);

/* parseAssociate
**
** Purpose:
**      Parse the buffer (read from the socket) and extract an Associate
**      PDU from it.
**
** Parameter Dictionary:
**      buf             Buffer holding the PDU in the stream format
**      pduLength       Length of the buffer
**      assoc           The Associate PDU to be extracted
**                      (returend to the caller)
**
** Return Values:
**
**      DUL_ILLEGALPDU
**      DUL_LISTERROR
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

OFCondition
parseAssociate(unsigned char *buf, unsigned long pduLength,
               PRV_ASSOCIATEPDU * assoc)
{
    OFCondition cond = EC_Normal;
    unsigned char
        type;
    unsigned long
        itemLength;
    PRV_PRESENTATIONCONTEXTITEM
        * context;

    (void) memset(assoc, 0, sizeof(*assoc));
    if ((assoc->presentationContextList = LST_Create()) == NULL) return EC_MemoryExhausted;
    if ((assoc->userInfo.SCUSCPRoleList = LST_Create()) == NULL) return EC_MemoryExhausted;

    // Check if the PDU actually is long enough for the fields we read
    if (pduLength < 2 + 2 + 16 + 16 + 32)
        return makeLengthError("associate PDU", pduLength, 2 + 2 + 16 + 16 + 32);

    assoc->type = *buf++;
    assoc->rsv1 = *buf++;
    EXTRACT_LONG_BIG(buf, assoc->length);
    buf += 4;

    EXTRACT_SHORT_BIG(buf, assoc->protocol);
    buf += 2;
    pduLength -= 2;
    if ((assoc->protocol & DUL_PROTOCOL) == 0)
    {
        char buffer[256];
        sprintf(buffer, "DUL Unsupported peer protocol %04x; expected %04x in %s", assoc->protocol, DUL_PROTOCOL, "parseAssociate");
        return makeDcmnetCondition(DULC_UNSUPPORTEDPEERPROTOCOL, OF_error, buffer);
    }
    assoc->rsv2[0] = *buf++;
    pduLength--;
    assoc->rsv2[1] = *buf++;
    pduLength--;
    (void) strncpy(assoc->calledAPTitle, (char *) buf, 16);
    assoc->calledAPTitle[16] = '\0';
    trim_trailing_spaces(assoc->calledAPTitle);

    buf += 16;
    pduLength -= 16;
    (void) strncpy(assoc->callingAPTitle, (char *) buf, 16);
    assoc->callingAPTitle[16] = '\0';
    trim_trailing_spaces(assoc->callingAPTitle);
    buf += 16;
    pduLength -= 16;
    (void) memcpy(assoc->rsv3, buf, 32);
    buf += 32;
    pduLength -= 32;

    if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::DEBUG_LOG_LEVEL)) {
        const char *s;
        DCMNET_DEBUG("Parsing an A-ASSOCIATE PDU");
        if (assoc->type == DUL_TYPEASSOCIATERQ)
            s = "A-ASSOCIATE RQ";
        else if (assoc->type == DUL_TYPEASSOCIATEAC)
            s = "A-ASSOCIATE AC";
        else
            s = "Unknown: Programming bug in parseAssociate";

/*      If we hit the "Unknown type", there is a programming bug somewhere.
**      This function is only supposed to parse A-ASSOCIATE PDUs and
**      expects its input to have been properly screened.
*/
        DCMNET_TRACE("PDU type: "
            << STD_NAMESPACE hex << ((unsigned int)assoc->type)
            << STD_NAMESPACE dec << " (" << s << "), PDU Length: " << assoc->length << OFendl
            << "DICOM Protocol: "
            << STD_NAMESPACE hex << assoc->protocol
            << STD_NAMESPACE dec << OFendl
            << "Called AP Title:  " << assoc->calledAPTitle << OFendl
            << "Calling AP Title: " << assoc->callingAPTitle);
    }
    while ((cond.good()) && (pduLength > 0))
    {
        type = *buf;
        DCMNET_TRACE("Parsing remaining " << pduLength << " bytes of A-ASSOCIATE PDU" << OFendl
                << "Next item type: "
                << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << ((unsigned int)type));
        switch (type) {
        case DUL_TYPEAPPLICATIONCONTEXT:
            cond = parseSubItem(&assoc->applicationContext,
                                buf, &itemLength, pduLength);
            if (cond.good())
            {
                buf += itemLength;
                pduLength -= itemLength;
                DCMNET_TRACE("Successfully parsed Application Context");
            }
            break;
        case DUL_TYPEPRESENTATIONCONTEXTRQ:
        case DUL_TYPEPRESENTATIONCONTEXTAC:
            context = (PRV_PRESENTATIONCONTEXTITEM*)malloc(sizeof(PRV_PRESENTATIONCONTEXTITEM));
            if (context == NULL) return EC_MemoryExhausted;
            (void) memset(context, 0, sizeof(*context));
            cond = parsePresentationContext(type, context, buf, &itemLength, pduLength);
            if (cond.bad()) return cond;
            buf += itemLength;
            pduLength -= itemLength;
            cond = LST_Enqueue(&assoc->presentationContextList, (LST_NODE*)context);
            if (cond.bad()) return cond;
            DCMNET_TRACE("Successfully parsed Presentation Context");
            break;
        case DUL_TYPEUSERINFO:
            cond = parseUserInfo(&assoc->userInfo, buf, &itemLength, assoc->type, pduLength);
            if (cond.bad())
                return cond;
            buf += itemLength;
            pduLength -= itemLength;
            DCMNET_TRACE("Successfully parsed User Information");
            break;
        default:
            cond = parseDummy(buf, &itemLength, pduLength);
            buf += itemLength;
            pduLength -= itemLength;
            break;
        }
    }
    return cond;
}


/* ============================================================
**  Private functions (to this module) defined below.
*/

/* parseSubItem
**
** Purpose:
**      Parse the buffer and extract the subitem structure
**
** Parameter Dictionary:
**      subItem         The subitem structure to be extracted
**      buf             Buffer to be parsed
**      itemLength      Length of the subitem extracted
**
** Return Values:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
parseSubItem(DUL_SUBITEM * subItem, unsigned char *buf,
             unsigned long *itemLength, unsigned long availData)
{
    // Need at least 4 bytes (type, rsv1, two bytes length field)
    if (availData < 4)
        return makeLengthError("subitem", availData, 4);

    subItem->type = *buf++;
    subItem->rsv1 = *buf++;
    EXTRACT_SHORT_BIG(buf, subItem->length);
    buf += 2;

    // Maximum allowed size and our buffer size is DICOM_UI_LENGTH
    if (subItem->length > DICOM_UI_LENGTH)
    {
        char buffer[256];
        sprintf(buffer, "DUL illegal subitem length %d. Maximum allowed size is %d.",
               subItem->length, DICOM_UI_LENGTH);
        return makeDcmnetCondition(DULC_ILLEGALPDULENGTH, OF_error, buffer);
    }

    // Does the subitem claim to be larger than the containing PDU?
    if (availData - 4 < subItem->length)
        return makeLengthError("subitem", availData, 0, subItem->length);

    (void) memcpy(subItem->data, buf, subItem->length);
    subItem->data[subItem->length] = '\0';

    *itemLength = 2 + 2 + subItem->length;

    DCMNET_TRACE("Subitem parse: Type "
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << ((unsigned int)subItem->type)
            << STD_NAMESPACE dec << ", Length " << STD_NAMESPACE setw(4) << (int)subItem->length
            << ", Content: " << subItem->data);
    return EC_Normal;
}


/* parsePresentationContext
**
** Purpose:
**      Parse the buffer and extract the presentation context.
**
** Parameter Dictionary:
**      context         The presentation context that is to be extracted
**      buf             The buffer to be parsed
**      itemLength      Total length of the presentation context that is
**                      extracted
**
** Return Values:
**
**      DUL_ILLEGALPDU
**      DUL_LISTERROR
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
parsePresentationContext(unsigned char type,
                  PRV_PRESENTATIONCONTEXTITEM * context, unsigned char *buf,
                         unsigned long *itemLength, unsigned long availData)
{
    unsigned long
        length;
    unsigned long
        presentationLength;
    OFCondition cond = EC_Normal;
    DUL_SUBITEM
        * subItem;

    // We need at least 8 bytes, anything smaller would be reading past the end
    if (availData < 8)
        return makeLengthError("presentation context", availData, 8);

    if ((context->transferSyntaxList = LST_Create()) == NULL) return EC_MemoryExhausted;

    *itemLength = 0;
    context->type = *buf++;
    context->rsv1 = *buf++;
    EXTRACT_SHORT_BIG(buf, context->length);
    buf += 2;
    context->contextID = *buf++;
    context->rsv2 = *buf++;
    context->result = *buf++;
    context->rsv3 = *buf++;

    length = context->length;
    *itemLength = 2 + 2 + length;

    // Does the length field claim to be larger than the containing PDU?
    if (availData - 4 < length || length < 4)
        return makeLengthError("presentation context", availData, 4, length);

    DCMNET_TRACE("Parsing Presentation Context: ("
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << (unsigned int)context->type
            << STD_NAMESPACE dec << "), Length: " << (unsigned long)context->length << OFendl
            << "Presentation Context ID: "
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << (unsigned int)context->contextID
            << STD_NAMESPACE dec);
    presentationLength = length - 4;
    if (!((type == DUL_TYPEPRESENTATIONCONTEXTAC) &&
          (context->result != DUL_PRESENTATION_ACCEPT))) {
        while (presentationLength > 0) {
            DCMNET_TRACE("Parsing remaining " << presentationLength << " bytes of Presentation Context" << OFendl
                    << "Next item type: "
                    << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << (unsigned int)*buf);
            switch (*buf) {
            case DUL_TYPEABSTRACTSYNTAX:
                cond = parseSubItem(&context->abstractSyntax, buf, &length, presentationLength);
                if (cond.bad())
                    return cond;

                buf += length;
                presentationLength -= length;
                DCMNET_TRACE("Successfully parsed Abstract Syntax");
                break;
            case DUL_TYPETRANSFERSYNTAX:
                subItem = (DUL_SUBITEM*)malloc(sizeof(DUL_SUBITEM));
                if (subItem == NULL) return EC_MemoryExhausted;
                cond = parseSubItem(subItem, buf, &length, presentationLength);
                if (cond.bad()) return cond;
                cond = LST_Enqueue(&context->transferSyntaxList, (LST_NODE*)subItem);
                if (cond.bad()) return cond;
                buf += length;
                presentationLength -= length;
                DCMNET_TRACE("Successfully parsed Transfer Syntax");
                break;
            default:
                cond = parseDummy(buf, &length, presentationLength);
                buf += length;
                presentationLength -= length;
                break;
            }
        }
    }
    return EC_Normal;
}



/* parseUserInfo
**
** Purpose:
**      Parse the buffer and extract the user info structure
**
** Parameter Dictionary:
**      userInfo        Structure to hold the extracted user info information
**      buf             The buffer that is to be parsed
**      itemLength      Length of structure extracted.
**
** Return Values:
**
**      DUL_ILLEGALPDU
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static OFCondition
parseUserInfo(DUL_USERINFO * userInfo,
              unsigned char *buf,
              unsigned long *itemLength,
              unsigned char typeRQorAC,
              unsigned long availData)
{
    unsigned short userLength;
    unsigned long length;
    OFCondition cond = EC_Normal;
    PRV_SCUSCPROLE *role;
    SOPClassExtendedNegotiationSubItem *extNeg = NULL;
    UserIdentityNegotiationSubItem *usrIdent = NULL;

    // The minimum allowed size is 4 byte, else we read past the buffer end
    if (availData < 4)
        return makeLengthError("user info", availData, 4);

    userInfo->type = *buf++;
    userInfo->rsv1 = *buf++;
    EXTRACT_SHORT_BIG(buf, userInfo->length);
    buf += 2;

    userLength = userInfo->length;
    *itemLength = userLength + 4;

    // Does this item claim to be larger than the available data?
    if (availData - 4 < userLength)
        return makeLengthError("user info", availData, 0, userLength);

    DCMNET_TRACE("Parsing user info field ("
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << (unsigned int)userInfo->type
            << STD_NAMESPACE dec << "), Length: " << (unsigned long)userInfo->length);
    while (userLength > 0) {
        DCMNET_TRACE("Parsing remaining " << (long)userLength << " bytes of User Information" << OFendl
                << "Next item type: "
                << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << (unsigned int)*buf);
        switch (*buf) {
        case DUL_TYPEMAXLENGTH:
            cond = parseMaxPDU(&userInfo->maxLength, buf, &length, userLength);
            if (cond.bad())
                return cond;
            buf += length;
            userLength -= (unsigned short) length;
            DCMNET_TRACE("Successfully parsed Maximum PDU Length");
            break;
        case DUL_TYPEIMPLEMENTATIONCLASSUID:
            cond = parseSubItem(&userInfo->implementationClassUID,
                                buf, &length, userLength);
            if (cond.bad())
                return cond;
            buf += length;
            userLength -= (unsigned short) length;
            break;

        case DUL_TYPEASYNCOPERATIONS:
            cond = parseDummy(buf, &length, userLength);
            buf += length;
            userLength -= (unsigned short) length;
            break;
        case DUL_TYPESCUSCPROLE:
            role = (PRV_SCUSCPROLE*)malloc(sizeof(PRV_SCUSCPROLE));
            if (role == NULL) return EC_MemoryExhausted;
            cond = parseSCUSCPRole(role, buf, &length, userLength);
            if (cond.bad()) return cond;
            cond = LST_Enqueue(&userInfo->SCUSCPRoleList, (LST_NODE*)role);
            if (cond.bad()) return cond;
            buf += length;
            userLength -= (unsigned short) length;
            break;
        case DUL_TYPEIMPLEMENTATIONVERSIONNAME:
            cond = parseSubItem(&userInfo->implementationVersionName,
                                buf, &length, userLength);
            if (cond.bad()) return cond;
            buf += length;
            userLength -= (unsigned short) length;
            break;

        case DUL_TYPESOPCLASSEXTENDEDNEGOTIATION:
            /* parse an extended negotiation sub-item */
            extNeg = new SOPClassExtendedNegotiationSubItem;
            if (extNeg == NULL)  return EC_MemoryExhausted;
            cond = parseExtNeg(extNeg, buf, &length, userLength);
            if (cond.bad()) return cond;
            if (userInfo->extNegList == NULL)
            {
                userInfo->extNegList = new SOPClassExtendedNegotiationSubItemList;
                if (userInfo->extNegList == NULL)  return EC_MemoryExhausted;
            }
            userInfo->extNegList->push_back(extNeg);
            buf += length;
            userLength -= (unsigned short) length;
            break;

        case DUL_TYPENEGOTIATIONOFUSERIDENTITY:
          if (typeRQorAC == DUL_TYPEASSOCIATERQ)
            usrIdent = new UserIdentityNegotiationSubItemRQ();
          else // assume DUL_TYPEASSOCIATEAC
            usrIdent = new UserIdentityNegotiationSubItemAC();
          if (usrIdent == NULL) return EC_MemoryExhausted;
          cond = usrIdent->parseFromBuffer(buf, length /*return value*/, userLength);
          if (cond.bad())
          {
            delete usrIdent;
            return cond;
          }
          userInfo->usrIdent = usrIdent;
          buf += length;
          userLength -= (unsigned short) length;
          break;
        default:
            cond = parseDummy(buf, &length, userLength);
            buf += length;
            userLength -= (unsigned short) length;
            break;
        }
    }

    return EC_Normal;
}



/* parseMaxPDU
**
** Purpose:
**      Parse the buffer and extract the Max PDU structure.
**
** Parameter Dictionary:
**      max             The structure to hold the Max PDU
**      buf             The buffer that is to be parsed
**      itemLength      Length of structure extracted.
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
parseMaxPDU(DUL_MAXLENGTH * max, unsigned char *buf,
            unsigned long *itemLength, unsigned long availData)
{
    // We want to read 8 bytes of data, is there enough data?
    if (availData < 8)
        return makeLengthError("Max PDU", availData, 8);

    max->type = *buf++;
    max->rsv1 = *buf++;
    EXTRACT_SHORT_BIG(buf, max->length);
    buf += 2;
    EXTRACT_LONG_BIG(buf, max->maxLength);
    *itemLength = 2 + 2 + max->length;

    if (max->length != 4)
        DCMNET_WARN("Invalid length (" << max->length << ") for maximum length item, must be 4");

    DCMNET_TRACE("Maximum PDU Length: " << (unsigned long)max->maxLength);

    return EC_Normal;
}

/* parseDummy
**
** Purpose:
**      Parse the buffer to extract just a dummy structure of length
**      User Length
**
** Parameter Dictionary:
**      buf             The buffer that is to be parsed
**      itemLength      Length of structure extracted.
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
parseDummy(unsigned char *buf, unsigned long *itemLength, unsigned long availData)
{
    unsigned short
        userLength;

    // Is there enough data for the length field?
    if (availData < 4)
        return makeLengthError("dummy item", availData, 4);

    buf++;
    buf++;
    EXTRACT_SHORT_BIG(buf, userLength);
    buf += 2;

    // Is there less data than the length field claims there is?
    if (availData - 4 < userLength)
        return makeLengthError("dummy item", availData, 0, userLength);

    *itemLength = userLength + 4;
    return EC_Normal;
}

/* parseSCUSCPRole
**
** Purpose:
**      Parse the buffer and extract the SCU-SCP role list
**
** Parameter Dictionary:
**      role            The structure to hold the SCU-SCP role list
**      buf             The buffer that is to be parsed
**      itemLength      Length of structure extracted.
**
** Return Values:
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/
static OFCondition
parseSCUSCPRole(PRV_SCUSCPROLE * role, unsigned char *buf,
                unsigned long *length, unsigned long availData)
{
    unsigned short
        UIDLength;

    // We need at least 8 bytes of data, else we read past the end of buf
    if (availData < 8)
        return makeLengthError("SCU-SCP role list", availData, 8);

    role->type = *buf++;
    role->rsv1 = *buf++;
    EXTRACT_SHORT_BIG(buf, role->length);
    buf += 2;

    EXTRACT_SHORT_BIG(buf, UIDLength);
    buf += 2;

    // Check if all the length fields are valid (we have the minimum needed
    // number of bytes, no field is larger than its surrounding field).
    if (availData - 4 < role->length)
        return makeLengthError("SCU-SCP role list", availData, 0, role->length);
    if (role->length < 4)
        return makeLengthError("SCU-SCP role list UID", role->length, 4);
    if (role->length - 4 < UIDLength)
        return makeLengthError("SCU-SCP role list UID", role->length, 0, UIDLength);

    (void) memcpy(role->SOPClassUID, buf, UIDLength);
    role->SOPClassUID[UIDLength] = '\0';
    buf += UIDLength;
    role->SCURole = *buf++;
    role->SCPRole = *buf++;

    *length = 2 + 2 + role->length;

    DCMNET_TRACE("Subitem parse: Type "
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << (unsigned int)role->type
            << STD_NAMESPACE dec << ", Length " << STD_NAMESPACE setw(4) << (int)role->length
            << ", Content: " << role->SOPClassUID << " " << (int)role->SCURole << " " << (int)role->SCPRole);
    return EC_Normal;
}

/* parseExtNeg
**
** Purpose:
**      Parse the buffer and extract the extended negotiation item
**
** Return Values:
**
*/

static OFCondition
parseExtNeg(SOPClassExtendedNegotiationSubItem* extNeg, unsigned char *buf,
                unsigned long *length, unsigned long availData)
{
    unsigned char *bufStart = buf;

    // An extended negotiation item has to be at least 6 bytes large
    if (availData < 6)
        return makeLengthError("extended negotiation", availData, 6);

    extNeg->itemType = *buf++;
    extNeg->reserved1 = *buf++;
    EXTRACT_SHORT_BIG(buf, extNeg->itemLength);
    buf += 2;

    EXTRACT_SHORT_BIG(buf, extNeg->sopClassUIDLength);
    buf += 2;

    // Check if all the length fields are valid (we have the minimum needed
    // number of bytes, no field is larger than its surrounding field).
    if (availData - 4 < extNeg->itemLength)
        return makeLengthError("extended negotiation", availData, 0, extNeg->itemLength);
    if (extNeg->itemLength < 2)
        return makeLengthError("extended negotiation item", availData, 2);
    if (extNeg->itemLength - 2 < extNeg->sopClassUIDLength)
        return makeLengthError("extended negotiation item", extNeg->itemLength, 0, extNeg->sopClassUIDLength);

    extNeg->sopClassUID.append((const char*)buf, extNeg->sopClassUIDLength);
    buf += extNeg->sopClassUIDLength;

    *length = 2 + 2 + extNeg->itemLength;

    int remain = (int)(*length - (buf - bufStart));

    extNeg->serviceClassAppInfoLength = remain;
    extNeg->serviceClassAppInfo = new unsigned char[remain];
    for (int i=0; i<remain; i++) {
        extNeg->serviceClassAppInfo[i] = *buf++;
    }

    if (DCM_dcmnetGetLogger().isEnabledFor(OFLogger::TRACE_LOG_LEVEL)) {
        DCMNET_TRACE("ExtNeg Subitem parse: Type "
            << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << OFstatic_cast(unsigned int, extNeg->itemType)
            << STD_NAMESPACE dec << ", Length " << STD_NAMESPACE setw(4) << (int)extNeg->itemLength
            << ", SOP Class: " << extNeg->sopClassUID.c_str());

        OFOStringStream str;
        str << "   values: ";
        for (int j=0; j<extNeg->serviceClassAppInfoLength; j++) {
            str << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(2) << extNeg->serviceClassAppInfo[j]
                << STD_NAMESPACE dec << " ";
        }
        str << OFStringStream_ends;
        OFSTRINGSTREAM_GETOFSTRING(str, res)
        DCMNET_TRACE(res);
    }

    return EC_Normal;
}

/* makeLengthError
 *
 * This function is used to generate the OFCondition code for an invalid field
 * length in a PDU.
 *
 * @param pdu The name of the field or PDU which got an invalid length field.
 * @param bufSize The size of the buffer that we received.
 * @param length The length as given by the length field.
 * @param minSize The minimum size that a 'pdu' has to have.
 */
static OFCondition
makeLengthError(const char *pdu, unsigned long bufSize, unsigned long minSize,
        unsigned long length)
{
    OFStringStream stream;
    stream << "DUL Illegal " << pdu << ". Got " << bufSize << " bytes of data";
    if (length != 0)
        stream << " with a length field of " << length << " (data before length field is not included in length field)";
    if (minSize != 0)
        stream << ". The minimum allowed size is " << minSize;
    stream << "." << OFStringStream_ends;

    OFCondition ret;
    OFSTRINGSTREAM_GETSTR(stream, tmpString)
    ret = makeDcmnetCondition(DULC_ILLEGALPDULENGTH, OF_error, tmpString);
    OFSTRINGSTREAM_FREESTR(tmpString)
    return ret;
}

/* trim_trailing_spaces
**
** Purpose:
**      trim trailing spaces
**
** Parameter Dictionary:
**      s       The character string from which the trailing spaces are to be
**              removed.
**
** Return Values:
**      None
**
** Notes:
**
** Algorithm:
**      Description of the algorithm (optional) and any other notes.
*/

static void
trim_trailing_spaces(char *s)
{
    char
       *p;

    p = s;
    while (*p != '\0')
        p++;

    if (p == s)
        return;

    p--;
    while (p >= s && *p == ' ')
        *p-- = '\0';
}

/*
** CVS Log
** $Log: dulparse.cc,v $
** Revision 1.33  2010-12-01 08:26:36  joergr
** Added OFFIS copyright header (beginning with the year 1994).
**
** Revision 1.32  2010-10-14 13:14:29  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.31  2010-09-14 11:42:14  uli
** Verify the length fields in the PDUs that we receive.
**
** Revision 1.30  2010-01-20 10:01:01  joergr
** Made sure that the calling and called AE title strings are always
** null-terminated.
**
** Revision 1.29  2009-11-18 11:53:59  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.28  2008-04-17 15:27:35  onken
** Reworked and extended User Identity Negotiation code.
**
** Revision 1.27  2007-09-07 08:49:30  onken
** Added basic support for Extended Negotiation of User Identity.
**
** Revision 1.26  2007/03/12 13:27:53  joergr
** Updated debug code to correctly compile when all standard C++ classes remain
** in namespace std.
**
** Revision 1.25  2006/08/15 16:04:29  meichel
** Updated the code in module dcmnet to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.24  2005/12/08 15:44:52  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.23  2004/02/04 15:33:48  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.22  2002/12/09 13:20:29  joergr
** Renamed local variable to avoid name clash with function parameter "buf".
**
** Revision 1.21  2002/11/29 12:15:25  wilkens
** Modified call to getsockopt() in order to avoid compiler warning.
** Modified variable initialization in order to avoid compiler warning.
** Corrected dumping of hex values.
**
** Revision 1.20  2002/11/27 13:04:46  meichel
** Adapted module dcmnet to use of new header file ofstdinc.h
**
** Revision 1.19  2002/04/16 13:57:34  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.18  2001/11/09 15:58:55  joergr
** Added '#include <iostream.h>' to avoid compiler errors reported by Sun CC
** 2.0.1 (required before <iomanip.h> is included).
**
** Revision 1.17  2001/11/02 15:18:15  meichel
** Including iomanip.h, required for gcc 3.0.x
**
** Revision 1.16  2001/10/12 10:18:40  meichel
** Replaced the CONDITION types, constants and functions in the dcmnet module
**   by an OFCondition based implementation which eliminates the global condition
**   stack.  This is a major change, caveat emptor!
**
** Revision 1.15  2001/06/05 10:07:10  joergr
** Minor code purifications to keep Sun CC 2.0.1 quiet.
**
** Revision 1.14  2000/03/08 11:23:40  joergr
** Added debug version of function 'parseDebug' to avoid compiler warnings
** (unused variable).
**
** Revision 1.13  2000/03/06 18:14:25  joergr
** Avoid empty statement in the body of if-statements (MSVC6 reports warnings).
**
** Revision 1.12  2000/03/03 14:11:25  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.11  2000/02/23 15:12:50  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.10  2000/02/01 10:24:14  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.9  1999/05/03 14:35:42  joergr
** Minor code purifications to keep Sun CC 2.0.1 quiet.
**
** Revision 1.8  1999/05/03 14:12:32  joergr
** Minor code purifications to keep Sun CC 2.0.1 quiet.
**
** Revision 1.7  1999/04/19 08:39:00  meichel
** Added experimental support for extended SOP class negotiation.
**
** Revision 1.6  1999/03/29 11:20:07  meichel
** Cleaned up dcmnet code for char* to const char* assignments.
**
** Revision 1.5  1997/07/21 08:47:24  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.4  1997/07/04 09:24:56  meichel
** Simplified some sizeof() constructs to avoid compiler warnings
**   on the IBM xlC compiler (AIX 3.x).
**
** Revision 1.3  1996/06/20 07:35:52  hewett
** Removed inclusion of system header already included by dcompat.h
** and made sure that dcompat.h is always included (via dicom.h).
**
** Revision 1.2  1996/05/03 10:31:56  hewett
** Move some common system include files out to include/dcompat.h which were
** causing problems due to multiple inclusion on some machines.
**
** Revision 1.1.1.1  1996/03/26 18:38:46  hewett
** Initial Release.
**
**
*/
