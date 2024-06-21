/*
 *
 *  Copyright (C) 2011-2024, OFFIS e.V.
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
 *  Purpose: Class for character encoding conversion (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofchrenc.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofdiag.h"
#include "dcmtk/ofstd/ofconsol.h"
#include <cerrno>

#ifdef HAVE_WINDOWS_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/*-------------*
 *  constants  *
 *-------------*/

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

#ifdef DCMTK_ENABLE_CHARSET_CONVERSION

#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_ICONV ||\
 DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV ||\
 DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_STDLIBC_ICONV

#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV

#include "dcmtk/oficonv/iconv.h"

// helper class for cleanup up oficonv at application exit
class OFiconvCleanupHelper
{
public:
  ~OFiconvCleanupHelper()
  {
    OFiconv_cleanup();
  }
};

// global helper object that will clean up the oficonv csmapper area buffer
// at application exit and thus avoid reports about memory leaks
OFiconvCleanupHelper cleanupHelper;

#undef LIBICONV_SECOND_ARGUMENT_CONST
#define iconv_open OFiconv_open
#define iconv_close OFiconv_close
#define iconv OFiconv
#define iconvctl OFiconvctl

#ifndef WITH_LIBICONV
#define WITH_LIBICONV
#endif

#else /* DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV */

#include <iconv.h>
#ifdef WITH_LIBICONV
#include <localcharset.h>
#endif
#ifdef __GLIBC__
#include <langinfo.h> // nl_langinfo / CODESET
#endif

#endif /* DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV */


#define ILLEGAL_DESCRIPTOR     OFreinterpret_cast(iconv_t, -1)
#define CONVERSION_ERROR       OFstatic_cast(size_t, -1)
#define CONVERSION_BUFFER_SIZE 1024

class OFCharacterEncoding::Implementation
{

  public:

    static Implementation* create(const OFString& fromEncoding,
                                  const OFString& toEncoding,
                                  OFCondition& result)
    {
        iconv_t descriptor = ::iconv_open(toEncoding.c_str(), fromEncoding.c_str());
        if (descriptor == ILLEGAL_DESCRIPTOR)
        {
            // return an appropriate error message
            createErrnoCondition(result, "Cannot open character encoding: ", EC_CODE_CannotOpenEncoding);
            return OFnullptr;
        }
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV
        // GNU libiconv does not perform transliteration by default. We emulate this when using oficonv.
        int flag1 = 1;
        (void) ::iconvctl(descriptor, ICONV_SET_ILSEQ_INVALID, &flag1);
#endif
        if (Implementation* pImplementation = new Implementation(descriptor))
        {
            result = EC_Normal;
            return pImplementation;
        }
        result = EC_MemoryExhausted;
        return OFnullptr;
    }

    static OFString getVersionString()
    {
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV
        return "Citrus iconv, Version FreeBSD 13.1 (modified)"
#ifdef DCMTK_ENABLE_BUILTIN_OFICONV_DATA
            ", with built-in data files"
#endif
            ;
#elif defined(WITH_LIBICONV)
        OFString versionStr = "LIBICONV, Version ";
        char buf[10];
        // extract major and minor version number
        OFStandard::snprintf(buf, sizeof(buf), "%i.%i", (_LIBICONV_VERSION >> 8), (_LIBICONV_VERSION & 0xff));
        versionStr.append(buf);
        return versionStr;
#elif defined(__GLIBC__)
        OFOStringStream oss;
        oss << "GNU C library (iconv), version "
            << __GLIBC__ << '.'
            << __GLIBC_MINOR__;
        OFSTRINGSTREAM_GETOFSTRING(oss, version);
        return version;
#else
        return "Unknown C library (iconv)";
#endif
    }

