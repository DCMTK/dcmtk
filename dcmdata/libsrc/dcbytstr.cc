/*
 *
 *  Copyright (C) 1994-2013, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmByteString
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcbytstr.h"
#include "dcmtk/dcmdata/dcvr.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_NEW
#include "dcmtk/ofstd/ofstdinc.h"


// global flags

OFGlobal<OFBool> dcmEnableVRCheckerForStringValues(OFTrue);


// global function to get a particular component of a DICOM string
OFCondition getStringPart(OFString &result,
                          const char *str,
                          const unsigned long len,
                          const unsigned long pos)
{
    OFCondition l_error = EC_Normal;
    /* check string parameter */
    if (str != NULL)
    {
        /* search for beginning of specified string component */
        unsigned long i = 0;
        unsigned long curPos = 0;
        while ((curPos < pos) && (i++ < len))
        {
            if (*str++ == '\\')
                curPos++;
        }
        /* if found ... */
        if (curPos == pos)
        {
            /* search for end of specified string component */
            const char *p = str;
            while ((*p != '\\') && (i++ < len))
                p++;
            /* check whether string component is non-empty */
            if (p - str > 0)
                result.assign(str, p - str);
            else
                result.clear();
        } else {
            /* specified component index not found in string */
            l_error = EC_IllegalParameter;
        }
    } else
        l_error = EC_IllegalParameter;
    return l_error;
}


// ********************************


DcmByteString::DcmByteString(const DcmTag &tag,
                             const Uint32 len)
  : DcmElement(tag, len),
    paddingChar(' '),
    maxLength(DCM_UndefinedLength),
    realLength(len),
    fStringMode(DCM_UnknownString),
    nonSignificantChars()
{
}


DcmByteString::DcmByteString(const DcmByteString &old)
  : DcmElement(old),
    paddingChar(old.paddingChar),
    maxLength(old.maxLength),
    realLength(old.realLength),
    fStringMode(old.fStringMode),
    nonSignificantChars(old.nonSignificantChars)
{
}


DcmByteString::~DcmByteString()
{
}


DcmByteString &DcmByteString::operator=(const DcmByteString &obj)
{
    if (this != &obj)
    {
        DcmElement::operator=(obj);

        /* copy member variables */
        paddingChar = obj.paddingChar;
        maxLength = obj.maxLength;
        realLength = obj.realLength;
        fStringMode = obj.fStringMode;
        nonSignificantChars = obj.nonSignificantChars;
    }
    return *this;
}


OFCondition DcmByteString::copyFrom(const DcmObject& rhs)
{
    if (this != &rhs)
    {
        if (rhs.ident() != ident()) return EC_IllegalCall;
        *this = OFstatic_cast(const DcmByteString &, rhs);
    }
    return EC_Normal;
}


// ********************************


DcmEVR DcmByteString::ident() const
{
    /* valid type identifier is set by derived classes */
    return EVR_UNKNOWN;
}


unsigned long DcmByteString::getVM()
{
    char *str = NULL;
    Uint32 len = 0;
    /* get stored string value */
    getString(str, len);
    /* and determine the VM */
    return DcmElement::determineVM(str, len);
}


OFCondition DcmByteString::clear()
{
    /* call inherited method */
    errorFlag = DcmElement::clear();
    /* set string representation to unknown */
    fStringMode = DCM_UnknownString;
    realLength = 0;
    return errorFlag;
}


Uint32 DcmByteString::getRealLength()
{
    /* convert string to internal representation (if required) */
    if (fStringMode != DCM_MachineString)
    {
        /* strips non-significant trailing spaces (padding) and determines 'realLength' */
        makeMachineByteString();
    }
    /* string length of the internal representation */
    return realLength;
}


Uint32 DcmByteString::getLength(const E_TransferSyntax /*xfer*/,
                                const E_EncodingType /*enctype*/)
{
    /* convert string to DICOM representation, i.e. add padding if required */
    makeDicomByteString();
    /* DICOM value length is always an even number */
    return getLengthField();
}


// ********************************

