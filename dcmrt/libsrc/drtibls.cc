/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTIonBlockSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtibls.h"


// --- item class ---

DRTIonBlockSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    AccessoryCode(DCM_AccessoryCode),
    BlockData(DCM_BlockData),
    BlockDivergence(DCM_BlockDivergence),
    BlockMountingPosition(DCM_BlockMountingPosition),
    BlockName(DCM_BlockName),
    BlockNumber(DCM_BlockNumber),
    BlockNumberOfPoints(DCM_BlockNumberOfPoints),
    BlockSlabSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    BlockThickness(DCM_BlockThickness),
    BlockTrayID(DCM_BlockTrayID),
    BlockType(DCM_BlockType),
    IsocenterToBlockTrayDistance(DCM_IsocenterToBlockTrayDistance),
    MaterialID(DCM_MaterialID),
    NumberOfBlockSlabItems(DCM_NumberOfBlockSlabItems)
{
}


DRTIonBlockSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    AccessoryCode(copy.AccessoryCode),
    BlockData(copy.BlockData),
    BlockDivergence(copy.BlockDivergence),
    BlockMountingPosition(copy.BlockMountingPosition),
    BlockName(copy.BlockName),
    BlockNumber(copy.BlockNumber),
    BlockNumberOfPoints(copy.BlockNumberOfPoints),
    BlockSlabSequence(copy.BlockSlabSequence),
    BlockThickness(copy.BlockThickness),
    BlockTrayID(copy.BlockTrayID),
    BlockType(copy.BlockType),
    IsocenterToBlockTrayDistance(copy.IsocenterToBlockTrayDistance),
    MaterialID(copy.MaterialID),
    NumberOfBlockSlabItems(copy.NumberOfBlockSlabItems)
{
}


DRTIonBlockSequence::Item::~Item()
{
}


DRTIonBlockSequence::Item &DRTIonBlockSequence::Item::operator=(const Item &copy)
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
        BlockSlabSequence = copy.BlockSlabSequence;
        BlockThickness = copy.BlockThickness;
        BlockTrayID = copy.BlockTrayID;
        BlockType = copy.BlockType;
        IsocenterToBlockTrayDistance = copy.IsocenterToBlockTrayDistance;
        MaterialID = copy.MaterialID;
        NumberOfBlockSlabItems = copy.NumberOfBlockSlabItems;
    }
    return *this;
}


void DRTIonBlockSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        BlockTrayID.clear();
        AccessoryCode.clear();
        IsocenterToBlockTrayDistance.clear();
        BlockType.clear();
        BlockDivergence.clear();
        BlockMountingPosition.clear();
        BlockNumber.clear();
        BlockName.clear();
        MaterialID.clear();
        BlockThickness.clear();
        BlockNumberOfPoints.clear();
        BlockData.clear();
        NumberOfBlockSlabItems.clear();
        BlockSlabSequence.clear();
    }
}


OFBool DRTIonBlockSequence::Item::isEmpty()
{
    return BlockTrayID.isEmpty() &&
           AccessoryCode.isEmpty() &&
           IsocenterToBlockTrayDistance.isEmpty() &&
           BlockType.isEmpty() &&
           BlockDivergence.isEmpty() &&
           BlockMountingPosition.isEmpty() &&
           BlockNumber.isEmpty() &&
           BlockName.isEmpty() &&
           MaterialID.isEmpty() &&
           BlockThickness.isEmpty() &&
           BlockNumberOfPoints.isEmpty() &&
           BlockData.isEmpty() &&
           NumberOfBlockSlabItems.isEmpty() &&
           BlockSlabSequence.isEmpty();
}


OFBool DRTIonBlockSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTIonBlockSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, BlockTrayID, "1", "3", "IonBlockSequence");
        getAndCheckElementFromDataset(item, AccessoryCode, "1", "3", "IonBlockSequence");
        getAndCheckElementFromDataset(item, IsocenterToBlockTrayDistance, "1", "1", "IonBlockSequence");
        getAndCheckElementFromDataset(item, BlockType, "1", "1", "IonBlockSequence");
        getAndCheckElementFromDataset(item, BlockDivergence, "1", "1", "IonBlockSequence");
        getAndCheckElementFromDataset(item, BlockMountingPosition, "1", "1", "IonBlockSequence");
        getAndCheckElementFromDataset(item, BlockNumber, "1", "1", "IonBlockSequence");
        getAndCheckElementFromDataset(item, BlockName, "1", "3", "IonBlockSequence");
        getAndCheckElementFromDataset(item, MaterialID, "1", "2", "IonBlockSequence");
        getAndCheckElementFromDataset(item, BlockThickness, "1", "1", "IonBlockSequence");
        getAndCheckElementFromDataset(item, BlockNumberOfPoints, "1", "1", "IonBlockSequence");
        getAndCheckElementFromDataset(item, BlockData, "2-2n", "1", "IonBlockSequence");
        getAndCheckElementFromDataset(item, NumberOfBlockSlabItems, "1", "3", "IonBlockSequence");
        BlockSlabSequence.read(item, "1-n", "1C", "IonBlockSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonBlockSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmShortString(BlockTrayID), "1", "3", "IonBlockSequence");
        addElementToDataset(result, item, new DcmLongString(AccessoryCode), "1", "3", "IonBlockSequence");
        addElementToDataset(result, item, new DcmFloatingPointSingle(IsocenterToBlockTrayDistance), "1", "1", "IonBlockSequence");
        addElementToDataset(result, item, new DcmCodeString(BlockType), "1", "1", "IonBlockSequence");
        addElementToDataset(result, item, new DcmCodeString(BlockDivergence), "1", "1", "IonBlockSequence");
        addElementToDataset(result, item, new DcmCodeString(BlockMountingPosition), "1", "1", "IonBlockSequence");
        addElementToDataset(result, item, new DcmIntegerString(BlockNumber), "1", "1", "IonBlockSequence");
        addElementToDataset(result, item, new DcmLongString(BlockName), "1", "3", "IonBlockSequence");
        addElementToDataset(result, item, new DcmShortString(MaterialID), "1", "2", "IonBlockSequence");
        addElementToDataset(result, item, new DcmDecimalString(BlockThickness), "1", "1", "IonBlockSequence");
        addElementToDataset(result, item, new DcmIntegerString(BlockNumberOfPoints), "1", "1", "IonBlockSequence");
        addElementToDataset(result, item, new DcmDecimalString(BlockData), "2-2n", "1", "IonBlockSequence");
        addElementToDataset(result, item, new DcmIntegerString(NumberOfBlockSlabItems), "1", "3", "IonBlockSequence");
        if (result.good()) result = BlockSlabSequence.write(item, "1-n", "1C", "IonBlockSequence");
    }
    return result;
}


