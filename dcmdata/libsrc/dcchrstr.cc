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
 *  Author:  Andreas Barth
 *
 *  Purpose: Implementation of class DcmCharString
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:16 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

//
// This implementation does not support 16 bit character sets. Since 8 bit
// character sets are supported by the class DcmByteString the class
// DcmCharString is derived from DcmByteString without any extensions.
// No special implementation is necessary.
//
// If the extension for 16 bit character sets will be implemented this class
// must be derived directly from DcmElement. This class is designed to support
// the value representations (LO, LT, PN, SH, ST, UT). They are a problem because
// their value width (1, 2, .. Bytes) is specified by the element
// SpecificCharacterSet (0008, 0005) and an implementation must support
// different value widths that cannot be derived from the value representation.
//


#include "dcmtk/dcmdata/dcchrstr.h"


DcmCharString::DcmCharString(const DcmTag &tag, const Uint32 len)
  : DcmByteString(tag, len)
{
}

DcmCharString::DcmCharString(const DcmCharString &old)
  : DcmByteString(old)
{
}

DcmCharString::~DcmCharString(void)
{
}


DcmCharString &DcmCharString::operator=(const DcmCharString &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmCharString::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmCharString &, rhs);
  }
  return EC_Normal;
}


OFBool DcmCharString::containsExtendedCharacters(const OFBool /*checkAllStrings*/)
{
    char *c = NULL;
    if (getString(c).good() && c)
    {
        while (*c)
        {
            /* check for 8 bit characters */
            if (OFstatic_cast(unsigned char, *c) > 127)
                return OFTrue;
            ++c;
        }
    }
    return OFFalse;
}


OFBool DcmCharString::isAffectedBySpecificCharacterSet() const
{
    return OFTrue;
}


/*
 * CVS/RCS Log:
 * $Log: dcchrstr.cc,v $
 * Revision 1.15  2010-10-20 16:44:16  joergr
 * Use type cast macros (e.g. OFstatic_cast) where appropriate.
 *
 * Revision 1.14  2010-10-14 13:14:06  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.13  2008-07-17 10:31:31  onken
 * Implemented copyFrom() method for complete DcmObject class hierarchy, which
 * permits setting an instance's value from an existing object. Implemented
 * assignment operator where necessary.
 *
 * Revision 1.12  2006-12-15 14:14:44  joergr
 * Added new method that checks whether a DICOM object or element is affected
 * by SpecificCharacterSet (0008,0005).
 *
 * Revision 1.11  2006/12/13 13:59:49  joergr
 * Added new optional parameter "checkAllStrings" to method containsExtended
 * Characters().
 *
 * Revision 1.10  2006/05/11 08:47:05  joergr
 * Moved checkForNonASCIICharacters() from application to library.
 *
 * Revision 1.9  2005/12/08 15:40:57  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.8  2002/12/06 13:08:18  joergr
 * Made source code formatting more consistent with other modules/files.
 *
 * Revision 1.7  2001/06/01 15:48:59  meichel
 * Updated copyright header
 *
 * Revision 1.6  2000/03/08 16:26:30  meichel
 * Updated copyright header.
 *
 * Revision 1.5  1999/03/31 09:25:17  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
