/*
 *
 *  Copyright (C) 1994-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: class DcmPersonName
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-11 12:28:56 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcvrpn.cc,v $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcvrpn.h"
#include "dcdebug.h"


// ********************************


DcmPersonName::DcmPersonName(const DcmTag &tag, const Uint32 len)
: DcmCharString(tag, len)
{
    maxLength = 64;     // not correct: max length of PN is 3*64+2 = 194 characters (not bytes!)
}


// ********************************


DcmPersonName::DcmPersonName(const DcmPersonName& old)
: DcmCharString(old)
{
}


// ********************************


DcmPersonName::~DcmPersonName(void)
{
}


// ********************************


OFCondition
DcmPersonName::getOFString(
    OFString & str,
    const unsigned long pos,
    OFBool normalize)
{
    OFCondition l_error = DcmCharString::getOFString(str, pos, normalize);
    if (l_error.good() && normalize)
	    normalizeString(str, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


OFCondition
DcmPersonName::getOFStringArray(
    OFString & str,
    OFBool normalize)
{
    OFCondition l_error = DcmCharString::getOFStringArray(str, normalize);
    if (l_error.good() && normalize)
	    normalizeString(str, MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition
DcmPersonName::getNameComponents(
    OFString &lastName,
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


OFCondition
DcmPersonName::getNameComponentsFromString(
    const OFString &dicomName,
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


OFCondition
DcmPersonName::getFormattedName(
    OFString &formattedName,
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


OFCondition
DcmPersonName::getFormattedNameFromString(
    const OFString &dicomName,
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


OFCondition
DcmPersonName::getFormattedNameFromComponents(
    const OFString &lastName,
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


/*
** CVS/RCS Log:
** $Log: dcvrpn.cc,v $
** Revision 1.14  2002-04-11 12:28:56  joergr
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
