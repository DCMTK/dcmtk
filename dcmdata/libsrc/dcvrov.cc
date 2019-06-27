/*
 *
 *  Copyright (C) 2019, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmOther64bitVeryLong
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofuuid.h"

#include "dcmtk/dcmdata/dcvrov.h"
#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/dcmdata/dcswap.h"


// ********************************


DcmOther64bitVeryLong::DcmOther64bitVeryLong(const DcmTag &tag)
  : DcmUnsigned64bitVeryLong(tag, 0)
{
}


DcmOther64bitVeryLong::DcmOther64bitVeryLong(const DcmTag &tag,
                                             const Uint32 len)
  : DcmUnsigned64bitVeryLong(tag, len)
{
}


DcmOther64bitVeryLong::DcmOther64bitVeryLong(const DcmOther64bitVeryLong &old)
  : DcmUnsigned64bitVeryLong(old)
{
}


DcmOther64bitVeryLong::~DcmOther64bitVeryLong()
{
}


DcmOther64bitVeryLong &DcmOther64bitVeryLong::operator=(const DcmOther64bitVeryLong &obj)
{
    DcmUnsigned64bitVeryLong::operator=(obj);
    return *this;
}


OFCondition DcmOther64bitVeryLong::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmOther64bitVeryLong &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmOther64bitVeryLong::ident() const
{
    return EVR_OV;
}


OFCondition DcmOther64bitVeryLong::checkValue(const OFString & /*vm*/,
                                              const OFBool /*oldFormat*/)
{
    /* currently no checks are performed */
    return EC_Normal;
}


unsigned long DcmOther64bitVeryLong::getVM()
{
    /* value multiplicity for OV is defined as 1 */
    return 1;
}


// ********************************


OFCondition DcmOther64bitVeryLong::writeXML(STD_NAMESPACE ostream &out,
                                            const size_t flags)
{
    /* always write XML start tag */
    writeXMLStartTag(out, flags);
    /* OV data requires special handling in the Native DICOM Model format */
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
                swapIfNecessary(EBO_BigEndian, gLocalByteOrder, byteValues, getLengthField(), sizeof(Uint64));
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
            Uint64 *uintVals = NULL;
            /* get and check 64 bit data */
            if (getUint64Array(uintVals).good() && (uintVals != NULL))
            {
                const size_t count = getNumberOfValues();
                /* count can be zero if we have an invalid element with less than eight bytes length */
                if (count > 0)
                {
                    /* print unsigned long values in hex mode */
                    out << (*(uintVals++));
                    for (unsigned long i = 1; i < count; i++)
                        out << "\\" << (*(uintVals++));
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


OFCondition DcmOther64bitVeryLong::writeJson(STD_NAMESPACE ostream &out,
                                             DcmJsonFormat &format)
{
    /* write JSON Opener */
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
    /* write JSON Closer */
    writeJsonCloser(out, format);
    /* always report success */
    return EC_Normal;
}


// ********************************


OFCondition DcmOther64bitVeryLong::createUint64Array(const Uint32 numQuadWords,
                                                     Uint64 *&quadWords)
{
    Uint32 bytesRequired = 0;
    /* make sure that max length is not exceeded */
    if (OFStandard::safeMult(numQuadWords, OFstatic_cast(Uint32, sizeof(Uint64)), bytesRequired))
        errorFlag = createEmptyValue(bytesRequired);
    else
        errorFlag = EC_ElemLengthExceeds32BitField;
    if (errorFlag.good())
        quadWords = OFstatic_cast(Uint64 *, this->getValue());
    else
        quadWords = NULL;
    return errorFlag;
}
