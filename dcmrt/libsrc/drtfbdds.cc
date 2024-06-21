/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTFixedRTBeamDelimiterDeviceSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtfbdds.h"


// --- item class ---

DRTFixedRTBeamDelimiterDeviceSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    CenterOfCircularOutline(DCM_CenterOfCircularOutline),
    DiameterOfCircularOutline(DCM_DiameterOfCircularOutline),
    NumberOfPolygonalVertices(DCM_NumberOfPolygonalVertices),
    OutlineLeftVerticalEdge(DCM_OutlineLeftVerticalEdge),
    OutlineLowerHorizontalEdge(DCM_OutlineLowerHorizontalEdge),
    OutlineRightVerticalEdge(DCM_OutlineRightVerticalEdge),
    OutlineShapeType(DCM_OutlineShapeType),
    OutlineUpperHorizontalEdge(DCM_OutlineUpperHorizontalEdge),
    VerticesOfThePolygonalOutline(DCM_VerticesOfThePolygonalOutline)
{
}


DRTFixedRTBeamDelimiterDeviceSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    CenterOfCircularOutline(copy.CenterOfCircularOutline),
    DiameterOfCircularOutline(copy.DiameterOfCircularOutline),
    NumberOfPolygonalVertices(copy.NumberOfPolygonalVertices),
    OutlineLeftVerticalEdge(copy.OutlineLeftVerticalEdge),
    OutlineLowerHorizontalEdge(copy.OutlineLowerHorizontalEdge),
    OutlineRightVerticalEdge(copy.OutlineRightVerticalEdge),
    OutlineShapeType(copy.OutlineShapeType),
    OutlineUpperHorizontalEdge(copy.OutlineUpperHorizontalEdge),
    VerticesOfThePolygonalOutline(copy.VerticesOfThePolygonalOutline)
{
}


DRTFixedRTBeamDelimiterDeviceSequence::Item::~Item()
{
}


DRTFixedRTBeamDelimiterDeviceSequence::Item &DRTFixedRTBeamDelimiterDeviceSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        CenterOfCircularOutline = copy.CenterOfCircularOutline;
        DiameterOfCircularOutline = copy.DiameterOfCircularOutline;
        NumberOfPolygonalVertices = copy.NumberOfPolygonalVertices;
        OutlineLeftVerticalEdge = copy.OutlineLeftVerticalEdge;
        OutlineLowerHorizontalEdge = copy.OutlineLowerHorizontalEdge;
        OutlineRightVerticalEdge = copy.OutlineRightVerticalEdge;
        OutlineShapeType = copy.OutlineShapeType;
        OutlineUpperHorizontalEdge = copy.OutlineUpperHorizontalEdge;
        VerticesOfThePolygonalOutline = copy.VerticesOfThePolygonalOutline;
    }
    return *this;
}


void DRTFixedRTBeamDelimiterDeviceSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        OutlineShapeType.clear();
        OutlineLeftVerticalEdge.clear();
        OutlineRightVerticalEdge.clear();
        OutlineUpperHorizontalEdge.clear();
        OutlineLowerHorizontalEdge.clear();
        CenterOfCircularOutline.clear();
        DiameterOfCircularOutline.clear();
        NumberOfPolygonalVertices.clear();
        VerticesOfThePolygonalOutline.clear();
    }
}


OFBool DRTFixedRTBeamDelimiterDeviceSequence::Item::isEmpty()
{
    return OutlineShapeType.isEmpty() &&
           OutlineLeftVerticalEdge.isEmpty() &&
           OutlineRightVerticalEdge.isEmpty() &&
           OutlineUpperHorizontalEdge.isEmpty() &&
           OutlineLowerHorizontalEdge.isEmpty() &&
           CenterOfCircularOutline.isEmpty() &&
           DiameterOfCircularOutline.isEmpty() &&
           NumberOfPolygonalVertices.isEmpty() &&
           VerticesOfThePolygonalOutline.isEmpty();
}


