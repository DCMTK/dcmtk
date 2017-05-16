/*
 *
 *  Copyright (C) 2011-2017, OFFIS e.V.
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
 *  Purpose: Class for supporting the Specific Character Set attribute
 *
 */


#ifndef DCSPCHRS_H
#define DCSPCHRS_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofchrenc.h"
#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/dcmdata/dcdefine.h"


// forward declaration
class DcmItem;


/** A class for managing and converting between different DICOM character sets.
 *  The conversion relies on the OFCharacterEncoding class, which again relies
 *  on an underlying character encoding library (e.g. libiconv or ICU).
 *  @note Please note that a current limitation is that only a single value is
 *    allowed for the destination character set (i.e. no code extensions).  Of
 *    course, for the source character set, also multiple values are supported.
 */
class DCMTK_DCMDATA_EXPORT DcmSpecificCharacterSet
{

  public:

    /** constructor.  Initializes the member variables.
     */
    DcmSpecificCharacterSet();

    /** destructor
     */
    ~DcmSpecificCharacterSet();

    /** clear the internal state.  This also forgets about the currently
     *  selected character sets, so selectCharacterSet() has to be called again
     *  before a string can be converted with convertString().
     */
    void clear();

    /** query whether selectCharacterSet() has successfully been called for this
     *  object, i.e.\ whether convertString() may be called.
     *  @return OFTrue if selectCharacterSet() was successfully called before,
     *    OFFalse if not (or clear() has been called in the meantime).
     */
#ifdef HAVE_CXX11
    explicit
#endif
    operator OFBool() const;

    /** query whether selectCharacterSet() has <b>not</b> been called before,
     *  i.e.\ convertString() would fail.
     *  @return OFTrue if selectCharacterSet() must be called before using
     *    convertString(), OFFalse if it has already been called.
     */
    OFBool operator!() const;

    /** get currently selected source DICOM character set(s).  Please note that
     *  the returned string can contain multiple values (defined terms separated
     *  by a backslash) if code extension techniques are used.  Furthermore,
     *  the returned string is always normalized, i.e. leading and trailing
     *  spaces have been removed.
     *  @return currently selected source DICOM character set(s) or an empty
     *    string if none is selected (identical to ASCII, which is the default)
     */
    const OFString &getSourceCharacterSet() const;

    /** get currently selected destination DICOM character set.  Please note
     *  that the returned string, which contains a defined term, is always
     *  normalized, i.e. leading and trailing spaces have been removed.
     *  @return currently selected destination DICOM character set or an empty
     *    string if none is selected (identical to ASCII, which is the default)
     */
    const OFString &getDestinationCharacterSet() const;

    /** get currently selected destination encoding, i.e.\ the name of the
     *  character set as used by the underlying character encoding library for
     *  the conversion.  If code extension techniques are used to switch between
     *  different character encodings, the main/default encoding is returned.
     *  @return currently selected destination encoding or an empty string if
     *    none is selected
     */
    const OFString &getDestinationEncoding() const;

    /** @copydoc OFCharacterEncoding::getConversionFlags()
     */
    unsigned getConversionFlags() const;

    /** @copydoc OFCharacterEncoding::setConversionFlags()
     */
    OFCondition setConversionFlags(const unsigned flags);

    /** select DICOM character sets for the input and output string, between
     *  which subsequent calls of convertString() convert.  The defined terms
     *  for a particular character set can be found in the DICOM standard, e.g.
     *  "ISO_IR 100" for ISO 8859-1 (Latin 1) or "ISO_IR 192" for Unicode in
     *  UTF-8.  An empty string denotes the default character repertoire, which
     *  is ASCII (7-bit).  If multiple values are given for 'fromCharset'
     *  (separated by a backslash) code extension techniques are used and
     *  escape sequences may be encountered in the source string to switch
     *  between the specified character sets.
     *  @param  fromCharset  name of the source character set(s) used for the
     *                       input string as given in the DICOM attribute
     *                       Specific Character Set (0008,0005).  Leading and
     *                       trailing spaces are removed automatically (if
     *                       present).
     *  @param  toCharset    name of the destination character set used for the
     *                       output string.  Only a single value is permitted
     *                       (no code extensions).  Leading and trailing spaces
     *                       are removed automatically (if present).  The
     *                       default value is "ISO_IR 192" (Unicode in UTF-8).
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition selectCharacterSet(const OFString &fromCharset,
                                   const OFString &toCharset = "ISO_IR 192");

    /** select DICOM character sets for the input and output string, between
     *  which subsequent calls of convertString() convert.  The source
     *  character set is determined from the DICOM element Specific Character
     *  Set (0008,0005) stored in the given dataset/item.  The defined terms
     *  for the destination character set can be found in the DICOM standard,
     *  e.g. "ISO_IR 100" for ISO 8859-1 (Latin 1) or "ISO_IR 192" for Unicode
     *  in UTF-8.  An empty string denotes the default character repertoire,
     *  which is ASCII (7-bit).  If multiple values are found in the Specific
     *  Character Set element of the given 'dataset' (separated by a backslash)
     *  code extension techniques are used and escape sequences may be
     *  encountered in the source string to switch between the specified
     *  character sets.
     *  @param  dataset    DICOM dataset or item from which the source character
     *                     set should be retrieved.  If the data element
     *                     Specific Character Set (0008,0005) is empty or
     *                     missing, the default character set (i.e. ASCII) is
     *                     used.
     *  @param  toCharset  name of the destination character set used for the
     *                     the output string.  Only a single value is permitted
     *                     (no code extensions).  Leading and trailing spaces
     *                     are removed automatically (if present).  The default
     *                     value is "ISO_IR 192" (Unicode in UTF-8).
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition selectCharacterSet(DcmItem &dataset,
                                   const OFString &toCharset = "ISO_IR 192");

    /** convert the given string from the selected source character set(s) to
     *  the selected destination character set.  That means selectCharacterSet()
     *  has to be called prior to this method.
     *  @param  fromString  input string to be converted (using the currently
     *                      selected source character set)
     *  @param  toString    reference to variable where the converted string
     *                      (using the currently selected destination character
     *                      set) is stored
     *  @param  delimiters  optional string of characters that are regarded as
     *                      delimiters, i.e.\ when found the character set is
     *                      switched back to the default.  CR, LF, FF and HT are
     *                      always regarded as delimiters (see DICOM PS 3.5).
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition convertString(const OFString &fromString,
                              OFString &toString,
                              const OFString &delimiters = "");

    /** convert the given string from the selected source character set(s) to
     *  the selected destination character set.  That means selectCharacterSet()
     *  has to be called prior to this method.  Since the length of the input
     *  string has to be specified explicitly, the string can contain more than
     *  one NULL byte.
     *  @param  fromString  input string to be converted (using the currently
     *                      selected character set)
     *  @param  fromLength  length of the input string (number of bytes without
     *                      the trailing NULL byte)
     *  @param  toString    reference to variable where the converted string
     *                      (using the currently selected destination character
     *                      set) is stored
     *  @param  delimiters  optional string of characters that are regarded as
     *                      delimiters, i.e.\ when found the character set is
     *                      switched back to the default.  CR, LF, FF and HT are
     *                      always regarded as delimiters (see DICOM PS 3.5).
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition convertString(const char *fromString,
                              const size_t fromLength,
                              OFString &toString,
                              const OFString &delimiters = "");

    // --- static helper functions ---

    /** check whether the underlying character set conversion library is
     *  available.  If not, no conversion between different character sets will
     *  be possible.
     *  @return OFTrue if the character set conversion is available, OFFalse
     *    otherwise
     */
    static OFBool isConversionAvailable();

