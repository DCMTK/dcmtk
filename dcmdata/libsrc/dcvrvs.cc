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
 *  Author:  Andrew Hewett
 *
 *  Purpose: class DcmVirtualString
 *  Value Representation VS is defined in Correction Proposal 101
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-03-31 09:26:04 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/Attic/dcvrvs.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrvs.h"
#include "dcdebug.h"


// ********************************


DcmVirtualString::DcmVirtualString(const DcmTag &tag,
			     const Uint32 len)
: DcmCharString(tag, len)
{
    maxLength = 1024;
}


// ********************************


DcmVirtualString::DcmVirtualString( const DcmVirtualString& old )
: DcmCharString(old)
{
}


// ********************************


DcmVirtualString::~DcmVirtualString()
{
}


// ********************************

E_Condition
DcmVirtualString::getOFString(
    OFString & str,
    const unsigned long pos,
    OFBool normalize)
{
    E_Condition l_error = DcmCharString::getOFString(str, pos, normalize);
    // leading/trailing spaces are padding and backslash is separator character
    if (l_error == EC_Normal && normalize)
	normalizeString(str, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}

// ********************************

E_Condition 
DcmVirtualString::getOFStringArray(
    OFString & str,
    OFBool normalize)
{
    E_Condition l_error = DcmCharString::getOFStringArray(str, normalize);
    // leading/trailing spaces are padding and backslash is separator character
    if (l_error == EC_Normal && normalize)
	normalizeString(str, MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


/*
** CVS/RCS Log:
** $Log: dcvrvs.cc,v $
** Revision 1.3  1999-03-31 09:26:04  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.2  1998/11/12 16:48:32  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.1  1998/01/19 13:19:51  hewett
** Initial version.
**
*/

