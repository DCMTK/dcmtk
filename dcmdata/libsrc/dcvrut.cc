/*
**
** Author: Andrew Hewett    18.12.97 -- First Creation
** Kuratorium OFFIS e.V.
**
** Module: dcvrut.cc
** 
** Purpose:
** Implementation of class DcmUnlimitedText
** Value Representation UT is defined in Correction Proposal 101
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1998-01-19 13:19:50 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrut.cc,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrut.h"
#include "dcdebug.h"


// ********************************


DcmUnlimitedText::DcmUnlimitedText(const DcmTag &tag,
			     const Uint32 len)
: DcmCharString(tag, len)
{
    maxLength = DCM_UndefinedLength;
}


// ********************************


DcmUnlimitedText::DcmUnlimitedText( const DcmUnlimitedText& old )
: DcmCharString( old, EVR_UT )
{
    maxLength = DCM_UndefinedLength;
}


// ********************************


DcmUnlimitedText::~DcmUnlimitedText()
{
}


// ********************************

E_Condition
DcmUnlimitedText::getOFString(
    OFString & str,
    const unsigned long pos,
    OFBool normalize)
{
    E_Condition l_error = DcmCharString::getOFString(str, pos, normalize);
    // leading spaces are significant and backslash is normal character
    if (l_error == EC_Normal && normalize)
	normalizeString(str, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}

// ********************************

E_Condition 
DcmUnlimitedText::getOFStringArray(
    OFString & str,
    OFBool normalize)
{
    E_Condition l_error = DcmCharString::getOFStringArray(str, normalize);
    // leading spaces are significant and backslash is normal character
    if (l_error == EC_Normal && normalize)
	normalizeString(str, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrut.cc,v $
** Revision 1.1  1998-01-19 13:19:50  hewett
** Initial version.
**
**
*/

