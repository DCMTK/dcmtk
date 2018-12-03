/*
 *
 *  Copyright (C) 1994-2018, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmOtherByteOtherWord
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofuuid.h"

#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for UID generation */

#define INCLUDE_CSTDIO
#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmOtherByteOtherWord::DcmOtherByteOtherWord(const DcmTag &tag)
: DcmElement(tag, 0)
, compactAfterTransfer(OFFalse)
{
}


DcmOtherByteOtherWord::DcmOtherByteOtherWord(const DcmTag &tag,
                                             const Uint32 len)
: DcmElement(tag, len)
, compactAfterTransfer(OFFalse)
{
}


DcmOtherByteOtherWord::DcmOtherByteOtherWord(const DcmOtherByteOtherWord &old)
: DcmElement(old)
, compactAfterTransfer(old.compactAfterTransfer)
{
}


DcmOtherByteOtherWord::~DcmOtherByteOtherWord()
{
}


DcmOtherByteOtherWord &DcmOtherByteOtherWord::operator=(const DcmOtherByteOtherWord &obj)
{
    DcmElement::operator=(obj);
    compactAfterTransfer = obj.compactAfterTransfer;
    return *this;
}


int DcmOtherByteOtherWord::compare(const DcmElement& rhs) const
{
    int result = DcmElement::compare(rhs);
    if (result != 0)
    {
        return result;
    }

    /* cast away constness (dcmdata is not const correct...) */
    DcmOtherByteOtherWord* myThis = NULL;
    DcmOtherByteOtherWord* myRhs = NULL;
    myThis = OFconst_cast(DcmOtherByteOtherWord*, this);
    myRhs =  OFstatic_cast(DcmOtherByteOtherWord*, OFconst_cast(DcmElement*, &rhs));

    /* compare length */
    unsigned long thisLength = myThis->getLength();
    unsigned long rhsLength= myRhs->getLength();
    if (thisLength < rhsLength)
    {
      return -1;
    }
    else if (thisLength > rhsLength)
    {
      return 1;
    }
    /* finally, check equality of values. getValue() makes sure byte
     * swapping is applied as necessary. */
    void* thisData = myThis->getValue();
    void* rhsData = myRhs->getValue();
    return memcmp(thisData, rhsData, thisLength);
}


OFCondition DcmOtherByteOtherWord::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmOtherByteOtherWord &, rhs);
    }
    return EC_Normal;
}


// ********************************


DcmEVR DcmOtherByteOtherWord::ident() const
{
    return getTag().getEVR();
}


OFCondition DcmOtherByteOtherWord::checkValue(const OFString & /*vm*/,
                                              const OFBool /*oldFormat*/)
{
    /* currently no checks are performed */
    return EC_Normal;
}


unsigned long DcmOtherByteOtherWord::getVM()
{
    /* value multiplicity for OB/OW is defined as 1 */
    return 1;
}


unsigned long DcmOtherByteOtherWord::getNumberOfValues()
{
    const DcmEVR evr = getTag().getEVR();
    unsigned long result = OFstatic_cast(unsigned long, getLengthField());
    /* check whether values are stored as 16 bit */
    if ((evr == EVR_OW) || (evr == EVR_lt))
        result /= 2;
    return result;
}


OFCondition DcmOtherByteOtherWord::setVR(DcmEVR vr)
{
    setTagVR(vr);
    return EC_Normal;
}


// ********************************


