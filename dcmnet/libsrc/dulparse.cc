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
** Module Name(s):	parseAssociate
**			parseDebug
** Author, Date:	Stephen M. Moore, 15-Apr-93
** Intent:		This file contains functions for parsing Dicom
**			Upper Layer (DUL) Protocol Data Units (PDUs)
**			into logical in-memory structures.
** Last Update:		$Author: joergr $, $Date: 2001-06-05 10:07:10 $
** Source File:		$RCSfile: dulparse.cc,v $
** Revision:		$Revision: 1.15 $
** Status:		$State: Exp $
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#include <stdio.h>
#include <string.h>

#include "dicom.h"
#include "cond.h"
#include "lst.h"
#include "dul.h"
#include "dulstruc.h"
#include "dulpriv.h"
#include "ofconsol.h"

static CONDITION
parseSubItem(DUL_SUBITEM * subItem, unsigned char *buf,
	     unsigned long *itemLength);
static CONDITION
parsePresentationContext(unsigned char type,
			 PRV_PRESENTATIONCONTEXTITEM * context,
			 unsigned char *buf, unsigned long *itemLength);
static CONDITION
parseUserInfo(DUL_USERINFO * userInfo,
	      unsigned char *buf, unsigned long *itemLength);
static CONDITION
parseMaxPDU(DUL_MAXLENGTH * max, unsigned char *buf,
	    unsigned long *itemLength);
static CONDITION
    parseDummy(unsigned char *buf, unsigned long *itemLength);
static CONDITION
parseSCUSCPRole(PRV_SCUSCPROLE * role, unsigned char *buf,
		unsigned long *length);
static void trim_trailing_spaces(char *s);

static CONDITION
parseExtNeg(SOPClassExtendedNegotiationSubItem* extNeg, unsigned char *buf,
	    unsigned long *length);

#ifdef DEBUG
static OFBool debug = OFFalse;
#endif

/* parseAssociate
**
** Purpose:
**	Parse the buffer (read from the socket) and extract an Associate
**	PDU from it.
**
** Parameter Dictionary:
**	buf		Buffer holding the PDU in the stream format
**	pduLength	Length of the buffer
**	assoc		The Associate PDU to be extracted
**			(returend to the caller)
**
** Return Values:
**
**	DUL_ILLEGALPDU
**	DUL_LISTCREATEFAILED
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

CONDITION
parseAssociate(unsigned char *buf, unsigned long pduLength,
	       PRV_ASSOCIATEPDU * assoc)
{
    CONDITION
	cond;
    unsigned char
        type;
    unsigned long
        itemLength;
    PRV_PRESENTATIONCONTEXTITEM
	* context;

    (void) memset(assoc, 0, sizeof(*assoc));
    if ((assoc->presentationContextList = LST_Create()) == NULL)
	return COND_PushCondition(DUL_LISTCREATEFAILED,
		       DUL_Message(DUL_LISTCREATEFAILED), "parseAssociate");
    if ((assoc->userInfo.SCUSCPRoleList = LST_Create()) == NULL)
	return COND_PushCondition(DUL_LISTCREATEFAILED,
		       DUL_Message(DUL_LISTCREATEFAILED), "parseAssociate");

    assoc->type = *buf++;
    assoc->rsv1 = *buf++;
    EXTRACT_LONG_BIG(buf, assoc->length);
    buf += 4;

    EXTRACT_SHORT_BIG(buf, assoc->protocol);
    buf += 2;
    pduLength -= 2;
    if ((assoc->protocol & DUL_PROTOCOL) == 0)
	return COND_PushCondition(DUL_UNSUPPORTEDPEERPROTOCOL,
				  DUL_Message(DUL_UNSUPPORTEDPEERPROTOCOL), assoc->protocol, DUL_PROTOCOL,
				  "parseAssociate");
    assoc->rsv2[0] = *buf++;
    pduLength--;
    assoc->rsv2[1] = *buf++;
    pduLength--;
    (void) strncpy(assoc->calledAPTitle, (char *) buf, 16);
    trim_trailing_spaces(assoc->calledAPTitle);

    buf += 16;
    pduLength -= 16;
    (void) strncpy(assoc->callingAPTitle, (char *) buf, 16);
    trim_trailing_spaces(assoc->callingAPTitle);
    buf += 16;
    pduLength -= 16;
    (void) memcpy(assoc->rsv3, buf, 32);
    buf += 32;
    pduLength -= 32;

#ifdef DEBUG
    if (debug) {
	const char *s;
	DEBUG_DEVICE << "Parsing an A-ASSOCIATE PDU" << endl;
	if (assoc->type == DUL_TYPEASSOCIATERQ)
	    s = "A-ASSOCIATE RQ";
	else if (assoc->type == DUL_TYPEASSOCIATEAC)
	    s = "A-ASSOCIATE AC";
	else
	    s = "Unknown: Programming bug in parseAssociate";

/*	If we hit the "Unknown type", there is a programming bug somewhere.
**	This function is only supposed to parse A-ASSOCIATE PDUs and
**	expects its input to have been properly screened.
*/
	DEBUG_DEVICE << "PDU type: " << hex << ((unsigned int)assoc->type) << dec
        << " (" << s << "), PDU Length: " << assoc->length << endl
	    << "DICOM Protocol: " << hex << assoc->protocol << dec << endl
	    << "Called AP Title:  " << assoc->calledAPTitle << endl
	    << "Calling AP Title: " << assoc->callingAPTitle << endl;
    }
