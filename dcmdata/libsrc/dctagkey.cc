/*
**
** Author: Andrew Hewett	Created: 4.11.95
** Kuratorium OFFIS e.V.
**
** Module: dctagkey.h
**
** Purpose:
** Basis class for dicom tags.  
** 
**
** Last Update:		$Author: meichel $
** Update Date:		$Date: 1999-03-17 11:08:58 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dctagkey.cc,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>

#include "dctagkey.h"

/*
 * DcmTagKey member functions
 */

const OFString 
DcmTagKey::toString() const
{
    char tagBuf[16];

    if (group == 0xffff && element == 0xffff) {
        strcpy(tagBuf, "(????,????)");
    } else {
        sprintf(tagBuf, "(%04x,%04x)", (unsigned)group, (unsigned)element);
    }
    return tagBuf;
}


/*
** DcmTagKey friend functions
*/

ostream& operator<<(ostream& s, const DcmTagKey& k) 
{
    s << k.toString();
    return s;
}


/*
** CVS/RCS Log:
** $Log: dctagkey.cc,v $
** Revision 1.2  1999-03-17 11:08:58  meichel
** added method DcmTagKey::toString()
**
** Revision 1.1  1995/11/23 17:02:55  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
**
*/
