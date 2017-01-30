/*
 *
 *  Copyright (C) 1994-2016, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Andreas Barth, Joerg Riesmeier
 *
 *  Purpose: Implementation of class DcmPersonName
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/dcmdata/dcvrpn.h"


// ********************************


const char* const DcmPersonName::componentGroupNames[3] = { "Alphabetic", "Ideographic", "Phonetic" };


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
                                      const OFBool /*oldFormat*/)
{
    OFString strVal;
    /* get "raw value" without any modifications (if possible) */
    OFCondition l_error = getStringValue(strVal);
    if (l_error.good())
    {
        OFString charset;
        /* try to determine the value of the SpecificCharacterSet element */
        getSpecificCharacterSet(charset);
        l_error = DcmPersonName::checkStringValue(strVal, vm, charset);
    }
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


OFCondition DcmPersonName::writeXML(STD_NAMESPACE ostream &out,
                                    const size_t flags)
{
    /* PN requires special handling in the Native DICOM Model format */
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* write normal XML start tag */
        DcmElement::writeXMLStartTag(out, flags);
        /* if the value is empty, we do not need to insert any PersonName attribute at all */
        if (!isEmpty())
        {
            /* condition variable, not meant to be used as a result of this method */
            OFCondition result;
            /* iterate over multiple Person Names if necessary */
            const unsigned long vm = getVM();
            /* strings to hold family, first, and middle name as well as prefix and suffix component */
            OFString components[5];
            /* arrays in order to permit looping while creating the output */
            const char* compNames[5] = { "FamilyName", "GivenName", "MiddleName", "NamePrefix", "NameSuffix" };
            for (unsigned int it = 0; it < vm; it++)
            {
                out << "<PersonName number=\"" << (it + 1) << "\">" << OFendl;
                OFString allGroups, oneCompGroup;
                result = getOFString(allGroups, it);
                if (result.good())
                {
                    /* process alphabetic, ideographic and phonetic encoding, as available */
                    for (unsigned int cg = 0; cg < 3; cg++)
                    {
                        /* get one component group (more efficient to check for non-zero length on whole group later) */
                        result = getComponentGroup(allGroups, cg, oneCompGroup);
                        if (result.good() && !oneCompGroup.empty())
                        {
                            /* get all name components from current group, i.e. last, first, middle name, prefix, suffix.
                             * uses single group, so the component group parameter is always 0
                             */
                            result = getNameComponentsFromString(oneCompGroup, components[0], components[1], components[2], components[3], components[4], 0);
                        }
                        /* output one component group, e.g. <SingleByte> <FamilyName>Onken</FamilyName> </SingleByte> */
                        if (result.good())
                        {
                            out << "<" << componentGroupNames[cg] << ">" << OFendl; // e.g. <SingleByte>
                            /* go through components (last name, first name, ...) */
                            for (unsigned short c = 0; c < 5; c++)
                            {
                                if (!components[c].empty())
                                {
                                    /* output name component, e.g. <FamilyName>Onken</FamilyName> */
                                    out << "<" << compNames[c] << ">" << components[c] << "</" << compNames[c] << ">" << OFendl;
                                }
                            }
                            out << "</" << componentGroupNames[cg] << ">" << OFendl; // e.g. </SingleByte>
                        }
                    }
                }
                out << "</PersonName>" << OFendl;
            }
        }
        /* write normal XML end tag */
        DcmElement::writeXMLEndTag(out, flags);
        /* always report success */
        return EC_Normal;
    } else  {
        /* DCMTK-specific format does not require anything special */
        return DcmElement::writeXML(out, flags);
    }
}


// ********************************


