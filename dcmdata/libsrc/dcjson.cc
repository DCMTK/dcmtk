/*
 *
 *  Copyright (C) 2016-2023, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  Module:  dcmdata
 *
 *  Author:  Sebastian Grallert
 *
 *  Purpose: Providing basic JSON formatting functionalities
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/ofstd/ofstring.h"

#include <cassert>

void DcmJsonFormat::escapeControlCharacters(STD_NAMESPACE ostream &out, const OFString &value)
{
    // escapes all forbidden control characters in JSON
    for (size_t i = 0; i < value.size(); ++i)
    {
        const char c = value.at(i);
        switch (c)
        {
        case '\\':
            out << "\\\\";
            break;
        case '"':
            out << "\\\"";
            break;
        case '\b':
            out << "\\b";
            break;
        case '\n':
            out << "\\n";
            break;
        case '\r':
            out << "\\r";
            break;
        case '\t':
            out << "\\t";
            break;
        case '\f':
            out << "\\f";
            break;
        default:
            //escapes all other control characters
            if (c >= '\0' && c < ' ')
            {
                out << "\\u" << STD_NAMESPACE hex
                    << STD_NAMESPACE setw(4) << STD_NAMESPACE setfill('0')
                    << OFstatic_cast(unsigned, OFstatic_cast(unsigned char, c));
            }
            else
            {
                out << c;
            }
        }
    }
}

// Formats the number to JSON standard as DecimalString
void DcmJsonFormat::normalizeDecimalString(OFString &value)
{
    // remove all plus characters that may occur in the string.
    // These are permitted in DICOM but not in JSON.
    size_t pos;
    while (OFString_npos != (pos = value.find('+')))
      value.erase(pos,1);

    // check if the first character is a minus sign.
    // if so, remove it and set "minus" to true
    OFBool minus = OFFalse;

    if (value.length() > 0 && value[0] == '-')
    {
        value = value.substr(1);
        minus = OFTrue;
    }

    // remove leading zeroes from the significand
    pos = value.find_first_not_of("0");

    if (pos == OFString_npos)
        value = "0";
    else
    {
        // make sure that the significand does not start
        // with a period, in this case, prepend a zero
        if (value[pos] == '.')
            value = '0' + value.substr(pos);
        else
            value = value.substr(pos);
    }

    // add back any minus sign
    if (minus)
        value = '-' + value;

    // make sure that the significand does not end
    // with a period
    if (OFString_npos != (pos = value.find('.')))
    {
      if (pos == value.length() -1)
      {
        // number ends with a period. Add a zero
        value.append("0");
      }
      else if ((value[pos+1] < '0') || (value[pos+1] > '9'))
      {
        // no digit after period. Insert a zero
        value.insert(pos+1, "0");
      }
    }

}

// Formats the number to JSON standard as IntegerString
void DcmJsonFormat::normalizeIntegerString(OFString &value)
{
    // remove all plus characters that may occur in the string.
    // These are permitted in DICOM but not in Json.
    size_t pos;
    while (OFString_npos != (pos = value.find('+')))
      value.erase(pos,1);

    OFBool minus = OFFalse;

    if (value.length() > 0 && value[0] == '-')
    {
        value = value.substr(1);
        minus = OFTrue;
    }

    pos = value.find_first_not_of("0");

    if (pos == OFString_npos)
        value = "0";
    else
        value = value.substr(pos);

    if (minus)
        value = '-' + value;
}

// Print a string in JSON format
void DcmJsonFormat::printString(STD_NAMESPACE ostream &out,
                                const OFString &value)
{
    out << "\"";
    escapeControlCharacters(out, value);
    out << "\"";
}

// Print a string in JSON format
// Print null if OFString is empty
void DcmJsonFormat::printValueString(STD_NAMESPACE ostream &out,
                                     const OFString &value)
{
    if (!value.empty())
    {
        printString(out, value);
    }
    else
    {
        out << "null";
    }
}

// Print a integer in JSON format
// Print null if OFString is empty
void DcmJsonFormat::printNumberInteger(STD_NAMESPACE ostream &out,
                                       OFString &value)
{
    if (!value.empty())
    {
        normalizeIntegerString(value);
        out << value;
    }
    else
    {
        out << "null";
    }
}

// Print a decimal in JSON format
// Print null if OFString is empty
void DcmJsonFormat::printNumberDecimal(STD_NAMESPACE ostream &out,
                                       OFString &value)
{
    if (!value.empty())
    {
        normalizeDecimalString(value);
        out << value;
    }
    else
    {
        out << "null";
    }
}

// Print the prefix for Value
void DcmJsonFormat::printValuePrefix(STD_NAMESPACE ostream &out)
{
    out << "," << newline() << indent() << "\"Value\":" << space() << "[" << newline();
    out << ++indent();
}

// Print the suffix for Value
void DcmJsonFormat::printValueSuffix(STD_NAMESPACE ostream &out)
{
    out << newline() << --indent() << "]";
}

// Print the prefix for BulkDataURI
void DcmJsonFormat::printBulkDataURIPrefix(STD_NAMESPACE ostream &out)
{
    out << "," << newline() << indent() << "\"BulkDataURI\":" << space();
}

// Print the prefix for InlineBinary
void DcmJsonFormat::printInlineBinaryPrefix(STD_NAMESPACE ostream &out)
{
    out << "," << newline() << indent() << "\"InlineBinary\":" << space();
}

// Print the prefix for array elements in JSON format
void DcmJsonFormat::printNextArrayElementPrefix(STD_NAMESPACE ostream &out)
{
    out << "," << newline() << indent();
}

// Method for holding and determine if BulkDataURI should be printed.
// This also manipulate uri String, if BulkDataURI should be printed.
OFBool DcmJsonFormat::asBulkDataURI(const DcmTagKey& /*tag*/, OFString& /*uri*/)
{
    return OFFalse;
}

//Class for formatted output
DcmJsonFormatPretty::DcmJsonFormatPretty(const OFBool printMetaInfo)
: DcmJsonFormat(printMetaInfo)
, m_IndentionLevel(0)
{

}

void DcmJsonFormatPretty::printIndention(STD_NAMESPACE ostream& out)
{
    for (unsigned ui = 0; ui < m_IndentionLevel; ++ui)
        out << "  ";
}

void DcmJsonFormatPretty::increaseIndention()
{
    ++m_IndentionLevel;
}

void DcmJsonFormatPretty::decreaseIndention()
{
    assert(m_IndentionLevel);
    --m_IndentionLevel;
}

OFString DcmJsonFormatPretty::newline()
{
    return "\n";
}

OFString DcmJsonFormatPretty::space()
{
    return " ";
}


//Class for unformatted output
DcmJsonFormatCompact::DcmJsonFormatCompact(const OFBool printMetaInfo)
: DcmJsonFormat(printMetaInfo)
{

}

void DcmJsonFormatCompact::printIndention(STD_NAMESPACE ostream& /*out*/)
{}

void DcmJsonFormatCompact::increaseIndention()
{}

void DcmJsonFormatCompact::decreaseIndention()
{}

OFString DcmJsonFormatCompact::newline()
{
    return OFString();
}

OFString DcmJsonFormatCompact::space()
{
    return OFString();
}
