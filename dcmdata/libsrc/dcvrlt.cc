/*
 *
 *  Copyright (C) 1994-99, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: class DcmLongText
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:25:54 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrlt.cc,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrlt.h"
#include "dcdebug.h"


// ********************************


DcmLongText::DcmLongText(const DcmTag &tag,
                         const Uint32 len)
: DcmCharString(tag, len)
{
    maxLength = 10240;
}


// ********************************


DcmLongText::DcmLongText(const DcmLongText& old)
: DcmCharString(old)
{
}


// ********************************


DcmLongText::~DcmLongText()
{
}


// ********************************

E_Condition
DcmLongText::getOFString(
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
DcmLongText::getOFStringArray(
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
** $Log: dcvrlt.cc,v $
** Revision 1.8  1999-03-31 09:25:54  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.7  1998/11/12 16:48:26  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.6  1997/08/29 13:11:47  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.5  1997/08/29 08:32:59  andreas
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
** Revision 1.4  1997/07/03 15:10:15  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:50  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