void DcmOtherByteOtherWord::print(STD_NAMESPACE ostream&out,
                                  const size_t flags,
                                  const int level,
                                  const char * /*pixelFileName*/,
                                  size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        const DcmEVR evr = getTag().getEVR();
        Uint16 *wordValues = NULL;
        Uint8 *byteValues = NULL;
        /* get 8 or 16 bit data respectively */
        if (evr == EVR_OW || evr == EVR_lt)
            errorFlag = getUint16Array(wordValues);
        else
            errorFlag = getUint8Array(byteValues);
        /* check data */
        if ((wordValues != NULL) || (byteValues != NULL))
        {
            const unsigned long count = getNumberOfValues();
            /* double-check length field for valid value */
            if (count > 0)
            {
                /* determine number of values to be printed */
                const unsigned int vrSize = (evr == EVR_OW || evr == EVR_lt) ? 4 : 2;
                unsigned long expectedLength = count * (vrSize + 1) - 1;
                const unsigned long printCount =
                    ((expectedLength > DCM_OptPrintLineLength) && (flags & DCMTypes::PF_shortenLongTagValues)) ?
                    (DCM_OptPrintLineLength - 3 /* for "..." */ + 1 /* for last "\" */) / (vrSize + 1) : count;
                unsigned long printedLength = printCount * (vrSize + 1) - 1;
                /* print line start with tag and VR */
                printInfoLineStart(out, flags, level);
                /* print multiple values */
                if (printCount > 0)
                {
                    out << STD_NAMESPACE hex << STD_NAMESPACE setfill('0');
                    if (evr == EVR_OW || evr == EVR_lt)
                    {
                        /* print word values in hex mode */
                        out << STD_NAMESPACE setw(vrSize) << (*(wordValues++));
                        for (unsigned long i = 1; i < printCount; i++)
                            out << "\\" << STD_NAMESPACE setw(vrSize) << (*(wordValues++));
                    } else {
                        /* print byte values in hex mode */
                        out << STD_NAMESPACE setw(vrSize) << OFstatic_cast(int, *(byteValues++));
                        for (unsigned long i = 1; i < printCount; i++)
                            out << "\\" << STD_NAMESPACE setw(vrSize) << OFstatic_cast(int, *(byteValues++));
                    }
                    /* reset i/o manipulators */
                    out << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
                }
                /* print trailing "..." if data has been truncated */
                if (printCount < count)
                {
                    out << "...";
                    printedLength += 3;
                }
                /* print line end with length, VM and tag name */
                printInfoLineEnd(out, flags, printedLength);
            } else {
                /* count can be zero if we have an invalid OW element with less than two bytes length */
                printInfoLine(out, flags, level, "(invalid value)");
            }
        } else
            printInfoLine(out, flags, level, "(no value available)");
    } else
        printInfoLine(out, flags, level, "(not loaded)");
}


void DcmOtherByteOtherWord::printPixel(STD_NAMESPACE ostream &out,
                                       const size_t flags,
                                       const int level,
                                       const char *pixelFileName,
                                       size_t *pixelCounter)
{
    if (pixelFileName != NULL)
    {
        /* create filename for pixel data file */
        OFString fname = pixelFileName;
        fname += ".";
        if (pixelCounter != NULL)
        {
            char num[20];
            sprintf(num, "%ld", OFstatic_cast(long, (*pixelCounter)++));
            fname += num;
        }
        fname += ".raw";
        /* create reference to pixel data file in dump output */
        OFString str = "=";
        str += fname;
        printInfoLine(out, flags, level, str.c_str(), NULL /*tag*/, OFFalse /*isInfo*/);
        /* check whether pixel data file already exists */
        if (!OFStandard::fileExists(fname))
        {
            /* create binary file for pixel data */
            FILE *file = fopen(fname.c_str(), "wb");
            if (file != NULL)
            {
                if (getTag().getEVR() == EVR_OW || getTag().getEVR() == EVR_lt)
                {
                    /* write 16 bit data in little endian byte-order */
                    Uint16 *data = NULL;
                    getUint16Array(data);
                    if (data != NULL)
                    {
                        swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, data, getLengthField(), sizeof(Uint16));
                        setByteOrder(EBO_LittleEndian);
                        const size_t tobewritten = OFstatic_cast(size_t, getLengthField() / sizeof(Uint16));
                        const size_t written = fwrite(data, sizeof(Uint16), tobewritten, file);
                        if (written != tobewritten)
                            DCMDATA_WARN("DcmOtherByteOtherWord: Some bytes were not written: " << (tobewritten - written));
                    }
                } else {
                    Uint8 *data = NULL;
                    getUint8Array(data);
                    if (data != NULL)
                    {
                        const size_t tobewritten = OFstatic_cast(size_t, getLengthField());
                        const size_t written = fwrite(data, sizeof(Uint8), tobewritten, file);
                        if (written != tobewritten)
                            DCMDATA_WARN("DcmOtherByteOtherWord: Some bytes were not written: " << (tobewritten - written));
                    }
                }
                fclose(file);
            } else {
                DCMDATA_WARN("DcmOtherByteOtherWord: Can't open output file for pixel data: " << fname);
            }
        } else {
            DCMDATA_WARN("DcmOtherByteOtherWord: Output file for pixel data already exists, skipping: " << fname);
        }
    } else
        DcmOtherByteOtherWord::print(out, flags, level, pixelFileName, pixelCounter);
}


