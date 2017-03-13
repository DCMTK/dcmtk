/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTBlockSequenceInRTImageModule
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtbl5.h"


// --- item class ---

DRTBlockSequenceInRTImageModule::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    BlockData(DCM_BlockData),
    BlockDivergence(DCM_BlockDivergence),
    BlockMountingPosition(DCM_BlockMountingPosition),
    BlockName(DCM_BlockName),
    BlockNumber(DCM_BlockNumber),
    BlockNumberOfPoints(DCM_BlockNumberOfPoints),
    BlockThickness(DCM_BlockThickness),
    BlockTrayID(DCM_BlockTrayID),
    BlockType(DCM_BlockType),
    MaterialID(DCM_MaterialID),
    SourceToBlockTrayDistance(DCM_SourceToBlockTrayDistance),
    TrayAccessoryCode(DCM_TrayAccessoryCode)
{
}


DRTBlockSequenceInRTImageModule::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    BlockData(copy.BlockData),
    BlockDivergence(copy.BlockDivergence),
    BlockMountingPosition(copy.BlockMountingPosition),
    BlockName(copy.BlockName),
    BlockNumber(copy.BlockNumber),
    BlockNumberOfPoints(copy.BlockNumberOfPoints),
    BlockThickness(copy.BlockThickness),
    BlockTrayID(copy.BlockTrayID),
    BlockType(copy.BlockType),
    MaterialID(copy.MaterialID),
    SourceToBlockTrayDistance(copy.SourceToBlockTrayDistance),
    TrayAccessoryCode(copy.TrayAccessoryCode)
{
}


DRTBlockSequenceInRTImageModule::Item::~Item()
{
}


DRTBlockSequenceInRTImageModule::Item &DRTBlockSequenceInRTImageModule::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        AccessoryCode = copy.AccessoryCode;
        BlockData = copy.BlockData;
        BlockDivergence = copy.BlockDivergence;
        BlockMountingPosition = copy.BlockMountingPosition;
        BlockName = copy.BlockName;
        BlockNumber = copy.BlockNumber;
        BlockNumberOfPoints = copy.BlockNumberOfPoints;
        BlockThickness = copy.BlockThickness;
        BlockTrayID = copy.BlockTrayID;
        BlockType = copy.BlockType;
        MaterialID = copy.MaterialID;
        SourceToBlockTrayDistance = copy.SourceToBlockTrayDistance;
        TrayAccessoryCode = copy.TrayAccessoryCode;
    }
    return *this;
}


void DRTBlockSequenceInRTImageModule::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        BlockTrayID.clear();
        TrayAccessoryCode.clear();
        AccessoryCode.clear();
        SourceToBlockTrayDistance.clear();
        BlockType.clear();
        BlockDivergence.clear();
        BlockMountingPosition.clear();
        BlockNumber.clear();
        BlockName.clear();
        MaterialID.clear();
        BlockThickness.clear();
        BlockNumberOfPoints.clear();
        BlockData.clear();
    }
}


OFBool DRTBlockSequenceInRTImageModule::Item::isEmpty()
{
    return BlockTrayID.isEmpty() &&
           TrayAccessoryCode.isEmpty() &&
           AccessoryCode.isEmpty() &&
           SourceToBlockTrayDistance.isEmpty() &&
           BlockType.isEmpty() &&
           BlockDivergence.isEmpty() &&
           BlockMountingPosition.isEmpty() &&
           BlockNumber.isEmpty() &&
           BlockName.isEmpty() &&
           MaterialID.isEmpty() &&
           BlockThickness.isEmpty() &&
           BlockNumberOfPoints.isEmpty() &&
           BlockData.isEmpty();
}


