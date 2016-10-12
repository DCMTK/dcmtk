/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTPerformedProtocolCodeSequence
 *
 *  Generated automatically from DICOM PS 3.3-2016d
 *  File created on 2016-10-12 13:44:31
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtppcs.h"


// --- item class ---

DRTPerformedProtocolCodeSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    CodeMeaning(DCM_CodeMeaning),
    CodeValue(DCM_CodeValue),
    CodingSchemeDesignator(DCM_CodingSchemeDesignator),
    CodingSchemeVersion(DCM_CodingSchemeVersion),
    ContextGroupExtensionCreatorUID(DCM_ContextGroupExtensionCreatorUID),
    ContextGroupExtensionFlag(DCM_ContextGroupExtensionFlag),
    ContextGroupLocalVersion(DCM_ContextGroupLocalVersion),
    ContextGroupVersion(DCM_ContextGroupVersion),
    ContextIdentifier(DCM_ContextIdentifier),
    ContextUID(DCM_ContextUID),
    EquivalentCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    LongCodeValue(DCM_LongCodeValue),
    MappingResource(DCM_MappingResource),
    MappingResourceName(DCM_MappingResourceName),
    MappingResourceUID(DCM_MappingResourceUID),
    ProtocolContextSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    URNCodeValue(DCM_URNCodeValue)
{
}


DRTPerformedProtocolCodeSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    CodeMeaning(copy.CodeMeaning),
    CodeValue(copy.CodeValue),
    CodingSchemeDesignator(copy.CodingSchemeDesignator),
    CodingSchemeVersion(copy.CodingSchemeVersion),
    ContextGroupExtensionCreatorUID(copy.ContextGroupExtensionCreatorUID),
    ContextGroupExtensionFlag(copy.ContextGroupExtensionFlag),
    ContextGroupLocalVersion(copy.ContextGroupLocalVersion),
    ContextGroupVersion(copy.ContextGroupVersion),
    ContextIdentifier(copy.ContextIdentifier),
    ContextUID(copy.ContextUID),
    EquivalentCodeSequence(copy.EquivalentCodeSequence),
    LongCodeValue(copy.LongCodeValue),
    MappingResource(copy.MappingResource),
    MappingResourceName(copy.MappingResourceName),
    MappingResourceUID(copy.MappingResourceUID),
    ProtocolContextSequence(copy.ProtocolContextSequence),
    URNCodeValue(copy.URNCodeValue)
{
}


DRTPerformedProtocolCodeSequence::Item::~Item()
{
}


DRTPerformedProtocolCodeSequence::Item &DRTPerformedProtocolCodeSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        CodeMeaning = copy.CodeMeaning;
        CodeValue = copy.CodeValue;
        CodingSchemeDesignator = copy.CodingSchemeDesignator;
        CodingSchemeVersion = copy.CodingSchemeVersion;
        ContextGroupExtensionCreatorUID = copy.ContextGroupExtensionCreatorUID;
        ContextGroupExtensionFlag = copy.ContextGroupExtensionFlag;
        ContextGroupLocalVersion = copy.ContextGroupLocalVersion;
        ContextGroupVersion = copy.ContextGroupVersion;
        ContextIdentifier = copy.ContextIdentifier;
        ContextUID = copy.ContextUID;
        EquivalentCodeSequence = copy.EquivalentCodeSequence;
        LongCodeValue = copy.LongCodeValue;
        MappingResource = copy.MappingResource;
        MappingResourceName = copy.MappingResourceName;
        MappingResourceUID = copy.MappingResourceUID;
        ProtocolContextSequence = copy.ProtocolContextSequence;
        URNCodeValue = copy.URNCodeValue;
    }
    return *this;
}


void DRTPerformedProtocolCodeSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        CodeValue.clear();
        CodingSchemeDesignator.clear();
        CodingSchemeVersion.clear();
        CodeMeaning.clear();
        LongCodeValue.clear();
        URNCodeValue.clear();
        EquivalentCodeSequence.clear();
        ContextIdentifier.clear();
        ContextUID.clear();
        MappingResource.clear();
        MappingResourceUID.clear();
        MappingResourceName.clear();
        ContextGroupVersion.clear();
        ContextGroupExtensionFlag.clear();
        ContextGroupLocalVersion.clear();
        ContextGroupExtensionCreatorUID.clear();
        ProtocolContextSequence.clear();
    }
}