// ********************************


OFCondition DcmOtherByteOtherWord::alignValue()
{
    errorFlag = EC_Normal;
    if ((getTag().getEVR() != EVR_OW && getTag().getEVR() != EVR_lt) && (getLengthField() > 0) && ((getLengthField() & 1) != 0))
    {
        // We have an odd number of bytes. This should never happen and is certainly not allowed in DICOM.
        // To fix this problem, we will add a zero pad byte at the end of the value field.
        // This requires us to load the value field into memory, which may very well be a problem
        // if this is part of a very large multi-frame object.
        Uint8 *bytes = OFstatic_cast(Uint8 *, getValue(getByteOrder()));

        // getValue() could call loadValue() which then calls postLoadValue()
        // which in turn calls this function again. Thus, we have to make sure
        // that the length field is still odd after getValue() returns.
        if (bytes && (getLengthField() & 1) != 0)
        {
            // newValueField always allocates an even number of bytes
            // and sets the pad byte to zero, so we can safely increase Length here
            setLengthField(getLengthField() + 1);
        }
    }
    return errorFlag;
}


void DcmOtherByteOtherWord::postLoadValue()
{
    if (dcmEnableAutomaticInputDataCorrection.get())
        alignValue();
}


// ********************************


OFCondition DcmOtherByteOtherWord::putUint8Array(const Uint8 *byteValue,
                                                 const unsigned long numBytes)
{
    errorFlag = EC_Normal;
    if (numBytes > 0)
    {
        /* check for valid 8 bit data */
        if ((byteValue != NULL) && (getTag().getEVR() != EVR_OW && getTag().getEVR() != EVR_lt))
        {
            errorFlag = putValue(byteValue, OFstatic_cast(Uint32, sizeof(Uint8) * OFstatic_cast(size_t, numBytes)));
            alignValue();
        } else
            errorFlag = EC_CorruptedData;
    } else
        putValue(NULL, 0);
    return errorFlag;
}