#endif
    cond = DUL_NORMAL;
    while ((cond == DUL_NORMAL) && (pduLength > 0)) {
	type = *buf;
#ifdef DEBUG
	if (debug) {
	    DEBUG_DEVICE << "Parsing remaining " << pduLength
            << " bytes of A-ASSOCIATE PDU" << endl
            << "Next item type: ";
        DEBUG_DEVICE.width(2); DEBUG_DEVICE.fill('0');
        DEBUG_DEVICE << hex << ((unsigned int)type) << dec << endl;
	}
#endif
	switch (type) {
	case DUL_TYPEAPPLICATIONCONTEXT:
	    cond = parseSubItem(&assoc->applicationContext,
				buf, &itemLength);
	    if (cond == DUL_NORMAL) {
		buf += itemLength;
		pduLength -= itemLength;
#ifdef DEBUG
		if (debug)
		    COUT << "Successfully parsed Application Context" << endl;
#endif
	    }
	    break;
	case DUL_TYPEPRESENTATIONCONTEXTRQ:
	case DUL_TYPEPRESENTATIONCONTEXTAC:
	    context = (PRV_PRESENTATIONCONTEXTITEM*)malloc(sizeof(PRV_PRESENTATIONCONTEXTITEM));
	    if (context == NULL)
		return COND_PushCondition(DUL_MALLOCERROR,
			     DUL_Message(DUL_MALLOCERROR), "parseAssociate",
					  sizeof(*context));
	    (void) memset(context, 0, sizeof(*context));
	    cond = parsePresentationContext(type, context, buf, &itemLength);
	    if (cond != DUL_NORMAL)
		return cond;
	    buf += itemLength;
	    pduLength -= itemLength;
	    if (LST_Enqueue(&assoc->presentationContextList, (LST_NODE*)context) !=
		LST_NORMAL) {
		return COND_PushCondition(DUL_LISTERROR,
			      DUL_Message(DUL_LISTERROR), "parseAssociate");
	    }
#ifdef DEBUG
	    if (debug)
		    DEBUG_DEVICE << "Successfully parsed Presentation Context " << endl;
#endif
	    break;
	case DUL_TYPEUSERINFO:
	    cond = parseUserInfo(&assoc->userInfo,
				 buf, &itemLength);
	    if (cond != DUL_NORMAL)
		return cond;
	    buf += itemLength;
	    pduLength -= itemLength;
#ifdef DEBUG
	    if (debug)
		    DEBUG_DEVICE << "Successfully parsed User Information" << endl;
#endif
	    break;
	default:
	    cond = parseDummy(buf, &itemLength);
	    buf += itemLength;
	    pduLength -= itemLength;
	    break;
	}
    }
    return cond;
}

