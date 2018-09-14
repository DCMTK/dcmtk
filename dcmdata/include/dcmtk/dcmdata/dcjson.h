/*
*
*  Copyright (C) 2017-2018, OFFIS e.V.
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
*  Author:  Sebastian Grallert
*
*  Purpose: Providing basic JSON formatting functionalities
*
*/

#ifndef DCJSON_H
#define DCJSON_H

#include "dcmtk/config/osconfig.h" // make sure OS specific configuration is included first

#include "dcmtk/ofstd/ofdefine.h"
#include "dcmtk/ofstd/ofstring.h"

#include "dcmtk/dcmdata/dctagkey.h"

/** Class for handling JSON format options.
 *  Base class to implement custom formatting.
 *  Purpose:
 *    - individual output formatting
 *    - normalization of DecimalString and IntegerString e.g.\ normalization of leading zeros
 *    - escaping of special JSON control characters
 *    - outputting the correct indention and symbols for "Value", "BulkDataURI", etc.
 *
 *  <h3>Usage Example:</h3>
 *  @code{.cpp}
 *  #include "dcmtk/dcmdata/dcjson.h"
 *  // ...
 *  DcmFileFormat fileformat;
 *  if(fileformat.loadFile("test.dcm").good())
 *  {
 *      // print the DICOM file in JSON format
 *      // using the pretty format (muti-line with indention and other whitespace)
 *      fileformat.writeJson(COUT, DcmJsonFormatPretty(OFTrue));
 *
 *      // using the compact (single line, without unneeded whitespace) format
 *      fileformat.writeJson(COUT, DcmJsonFormatCompact(OFTrue));
 *  }
 *  @endcode
 *  <h3>Implementing a custom formatter:</h3>
 *  @code{.cpp}
 *  struct CustomJsonFormat : DcmJsonFormatPretty
 *  {
 *    CustomJsonFormat(const OFBool printMetaInfo = OFTrue)
 *    : DcmJsonFormatPretty(printMetaInfo)
 *    {
 *
 *    }
 *
 *    OFString OFJsonFormatExample::space()
 *    {
 *      // use tabstops instead of spaces for indention
 *      return "\t";
 *    }
 *  }
 *  @endcode
 */
class DCMTK_DCMDATA_EXPORT DcmJsonFormat
{
public:
    /** A class to create small proxy objects that ease indention handling.
     *  Each Indention object only contains a reference to the DcmJsonFormat object
     *  that created it and its only purpose is to call the respective methods
     *  of that object when one of its overloaded operators is used.
     */
    class Indention
    {
    public:
        /** output current indention to an output stream.
         *  @param out the output stream to use
         *  @param indention the indention to print
         *  @return out
         */
        friend inline STD_NAMESPACE ostream& operator<<(STD_NAMESPACE ostream& out, const Indention& indention)
        {
            indention.printIndention(out);
            return out;
        }

        /** increases current indention.
         *  @return *this
         */
        inline Indention& operator++()
        {
            m_Format.increaseIndention();
            return *this;
        }

        /** decreases current indention
         *  @return *this
         */
        inline Indention& operator--()
        {
            m_Format.decreaseIndention();
            return *this;
        }

    private:
        /// allow DcmJsonFormat to use this class' private members
        friend class DcmJsonFormat;

        /// private constructor, used by DcmJsonFormat
        inline Indention(DcmJsonFormat& format) : m_Format(format)
        {

        }

        /// prints the current indention using the parent formatter
        inline void printIndention(STD_NAMESPACE ostream& out) const
        {
            m_Format.printIndention(out);
        }

        /// reference to the parent formatter object
        DcmJsonFormat& m_Format;
    };

    /** Escapes all forbidden control characters in JSON
     *  @param out output stream to which the escaped String is written
     *  @param value String that should be escaped
     */
    static void escapeControlCharacters(STD_NAMESPACE ostream &out, OFString const &value);