    static OFString getLocaleEncoding()
    {
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV
        iconv_locale_allocation_t buf;
        return OFSTRING_GUARD(::OFlocale_charset(&buf));
#elif defined(WITH_LIBICONV)
        // basically, the function below should always return a non-empty string
        // but older versions of libiconv might return NULL in certain cases
        return OFSTRING_GUARD(::locale_charset());
#elif defined(__GLIBC__)
        const char *oldlocale = setlocale(LC_ALL, "");
        const char *codeset = nl_langinfo (CODESET);
        setlocale(LC_ALL, oldlocale);
        return OFSTRING_GUARD(codeset);
#else
        return OFString();
#endif
    }

    static OFBool supportsConversionFlags(const unsigned flags)
    {
#if defined(WITH_LIBICONV) && _LIBICONV_VERSION >= 0x0108
        return  flags == AbortTranscodingOnIllegalSequence
            || flags == DiscardIllegalSequences
            || flags == TransliterateIllegalSequences
            || flags == (DiscardIllegalSequences | TransliterateIllegalSequences)
            ;
#elif defined DCMTK_FIXED_ICONV_CONVERSION_FLAGS
        // the iconvctl function is implemented only in newer versions of the
        // GNU libiconv and not in other iconv implementations. For instance,
        // the iconv implementation in the C standard library does not support
        // different encoding flags and only has a (varying) fixed functionality
        // that we detect with a configuration test.
        return flags == DCMTK_FIXED_ICONV_CONVERSION_FLAGS;
#else
        return OFFalse;
#endif
    }

    unsigned getConversionFlags() const
    {
#if defined(WITH_LIBICONV) && _LIBICONV_VERSION >= 0x0108
        unsigned result = 0;
        int flag;
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV
        if (::iconvctl(ConversionDescriptor, ICONV_GET_ILSEQ_INVALID, &flag))
            return 0;
        if (flag == 0)
            result |= TransliterateIllegalSequences;
#else
        if (::iconvctl(ConversionDescriptor, ICONV_GET_TRANSLITERATE, &flag))
            return 0;
        if (flag)
            result |= TransliterateIllegalSequences;
#endif
        if (::iconvctl(ConversionDescriptor, ICONV_GET_DISCARD_ILSEQ, &flag))
            return 0;
        if (flag)
            result |= DiscardIllegalSequences;
        if (result)
            return result;
        // no flags set, so return libiconv default (=abort)
        return AbortTranscodingOnIllegalSequence;
#elif defined DCMTK_FIXED_ICONV_CONVERSION_FLAGS
        // the iconvctl function is implemented only in newer versions of the
        // GNU libiconv and not in other iconv implementations. For instance,
        // the iconv implementation in the C standard library does not support
        // different encoding flags and only has a (varying) fixed functionality
        // that we detect with a configuration test.
        return DCMTK_FIXED_ICONV_CONVERSION_FLAGS;
#else
        return 0;
#endif
    }

