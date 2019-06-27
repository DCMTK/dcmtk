/*
 *
 *  Copyright (C) 2013-2019, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmOtherDouble
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofuuid.h"

#include "dcmtk/dcmdata/dcvrod.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/dcmdata/dcjson.h"


// ********************************


DcmOtherDouble::DcmOtherDouble(const DcmTag &tag,
                               const Uint32 len)
  : DcmFloatingPointDouble(tag, len)
{
}


DcmOtherDouble::DcmOtherDouble(const DcmOtherDouble &old)
  : DcmFloatingPointDouble(old)
{
}


DcmOtherDouble::~DcmOtherDouble()
{
}


DcmOtherDouble &DcmOtherDouble::operator=(const DcmOtherDouble &obj)
{
    DcmFloatingPointDouble::operator=(obj);
    return *this;
}


OFCondition DcmOtherDouble::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmOtherDouble &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmOtherDouble::ident() const
{
    return EVR_OD;
}


OFCondition DcmOtherDouble::checkValue(const OFString & /*vm*/,
                                       const OFBool /*oldFormat*/)
{
    /* currently no checks are performed */
    return EC_Normal;
}


unsigned long DcmOtherDouble::getVM()
{
    /* value multiplicity for OD is defined as 1 */
    return 1;
}


// ********************************


OFCondition DcmOtherDouble::writeXML(STD_NAMESPACE ostream &out,
                                    const size_t flags)
{
    /* always write XML start tag */
    writeXMLStartTag(out, flags);
    /* OD data requires special handling in the Native DICOM Model format */
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* for an empty value field, we do not need to do anything */
        if (getLengthField() > 0)
        {
            /* encode binary data as Base64 */
            if (flags & DCMTypes::XF_encodeBase64)
            {
                out << "<InlineBinary>";
                Uint8 *byteValues = OFstatic_cast(Uint8 *, getValue());
                /* Base64 encoder requires big endian input data */
                swapIfNecessary(EBO_BigEndian, gLocalByteOrder, byteValues, getLengthField(), sizeof(Float64));
                /* update the byte order indicator variable correspondingly */
                setByteOrder(EBO_BigEndian);
                OFStandard::encodeBase64(out, byteValues, OFstatic_cast(size_t, getLengthField()));
                out << "</InlineBinary>" << OFendl;
            } else {
                /* generate a new UID but the binary data is not (yet) written. */
                OFUUID uuid;
                out << "<BulkData uuid=\"";
                uuid.print(out, OFUUID::ER_RepresentationHex);
                out << "\"/>" << OFendl;
            }
        }
    } else {
        /* write element value (if loaded) */
        if (valueLoaded())
        {
            Float64 *floatValues = NULL;
            /* get and check 64 bit float data */
            if (getFloat64Array(floatValues).good() && (floatValues != NULL))
            {
                const size_t count = getNumberOfValues();
                /* count can be zero if we have an invalid element with less than eight bytes length */
                if (count > 0)
                {
                    /* increase default precision - see DcmFloatingPointDouble::print() */
                    const STD_NAMESPACE streamsize oldPrecision = out.precision(17);
                    /* print float values with separators */
                    out << (*(floatValues++));
                    for (unsigned long i = 1; i < count; i++)
                        out << "\\" << (*(floatValues++));
                    /* reset i/o manipulators */
                    out.precision(oldPrecision);
                }
            }
        }
    }
    /* always write XML end tag */
    writeXMLEndTag(out, flags);
    /* always report success */
    return EC_Normal;
}


// ********************************


OFCondition DcmOtherDouble::writeJson(STD_NAMESPACE ostream &out,
                                      DcmJsonFormat &format)
{
    /* always write JSON Opener */
    writeJsonOpener(out, format);
    /* for an empty value field, we do not need to do anything */
    if (getLengthField() > 0)
    {
        OFString value;
        if (format.asBulkDataURI(getTag(), value))
        {
            /* return defined BulkDataURI */
            format.printBulkDataURIPrefix(out);
            DcmJsonFormat::printString(out, value);
        }
        else
        {
            /* encode binary data as Base64 */
            format.printInlineBinaryPrefix(out);
            out << "\"";
            Uint8 *byteValues = OFstatic_cast(Uint8 *, getValue());
            OFStandard::encodeBase64(out, byteValues, OFstatic_cast(size_t, getLengthField()));
            out << "\"";
        }
    }
    /* write JSON Closer  */
    writeJsonCloser(out, format);
    /* always report success */
    return EC_Normal;
}


// ********************************


OFCondition DcmOtherDouble::createFloat64Array(const Uint32 numDoubles,
                                               Float64 *&doubleVals)
{
    Uint32 bytesRequired = 0;
    /* make sure that max length is not exceeded */
    if (OFStandard::safeMult(numDoubles, OFstatic_cast(Uint32, sizeof(Float64)), bytesRequired))
        errorFlag = createEmptyValue(bytesRequired);
    else
        errorFlag = EC_ElemLengthExceeds32BitField;
    if (errorFlag.good())
        doubleVals = OFstatic_cast(Float64 *, this->getValue());
    else
        doubleVals = NULL;
    return errorFlag;
}