    /** count characters in given UTF-8 string and return the resulting number
     *  of so-called "code points".  Please note that invalid UTF-8 encodings
     *  are not handled properly.  ASCII strings (7-bit) are also supported,
     *  although OFString::length() is probably much faster.
     *  @param  utf8String  valid character string with UTF-8 encoding
     *  @return number of characters (code points) in given UTF-8 string
     */
    static size_t countCharactersInUTF8String(const OFString &utf8String);


  protected:

    /** determine the destination character encoding (as used by the underlying
     *  character encoding library) from the given DICOM defined term (specific
     *  character set), and set the member variables accordingly.
     *  @param  toCharset  name of the destination character set used for the
     *                     output string
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition determineDestinationEncoding(const OFString &toCharset);

    /** select a particular DICOM character set without code extensions for
     *  subsequent conversions.  The corresponding DICOM defined term for the
     *  source character set is determined from the member variable
     *  'SourceCharacterSet'.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition selectCharacterSetWithoutCodeExtensions();

    /** select a particular DICOM character set with code extensions for
     *  subsequent conversions.  The corresponding DICOM defined terms for the
     *  source character set are determined from the member variable
     *  'SourceCharacterSet'.
     *  @param  sourceVM  value multiplicity of the member variable
     *                    'SourceCharacterSet'.  Usually, this value has
     *                    already been determined by the calling method.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition selectCharacterSetWithCodeExtensions(const unsigned long sourceVM);


    /** check whether the given string contains at least one escape character
     *  (ESC), because it is used for code extension techniques like ISO 2022
     *  @param  strValue   input string to be checked for any escape character
     *  @param  strLength  length of the input string
     *  @return OFTrue if an escape character has been found, OFFalse otherwise
     */
    OFBool checkForEscapeCharacter(const char *strValue,
                                   const size_t strLength) const;

    /** convert given string to octal format, i.e.\ all non-ASCII and control
     *  characters are converted to their octal representation.  The total
     *  length of the string is always limited to a particular maximum (see
     *  implementation).  If the converted string would be longer, it is
     *  cropped and "..." is appended to indicate this cropping.
     *  @param  strValue   input string to be converted and possibly cropped
     *  @param  strLength  length of the input string
     *  @return resulting string in octal format
     */
    OFString convertToLengthLimitedOctalString(const char *strValue,
                                               const size_t strLength) const;


  private:

    /// type definition of a map storing the identifier (key) of a character
    /// set and the associated character set converter
    typedef OFMap<OFString, OFCharacterEncoding> T_EncodingConvertersMap;

    // private undefined copy constructor
    DcmSpecificCharacterSet(const DcmSpecificCharacterSet &);

    // private undefined assignment operator
    DcmSpecificCharacterSet &operator=(const DcmSpecificCharacterSet &);

    /// selected source character set(s) based on one or more DICOM defined terms
    OFString SourceCharacterSet;

    /// selected destination character set based on a single DICOM defined term
    OFString DestinationCharacterSet;

    /// selected destination encoding based on names supported by the underlying
    /// character encoding library
    OFString DestinationEncoding;

    /// character encoding converter
    OFCharacterEncoding DefaultEncodingConverter;

    /// map of character set conversion descriptors
    /// (only used if multiple character sets are needed)
    T_EncodingConvertersMap EncodingConverters;
};


#endif
