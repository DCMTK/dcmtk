/*
 *
 *  Copyright (C) 1994-2009, OFFIS
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmApplicationEntity
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2009-08-07 14:35:49 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrae.h"


#define MAX_AE_LENGTH 16


// ********************************


DcmApplicationEntity::DcmApplicationEntity(const DcmTag &tag,
                                           const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(MAX_AE_LENGTH);
    setNonSignificantChars(" \\");
}


DcmApplicationEntity::DcmApplicationEntity(const DcmApplicationEntity &old)
  : DcmByteString(old)
{
}


DcmApplicationEntity::~DcmApplicationEntity()
{
}


DcmApplicationEntity &DcmApplicationEntity::operator=(const DcmApplicationEntity &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmApplicationEntity::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = (DcmApplicationEntity&) rhs;
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmApplicationEntity::ident() const
{
    return EVR_AE;
}


// ********************************


OFCondition DcmApplicationEntity::getOFString(OFString &stringVal,
                                              const unsigned long pos,
                                              OFBool normalize)
{
    /* call inherited method */
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos, normalize);
    /* normalize string if required */
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmApplicationEntity::checkValue(const OFString &value,
                                             const OFString &vm)
{
    return DcmByteString::checkValue(value, vm, "ae", 13, MAX_AE_LENGTH);
}


/*
** CVS/RCS Log:
** $Log: dcvrae.cc,v $
** Revision 1.18  2009-08-07 14:35:49  joergr
** Enhanced isEmpty() method by checking whether the data element value consists
** of non-significant characters only.
**
** Revision 1.17  2009-08-03 09:02:59  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.16  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.15  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.14  2005/12/08 15:41:45  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.13  2002/12/06 13:20:48  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.12  2002/04/25 10:26:53  joergr
** Removed getOFStringArray() implementation.
**
** Revision 1.11  2001/09/25 17:19:55  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.10  2001/06/01 15:49:14  meichel
** Updated copyright header
**
** Revision 1.9  2000/03/08 16:26:45  meichel
** Updated copyright header.
**
** Revision 1.8  1999/03/31 09:25:46  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.7  1998/11/12 16:48:21  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.6  1997/08/29 13:11:43  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.5  1997/08/29 08:32:55  andreas
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
** Revision 1.4  1997/07/03 15:10:07  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:45  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
