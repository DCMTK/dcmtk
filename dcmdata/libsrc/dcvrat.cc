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
 *  Purpose: Implementation of class DcmAttributeTag
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcvrat.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmAttributeTag::DcmAttributeTag(const DcmTag &tag,
                                 const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmAttributeTag::DcmAttributeTag(const DcmAttributeTag &old)
  : DcmElement(old)
{
}


DcmAttributeTag::~DcmAttributeTag()
{
}


DcmAttributeTag &DcmAttributeTag::operator=(const DcmAttributeTag &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


OFCondition DcmAttributeTag::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmAttributeTag &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmAttributeTag::ident() const
{
    return EVR_AT;
}


OFCondition DcmAttributeTag::checkValue(const OFString &vm,
                                       const OFBool /*oldFormat*/)
{
    /* check VM only, further checks on the attribute tags could be added later */
    return DcmElement::checkVM(getVM(), vm);
}


unsigned long DcmAttributeTag::getVM()
{
    /* attribute tags store pairs of 16 bit values */
    return OFstatic_cast(unsigned long, getLengthField() / (2 * sizeof(Uint16)));
}


// ********************************


void DcmAttributeTag::print(STD_NAMESPACE ostream& out,
                            const size_t flags,
                            const int level,
                            const char * /*pixelFileName*/,
                            size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get unsigned integer data */
        Uint16 *uintVals;
        errorFlag = getUint16Array(uintVals);
        const unsigned long count = getVM();
        if ((uintVals != NULL) && (count > 0))
        {
            /* determine number of values to be printed */
            unsigned long expectedLength = count * (11 + 1) - 1;
            const unsigned long printCount =
                ((expectedLength > DCM_OptPrintLineLength) && (flags & DCMTypes::PF_shortenLongTagValues)) ?
                (DCM_OptPrintLineLength - 3 /* for "..." */ + 1) / (11 /* (gggg,eeee) */ + 1 /* for "\" */) : count;
            unsigned long printedLength = printCount * (11 + 1) - 1;
            /* print line start with tag and VR */
            printInfoLineStart(out, flags, level);
            /* print multiple values */
            if (printCount > 0)
            {
                out << STD_NAMESPACE hex << STD_NAMESPACE setfill('0');
                /* print tag values (group,element) in hex mode */
                out << '(' << STD_NAMESPACE setw(4) << (*(uintVals++));
                out << ',' << STD_NAMESPACE setw(4) << (*(uintVals++)) << ')';
                for (unsigned long i = 1; i < printCount; i++)
                {
                    out << "\\" << '(' << STD_NAMESPACE setw(4) << (*(uintVals++));
                    out << ',' << STD_NAMESPACE setw(4) << (*(uintVals++)) << ')';
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
        } else
            printInfoLine(out, flags, level, "(no value available)");
    } else
        printInfoLine(out, flags, level, "(not loaded)");
}


// ********************************


OFCondition DcmAttributeTag::writeXML(STD_NAMESPACE ostream &out,
                                      const size_t flags)
{
    /* AT requires special handling in the Native DICOM Model format */
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* write normal XML start tag */
        DcmElement::writeXMLStartTag(out, flags);
        /* get unsigned integer data */
        Uint16 *uintVals;
        getUint16Array(uintVals);
        const unsigned long vm = getVM();
        /* check for empty/invalid value */
        if ((uintVals != NULL) && (vm > 0))
        {
            out << STD_NAMESPACE uppercase << STD_NAMESPACE setfill('0');
            /* print tag values "ggggeeee" in hex mode (upper case!) */
            for (unsigned long valNo = 0; valNo < vm; valNo++)
            {
                out << "<Value number=\"" << (valNo + 1) << "\">";
                out << STD_NAMESPACE hex << STD_NAMESPACE setw(4) << (*(uintVals++));
                out << STD_NAMESPACE setw(4) << (*(uintVals++)) << STD_NAMESPACE dec;
                out << "</Value>" << OFendl;
            }
            /* reset i/o manipulators */
            out << STD_NAMESPACE nouppercase << STD_NAMESPACE setfill(' ');
        }
        /* write normal XML end tag */
        DcmElement::writeXMLEndTag(out, flags);
        /* always report success */
        return EC_Normal;
    } else  {
        /* DCMTK-specific format does not require anything special */
        return DcmElement::writeXML(out, flags);
    }
}


// ********************************


OFCondition DcmAttributeTag::getTagVal(DcmTagKey &tagVal,
                                       const unsigned long pos)
{
    /* get unsigned integer data */
    Uint16 *uintValues;
    errorFlag = getUint16Array(uintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (uintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getVM())
            errorFlag = EC_IllegalParameter;
        else
            tagVal.set(uintValues[2 * pos] /*group*/, uintValues[2 * pos + 1] /*element*/);
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        tagVal = DcmTagKey();
    return errorFlag;
}


OFCondition DcmAttributeTag::getUint16Array(Uint16 *&uintVals)
{
    uintVals = OFstatic_cast(Uint16 *, getValue());
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::getOFString(OFString &stringVal,
                                         const unsigned long pos,
                                         OFBool /*normalize*/)
{
    DcmTagKey tagVal;
    /* get the specified tag value */
    errorFlag = getTagVal(tagVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
        sprintf(buffer, "(%4.4x,%4.4x)", tagVal.getGroup(), tagVal.getElement());
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::putTagVal(const DcmTagKey &tagVal,
                                       const unsigned long pos)
{
    /* create tag data */
    Uint16 uintVals[2];
    uintVals[0] = tagVal.getGroup();
    uintVals[1] = tagVal.getElement();
    /* change element value */
    errorFlag = changeValue(uintVals, 2 * sizeof(Uint16) * OFstatic_cast(Uint32, pos), 2 * sizeof(Uint16));
    return errorFlag;
}


OFCondition DcmAttributeTag::putUint16Array(const Uint16 *uintVals,
                                            const unsigned long numUints)
{
    errorFlag = EC_Normal;
    if (numUints > 0)
    {
        /* check for valid data */
        if (uintVals != NULL)
            errorFlag = putValue(uintVals, 2 * sizeof(Uint16) * Uint32(numUints));
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::putString(const char *stringVal)
{
    /* determine length of the string value */
    const Uint32 stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, stringLen);
}


OFCondition DcmAttributeTag::putString(const char *stringVal,
                                       const Uint32 stringLen)
{
    errorFlag = EC_Normal;
    /* determine VM of the string */
    unsigned long vm = DcmElement::determineVM(stringVal, stringLen);
    if (vm > 0)
    {
        Uint16 * field = new Uint16[2 * vm];
        OFString value;
        size_t pos = 0;
        /* retrieve attribute tag data from character string */
        for (unsigned long i = 0; (i < 2 * vm) && errorFlag.good(); i += 2)
        {
            /* get specified value from multi-valued string */
            pos = DcmElement::getValueFromString(stringVal, pos, stringLen, value);
            if (value.empty() || sscanf(value.c_str(), "(%hx,%hx)", &field[i], &field[i + 1]) != 2)
                errorFlag = EC_CorruptedData;
        }
        /* set binary data as the element value */
        if (errorFlag.good())
            errorFlag = putUint16Array(field, vm);
        /* delete temporary buffer */
        delete[] field;
    } else
        putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (getLengthField() % (2 * sizeof(Uint16)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            setLengthField(getLengthField() - (getLengthField() % (2* sizeof(Uint16))));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::checkStringValue(const OFString &value,
                                              const OFString &vm)
{
    return DcmElement::checkVM(DcmElement::determineVM(value.c_str(), value.length()), vm);
}
