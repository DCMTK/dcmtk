/*
 *
 *  Copyright (C) 1994-2000, OFFIS
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
 *  Author:  Andrew Hewett
 *
 *  Purpose: 
 *  class DcmUnlimitedText
 *  Value Representation UT is defined in Correction Proposal 101
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:26:53 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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
: DcmCharString(old)
{
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
** Revision 1.4  2000-03-08 16:26:53  meichel
** Updated copyright header.
**
** Revision 1.3  1999/03/31 09:26:04  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.2  1998/11/12 16:48:32  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.1  1998/01/19 13:19:50  hewett
** Initial version.
**
*/