/* parseDebug
**
** Purpose:
**	Set global 'debug' flag for this module (OFTrue or OFFalse).
**
** Parameter Dictionary:
**	flag	OFBool flag to be copied to global 'debug'
**
** Return Values:
**	None
**
** Algorithm:
*/

#ifdef DEBUG
void
parseDebug(OFBool flag)
{
    debug = flag;
}
#else
void
parseDebug(OFBool /*flag*/)
{
}
#endif

/* ============================================================
**  Private functions (to this module) defined below.
*/

/* parseSubItem
**
** Purpose:
**	Parse the buffer and extract the subitem structure
**
** Parameter Dictionary:
**	subItem		The subitem structure to be extracted
**	buf		Buffer to be parsed
**	itemLength	Length of the subitem extracted
**
** Return Values:
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
parseSubItem(DUL_SUBITEM * subItem, unsigned char *buf,
	     unsigned long *itemLength)
{
    subItem->type = *buf++;
    subItem->rsv1 = *buf++;
    EXTRACT_SHORT_BIG(buf, subItem->length);
    buf += 2;
    (void) memcpy(subItem->data, buf, subItem->length);
    subItem->data[subItem->length] = '\0';

    *itemLength = 2 + 2 + subItem->length;

#ifdef DEBUG
    if (debug) {
        DEBUG_DEVICE.width(2); DEBUG_DEVICE.fill('0');
        DEBUG_DEVICE << "Subitem parse: Type " << hex << ((unsigned int)subItem->type) << dec << ", Length ";
        DEBUG_DEVICE.width(4);
        DEBUG_DEVICE << (int)subItem->length << ", Content: " << subItem->data << endl;
    }
#endif
    return DUL_NORMAL;
}


/* parsePresentationContext
**
** Purpose:
**	Parse the buffer and extract the presentation context.
**
** Parameter Dictionary:
**	context		The presentation context that is to be extracted
**	buf		The buffer to be parsed
**	itemLength	Total length of the presentation context that is
**			extracted
**
** Return Values:
**
**	DUL_ILLEGALPDU
**	DUL_LISTCREATEFAILED
**	DUL_LISTERROR
**	DUL_MALLOCERROR
**	DUL_NORMAL
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
parsePresentationContext(unsigned char type,
		  PRV_PRESENTATIONCONTEXTITEM * context, unsigned char *buf,
			 unsigned long *itemLength)
{
    unsigned long
        length;
    unsigned long
        presentationLength;
    CONDITION
	cond;
    DUL_SUBITEM
	* subItem;

    if ((context->transferSyntaxList = LST_Create()) == NULL)
	return COND_PushCondition(DUL_LISTCREATEFAILED,
	     DUL_Message(DUL_LISTCREATEFAILED), "parsePresentationContext");

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

#ifdef DEBUG
    if (debug) {
        DEBUG_DEVICE.width(2);
	DEBUG_DEVICE << "Parsing Presentation Context: ("
            << hex << (unsigned int)context->type << dec
            << "), Length: " << (unsigned long)context->length << endl
            << "Presentation Context ID: ";
        DEBUG_DEVICE.width(2);
        DEBUG_DEVICE << hex << (unsigned int)context->contextID << dec << endl;
    }
#endif
    presentationLength = length - 4;
    if (!((type == DUL_TYPEPRESENTATIONCONTEXTAC) &&
	  (context->result != DUL_PRESENTATION_ACCEPT))) {
	while (presentationLength > 0) {
#ifdef DEBUG
	    if (debug) {
              DEBUG_DEVICE << "Parsing remaining " << presentationLength
                << " bytes of Presentation Ctx" << endl;
              DEBUG_DEVICE.width(2); DEBUG_DEVICE.fill('0');
	      DEBUG_DEVICE << "Next item type: " << hex << (unsigned int)*buf << dec << endl;
	    }
#endif
	    switch (*buf) {
	    case DUL_TYPEABSTRACTSYNTAX:
		cond = parseSubItem(&context->abstractSyntax, buf, &length);
		if (cond != DUL_NORMAL)
		    return cond;

		buf += length;
		presentationLength -= length;
#ifdef DEBUG
		if (debug) {
		    DEBUG_DEVICE << "Successfully parsed Abstract Syntax" << endl;
		}
#endif
		break;
	    case DUL_TYPETRANSFERSYNTAX:
		subItem = (DUL_SUBITEM*)malloc(sizeof(DUL_SUBITEM));
		if (subItem == NULL)
		    return COND_PushCondition(DUL_MALLOCERROR,
					      DUL_Message(DUL_MALLOCERROR),
			      "parsePresentationContext", sizeof(*subItem));

		cond = parseSubItem(subItem, buf, &length);
		if (cond != DUL_NORMAL)
		    return cond;
		if (LST_Enqueue(&context->transferSyntaxList, (LST_NODE*)subItem) !=
		    LST_NORMAL)
		    return COND_PushCondition(DUL_LISTERROR,
		    DUL_Message(DUL_LISTERROR), "parsePresentationContext");

		buf += length;
		presentationLength -= length;
#ifdef DEBUG
		if (debug) {
		    DEBUG_DEVICE << "Successfully parsed Transfer Syntax" << endl;
		}
#endif
		break;
	    default:
		cond = parseDummy(buf, &length);
		buf += length;
		presentationLength -= length;
		break;
	    }
	}
    }
    return DUL_NORMAL;
}



/* parseUserInfo
**
** Purpose:
**	Parse the buffer and extract the user info structure
**
** Parameter Dictionary:
**	userInfo	Structure to hold the extracted user info information
**	buf		The buffer that is to be parsed
**	itemLength	Length of structure extracted.
**
** Return Values:
**
**	DUL_ILLEGALPDU
**	DUL_MALLOCERROR
**	DUL_NORMAL
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
*/