OFCondition DRTIonBlockSequence::Item::getAccessoryCode(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(AccessoryCode, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockData(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockData, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockData(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BlockData).getFloat64(value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockData(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BlockData).getFloat64Vector(value);
}


OFCondition DRTIonBlockSequence::Item::getBlockDivergence(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockDivergence, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockMountingPosition(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockMountingPosition, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockName(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockName, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockNumber, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, BlockNumber).getSint32(value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockNumberOfPoints(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockNumberOfPoints, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockNumberOfPoints(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, BlockNumberOfPoints).getSint32(value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockThickness(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockThickness, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockThickness(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, BlockThickness).getFloat64(value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockTrayID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockTrayID, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getBlockType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(BlockType, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getIsocenterToBlockTrayDistance(Float32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointSingle &, IsocenterToBlockTrayDistance).getFloat32(value, pos);
}


OFCondition DRTIonBlockSequence::Item::getMaterialID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(MaterialID, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getNumberOfBlockSlabItems(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(NumberOfBlockSlabItems, value, pos);
}


OFCondition DRTIonBlockSequence::Item::getNumberOfBlockSlabItems(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, NumberOfBlockSlabItems).getSint32(value, pos);
}


OFCondition DRTIonBlockSequence::Item::setAccessoryCode(const OFString &value, const OFBool check)
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


OFCondition DRTIonBlockSequence::Item::setBlockData(const OFString &value, const OFBool check)
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


OFCondition DRTIonBlockSequence::Item::setBlockDivergence(const OFString &value, const OFBool check)
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


OFCondition DRTIonBlockSequence::Item::setBlockMountingPosition(const OFString &value, const OFBool check)
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


OFCondition DRTIonBlockSequence::Item::setBlockName(const OFString &value, const OFBool check)
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


OFCondition DRTIonBlockSequence::Item::setBlockNumber(const OFString &value, const OFBool check)
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


OFCondition DRTIonBlockSequence::Item::setBlockNumberOfPoints(const OFString &value, const OFBool check)
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


OFCondition DRTIonBlockSequence::Item::setBlockThickness(const OFString &value, const OFBool check)
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


OFCondition DRTIonBlockSequence::Item::setBlockTrayID(const OFString &value, const OFBool check)
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


OFCondition DRTIonBlockSequence::Item::setBlockType(const OFString &value, const OFBool check)
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


OFCondition DRTIonBlockSequence::Item::setIsocenterToBlockTrayDistance(const Float32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return IsocenterToBlockTrayDistance.putFloat32(value, pos);
}


OFCondition DRTIonBlockSequence::Item::setMaterialID(const OFString &value, const OFBool check)
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


OFCondition DRTIonBlockSequence::Item::setNumberOfBlockSlabItems(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = NumberOfBlockSlabItems.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTIonBlockSequence::DRTIonBlockSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTIonBlockSequence::DRTIonBlockSequence(const DRTIonBlockSequence &copy)
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


DRTIonBlockSequence &DRTIonBlockSequence::operator=(const DRTIonBlockSequence &copy)
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


DRTIonBlockSequence::~DRTIonBlockSequence()
{
    clear();
}


void DRTIonBlockSequence::clear()
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


OFBool DRTIonBlockSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTIonBlockSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTIonBlockSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTIonBlockSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonBlockSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTIonBlockSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTIonBlockSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTIonBlockSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTIonBlockSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTIonBlockSequence::Item &DRTIonBlockSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTIonBlockSequence::Item &DRTIonBlockSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTIonBlockSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTIonBlockSequence::Item &DRTIonBlockSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTIonBlockSequence::Item &DRTIonBlockSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTIonBlockSequence::Item &DRTIonBlockSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTIonBlockSequence::Item &DRTIonBlockSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTIonBlockSequence::addItem(Item *&item)
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


OFCondition DRTIonBlockSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTIonBlockSequence::removeItem(const size_t pos)
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


OFCondition DRTIonBlockSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_IonBlockSequence, sequence);
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
            DcmSequenceOfItems element(DCM_IonBlockSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTIonBlockSequence::write(DcmItem &dataset,
                                       const OFString &card,
                                       const OFString &type,
                                       const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_IonBlockSequence);
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
