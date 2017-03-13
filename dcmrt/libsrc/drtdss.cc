/*
 *
 *  Copyright (C) 2008-2012, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2017, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTDigitalSignaturesSequence
 *
 *  Generated automatically from DICOM PS 3.3-2017a
 *  File created on 2017-03-13 11:22:36
 *
 */


#include "dcmtk/config/osconfig.h"     // make sure OS specific configuration is included first

#include "dcmtk/dcmrt/seq/drtdss.h"


// --- item class ---

DRTDigitalSignaturesSequence::Item::Item(const OFBool emptyDefaultItem)
  : EmptyDefaultItem(emptyDefaultItem),
    CertificateOfSigner(DCM_CertificateOfSigner),
    CertificateType(DCM_CertificateType),
    CertifiedTimestamp(DCM_CertifiedTimestamp),
    CertifiedTimestampType(DCM_CertifiedTimestampType),
    DigitalSignatureDateTime(DCM_DigitalSignatureDateTime),
    DigitalSignaturePurposeCodeSequence(emptyDefaultItem /*emptyDefaultSequence*/),
    DigitalSignatureUID(DCM_DigitalSignatureUID),
    MACIDNumber(DCM_MACIDNumber),
    Signature(DCM_Signature)
{
}


DRTDigitalSignaturesSequence::Item::Item(const Item &copy)
  : EmptyDefaultItem(copy.EmptyDefaultItem),
    CertificateOfSigner(copy.CertificateOfSigner),
    CertificateType(copy.CertificateType),
    CertifiedTimestamp(copy.CertifiedTimestamp),
    CertifiedTimestampType(copy.CertifiedTimestampType),
    DigitalSignatureDateTime(copy.DigitalSignatureDateTime),
    DigitalSignaturePurposeCodeSequence(copy.DigitalSignaturePurposeCodeSequence),
    DigitalSignatureUID(copy.DigitalSignatureUID),
    MACIDNumber(copy.MACIDNumber),
    Signature(copy.Signature)
{
}


DRTDigitalSignaturesSequence::Item::~Item()
{
}


DRTDigitalSignaturesSequence::Item &DRTDigitalSignaturesSequence::Item::operator=(const Item &copy)
{
    if (this != &copy)
    {
        EmptyDefaultItem = copy.EmptyDefaultItem;
        CertificateOfSigner = copy.CertificateOfSigner;
        CertificateType = copy.CertificateType;
        CertifiedTimestamp = copy.CertifiedTimestamp;
        CertifiedTimestampType = copy.CertifiedTimestampType;
        DigitalSignatureDateTime = copy.DigitalSignatureDateTime;
        DigitalSignaturePurposeCodeSequence = copy.DigitalSignaturePurposeCodeSequence;
        DigitalSignatureUID = copy.DigitalSignatureUID;
        MACIDNumber = copy.MACIDNumber;
        Signature = copy.Signature;
    }
    return *this;
}


void DRTDigitalSignaturesSequence::Item::clear()
{
    if (!EmptyDefaultItem)
    {
        /* clear all DICOM attributes */
        MACIDNumber.clear();
        DigitalSignatureUID.clear();
        DigitalSignatureDateTime.clear();
        CertificateType.clear();
        CertificateOfSigner.clear();
        Signature.clear();
        CertifiedTimestampType.clear();
        CertifiedTimestamp.clear();
        DigitalSignaturePurposeCodeSequence.clear();
    }
}


OFBool DRTDigitalSignaturesSequence::Item::isEmpty()
{
    return MACIDNumber.isEmpty() &&
           DigitalSignatureUID.isEmpty() &&
           DigitalSignatureDateTime.isEmpty() &&
           CertificateType.isEmpty() &&
           CertificateOfSigner.isEmpty() &&
           Signature.isEmpty() &&
           CertifiedTimestampType.isEmpty() &&
           CertifiedTimestamp.isEmpty() &&
           DigitalSignaturePurposeCodeSequence.isEmpty();
}


OFBool DRTDigitalSignaturesSequence::Item::isValid() const
{
    return !EmptyDefaultItem;
}