    OFBool setConversionFlags(const unsigned flags)
    {
#if defined(WITH_LIBICONV) && _LIBICONV_VERSION >= 0x0108
        int flag0 = 0;
        int flag1 = 1;
        switch (flags)
        {
            case AbortTranscodingOnIllegalSequence:
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV
                if (::iconvctl(ConversionDescriptor, ICONV_SET_DISCARD_ILSEQ, &flag0))
                    return OFFalse;
                if (::iconvctl(ConversionDescriptor, ICONV_SET_ILSEQ_INVALID, &flag1))
                    return OFFalse;
#else
                if (::iconvctl(ConversionDescriptor, ICONV_SET_DISCARD_ILSEQ, &flag0))
                    return OFFalse;
                if (::iconvctl(ConversionDescriptor, ICONV_SET_TRANSLITERATE, &flag0))
                    return OFFalse;
#endif
                return OFTrue;
            case DiscardIllegalSequences:
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV
                if (::iconvctl(ConversionDescriptor, ICONV_SET_DISCARD_ILSEQ, &flag1))
                    return OFFalse;
                if (::iconvctl(ConversionDescriptor, ICONV_SET_ILSEQ_INVALID, &flag0))
                    return OFFalse;
#else
                if (::iconvctl(ConversionDescriptor, ICONV_SET_DISCARD_ILSEQ, &flag1))
                    return OFFalse;
                if (::iconvctl(ConversionDescriptor, ICONV_SET_TRANSLITERATE, &flag0))
                    return OFFalse;
#endif
                return OFTrue;
            case TransliterateIllegalSequences:
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV
                if (::iconvctl(ConversionDescriptor, ICONV_SET_DISCARD_ILSEQ, &flag0))
                    return OFFalse;
                if (::iconvctl(ConversionDescriptor, ICONV_SET_ILSEQ_INVALID, &flag0))
                    return OFFalse;
#else
                if (::iconvctl(ConversionDescriptor, ICONV_SET_DISCARD_ILSEQ, &flag0))
                    return OFFalse;
                if (::iconvctl(ConversionDescriptor, ICONV_SET_TRANSLITERATE, &flag1))
                    return OFFalse;
#endif
                return OFTrue;
            case (TransliterateIllegalSequences | DiscardIllegalSequences):
#if DCMTK_ENABLE_CHARSET_CONVERSION == DCMTK_CHARSET_CONVERSION_OFICONV
                if (::iconvctl(ConversionDescriptor, ICONV_SET_DISCARD_ILSEQ, &flag1))
                    return OFFalse;
                if (::iconvctl(ConversionDescriptor, ICONV_SET_ILSEQ_INVALID, &flag0))
                    return OFFalse;
#else
                if (::iconvctl(ConversionDescriptor, ICONV_SET_DISCARD_ILSEQ, &flag1))
                    return OFFalse;
                if (::iconvctl(ConversionDescriptor, ICONV_SET_TRANSLITERATE, &flag1))
                    return OFFalse;
#endif
                return OFTrue;
            default:
                return OFFalse;
        }
#elif defined DCMTK_FIXED_ICONV_CONVERSION_FLAGS
        // the iconvctl function is implemented only in newer versions of the
        // GNU libiconv and not in other iconv implementations. For instance,
        // the iconv implementation in the C standard library does not support
        // different encoding flags and only has a (varying) fixed functionality
        // that we detect with a configuration test.
        return flags == DCMTK_FIXED_ICONV_CONVERSION_FLAGS;
#else
        return OFFalse;
#endif
    }