    /** Normalize Decimal String to specific JSON format.
     *  remove leading zeros, except before dot.
     *  @b Example:
     *  @code{.txt}
     *  00.123 --> 0.123
     *  023.12 --> 23.12
     *  -01.00 --> -1.00
     *    0200 --> 200
     *     .12 --> 0.12
     *   000.1 --> 0.1
     *  @endcode
     *  @param value String that should be normalize
     */
    static void normalizeDecimalString(OFString &value);

    /** Normalize Integer String to specific JSON format.
     *  remove leading zeros, except before dot.
     *  @b Example:
     *  @code{.txt}
     *   000 --> 0
     *   023 --> 23
     *   -01 --> -1
     *  0200 --> 200
     *  @endcode
     *  @param value String that should be normalize
     */
    static void normalizeIntegerString(OFString &value);

    /** Prints either null if empty or the string value
     *  (with all illegal characters escaped).
     *  @param out output stream to which the Value prefix is written
     *  @param value String that should be printed
     */
    static void printString(STD_NAMESPACE ostream &out,
                            const OFString &value);

    /** Prints either null if empty or a quoted string
     *  (with leading and ending quotation marks and all
     *  illegal characters escaped).
     *  @param out output stream to which the Value prefix is written
     *  @param value String that should be printed
     */
    static void printValueString(STD_NAMESPACE ostream &out,
                                 const OFString &value);

    /** Print either null if empty or a Number as normalized IntegerString
     *  @param out output stream to which the Value prefix is written
     *  @param value String that should be printed
     */
    static void printNumberInteger(STD_NAMESPACE ostream &out,
                                   OFString &value);

    /** Print either null if empty or a Number as normalized IntegerDecimal
     *  @param out output stream to which the Value prefix is written
     *  @param value String that should be printed
     */
    static void printNumberDecimal(STD_NAMESPACE ostream &out,
                                   OFString &value);

    /** Constructor
     *  @param printMetaInfo parameter that defines if meta information should be written
     */
    inline DcmJsonFormat(const OFBool printMetaInfo)
    : printMetaheaderInformation(printMetaInfo)
    {

    }

    /** Virtual destructor, does nothing
     */
    virtual ~DcmJsonFormat() {}

    /** Method to return line break(s)
     *  @return line break(s).
     */
    virtual OFString newline() = 0;

    /** Method to return whitespace(s)
     *  @return whitespace(s).
     */
    virtual OFString space() = 0;

    /** Method to return an indention proxy object for increasing, decreasing or printing indention
     *  @return an indention proxy object.
     */
    inline Indention indent()
    {
        return Indention(*this);
    }

    /** Check if an attribute should be exported as BulkDataURI.
     *  Override this function to implement bulk data URI output.
     *  @param tag the tag of the attribute being printed, for letting
     *    the implementation decide how to handle it.
     *  @param uri the resulting URI to output.
     *  @return OFTrue if yes, OFFalse if no.
     *  @details
     *  <h3>Usage Example:</h3>
     *  @code{.cpp}
     *  struct BulkDataURIJsonFormat : DcmJsonFormatPretty
     *  {
     *    CustomJsonFormat(const OFBool printMetaInfo = OFTrue,
     *                     ... bulkDataURIDatabase)
     *    : DcmJsonFormatPretty(printMetaInfo)
     *    , TheDatabase(bulkDataURIDatabase)
     *    {
     *
     *    }
     *
     *    virtual OFBool asBulkDataURI(const DcmTagKey& tag, OFString& uri)
     *    {
     *      ... result = TheDatabase.findBulkDataFor(tag);
     *      if (result.found())
     *      {
     *        uri = result.uri();
     *        return OFTrue;
     *      }
     *      return OFFalse;
     *    }
     *
     *    ... TheDatabase;
     *  }
     *  @endcode
     */
    virtual OFBool asBulkDataURI(const DcmTagKey& tag, OFString& uri);