void DcmByteString::print(STD_NAMESPACE ostream& out,
                          const size_t flags,
                          const int level,
                          const char * /*pixelFileName*/,
                          size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get string data */
        char *stringVal = NULL;
        Uint32 stringLen = 0;
        getString(stringVal, stringLen);
        if ((stringVal != NULL) && (stringLen > 0))
        {
            /* print line start with tag and VR */
            printInfoLineStart(out, flags, level);
            out << '[';

            OFString outString;
            /* do not create more output than actually needed */
            const size_t outStrLen = (flags & DCMTypes::PF_shortenLongTagValues) ? DCM_OptPrintLineLength : 0 /* all characters */;
            /* check whether string has to be converted to markup or octal representation */
            if (flags & DCMTypes::PF_convertToMarkup)
            {
                OFString inString(stringVal, stringLen);
                OFStandard::convertToMarkupString(inString, outString, OFTrue, OFStandard::MM_XML, OFFalse, outStrLen);
            }
            else if (flags & DCMTypes::PF_convertToOctalNumbers)
            {
                OFString inString(stringVal, stringLen);
                OFStandard::convertToOctalString(inString, outString, outStrLen);
            } else {
                /* check whether we need the full string or the prefix only */
                if ((outStrLen == 0) || (outStrLen > stringLen))
                    outString.assign(stringVal, stringLen);
                else
                    outString.assign(stringVal, outStrLen);
            }

            unsigned long printedLength = outString.length() + 2 /* for enclosing brackets */;

            /* check whether full value text should be printed */
            if ((flags & DCMTypes::PF_shortenLongTagValues) && (printedLength > DCM_OptPrintLineLength))
            {
                /* truncate value text and append "..." */
                outString.erase(DCM_OptPrintLineLength - 4);
                out << outString << "...";
                printedLength = DCM_OptPrintLineLength;
            } else
                out << outString << ']';

            /* print line end with length, VM and tag name */
            printInfoLineEnd(out, flags, printedLength);
        } else
            printInfoLine(out, flags, level, "(no value available)");
    } else
        printInfoLine(out, flags, level, "(not loaded)");
}


// ********************************


OFCondition DcmByteString::write(DcmOutputStream &outStream,
                                 const E_TransferSyntax oxfer,
                                 const E_EncodingType enctype,
                                 DcmWriteCache *wcache)
{
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* convert string value to DICOM representation and call inherited method */
        if (getTransferState() == ERW_init)
            makeDicomByteString();

        errorFlag = DcmElement::write(outStream, oxfer, enctype, wcache);
    }
    return errorFlag;
}


OFCondition DcmByteString::writeSignatureFormat(DcmOutputStream &outStream,
                                                const E_TransferSyntax oxfer,
                                                const E_EncodingType enctype,
                                                DcmWriteCache *wcache)
{
    if (getTransferState() == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* convert string value to DICOM representation and call inherited method */
        if (getTransferState() == ERW_init)
            makeDicomByteString();
        errorFlag = DcmElement::writeSignatureFormat(outStream, oxfer, enctype, wcache);
    }
    return errorFlag;
}


// ********************************


OFCondition DcmByteString::getOFString(OFString &stringVal,
                                       const unsigned long pos,
                                       OFBool /*normalize*/)
{
    /* check given string position index */
    if (pos >= getVM())
    {
        /* treat an empty string as a special case */
        if (pos == 0)
        {
            errorFlag = EC_Normal;
            stringVal.clear();
        } else
            errorFlag = EC_IllegalParameter;
    } else {
        /* get string data */
        char *str = NULL;
        Uint32 len = 0;
        errorFlag = getString(str, len);
        /* check whether string value is present */
        if ((str != NULL) && (len > 0))
        {
            /* extract specified string component */
            errorFlag = getStringPart(stringVal, str, len, pos);
        } else
            stringVal.clear();
    }
    return errorFlag;
}


OFCondition DcmByteString::getOFStringArray(OFString &stringVal,
                                            OFBool normalize)
{
    /* check whether time-consuming normalization is really needed */
    if (normalize)
        errorFlag = DcmElement::getOFStringArray(stringVal, normalize);
    else
        errorFlag = getStringValue(stringVal);
    return errorFlag;
}


