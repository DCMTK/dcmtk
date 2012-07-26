/*
 *
 *  Copyright (C) 2011-2012, OFFIS e.V.
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
 *  Purpose: Class for character encoding conversion (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofchrenc.h"
#include "dcmtk/ofstd/ofstd.h"

#ifdef WITH_LIBICONV
#include <iconv.h>
#include <localcharset.h>
#endif

BEGIN_EXTERN_C
#ifdef HAVE_SYS_ERRNO_H
#include <sys/errno.h>
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#define ILLEGAL_DESCRIPTOR     OFreinterpret_cast(OFCharacterEncoding::T_Descriptor, -1)
#define CONVERSION_ERROR       OFstatic_cast(size_t, -1)
#define CONVERSION_BUFFER_SIZE 1024


/*-------------*
 *  constants  *
 *-------------*/

#ifdef HAVE_WINDOWS_H
// Windows-specific code page identifiers
const unsigned int OFCharacterEncoding::CPC_ANSI   = CP_ACP;
const unsigned int OFCharacterEncoding::CPC_OEM    = CP_OEMCP;
const unsigned int OFCharacterEncoding::CPC_ASCII  = 20127;
const unsigned int OFCharacterEncoding::CPC_Latin1 = 28591;
const unsigned int OFCharacterEncoding::CPC_UTF8   = CP_UTF8;
#endif


/*------------------*
 *  implementation  *
 *------------------*/

OFCharacterEncoding::OFCharacterEncoding()
  : LocaleEncoding(),
    ConversionDescriptor(ILLEGAL_DESCRIPTOR),
    TransliterationMode(OFFalse),
    DiscardIllegalSequenceMode(OFFalse)
{
    updateLocaleEncoding();
}


OFCharacterEncoding::~OFCharacterEncoding()
{
    clear();
}


void OFCharacterEncoding::clear()
{
    // close conversion descriptor (if needed)
    closeDescriptor(ConversionDescriptor);
    // reset conversion modes
    TransliterationMode = OFFalse;
    DiscardIllegalSequenceMode = OFFalse;
}


OFBool OFCharacterEncoding::getTransliterationMode() const
{
    return TransliterationMode;
}


OFBool OFCharacterEncoding::getDiscardIllegalSequenceMode() const
{
    return DiscardIllegalSequenceMode;
}


OFCondition OFCharacterEncoding::setTransliterationMode(const OFBool mode)
{
#if defined(WITH_LIBICONV) && _LIBICONV_VERSION >= 0x0108
    TransliterationMode = mode;
    return EC_Normal;
#else
    // return with an error because iconvctl() is not supported
    return makeOFCondition(0, EC_CODE_CannotControlConverter, OF_error,
        "Cannot control character encoding converter: iconvctl() not supported");
#endif
}


OFCondition OFCharacterEncoding::setDiscardIllegalSequenceMode(const OFBool mode)
{
#if defined(WITH_LIBICONV) && _LIBICONV_VERSION >= 0x0108
    DiscardIllegalSequenceMode = mode;
    return EC_Normal;
#else
    // return with an error because iconvctl() is not supported
    return makeOFCondition(0, EC_CODE_CannotControlConverter, OF_error,
        "Cannot control character encoding converter: iconvctl() not supported");
#endif
}


const OFString &OFCharacterEncoding::getLocaleEncoding() const
{
    return LocaleEncoding;
}


OFCondition OFCharacterEncoding::updateLocaleEncoding()
{
#ifdef WITH_LIBICONV
    // determine current locale's character encoding
    LocaleEncoding = OFSTRING_GUARD(::locale_charset());
    // basically, the above function should always return a non-empty string
    // but older versions of libiconv might return NULL is certain cases
    return EC_Normal;
#else
    return EC_NoEncodingLibrary;
#endif
}


OFCondition OFCharacterEncoding::selectEncoding(const OFString &fromEncoding,
                                                const OFString &toEncoding)
{
#ifdef WITH_LIBICONV
    // first, close the current conversion descriptor (if needed)
    closeDescriptor(ConversionDescriptor);
    // then, try to open a new descriptor for the specified character encodings
    return openDescriptor(ConversionDescriptor, fromEncoding.c_str(), toEncoding.c_str());
#else
    return EC_NoEncodingLibrary;
#endif
}


OFCondition OFCharacterEncoding::convertString(const OFString &fromString,
                                               OFString &toString,
                                               const OFBool clearMode)
{
    // call the real method converting the given string
    return convertString(ConversionDescriptor, fromString.c_str(), fromString.length(), toString, clearMode);
}


OFCondition OFCharacterEncoding::convertString(const char *fromString,
                                               const size_t fromLength,
                                               OFString &toString,
                                               const OFBool clearMode)
{
    // call the real method converting the given string
    return convertString(ConversionDescriptor, fromString, fromLength, toString, clearMode);
}


