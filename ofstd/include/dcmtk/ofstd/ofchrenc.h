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
 *  Module:  ofstd
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Class for character encoding conversion (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-10-25 17:13:59 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef OFCHRENC_H
#define OFCHRENC_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A class for managing and converting between different character encodings.
 *  The implementation relies on the libiconv toolkit (if available).
 */
class OFCharacterEncoding
{

  public:

    /** constructor. Initializes the member variables, which includes the
     *  current locale's character encoding.
     */
    OFCharacterEncoding();

    /** destructor
     */
    ~OFCharacterEncoding();

    /** clear the internal state.  This also closes the conversion descriptor
     *  if it was allocated before, so selectEncoding() has to be called again
     *  before a string can be converted to a new character encoding.
     */
    void clear();

    /** get mode specifying whether a character that cannot be represented in
     *  the destination character encoding is approximated through one or more
     *  characters that look similar to the original one
     *  @return current value of the mode.  OFTrue means that the mode is
     *    enabled, OFFalse means disabled.
     */
    OFBool getTransliterationMode() const;

    /** get mode specifying whether characters that cannot be represented in
     *  the destination character encoding will be silently discarded
     *  @return current value of the mode.  OFTrue means that the mode is
     *    enabled, OFFalse means disabled.
     */
    OFBool getDiscardIllegalSequenceMode() const;

    /** get mode specifying whether a character that cannot be represented in
     *  the destination character encoding is approximated through one or more
     *  characters that look similar to the original one.  By default, this
     *  mode is disabled.
     *  @param  mode  enable mode by OFTrue or disable it by OFFalse
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setTransliterationMode(const OFBool mode);

    /** set mode specifying whether characters that cannot be represented in
     *  the destination character encoding will be silently discarded.  By
     *  default, this mode is disabled.
     *  @param  mode  enable mode by OFTrue or disable it by OFFalse
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setDiscardIllegalSequenceMode(const OFBool mode);

    /** get the current locale's character encoding
     *  @return the current locale's character encoding
     */
    const OFString &getLocaleEncoding() const;

    /** updates the current locale's character encoding.  This is only needed
     *  if the locale setting changed during the lifetime of this object,
     *  because the current locale's character encoding is always determined
     *  in the constructor.  If possible the canonical encoding names listed
     *  in "config.charset" (see libiconv toolkit) are used.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition updateLocaleEncoding();

    /** select source and destination character encoding for subsequent
     *  conversion(s).  The encoding names can be found in the documentation
     *  of the libiconv toolkit.  Typical names are "ASCII", "ISO-8859-1" and
     *  "UTF-8".  An empty string denotes the locale dependent character
     *  encoding (see getLocaleEncoding()).
     *  @param  fromEncoding  name of the source character encoding
     *  @param  toEncoding    name of the destination character encoding
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition selectEncoding(const OFString &fromEncoding,
                               const OFString &toEncoding);

    /** convert the given string between the selected character encodings.
     *  That means selectEncoding() has to be called prior to this method.
     *  @param  fromString  input string to be converted (using the source
     *                      character encoding)
     *  @param  toString    reference to variable where the converted string
     *                      (using the destination character encoding) is
     *                      stored (or appended, see parameter 'clear')
     *  @param  clear       flag indicating whether to clear the variable
     *                      'toString' before appending the converted string
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition convertString(const OFString &fromString,
                              OFString &toString,
                              const OFBool clear = OFTrue);

    /** convert the given string between the selected character encodings.
     *  That means selectEncoding() has to be called prior to this method.
     *  Since the length of the input string has to be specified explicitly,
     *  the string can contain more than one NULL byte.
     *  @param  fromString  input string to be converted (using the source
     *                      character encoding)
     *  @param  fromLength  length of the input string (number of bytes
     *                      without the trailing NULL byte)
     *  @param  toString    reference to variable where the converted string
     *                      (using the destination character encoding) is
     *                      stored (or appended, see parameter 'clear')
     *  @param  clear       flag indicating whether to clear the variable
     *                      'toString' before appending the converted string
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition convertString(const char *fromString,
                              const size_t fromLength,
                              OFString &toString,
                              const OFBool clear = OFTrue);

    // --- static helper functions ---

    /** check whether the underlying character encoding library is available.
     *  If the library is not available, no conversion between different
     *  character encodings will be possible.
     *  @return OFTrue if the character encoding library is available, OFFalse
     *    otherwise
     */
    static OFBool isLibraryAvailable();

    /** get version information of the underlying character encoding library.
     *  Typical output format: "LIBICONV, Version 1.14".  If the library is not
     *  available the output is: "<no character encoding library available>"
     *  @return name and version number of the character encoding library
     */
    static OFString getLibraryVersionString();

    /** count characters in given UTF-8 string and return the resulting number
     *  of so-called "code points".  Please note that invalid UTF-8 encodings
     *  are not handled properly.  ASCII strings (7-bit) are also supported,
     *  although OFString::length() is probably much faster.
     *  @param  utf8String  valid character string with UTF-8 encoding
     *  @return number of characters (code points) in given UTF-8 string
     */
    static size_t countCharactersInUTF8String(const OFString &utf8String);


  protected:

    /** close the conversion descriptor if it was allocated before
     */
    void closeConversionDescriptor();

    /** check whether the conversion descriptor is valid, i.e.\ has been
     *  allocated by a previous call to selectEncoding()
     *  @return OFTrue if the conversion descriptor is valid, OFFalse otherwise
     */
    OFBool isConversionDescriptorValid() const;


  private:

    // private undefined copy constructor
    OFCharacterEncoding(const OFCharacterEncoding &);

    // private undefined assignment operator
    OFCharacterEncoding &operator=(const OFCharacterEncoding &);

    /** create an error condition based on the curent value of "errno" and the
     *  given parameters.  The function OFStandard::strerror() is used to map
     *  the numerical value of the error to a textual description.
     *  @param  status   reference to variable where the condition is stored
     *  @param  message  message text that is used as a prefix to strerror()
     *  @param  code     unique status code of the error condition
     */
    void createErrnoCondition(OFCondition &status,
                              OFString message,
                              const unsigned short code);

    /// current locale's character encoding
    OFString LocaleEncoding;

    /// conversion descriptor used by libiconv
    void *ConversionDescriptor;

    /// transliteration mode (default: disabled)
    OFBool TransliterationMode;

    /// discard illegal sequence mode (default: disabled)
    OFBool DiscardIllegalSequenceMode;
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: ofchrenc.h,v $
 * Revision 1.5  2011-10-25 17:13:59  joergr
 * Be prepared that older versions of locale_charset() might return NULL.
 *
 * Revision 1.4  2011-10-25 07:10:02  joergr
 * Added new convert method that accepts a C string and its length as input.
 *
 * Revision 1.3  2011-10-24 15:07:33  joergr
 * Added static method counting the characters in a given UTF-8 string.
 *
 * Revision 1.2  2011-10-24 12:49:33  joergr
 * Made sure that iconvctl() is really supported by the libiconv toolkit.
 *
 * Revision 1.1  2011-10-21 09:14:57  joergr
 * Added class for managing and converting between different character encodings
 * based on the libiconv toolkit.
 *
 *
 */
