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
 *  Purpose: Basis class for dicom tags.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:26:43 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dctagkey.cc,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include <stdio.h>
#include "dctagkey.h"

/*
 * DcmTagKey member functions
 */

OFString DcmTagKey::toString() const
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
** Revision 1.5  2000-03-08 16:26:43  meichel
** Updated copyright header.
**
** Revision 1.4  2000/02/07 14:45:17  meichel
** Removed const qualifier from DcmTagKey::toString(), avoids warning on Irix.
**
** Revision 1.3  1999/03/31 09:25:42  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.2  1999/03/17 11:08:58  meichel
** added method DcmTagKey::toString()
**
** Revision 1.1  1995/11/23 17:02:55  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
*/
