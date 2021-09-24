/*
 *
 *  Copyright (C) 1994-2021, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmUniqueIdentifier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcuid.h"

#define MAX_UI_LENGTH 64


// ********************************


DcmUniqueIdentifier::DcmUniqueIdentifier(const DcmTag &tag,
                                         const Uint32 len)
  : DcmByteString(tag, len)
{
    /* padding character is NULL not a space! */
    setPaddingChar('\0');
    setMaxLength(MAX_UI_LENGTH);
    setNonSignificantChars("\\");
}


DcmUniqueIdentifier::DcmUniqueIdentifier(const DcmUniqueIdentifier &old)
  : DcmByteString(old)
{
}


DcmUniqueIdentifier::~DcmUniqueIdentifier()
{
}


DcmUniqueIdentifier &DcmUniqueIdentifier::operator=(const DcmUniqueIdentifier &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


OFCondition DcmUniqueIdentifier::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmUniqueIdentifier &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmUniqueIdentifier::ident() const
{
    return EVR_UI;
}


OFCondition DcmUniqueIdentifier::checkValue(const OFString &vm,
                                            const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmUniqueIdentifier::checkStringValue(strVal, vm);
    return l_error;
}


// ********************************


void DcmUniqueIdentifier::print(STD_NAMESPACE ostream &out,
                                const size_t flags,
                                const int level,
                                const char * /*pixelFileName*/,
                                size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get string data (possibly multi-valued) */
        char *stringVal = NULL;
        Uint32 stringLen = 0;
        getString(stringVal, stringLen);
        if ((stringVal != NULL) && (stringLen > 0))
        {
            const char *symbol = NULL;
            if (!(flags & DCMTypes::PF_doNotMapUIDsToNames))
            {
                /* check whether UID number can be mapped to a UID name */
                symbol = dcmFindNameOfUID(stringVal);
            }
            if ((symbol != NULL) && (strlen(symbol) > 0))
            {
                const size_t bufSize = strlen(symbol) + 1 /* for "=" */ + 1;
                char *buffer = new char[bufSize];
                if (buffer != NULL)
                {
                    /* concatenate "=" and the UID name */
                    OFStandard::strlcpy(buffer, "=", bufSize);
                    OFStandard::strlcat(buffer, symbol, bufSize);
                    printInfoLine(out, flags, level, buffer, NULL /*tag*/, OFFalse /*isInfo*/);
                    /* delete temporary character buffer */
                    delete[] buffer;
                } else /* could not allocate buffer */
                    DcmByteString::print(out, flags, level);
            } else /* no symbol (UID name) found or mapping switched off */
                DcmByteString::print(out, flags, level);
        } else
            printInfoLine(out, flags, level, "(no value available)");
    } else
        printInfoLine(out, flags, level, "(not loaded)");
}


// ********************************


OFCondition DcmUniqueIdentifier::getOFString(OFString &stringVal,
                                             const unsigned long pos,
                                             OFBool normalize)
{
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING, getPaddingChar() /* NULL-byte */);
    return l_error;
}


// ********************************


OFCondition DcmUniqueIdentifier::putString(const char *stringVal)
{
    /* determine length of the string value */
    const size_t stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, OFstatic_cast(Uint32, stringLen));
}


OFCondition DcmUniqueIdentifier::putString(const char *stringVal,
                                           const Uint32 stringLen)
{
    const char *uid = stringVal;
    Uint32 uidLen = stringLen;
    /* check whether parameter contains a UID name instead of a UID number */
    if ((stringVal != NULL) && (stringVal[0] == '='))
    {
        uid = dcmFindUIDFromName(stringVal + 1);
        /* check whether UID name could be mapped to a UID number */
        if (uid == NULL)
        {
            DCMDATA_DEBUG("DcmUniqueIdentifier::putString() cannot map UID name '"
                << OFSTRING_GUARD(stringVal + 1) << "' to UID value");
            /* return with an error */
            return EC_UnknownUIDName;
        } else
            uidLen = OFstatic_cast(Uint32, strlen(uid));
    }
    /* call inherited method to set the UID string */
    return DcmByteString::putString(uid, uidLen);
}


// ********************************


OFCondition DcmUniqueIdentifier::makeMachineByteString(const Uint32 length)
{
    /* get string data */
    char *value = OFstatic_cast(char *, getValue());
    /* determine initial string length */
    const size_t len = (length == 0) ? getLengthField() : length;
    if ((value != NULL) && (len > 0))
    {
        /* check whether string representation is not the internal one */
        if (getStringMode() != DCM_MachineString)
        {
            /* check whether automatic input data correction is enabled */
            if (dcmEnableAutomaticInputDataCorrection.get())
            {
                /*
                ** Remove any leading, embedded, or trailing white space.
                ** This manipulation attempts to correct problems with
                ** incorrectly encoded UIDs which have been observed in
                ** some images.
                */
                size_t curPos = 0;
                for (size_t i = 0; i < len; i++)
                {
                   if (!isspace(OFstatic_cast(unsigned char, value[i])))
                      value[curPos++] = value[i];
                }
                /* there was at least one space character in the string */
                if (curPos < len)
                {
                    DCMDATA_WARN("DcmUniqueIdentifier: Element " << getTagName() << " " << getTag()
                        << " contains one or more space characters, which were removed");
                    /* remember new length */
                    const Uint32 newLen = OFstatic_cast(Uint32, curPos);
                    /* blank out all trailing characters */
                    while (curPos < len)
                        value[curPos++] = '\0';
                    /* call inherited method: re-computes the string length, etc. */
                    return DcmByteString::makeMachineByteString(newLen);
                }
            }
        }
    }
    /* call inherited method: re-computes the string length, etc. */
    return DcmByteString::makeMachineByteString(OFstatic_cast(Uint32, len));
}


// ********************************


OFCondition DcmUniqueIdentifier::checkStringValue(const OFString &value,
                                                  const OFString &vm)
{
    return DcmByteString::checkStringValue(value, vm, "ui", 9, MAX_UI_LENGTH);
}