OFCondition OFCharacterEncoding::convertString(T_Descriptor descriptor,
                                               const char *fromString,
                                               const size_t fromLength,
                                               OFString &toString,
                                               const OFBool clearMode)
{
    // first, clear result variable if requested
    if (clearMode)
        toString.clear();
#ifdef WITH_LIBICONV
    OFCondition status = EC_Normal;
    // check whether the given conversion descriptor has been allocated
    if (isDescriptorValid(descriptor))
    {
#if _LIBICONV_VERSION >= 0x0108
        // enable/disable transliteration (use of similar characters) in the conversion
        int translit = (TransliterationMode) ? 1 : 0;
        if (::iconvctl(descriptor, ICONV_SET_TRANSLITERATE, &translit) < 0)
        {
            // if this didn't work, return with an appropriate error message
            createErrnoCondition(status, "Cannot control character encoding feature TRANSLITERATE: ",
                EC_CODE_CannotControlConverter);
        }
        // enable/disable discarding of illegal sequences in the conversion
        int discard = (DiscardIllegalSequenceMode) ? 1 : 0;
        if (::iconvctl(descriptor, ICONV_SET_DISCARD_ILSEQ, &discard) < 0)
        {
            // if this didn't work, return with an appropriate error message
            createErrnoCondition(status, "Cannot control character encoding feature DISCARD_ILSEQ: ",
                EC_CODE_CannotControlConverter);
        }
#endif
        // if the input string is empty or NULL, we are done
        if (status.good() && (fromString != NULL) && (fromLength > 0))
        {
#if defined(_WIN32) && !defined(__MINGW32__)
            const char *inputPos = fromString;
#else
            char *inputPos = OFconst_cast(char *, fromString);
#endif
            size_t inputLeft = fromLength;
            // set the conversion descriptor to the initial state
            ::iconv(descriptor, NULL, NULL, NULL, NULL);
            // iterate as long as there are characters to be converted
            while (inputLeft > 0)
            {
                char buffer[CONVERSION_BUFFER_SIZE];
                char *bufferPos = buffer;
                const size_t bufferLength = sizeof(buffer);
                size_t bufferLeft = bufferLength;
                // convert the current block of the given string to the selected character encoding
                if (::iconv(descriptor, &inputPos, &inputLeft, &bufferPos, &bufferLeft) == CONVERSION_ERROR)
                {
                    // check whether the output buffer was too small for the next converted character
                    // (also make sure that the output buffer has been filled to avoid an endless loop)
                    if ((errno != E2BIG) || (bufferLeft == bufferLength))
                    {
                        // if the conversion was unsuccessful, return with an appropriate error message
                        createErrnoCondition(status, "Cannot convert character encoding: ",
                            EC_CODE_CannotConvertEncoding);
                        break;
                    }
                }
                // append the converted character string to the result variable
                toString.append(buffer, bufferLength - bufferLeft);
            }
        }
    } else
        status = EC_NoEncodingSelected;
    return status;
#else
    return EC_NoEncodingLibrary;
#endif
}


#ifdef HAVE_WINDOWS_H  // Windows-specific conversion functions

OFCondition OFCharacterEncoding::convertFromWideCharString(const wchar_t *fromString,
                                                           const size_t fromLength,
                                                           OFString &toString,
                                                           const unsigned int codePage,
                                                           const OFBool clearMode)
{
    // first, clear result variable if requested
    if (clearMode)
        toString.clear();
    OFCondition status = EC_Normal;
    // check for empty string
    if ((fromString != NULL) && (fromLength > 0))
    {
        // determine required size for output buffer
        const int sizeNeeded = WideCharToMultiByte(codePage, 0, fromString, fromLength, NULL, 0, NULL, NULL);
        if (sizeNeeded > 0)
        {
            // allocate temporary buffer
            char *toBuffer = new char[sizeNeeded];
            if (toBuffer != NULL)
            {
                // convert characters (without trailing NULL byte)
                const int charsConverted = WideCharToMultiByte(codePage, 0, fromString, fromLength, toBuffer, sizeNeeded, NULL, NULL);
                if (charsConverted > 0)
                {
                    // append the converted character string to the result variable
                    toString.append(toBuffer, charsConverted);
                } else {
                    // if conversion failed, create appropriate condition text
                    createGetLastErrorCondition(status, "Cannot convert character encoding: ",
                        EC_CODE_CannotConvertEncoding);
                }
                delete[] toBuffer;
            } else {
                // output buffer could not be allocated
                status = EC_MemoryExhausted;
            }
        }
    }
    return status;
}


OFCondition OFCharacterEncoding::convertToWideCharString(const OFString &fromString,
                                                         wchar_t *&toString,
                                                         size_t &toLength,
                                                         const unsigned int codePage)
{
    // call the real method converting the given string
    return OFCharacterEncoding::convertToWideCharString(fromString.c_str(), fromString.length(),
        toString, toLength, codePage);
}


