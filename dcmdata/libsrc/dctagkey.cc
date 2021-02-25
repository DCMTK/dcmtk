/*
 *
 *  Copyright (C) 1994-2021, OFFIS e.V.
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
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/ofstd/ofstd.h"

/*
 * DcmTagKey member functions
 */

DcmTagKey DcmTagKey::getBaseTag() const
{
    // make sure that the group number is even (i.e. a standard tag).  We also
    // accept more than 16 repeating groups because of known real-world examples.
    const Uint16 baseGroup = group & 0xFF01;
    // check for curve or overlay repeating group
    if ((baseGroup == 0x5000) || (baseGroup == 0x6000))
        return DcmTagKey(baseGroup, element);
    else
        return *this;
}


OFString DcmTagKey::toString() const
{
    char tagBuf[16];

    if (group == 0xffff && element == 0xffff)
    {
        OFStandard::strlcpy(tagBuf, "(\?\?\?\?,\?\?\?\?)", 16); // prevent trigraph expansion in string constant
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
