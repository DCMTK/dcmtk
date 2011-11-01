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
 *  Update Date:      $Date: 2011-11-01 14:53:59 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCSPCHRS_H
#define DCSPCHRS_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofchrenc.h"
#include "dcmtk/ofstd/ofmap.h"


/** A class for managing and converting between different DICOM character sets.
 *  The conversion relies on the OFCharacterEncoding class, which again relies
 *  on the libiconv toolkit (if available).
 *  \note Code extension techniques according to ISO 2022 are not yet supported,
 *        i.e. a Specific Character Set (0008,0005) element with multiple values
 *        cannot be selected.
 */
class DcmSpecificCharacterSet
{

  public:

    /** constructor. Initializes the member variables.
     */
    DcmSpecificCharacterSet();

    /** destructor
     */
    ~DcmSpecificCharacterSet();

    /** get currently selected DICOM character set(s).  Please note that the
     *  returned string can contain mutiple values (defined terms separated by
     *  a backslash) if code extension techniques are used.  Furthermore, the
     *  returned string is always normalized, i.e. leading and trailing spaces
     *  have been removed.
     *  @return currently selected DICOM character set(s) or an empty string
     *    if none is selected (identical to ASCII, which is the default)
     */
    const OFString &getCharacterSet() const;

    /** select DICOM character set(s) for the source string to be converted.
     *  The defined terms for a particular character set can be found in the
     *  DICOM standard, e.g. "ISO_IR 100" for ISO 8859-1 (Latin 1) or "ISO_IR
     *  192" for Unicode in UTF-8.  An empty string denotes the default
     *  character repertoire, which is ASCII (7-bit).  If multiple values are
     *  given (separated by a backslash) code extension techniques are used
     *  and escape sequences may be encountered in the source string to switch
     *  between the specified character sets.
     *  @param  charset  name of the character set(s) as given in the DICOM
     *                   attribute Specific Character Set (0008,0005).
     *                   Leading and trailing spaces are removed automatically
     *                   (if present).
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition selectCharacterSet(const OFString &charset);

    /** convert the given string from the selected character set(s) to UTF-8.
     *  That means selectCharacterSet() has to be called prior to this method.
     *  @param  fromString  input string to be converted (using the previously
     *                      selected character set)
     *  @param  toString    reference to variable where the converted string
     *                      (using the UTF-8 character set) is stored
     *  @param  delimiters  optional string of characters that are regarded as
     *                      delimiters, i.e.\ when found the character set is
     *                      switched back to the default.  CR, LF and FF are
     *                      always regarded as delimiters (see DICOM PS 3.5).
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition convertToUTF8String(const OFString &fromString,
                                    OFString &toString,
                                    const OFString &delimiters = "");

    /** convert the given string from the selected character set(s) to UTF-8.
     *  That means selectCharacterSet() has to be called prior to this method.
     *  Since the length of the input string has to be specified explicitly,
     *  the string can contain more than one NULL byte.
     *  @param  fromString  input string to be converted (using the previously
     *                      selected character set)
     *  @param  fromLength  length of the input string (number of bytes without
     *                      the trailing NULL byte)
     *  @param  toString    reference to variable where the converted string
     *                      (using the UTF-8 character set) is stored
     *  @param  delimiters  optional string of characters that are regarded as
     *                      delimiters, i.e.\ when found the character set is
     *                      switched back to the default.  CR, LF and FF are
     *                      always regarded as delimiters (see DICOM PS 3.5).
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition convertToUTF8String(const char *fromString,
                                    const size_t fromLength,
                                    OFString &toString,
                                    const OFString &delimiters = "");

    // --- static helper functions ---

    /** check whether the underlying character set conversion library is
     *  available.  If the library is not available, no conversion between
     *  different character sets will be possible.
     *  @return OFTrue if the character set conversion library is available,
     *    OFFalse otherwise
     */
    static OFBool isConversionLibraryAvailable();

    /** count characters in given UTF-8 string and return the resulting number
     *  of so-called "code points".  Please note that invalid UTF-8 encodings
     *  are not handled properly.  ASCII strings (7-bit) are also supported,
     *  although OFString::length() is probably much faster.
     *  @param  utf8String  valid character string with UTF-8 encoding
     *  @return number of characters (code points) in given UTF-8 string
     */
    static size_t countCharactersInUTF8String(const OFString &utf8String);


  protected:

    /// type definition of a map storing the identifier (key) of a character
    /// set and the associated conversion descriptor
    typedef OFMap<OFString, OFCharacterEncoding::T_Descriptor> T_DescriptorMap;

    /** select a particular DICOM character set without code extensions for
     *  subsequent conversions.  The corresponding DICOM defined term is
     *  determined from the member variable 'SpecificCharacterSet'.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition selectCharacterSetWithoutCodeExtensions();

    /** select a particular DICOM character set with code extensions for
     *  subsequent conversions.  The corresponding DICOM defined terms are
     *  determined from the member variable 'SpecificCharacterSet'.
     *  @param  charsetVM  value multiplicity of the member variable
     *                     'SpecificCharacterSet'.  Usually, this value has
     *                     already been determined by the calling method.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition selectCharacterSetWithCodeExtensions(const unsigned long charsetVM);

    /** close any currently open character set conversion descriptor(s).
     *  Afterwards, no conversion descriptor is selected, pretty much like
     *  after the initialization with the constructor.
     */
    void closeConversionDescriptors();

    /** check whether the given string contains at least one escape character
     *  (ESC), because it is used for code extension techniques like ISO 2022
     *  @param  strValue   string to be checked for any escape character
     *  @param  strLength  length of the string to be checked
     *  @return OFTrue if an escape character has been found, OFFalse otherwise
     */
    OFBool checkForEscapeCharacter(const char *strValue,
                                   const size_t strLength) const;

    /** convert given string to octal format, i.e.\ all non-ASCII and control
     *  characters are converted to their octal representation.  The total
     *  length of the string is always limited to a particular maximum (see
     *  implementation).  If the converted string would be longer, it is
     *  cropped and "..." is appended to indicate this cropping.
     *  @param  strValue   string to be checked for any escape character
     *  @param  strLength  length of the string to be checked
     *  @return resulting string in octal format
     */
    OFString convertToLengthLimitedOctalString(const char *strValue,
                                               const size_t strLength) const;


  private:

    // private undefined copy constructor
    DcmSpecificCharacterSet(const DcmSpecificCharacterSet &);

    // private undefined assignment operator
    DcmSpecificCharacterSet &operator=(const DcmSpecificCharacterSet &);

    /// selected character set(s) based on one or more DICOM defined terms
    OFString SpecificCharacterSet;

    /// character encoding converter
    OFCharacterEncoding EncodingConverter;

    /// map of character set conversion descriptors
    /// (only used if multiple character sets are needed)
    T_DescriptorMap ConversionDescriptors;
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dcspchrs.h,v $
 * Revision 1.2  2011-11-01 14:53:59  joergr
 * Added support for code extensions (escape sequences) according to ISO 2022
 * to the character set conversion code.
 *
 * Revision 1.1  2011-10-26 16:12:59  joergr
 * Added helper class for converting between different DICOM character sets.
 * This initial version only supports the conversion to UTF-8 (Unicode) and only
 * from DICOM characters sets without code extension techniques (i.e. ISO 2022).
 *
 *
 */
