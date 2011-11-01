/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
 *  Update Date:      $Date: 2011-11-01 14:54:04 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcspchrs.h"   /* for class DcmSpecificCharacterSet */

//
// This implementation does not support 16 bit character sets. Since 8 bit
// character sets are supported by the class DcmByteString the class
// DcmCharString is derived from DcmByteString without any extensions.
// No special implementation is necessary.
//
// If the extension for > 8 bit character sets will be implemented this class
// must be derived directly from DcmElement. This class is designed to support
// the value representations (LO, LT, PN, SH, ST, UT). They are a problem because
// their value width (1, 2, .. Bytes) is specified by the element
// SpecificCharacterSet (0008, 0005) and an implementation must support
// different value widths that cannot be derived from the value representation.
//


#include "dcmtk/dcmdata/dcchrstr.h"


DcmCharString::DcmCharString(const DcmTag &tag, const Uint32 len)
  : DcmByteString(tag, len),
    delimiterChars()
{
}

DcmCharString::DcmCharString(const DcmCharString &old)
  : DcmByteString(old),
    delimiterChars(old.delimiterChars)
{
}

DcmCharString::~DcmCharString(void)
{
}


DcmCharString &DcmCharString::operator=(const DcmCharString &obj)
{
    if (this != &obj)
    {
        DcmByteString::operator=(obj);

        /* copy member variables */
        delimiterChars = obj.delimiterChars;
    }
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


// ********************************


OFCondition DcmCharString::verify(const OFBool autocorrect)
{
    const Uint32 maxLen = getMaxLength();
    char *str = NULL;
    Uint32 len = 0;
    /* get string data */
    errorFlag = getString(str, len);
    /* check for non-empty string */
    if ((str != NULL) && (len > 0))
    {
        /* check whether there is anything to verify at all */
        if (maxLen != DCM_UndefinedLength)
        {
            /* TODO: is it really a good idea to create a copy of the string? */
            OFString value(str, len);
            size_t posStart = 0;
            unsigned long vmNum = 0;
            /* check all string components */
            while (posStart != OFString_npos)
            {
                ++vmNum;
                /* search for next component separator */
                const size_t posEnd = value.find('\\', posStart);
                const size_t fieldLen = (posEnd == OFString_npos) ? value.length() - posStart : posEnd - posStart;
                /* check size limit for each string component */
                if (fieldLen > maxLen)
                {
                    DCMDATA_DEBUG("DcmCharString::verify() maximum length violated in element "
                        << getTagName() << " " << getTag() << " value " << vmNum << ": "
                        << fieldLen << " bytes found but only " << maxLen << " characters allowed");
                    errorFlag = EC_MaximumLengthViolated;
                    if (autocorrect)
                    {
                        /*  We are currently not removing any charaters since we do not know
                         *  whether a character consists of one or more bytes.  This will be
                         *  fixed in a future version.
                         */
                        DCMDATA_DEBUG("DcmCharString::verify() not correcting value length since "
                            << "multi-byte character sets are not yet supported, so cannot decide");
                    }
                }
                posStart = (posEnd == OFString_npos) ? posEnd : posEnd + 1;
            }
        }
    }
    /* report a debug message if an error occurred */
    if (errorFlag.bad())
    {
        DCMDATA_WARN("DcmCharString: One or more illegal values in element "
            << getTagName() << " " << getTag() << " with VM=" << getVM());
        /* do not return with an error since we do not know whether there really is a violation */
        errorFlag = EC_Normal;
    }
    return errorFlag;
}


OFBool DcmCharString::containsExtendedCharacters(const OFBool /*checkAllStrings*/)
{
    char *str = NULL;
    Uint32 len = 0;
    /* determine length in order to support possibly embedded NULL bytes */
    if (getString(str, len).good() && (str != NULL))
    {
        const char *p = str;
        for (Uint32 i = 0; i < len; i++)
        {
            /* check for 8 bit characters */
            if (OFstatic_cast(unsigned char, *p++) > 127)
                return OFTrue;
        }
    }
    return OFFalse;
}


OFBool DcmCharString::isAffectedBySpecificCharacterSet() const
{
    return OFTrue;
}


OFCondition DcmCharString::convertToUTF8(DcmSpecificCharacterSet *converter,
                                         const OFBool /*checkCharset*/)
{
    OFCondition status = EC_IllegalParameter;
    // it is an error if no converter is specified
    if (converter != NULL)
    {
        char *str = NULL;
        Uint32 len = 0;
        status = getString(str, len);
        // do nothing if string value is empty
        if (status.good() && (str != NULL) && (len > 0))
        {
            OFString resultStr;
            // convert string to UTF-8 and replace the element value
            status = converter->convertToUTF8String(str, len, resultStr, delimiterChars);
            if (status.good())
            {
                DCMDATA_TRACE("DcmCharString::convertToUTF8() updating value of element "
                    << getTagName() << " " << getTag());
                // currently, we are not checking whether the value has really changed
                // during the character set conversion - would that make any sense?
                status = putOFStringArray(resultStr);
            }
        }
    }
    return status;
}


/*
 * CVS/RCS Log:
 * $Log: dcchrstr.cc,v $
 * Revision 1.18  2011-11-01 14:54:04  joergr
 * Added support for code extensions (escape sequences) according to ISO 2022
 * to the character set conversion code.
 *
 * Revision 1.17  2011-10-26 16:20:19  joergr
 * Added method that allows for converting a dataset or element value to UTF-8.
 *
 * Revision 1.16  2011-10-18 14:00:12  joergr
 * Added support for embedded NULL bytes in string element values.
 *
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