OFCondition DcmByteString::getStringValue(OFString &stringVal)
{
    char *str = NULL;
    Uint32 len = 0;
    errorFlag = getString(str, len);
    /* check whether string value is present */
    if ((str != NULL) && (len > 0))
        stringVal.assign(str, len);
    else
        stringVal.clear();
    return errorFlag;
}


OFCondition DcmByteString::getString(char *&stringVal)
{
    errorFlag = EC_Normal;
    /* get string data */
    stringVal = OFstatic_cast(char *, getValue());
    /* convert to internal string representation (without padding) if required */
    if ((stringVal != NULL) && (fStringMode != DCM_MachineString))
        makeMachineByteString();
    return errorFlag;
}


OFCondition DcmByteString::getString(char *&stringVal,
                                     Uint32 &stringLen)
{
    /* get string data */
    errorFlag = getString(stringVal);
    /* return the real length of the value */
    stringLen = realLength;
    return errorFlag;
}


// ********************************


OFCondition DcmByteString::putString(const char *stringVal)
{
    /* determine length of the string value */
    const Uint32 stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, stringLen);
}


OFCondition DcmByteString::putString(const char *stringVal,
                                     const Uint32 stringLen)
{
    errorFlag = EC_Normal;
    /* check for an empty string parameter */
    if ((stringVal != NULL) && (stringLen > 0))
        putValue(stringVal, stringLen);
    else
        putValue(NULL, 0);
    /* make sure that extra padding is removed from the string */
    fStringMode = DCM_UnknownString;
    makeMachineByteString(stringLen);
    return errorFlag;
}


// ********************************


OFCondition DcmByteString::makeDicomByteString()
{
    /* get string data */
    char *value = NULL;
    errorFlag = getString(value);
    if (value != NULL)
    {
        /* check for odd length */
        if (realLength & 1)
        {
            /* if so add a padding character */
            setLengthField(realLength + 1);
            value[realLength] = paddingChar;
        } else if (realLength < getLengthField())
            setLengthField(realLength);
        /* terminate string (removes additional trailing padding characters) */
        value[getLengthField()] = '\0';
    }
    /* current string representation is now the DICOM one */
    fStringMode = DCM_DicomString;
    return errorFlag;
}


OFCondition DcmByteString::makeMachineByteString(const Uint32 length)
{
    errorFlag = EC_Normal;
    /* get string data */
    char *value = OFstatic_cast(char *, getValue());
    if (value != NULL)
    {
        /* check whether string representation is not the internal one */
        if (fStringMode != DCM_MachineString)
        {
            /* determine initial string length */
            realLength = (length == 0) ? getLengthField() : length;
            /* remove all trailing spaces if automatic input data correction is enabled */
            if (dcmEnableAutomaticInputDataCorrection.get())
            {
                /*
                ** This code removes extra padding characters at the end of a ByteString.
                ** Trailing padding can cause problems when comparing strings.  This kind
                ** of padding is non-significant for all string-based value representations.
                */
                if (realLength > 0)
                {
                    size_t i = OFstatic_cast(size_t, realLength);
                    while ((i > 0) && (value[i - 1] == paddingChar))
                        value[--i] = '\0';
                    realLength = OFstatic_cast(Uint32, i);
                }
            }
        }
    } else
        realLength = 0;
    /* current string representation is now the internal one */
    fStringMode = DCM_MachineString;
    return errorFlag;
}


// ********************************