static CONDITION
parseUserInfo(DUL_USERINFO * userInfo,
	      unsigned char *buf, unsigned long *itemLength)
{
    unsigned short userLength;
    unsigned long length;
    CONDITION cond;
    PRV_SCUSCPROLE *role;
    SOPClassExtendedNegotiationSubItem *extNeg = NULL;

    userInfo->type = *buf++;
    userInfo->rsv1 = *buf++;
    EXTRACT_SHORT_BIG(buf, userInfo->length);
    buf += 2;

    userLength = userInfo->length;
    *itemLength = userLength + 4;

#ifdef DEBUG
    if (debug) {
        DEBUG_DEVICE.width(2); DEBUG_DEVICE.fill('0');
	DEBUG_DEVICE << "Parsing user info field ("
            << hex << (unsigned int)userInfo->type << dec << "), Length: "
            << (unsigned long)userInfo->length << endl;
    }
#endif
    while (userLength > 0) {
#ifdef DEBUG
	if (debug) {
	    DEBUG_DEVICE << "Parsing remaining " << (long)userLength
            << " bytes of User Information" << endl;
            DEBUG_DEVICE.width(2); DEBUG_DEVICE.fill('0');
	    DEBUG_DEVICE << "Next item type: " << hex << (unsigned int)*buf << dec << endl;
	}
#endif
	switch (*buf) {
	case DUL_TYPEMAXLENGTH:
	    cond = parseMaxPDU(&userInfo->maxLength, buf, &length);
	    if (cond != DUL_NORMAL)
		return cond;
	    buf += length;
	    userLength -= (unsigned short) length;
#ifdef DEBUG
	    if (debug) {
		    DEBUG_DEVICE << "Successfully parsed Maximum PDU Length" << endl;
	    }
#endif
	    break;
	case DUL_TYPEIMPLEMENTATIONCLASSUID:
	    cond = parseSubItem(&userInfo->implementationClassUID,
				buf, &length);
	    if (cond != DUL_NORMAL)
		return cond;
	    buf += length;
	    userLength -= (unsigned short) length;
	    break;

	case DUL_TYPEASYNCOPERATIONS:
	    cond = parseDummy(buf, &length);
	    buf += length;
	    userLength -= (unsigned short) length;
	    break;
	case DUL_TYPESCUSCPROLE:
	    role = (PRV_SCUSCPROLE*)malloc(sizeof(PRV_SCUSCPROLE));
	    if (role == NULL)
		return COND_PushCondition(DUL_MALLOCERROR,
					  DUL_Message(DUL_MALLOCERROR),
					  "parseUserInfo", sizeof(*role));
	    cond = parseSCUSCPRole(role, buf, &length);
	    if (cond != DUL_NORMAL)
		return cond;
	    cond = LST_Enqueue(&userInfo->SCUSCPRoleList, (LST_NODE*)role);
	    if (cond != LST_NORMAL)
		return COND_PushCondition(DUL_LISTERROR,
			       DUL_Message(DUL_LISTERROR), "parseUserInfo");
	    buf += length;
	    userLength -= (unsigned short) length;
	    break;
	case DUL_TYPEIMPLEMENTATIONVERSIONNAME:
	    cond = parseSubItem(&userInfo->implementationVersionName,
				buf, &length);
	    if (cond != DUL_NORMAL)
		return cond;
	    buf += length;
	    userLength -= (unsigned short) length;
	    break;

        case DUL_TYPESOPCLASSEXTENDEDNEGOTIATION: 
            /* parse an extended negotiation sub-item */
            extNeg = new SOPClassExtendedNegotiationSubItem;
            if (extNeg == NULL) return COND_PushCondition(DUL_MALLOCERROR, 
               DUL_Message(DUL_MALLOCERROR), "parseUserInfo", sizeof(*extNeg));
	    cond = parseExtNeg(extNeg, buf, &length);
	    if (cond != DUL_NORMAL) return cond;
            if (userInfo->extNegList == NULL)
            {
                userInfo->extNegList = new SOPClassExtendedNegotiationSubItemList;
                if (userInfo->extNegList == NULL) return COND_PushCondition(DUL_MALLOCERROR,
		  DUL_Message(DUL_MALLOCERROR), "parseUserInfo", sizeof(*(userInfo->extNegList)));
            }
            userInfo->extNegList->push_back(extNeg);
	    buf += length;
	    userLength -= (unsigned short) length;
	    break;

	default:
	    cond = parseDummy(buf, &length);
	    buf += length;
	    userLength -= (unsigned short) length;
/*	    return COND_PushCondition(DUL_ILLEGALPDU,
				      DUL_Message(DUL_ILLEGALPDU), 0xfff);
*/
	    break;
	}
    }

    return DUL_NORMAL;
}