OFCondition OFCharacterEncoding::convertToWideCharString(const char *fromString,
                                                         const size_t fromLength,
                                                         wchar_t *&toString,
                                                         size_t &toLength,
                                                         const unsigned int codePage)
{
    OFCondition status = EC_Normal;
    // check for empty string
    if ((fromString != NULL) && (fromLength > 0))
    {
        // determine required size for output buffer
        const int sizeNeeded = MultiByteToWideChar(codePage, 0, fromString, fromLength, NULL, 0);
        // allocate output buffer (one extra byte for the terminating NULL)
        toString = new wchar_t[sizeNeeded + 1];
        if (toString != NULL)
        {
            // convert characters (without trailing NULL byte)
            toLength = MultiByteToWideChar(codePage, 0, fromString, fromLength, toString, sizeNeeded);
            // append NULL byte to mark "end of string"
            toString[toLength] = L'\0';
            if (toLength == 0)
            {
                // if conversion failed, create appropriate condition text
                createGetLastErrorCondition(status, "Cannot convert character encoding: ",
                    EC_CODE_CannotConvertEncoding);
            }
        } else {
            // output buffer could not be allocated
            status = EC_MemoryExhausted;
        }
    } else {
        // create an empty string (should never fail)
        toString = new wchar_t[1];
        toString[0] = L'\0';
        toLength = 0;
    }
    return status;
}

#endif  // HAVE_WINDOWS_H


OFCondition OFCharacterEncoding::openDescriptor(T_Descriptor &descriptor,
                                                const OFString &fromEncoding,
                                                const OFString &toEncoding)
{
#ifdef WITH_LIBICONV
    OFCondition status = EC_Normal;
    // try to open a new descriptor for the specified character encodings
    descriptor = ::iconv_open(toEncoding.c_str(), fromEncoding.c_str());
    // check whether the conversion descriptor could be allocated
    if (!isDescriptorValid(descriptor))
    {
        // if not, return with an appropriate error message
        createErrnoCondition(status, "Cannot open character encoding: ",
            EC_CODE_CannotOpenEncoding);
    }
    return status;
#else
    descriptor = ILLEGAL_DESCRIPTOR;
    return EC_NoEncodingLibrary;
#endif
}


OFCondition OFCharacterEncoding::closeDescriptor(T_Descriptor &descriptor)
{
#ifdef WITH_LIBICONV
    OFCondition status = EC_Normal;
    // check whether the conversion descriptor is valid
    if (isDescriptorValid(descriptor))
    {
        // try to close given descriptor and check whether it worked
        if (::iconv_close(descriptor) == -1)
        {
            // if not, return with an appropriate error message
            createErrnoCondition(status, "Cannot close character encoding: ",
                EC_CODE_CannotCloseEncoding);
        }
    }
    // in any case, make the descriptor invalid
    descriptor = ILLEGAL_DESCRIPTOR;
    return status;
#else
    OFCondition status = EC_Normal;
    // we cannot use isDescriptorValid() because it always returns OFFalse
    if (descriptor != ILLEGAL_DESCRIPTOR)
    {
        descriptor = ILLEGAL_DESCRIPTOR;
        status = EC_NoEncodingLibrary;
    }
    return status;
#endif
}


OFBool OFCharacterEncoding::isDescriptorValid(const T_Descriptor descriptor)
{
#ifdef WITH_LIBICONV
    return (descriptor != ILLEGAL_DESCRIPTOR);
#else
    return OFFalse;
#endif
}


void OFCharacterEncoding::createErrnoCondition(OFCondition &status,
                                               OFString message,
                                               const unsigned short code)
{
#ifdef WITH_LIBICONV
    char errBuf[256];
    message.append(OFStandard::strerror(errno, errBuf, sizeof(errBuf)));
    status = makeOFCondition(0, code, OF_error, message.c_str());
#endif
}


#ifdef HAVE_WINDOWS_H  // Windows-specific function

void OFCharacterEncoding::createGetLastErrorCondition(OFCondition &status,
                                                      OFString message,
                                                      const unsigned short code)
{
    LPVOID errBuf = NULL;
    // obtain an error string from system error code
    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &errBuf, 0, NULL) > 0)
    {
        message.append(OFstatic_cast(const char *, errBuf));
    } else
        message.append("unknown error code");
    LocalFree(errBuf);
    status = makeOFCondition(0, code, OF_error, message.c_str());
}

#endif  // HAVE_WINDOWS_H


OFBool OFCharacterEncoding::isLibraryAvailable()
{
#ifdef WITH_LIBICONV
    return OFTrue;
#else
    return OFFalse;
#endif
}


OFString OFCharacterEncoding::getLibraryVersionString()
{
#ifdef WITH_LIBICONV
    OFString versionStr = "LIBICONV, Version ";
    char buf[10];
    // extract major and minor version number
    sprintf(buf, "%i.%i", (_libiconv_version >> 8), (_libiconv_version & 0xff));
    versionStr.append(buf);
    return versionStr;
#else
    return "<no character encoding library available>";
#endif
}


size_t OFCharacterEncoding::countCharactersInUTF8String(const OFString &utf8String)
{
    const size_t length = utf8String.length();
    size_t count = 0;
    // iterate over all bytes and count start of UTF-8 characters
    for (size_t i = 0; i < length; i++)
    {
        if ((utf8String.at(i) & 0xc0) != 0x80)
            count++;
    }
    return count;
}