Uint8 *DcmByteString::newValueField()
{
    Uint8 *value = NULL;
    Uint32 lengthField = getLengthField();
    /* check for odd length (in case of a protocol error) */
    if (lengthField & 1)
    {
        if (lengthField == DCM_UndefinedLength)
        {
            /* Print an error message when private attribute states to have an odd length
             * equal to the maximum length, because we are not able then to make this value even (+1)
             * which would an overflow on some systems as well as being illegal in DICOM
             */
            DCMDATA_WARN("DcmByteString: Element " << getTagName() << " " << getTag()
                << " has odd maximum length (" << DCM_UndefinedLength << ") and therefore is not loaded");
            errorFlag = EC_CorruptedData;
            return NULL;
        }
        /* allocate space for extra padding character (required for the DICOM representation of the string) */
#ifdef HAVE_STD__NOTHROW
        // we want to use a non-throwing new here if available.
        // If the allocation fails, we report an EC_MemoryExhausted error
        // back to the caller.
        value = new (std::nothrow) Uint8[lengthField + 2];
#else
        value = new Uint8[lengthField + 2];
#endif

        /* terminate string after real length */
        if (value != NULL)
            value[lengthField] = 0;
        /* enforce old (pre DCMTK 3.5.2) behaviour? */
        if (!dcmAcceptOddAttributeLength.get())
        {
            /* make length even */
            lengthField++;
            setLengthField(lengthField);
        }
    } else {
        /* length is even, but we need an extra byte for the terminating 0 byte */
#ifdef HAVE_STD__NOTHROW
        // we want to use a non-throwing new here if available.
        // If the allocation fails, we report an EC_MemoryExhausted error
        // back to the caller.
        value = new (std::nothrow) Uint8[lengthField + 1];
#else
        value = new Uint8[lengthField + 1];
#endif
    }
    /* make sure that the string is properly terminated by a 0 byte */
    if (value != NULL)
        value[lengthField] = 0;
    else
        errorFlag = EC_MemoryExhausted;
    return value;
}


// ********************************


void DcmByteString::postLoadValue()
{
    /* initially, after loading an attribute the string mode is unknown */
    fStringMode = DCM_UnknownString;
    /* correct value length if automatic input data correction is enabled */
    if (dcmEnableAutomaticInputDataCorrection.get())
    {
        /* check for odd length */
        if (getLengthField() & 1)
        {
            // newValueField always allocates an even number of bytes and sets
            // the pad byte to zero, so we can safely increase Length here.
            setLengthField(getLengthField() + 1);
        }
    }
}


// ********************************


OFCondition DcmByteString::verify(const OFBool autocorrect)
{
    char *str = NULL;
    Uint32 len = 0;
    /* get string data */
    errorFlag = getString(str, len);
    /* check for non-empty string */
    if ((str != NULL) && (len > 0))
    {
        /* check whether there is anything to verify at all */
        if (maxLength != DCM_UndefinedLength)
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
                size_t posEnd = value.find('\\', posStart);
                const size_t fieldLen = (posEnd == OFString_npos) ? value.length() - posStart : posEnd - posStart;
                /* check size limit for each string component */
                if (fieldLen > maxLength)
                {
                    DCMDATA_DEBUG("DcmByteString::verify() Maximum length violated in element "
                        << getTagName() << " " << getTag() << " value " << vmNum << ": " << fieldLen
                        << " bytes found but only " << maxLength << " bytes allowed");
                    errorFlag = EC_MaximumLengthViolated;
                    if (autocorrect)
                    {
                        const size_t excess = fieldLen - maxLength;
                        DCMDATA_DEBUG("DcmByteString::verify() Removing " << excess
                            << " bytes from the end of value " << vmNum);
                        /* erase excessive part of the string component */
                        value.erase(posStart + maxLength, excess);
                        /* correct the position end marker */
                        posEnd -= excess;
                    }
                }
                posStart = (posEnd == OFString_npos) ? posEnd : posEnd + 1;
            }
            /* replace current string value if auto correction is enabled */
            if (autocorrect && errorFlag.bad())
            {
                putOFStringArray(value);
                /* the above method also sets 'errorFlag', so we need to assign the error code again */
                errorFlag = EC_MaximumLengthViolated;
            }
        }
    }
    /* report a debug message if an error occurred */
    if (errorFlag.bad())
    {
        DCMDATA_WARN("DcmByteString: One or more illegal values in element "
            << getTagName() << " " << getTag() << " with VM=" << getVM());
    }
    return errorFlag;
}


OFBool DcmByteString::containsExtendedCharacters(const OFBool checkAllStrings)
{
    /* only check if parameter is true since derived VRs are not affected
       by the attribute SpecificCharacterSet (0008,0005) */
    if (checkAllStrings)
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
    }
    return OFFalse;
}


OFBool DcmByteString::isAffectedBySpecificCharacterSet() const
{
    return OFFalse;
}


