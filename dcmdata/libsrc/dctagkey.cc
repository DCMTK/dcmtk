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
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1995-11-23 17:02:55 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dctagkey.cc,v $
** CVS/RCS Revision:	$Revision: 1.1 $
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



/*
** DcmTagKey friend functions
*/

ostream& operator<<(ostream& s, const DcmTagKey& k) 
{

    const DcmTagKey *kp = &k;
    if (kp == NULL) {
        s << "(????,????)";
        return s;
    }

    char tagBuf[16];

    sprintf(tagBuf, "(%04x,%04x)", k.getGroup(), k.getElement());
    s << tagBuf;

    return s;
}


/*
** CVS/RCS Log:
** $Log: dctagkey.cc,v $
** Revision 1.1  1995-11-23 17:02:55  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
**
*/
