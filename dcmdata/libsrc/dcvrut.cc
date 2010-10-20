/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmUnlimitedText
 *           Value Representation UT is defined in Correction Proposal 101
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:18 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrut.h"


// ********************************


DcmUnlimitedText::DcmUnlimitedText(const DcmTag &tag,
                                   const Uint32 len)
  : DcmCharString(tag, len)
{
    setMaxLength(DCM_UndefinedLength);
}


DcmUnlimitedText::DcmUnlimitedText(const DcmUnlimitedText &old)
  : DcmCharString(old)
{
}


DcmUnlimitedText::~DcmUnlimitedText()
{
}


DcmUnlimitedText &DcmUnlimitedText::operator=(const DcmUnlimitedText &obj)
{
    DcmCharString::operator=(obj);
    return *this;
}


OFCondition DcmUnlimitedText::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmUnlimitedText &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmUnlimitedText::ident() const
{
    return EVR_UT;
}


OFCondition DcmUnlimitedText::checkValue(const OFString & /*vm*/,
                                         const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmUnlimitedText::checkStringValue(strVal);
    return l_error;
}


unsigned long DcmUnlimitedText::getVM()
{
    /* value multiplicity is 1 for non-empty string, 0 otherwise */
    return (getRealLength() > 0) ? 1 : 0;
}


// ********************************


OFCondition DcmUnlimitedText::getOFString(OFString &strValue,
                                          const unsigned long /*pos*/,
                                          OFBool normalize)
{
    /* treat backslash as a normal character */
    return getOFStringArray(strValue, normalize);
}


OFCondition DcmUnlimitedText::getOFStringArray(OFString &strValue,
                                               OFBool normalize)
{
    /* get string value without handling the "\" as a delimiter */
    OFCondition l_error = getStringValue(strValue);
    // leading spaces are significant and backslash is normal character
    if (l_error.good() && normalize)
        normalizeString(strValue, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmUnlimitedText::checkStringValue(const OFString &value)
{
    return DcmByteString::checkStringValue(value, "" /* vm */, "lt", 14 /*, maxLength: 4294967295 characters */);
}


/*
** CVS/RCS Log:
** $Log: dcvrut.cc,v $
** Revision 1.16  2010-10-20 16:44:18  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.15  2010-10-14 13:14:11  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.14  2010-04-23 14:30:35  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.13  2009-08-03 09:03:00  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.12  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.11  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.10  2005/12/08 15:42:10  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.9  2004/01/16 13:45:06  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.8  2002/12/06 13:01:52  joergr
** Fixed bug in Unlimited Text (UT) class: the backslash character was treated
** as a component separator which is wrong according to the DICOM standard.
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.7  2002/04/25 10:35:29  joergr
** Added/modified getOFStringArray() implementation.
**
** Revision 1.6  2001/09/25 17:20:03  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.5  2001/06/01 15:49:22  meichel
** Updated copyright header
**
** Revision 1.4  2000/03/08 16:26:53  meichel
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
