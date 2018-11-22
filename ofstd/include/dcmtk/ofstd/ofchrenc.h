/*
 *
 *  Copyright (C) 2011-2018, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier, Jan Schlamelcher
 *
 *  Purpose: Class for character encoding conversion (Header)
 *
 */


#ifndef OFCHRENC_H
#define OFCHRENC_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofmem.h"

/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A class for managing and converting between different character encodings.
 *  The implementation relies on ICONV (native implementation or libiconv) or
 *  ICU, depending on the configuration.
 *  @remarks An encoder might be shared by copy constructing an
 *    OFCharacterEncoding object from an existing one.  Both objects will refer
 *    to the same encoder once this is done, which will only be destroyed after
 *    both objects are, using OFshared_ptr internally.
 */
class DCMTK_OFSTD_EXPORT OFCharacterEncoding
{
public:

    /** Constants to control encoder behavior, e.g.\ regarding illegal character
     *  sequences.
     *  Currently defined constants may be used to control the implementation's
     *  behavior regarding illegal character sequences.
     *  An illegal character sequence is a sequence of characters in the
     *  source string that is only valid in the context of the source string's
     *  character set and has no valid representation in the character set of
     *  the destination string.
     *  Use these constants to control the transcoding behavior in case an
     *  illegal sequence is encountered.
     *  @note You may set a single one of the constants as the encoder behavior
     *    or even a combination (bitwise OR), however, it depends on the
     *    underlying implementation which flags/combinations are supported.
     *    Use supportsConversionFlags() to query this information at runtime.
     */
    enum ConversionFlags
    {
        /** Abort transcoding (returning an error condition) if an illegal
         *  sequence is encountered.
         */
        AbortTranscodingOnIllegalSequence = 1,

        /** Skip over any illegal character sequences that are encountered.
         */
        DiscardIllegalSequences = 2,

        /** Replace illegal character sequences with an available representation
         *  in the destination character set that somewhat resembles the meaning
         *  (i.e.\ &ouml; -> "o).  The actual results may vary depending on the
         *  underlying implementation.
         */
        TransliterateIllegalSequences = 4
    };

    /** determine whether the underlying implementations defines a default
     *  encoding.
     *  Most implementations define a default encoding, i.e. one can pass an
     *  empty string as the toEncoding and/or fromEncoding argument(s) of
     *  selectEncoding() to select the current locale's encoding.
     *  However, some iconv implementations inside the C standard library do
     *  not understand this.
     *  @warning Using getLocaleEncoding() instead of an empty string argument
     *    in this case typically won't work, since the implementations that
     *    don't define a default encoding typically also don't support
     *    determining the current locale's encoding.
     *  @return OFTrue if a default encoding is defined and empty strings
     *    are valid arguments to selectEncoding(), OFFalse otherwise.
     */
    static OFBool hasDefaultEncoding();

    /** get the character encoding of the currently set global locale.
     *  @remarks Calling this function might be rather exhaustive depending on
     *    employed character set conversion library.  Caching the result might
     *    therefore be recommended.
     *  @note The result may be an empty string, if the name of the current
     *    encoding cannot be determined.
     *  @return the current locale's character encoding
     */
    static OFString getLocaleEncoding();

    /** determine whether the underlying implementation supports the given
     *  conversion flags.
     *  @param flags the flags to query, a combination of
     *    OFCharacterEncoding::ConversionFlags constants, e.g.
     *    TransliterateIllegalSequences | DiscardIllegalSequences.
     *  @return OFTrue if the given flags are supported, OFFalse if not
     *    or support is unknown.
     */
    static OFBool supportsConversionFlags(const unsigned flags);

    /** constructor.
     *  Will create an OFCharacterEncoding instance that does not refer to an
     *  encoder.
     */
    OFCharacterEncoding();

    /** copy constructor.
     *  Will share the encoder of another OFCharacterEncoding instance.
     *  @param rhs another OFCharacterEncoding instance.
     */
    OFCharacterEncoding(const OFCharacterEncoding& rhs);

    /** destructor
     */
    ~OFCharacterEncoding();

    /** copy assignment.
     *  Effectively calls clear() and then shares the encoder of another
     *  OFCharacterEncoding instance.
     *  @param rhs another OFCharacterEncoding instance.
     *  @return reference to this object
     */
    OFCharacterEncoding& operator=(const OFCharacterEncoding& rhs);

    /** check whether this object refers to a valid encoder.
     *  @result OFTrue if this refers to a valid encoder, OFFalse otherwise.
     */
#ifdef HAVE_CXX11
    explicit
#endif
    operator OFBool() const;

    /** check whether this object does not refer to a valid encoder.
     *  @result OFFalse if this refers to a valid encoder, OFTrue otherwise.
     */
    OFBool operator!() const;

    /** check whether two OFCharacterEncoding instances refer to the same
     *  encoder.
     *  @note This only tests if both objects refer to the exactly same
     *    encoder, originating from one and the same call to selectEncoding().
     *    The result will be OFFalse if both encoders were constructed
     *    independently of each other, even if exactly the same parameters
     *    were used.
     *  @param rhs another OFCharacterEncoding instance.
     *  @return OFTrue if both instances refer to the same encoder, OFFalse
     *    otherwise.
     */
    OFBool operator==(const OFCharacterEncoding& rhs) const;

