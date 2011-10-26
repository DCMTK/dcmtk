/*
 *
 *  Copyright (C) 2011, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Class for supporting the Specfic Character Set attribute
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-10-26 16:13:01 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcspchrs.h"
#include "dcmtk/dcmdata/dcbytstr.h"


DcmSpecificCharacterSet::DcmSpecificCharacterSet()
  : SpecificCharacterSet(),
    EncodingConverter()
{
}


DcmSpecificCharacterSet::~DcmSpecificCharacterSet()
{
}


const OFString &DcmSpecificCharacterSet::getCharacterSet() const
{
    return SpecificCharacterSet;
}


OFCondition DcmSpecificCharacterSet::selectCharacterSet(const OFString &charset)
{
    OFCondition status = EC_Normal;
    // first normalize the given string (original VR is "CS" with VM "1-n")
    SpecificCharacterSet = charset;
    normalizeString(SpecificCharacterSet, MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    // check whether it is multi-valued
    const unsigned long charsetVM = DcmElement::determineVM(SpecificCharacterSet.c_str(), SpecificCharacterSet.length());
    if (charsetVM == 0)
    {
        // no character set specified, use ASCII
        status = EncodingConverter.selectEncoding("ASCII", "UTF-8");
    }
    else if (charsetVM == 1)
    {
        // a single character set specified (no code extensions)
        if (SpecificCharacterSet == "ISO_IR 6")            // ASCII
        {
            DCMDATA_WARN("DcmSpecificCharacterSet: 'ISO_IR 6' is not a defined term in DICOM, will be treated as ASCII");
            status = EncodingConverter.selectEncoding("ASCII", "UTF-8");
        }
        else if (SpecificCharacterSet == "ISO_IR 100")     // Latin alphabet No. 1
            status = EncodingConverter.selectEncoding("ISO-8859-1", "UTF-8");
        else if (SpecificCharacterSet == "ISO_IR 101")     // Latin alphabet No. 2
            status = EncodingConverter.selectEncoding("ISO-8859-2", "UTF-8");
        else if (SpecificCharacterSet == "ISO_IR 109")     // Latin alphabet No. 3
            status = EncodingConverter.selectEncoding("ISO-8859-3", "UTF-8");
        else if (SpecificCharacterSet == "ISO_IR 110")     // Latin alphabet No. 4
            status = EncodingConverter.selectEncoding("ISO-8859-4", "UTF-8");
        else if (SpecificCharacterSet == "ISO_IR 144")     // Cyrillic
            status = EncodingConverter.selectEncoding("ISO-8859-5", "UTF-8");
        else if (SpecificCharacterSet == "ISO_IR 127")     // Arabic
            status = EncodingConverter.selectEncoding("ISO-8859-6", "UTF-8");
        else if (SpecificCharacterSet == "ISO_IR 126")     // Greek
            status = EncodingConverter.selectEncoding("ISO-8859-7", "UTF-8");
        else if (SpecificCharacterSet == "ISO_IR 138")     // Hebrew
            status = EncodingConverter.selectEncoding("ISO-8859-8", "UTF-8");
        else if (SpecificCharacterSet == "ISO_IR 148")     // Latin alphabet No. 5
            status = EncodingConverter.selectEncoding("ISO-8859-9", "UTF-8");
        else if (SpecificCharacterSet == "ISO_IR 13")      // Japanese
            status = EncodingConverter.selectEncoding("SHIFT_JIS", "UTF-8");  // is this really equivalent to "JIS X 0201"?
        else if (SpecificCharacterSet == "ISO_IR 166")     // Thai
            status = EncodingConverter.selectEncoding("TIS620", "UTF-8");
        else if (SpecificCharacterSet == "ISO_IR 192")     // Unicode in UTF-8
            status = EncodingConverter.selectEncoding("UTF-8", "UTF-8");
        else if (SpecificCharacterSet == "GB18030")        // Chinese
            status = EncodingConverter.selectEncoding("GB18030", "UTF-8");
        else {
            OFString message = "Cannot select character set: SpecificCharacterSet (0008,0005) value '";
            message += SpecificCharacterSet;
            message += "' not supported";
            status = makeOFCondition(OFM_dcmdata, EC_CODE_CannotSelectCharacterSet, OF_error, message.c_str());
        }
    } else {
        // multiple character sets specified (code extensions)
        status = makeOFCondition(OFM_dcmdata, EC_CODE_CannotSelectCharacterSet, OF_error,
            "Cannot select character set: code extensions according to ISO 2022 not yet supported");
    }
    return status;
}


OFCondition DcmSpecificCharacterSet::convertToUTF8String(const OFString &fromString,
                                                         OFString &toString)
{
    // call the real method converting the given string to UTF-8
    return convertToUTF8String(fromString.c_str(), fromString.length(), toString);
}


OFCondition DcmSpecificCharacterSet::convertToUTF8String(const char *fromString,
                                                         const size_t fromLength,
                                                         OFString &toString)
{
    // currently, we do not support ISO 2022, so it's quite easy to convert
    return EncodingConverter.convertString(fromString, fromLength, toString);
}


OFBool DcmSpecificCharacterSet::isConversionLibraryAvailable()
{
    // just call the appropriate function from the underlying class
    return OFCharacterEncoding::isLibraryAvailable();
}


size_t DcmSpecificCharacterSet::countCharactersInUTF8String(const OFString &utf8String)
{
    // just call the appropriate function from the underlying class
    return OFCharacterEncoding::countCharactersInUTF8String(utf8String);
}


/*
 *
 *  CVS/RCS Log:
 *  $Log: dcspchrs.cc,v $
 *  Revision 1.1  2011-10-26 16:13:01  joergr
 *  Added helper class for converting between different DICOM character sets.
 *  This initial version only supports the conversion to UTF-8 (Unicode) and only
 *  from DICOM characters sets without code extension techniques (i.e. ISO 2022).
 *
 *
 *
 */