OFCondition DcmOtherByteOtherWord::putUint16Array(const Uint16 *wordValue,
                                                  const unsigned long numWords)
{
    errorFlag = EC_Normal;
    if (numWords > 0)
    {
        /* check for valid 16 bit data */
        if ((wordValue != NULL) && (getTag().getEVR() == EVR_OW || getTag().getEVR() == EVR_lt))
            errorFlag = putValue(wordValue, OFstatic_cast(Uint32, sizeof(Uint16) * OFstatic_cast(size_t, numWords)));
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::createUint8Array(const Uint32 numBytes,
                                                    Uint8 *&bytes)
{
    /* check value representation */
    if ((getTag().getEVR() != EVR_OW) && (getTag().getEVR() != EVR_lt))
        errorFlag = createEmptyValue(OFstatic_cast(Uint32, sizeof(Uint8) * OFstatic_cast(size_t, numBytes)));
    else
        errorFlag = EC_CorruptedData;
    if (errorFlag.good())
        bytes = OFstatic_cast(Uint8 *, this->getValue());
    else
        bytes = NULL;
    return errorFlag;
}


OFCondition DcmOtherByteOtherWord::createUint16Array(const Uint32 numWords,
                                                     Uint16 *&words)
{
    /* check value representation */
    if ((getTag().getEVR() == EVR_OW) || (getTag().getEVR() == EVR_lt))
        errorFlag = createEmptyValue(OFstatic_cast(Uint32, sizeof(Uint16) * OFstatic_cast(size_t, numWords)));
    else
        errorFlag = EC_CorruptedData;
    if (errorFlag.good())
        words = OFstatic_cast(Uint16 *, this->getValue());
    else
        words = NULL;
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::putString(const char *stringVal)
{
    /* determine length of the string value */
    const size_t stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, OFstatic_cast(Uint32, stringLen));
}


OFCondition DcmOtherByteOtherWord::putString(const char *stringVal,
                                             const Uint32 stringLen)
{
    errorFlag = EC_Normal;
    /* determine VM of the string */
    unsigned long vm = DcmElement::determineVM(stringVal, stringLen);
    if (vm > 0)
    {
        const DcmEVR evr = getTag().getEVR();
        Uint8 *byteField = NULL;
        Uint16 *wordField = NULL;
        /* create new value field */
        if (evr == EVR_OW || evr == EVR_lt)
            wordField = new Uint16[vm];
        else
            byteField = new Uint8[vm];
        Uint16 intVal = 0;
        OFString value;
        size_t pos = 0;
        /* retrieve binary data from hexa-decimal string */
        for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
        {
            /* get specified value from multi-valued string */
            pos = DcmElement::getValueFromString(stringVal, pos, stringLen, value);
            if (!value.empty())
            {
                /* integer overflow is currently not checked! */
                if (sscanf(value.c_str(), "%hx", &intVal) != 1)
                    errorFlag = EC_CorruptedData;
                else if (evr == EVR_OW || evr == EVR_lt)
                    wordField[i] = OFstatic_cast(Uint16, intVal);
                else
                    byteField[i] = OFstatic_cast(Uint8, intVal);
            } else
                errorFlag = EC_CorruptedData;
        }
        /* set binary data as the element value */
        if (errorFlag.good())
        {
            if (evr == EVR_OW || evr == EVR_lt)
                errorFlag = putUint16Array(wordField, vm);
            else
                errorFlag = putUint8Array(byteField, vm);
        }
        /* delete temporary buffers */
        delete[] byteField;
        delete[] wordField;
    } else
        putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::getUint8(Uint8 &byteVal,
                                            const unsigned long pos)
{
    /* get 8 bit data */
    Uint8 *uintValues = NULL;
    errorFlag = getUint8Array(uintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (uintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getLength() /*bytes*/)
            errorFlag = EC_IllegalParameter;
        else
            byteVal = uintValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        byteVal = 0;
    return errorFlag;
}


OFCondition DcmOtherByteOtherWord::getUint8Array(Uint8 *&byteVals)
{
    errorFlag = EC_Normal;
    if (getTag().getEVR() != EVR_OW && getTag().getEVR() != EVR_lt)
        byteVals = OFstatic_cast(Uint8 *, getValue());
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::getUint16(Uint16 &wordVal,
                                             const unsigned long pos)
{
    Uint16 *uintValues = NULL;
    errorFlag = getUint16Array(uintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (uintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getLength() / sizeof(Uint16) /*words*/)
            errorFlag = EC_IllegalParameter;
        else
            wordVal = uintValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        wordVal = 0;
    return errorFlag;
}


OFCondition DcmOtherByteOtherWord::getUint16Array(Uint16 *&wordVals)
{
    errorFlag = EC_Normal;
    if (getTag().getEVR() == EVR_OW || getTag().getEVR() == EVR_lt)
        wordVals = OFstatic_cast(Uint16 *, getValue());
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::getOFString(OFString &stringVal,
                                               const unsigned long pos,
                                               OFBool /*normalize*/)
{
    if (getTag().getEVR() == EVR_OW || getTag().getEVR() == EVR_lt)
    {
        Uint16 uint16Val;
        /* get the specified numeric value (16 bit) */
        errorFlag = getUint16(uint16Val, pos);
        if (errorFlag.good())
        {
            /* ... and convert it to a character string (hex mode) */
            char buffer[32];
            sprintf(buffer, "%4.4hx", uint16Val);
            /* assign result */
            stringVal = buffer;
        }
    } else {
        Uint8 uint8Val;
        /* get the specified numeric value (8 bit) */
        errorFlag = getUint8(uint8Val, pos);
        if (errorFlag.good())
        {
            /* ... and convert it to a character string (hex mode) */
            char buffer[32];
            sprintf(buffer, "%2.2hx", OFstatic_cast(unsigned short, uint8Val));
            /* assign result */
            stringVal = buffer;
        }
    }
    return errorFlag;
}


OFCondition DcmOtherByteOtherWord::getOFStringArray(OFString &stringVal,
                                                    OFBool /*normalize*/)
{
    if (getTag().getEVR() == EVR_OW || getTag().getEVR() == EVR_lt)
    {
        /* get array of 16 bit values */
        Uint16 *uint16Vals = OFstatic_cast(Uint16 *, getValue());
        const size_t count = OFstatic_cast(size_t, getLength() / sizeof(Uint16));
        if ((uint16Vals != NULL) && (count > 0))
        {
            OFOStringStream stream;
            /* output first value in hexadecimal format */
            stream << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                   << STD_NAMESPACE setw(4) << *(uint16Vals++);
            /* for all other array elements ... */
            for (size_t i = 1; i < count; i++)
                stream << "\\" << STD_NAMESPACE setw(4) << *(uint16Vals++);
            stream << OFStringStream_ends;
            /* convert string stream into a character string */
            OFSTRINGSTREAM_GETSTR(stream, buffer_str)
            stringVal.assign(buffer_str);
            OFSTRINGSTREAM_FREESTR(buffer_str)
            errorFlag = EC_Normal;
        } else
            errorFlag = EC_IllegalCall;
    } else {
        /* get array of 8 bit values */
        Uint8 *uint8Vals = OFstatic_cast(Uint8 *, getValue());
        const size_t count = OFstatic_cast(size_t, getLength());
        if ((uint8Vals != NULL) && (count > 0))
        {
            OFOStringStream stream;
            /* output first value in hexadecimal format */
            stream << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                   << STD_NAMESPACE setw(2) << OFstatic_cast(int, *(uint8Vals++));
            /* for all other array elements ... */
            for (size_t i = 1; i < count; i++)
                stream << "\\" << STD_NAMESPACE setw(2) << OFstatic_cast(int, *(uint8Vals++));
            stream << OFStringStream_ends;
            /* convert string stream into a character string */
            OFSTRINGSTREAM_GETSTR(stream, buffer_str)
            stringVal.assign(buffer_str);
            OFSTRINGSTREAM_FREESTR(buffer_str)
            errorFlag = EC_Normal;
        } else
            errorFlag = EC_IllegalCall;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::verify(const OFBool autocorrect)
{
    errorFlag = EC_Normal;
    if (autocorrect)
        errorFlag = alignValue();
    return errorFlag;
}


// ********************************


OFBool DcmOtherByteOtherWord::canWriteXfer(const E_TransferSyntax newXfer,
                                           const E_TransferSyntax /*oldXfer*/)
{
    DcmXfer newXferSyn(newXfer);
    return (getTag() != DCM_PixelData) || !newXferSyn.isEncapsulated();
}


// ********************************


OFCondition DcmOtherByteOtherWord::write(
    DcmOutputStream &outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype,
    DcmWriteCache *wcache)
{
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        if (getTransferState() == ERW_init)
        {
          // if the attribute value is in file, we should call compact() if the write
          // operation causes the value to be loaded into main memory, which can happen
          // for odd length attributes.
          compactAfterTransfer = ! valueLoaded();

          // this call may cause the attribute to be loaded into memory
          alignValue();
        }

        // call inherited method
        errorFlag = DcmElement::write(outStream, oxfer, enctype, wcache);
    }

    // if the write operation has completed successfully, call compact if the
    // attribute value resided in file prior to the write operation.
    if (errorFlag.good() && compactAfterTransfer) compact();

    // return error status
    return errorFlag;
}


OFCondition DcmOtherByteOtherWord::writeSignatureFormat(
    DcmOutputStream &outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype,
    DcmWriteCache *wcache)
{
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        if (getTransferState() == ERW_init)
        {
          // if the attribute value is in file, we should call compact() if the write
          // operation causes the value to be loaded into main memory, which can happen
          // for odd length attributes.
          compactAfterTransfer = ! valueLoaded();

          // this call may cause the attribute to be loaded into memory
          alignValue();
        }

        // call inherited method
        errorFlag = DcmElement::writeSignatureFormat(outStream, oxfer, enctype, wcache);
    }

    // if the write operation has completed successfully, call compact if the
    // attribute value resided in file prior to the write operation.
    if (errorFlag.good() && compactAfterTransfer) compact();

    // return error status
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::writeXML(STD_NAMESPACE ostream &out,
                                            const size_t flags)
{
    /* OB/OW data requires special handling in the Native DICOM Model format */
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* write XML start tag */
        writeXMLStartTag(out, flags);
        /* for an empty value field, we do not need to do anything */
        if (getLengthField() > 0)
        {
            /* encode binary data as Base64 */
            if (flags & DCMTypes::XF_encodeBase64)
            {
                const DcmEVR evr = getTag().getEVR();
                out << "<InlineBinary>";
                Uint8 *byteValues = OFstatic_cast(Uint8 *, getValue());
                if ((evr == EVR_OW) || (evr == EVR_lt))
                {
                    /* Base64 encoder requires big endian input data */
                    swapIfNecessary(EBO_BigEndian, gLocalByteOrder, byteValues, getLengthField(), sizeof(Uint16));
                    /* update the byte order indicator variable correspondingly */
                    setByteOrder(EBO_BigEndian);
                }
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
        /* write XML end tag */
        writeXMLEndTag(out, flags);
    } else {
        /* XML start tag: <element tag="gggg,eeee" vr="XX" ...> */
        if (!(flags & DCMTypes::XF_writeBinaryData))
            writeXMLStartTag(out, flags, "binary=\"hidden\"");
        else if (flags & DCMTypes::XF_encodeBase64)
            writeXMLStartTag(out, flags, "binary=\"base64\"");
        else
            writeXMLStartTag(out, flags, "binary=\"yes\"");
        /* write element value (if loaded) */
        if (valueLoaded() && (flags & DCMTypes::XF_writeBinaryData))
        {
            const DcmEVR evr = getTag().getEVR();
            /* encode binary data as Base64 */
            if (flags & DCMTypes::XF_encodeBase64)
            {
                Uint8 *byteValues = OFstatic_cast(Uint8 *, getValue());
                if ((evr == EVR_OW) || (evr == EVR_lt))
                {
                    /* Base64 encoder requires big endian input data */
                    swapIfNecessary(EBO_BigEndian, gLocalByteOrder, byteValues, getLengthField(), sizeof(Uint16));
                    /* update the byte order indicator variable correspondingly */
                    setByteOrder(EBO_BigEndian);
                }
                OFStandard::encodeBase64(out, byteValues, OFstatic_cast(size_t, getLengthField()));
            } else {
                if ((evr == EVR_OW) || (evr == EVR_lt))
                {
                    /* get and check 16 bit data */
                    Uint16 *wordValues = NULL;
                    if (getUint16Array(wordValues).good() && (wordValues != NULL))
                    {
                        const unsigned long count = getLengthField() / OFstatic_cast(unsigned long, sizeof(Uint16));
                        /* count can be zero if we have an invalid element with less than two bytes length */
                        if (count > 0)
                        {
                            out << STD_NAMESPACE hex << STD_NAMESPACE setfill('0');
                            /* print word values in hex mode */
                            out << STD_NAMESPACE setw(4) << (*(wordValues++));
                            for (unsigned long i = 1; i < count; i++)
                                out << "\\" << STD_NAMESPACE setw(4) << (*(wordValues++));
                            /* reset i/o manipulators */
                            out << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
                        }
                    }
                } else {
                    /* get and check 8 bit data */
                    Uint8 *byteValues = NULL;
                    if (getUint8Array(byteValues).good() && (byteValues != NULL))
                    {
                        const unsigned long count = getLengthField();
                        out << STD_NAMESPACE hex << STD_NAMESPACE setfill('0');
                        /* print byte values in hex mode */
                        out << STD_NAMESPACE setw(2) << OFstatic_cast(int, *(byteValues++));
                        for (unsigned long i = 1; i < count; i++)
                            out << "\\" << STD_NAMESPACE setw(2) << OFstatic_cast(int, *(byteValues++));
                        /* reset i/o manipulators */
                        out << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ');
                    }
                }
            }
        }
        /* XML end tag: </element> */
        writeXMLEndTag(out, flags);
    }
    /* always report success */
    return EC_Normal;
}


// ********************************


OFCondition DcmOtherByteOtherWord::writeJson(STD_NAMESPACE ostream &out,
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