OFCondition DRTDigitalSignaturesSequence::Item::read(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        /* re-initialize object */
        clear();
        getAndCheckElementFromDataset(item, MACIDNumber, "1", "1", "DigitalSignaturesSequence");
        getAndCheckElementFromDataset(item, DigitalSignatureUID, "1", "1", "DigitalSignaturesSequence");
        getAndCheckElementFromDataset(item, DigitalSignatureDateTime, "1", "1", "DigitalSignaturesSequence");
        getAndCheckElementFromDataset(item, CertificateType, "1", "1", "DigitalSignaturesSequence");
        getAndCheckElementFromDataset(item, CertificateOfSigner, "1", "1", "DigitalSignaturesSequence");
        getAndCheckElementFromDataset(item, Signature, "1", "1", "DigitalSignaturesSequence");
        getAndCheckElementFromDataset(item, CertifiedTimestampType, "1", "1C", "DigitalSignaturesSequence");
        getAndCheckElementFromDataset(item, CertifiedTimestamp, "1", "3", "DigitalSignaturesSequence");
        DigitalSignaturePurposeCodeSequence.read(item, "1-n", "3", "DigitalSignaturesSequence");
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::Item::write(DcmItem &item)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = EC_Normal;
        addElementToDataset(result, item, new DcmUnsignedShort(MACIDNumber), "1", "1", "DigitalSignaturesSequence");
        addElementToDataset(result, item, new DcmUniqueIdentifier(DigitalSignatureUID), "1", "1", "DigitalSignaturesSequence");
        addElementToDataset(result, item, new DcmDateTime(DigitalSignatureDateTime), "1", "1", "DigitalSignaturesSequence");
        addElementToDataset(result, item, new DcmCodeString(CertificateType), "1", "1", "DigitalSignaturesSequence");
        addElementToDataset(result, item, new DcmOtherByteOtherWord(CertificateOfSigner), "1", "1", "DigitalSignaturesSequence");
        addElementToDataset(result, item, new DcmOtherByteOtherWord(Signature), "1", "1", "DigitalSignaturesSequence");
        addElementToDataset(result, item, new DcmCodeString(CertifiedTimestampType), "1", "1C", "DigitalSignaturesSequence");
        addElementToDataset(result, item, new DcmOtherByteOtherWord(CertifiedTimestamp), "1", "3", "DigitalSignaturesSequence");
        if (result.good()) result = DigitalSignaturePurposeCodeSequence.write(item, "1-n", "3", "DigitalSignaturesSequence");
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::Item::getCertificateOfSigner(Uint8 *&value, unsigned long *count) const
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = OFconst_cast(DcmOtherByteOtherWord &, CertificateOfSigner).getUint8Array(value);
        /* set optional count parameter */
        if (count != NULL)
        {
            if (result.good())
                *count = OFconst_cast(DcmOtherByteOtherWord &, CertificateOfSigner).getLength() / sizeof(Uint8);
            else
                *count = 0;
        }
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::Item::getCertificateType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CertificateType, value, pos);
}


OFCondition DRTDigitalSignaturesSequence::Item::getCertifiedTimestamp(Uint8 *&value, unsigned long *count) const
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = OFconst_cast(DcmOtherByteOtherWord &, CertifiedTimestamp).getUint8Array(value);
        /* set optional count parameter */
        if (count != NULL)
        {
            if (result.good())
                *count = OFconst_cast(DcmOtherByteOtherWord &, CertifiedTimestamp).getLength() / sizeof(Uint8);
            else
                *count = 0;
        }
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::Item::getCertifiedTimestampType(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(CertifiedTimestampType, value, pos);
}


OFCondition DRTDigitalSignaturesSequence::Item::getDigitalSignatureDateTime(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DigitalSignatureDateTime, value, pos);
}


OFCondition DRTDigitalSignaturesSequence::Item::getDigitalSignatureUID(OFString &value, const signed long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return getStringValueFromElement(DigitalSignatureUID, value, pos);
}


OFCondition DRTDigitalSignaturesSequence::Item::getMACIDNumber(Uint16 &value, const unsigned long pos) const
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return OFconst_cast(DcmUnsignedShort &, MACIDNumber).getUint16(value, pos);
}


OFCondition DRTDigitalSignaturesSequence::Item::getSignature(Uint8 *&value, unsigned long *count) const
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = OFconst_cast(DcmOtherByteOtherWord &, Signature).getUint8Array(value);
        /* set optional count parameter */
        if (count != NULL)
        {
            if (result.good())
                *count = OFconst_cast(DcmOtherByteOtherWord &, Signature).getLength() / sizeof(Uint8);
            else
                *count = 0;
        }
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::Item::setCertificateOfSigner(const Uint8 *value, const unsigned long count)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return CertificateOfSigner.putUint8Array(value, count);
}


OFCondition DRTDigitalSignaturesSequence::Item::setCertificateType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CertificateType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::Item::setCertifiedTimestamp(const Uint8 *value, const unsigned long count)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return CertifiedTimestamp.putUint8Array(value, count);
}