OFBool DRTBlockSequenceInRTImageModule::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, BlockTrayID, "1", "3", "BlockSequence");
        getAndCheckElementFromDataset(item, TrayAccessoryCode, "1", "3", "BlockSequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "BlockSequence");
        getAndCheckElementFromDataset(item, SourceToBlockTrayDistance, "1", "2", "BlockSequence");
        getAndCheckElementFromDataset(item, BlockType, "1", "1", "BlockSequence");
        getAndCheckElementFromDataset(item, BlockDivergence, "1", "2", "BlockSequence");
        getAndCheckElementFromDataset(item, BlockMountingPosition, "1", "3", "BlockSequence");
        getAndCheckElementFromDataset(item, BlockNumber, "1", "1", "BlockSequence");
        getAndCheckElementFromDataset(item, BlockName, "1", "3", "BlockSequence");
        getAndCheckElementFromDataset(item, MaterialID, "1", "2", "BlockSequence");
        getAndCheckElementFromDataset(item, BlockThickness, "1", "3", "BlockSequence");
        getAndCheckElementFromDataset(item, BlockNumberOfPoints, "1", "2", "BlockSequence");
        getAndCheckElementFromDataset(item, BlockData, "2-2n", "2", "BlockSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmShortString(BlockTrayID), "1", "3", "BlockSequence");
        addElementToDataset(result, item, new DcmLongString(TrayAccessoryCode), "1", "3", "BlockSequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "BlockSequence");
        addElementToDataset(result, item, new DcmDecimalString(SourceToBlockTrayDistance), "1", "2", "BlockSequence");
        addElementToDataset(result, item, new DcmCodeString(BlockType), "1", "1", "BlockSequence");
        addElementToDataset(result, item, new DcmCodeString(BlockDivergence), "1", "2", "BlockSequence");
        addElementToDataset(result, item, new DcmCodeString(BlockMountingPosition), "1", "3", "BlockSequence");
        addElementToDataset(result, item, new DcmIntegerString(BlockNumber), "1", "1", "BlockSequence");
        addElementToDataset(result, item, new DcmLongString(BlockName), "1", "3", "BlockSequence");
        addElementToDataset(result, item, new DcmShortString(MaterialID), "1", "2", "BlockSequence");
        addElementToDataset(result, item, new DcmDecimalString(BlockThickness), "1", "3", "BlockSequence");
        addElementToDataset(result, item, new DcmIntegerString(BlockNumberOfPoints), "1", "2", "BlockSequence");
        addElementToDataset(result, item, new DcmDecimalString(BlockData), "2-2n", "2", "BlockSequence");
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockData(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockData, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockData(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BlockData).getFloat64(value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockData(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BlockData).getFloat64Vector(value);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockDivergence(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockDivergence, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockMountingPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockMountingPosition, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockName, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockNumber, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, BlockNumber).getSint32(value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockNumberOfPoints(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockNumberOfPoints, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockNumberOfPoints(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, BlockNumberOfPoints).getSint32(value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockThickness(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockThickness, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockThickness(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BlockThickness).getFloat64(value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockTrayID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockTrayID, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getBlockType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockType, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getMaterialID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MaterialID, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getSourceToBlockTrayDistance(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(SourceToBlockTrayDistance, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getSourceToBlockTrayDistance(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, SourceToBlockTrayDistance).getFloat64(value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::getTrayAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TrayAccessoryCode, value, pos);
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setAccessoryCode(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = AccessoryCode.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setBlockData(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "2-2n") : EC_Normal;
        if (result.good())
            result = BlockData.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setBlockDivergence(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BlockDivergence.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setBlockMountingPosition(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BlockMountingPosition.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setBlockName(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BlockName.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setBlockNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BlockNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setBlockNumberOfPoints(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BlockNumberOfPoints.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setBlockThickness(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BlockThickness.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setBlockTrayID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BlockTrayID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setBlockType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = BlockType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setMaterialID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = MaterialID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setSourceToBlockTrayDistance(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = SourceToBlockTrayDistance.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::Item::setTrayAccessoryCode(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TrayAccessoryCode.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTBlockSequenceInRTImageModule::DRTBlockSequenceInRTImageModule(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTBlockSequenceInRTImageModule::DRTBlockSequenceInRTImageModule(const DRTBlockSequenceInRTImageModule &copy)
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


DRTBlockSequenceInRTImageModule &DRTBlockSequenceInRTImageModule::operator=(const DRTBlockSequenceInRTImageModule &copy)
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


DRTBlockSequenceInRTImageModule::~DRTBlockSequenceInRTImageModule()
{
    clear();
}


void DRTBlockSequenceInRTImageModule::clear()
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


OFBool DRTBlockSequenceInRTImageModule::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTBlockSequenceInRTImageModule::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTBlockSequenceInRTImageModule::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTBlockSequenceInRTImageModule::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTBlockSequenceInRTImageModule::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTBlockSequenceInRTImageModule::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTBlockSequenceInRTImageModule::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTBlockSequenceInRTImageModule::Item &DRTBlockSequenceInRTImageModule::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTBlockSequenceInRTImageModule::Item &DRTBlockSequenceInRTImageModule::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTBlockSequenceInRTImageModule::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTBlockSequenceInRTImageModule::Item &DRTBlockSequenceInRTImageModule::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTBlockSequenceInRTImageModule::Item &DRTBlockSequenceInRTImageModule::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTBlockSequenceInRTImageModule::Item &DRTBlockSequenceInRTImageModule::operator[](const size_t num)
{
    return getItem(num);
}


const DRTBlockSequenceInRTImageModule::Item &DRTBlockSequenceInRTImageModule::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTBlockSequenceInRTImageModule::addItem(Item *&item)
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


OFCondition DRTBlockSequenceInRTImageModule::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTBlockSequenceInRTImageModule::removeItem(const size_t pos)
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


OFCondition DRTBlockSequenceInRTImageModule::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_BlockSequence, sequence);
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
            DcmSequenceOfItems element(DCM_BlockSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTBlockSequenceInRTImageModule::write(DcmItem &dataset,
                                                   const OFString &card,
                                                   const OFString &type,
                                                   const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_BlockSequence);
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