    /** check whether two OFCharacterEncoding instances do not refer to the
     *  same encoder.
     *  @note This only tests if both objects refer to the exactly same
     *    encoder, originating from one and the same call to selectEncoding().
     *    The result will be OFTrue if both encoders were constructed
     *    independently of each other, even if exactly the same parameters
     *    were used.
     *  @param rhs another OFCharacterEncoding instance.
     *  @return OFFalse if both instances refer to the same encoder, OFTrue
     *    otherwise.
     */
    OFBool operator!=(const OFCharacterEncoding& rhs) const;

    /** clear the internal state.
     *  This resets the converter and potentially frees all used resources
     *  (if this is the last OFCharacterEncoding instance referring to the
     *  encoder).
     */
    void clear();

    /** get flags controlling converter behavior, e.g.\ specifying how illegal
     *  character sequences should be handled during conversion.
     *  @note This method will always return 0 if no encoder was selected
     *    using selectEncoding() before calling it.
     *  @return a combination the IllegalSequenceMode constants (bitwise or)
     *    that is currently set or 0 if the current mode cannot be determined.
     */
    unsigned getConversionFlags() const;

    /** set flags controlling converter behavior, e.g.\ illegal character
     *  sequences should be handled during conversion.
     *  @pre An encoding has been selected by successfully calling
     *    OFCharacterEncoding::selectEncoding(), i.e.
     *    OFCharacterEncoding::isLibraryAvailable() and *this evaluate to
     *    OFTrue.
     *  @param flags the conversion flags that shall be used, a combination of
     *    the OFCharacterEncoding::ConversionFlags constants, e.g.
     *    TransliterateIllegalSequences | DiscardIllegalSequences.
     *  @return EC_Normal if the flags were set, an error code otherwise, i.e.
     *    if the flags are not supported by the underlying implementation.
     *  @see OFCharacterEncoding::supportsConversionFlags()
     */
    OFCondition setConversionFlags(const unsigned flags);

    /** select source and destination character encoding for subsequent
     *  conversion(s).  The encoding names can be found in the documentation
     *  of the underlying implementation (e.g. libiconv).  Typical names
     *  are "ASCII", "ISO-8859-1" and "UTF-8".  An empty string denotes the
     *  encoding of the current locale (see getLocaleEncoding()).
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
     *                      character encoding).  A NULL pointer is regarded as
     *                      an empty string.
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
     *  @remark This list of code pages is only available if DCMTK is compiled
     *  on Windows Operating Systems (defining _WIN32)
     */
    //@{

    /// system default Windows ANSI code page.  See Windows function GetACP().
    /// @remark Only available on Windows Operating Systems (defining _WIN32).
    static const unsigned int CPC_ANSI;
    /// current system OEM code page.  See Windows function GetOEMCP().
    /// @remark Only available on Windows Operating Systems (defining _WIN32).
    static const unsigned int CPC_OEM;
    /// code page for US-ASCII (7-bit)
    /// @remark Only available on Windows Operating Systems (defining _WIN32).
    static const unsigned int CPC_ASCII;
    /// code page for ISO 8859-1 (Latin-1)
    /// @remark Only available on Windows Operating Systems (defining _WIN32).
    static const unsigned int CPC_Latin1;
    /// code page for UTF-8
    /// @remark Only available on Windows Operating Systems (defining _WIN32).
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
     *  @remark this method is only available if DCMTK is compiled on Windows
     *  Operating Systems (defining _WIN32)
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
     *  @remark this method is only available if DCMTK is compiled on Windows
     *  Operating Systems (defining _WIN32)
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
     *  @remark this method is only available if DCMTK is compiled on Windows
     *  Operating Systems (defining _WIN32)
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

    /** check whether character set conversion is available, e.g.\ the
     *  underlying encoding library is available.
     *  If not, no conversion between different character encodings will be
     *  possible (apart from the Windows-specific wide character conversion
     *  functions).
     *  @return OFTrue if character set conversion is possible, OFFalse
     *    otherwise
     */
    static OFBool isLibraryAvailable();

    /** get version information of the underlying character encoding library.
     *  Typical output format: "LIBICONV, Version 1.14".  If character encoding
     *  is not available the output is:
     *    "<no character encoding library available>"
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

  private:

    class Implementation;

    // --- static helper functions ---

#ifdef HAVE_WINDOWS_H

    /** create an error condition based on the return value of "getLastError()"
     *  and the given parameters.  The Windows function FormatMessage() is used
     *  to map the numerical value of the error to a textual description.
     *  @remark this method is only available if DCMTK is compiled on Windows
     *  Operating Systems (defining _WIN32)
     *  @param  status   reference to variable where the condition is stored
     *  @param  message  message text that is used as a prefix to the error
     *  @param  code     unique status code of the error condition
     */
    static void createGetLastErrorCondition(OFCondition &status,
                                            OFString message,
                                            const unsigned short code);

#endif  // HAVE_WINDOWS_H

    /// shared pointer to internal implementation (interface to character
    /// encoding library)
    OFshared_ptr<Implementation> TheImplementation;
};


#endif
