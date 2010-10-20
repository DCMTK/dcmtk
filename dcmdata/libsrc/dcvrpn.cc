/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Purpose: Implementation of class DcmPersonName
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-20 16:44:17 $
 *  CVS/RCS Revision: $Revision: 1.25 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcvrpn.h"


// ********************************


DcmPersonName::DcmPersonName(const DcmTag &tag,
                             const Uint32 len)
  : DcmCharString(tag, len)
{
    setMaxLength(64);     // not correct: max length of PN is 3*64+2 = 194 characters (not bytes!)
    setNonSignificantChars(" \\^=");
}


DcmPersonName::DcmPersonName(const DcmPersonName& old)
  : DcmCharString(old)
{
}


DcmPersonName::~DcmPersonName()
{
}


DcmPersonName &DcmPersonName::operator=(const DcmPersonName &obj)
{
    DcmCharString::operator=(obj);
    return *this;
}


OFCondition DcmPersonName::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmPersonName &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmPersonName::ident() const
{
    return EVR_PN;
}


OFCondition DcmPersonName::checkValue(const OFString &vm,
                                      const OFBool oldFormat)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
        l_error = DcmPersonName::checkStringValue(strVal, vm, oldFormat);
    return l_error;
}


// ********************************


OFCondition DcmPersonName::getOFString(OFString &stringVal,
                                       const unsigned long pos,
                                       OFBool normalize)
{
    OFCondition l_error = DcmCharString::getOFString(stringVal, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmPersonName::getNameComponents(OFString &lastName,
                                             OFString &firstName,
                                             OFString &middleName,
                                             OFString &namePrefix,
                                             OFString &nameSuffix,
                                             const unsigned long pos,
                                             const unsigned int componentGroup)
{
    OFString dicomName;
    OFCondition l_error = getOFString(dicomName, pos);
    if (l_error.good())
        l_error = getNameComponentsFromString(dicomName, lastName, firstName, middleName, namePrefix, nameSuffix, componentGroup);
    else
    {
        lastName.clear();
        firstName.clear();
        middleName.clear();
        namePrefix.clear();
        nameSuffix.clear();
    }
    return l_error;
}


OFCondition DcmPersonName::getNameComponentsFromString(const OFString &dicomName,
                                                       OFString &lastName,
                                                       OFString &firstName,
                                                       OFString &middleName,
                                                       OFString &namePrefix,
                                                       OFString &nameSuffix,
                                                       const unsigned int componentGroup)
{
    OFCondition l_error = EC_Normal;
    /* initialize all name components */
    lastName.clear();
    firstName.clear();
    middleName.clear();
    namePrefix.clear();
    nameSuffix.clear();
    if (dicomName.length() > 0)
    {
        /* Excerpt from DICOM part 5:
           "For the purpose of writing names in ideographic characters and in
            phonetic characters, up to 3 groups of components may be used."
        */
        if (componentGroup < 3)
        {
            OFString name;
            // find component group (0..2)
            const size_t posA = dicomName.find('=');
            if (posA != OFString_npos)
            {
                if (componentGroup > 0)
                {
                    const size_t posB = dicomName.find('=', posA + 1);
                    if (posB != OFString_npos)
                    {
                        if (componentGroup == 1)
                            name = dicomName.substr(posA + 1, posB - posA - 1);
                        else /* componentGroup == 2 */
                            name = dicomName.substr(posB + 1);
                    } else if (componentGroup == 1)
                        name = dicomName.substr(posA + 1);
                } else /* componentGroup == 0 */
                    name = dicomName.substr(0, posA);
            } else if (componentGroup == 0)
                name = dicomName;
            /* check whether component group is valid (= non-empty) */
            if (name.length() > 0)
            {
                /* find caret separators */
                /* (tbd: add more sophisticated heuristics for comma and space separated names) */
                const size_t pos1 = name.find('^');
                if (pos1 != OFString_npos)
                {
                    const size_t pos2 = name.find('^', pos1 + 1);
                    lastName = name.substr(0, pos1);
                    if (pos2 != OFString_npos)
                    {
                        const size_t pos3 = name.find('^', pos2 + 1);
                        firstName = name.substr(pos1 + 1, pos2 - pos1 - 1);
                        if (pos3 != OFString_npos)
                        {
                            const size_t pos4 = name.find('^', pos3 + 1);
                            middleName = name.substr(pos2 + 1, pos3 - pos2 - 1);
                            if (pos4 != OFString_npos)
                            {
                                namePrefix = name.substr(pos3 + 1, pos4 - pos3 - 1);
                                nameSuffix = name.substr(pos4 + 1);
                            } else
                                namePrefix = name.substr(pos3 + 1);
                        } else
                            middleName = name.substr(pos2 + 1);
                    } else
                        firstName = name.substr(pos1 + 1);
                } else
                    lastName = name;
            }
        } else
            l_error = EC_IllegalParameter;
    }
    return l_error;
}


// ********************************


OFCondition DcmPersonName::getFormattedName(OFString &formattedName,
                                            const unsigned long pos,
                                            const unsigned int componentGroup)
{
    OFString dicomName;
    OFCondition l_error = getOFString(dicomName, pos);
    if (l_error.good())
        l_error = getFormattedNameFromString(dicomName, formattedName, componentGroup);
    else
        formattedName.clear();
    return l_error;
}


OFCondition DcmPersonName::getFormattedNameFromString(const OFString &dicomName,
                                                      OFString &formattedName,
                                                      const unsigned int componentGroup)
{
    OFString lastName, firstName, middleName, namePrefix, nameSuffix;
    OFCondition l_error = getNameComponentsFromString(dicomName, lastName, firstName, middleName, namePrefix, nameSuffix, componentGroup);
    if (l_error.good())
        l_error = getFormattedNameFromComponents(lastName, firstName, middleName, namePrefix, nameSuffix, formattedName);
    else
        formattedName.clear();
    return l_error;
}


OFCondition DcmPersonName::getFormattedNameFromComponents(const OFString &lastName,
                                                          const OFString &firstName,
                                                          const OFString &middleName,
                                                          const OFString &namePrefix,
                                                          const OFString &nameSuffix,
                                                          OFString &formattedName)
{
    formattedName.clear();
    /* concatenate name components */
    if (namePrefix.length() > 0)
        formattedName += namePrefix;
    if (firstName.length() > 0)
    {
        if (formattedName.length() > 0)
            formattedName += ' ';
        formattedName += firstName;
    }
    if (middleName.length() > 0)
    {
        if (formattedName.length() > 0)
            formattedName += ' ';
        formattedName += middleName;
    }
    if (lastName.length() > 0)
    {
        if (formattedName.length() > 0)
            formattedName += ' ';
        formattedName += lastName;
    }
    if (nameSuffix.length() > 0)
    {
        if (formattedName.length() > 0)
            formattedName += ", ";
        formattedName += nameSuffix;
    }
    return EC_Normal;
}


OFCondition DcmPersonName::getStringFromNameComponents(const OFString &lastName,
                                                       const OFString &firstName,
                                                       const OFString &middleName,
                                                       const OFString &namePrefix,
                                                       const OFString &nameSuffix,
                                                       OFString &dicomName)
{
    const size_t middleLen = middleName.length();
    const size_t prefixLen = namePrefix.length();
    const size_t suffixLen = nameSuffix.length();
    /* concatenate name components */
    dicomName = lastName;
    if (firstName.length() + middleLen + prefixLen + suffixLen > 0)
        dicomName += '^';
    dicomName += firstName;
    if (middleLen + prefixLen + suffixLen > 0)
        dicomName += '^';
    dicomName += middleName;
    if (prefixLen + suffixLen > 0)
        dicomName += '^';
    dicomName += namePrefix;
    if (suffixLen > 0)
        dicomName += '^';
    dicomName += nameSuffix;
    return EC_Normal;
}


// ********************************


OFCondition DcmPersonName::putNameComponents(const OFString &lastName,
                                             const OFString &firstName,
                                             const OFString &middleName,
                                             const OFString &namePrefix,
                                             const OFString &nameSuffix)
{
    OFString dicomName;
    /* concatenate name components */
    OFCondition l_error = getStringFromNameComponents(lastName, firstName, middleName, namePrefix, nameSuffix, dicomName);
    /* put element value */
    if (l_error.good())
        l_error = putString(dicomName.c_str());
    return l_error;
}


// ********************************


OFCondition DcmPersonName::checkStringValue(const OFString &value,
                                            const OFString &vm,
                                            const OFBool oldFormat)
{
    /* currently not checked: maximum length per component group (64 characters) */
    OFCondition result = EC_Normal;
    const size_t valLen = value.length();
    if (valLen > 0)
    {
      size_t posStart = 0;
      unsigned long vmNum = 0;
      /* iterate over all value components */
      while (posStart != OFString_npos)
      {
        ++vmNum;
        /* search for next component separator */
        const size_t posEnd = value.find('\\', posStart);
        const size_t length = (posEnd == OFString_npos) ? valLen - posStart : posEnd - posStart;
        /* check value representation */
        const int vrID = DcmElement::scanValue(value, "pn", posStart, length);
        if ((vrID != 11) && (!oldFormat || (vrID != 15)))
        {
          result = EC_ValueRepresentationViolated;
          break;
        }
        posStart = (posEnd == OFString_npos) ? posEnd : posEnd + 1;
      }
      if (result.good() && !vm.empty())
      {
        /* check value multiplicity */
        result = DcmElement::checkVM(vmNum, vm);
      }
    }
    return result;
}


/*
** CVS/RCS Log:
** $Log: dcvrpn.cc,v $
** Revision 1.25  2010-10-20 16:44:17  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.24  2010-10-14 13:14:10  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.23  2010-04-23 14:30:34  joergr
** Added new method to all VR classes which checks whether the stored value
** conforms to the VR definition and to the specified VM.
**
** Revision 1.22  2009-08-07 14:35:49  joergr
** Enhanced isEmpty() method by checking whether the data element value consists
** of non-significant characters only.
**
** Revision 1.21  2009-08-03 09:03:00  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.20  2008-07-17 10:31:32  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.19  2007-06-29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.18  2005/12/08 15:41:59  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.17  2003/05/20 09:15:14  joergr
** Added methods and static functions to compose a DICOM Person Name from five
** name components.
**
** Revision 1.16  2002/12/06 13:20:51  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.15  2002/04/25 10:32:45  joergr
** Removed getOFStringArray() implementation.
**
** Revision 1.14  2002/04/11 12:28:56  joergr
** Enhanced documentation.
**
** Revision 1.13  2001/10/10 15:22:05  joergr
** Updated comments.
**
** Revision 1.12  2001/10/01 15:04:44  joergr
** Introduced new general purpose functions to get/set person names, date, time
** and date/time.
**
** Revision 1.11  2001/09/25 17:19:59  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.10  2001/06/01 15:49:19  meichel
** Updated copyright header
**
** Revision 1.9  2000/03/08 16:26:49  meichel
** Updated copyright header.
**
** Revision 1.8  1999/03/31 09:25:55  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.7  1998/11/12 16:48:27  meichel
** Implemented operator= for all classes derived from DcmObject.
**
** Revision 1.6  1997/08/29 13:11:48  andreas
** Corrected Bug in getOFStringArray Implementation
**
** Revision 1.5  1997/08/29 08:33:00  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.4  1997/07/03 15:10:16  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.3  1996/01/05 13:27:52  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