/* parseMaxPDU
**
** Purpose:
**	Parse the buffer and extract the Max PDU structure.
**
** Parameter Dictionary:
**	max		The structure to hold the Max PDU
**	buf		The buffer that is to be parsed
**	itemLength	Length of structure extracted.
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
parseMaxPDU(DUL_MAXLENGTH * max, unsigned char *buf,
	    unsigned long *itemLength)
{
    max->type = *buf++;
    max->rsv1 = *buf++;
    EXTRACT_SHORT_BIG(buf, max->length);
    buf += 2;
    EXTRACT_LONG_BIG(buf, max->maxLength);
    *itemLength = 2 + 2 + max->length;

#ifdef DEBUG
    if (debug) {
	    DEBUG_DEVICE << "Maximum PDU Length: " << (unsigned long)max->maxLength << endl;
    }
#endif

    return DUL_NORMAL;
}

/* parseDummy
**
** Purpose:
**	Parse the buffer to extract just a dummy structure of length
**	User Length
**
** Parameter Dictionary:
**	buf		The buffer that is to be parsed
**	itemLength	Length of structure extracted.
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
parseDummy(unsigned char *buf, unsigned long *itemLength)
{
    unsigned short
        userLength;

    buf++;
    buf++;
    EXTRACT_SHORT_BIG(buf, userLength);
    buf += 2;

    *itemLength = userLength + 4;
    return DUL_NORMAL;
}

/* parseSCUSCPRole
**
** Purpose:
**	Parse the buffer and extract the SCU-SCP role list
**
** Parameter Dictionary:
**	role		The structure to hold the SCU-SCP role list
**	buf		The buffer that is to be parsed
**	itemLength	Length of structure extracted.
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
parseSCUSCPRole(PRV_SCUSCPROLE * role, unsigned char *buf,
		unsigned long *length)
{
    unsigned short
        UIDLength;

    role->type = *buf++;
    role->rsv1 = *buf++;
    EXTRACT_SHORT_BIG(buf, role->length);
    buf += 2;

    EXTRACT_SHORT_BIG(buf, UIDLength);
    buf += 2;

    (void) memcpy(role->SOPClassUID, buf, UIDLength);
    role->SOPClassUID[UIDLength] = '\0';
    buf += UIDLength;
    role->SCURole = *buf++;
    role->SCPRole = *buf++;

    *length = 2 + 2 + role->length;

#ifdef DEBUG
    if (debug) {
      DEBUG_DEVICE.width(2); DEBUG_DEVICE.fill('0');
      DEBUG_DEVICE << "Subitem parse: Type "
        << hex << (unsigned int)role->type << dec << ", Length ";
      DEBUG_DEVICE.width(4);
      DEBUG_DEVICE << (int)role->length << ", Content: " << role->SOPClassUID
        << " " << (int)role->SCURole << " " << (int)role->SCPRole << endl;
    }
#endif
    return DUL_NORMAL;
}

/* parseExtNeg
**
** Purpose:
**	Parse the buffer and extract the extended negotiation item
**
** Return Values:
**	DUL_NORMAL
**
*/