OFBool DRTPerformedProtocolCodeSequence::Item::isEmpty()
{
    return CodeValue.isEmpty() &&
           CodingSchemeDesignator.isEmpty() &&
           CodingSchemeVersion.isEmpty() &&
           CodeMeaning.isEmpty() &&
           LongCodeValue.isEmpty() &&
           URNCodeValue.isEmpty() &&
           EquivalentCodeSequence.isEmpty() &&
           ContextIdentifier.isEmpty() &&
           ContextUID.isEmpty() &&
           MappingResource.isEmpty() &&
           MappingResourceUID.isEmpty() &&
           MappingResourceName.isEmpty() &&
           ContextGroupVersion.isEmpty() &&
           ContextGroupExtensionFlag.isEmpty() &&
           ContextGroupLocalVersion.isEmpty() &&
           ContextGroupExtensionCreatorUID.isEmpty() &&
           ProtocolContextSequence.isEmpty();
}


OFBool DRTPerformedProtocolCodeSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, CodeValue, "1", "1C", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, CodingSchemeDesignator, "1", "1C", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, CodingSchemeVersion, "1", "1C", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, CodeMeaning, "1", "1", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, LongCodeValue, "1", "1C", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, URNCodeValue, "1", "1C", "PerformedProtocolCodeSequence");
        EquivalentCodeSequence.read(item, "1-n", "3", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, ContextIdentifier, "1", "3", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, ContextUID, "1", "3", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, MappingResource, "1", "1C", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, MappingResourceUID, "1", "3", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, MappingResourceName, "1", "3", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, ContextGroupVersion, "1", "1C", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, ContextGroupExtensionFlag, "1", "3", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, ContextGroupLocalVersion, "1", "1C", "PerformedProtocolCodeSequence");
        getAndCheckElementFromDataset(item, ContextGroupExtensionCreatorUID, "1", "1C", "PerformedProtocolCodeSequence");
        ProtocolContextSequence.read(item, "1-n", "3", "PerformedProtocolCodeSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmShortString(CodeValue), "1", "1C", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmShortString(CodingSchemeDesignator), "1", "1C", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmShortString(CodingSchemeVersion), "1", "1C", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmLongString(CodeMeaning), "1", "1", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmUnlimitedCharacters(LongCodeValue), "1", "1C", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmUniversalResourceIdentifierOrLocator(URNCodeValue), "1", "1C", "PerformedProtocolCodeSequence");
        if (result.good()) result = EquivalentCodeSequence.write(item, "1-n", "3", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmCodeString(ContextIdentifier), "1", "3", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ContextUID), "1", "3", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmCodeString(MappingResource), "1", "1C", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(MappingResourceUID), "1", "3", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmLongString(MappingResourceName), "1", "3", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmDateTime(ContextGroupVersion), "1", "1C", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmCodeString(ContextGroupExtensionFlag), "1", "3", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmDateTime(ContextGroupLocalVersion), "1", "1C", "PerformedProtocolCodeSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ContextGroupExtensionCreatorUID), "1", "1C", "PerformedProtocolCodeSequence");
        if (result.good()) result = ProtocolContextSequence.write(item, "1-n", "3", "PerformedProtocolCodeSequence");
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getCodeMeaning(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodeMeaning, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getCodeValue(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodeValue, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getCodingSchemeDesignator(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodingSchemeDesignator, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getCodingSchemeVersion(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodingSchemeVersion, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getContextGroupExtensionCreatorUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextGroupExtensionCreatorUID, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getContextGroupExtensionFlag(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextGroupExtensionFlag, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getContextGroupLocalVersion(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextGroupLocalVersion, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getContextGroupVersion(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextGroupVersion, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getContextIdentifier(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextIdentifier, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getContextUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextUID, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getLongCodeValue(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LongCodeValue, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getMappingResource(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MappingResource, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getMappingResourceName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MappingResourceName, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getMappingResourceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MappingResourceUID, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::getURNCodeValue(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(URNCodeValue, value, pos);
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setCodeMeaning(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CodeMeaning.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setCodeValue(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CodeValue.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setCodingSchemeDesignator(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CodingSchemeDesignator.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setCodingSchemeVersion(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CodingSchemeVersion.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setContextGroupExtensionCreatorUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContextGroupExtensionCreatorUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setContextGroupExtensionFlag(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContextGroupExtensionFlag.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setContextGroupLocalVersion(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContextGroupLocalVersion.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setContextGroupVersion(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContextGroupVersion.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setContextIdentifier(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContextIdentifier.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setContextUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = ContextUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setLongCodeValue(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUnlimitedCharacters::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = LongCodeValue.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setMappingResource(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MappingResource.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setMappingResourceName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MappingResourceName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setMappingResourceUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MappingResourceUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::Item::setURNCodeValue(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniversalResourceIdentifierOrLocator::checkStringValue(value) : EC_Normal;
        if (result.good())
            result = URNCodeValue.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTPerformedProtocolCodeSequence::DRTPerformedProtocolCodeSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTPerformedProtocolCodeSequence::DRTPerformedProtocolCodeSequence(const DRTPerformedProtocolCodeSequence &copy)
  : EmptyDefaultSequence(copy.EmptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    /* create a copy of the internal sequence of items */
    Item *item = NULL;
    OFListConstIterator(Item *) current = copy.SequenceOfItems.begin();
    const OFListConstIterator(Item *) last = copy.SequenceOfItems.end();
    while (current != last)
    {
        item = new Item(**current);
        if (item != NULL)
        {
            SequenceOfItems.push_back(item);
        } else {
            /* memory exhausted, there is nothing we can do about it */
            break;
        }
        ++current;
    }
    CurrentItem = SequenceOfItems.begin();
}


DRTPerformedProtocolCodeSequence &DRTPerformedProtocolCodeSequence::operator=(const DRTPerformedProtocolCodeSequence &copy)
{
    if (this != &copy)
    {
        clear();
        EmptyDefaultSequence = copy.EmptyDefaultSequence;
        /* create a copy of the internal sequence of items */
        Item *item = NULL;
        OFListConstIterator(Item *) current = copy.SequenceOfItems.begin();
        const OFListConstIterator(Item *) last = copy.SequenceOfItems.end();
        while (current != last)
        {
            item = new Item(**current);
            if (item != NULL)
            {
                SequenceOfItems.push_back(item);
            } else {
                /* memory exhausted, there is nothing we can do about it */
                break;
            }
            ++current;
        }
        CurrentItem = SequenceOfItems.begin();
    }
    return *this;
}


DRTPerformedProtocolCodeSequence::~DRTPerformedProtocolCodeSequence()
{
    clear();
}


void DRTPerformedProtocolCodeSequence::clear()
{
    if (!EmptyDefaultSequence)
    {
        CurrentItem = SequenceOfItems.begin();
        const OFListConstIterator(Item *) last = SequenceOfItems.end();
        /* delete all items and free memory */
        while (CurrentItem != last)
        {
            delete (*CurrentItem);
            CurrentItem = SequenceOfItems.erase(CurrentItem);
        }
        /* make sure that the list is empty */
        SequenceOfItems.clear();
        CurrentItem = SequenceOfItems.end();
    }
}


OFBool DRTPerformedProtocolCodeSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTPerformedProtocolCodeSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTPerformedProtocolCodeSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTPerformedProtocolCodeSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        unsigned long idx = num + 1;
        iterator = SequenceOfItems.begin();
        const OFListConstIterator(Item *) last = SequenceOfItems.end();
        while ((--idx > 0) && (iterator != last))
            ++iterator;
        /* specified list item found? */
        if ((idx == 0) && (iterator != last))
            result = EC_Normal;
        else
            result = EC_IllegalParameter;
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        unsigned long idx = num + 1;
        iterator = SequenceOfItems.begin();
        const OFListConstIterator(Item *) last = SequenceOfItems.end();
        while ((--idx > 0) && (iterator != last))
            ++iterator;
        /* specified list item found? */
        if ((idx == 0) && (iterator != last))
            result = EC_Normal;
        else
            result = EC_IllegalParameter;
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTPerformedProtocolCodeSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTPerformedProtocolCodeSequence::Item &DRTPerformedProtocolCodeSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTPerformedProtocolCodeSequence::Item &DRTPerformedProtocolCodeSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTPerformedProtocolCodeSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTPerformedProtocolCodeSequence::Item &DRTPerformedProtocolCodeSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTPerformedProtocolCodeSequence::Item &DRTPerformedProtocolCodeSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTPerformedProtocolCodeSequence::Item &DRTPerformedProtocolCodeSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTPerformedProtocolCodeSequence::Item &DRTPerformedProtocolCodeSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTPerformedProtocolCodeSequence::addItem(Item *&item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        item = new Item();
        if (item != NULL)
        {
            SequenceOfItems.push_back(item);
            result = EC_Normal;
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::insertItem(const unsigned long pos, Item *&item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        OFListIterator(Item *) iterator;
        result = gotoItem(pos, iterator);
        if (result.good())
        {
            item = new Item();
            if (item != NULL)
            {
                SequenceOfItems.insert(iterator, 1, item);
                result = EC_Normal;
            } else
                result = EC_MemoryExhausted;
        } else
            result = addItem(item);
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::removeItem(const unsigned long pos)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        OFListIterator(Item *) iterator;
        if (gotoItem(pos, iterator).good())
        {
            delete *iterator;
            iterator = SequenceOfItems.erase(iterator);
            result = EC_Normal;
        } else
            result = EC_IllegalParameter;
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::read(DcmItem &dataset,
                                                   const OFString &card,
                                                   const OFString &type,
                                                   const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        /* re-initialize object */
        clear();
        /* retrieve sequence element from dataset */
        DcmSequenceOfItems *sequence;
        result = dataset.findAndGetSequence(DCM_PerformedProtocolCodeSequence, sequence);
        if (sequence != NULL)
        {
            if (checkElementValue(*sequence, card, type, result, moduleName))
            {
                DcmStack stack;
                OFBool first = OFTrue;
                /* iterate over all sequence items */
                while (result.good() && sequence->nextObject(stack, first /*intoSub*/).good())
                {
                    DcmItem *ditem = OFstatic_cast(DcmItem *, stack.top());
                    if (ditem != NULL)
                    {
                        Item *item = new Item();
                        if (item != NULL)
                        {
                            result = item->read(*ditem);
                            if (result.good())
                            {
                                /* append new item to the end of the list */
                                SequenceOfItems.push_back(item);
                                first = OFFalse;
                            }
                        } else
                            result = EC_MemoryExhausted;
                    } else
                        result = EC_CorruptedData;
                }
            }
        } else {
            DcmSequenceOfItems element(DCM_PerformedProtocolCodeSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTPerformedProtocolCodeSequence::write(DcmItem &dataset,
                                                    const OFString &card,
                                                    const OFString &type,
                                                    const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_PerformedProtocolCodeSequence);
        if (sequence != NULL)
        {
            result = EC_Normal;
            /* an empty optional sequence is not written */
            if ((type == "2") || !SequenceOfItems.empty())
            {
                OFListIterator(Item *) iterator = SequenceOfItems.begin();
                const OFListConstIterator(Item *) last = SequenceOfItems.end();
                /* iterate over all sequence items */
                while (result.good() && (iterator != last))
                {
                    DcmItem *item = new DcmItem();
                    if (item != NULL)
                    {
                        /* append new item to the end of the sequence */
                        result = sequence->append(item);
                        if (result.good())
                        {
                            result = (*iterator)->write(*item);
                            ++iterator;
                        } else
                            delete item;
                    } else
                        result = EC_MemoryExhausted;
                }
                if (result.good())
                {
                    /* insert sequence element into the dataset */
                    result = dataset.insert(sequence, OFTrue /*replaceOld*/);
                }
                if (DCM_dcmrtLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
                    checkElementValue(*sequence, card, type, result, moduleName);
                if (result.good())
                {
                    /* forget reference to sequence object (avoid deletion below) */
                    sequence = NULL;
                }
            }
            else if (type == "1")
            {
                /* empty type 1 sequence not allowed */
                result = RT_EC_InvalidValue;
                if (DCM_dcmrtLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
                    checkElementValue(*sequence, card, type, result, moduleName);
            }
            /* delete sequence (if not inserted into the dataset) */
            delete sequence;
        }
    }
    return result;
}


// end of source file
