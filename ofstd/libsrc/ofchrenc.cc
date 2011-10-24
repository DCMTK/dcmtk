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
 *  Purpose: Class for character encoding conversion (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-10-24 13:03:09 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofchrenc.h"
#include "dcmtk/ofstd/ofstd.h"

#ifdef WITH_LIBICONV
#include <iconv.h>
#include <localcharset.h>
#endif


#define ILLEGAL_DESCRIPTOR     OFreinterpret_cast(void *, -1)
#define CONVERSION_ERROR       OFstatic_cast(size_t, -1)
#define CONVERSION_BUFFER_SIZE 1024


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
    // close any still open conversion descriptor
    closeConversionDescriptor();
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
#if _LIBICONV_VERSION >= 0x0108
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
#if _LIBICONV_VERSION >= 0x0108
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
    LocaleEncoding = ::locale_charset();
    // the above function always returns a non-empty string
    return EC_Normal;
#else
    return EC_NoEncodingLibrary;
#endif
}


OFCondition OFCharacterEncoding::selectEncoding(const OFString &fromEncoding,
                                                const OFString &toEncoding)
{
#ifdef WITH_LIBICONV
    OFCondition status = EC_Normal;
    // first, close any open conversion descriptor
    closeConversionDescriptor();
    ConversionDescriptor = ::iconv_open(toEncoding.c_str(), fromEncoding.c_str());
    // check whether the conversion descriptor could be allocated
    if (!isConversionDescriptorValid())
    {
        // if not, return with an appropriate error message
        createErrnoCondition(status, "Cannot select character encoding: ",
            EC_CODE_CannotSelectEncoding);
    }
    return status;
#else
    return EC_NoEncodingLibrary;
#endif
}


OFCondition OFCharacterEncoding::convertString(const OFString &fromString,
                                               OFString &toString,
                                               const OFBool clear)
{
    // first, clear result variable if requested
    if (clear)
        toString.clear();
#ifdef WITH_LIBICONV
    OFCondition status = EC_Normal;
    // check whether a conversion descriptor has been allocated
    if (isConversionDescriptorValid())
    {
#if _LIBICONV_VERSION >= 0x0108
        // enable/disable transliteration (use of similar characters) in the conversion
        int translit = (TransliterationMode) ? 1 : 0;
        if (::iconvctl(ConversionDescriptor, ICONV_SET_TRANSLITERATE, &translit) < 0)
        {
            // if this didn't work, return with an appropriate error message
            createErrnoCondition(status, "Cannot control character encoding feature TRANSLITERATE: ",
                EC_CODE_CannotControlConverter);
        }
        // enable/disable discarding of illegal sequences in the conversion
        int discard = (DiscardIllegalSequenceMode) ? 1 : 0;
        if (::iconvctl(ConversionDescriptor, ICONV_SET_DISCARD_ILSEQ, &discard) < 0)
        {
            // if this didn't work, return with an appropriate error message
            createErrnoCondition(status, "Cannot control character encoding feature DISCARD_ILSEQ: ",
                EC_CODE_CannotControlConverter);
        }
#endif
        // if the input string is empty, we are done
        if (status.good() && !fromString.empty())
        {
#ifdef _WIN32
            const char *inputPos = fromString.c_str();
#else
            char *inputPos = OFconst_cast(char *, fromString.c_str());
#endif
            size_t inputLeft = fromString.length();
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
    } else
        status = EC_NoEncodingSelected;
    return status;
#else
    return EC_NoEncodingLibrary;
#endif
}


void OFCharacterEncoding::closeConversionDescriptor()
{
#ifdef WITH_LIBICONV
    if (isConversionDescriptorValid())
    {
        ::iconv_close(ConversionDescriptor);
        // make the descriptor invalid
        ConversionDescriptor = ILLEGAL_DESCRIPTOR;
    }
#endif
}


OFBool OFCharacterEncoding::isConversionDescriptorValid() const
{
    return (ConversionDescriptor != ILLEGAL_DESCRIPTOR);
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


/*
 *
 * CVS/RCS Log:
 * $Log: ofchrenc.cc,v $
 * Revision 1.3  2011-10-24 13:03:09  joergr
 * Changed name of status code constant for "illegal character encoding".
 *
 * Revision 1.2  2011-10-24 12:49:34  joergr
 * Made sure that iconvctl() is really supported by the libiconv toolkit.
 *
 * Revision 1.1  2011-10-21 09:14:59  joergr
 * Added class for managing and converting between different character encodings
 * based on the libiconv toolkit.
 *
 *
 */
