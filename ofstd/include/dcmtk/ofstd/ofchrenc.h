/*
 *
 *  Copyright (C) 2011-2016, OFFIS e.V.
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
class DCMTK_OFSTD_EXPORT OFCharacterEncoding
{

  // allow the DICOM-specific character set class to access protected methods
  friend class DcmSpecificCharacterSet;

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

    /** set mode specifying whether a character that cannot be represented in
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
     *                      stored (or appended, see parameter 'clearMode')
     *  @param  clearMode   flag indicating whether to clear the variable
     *                      'toString' before appending the converted string
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition convertString(const OFString &fromString,
                              OFString &toString,
                              const OFBool clearMode = OFTrue);

    /** convert the given string between the selected character encodings.
     *  That means selectEncoding() has to be called prior to this method.
     *  Since the length of the input string has to be specified explicitly,
     *  the string can contain more than one NULL byte.
     *  @param  fromString  input string to be converted (using the source
     *                      character encoding).  A NULL pointer is regarded
     *                      as an empty string.
     *  @param  fromLength  length of the input string (number of bytes without
     *                      the trailing NULL byte)
     *  @param  toString    reference to variable where the converted string
     *                      (using the destination character encoding) is
     *                      stored (or appended, see parameter 'clearMode')
     *  @param  clearMode   flag indicating whether to clear the variable
     *                      'toString' before appending the converted string
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition convertString(const char *fromString,
                              const size_t fromLength,
                              OFString &toString,
                              const OFBool clearMode = OFTrue);

#ifdef HAVE_WINDOWS_H

    /** @name code page definitions.
     *  Short list of common code page identifiers used for the conversion to
     *  and from Windows-specific wide character encoding (UTF-16).
     *  For further code pages, please refer to the MSDN documentation on
     *  "Code Page Identifiers".
     */
    //@{

    /// system default Windows ANSI code page.  See Windows function GetACP().
    static const unsigned int CPC_ANSI;
    /// current system OEM code page.  See Windows function GetOEMCP().
    static const unsigned int CPC_OEM;
    /// code page for US-ASCII (7-bit)
    static const unsigned int CPC_ASCII;
    /// code page for ISO 8859-1 (Latin-1)
    static const unsigned int CPC_Latin1;
    /// code page for UTF-8
    static const unsigned int CPC_UTF8;

    //@}

    // --- static Windows-specific functions ---

    /** convert the given string between Windows-specific wide character
     *  encoding (UTF-16) and the specified code page.  In contrast to
     *  convertString(), no special character encoding library is needed,
     *  but on the other hand it only works on Windows systems.
     *  Please note that no conversion flags are specified for the internal
     *  call to the WideCharToMultiByte() function.
     *  Since the length of the input string has to be specified explicitly,
     *  the string can contain more than one NULL character.
     *  @param  fromString  input string to be converted (using the UTF-16
     *                      character encoding).  A NULL pointer is regarded
     *                      as an empty string.
     *  @param  fromLength  length of the input string (number of characters
     *                      without the trailing NULL character)
     *  @param  toString    reference to variable where the converted string
     *                      (using the character encoding specified by
     *                      'codePage') is stored (or appended, see parameter
     *                      'clearMode')
     *  @param  codePage    identifier of the code page to be used for the
     *                      conversion (default: UTF-8)
     *  @param  clearMode   flag indicating whether to clear the variable
     *                      'toString' before appending the converted string
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition convertFromWideCharString(const wchar_t *fromString,
                                                 const size_t fromLength,
                                                 OFString &toString,
                                                 const unsigned int codePage = CPC_UTF8,
                                                 const OFBool clearMode = OFTrue);

    /** convert the given string between the specified code page and the
     *  Windows-specific wide character encoding (UTF-16).  In contrast to
     *  convertString(), no special character encoding library is needed, but
     *  on the other hand it only works on Windows systems.
     *  Please note that no conversion flags are specified for the internal
     *  call to the MultiByteToWideChar() function.
     *  @param  fromString  input string to be converted (using character
     *                      encoding specified by 'codePage')
     *  @param  toString    reference to variable in which the pointer to the
     *                      converted string (using the UTF-16 character
     *                      encoding) is stored.  Might only be NULL if memory
     *                      is exhausted.  Please note that the buffer is
     *                      created with new[] and has to be deleted by the
     *                      caller.
     *  @param  toLength    number of converted characters, i.e.\ length of
     *                      'toString'
     *  @param  codePage    identifier of the code page to be used for the
     *                      conversion (default: UTF-8)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition convertToWideCharString(const OFString &fromString,
                                               wchar_t *&toString,
                                               size_t &toLength,
                                               const unsigned int codePage = CPC_UTF8);

    /** convert the given string between the specified code page and the
     *  Windows-specific wide character encoding (UTF-16).  In contrast to
     *  convertString(), no special character encoding library is needed, but
     *  on the other hand it only works on Windows systems.
     *  Please note that no conversion flags are specified for the internal
     *  call to the MultiByteToWideChar() function.
     *  Since the length of the input string has to be specified explicitly,
     *  the string can contain more than one NULL byte.
     *  @param  fromString  input string to be converted (using the  character
     *                      encoding specified by 'codePage').  A NULL pointer
     *                      is regarded as an empty string.
     *  @param  fromLength  length of the input string (number of bytes
     *                      without the trailing NULL byte)
     *  @param  toString    reference to variable in which the pointer to the
     *                      converted string (using the UTF-16 character
     *                      encoding) is stored.  Might only be NULL if memory
     *                      is exhausted.  Please note that the buffer is
     *                      created with new[] and has to be deleted by the
     *                      caller.
     *  @param  toLength    number of converted characters, i.e.\ length of
     *                      'toString'
     *  @param  codePage    identifier of the code page to be used for the
     *                      conversion (default: UTF-8)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition convertToWideCharString(const char *fromString,
                                               const size_t fromLength,
                                               wchar_t *&toString,
                                               size_t &toLength,
                                               const unsigned int codePage = CPC_UTF8);

#endif  // HAVE_WINDOWS_H

    // --- static helper functions ---

    /** check whether the underlying character encoding library is available.
     *  If the library is not available, no conversion between different
     *  character encodings will be possible (apart from the Windows-specific
     *  wide character conversion functions).
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

    /// type of the conversion descriptor (used by libiconv)
    typedef void* T_Descriptor;

    /** allocate conversion descriptor for the given source and destination
     *  character encoding.  Please make sure that the descriptor is
     *  deallocated with closeDescriptor() when not needed any longer.
     *  @param  descriptor    reference to variable where the newly allocated
     *                        conversion descriptor is stored
     *  @param  fromEncoding  name of the source character encoding
     *  @param  toEncoding    name of the destination character encoding
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition openDescriptor(T_Descriptor &descriptor,
                               const OFString &fromEncoding,
                               const OFString &toEncoding);

    /** deallocate the given conversion descriptor that was previously
     *  allocated with openDescriptor().  Please do not pass arbitrary values
     *  to this method, since this will result in a segmentation fault.
     *  @param  descriptor  conversion descriptor to be closed.  After the
     *                      descriptor has been deallocated, 'descriptor' is
     *                      set to an invalid value - see isDescriptorValid().
     *  @return status, EC_Normal if successful, an error code otherwise.  In
     *    case an invalid descriptor is passed, it is not regarded as an error.
     */
    OFCondition closeDescriptor(T_Descriptor &descriptor);

    /** check whether the given conversion descriptor is valid, i.e.\ has been
     *  allocated by a previous call to openDescriptor()
     *  @param  descriptor  conversion descriptor to be checked
     *  @return OFTrue if the conversion descriptor is valid, OFFalse otherwise
     */
    OFBool isDescriptorValid(const T_Descriptor descriptor);

    /** convert the given string between the specified character encodings.
     *  Since the length of the input string has to be specified explicitly,
     *  the string can contain more than one NULL byte.
     *  @param  descriptor  previously allocated conversion descriptor to be
     *                      used for the conversion of the character encodings
     *  @param  fromString  input string to be converted (using the source
     *                      character encoding).  A NULL pointer is regarded
     *                      as an empty string.
     *  @param  fromLength  length of the input string (number of bytes without
     *                      the trailing NULL byte)
     *  @param  toString    reference to variable where the converted string
     *                      (using the destination character encoding) is
     *                      stored (or appended, see parameter 'clearMode')
     *  @param  clearMode   flag indicating whether to clear the variable
     *                      'toString' before appending the converted string
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition convertString(T_Descriptor descriptor,
                              const char *fromString,
                              const size_t fromLength,
                              OFString &toString,
                              const OFBool clearMode = OFTrue);


  private:

    // private undefined copy constructor
    OFCharacterEncoding(const OFCharacterEncoding &);

    // private undefined assignment operator
    OFCharacterEncoding &operator=(const OFCharacterEncoding &);

    // --- static helper functions ---

    /** create an error condition based on the current value of "errno" and the
     *  given parameters.  The function OFStandard::strerror() is used to map
     *  the numerical value of the error to a textual description.
     *  @param  status   reference to variable where the condition is stored
     *  @param  message  message text that is used as a prefix to strerror()
     *  @param  code     unique status code of the error condition
     */
    static void createErrnoCondition(OFCondition &status,
                                     OFString message,
                                     const unsigned short code);

#ifdef HAVE_WINDOWS_H

    /** create an error condition based on the return value of "getLastError()"
     *  and the given parameters.  The Windows function FormatMessage() is used
     *  to map the numerical value of the error to a textual description.
     *  @param  status   reference to variable where the condition is stored
     *  @param  message  message text that is used as a prefix to the error
     *  @param  code     unique status code of the error condition
     */
    static void createGetLastErrorCondition(OFCondition &status,
                                            OFString message,
                                            const unsigned short code);

#endif  // HAVE_WINDOWS_H

    /// current locale's character encoding
    OFString LocaleEncoding;

    /// conversion descriptor used by libiconv
    T_Descriptor ConversionDescriptor;

    /// transliteration mode (default: disabled)
    OFBool TransliterationMode;

    /// discard illegal sequence mode (default: disabled)
    OFBool DiscardIllegalSequenceMode;
};


#endif
