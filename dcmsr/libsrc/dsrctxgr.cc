/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRContextGroup
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctxgr.h"


DSRContextGroup::DSRContextGroup(const OFString &contextIdentifier,
                                 const OFString &mappingResource,
                                 const OFString &contextGroupVersion,
                                 const OFString &contextGroupUID,
                                 const DSRCodedEntryValue &selectedValue)
  : Identifier(contextIdentifier),
    MappingResource(mappingResource),
    Version(contextGroupVersion),
    UID(contextGroupUID),
    SelectedValue(selectedValue),
    ExtensibleMode(OFFalse),
    ExtendedCodes()
{
    /* by default, a context group non-extensible */
}


DSRContextGroup::~DSRContextGroup()
{
}


void DSRContextGroup::clear()
{
    SelectedValue.clear();
    ExtendedCodes.clear();
}


OFBool DSRContextGroup::hasSelectedValue() const
{
    return !SelectedValue.isEmpty();
}


OFCondition DSRContextGroup::selectValue(const DSRCodedEntryValue &codedEntryValue,
                                         const OFBool check,
                                         const OFBool definedContextGroup)
{
    OFCondition result = (check) ? checkCodedEntry(codedEntryValue, definedContextGroup) : EC_Normal;
    /* if coded entry is valid, select it as the current value */
    if (result.good())
        SelectedValue = codedEntryValue;
    return result;
}


OFBool DSRContextGroup::hasCodedEntry(const DSRCodedEntryValue &codedEntryValue) const
{
    /* check whether given coded entry is contained in one of the lists */
    return findCodedEntry(codedEntryValue).good();
}


OFCondition DSRContextGroup::findCodedEntry(const DSRCodedEntryValue &codedEntryValue) const
{
    OFCondition result = SR_EC_CodedEntryNotInContextGroup;
    OFListConstIterator(DSRCodedEntryValue) iter = ExtendedCodes.begin();
    OFListConstIterator(DSRCodedEntryValue) last = ExtendedCodes.end();
    /* iterate over coded entry list */
    while (iter != last)
    {
        /* if found, exit loop */
        if (*iter == codedEntryValue)
        {
            result = SR_EC_CodedEntryIsExtensionOfContextGroup;
            break;
        }
        ++iter;
    }
    return result;
}


OFCondition DSRContextGroup::addCodedEntry(const DSRCodedEntryValue &codedEntryValue)
{
    OFCondition result = EC_Normal;
    /* first, check whether context group is extensible */
    if (ExtensibleMode)
    {
        /* if so, add coded entry to the list */
        ExtendedCodes.push_back(codedEntryValue);
    } else {
        /* if not, return an error code */
        result = SR_EC_NonExtensibleContextGroup;
    }
    return result;
}


void DSRContextGroup::print(STD_NAMESPACE ostream &stream) const
{
    printHeader(stream);
    /* output currently selected coded entry (if there is one) */
    if (hasSelectedValue())
    {
        stream << "Selected value:" << OFendl;
        stream << "  ";
        SelectedValue.print(stream);
        stream << OFendl;
    }
    printCodes(stream);
}


OFCondition DSRContextGroup::checkSelectedValue(const OFBool definedContextGroup) const
{
    OFCondition result = EC_Normal;
    /* check whether a coded entry is selected as the current value */
    if (hasSelectedValue())
        result = checkCodedEntry(SelectedValue, definedContextGroup);
    return result;
}


// protected methods

void DSRContextGroup::printHeader(STD_NAMESPACE ostream &stream) const
{
    /* output some general information on the context group */
    stream << "CID " << getIdentifier() << OFendl;
    stream << "  Resource : " << getMappingResource() << OFendl;
    stream << "  Version  : " << getVersion() << OFendl;
    stream << "  UID      : " << getUID() << OFendl;
    stream << "  Type     : " << (isExtensible() ? "extensible" : "non-extensible") << OFendl;
}


void DSRContextGroup::printCodes(STD_NAMESPACE ostream &stream) const
{
    /* are there any extended codes at all? */
    if (!ExtendedCodes.empty())
    {
        stream << "Extended codes:" << OFendl;
        OFListConstIterator(DSRCodedEntryValue) iter = ExtendedCodes.begin();
        OFListConstIterator(DSRCodedEntryValue) last = ExtendedCodes.end();
        /* iterate over coded entry list */
        while (iter != last)
        {
            stream << "  ";
            /* print coded entry */
            iter->print(stream);
            stream << OFendl;
            ++iter;
        }
    }
}


OFCondition DSRContextGroup::checkCodedEntry(const DSRCodedEntryValue &codedEntryValue,
                                             const OFBool definedContextGroup) const
{
    OFCondition result = EC_Normal;
    /* check whether currently selected coded entry is valid */
    if (!codedEntryValue.isValid())
        result = SR_EC_InvalidValue;
    /* if so, also check whether the coded entry is allowed for this context group */
    else if (definedContextGroup && !hasCodedEntry(codedEntryValue))
        result = SR_EC_CodedEntryNotInContextGroup;
    return result;
}
