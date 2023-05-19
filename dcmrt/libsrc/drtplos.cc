/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2023, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTPlanOverviewSequence
 *
 *  Generated automatically from DICOM PS 3.3-2023b
 *  File created on 2023-05-19 16:00:57
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtplos.h"


// --- item class ---

DRTPlanOverviewSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    CurrentFractionNumber(DCM_CurrentFractionNumber),
    NumberOfFractionsIncluded(DCM_NumberOfFractionsIncluded),
    PlanOverviewIndex(DCM_PlanOverviewIndex),
    PrescriptionOverviewSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    RTPlanLabel(DCM_RTPlanLabel),
    ReferencedImageSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    ReferencedStructureSetSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    TreatmentSite(DCM_TreatmentSite),
    TreatmentSiteCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/)
{
}


DRTPlanOverviewSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    CurrentFractionNumber(copy.CurrentFractionNumber),
    NumberOfFractionsIncluded(copy.NumberOfFractionsIncluded),
    PlanOverviewIndex(copy.PlanOverviewIndex),
    PrescriptionOverviewSequence(copy.PrescriptionOverviewSequence),
    RTPlanLabel(copy.RTPlanLabel),
    ReferencedImageSequence(copy.ReferencedImageSequence),
    ReferencedStructureSetSequence(copy.ReferencedStructureSetSequence),
    TreatmentSite(copy.TreatmentSite),
    TreatmentSiteCodeSequence(copy.TreatmentSiteCodeSequence)
{
}


DRTPlanOverviewSequence::Item::~Item()
{
}


DRTPlanOverviewSequence::Item &DRTPlanOverviewSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        CurrentFractionNumber = copy.CurrentFractionNumber;
        NumberOfFractionsIncluded = copy.NumberOfFractionsIncluded;
        PlanOverviewIndex = copy.PlanOverviewIndex;
        PrescriptionOverviewSequence = copy.PrescriptionOverviewSequence;
        RTPlanLabel = copy.RTPlanLabel;
        ReferencedImageSequence = copy.ReferencedImageSequence;
        ReferencedStructureSetSequence = copy.ReferencedStructureSetSequence;
        TreatmentSite = copy.TreatmentSite;
        TreatmentSiteCodeSequence = copy.TreatmentSiteCodeSequence;
    }
    return *this;
}


void DRTPlanOverviewSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        PlanOverviewIndex.clear();
        RTPlanLabel.clear();
        NumberOfFractionsIncluded.clear();
        CurrentFractionNumber.clear();
        TreatmentSite.clear();
        TreatmentSiteCodeSequence.clear();
        PrescriptionOverviewSequence.clear();
        ReferencedStructureSetSequence.clear();
        ReferencedImageSequence.clear();
    }
}


OFBool DRTPlanOverviewSequence::Item::isEmpty()
{
    return PlanOverviewIndex.isEmpty() &&
           RTPlanLabel.isEmpty() &&
           NumberOfFractionsIncluded.isEmpty() &&
           CurrentFractionNumber.isEmpty() &&
           TreatmentSite.isEmpty() &&
           TreatmentSiteCodeSequence.isEmpty() &&
           PrescriptionOverviewSequence.isEmpty() &&
           ReferencedStructureSetSequence.isEmpty() &&
           ReferencedImageSequence.isEmpty();
}