OFBool DRTFixedRTBeamDelimiterDeviceSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, OutlineShapeType, "1", "1", "FixedRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, OutlineLeftVerticalEdge, "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, OutlineRightVerticalEdge, "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, OutlineUpperHorizontalEdge, "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, OutlineLowerHorizontalEdge, "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, CenterOfCircularOutline, "2", "1C", "FixedRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, DiameterOfCircularOutline, "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, NumberOfPolygonalVertices, "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        getAndCheckElementFromDataset(item, VerticesOfThePolygonalOutline, "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmCodeString(OutlineShapeType), "1", "1", "FixedRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(OutlineLeftVerticalEdge), "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(OutlineRightVerticalEdge), "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(OutlineUpperHorizontalEdge), "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(OutlineLowerHorizontalEdge), "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(CenterOfCircularOutline), "2", "1C", "FixedRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmFloatingPointDouble(DiameterOfCircularOutline), "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmUnsignedLong(NumberOfPolygonalVertices), "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
        addElementToDataset(result, item, new DcmOtherFloat(VerticesOfThePolygonalOutline), "1", "1C", "FixedRTBeamDelimiterDeviceSequence");
    }
    return result;
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::getCenterOfCircularOutline(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, CenterOfCircularOutline).getFloat64(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::getDiameterOfCircularOutline(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, DiameterOfCircularOutline).getFloat64(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::getNumberOfPolygonalVertices(Uint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedLong &, NumberOfPolygonalVertices).getUint32(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::getOutlineLeftVerticalEdge(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, OutlineLeftVerticalEdge).getFloat64(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::getOutlineLowerHorizontalEdge(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, OutlineLowerHorizontalEdge).getFloat64(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::getOutlineRightVerticalEdge(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, OutlineRightVerticalEdge).getFloat64(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::getOutlineShapeType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(OutlineShapeType, value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::getOutlineUpperHorizontalEdge(Float64 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmFloatingPointDouble &, OutlineUpperHorizontalEdge).getFloat64(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::setCenterOfCircularOutline(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return CenterOfCircularOutline.putFloat64(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::setDiameterOfCircularOutline(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return DiameterOfCircularOutline.putFloat64(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::setNumberOfPolygonalVertices(const Uint32 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return NumberOfPolygonalVertices.putUint32(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::setOutlineLeftVerticalEdge(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OutlineLeftVerticalEdge.putFloat64(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::setOutlineLowerHorizontalEdge(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OutlineLowerHorizontalEdge.putFloat64(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::setOutlineRightVerticalEdge(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OutlineRightVerticalEdge.putFloat64(value, pos);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::setOutlineShapeType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = OutlineShapeType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::Item::setOutlineUpperHorizontalEdge(const Float64 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OutlineUpperHorizontalEdge.putFloat64(value, pos);
}


// --- sequence class ---

DRTFixedRTBeamDelimiterDeviceSequence::DRTFixedRTBeamDelimiterDeviceSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTFixedRTBeamDelimiterDeviceSequence::DRTFixedRTBeamDelimiterDeviceSequence(const DRTFixedRTBeamDelimiterDeviceSequence &copy)
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


DRTFixedRTBeamDelimiterDeviceSequence &DRTFixedRTBeamDelimiterDeviceSequence::operator=(const DRTFixedRTBeamDelimiterDeviceSequence &copy)
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


DRTFixedRTBeamDelimiterDeviceSequence::~DRTFixedRTBeamDelimiterDeviceSequence()
{
    clear();
}


void DRTFixedRTBeamDelimiterDeviceSequence::clear()
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


OFBool DRTFixedRTBeamDelimiterDeviceSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTFixedRTBeamDelimiterDeviceSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTFixedRTBeamDelimiterDeviceSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::gotoNextItem()
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


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTFixedRTBeamDelimiterDeviceSequence::Item &DRTFixedRTBeamDelimiterDeviceSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTFixedRTBeamDelimiterDeviceSequence::Item &DRTFixedRTBeamDelimiterDeviceSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTFixedRTBeamDelimiterDeviceSequence::Item &DRTFixedRTBeamDelimiterDeviceSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTFixedRTBeamDelimiterDeviceSequence::Item &DRTFixedRTBeamDelimiterDeviceSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTFixedRTBeamDelimiterDeviceSequence::Item &DRTFixedRTBeamDelimiterDeviceSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTFixedRTBeamDelimiterDeviceSequence::Item &DRTFixedRTBeamDelimiterDeviceSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::addItem(Item *&item)
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


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::removeItem(const size_t pos)
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


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_FixedRTBeamDelimiterDeviceSequence, sequence);
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
            DcmSequenceOfItems element(DCM_FixedRTBeamDelimiterDeviceSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTFixedRTBeamDelimiterDeviceSequence::write(DcmItem &dataset,
                                                         const OFString &card,
                                                         const OFString &type,
                                                         const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_FixedRTBeamDelimiterDeviceSequence);
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
