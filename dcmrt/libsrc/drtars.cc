/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTAnatomicRegionSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtars.h"


// --- item class ---

DRTAnatomicRegionSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AnatomicRegionModifierSequence(emptyDefaultItem /*emptyDefaultSequence*/),
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
    URNCodeValue(DCM_URNCodeValue)
{
}


DRTAnatomicRegionSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AnatomicRegionModifierSequence(copy.AnatomicRegionModifierSequence),
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
    URNCodeValue(copy.URNCodeValue)
{
}


DRTAnatomicRegionSequence::Item::~Item()
{
}


DRTAnatomicRegionSequence::Item &DRTAnatomicRegionSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AnatomicRegionModifierSequence = copy.AnatomicRegionModifierSequence;
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
        URNCodeValue = copy.URNCodeValue;
    }
    return *this;
}


void DRTAnatomicRegionSequence::Item::clear()
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
        AnatomicRegionModifierSequence.clear();
    }
}


OFBool DRTAnatomicRegionSequence::Item::isEmpty()
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
           AnatomicRegionModifierSequence.isEmpty();
}


OFBool DRTAnatomicRegionSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTAnatomicRegionSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, CodeValue, "1", "1C", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, CodingSchemeDesignator, "1", "1C", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, CodingSchemeVersion, "1", "1C", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, CodeMeaning, "1", "1", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, LongCodeValue, "1", "1C", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, URNCodeValue, "1", "1C", "AnatomicRegionSequence");
        EquivalentCodeSequence.read(item, "1-n", "3", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, ContextIdentifier, "1", "3", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, ContextUID, "1", "3", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, MappingResource, "1", "1C", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, MappingResourceUID, "1", "3", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, MappingResourceName, "1", "3", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, ContextGroupVersion, "1", "1C", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, ContextGroupExtensionFlag, "1", "3", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, ContextGroupLocalVersion, "1", "1C", "AnatomicRegionSequence");
        getAndCheckElementFromDataset(item, ContextGroupExtensionCreatorUID, "1", "1C", "AnatomicRegionSequence");
        AnatomicRegionModifierSequence.read(item, "1-n", "3", "AnatomicRegionSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTAnatomicRegionSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmShortString(CodeValue), "1", "1C", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmShortString(CodingSchemeDesignator), "1", "1C", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmShortString(CodingSchemeVersion), "1", "1C", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmLongString(CodeMeaning), "1", "1", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmUnlimitedCharacters(LongCodeValue), "1", "1C", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmUniversalResourceIdentifierOrLocator(URNCodeValue), "1", "1C", "AnatomicRegionSequence");
        if (result.good()) result = EquivalentCodeSequence.write(item, "1-n", "3", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmCodeString(ContextIdentifier), "1", "3", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ContextUID), "1", "3", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmCodeString(MappingResource), "1", "1C", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(MappingResourceUID), "1", "3", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmLongString(MappingResourceName), "1", "3", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmDateTime(ContextGroupVersion), "1", "1C", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmCodeString(ContextGroupExtensionFlag), "1", "3", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmDateTime(ContextGroupLocalVersion), "1", "1C", "AnatomicRegionSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(ContextGroupExtensionCreatorUID), "1", "1C", "AnatomicRegionSequence");
        if (result.good()) result = AnatomicRegionModifierSequence.write(item, "1-n", "3", "AnatomicRegionSequence");
    }
    return result;
}


OFCondition DRTAnatomicRegionSequence::Item::getCodeMeaning(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodeMeaning, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getCodeValue(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodeValue, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getCodingSchemeDesignator(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodingSchemeDesignator, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getCodingSchemeVersion(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CodingSchemeVersion, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getContextGroupExtensionCreatorUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextGroupExtensionCreatorUID, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getContextGroupExtensionFlag(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextGroupExtensionFlag, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getContextGroupLocalVersion(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextGroupLocalVersion, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getContextGroupVersion(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextGroupVersion, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getContextIdentifier(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextIdentifier, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getContextUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(ContextUID, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getLongCodeValue(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(LongCodeValue, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getMappingResource(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MappingResource, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getMappingResourceName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MappingResourceName, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getMappingResourceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MappingResourceUID, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::getURNCodeValue(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(URNCodeValue, value, pos);
}


OFCondition DRTAnatomicRegionSequence::Item::setCodeMeaning(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setCodeValue(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setCodingSchemeDesignator(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setCodingSchemeVersion(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setContextGroupExtensionCreatorUID(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setContextGroupExtensionFlag(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setContextGroupLocalVersion(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setContextGroupVersion(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setContextIdentifier(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setContextUID(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setLongCodeValue(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setMappingResource(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setMappingResourceName(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setMappingResourceUID(const OFString &value, const OFBool check)
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


OFCondition DRTAnatomicRegionSequence::Item::setURNCodeValue(const OFString &value, const OFBool check)
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

DRTAnatomicRegionSequence::DRTAnatomicRegionSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTAnatomicRegionSequence::DRTAnatomicRegionSequence(const DRTAnatomicRegionSequence &copy)
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


DRTAnatomicRegionSequence &DRTAnatomicRegionSequence::operator=(const DRTAnatomicRegionSequence &copy)
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


DRTAnatomicRegionSequence::~DRTAnatomicRegionSequence()
{
    clear();
}


void DRTAnatomicRegionSequence::clear()
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


OFBool DRTAnatomicRegionSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTAnatomicRegionSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTAnatomicRegionSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTAnatomicRegionSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTAnatomicRegionSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (++CurrentItem != SequenceOfItems.end())
    {
        if (*CurrentItem != NULL)
            result = EC_Normal;
        else
            result = EC_CorruptedData;
    }
    return result;
}


OFCondition DRTAnatomicRegionSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        size_t idx = num + 1;
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


OFCondition DRTAnatomicRegionSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        size_t idx = num + 1;
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


OFCondition DRTAnatomicRegionSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTAnatomicRegionSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTAnatomicRegionSequence::Item &DRTAnatomicRegionSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTAnatomicRegionSequence::Item &DRTAnatomicRegionSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTAnatomicRegionSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTAnatomicRegionSequence::Item &DRTAnatomicRegionSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTAnatomicRegionSequence::Item &DRTAnatomicRegionSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTAnatomicRegionSequence::Item &DRTAnatomicRegionSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTAnatomicRegionSequence::Item &DRTAnatomicRegionSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTAnatomicRegionSequence::addItem(Item *&item)
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


OFCondition DRTAnatomicRegionSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTAnatomicRegionSequence::removeItem(const size_t pos)
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


OFCondition DRTAnatomicRegionSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_AnatomicRegionSequence, sequence);
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
            DcmSequenceOfItems element(DCM_AnatomicRegionSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTAnatomicRegionSequence::write(DcmItem &dataset,
                                             const OFString &card,
                                             const OFString &type,
                                             const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_AnatomicRegionSequence);
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