    /** Print the Prefix which for JSON Values needed
     *  with indention and newlines as in the format Variable given.
     *  @b Example:
     *  @code{.txt}
     *  ,"Value":[
     *  @endcode
     *  @param out output stream to which the Value prefix is written
     */
    virtual void printValuePrefix(STD_NAMESPACE ostream &out);

    /** Print the Suffix which for JSON Values needed
     *  with indention and newlines as in the format Variable given.
     *  @b Example:
     *  @code{.txt}
     *  ]\n
     *  @endcode
     *  @param out output stream to which the Value prefix is written
     */
    virtual void printValueSuffix(STD_NAMESPACE ostream &out);

    /** Print the Prefix which for JSON BulkDataURI needed
     *  with indention and newlines as in the format Variable given.
     *  @b Example:
     *  @code{.txt}
     *  ,"BulkDataURI":
     *  @endcode
     *  @param out output stream to which the Value prefix is written
     */
    virtual void printBulkDataURIPrefix(STD_NAMESPACE ostream &out);

    /** Print the Prefix which for JSON InlineBinary needed
     *  with indention and newlines as the format specifies.
     *  @b Example:
     *  @code{.txt}
     *  ,"InlineBinary":
     *  @endcode
     *  @param out output stream to which the Value prefix is written
     */
    virtual void printInlineBinaryPrefix(STD_NAMESPACE ostream &out);

    /** Print the prefix for array elements (except the first one), with
     *  indention and newlines as the format specifies.
     *  @b Example:
     *  @code{.txt}
     *      Example,\n
     *      Example...
     *  @endcode
     *  @param out output stream to which the Value prefix is written
     */
    virtual void printNextArrayElementPrefix(STD_NAMESPACE ostream &out);

    /** Option that defines if metaheader information should be printed.
     */
    const OFBool printMetaheaderInformation;

protected:
    /** Indent to the specific level.
     *  @param out output stream to which the indention is written.
     */
    virtual void printIndention(STD_NAMESPACE ostream& out) = 0;

    /** Used for increasing the indention level.
     */
    virtual void increaseIndention() = 0;

    /** Used for decreasing the indention level.
     */
    virtual void decreaseIndention() = 0;
};


/** Subclass for handling JSON formatted output.
 *  Standard class for formatted output.
 */
class DCMTK_DCMDATA_EXPORT DcmJsonFormatPretty : public DcmJsonFormat
{
private:
    /** Variable for the indentenlevel of DcmJsonFormat
     */
    unsigned m_IndentionLevel;

public:
    /** DcmJsonFormatPretty constructor
     *  @param printMetaInfo Enable/Disable including Metaheader in the output
     */
    explicit DcmJsonFormatPretty(const OFBool printMetaInfo = OFTrue);

    /** Indent to the specific level.
     *  @param out output stream to which the indention is written.
     */
    void printIndention(STD_NAMESPACE ostream& out);

    /** Increase the indention level.
     */
    void increaseIndention();

    /** Decrease the indention level.
     */
    void decreaseIndention();

    /** Print a newline
     *  @returns a newline
     */
    OFString newline();

    /** Print a space
     *  @returns a space
     */
    OFString space();
};

/** Subclass for handling JSON formatted output.
 *  Standard class for non-formatted output.
 */
class DCMTK_DCMDATA_EXPORT DcmJsonFormatCompact : public DcmJsonFormat
{
public:
    /** DcmJsonFormatCompact constructor
     *  @param printMetaInfo Enable/Disable including Metaheader in the output
     */
    explicit DcmJsonFormatCompact(const OFBool printMetaInfo = OFTrue);

    /** Does nothing.
     *  @param out output stream to which the indention is written.
     */
    void printIndention(STD_NAMESPACE ostream& out);

    /** Does nothing.
     */
    void increaseIndention();

    /** Does nothing.
     */
    void decreaseIndention();

    /** Does nothing.
     *  @returns a empty String.
     */
    OFString newline();

    /** Does nothing.
     *  @returns a empty String.
     */
    OFString space();
};

#endif /* DCJSON_H */
