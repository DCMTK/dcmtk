/*
**
** Author: Gerd Ehlers      01.05.94 
**
** Kuratorium OFFIS e.V.
**
** Module: dcvrst.cc
** 
** Purpose:
** Implementation of class DcmShortText
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-08-29 13:11:49 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrst.cc,v $
** CVS/RCS Revision:	$Revision: 1.6 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "dcvrst.h"
#include "dcdebug.h"


// ********************************


DcmShortText::DcmShortText(const DcmTag &tag, const Uint32 len)
: DcmCharString(tag, len)
{
    maxLength = 1024;
}


// ********************************


DcmShortText::DcmShortText( const DcmShortText& old )
: DcmCharString( old, EVR_ST )
{
    maxLength = 1024;
}


// ********************************


DcmShortText::~DcmShortText()
{
}


// ********************************

E_Condition
DcmShortText::getOFString(
    OFString & str,
    const unsigned long pos,
    OFBool normalize)
{
    E_Condition l_error = DcmCharString::getOFString(str, pos, normalize);
    if (l_error == EC_Normal && normalize)
	normalizeString(str, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}

// ********************************

E_Condition 
DcmShortText::getOFStringArray(
    OFString & str,
    OFBool normalize)
{
    E_Condition l_error = DcmCharString::getOFStringArray(str, normalize);
    if (l_error == EC_Normal && normalize)
	normalizeString(str, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrst.cc,v $
** Revision 1.6  1997-08-29 13:11:49  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.5  1997/08/29 08:33:01  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.4  1997/07/03 15:10:19  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:54  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/