OFCondition DRTDigitalSignaturesSequence::Item::setCertifiedTimestampType(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = CertifiedTimestampType.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::Item::setDigitalSignatureDateTime(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DigitalSignatureDateTime.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::Item::setDigitalSignatureUID(const OFString &value, const OFBool check)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultItem)
    {
        result = (check) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
        if (result.good())
            result = DigitalSignatureUID.putOFStringArray(value);
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::Item::setMACIDNumber(const Uint16 value, const unsigned long pos)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return MACIDNumber.putUint16(value, pos);
}


OFCondition DRTDigitalSignaturesSequence::Item::setSignature(const Uint8 *value, const unsigned long count)
{
    if (EmptyDefaultItem)
        return EC_IllegalCall;
    else
        return Signature.putUint8Array(value, count);
}


// --- sequence class ---

DRTDigitalSignaturesSequence::DRTDigitalSignaturesSequence(const OFBool emptyDefaultSequence)
  : EmptyDefaultSequence(emptyDefaultSequence),
    SequenceOfItems(),
    CurrentItem(),
    EmptyItem(OFTrue /*emptyDefaultItem*/)
{
    CurrentItem = SequenceOfItems.end();
}


DRTDigitalSignaturesSequence::DRTDigitalSignaturesSequence(const DRTDigitalSignaturesSequence &copy)
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


DRTDigitalSignaturesSequence &DRTDigitalSignaturesSequence::operator=(const DRTDigitalSignaturesSequence &copy)
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


DRTDigitalSignaturesSequence::~DRTDigitalSignaturesSequence()
{
    clear();
}


void DRTDigitalSignaturesSequence::clear()
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


OFBool DRTDigitalSignaturesSequence::isEmpty()
{
    return SequenceOfItems.empty();
}


OFBool DRTDigitalSignaturesSequence::isValid() const
{
    return !EmptyDefaultSequence;
}


size_t DRTDigitalSignaturesSequence::getNumberOfItems() const
{
    return SequenceOfItems.size();
}


OFCondition DRTDigitalSignaturesSequence::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    if (!SequenceOfItems.empty())
    {
        CurrentItem = SequenceOfItems.begin();
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        ++CurrentItem;
        result = EC_Normal;
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::gotoItem(const size_t num, OFListIterator(Item *) &iterator)
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


OFCondition DRTDigitalSignaturesSequence::gotoItem(const size_t num, OFListConstIterator(Item *) &iterator) const
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


OFCondition DRTDigitalSignaturesSequence::gotoItem(const size_t num)
{
    return gotoItem(num, CurrentItem);
}


OFCondition DRTDigitalSignaturesSequence::getCurrentItem(Item *&item) const
{
    OFCondition result = EC_IllegalCall;
    if (CurrentItem != SequenceOfItems.end())
    {
        item = *CurrentItem;
        result = EC_Normal;
    }
    return result;
}


DRTDigitalSignaturesSequence::Item &DRTDigitalSignaturesSequence::getCurrentItem()
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


const DRTDigitalSignaturesSequence::Item &DRTDigitalSignaturesSequence::getCurrentItem() const
{
    if (CurrentItem != SequenceOfItems.end())
        return **CurrentItem;
    else
        return EmptyItem;
}


OFCondition DRTDigitalSignaturesSequence::getItem(const size_t num, Item *&item)
{
    OFListIterator(Item *) iterator;
    OFCondition result = gotoItem(num, iterator);
    if (result.good())
        item = *iterator;
    return result;
}


DRTDigitalSignaturesSequence::Item &DRTDigitalSignaturesSequence::getItem(const size_t num)
{
    OFListIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


const DRTDigitalSignaturesSequence::Item &DRTDigitalSignaturesSequence::getItem(const size_t num) const
{
    OFListConstIterator(Item *) iterator;
    if (gotoItem(num, iterator).good())
        return **iterator;
    else
        return EmptyItem;
}


DRTDigitalSignaturesSequence::Item &DRTDigitalSignaturesSequence::operator[](const size_t num)
{
    return getItem(num);
}


const DRTDigitalSignaturesSequence::Item &DRTDigitalSignaturesSequence::operator[](const size_t num) const
{
    return getItem(num);
}


OFCondition DRTDigitalSignaturesSequence::addItem(Item *&item)
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


OFCondition DRTDigitalSignaturesSequence::insertItem(const size_t pos, Item *&item)
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


OFCondition DRTDigitalSignaturesSequence::removeItem(const size_t pos)
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


OFCondition DRTDigitalSignaturesSequence::read(DcmItem &dataset,
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
        result = dataset.findAndGetSequence(DCM_DigitalSignaturesSequence, sequence);
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
            DcmSequenceOfItems element(DCM_DigitalSignaturesSequence);
            checkElementValue(element, card, type, result, moduleName);
        }
    }
    return result;
}


OFCondition DRTDigitalSignaturesSequence::write(DcmItem &dataset,
                                                const OFString &card,
                                                const OFString &type,
                                                const char *moduleName)
{
    OFCondition result = EC_IllegalCall;
    if (!EmptyDefaultSequence)
    {
        result = EC_MemoryExhausted;
        DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_DigitalSignaturesSequence);
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