OFBool DRTPlanOverviewSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTPlanOverviewSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, PlanOverviewIndex, "1", "1", "PlanOverviewSequence");
        getAndCheckElementFromDataset(item, RTPlanLabel, "1", "2", "PlanOverviewSequence");
        getAndCheckElementFromDataset(item, NumberOfFractionsIncluded, "1", "1C", "PlanOverviewSequence");
        getAndCheckElementFromDataset(item, CurrentFractionNumber, "1", "1C", "PlanOverviewSequence");
        getAndCheckElementFromDataset(item, TreatmentSite, "1", "2", "PlanOverviewSequence");
        TreatmentSiteCodeSequence.read(item, "1-n", "2", "PlanOverviewSequence");
        PrescriptionOverviewSequence.read(item, "1-n", "2", "PlanOverviewSequence");
        ReferencedStructureSetSequence.read(item, "1-n", "1C", "PlanOverviewSequence");
        ReferencedImageSequence.read(item, "1-n", "1C", "PlanOverviewSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPlanOverviewSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUnsignedShort(PlanOverviewIndex), "1", "1", "PlanOverviewSequence");
        addElementToDataset(result, item, new DcmShortString(RTPlanLabel), "1", "2", "PlanOverviewSequence");
        addElementToDataset(result, item, new DcmUnsignedShort(NumberOfFractionsIncluded), "1", "1C", "PlanOverviewSequence");
        addElementToDataset(result, item, new DcmIntegerString(CurrentFractionNumber), "1", "1C", "PlanOverviewSequence");
        addElementToDataset(result, item, new DcmLongString(TreatmentSite), "1", "2", "PlanOverviewSequence");
        if (result.good()) result = TreatmentSiteCodeSequence.write(item, "1-n", "2", "PlanOverviewSequence");
        if (result.good()) result = PrescriptionOverviewSequence.write(item, "1-n", "2", "PlanOverviewSequence");
        if (result.good()) result = ReferencedStructureSetSequence.write(item, "1-n", "1C", "PlanOverviewSequence");
        if (result.good()) result = ReferencedImageSequence.write(item, "1-n", "1C", "PlanOverviewSequence");
    }
    return result;
}


OFCondition DRTPlanOverviewSequence::Item::getCurrentFractionNumber(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CurrentFractionNumber, value, pos);
}


OFCondition DRTPlanOverviewSequence::Item::getCurrentFractionNumber(Sint32 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmIntegerString &, CurrentFractionNumber).getSint32(value, pos);
}


OFCondition DRTPlanOverviewSequence::Item::getNumberOfFractionsIncluded(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, NumberOfFractionsIncluded).getUint16(value, pos);
}


OFCondition DRTPlanOverviewSequence::Item::getPlanOverviewIndex(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, PlanOverviewIndex).getUint16(value, pos);
}


OFCondition DRTPlanOverviewSequence::Item::getRTPlanLabel(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(RTPlanLabel, value, pos);
}


OFCondition DRTPlanOverviewSequence::Item::getTreatmentSite(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(TreatmentSite, value, pos);
}


OFCondition DRTPlanOverviewSequence::Item::setCurrentFractionNumber(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CurrentFractionNumber.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPlanOverviewSequence::Item::setNumberOfFractionsIncluded(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return NumberOfFractionsIncluded.putUint16(value, pos);
}


OFCondition DRTPlanOverviewSequence::Item::setPlanOverviewIndex(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return PlanOverviewIndex.putUint16(value, pos);
}


OFCondition DRTPlanOverviewSequence::Item::setRTPlanLabel(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = RTPlanLabel.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTPlanOverviewSequence::Item::setTreatmentSite(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = TreatmentSite.putOFStringArray(value);
    }
    return result;
}


// --- sequence class ---

DRTPlanOverviewSequence::DRTPlanOverviewSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTPlanOverviewSequence::DRTPlanOverviewSequence(const DRTPlanOverviewSequence &copy)
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


DRTPlanOverviewSequence &DRTPlanOverviewSequence::operator=(const DRTPlanOverviewSequence &copy)
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


DRTPlanOverviewSequence::~DRTPlanOverviewSequence()
{
    clear();
}


void DRTPlanOverviewSequence::clear()
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


OFBool DRTPlanOverviewSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTPlanOverviewSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTPlanOverviewSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTPlanOverviewSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTPlanOverviewSequence::gotoNextItem()
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


OFCondition DRTPlanOverviewSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTPlanOverviewSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTPlanOverviewSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTPlanOverviewSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTPlanOverviewSequence::Item &DRTPlanOverviewSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTPlanOverviewSequence::Item &DRTPlanOverviewSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTPlanOverviewSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTPlanOverviewSequence::Item &DRTPlanOverviewSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTPlanOverviewSequence::Item &DRTPlanOverviewSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTPlanOverviewSequence::Item &DRTPlanOverviewSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTPlanOverviewSequence::Item &DRTPlanOverviewSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTPlanOverviewSequence::addItem(Item *&item)
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


OFCondition DRTPlanOverviewSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTPlanOverviewSequence::removeItem(const size_t pos)
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


OFCondition DRTPlanOverviewSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_PlanOverviewSequence, sequence);
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
            DcmSequenceOfItems element(DCM_PlanOverviewSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTPlanOverviewSequence::write(DcmItem &dataset,
                                           const OFString &card,
                                           const OFString &type,
                                           const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_PlanOverviewSequence);
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
