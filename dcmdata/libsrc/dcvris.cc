/*
**
** Author: Gerd Ehlers      01.05.94 -- First Creation
**         Andreas Barth    05.12.95 -- new Stream class, unique value field
** Kuratorium OFFIS e.V.
**
** Module: dcvris.cc
** 
** Purpose:
** Implementation of class DcmIntegerString
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-08-29 13:11:46 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvris.cc,v $
** CVS/RCS Revision:	$Revision: 1.9 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvris.h"
#include "dcdebug.h"
#include "ofstring.h"


// ********************************


DcmIntegerString::DcmIntegerString(const DcmTag &tag,
				   const Uint32 len)
: DcmByteString(tag, len)
{
    maxLength = 12;
}


// ********************************


DcmIntegerString::DcmIntegerString( const DcmIntegerString& old )
: DcmByteString( old, EVR_IS )
{
    maxLength = 12;
}


// ********************************


DcmIntegerString::~DcmIntegerString()
{
}


// ********************************

E_Condition 
DcmIntegerString::getSint32(
    Sint32 &val,
    const unsigned long pos)
{
    OFString str;
    E_Condition l_error = getOFString(str, pos, OFTrue);
    if (l_error == EC_Normal)
    {
#if SIZEOF_LONG == 8
	if (sscanf(str.c_str(), "%d", &val) != 1)
#else
	if (sscanf(str.c_str(), "%ld", &val) != 1)
#endif
	    l_error = EC_CorruptedData;
    }
    return l_error;
}

// ********************************

E_Condition
DcmIntegerString::getOFString(
    OFString & str,
    const unsigned long pos,
    OFBool normalize)
{
    E_Condition l_error = DcmByteString::getOFString(str, pos, normalize);
    if (l_error == EC_Normal && normalize)
	normalizeString(str, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}

// ********************************

E_Condition 
DcmIntegerString::getOFStringArray(
    OFString & str,
    OFBool normalize)
{
    E_Condition l_error = DcmByteString::getOFStringArray(str, normalize);
    if (l_error == EC_Normal && normalize)
	normalizeString(str, MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


/*
** CVS/RCS Log:
** $Log: dcvris.cc,v $
** Revision 1.9  1997-08-29 13:11:46  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.8  1997/08/29 08:32:58  andreas
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
** Revision 1.7  1997/07/03 15:10:13  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.6  1997/05/30 06:45:00  andreas
** - fixed scanf format problem leading to warnings on 64 bit machines.
**
** Revision 1.5  1997/05/12 09:57:02  andreas
** - warnings deletetd
**
** Revision 1.4  1997/05/12 07:38:27  andreas
** - new get-Methods for DcmDecimalString: getFloat64 and
**   DcmIntegerString: getSint32
**
** Revision 1.3  1996/01/05 13:27:49  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
**
**
*/



