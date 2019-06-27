/*
 *
 *  Copyright (C) 2002-2017, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmOtherFloat
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofuuid.h"

#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/dcmdata/dcvrof.h"
#include "dcmtk/dcmdata/dcswap.h"


// ********************************


DcmOtherFloat::DcmOtherFloat(const DcmTag &tag,
                             const Uint32 len)
  : DcmFloatingPointSingle(tag, len)
{
}


DcmOtherFloat::DcmOtherFloat(const DcmOtherFloat &old)
  : DcmFloatingPointSingle(old)
{
}


DcmOtherFloat::~DcmOtherFloat()
{
}


DcmOtherFloat &DcmOtherFloat::operator=(const DcmOtherFloat &obj)
{
    DcmFloatingPointSingle::operator=(obj);
    return *this;
}


OFCondition DcmOtherFloat::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmOtherFloat &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmOtherFloat::ident() const
{
    return EVR_OF;
}


OFCondition DcmOtherFloat::checkValue(const OFString & /*vm*/,
                                      const OFBool /*oldFormat*/)
{
    /* currently no checks are performed */
    return EC_Normal;
}


unsigned long DcmOtherFloat::getVM()
{
    /* value multiplicity for OF is defined as 1 */
    return 1;
}


// ********************************


OFCondition DcmOtherFloat::writeXML(STD_NAMESPACE ostream &out,
                                    const size_t flags)
{
    /* always write XML start tag */
    writeXMLStartTag(out, flags);
    /* OF data requires special handling in the Native DICOM Model format */
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
                swapIfNecessary(EBO_BigEndian, gLocalByteOrder, byteValues, getLengthField(), sizeof(Float32));
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
            Float32 *floatValues = NULL;
            /* get and check 32 bit float data */
            if (getFloat32Array(floatValues).good() && (floatValues != NULL))
            {
                const size_t count = getNumberOfValues();
                /* count can be zero if we have an invalid element with less than four bytes length */
                if (count > 0)
                {
                    /* increase default precision - see DcmFloatingPointSingle::print() */
                    const STD_NAMESPACE streamsize oldPrecision = out.precision(8);
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


OFCondition DcmOtherFloat::writeJson(STD_NAMESPACE ostream &out,
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
    /* always write JSON Closer */
    writeJsonCloser(out, format);
    /* always report success */
    return EC_Normal;
}


// ********************************


OFCondition DcmOtherFloat::createFloat32Array(const Uint32 numFloats,
                                              Float32 *&floatVals)
{
    Uint32 bytesRequired = 0;
    /* make sure that max length is not exceeded */
    if (OFStandard::safeMult(numFloats, OFstatic_cast(Uint32, sizeof(Float32)), bytesRequired))
        errorFlag = createEmptyValue(bytesRequired);
    else
        errorFlag = EC_ElemLengthExceeds32BitField;
    if (errorFlag.good())
        floatVals = OFstatic_cast(Float32 *, this->getValue());
    else
        floatVals = NULL;
    return errorFlag;
}