// ********************************


OFBool DcmByteString::isEmpty(const OFBool normalize)
{
    OFBool result = OFFalse;
    if (normalize && !nonSignificantChars.empty())
    {
        OFString value;
        DcmByteString::getStringValue(value);
        /* check whether string value consists of non-significant characters only */
        result = (value.find_first_not_of(nonSignificantChars) == OFString_npos);
    } else
        result = DcmObject::isEmpty(normalize);
    return result;
}


// ********************************


// global function for normalizing a DICOM string
void normalizeString(OFString &string,
                     const OFBool multiPart,
                     const OFBool leading,
                     const OFBool trailing,
                     const char paddingChar)
{
    /* check for non-empty string */
    if (!string.empty())
    {
        size_t partindex = 0;
        size_t offset = 0;
        size_t len = string.length();
        while (partindex < len)
        {
            // remove leading spaces in every part of the string
            if (leading)
            {
                offset = 0;
                while ((partindex + offset < len) && (string[partindex + offset] == paddingChar))
                    offset++;
                if (offset > 0)
                    string.erase(partindex, offset);
            }
            len = string.length();
            // compute begin to the next separator index!
            if (multiPart)
            {
                partindex = string.find('\\', partindex);
                if (partindex == OFString_npos)
                    partindex = len;
            } else
                partindex = len;
            // remove trailing spaces in every part of the string
            if (trailing && partindex)
            {
                offset = partindex - 1;
                while ((offset > 0) && (string[offset] == paddingChar))
                    offset--;
                if (offset != partindex - 1)
                {
                    if (string[offset] == ' ')
                    {
                        string.erase(offset, partindex - offset);
                        partindex = offset;
                    } else {
                        string.erase(offset + 1, partindex - offset - 1);
                        partindex = offset + 1;
                    }
                }
            }
            len = string.length();
            if (partindex != len)
                ++partindex;
        }
    }
}


// ********************************


OFCondition DcmByteString::checkStringValue(const OFString &value,
                                            const OFString &vm,
                                            const OFString &vr,
                                            const int vrID,
                                            const size_t maxLen,
                                            const OFString &charset)
{
    OFCondition result = EC_Normal;
    const size_t valLen = value.length();
    if (valLen > 0)
    {
        /* do we need to search for value components at all? */
        if (vm.empty())
        {
            /* check value length (if a maximum is specified) */
            if ((maxLen > 0) && (value.length() > maxLen))
                result = EC_MaximumLengthViolated;
            else if (dcmEnableVRCheckerForStringValues.get())
            {
                /* currently, the VR checker only supports ASCII and Latin-1 */
                if (charset.empty() || (charset == "ISO_IR 6") || (charset == "ISO_IR 100"))
                {
                    /* check value representation */
                    if (DcmElement::scanValue(value, vr) != vrID)
                        result = EC_ValueRepresentationViolated;
                }
            }
        } else {
            size_t posStart = 0;
            unsigned long vmNum = 0;
            /* iterate over all value components */
            while (posStart != OFString_npos)
            {
                ++vmNum;
                /* search for next component separator */
                const size_t posEnd = value.find('\\', posStart);
                const size_t length = (posEnd == OFString_npos) ? valLen - posStart : posEnd - posStart;
                /* check length of current value component */
                if ((maxLen > 0) && (length > maxLen))
                {
                    result = EC_MaximumLengthViolated;
                    break;
                }
                else if (dcmEnableVRCheckerForStringValues.get())
                {
                    /* currently, the VR checker only supports ASCII and Latin-1 */
                    if (charset.empty() || (charset == "ISO_IR 6") || (charset == "ISO_IR 100"))
                    {
                        /* check value representation */
                        if (DcmElement::scanValue(value, vr, posStart, length) != vrID)
                        {
                            result = EC_ValueRepresentationViolated;
                            break;
                        }
                    }
                }
                posStart = (posEnd == OFString_npos) ? posEnd : posEnd + 1;
            }
            if (result.good())
            {
                /* check value multiplicity */
                result = DcmElement::checkVM(vmNum, vm);
            }
        }
    }
    return result;
}
