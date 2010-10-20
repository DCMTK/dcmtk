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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmLongText
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:17 $
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrlt.h"


// ********************************


DcmLongText::DcmLongText(const DcmTag &tag,
                         const Uint32 len)
  : DcmCharString(tag, len)
{
    setMaxLength(10240);
}


DcmLongText::DcmLongText(const DcmLongText& old)
  : DcmCharString(old)
{
}


DcmLongText::~DcmLongText()
{
}


DcmLongText &DcmLongText::operator=(const DcmLongText &obj)
{
    DcmCharString::operator=(obj);
    return *this;
}


OFCondition DcmLongText::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmLongText &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmLongText::ident() const
{
    return EVR_LT;
}


OFCondition DcmLongText::checkValue(const OFString & /*vm*/,
                                    const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmLongText::checkStringValue(strVal);
    return l_error;
}


unsigned long DcmLongText::getVM()
{
    /* value multiplicity is 1 for non-empty string, 0 otherwise */
    return (getRealLength() > 0) ? 1 : 0;
}


// ********************************


OFCondition DcmLongText::getOFString(OFString &stringVal,
                                     const unsigned long /*pos*/,
                                     OFBool normalize)
{
    /* treat backslash as a normal character */
    return getOFStringArray(stringVal, normalize);
}


OFCondition DcmLongText::getOFStringArray(OFString &stringVal,
                                          OFBool normalize)
{
    /* get string value without handling the "\" as a delimiter */
    OFCondition l_error = getStringValue(stringVal);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmLongText::checkStringValue(const OFString &value)
{
    return DcmByteString::checkStringValue(value, "" /* vm */, "lt", 14 /*, maxLength: 10240 characters */);
}


/*
** CVS/RCS Log:
** $Log: dcvrlt.cc,v $
** Revision 1.21  2010-10-20 16:44:17  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.20  2010-10-14 13:14:10  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.19  2010-04-23 14:30:34  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.18  2009-08-03 09:03:00  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.17  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.16  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.15  2005/12/08 15:41:56  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.14  2004/01/16 13:48:20  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.13  2002/12/06 13:05:51  joergr
** Fixed bug in Unlimited Text (UT) class: the backslash character was treated
** as a component separator which is wrong according to the DICOM standard.
** The same bug was found in class Long Text (LT) and Short Text (ST).  Also
** changed the behaviour of the getVM() method; now returns 1 only in case of
** non-empty string values.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.12  2002/04/25 10:31:20  joergr
** Added/modified getOFStringArray() implementation.
**
** Revision 1.11  2001/09/25 17:19:58  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.10  2001/06/01 15:49:18  meichel
** Updated copyright header
**
** Revision 1.9  2000/03/08 16:26:48  meichel
** Updated copyright header.
**
** Revision 1.8  1999/03/31 09:25:54  meichel
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