OFCondition DcmPersonName::writeJson(STD_NAMESPACE ostream &out,
                                     DcmJsonFormat &format)
{
    // helper struct to operate on the value pointers
    struct Lexer
    {
        // initializes everything with foolproof defaults
        Lexer()
        : it(OFnullptr)
        , end(OFnullptr)
        , componentGroupName(componentGroupNames)
        , currentComponent(0)
        , hasTrailingNull(OFFalse)
        {

        }

        // resets component groups and components for the new value and
        // remembers that there was a value separator, so that trailing
        // 'null' values can be printed.
        void handleValue()
        {
            currentComponent = 0;
            componentGroupName = componentGroupNames;
            hasTrailingNull = OFTrue;
        }

        // check component group validity and set members
        // appropriately
        // emit warnings for invalid component groups
        void handleComponentGroup()
        {
            if (componentGroupName != componentGroupNames + 2)
            {
                currentComponent = 0;
                ++componentGroupName;
            }
            else
            {
                DCMDATA_ERROR("DcmPersonName::writeJson(): omitting invalid "
                    "component group (more than three component groups present)");
                // seek to the end of the current component group - 1, since
                // the iterator will be incremented in the calling code
                while (++it != end && *it != '\\' && *it != '=');
                --it;
            }
        }

        // searches for the next significant character
        // (no delimiter or padding character).
        // keeps track of the component group name and component
        // number.
        // returns OFFalse if no more significant characters exist.
        OFBool nextValue()
        {
            for (; it != end; ++it) switch (*it)
            {
            case '\\':
                handleValue();
                return OFTrue;
            case '=':
                handleComponentGroup();
                break;
            case '^':
                ++currentComponent;
                break;
            case ' ':
                break;
            default:
                return OFTrue;
            }
            // this ends the output, unless we have to write
            // a trailing 'null'
            return hasTrailingNull;
        }

        // searches for the next significant character
        // (no delimiter or padding character)
        // within the current value.
        // keeps track of the current component group and component
        // number.
        // returns OFFalse if no more significant characters exist
        // within the current value.
        OFBool nextComponentGroup()
        {
            for (; it != end; ++it) switch(*it)
            {
            case '\\':
                // skip over any value separator
                ++it;
                handleValue();
                return OFFalse;
            case '=':
                handleComponentGroup();
                break;
            case '^':
                ++currentComponent;
                break;
            case ' ':
                break;
            default:
                return OFTrue;
            }
            return OFFalse;
        }

        // searches for the next significant character
        // (no delimiter or padding character)
        // within the current component group.
        // keeps track of the current component number.
        // returns OFFalse if no more significant characters exist
        // within the current component group.
        OFBool nextComponent()
        {
            for (; it != end; ++it) switch(*it)
            {
            case '\\':
            case '=':
                return OFFalse;
            case '^':
                ++currentComponent;
                break;
            case ' ':
                break;
            default:
                return OFTrue;
            }
            return OFFalse;
        }

        // writes all significant characters of the current component,
        // prepending '^' characters as required by the current component
        // number.
        void writeComponent(STD_NAMESPACE ostream &out)
        {
            for (; currentComponent; --currentComponent)
                out << '^';
            const char* const begin = it;
            while (++it != end && *it != '\\' && *it != '=' && *it != '^');
            const char* componentEnd = it - 1;
            while (*componentEnd == ' ')
                --componentEnd;
            out.write(begin, componentEnd - begin + 1);
        }

        // writes the name of the current component group and all components
        // within the current component group that contain at least one significant
        // character.
        // All others are replaced by a single '^' character each, except for the
        // trailing ones (that are omitted).
        void writeComponentGroup(STD_NAMESPACE ostream &out, DcmJsonFormat &format)
        {
            out << format.indent() << '"' << *componentGroupName << "\":"
                << format.space() << '"';
            writeComponent(out);
            while (nextComponent())
                writeComponent(out);
            out << '"';
        }

        // writes all component groups of the current value that contain at least
        // a single significant character, separated by ',' characters.
        // all other component groups are omitted entirely.
        // writes 'null' if all component groups are empty.
        void writeCurrentValue(STD_NAMESPACE ostream &out, DcmJsonFormat &format)
        {
            hasTrailingNull = OFFalse;
            if (nextComponentGroup())
            {
                out << '{' << format.newline();
                ++format.indent();
                writeComponentGroup(out, format);
                while (nextComponentGroup())
                {
                    out << ',' << format.newline();
                    writeComponentGroup(out, format);
                }
                out << format.newline() << --format.indent() << '}';
            }
            else out << "null";
        }

        // points to the character that is currently being processed
        const char* it;

        // points to the end of the entire attribute's data, including
        // all values
        const char* const end;

        // points to the respective entry of the componentGroupNames
        // array that contains the name of the component group that
        // is currently being processed
        const char* const * componentGroupName;

        // denotes the number of empty components within the current
        // component group since the last component that contained
        // at least one significant character.
        unsigned currentComponent;

        // will be OFTrue if the last value is empty, so that a
        // trailing 'null' will be written
        OFBool hasTrailingNull;
    } lexer;

    // initialize the lexer, return an error if the data
    // of the current attribute can't be accessed for whatever
    // reason
    OFCondition result = DcmCharString::getString(OFconst_cast(char*&, lexer.it));
    if (result.bad())
        return result;
    OFconst_cast(const char*&, lexer.end) = lexer.it + DcmCharString::getLength();

    // write attribute name etc., even if the attribute is empty
    DcmElement::writeJsonOpener(out, format);

    // search for the first significant character within the
    // entire data or the first value separator '\'
    if (lexer.nextValue())
    {
        // write the fist value or 'null'
        format.printValuePrefix(out);
        lexer.writeCurrentValue(out, format);
        // search for the first significant character within
        // the next value or the next '\'
        while (lexer.nextValue())
        {
            // write array element separator and the next
            // value or 'null'
            format.printNextArrayElementPrefix(out);
            lexer.writeCurrentValue(out, format);
        }
        // close the array after the last value
        format.printValueSuffix(out);
    }

    // close the current attribute
    DcmElement::writeJsonCloser(out, format);

    return EC_Normal;
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
    if (!dicomName.empty())
    {
        /* Excerpt from DICOM part 5:
           "For the purpose of writing names in ideographic characters and in
            phonetic characters, up to 3 groups of components may be used."
        */
        OFString name;
        l_error = getComponentGroup(dicomName, componentGroup, name);
        /* check whether component group is valid (= non-empty) */
        if (l_error.good() && !name.empty())
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


OFCondition DcmPersonName::getComponentGroup(const OFString &allCmpGroups,
                                             const unsigned int groupNo,
                                             OFString &cmpGroup)
{
    OFCondition l_error = EC_IllegalParameter;
    cmpGroup.clear();
    /* Excerpt from DICOM part 5:
       "For the purpose of writing names in ideographic characters and in
        phonetic characters, up to 3 groups of components may be used."
    */
    if (groupNo < 3)
    {
        // find component group (0..2)
        const size_t posA = allCmpGroups.find('=');
        if (posA != OFString_npos)
        {
            if (groupNo > 0)
            {
                const size_t posB = allCmpGroups.find('=', posA + 1);
                if (posB != OFString_npos)
                {
                    if (groupNo == 1)
                        cmpGroup = allCmpGroups.substr(posA + 1, posB - posA - 1);
                    else /* groupNo == 2 */
                        cmpGroup = allCmpGroups.substr(posB + 1);
                    l_error = EC_Normal;
                }
                else if (groupNo == 1)
                {
                    cmpGroup = allCmpGroups.substr(posA + 1);
                    l_error = EC_Normal;
                }
            } else {
                /* groupNo == 0 */
                cmpGroup = allCmpGroups.substr(0, posA);
                l_error = EC_Normal;
            }
        }
        else if (groupNo == 0)
        {
            cmpGroup = allCmpGroups;
            l_error = EC_Normal;
        }
    }
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
    if (!namePrefix.empty())
        formattedName += namePrefix;
    if (!firstName.empty())
    {
        if (!formattedName.empty())
            formattedName += ' ';
        formattedName += firstName;
    }
    if (!middleName.empty())
    {
        if (!formattedName.empty())
            formattedName += ' ';
        formattedName += middleName;
    }
    if (!lastName.empty())
    {
        if (!formattedName.empty())
            formattedName += ' ';
        formattedName += lastName;
    }
    if (!nameSuffix.empty())
    {
        if (!formattedName.empty())
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
        l_error = putOFStringArray(dicomName);
    return l_error;
}


// ********************************


OFCondition DcmPersonName::checkStringValue(const OFString &value,
                                            const OFString &vm,
                                            const OFString &charset)
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
            if (dcmEnableVRCheckerForStringValues.get())
            {
                /* check for non-ASCII characters (if default character set used) */
                if (charset.empty() || (charset == "ISO_IR 6"))
                {
                    if (DcmByteString::containsExtendedCharacters(value.c_str() + posStart, length))
                    {
                        result = EC_InvalidCharacter;
                        break;
                    }
                }
                /* currently, the VR checker only supports ASCII and Latin-1 */
                if (charset.empty() || (charset == "ISO_IR 6") || (charset == "ISO_IR 100"))
                {
                    /* check value representation (VR) */
                    const int vrID = DcmElement::scanValue(value, "pn", posStart, length);
                    if (vrID != 11)
                    {
                        result = EC_ValueRepresentationViolated;
                        break;
                    }
                }
            }
            posStart = (posEnd == OFString_npos) ? posEnd : posEnd + 1;
        }
        if (result.good() && !vm.empty())
        {
            /* check value multiplicity (VM) */
            result = DcmElement::checkVM(vmNum, vm);
        }
    }
    return result;
}


// ********************************


const OFString& DcmPersonName::getDelimiterChars() const
{
    return DcmVR(EVR_PN).getDelimiterChars();
}