static CONDITION
parseExtNeg(SOPClassExtendedNegotiationSubItem* extNeg, unsigned char *buf,
		unsigned long *length)
{
    unsigned char *bufStart = buf;
    extNeg->itemType = *buf++;
    extNeg->reserved1 = *buf++;
    EXTRACT_SHORT_BIG(buf, extNeg->itemLength);
    buf += 2;

    EXTRACT_SHORT_BIG(buf, extNeg->sopClassUIDLength);
    buf += 2;

    extNeg->sopClassUID.append((const char*)buf, extNeg->sopClassUIDLength);
    buf += extNeg->sopClassUIDLength;

    *length = 2 + 2 + extNeg->itemLength;

    int remain = (int)(*length - (buf - bufStart));

    extNeg->serviceClassAppInfoLength = remain;
    extNeg->serviceClassAppInfo = new unsigned char[remain];
    for (int i=0; i<remain; i++) {
        extNeg->serviceClassAppInfo[i] = *buf++;
    }

#ifdef DEBUG
    if (debug) {
        DEBUG_DEVICE.width(2); DEBUG_DEVICE.fill('0');
	DEBUG_DEVICE << "ExtNeg Subitem parse: Type "
            << hex << (unsigned int)extNeg->itemType << dec << ", Length ";
        DEBUG_DEVICE.width(4);
        DEBUG_DEVICE << (int)extNeg->itemLength << ", SOP Class: "
            << extNeg->sopClassUID.c_str() << endl;

	DEBUG_DEVICE << "   values: ";
        for (int j=0; j<extNeg->serviceClassAppInfoLength; j++) {
            DEBUG_DEVICE.width(2); DEBUG_DEVICE.fill('0');
    	    DEBUG_DEVICE << hex << extNeg->serviceClassAppInfo[j] << dec << " ";
        }
	DEBUG_DEVICE << endl;
    }
#endif

    return DUL_NORMAL;
}

/* trim_trailing_spaces
**
** Purpose:
**	trim trailing spaces
**
** Parameter Dictionary:
**	s	The character string from which the trailing spaces are to be
**		removed.
**
** Return Values:
**	None
**
** Notes:
**
** Algorithm:
**	Description of the algorithm (optional) and any other notes.
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
** Revision 1.15  2001-06-05 10:07:10  joergr
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
