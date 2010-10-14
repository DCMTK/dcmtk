/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Basis class for dicom tags.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:09 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctagkey.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

/*
 * DcmTagKey member functions
 */

OFString DcmTagKey::toString() const
{
    char tagBuf[16];

    if (group == 0xffff && element == 0xffff)
    {
        strcpy(tagBuf, "(\?\?\?\?,\?\?\?\?)"); // prevent trigraph expansion in string constant
    } else {
        sprintf(tagBuf, "(%04x,%04x)", OFstatic_cast(unsigned, group), OFstatic_cast(unsigned, element));
    }
    return tagBuf;
}


OFBool DcmTagKey::isSignableTag() const
{
  //no group length tags (element number of 0000)
  if (element == 0) return OFFalse;

  // no Length to End Tag
  if ((group == 0x0008)&&(element==0x0001)) return OFFalse;

  //no tags with group number less than 0008
  if (group < 8) return OFFalse;

  //no tags from group FFFA (digital signatures sequence)
  if (group == 0xfffa) return OFFalse;

  // no MAC Parameters sequence
  if ((group == 0x4ffe)&&(element==0x0001)) return OFFalse;

  //no Data Set trailing Padding
  if ((group == 0xfffc)&&(element==0xfffc)) return OFFalse;

  //no Sequence or Item Delimitation Tag
  if ((group == 0xfffe)&&((element==0xe00d)||(element==0xe0dd))) return OFFalse;

  return OFTrue;
}

/*
** DcmTagKey friend functions
*/

STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& s, const DcmTagKey& k)
{
    s << k.toString();
    return s;
}


/*
** CVS/RCS Log:
** $Log: dctagkey.cc,v $
** Revision 1.16  2010-10-14 13:14:09  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.15  2010-06-25 09:15:19  uli
** Fixed issues with compiling with HAVE_STD_STRING.
**
** Revision 1.14  2009-11-04 09:58:10  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.13  2006-08-15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.12  2005/12/08 15:41:40  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.11  2004/02/04 16:46:20  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
**
** Revision 1.10  2002/11/27 12:06:53  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.9  2001/11/19 15:23:29  meichel
** Cleaned up signature code to avoid some gcc warnings.
**
** Revision 1.8  2001/11/02 13:18:52  meichel
** Removed character sequences that could be interpreted as ISO C++ trigraphs
**
** Revision 1.7  2001/06/01 15:49:11  meichel
** Updated copyright header
**
** Revision 1.6  2000/11/07 16:56:23  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.5  2000/03/08 16:26:43  meichel
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
