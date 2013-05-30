/*
 *
 *  Copyright (c) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTFrameOfReferenceRelationshipSequence
 *
 *  Generated automatically from DICOM PS 3.3-2007
 *  File created on 2012-02-01 16:51:15 
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtforrs.h"


// --- item class ---

DRTFrameOfReferenceRelationshipSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    FrameOfReferenceTransformationComment(DCM_FrameOfReferenceTransformationComment),
    FrameOfReferenceTransformationMatrix(DCM_FrameOfReferenceTransformationMatrix),
    FrameOfReferenceTransformationType(DCM_RETIRED_FrameOfReferenceTransformationType),
    RelatedFrameOfReferenceUID(DCM_RETIRED_RelatedFrameOfReferenceUID)
{
}


DRTFrameOfReferenceRelationshipSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    FrameOfReferenceTransformationComment(copy.FrameOfReferenceTransformationComment),
    FrameOfReferenceTransformationMatrix(copy.FrameOfReferenceTransformationMatrix),
    FrameOfReferenceTransformationType(copy.FrameOfReferenceTransformationType),
    RelatedFrameOfReferenceUID(copy.RelatedFrameOfReferenceUID)
{
}


DRTFrameOfReferenceRelationshipSequence::Item::~Item()
{
}


DRTFrameOfReferenceRelationshipSequence::Item &DRTFrameOfReferenceRelationshipSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        FrameOfReferenceTransformationComment = copy.FrameOfReferenceTransformationComment;
        FrameOfReferenceTransformationMatrix = copy.FrameOfReferenceTransformationMatrix;
        FrameOfReferenceTransformationType = copy.FrameOfReferenceTransformationType;
        RelatedFrameOfReferenceUID = copy.RelatedFrameOfReferenceUID;
    }
    return *this;
}


void DRTFrameOfReferenceRelationshipSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        RelatedFrameOfReferenceUID.clear();
        FrameOfReferenceTransformationType.clear();
        FrameOfReferenceTransformationMatrix.clear();
        FrameOfReferenceTransformationComment.clear();
    }
}


OFBool DRTFrameOfReferenceRelationshipSequence::Item::isEmpty()
{
    return RelatedFrameOfReferenceUID.isEmpty() &&
           FrameOfReferenceTransformationType.isEmpty() &&
           FrameOfReferenceTransformationMatrix.isEmpty() &&
           FrameOfReferenceTransformationComment.isEmpty();
}


OFBool DRTFrameOfReferenceRelationshipSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, RelatedFrameOfReferenceUID, "1", "1C", "FrameOfReferenceRelationshipSequence");
        getAndCheckElementFromDataset(item, FrameOfReferenceTransformationType, "1", "1C", "FrameOfReferenceRelationshipSequence");
        getAndCheckElementFromDataset(item, FrameOfReferenceTransformationMatrix, "16", "1C", "FrameOfReferenceRelationshipSequence");
        getAndCheckElementFromDataset(item, FrameOfReferenceTransformationComment, "1", "3", "FrameOfReferenceRelationshipSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUniqueIdentifier(RelatedFrameOfReferenceUID), "1", "1C", "FrameOfReferenceRelationshipSequence");
        addElementToDataset(result, item, new DcmCodeString(FrameOfReferenceTransformationType), "1", "1C", "FrameOfReferenceRelationshipSequence");
        addElementToDataset(result, item, new DcmDecimalString(FrameOfReferenceTransformationMatrix), "16", "1C", "FrameOfReferenceRelationshipSequence");
        addElementToDataset(result, item, new DcmLongString(FrameOfReferenceTransformationComment), "1", "3", "FrameOfReferenceRelationshipSequence");
    }
    return result;
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::getFrameOfReferenceTransformationComment(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FrameOfReferenceTransformationComment, value, pos);
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::getFrameOfReferenceTransformationMatrix(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FrameOfReferenceTransformationMatrix, value, pos);
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::getFrameOfReferenceTransformationMatrix(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, FrameOfReferenceTransformationMatrix).getFloat64(value, pos);
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::getFrameOfReferenceTransformationMatrix(OFVector<Float64> &value) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmDecimalString &, FrameOfReferenceTransformationMatrix).getFloat64Vector(value);
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::getFrameOfReferenceTransformationType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(FrameOfReferenceTransformationType, value, pos);
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::getRelatedFrameOfReferenceUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RelatedFrameOfReferenceUID, value, pos);
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::setFrameOfReferenceTransformationComment(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FrameOfReferenceTransformationComment.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::setFrameOfReferenceTransformationMatrix(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDecimalString::checkStringValue(value, "16") : EC_Normal;
        if (result.good())
            result = FrameOfReferenceTransformationMatrix.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::setFrameOfReferenceTransformationType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = FrameOfReferenceTransformationType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFrameOfReferenceRelationshipSequence::Item::setRelatedFrameOfReferenceUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RelatedFrameOfReferenceUID.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTFrameOfReferenceRelationshipSequence::DRTFrameOfReferenceRelationshipSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTFrameOfReferenceRelationshipSequence::DRTFrameOfReferenceRelationshipSequence(const DRTFrameOfReferenceRelationshipSequence &copy)
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


DRTFrameOfReferenceRelationshipSequence &DRTFrameOfReferenceRelationshipSequence::operator=(const DRTFrameOfReferenceRelationshipSequence &copy)
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


DRTFrameOfReferenceRelationshipSequence::~DRTFrameOfReferenceRelationshipSequence()
{
    clear();
}


void DRTFrameOfReferenceRelationshipSequence::clear()
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


OFBool DRTFrameOfReferenceRelationshipSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTFrameOfReferenceRelationshipSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


unsigned long DRTFrameOfReferenceRelationshipSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTFrameOfReferenceRelationshipSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFrameOfReferenceRelationshipSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFrameOfReferenceRelationshipSequence::gotoItem(const unsigned long num, OFListIterator(Item *) &iterator)
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


OFCondition DRTFrameOfReferenceRelationshipSequence::gotoItem(const unsigned long num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTFrameOfReferenceRelationshipSequence::gotoItem(const unsigned long num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTFrameOfReferenceRelationshipSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTFrameOfReferenceRelationshipSequence::Item &DRTFrameOfReferenceRelationshipSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTFrameOfReferenceRelationshipSequence::Item &DRTFrameOfReferenceRelationshipSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTFrameOfReferenceRelationshipSequence::getItem(const unsigned long num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTFrameOfReferenceRelationshipSequence::Item &DRTFrameOfReferenceRelationshipSequence::getItem(const unsigned long num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTFrameOfReferenceRelationshipSequence::Item &DRTFrameOfReferenceRelationshipSequence::getItem(const unsigned long num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTFrameOfReferenceRelationshipSequence::Item &DRTFrameOfReferenceRelationshipSequence::operator[](const unsigned long num)
{
    return getItem(num);
}


const DRTFrameOfReferenceRelationshipSequence::Item &DRTFrameOfReferenceRelationshipSequence::operator[](const unsigned long num) const
{
    return getItem(num);
}


OFCondition DRTFrameOfReferenceRelationshipSequence::addItem(Item *&item)
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


OFCondition DRTFrameOfReferenceRelationshipSequence::insertItem(const unsigned long pos, Item *&item)
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


OFCondition DRTFrameOfReferenceRelationshipSequence::removeItem(const unsigned long pos)
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


OFCondition DRTFrameOfReferenceRelationshipSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_RETIRED_FrameOfReferenceRelationshipSequence, sequence);
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
            DcmSequenceOfItems element(DCM_RETIRED_FrameOfReferenceRelationshipSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTFrameOfReferenceRelationshipSequence::write(DcmItem &dataset,
                                                           const OFString &card,
                                                           const OFString &type,
                                                           const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_RETIRED_FrameOfReferenceRelationshipSequence);
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
