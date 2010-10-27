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
 *  Purpose: Implementation of class DcmDecimalString
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-27 09:18:31 $
 *  CVS/RCS Revision: $Revision: 1.28 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstd.h"


#define MAX_DS_LENGTH 16


// ********************************


DcmDecimalString::DcmDecimalString(const DcmTag &tag,
                                   const Uint32 len)
  : DcmByteString(tag, len)
{
    setMaxLength(MAX_DS_LENGTH);
    setNonSignificantChars(" \\");
}


DcmDecimalString::DcmDecimalString(const DcmDecimalString &old)
  : DcmByteString(old)
{
}


DcmDecimalString::~DcmDecimalString()
{
}


DcmDecimalString &DcmDecimalString::operator=(const DcmDecimalString &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmDecimalString::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmDecimalString &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmDecimalString::ident() const
{
    return EVR_DS;
}


OFCondition DcmDecimalString::checkValue(const OFString &vm,
                                         const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmDecimalString::checkStringValue(strVal, vm);
    return l_error;
}


// ********************************


OFCondition DcmDecimalString::getFloat64(Float64 &doubleVal,
                                         const unsigned long pos)
{
    /* get decimal string value */
    OFString str;
    OFCondition l_error = getOFString(str, pos, OFTrue /*normalize*/);
    if (l_error.good())
    {
        OFBool success = OFFalse;
        /* convert string to float value */
        doubleVal = OFStandard::atof(str.c_str(), &success);
        if (!success)
            l_error = EC_CorruptedData;
    }
    return l_error;
}


OFCondition DcmDecimalString::getFloat64Vector(OFVector<Float64> &doubleVals)
{
    /* get stored value */
    char *strVal = NULL;
    OFCondition l_error = getString(strVal);
    /* clear result variable */
    doubleVals.clear();
    if (l_error.good() && (strVal != NULL))
    {
        /* determine number of stored values */
        const unsigned long vm = getVM();
        if (vm > 0)
        {
            char c;
            Float64 doubleVal;
            OFString doubleStr;
            char *p = strVal;
            OFBool success = OFFalse;
            /* avoid memory re-allocations by specifying the expected size */
            doubleVals.reserve(vm);
            /* iterate over the string value and search for delimiters */
            do {
                c = *p;
                if ((c == '\\') || (c == '\0'))
                {
                    /* extract single value and convert it to floating point */
                    doubleStr.assign(strVal, p - strVal);
                    doubleVal = OFStandard::atof(doubleStr.c_str(), &success);
                    if (success)
                    {
                        /* store floating point value in result variable */
                        doubleVals.push_back(doubleVal);
                        strVal = p + 1;
                    }
                    else
                    {
                        l_error = EC_CorruptedData;
                        break;
                    }
                }
                ++p;
            } while (c != '\0');
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmDecimalString::getOFString(OFString &stringVal,
                                          const unsigned long pos,
                                          OFBool normalize)
{
    /* call inherited method */
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos);
    /* normalize string if required */
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmDecimalString::writeXML(STD_NAMESPACE ostream &out,
                                       const size_t flags)
{
    /* XML start tag: <element tag="gggg,eeee" vr="XX" ...> */
    writeXMLStartTag(out, flags);
    /* write element value (if loaded) */
    if (valueLoaded())
    {
        /* get string data (without normalization) */
        char *value = NULL;
        getString(value);
        if (value != NULL)
        {
            /* check whether conversion to XML markup string is required */
            if (OFStandard::checkForMarkupConversion(value))
                OFStandard::convertToMarkupStream(out, value);
            else
                out << value;
        }
    }
    /* XML end tag: </element> */
    writeXMLEndTag(out, flags);
    /* always report success */
    return EC_Normal;
}


// ********************************


OFCondition DcmDecimalString::checkStringValue(const OFString &value,
                                               const OFString &vm)
{
    return DcmByteString::checkStringValue(value, vm, "ds", 6, MAX_DS_LENGTH);
}


/*
** CVS/RCS Log:
** $Log: dcvrds.cc,v $
** Revision 1.28  2010-10-27 09:18:31  joergr
** Added getFloat64Vector() method which allows for retrieving the stored
** floating point values more efficiently (especially when there are many
** values).
**
** Revision 1.27  2010-10-20 16:44:17  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.26  2010-10-14 13:14:10  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.25  2010-04-23 14:30:34  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.24  2010-01-21 15:05:59  joergr
** Switched to new stream variant of method convertToMarkupString() where
** appropriate.
**
** Revision 1.23  2009-08-07 14:35:49  joergr
** Enhanced isEmpty() method by checking whether the data element value consists
** of non-significant characters only.
**
** Revision 1.22  2009-08-03 09:02:59  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.21  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.20  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.19  2006/10/13 10:08:19  joergr
** Enhanced performance of writeXML() for large multi-valued DS elements.
**
** Revision 1.18  2005/12/08 15:41:50  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.17  2004/01/16 13:49:00  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.16  2002/12/06 13:20:49  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.15  2002/06/20 12:06:16  meichel
** Changed toolkit to use OFStandard::atof instead of atof, strtod or
**   sscanf for all string to double conversions that are supposed to
**   be locale independent
**
** Revision 1.14  2002/04/25 10:28:42  joergr
** Removed getOFStringArray() implementation.
**
** Revision 1.13  2001/09/25 17:19:56  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.12  2001/06/01 15:49:15  meichel
** Updated copyright header
**
** Revision 1.11  2000/03/08 16:26:46  meichel
** Updated copyright header.
**
** Revision 1.10  1999/03/31 09:25:49  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.9  1998/11/12 16:48:23  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.8  1997/08/29 13:11:45  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.7  1997/08/29 08:32:57  andreas
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
** Revision 1.6  1997/07/03 15:10:10  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.5  1997/07/03 10:22:45  andreas
** - corrected Bug in DcmDecimalString::getFloat64
**
** Revision 1.4  1997/05/12 07:38:26  andreas
** - new get-Methods for DcmDecimalString: getFloat64 and
**   DcmIntegerString: getSint32
**
** Revision 1.3  1996/01/05 13:27:47  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