    OFCondition convert(OFString& toString,
                        const char* fromString,
                        const size_t fromLength)
    {
        OFCondition status = EC_Normal;
        // if the input string is empty or NULL, we are done
        if (status.good() && (fromString != NULL) && (fromLength > 0))
        {
#ifdef LIBICONV_SECOND_ARGUMENT_CONST
            const char *inputPos = fromString;
#else
            char *inputPos = OFconst_cast(char *, fromString);
#endif
            size_t inputLeft = fromLength;
            // set the conversion descriptor to the initial state
            ::iconv(ConversionDescriptor, NULL, NULL, NULL, NULL);
            // iterate as long as there are characters to be converted
            while (inputLeft > 0)
            {
                char buffer[CONVERSION_BUFFER_SIZE];
                char *bufferPos = buffer;
                const size_t bufferLength = sizeof(buffer);
                size_t bufferLeft = bufferLength;
                // convert the current block of the given string to the selected character encoding
                if (::iconv(ConversionDescriptor, &inputPos, &inputLeft, &bufferPos, &bufferLeft) == CONVERSION_ERROR)
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
        return status;
    }

    ~Implementation()
    {
        // try to close given descriptor and check whether it worked
        if (::iconv_close(ConversionDescriptor) == -1)
        {
            char errBuf[256];
            CERR << "Cannot close character encoding: "
                 << OFStandard::strerror(errno, errBuf, sizeof(errBuf))
                 << OFendl;
        }
    }

  private:

#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_SHADOW
    Implementation(iconv_t ConversionDescriptor)
    : ConversionDescriptor(ConversionDescriptor)
    {

    }
#include DCMTK_DIAGNOSTIC_POP

    static void createErrnoCondition(OFCondition &status,
                                     OFString message,
                                     const unsigned short code)
    {
        char errBuf[256];
        message.append(OFStandard::strerror(errno, errBuf, sizeof(errBuf)));
        status = makeOFCondition(0, code, OF_error, message.c_str());
    }

    iconv_t ConversionDescriptor;
};

#endif // ICONV

#else // DCMTK_ENABLE_CHARSET_CONVERSION

// for suppressing unnecessary warnings
class OFCharacterEncoding::Implementation {};

#endif // NOT DCMTK_ENABLE_CHARSET_CONVERSION


OFBool OFCharacterEncoding::isLibraryAvailable()
{
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
    return OFTrue;
#else
    return OFFalse;
#endif
}


OFString OFCharacterEncoding::getLibraryVersionString()
{
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
    return Implementation::getVersionString();
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

OFBool OFCharacterEncoding::hasDefaultEncoding()
{
#if defined(DCMTK_ENABLE_CHARSET_CONVERSION) &&\
    (\
        DCMTK_ENABLE_CHARSET_CONVERSION != DCMTK_CHARSET_CONVERSION_STDLIBC_ICONV\
     || DCMTK_STDLIBC_ICONV_HAS_DEFAULT_ENCODING\
    )
    return OFTrue;
#else
    return OFFalse;
#endif
}

OFString OFCharacterEncoding::getLocaleEncoding()
{
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
    return Implementation::getLocaleEncoding();
#else
    return OFString();
#endif
}


#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
OFBool OFCharacterEncoding::supportsConversionFlags(const unsigned flags)
{
    return Implementation::supportsConversionFlags(flags);
}
#else
OFBool OFCharacterEncoding::supportsConversionFlags(const unsigned /* flags */)
{
    return OFFalse;
}
#endif


OFCharacterEncoding::OFCharacterEncoding()
  : TheImplementation()
{

}


OFCharacterEncoding::OFCharacterEncoding(const OFCharacterEncoding& rhs)
  : TheImplementation(rhs.TheImplementation)
{

}


OFCharacterEncoding::~OFCharacterEncoding()
{

}


OFCharacterEncoding& OFCharacterEncoding::operator=(const OFCharacterEncoding& rhs)
{
    TheImplementation = rhs.TheImplementation;
    return *this;
}


OFCharacterEncoding::operator OFBool() const
{
    return OFstatic_cast(OFBool, TheImplementation);
}


OFBool OFCharacterEncoding::operator!() const
{
    return !TheImplementation;
}


OFBool OFCharacterEncoding::operator==(const OFCharacterEncoding& rhs) const
{
    return TheImplementation == rhs.TheImplementation;
}

OFBool OFCharacterEncoding::operator!=(const OFCharacterEncoding& rhs) const
{
    return TheImplementation != rhs.TheImplementation;
}


void OFCharacterEncoding::clear()
{
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
    TheImplementation.reset();
#endif
}


unsigned OFCharacterEncoding::getConversionFlags() const
{
#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
    if (TheImplementation)
        return TheImplementation->getConversionFlags();
#endif
    return 0;
}


#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
OFCondition OFCharacterEncoding::setConversionFlags(const unsigned flags)
{
    if (TheImplementation)
    {
        if (TheImplementation->setConversionFlags(flags))
            return EC_Normal;
        return makeOFCondition(0, EC_CODE_CannotControlConverter, OF_error,
            "Conversion flags not supported by the underlying implementation");
    }
    return EC_NoEncodingSelected;
}
#else
OFCondition OFCharacterEncoding::setConversionFlags(const unsigned /* flags */)
{
    return EC_NoEncodingLibrary;
}
#endif


#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
OFCondition OFCharacterEncoding::selectEncoding(const OFString &fromEncoding,
                                                const OFString &toEncoding)
{
    OFCondition result;
    TheImplementation.reset(Implementation::create(fromEncoding, toEncoding, result));
    return result;
}
#else
OFCondition OFCharacterEncoding::selectEncoding(const OFString & /* fromEncoding */,
                                                const OFString & /* toEncoding */)
{
    return EC_NoEncodingLibrary;
}
#endif


#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
OFCondition OFCharacterEncoding::convertString(const OFString &fromString,
                                               OFString &toString,
                                               const OFBool clearMode)
{
    return convertString(fromString.c_str(), fromString.length(), toString, clearMode);
}
#else
OFCondition OFCharacterEncoding::convertString(const OFString & /* fromString */,
                                               OFString & /* toString */,
                                               const OFBool /* clearMode */)
{
    return EC_NoEncodingLibrary;
}
#endif


#ifdef DCMTK_ENABLE_CHARSET_CONVERSION
OFCondition OFCharacterEncoding::convertString(const char *fromString,
                                               const size_t fromLength,
                                               OFString &toString,
                                               const OFBool clearMode)
{
    if (TheImplementation)
    {
        // first, clear result variable if requested
        if (clearMode)
            toString.clear();
        return TheImplementation->convert(toString, fromString, fromLength);
    }
    return EC_NoEncodingSelected;
}
#else
OFCondition OFCharacterEncoding::convertString(const char * /* fromString */,
                                               const size_t /* fromLength */,
                                               OFString & /* toString */,
                                               const OFBool /* clearMode */)
{
    return EC_NoEncodingLibrary;
}
#endif


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
        const int sizeNeeded = WideCharToMultiByte(codePage, 0, fromString, OFstatic_cast(int, fromLength), NULL, 0, NULL, NULL);
        if (sizeNeeded > 0)
        {
            // allocate temporary buffer
            char *toBuffer = new char[sizeNeeded];
            if (toBuffer != NULL)
            {
                // convert characters (without trailing NULL byte)
                const int charsConverted = WideCharToMultiByte(codePage, 0, fromString, OFstatic_cast(int, fromLength), toBuffer, sizeNeeded, NULL, NULL);
                if (charsConverted > 0)
                {
                    // append the converted character string to the result variable
                    toString.append(toBuffer, charsConverted);
                } else {
                    // if conversion failed, create appropriate condition text
                    createGetLastErrorCondition(status, "Cannot convert character encoding: ", EC_CODE_CannotConvertEncoding);
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
        const int sizeNeeded = MultiByteToWideChar(codePage, 0, fromString, OFstatic_cast(int, fromLength), NULL, 0);
        // allocate output buffer (one extra byte for the terminating NULL)
        toString = new wchar_t[sizeNeeded + 1];
        if (toString != NULL)
        {
            // convert characters (without trailing NULL byte)
            toLength = MultiByteToWideChar(codePage, 0, fromString, OFstatic_cast(int, fromLength), toString, sizeNeeded);
            // append NULL byte to mark "end of string"
            toString[toLength] = L'\0';
            if (toLength == 0)
            {
                // if conversion failed, create appropriate condition text
                createGetLastErrorCondition(status, "Cannot convert character encoding: ", EC_CODE_CannotConvertEncoding);
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


void OFCharacterEncoding::createGetLastErrorCondition(OFCondition &status,
                                                      OFString message,
                                                      const unsigned short code)
{
    LPVOID errBuf = NULL;
    // obtain an error string from system error code
    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), OFreinterpret_cast(LPTSTR, &errBuf), 0, NULL) > 0)
    {
        message.append(OFstatic_cast(const char *, errBuf));
    } else
        message.append("unknown error code");
    LocalFree(errBuf);
    status = makeOFCondition(0, code, OF_error, message.c_str());
}

#endif  // HAVE_WINDOWS_H
