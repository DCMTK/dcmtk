/*
**
** Author: Gerd Ehlers	    01.05.94 -- Created
**         Andreas Barth	04.12.95 -- New Stream 
** Kuratorium OFFIS e.V.
**
** Module: dcvrpn.cc
** 
** Purpose:
** Implementation of class DcmPersonName
**
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1998-11-12 16:48:27 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrpn.cc,v $
** CVS/RCS Revision:	$Revision: 1.7 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "dcvrpn.h"
#include "dcdebug.h"


// ********************************


DcmPersonName::DcmPersonName(const DcmTag &tag, const Uint32 len)
: DcmCharString(tag, len)
{
    maxLength = 64;
}


// ********************************


DcmPersonName::DcmPersonName(const DcmPersonName& old)
: DcmCharString(old)
{
}


// ********************************


DcmPersonName::~DcmPersonName(void)
{
}


// ********************************

E_Condition
DcmPersonName::getOFString(
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
DcmPersonName::getOFStringArray(
    OFString & str,
    OFBool normalize)
{
    E_Condition l_error = DcmCharString::getOFStringArray(str, normalize);
    if (l_error == EC_Normal && normalize)
	normalizeString(str, MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvrpn.cc,v $
** Revision 1.7  1998-11-12 16:48:27  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.6  1997/08/29 13:11:48  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.5  1997/08/29 08:33:00  andreas
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
** Revision 1.4  1997/07/03 15:10:16  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:52  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
*/
