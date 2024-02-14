/*
 *
 *  Copyright (C) 2015-2024, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmiod
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Static helper functionality for dcmiod module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmdata/dctypes.h" // logger
#include "dcmtk/dcmiod/iodrules.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/ofstd/ofstring.h"

// --- static helpers ---

OFCondition DcmIODUtil::getAndCheckElementFromDataset(
    DcmItem& dataset, DcmElement& delem, const OFString& vm, const OFString& type, const char* moduleName)
{
    DcmStack stack;
    const DcmTagKey tagKey = delem.getTag();
    OFCondition result     = dataset.search(tagKey, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    if (result.good())
    {
        /* copy object from search stack */
        result = delem.copyFrom(*stack.top());
        /* we need a reference to the original element in order to determine the SpecificCharacterSet */
        checkElementValue(OFstatic_cast(DcmElement*, stack.top()),
                          tagKey,
                          vm,
                          type,
                          result,
                          moduleName,
                          dcmtk::log4cplus::WARN_LOG_LEVEL);
    }
    /* the element could not be found in the dataset */
    else
    {
        checkElementValue(delem, vm, type, result, moduleName, dcmtk::log4cplus::WARN_LOG_LEVEL);
    }
    return result;
}

OFCondition DcmIODUtil::getAndCheckElementFromDataset(DcmItem& dataset,
                                                      const DcmTagKey& tagKey,
                                                      DcmElement*& delem,
                                                      const OFString& vm,
                                                      const OFString& type,
                                                      const char* moduleName)
{
    if (delem)
        return EC_IllegalParameter;

    DcmStack stack;
    OFCondition result = dataset.search(tagKey, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    if (result.good())
    {
        /* copy object from search stack */
        delem = OFstatic_cast(DcmElement*, stack.top()->clone());
        /* we need a reference to the original element in order to determine the SpecificCharacterSet */
        checkElementValue(OFstatic_cast(DcmElement*, stack.top()),
                          tagKey,
                          vm,
                          type,
                          result,
                          moduleName,
                          dcmtk::log4cplus::WARN_LOG_LEVEL);
    }
    /* the element could not be found in the dataset */
    else
        checkElementValue(delem, tagKey, vm, type, result, moduleName, dcmtk::log4cplus::WARN_LOG_LEVEL);

    return result;
}

OFCondition DcmIODUtil::getAndCheckElementFromDataset(DcmItem& dataset, DcmElement& delem, const IODRule* rule)
{
    if (rule == NULL)
        return EC_CannotCheck;

    return getAndCheckElementFromDataset(dataset, delem, rule->getVM(), rule->getType(), rule->getModule().c_str());
}

OFCondition DcmIODUtil::getAndCheckElementFromDataset(DcmItem& dataset, DcmElement*& delem, const IODRule* rule)
{
    if (rule == NULL)
        return EC_CannotCheck;

    return getAndCheckElementFromDataset(
        dataset, rule->getTagKey(), delem, rule->getVM(), rule->getType(), rule->getModule().c_str());
}

OFCondition
DcmIODUtil::copyElementToDataset(OFCondition& result, DcmItem& dataset, const DcmElement& delem, const IODRule* rule)
{
    if (rule == NULL)
        return EC_CannotCheck;

    if (result.bad())
        return result;

    // addElementToDataset() consumes element, so create a copy
    DcmElement* copy = OFstatic_cast(DcmElement*, delem.clone());
    if (!copy)
        return EC_MemoryExhausted;

    return addElementToDataset(result, dataset, copy, rule);
}

OFCondition DcmIODUtil::copyElementToDataset(OFCondition& result,
                                             DcmItem& dataset,
                                             const DcmElement& delem,
                                             const OFString& vm,
                                             const OFString& type,
                                             const char* moduleName)
{
    // Create temporary rule (in order to avoid code-copying)
    IODRule rule(delem.getTag(), vm, type, moduleName, DcmIODTypes::IE_UNDEFINED);
    return copyElementToDataset(result, dataset, delem, &rule);
}

OFCondition
DcmIODUtil::addElementToDataset(OFCondition& result, DcmItem& dataset, DcmElement* delem, const IODRule* rule, const OFBool checkValue)
{
    OFBool insertionOK = OFFalse;
    if (result.good())
    {
        if (rule != NULL)
        {
            // Create empty type 2 element if required
            OFString type = rule->getType();
            if (delem == NULL)
            {
                if (type == "2")
                {
                    delem = DcmItem::newDicomElement(rule->getTagKey());
                    if (delem == NULL)
                    {
                        result = EC_MemoryExhausted;
                        return result;
                    };
                }
                else if (type != "1")
                {
                    // Not type 1 or type 2 means the is type 1C, 2C or 3. For those it is
                    // fine to not insert anything
                    return EC_Normal;
                }
                else
                {
                    // type 1 is missing
                    DCMIOD_ERROR(DcmTag(rule->getTagKey()).getTagName()
                                 << " " << rule->getTagKey() << " absent in " << rule->getModule() << " (type "
                                 << rule->getType() << ")");
                    result = IOD_EC_MissingAttribute;
                    return result;
                }
            }
            // At this point, we certainly have an element. Check its value (empty ok for type 2).
            dcmtk::log4cplus::LogLevel logLevel;
            if (checkValue) logLevel = dcmtk::log4cplus::ERROR_LOG_LEVEL; else logLevel = dcmtk::log4cplus::WARN_LOG_LEVEL;
            if ((type == "2") || !delem->isEmpty())
            {
                // Insert non-empty element or empty "type 2" element. First, perform the insertion, and then
                // check the value. This is (at least) required for checking the character set of string values which
                // relies on the element context, i.e. the surrounding item or dataset.
                result = dataset.insert(delem, OFTrue /*replaceOld*/);
                if (result.good())
                {
                    result = checkElementValue(*delem,
                                               rule->getVM(),
                                               type,
                                               result,
                                               rule->getModule().c_str(),
                                               logLevel);
                    reset_value_check_result(result, checkValue, *delem);
                }
                if (result.good())
                {
                    insertionOK = OFTrue;
                }
                // remove element if value is invalid
                else
                {
                    dataset.remove(delem);
                }
            }
            else if (type == "1")
            {
                // Empty element value not allowed for "type 1"
                result = EC_InvalidValue;
                checkElementValue(
                    *delem, rule->getVM(), type, result, rule->getModule().c_str(), logLevel);
            }
        }
        else
        {
            // No rule, no checks
            result = EC_CannotCheck;
        }
    }
    if (!insertionOK)
    {
        // Delete element since dataset did not take over ownership
        delete delem;
    }

    return result;
}

const OFString& DcmIODUtil::currentDate(OFString& dateString)
{
    if (DcmDate::getCurrentDate(dateString).bad())
    {
        dateString = "";
    }
    return dateString;
}

const OFString& DcmIODUtil::currentTime(OFString& timeString)
{
    if (DcmTime::getCurrentTime(timeString, OFTrue /*seconds*/, OFFalse /*fraction*/).bad())
    {
        timeString = "";
    }
    return timeString;
}

OFCondition DcmIODUtil::checkElementValue(const DcmElement* delem,
                                          const DcmTagKey& tagKey,
                                          const OFString& vm,
                                          const OFString& type,
                                          const OFCondition& searchCond,
                                          const char* moduleName,
                                          const dcmtk::log4cplus::LogLevel logLevel)
{
    OFCondition result     = EC_Normal;
    const OFString tagName = DcmTag(tagKey).getTagName();
    const OFString module  = (moduleName == NULL) ? "IOD" : moduleName;
    OFOStringStream error;
    /* NB: type 1C and 2C cannot be checked, assuming to be optional */
    if (((type == "1") || (type == "2")) && searchCond.bad())
    {
        error << tagName << " " << tagKey << " absent in " << module << " (type " << type << ")";
        result = IOD_EC_MissingAttribute;
    }
    else if ((delem == NULL)
             || OFconst_cast(DcmElement*, delem)
                    ->isEmpty(OFTrue /*normalize*/)) // cast away constness of delem; value modification can happen (eg.
                                                     // to remove padding)
    {
        /* however, type 1C should never be present with empty value */
        if (((type == "1") || (type == "1C")) && searchCond.good())
        {
            error << tagName << " " << tagKey << " empty in " << module << " (type " << type << ")";
            result = EC_MissingValue;
        }
    }
    else
    {
        result = OFconst_cast(DcmElement*, delem)
                     ->checkValue(vm, OFTrue /*oldFormat*/); // cast away constness of delem; value modification can
                                                             // happen (eg. to remove padding)
        if (result == EC_InvalidCharacter)
        {
            error << tagName << " " << tagKey << " contains invalid character(s) in " << module;
        }
        else if (result == EC_ValueRepresentationViolated)
        {
            error << tagName << " " << tagKey << " violates VR definition in " << module;
        }
        else if (result == EC_ValueMultiplicityViolated)
        {
            const OFString vmText = (delem->getVR() == EVR_SQ) ? " #items" : " VM";
            error << tagName << " " << tagKey << vmText << " != " << vm << " in " << module;
        }
        else if (result == EC_MaximumLengthViolated)
        {
            error << tagName << " " << tagKey << " violates maximum VR length in " << module;
        }
        else if (result.bad())
        {
            error << "INTERNAL ERROR while checking value of " << tagName << " " << tagKey << " in " << module;
            result = EC_Normal;
        }
    }
    OFSTRINGSTREAM_GETSTR(error, tmpString)
    if (strlen(tmpString) > 0)
    {
        switch (logLevel)
        {
            case dcmtk::log4cplus::TRACE_LOG_LEVEL:
                DCMIOD_TRACE(tmpString);
                break;
            case dcmtk::log4cplus::DEBUG_LOG_LEVEL:
                DCMIOD_DEBUG(tmpString);
                break;
            case dcmtk::log4cplus::WARN_LOG_LEVEL:
                DCMIOD_WARN(tmpString);
                break;
            case dcmtk::log4cplus::INFO_LOG_LEVEL:
                DCMIOD_INFO(tmpString);
                break;
            case dcmtk::log4cplus::ERROR_LOG_LEVEL:
                DCMIOD_ERROR(tmpString);
                break;
            case dcmtk::log4cplus::FATAL_LOG_LEVEL:
                DCMIOD_FATAL(tmpString);
                break;
            default:
                DCMIOD_WARN(tmpString);
                ;
        }
    }
    OFSTRINGSTREAM_FREESTR(tmpString)
    return result;
}

OFCondition DcmIODUtil::checkElementValue(const DcmElement& delem,
                                          const OFString& vm,
                                          const OFString& type,
                                          const OFCondition& searchCond,
                                          const char* moduleName,
                                          const dcmtk::log4cplus::LogLevel logLevel)
{
    /* call the real function */
    return checkElementValue(&delem, delem.getTag(), vm, type, searchCond, moduleName, logLevel);
}

OFCondition DcmIODUtil::getStringValueFromElement(const DcmElement& delem, OFString& stringValue, const signed long pos)
{
    OFCondition result = EC_Normal;
    // cast away constness of delem; value modification can happen (eg. to remove padding)
    if (pos < 0)
        result = OFconst_cast(DcmElement&, delem).getOFStringArray(stringValue);
    else
        result = OFconst_cast(DcmElement&, delem).getOFString(stringValue, OFstatic_cast(unsigned long, pos));
    if (result.bad())
        stringValue.clear();
    return result;
}

OFCondition DcmIODUtil::getFloat64ValueFromElement(const DcmElement& delem, Float64& result, const unsigned long pos)
{
    return OFconst_cast(DcmElement&, delem).getFloat64(result, OFstatic_cast(unsigned long, pos));
}

OFCondition DcmIODUtil::getFloat64ValuesFromElement(const DcmElement& delem, OFVector<Float64>& result)
{
    Float64* floats  = NULL;
    OFCondition cond = OFconst_cast(DcmElement&, delem).getFloat64Array(floats);
    if (cond.good())
    {
        size_t vm = OFconst_cast(DcmElement&, delem).getNumberOfValues();
        for (size_t n = 0; n < vm; n++)
        {
            result.push_back(floats[n]);
        }
    }
    return cond;
}

OFCondition
DcmIODUtil::getStringValueFromItem(const DcmTagKey& key, DcmItem& item, OFString& result, const signed long& pos)
{
    DcmElement* elem = NULL;
    item.findAndGetElement(key, elem);
    if (elem != NULL)
        return DcmIODUtil::getStringValueFromElement(*elem, result, pos);
    else
        return EC_TagNotFound;
}

OFCondition
DcmIODUtil::getFloat64ValueFromItem(const DcmTagKey& key, DcmItem& item, Float64& result, const unsigned long& pos)
{
    DcmElement* elem = NULL;
    OFCondition cond = item.findAndGetElement(key, elem);
    if (elem != NULL)
    {
        cond = DcmIODUtil::getFloat64ValueFromElement(*elem, result, pos);
    }
    return cond;
}

OFCondition DcmIODUtil::getFloat64ValuesFromItem(const DcmTagKey& key, DcmItem& item, OFVector<Float64>& result)
{
    DcmElement* elem = NULL;
    OFCondition cond = item.findAndGetElement(key, elem);
    if (elem != NULL)
    {
        cond = DcmIODUtil::getFloat64ValuesFromElement(*elem, result);
    }
    return cond;
}

OFCondition DcmIODUtil::setFloat64ValuesOnElement(DcmElement& delem,
                                                  const OFVector<Float64>& values,
                                                  const OFString& vm,
                                                  const OFBool check)
{
    OFCondition result;
    if (values.size() > OFnumeric_limits<unsigned long>::max())
    {
        DCMIOD_ERROR("Too many values provided (" << values.size() << " for element: " << delem.getTag());
        return IOD_EC_InvalidElementValue;
    }
    const unsigned long vmCount          = OFstatic_cast(unsigned long, values.size());
    OFVector<Float64>::const_iterator it = values.begin();
    for (unsigned long count = 0; count < vmCount; count++)
    {
        result = delem.putFloat64((*it), count);
        if (result.bad())
        {
            DCMIOD_WARN(delem.getTag() << ": Setting value "
                                       << " #" << count << " to \" " << *it << "\" not possible");
        }
        else if (check)
        {
            result = DcmElement::checkVM(vmCount, vm);
        }
        it++;
    }
    return result;
}

OFCondition DcmIODUtil::getFloat32ValueFromElement(const DcmElement& delem, Float32& result, const unsigned long pos)
{
    return OFconst_cast(DcmElement&, delem).getFloat32(result, OFstatic_cast(unsigned long, pos));
}

OFCondition DcmIODUtil::setFloat32ValuesOnElement(DcmElement& delem,
                                                  const OFVector<Float32>& values,
                                                  const OFString& vm,
                                                  const OFBool check)
{
    OFCondition result;
    if (values.size() > OFnumeric_limits<unsigned long>::max())
    {
        DCMIOD_ERROR("Too many values provided (" << values.size() << " for element: " << delem.getTag());
        return IOD_EC_InvalidElementValue;
    }
    const unsigned long vmCount          = OFstatic_cast(unsigned long, values.size());
    OFVector<Float32>::const_iterator it = values.begin();
    for (unsigned long count = 0; count < vmCount; count++)
    {
        result = delem.putFloat32((*it), count);
        if (result.bad())
        {
            DCMIOD_WARN(delem.getTag() << ": Setting value "
                                       << " #" << count << " to \" " << *it << "\" not possible");
        }
        else if (check)
        {
            result = DcmElement::checkVM(vmCount, vm);
        }
        it++;
    }
    return result;
}

OFCondition DcmIODUtil::setUint16ValuesOnElement(DcmElement& delem,
                                                 const OFVector<Uint16>& values,
                                                 const OFString& vm,
                                                 const OFBool check)
{
    OFCondition result;
    if (values.size() > OFnumeric_limits<unsigned long>::max())
    {
        DCMIOD_ERROR("Too many values provided (" << values.size() << " for element: " << delem.getTag());
        return IOD_EC_InvalidElementValue;
    }
    const unsigned long vmCount         = OFstatic_cast(unsigned long, values.size());
    OFVector<Uint16>::const_iterator it = values.begin();
    for (unsigned long count = 0; count < vmCount; count++)
    {
        result = delem.putUint16((*it), count);
        if (result.bad())
        {
            DCMIOD_WARN(delem.getTag() << ": Setting value "
                                       << " #" << count << " to \" " << *it << "\" not possible");
        }
        else if (check)
        {
            result = DcmElement::checkVM(vmCount, vm);
        }
        it++;
    }
    return result;
}

OFCondition DcmIODUtil::getUint16ValuesFromElement(DcmElement& delem, OFVector<Uint16>& values)
{
    OFCondition result;
    const size_t count = delem.getNumberOfValues();
    values.reserve(count);
    for (size_t i = 0; i < count; i++)
    {
        Uint16 val;
        result = delem.getUint16(val, OFstatic_cast(unsigned long, i));
        if (result.bad())
        {
            DCMIOD_WARN(delem.getTag() << ": Getting value "
                                       << " #" << i << " not possible");
            break;
        }
        values.push_back(val);
    }
    return result;
}

void DcmIODUtil::checkSubSequence(OFCondition& result,
                                  DcmItem& surroundingItem,
                                  const DcmTagKey& seqKey,
                                  const OFString& cardinality,
                                  const OFString& type,
                                  const OFString& module,
                                  const dcmtk::log4cplus::LogLevel logLevel)
{
    OFCondition exists = EC_Normal;
    /* check result */
    if (result.good())
    {
        DcmSequenceOfItems* seq = NULL;
        exists                  = surroundingItem.findAndGetSequence(seqKey, seq);
        result = DcmIODUtil::checkElementValue(seq, seqKey, cardinality, type, exists, module.c_str(), logLevel);
    }
}

OFCondition DcmIODUtil::getAndCheckSingleItem(DcmSequenceOfItems& seq, DcmItem*& item, const DcmTagKey& checkKey)
{
    item                   = NULL;
    const OFString tagName = OFconst_cast(DcmTag*, &seq.getTag())->getTagName(); // getTagName is not const...
    if (checkKey != DCM_UndefinedTagKey)
    {
        if (seq.getTag() != checkKey)
        {
            DCMIOD_ERROR("Expected sequence " << checkKey << " but got " << &seq.getTag() << "(" << tagName << ")");
            return EC_ItemNotFound;
        }
    }

    // get actual tag name and cardinality
    const unsigned long card = seq.card();
    if (card != 1)
    {
        if (card > 1)
        {
            DCMIOD_WARN("Only single item allowed in " << tagName << ", ignoring " << card - 1 << " other items");
        }
        else // card = 0
        {
            DCMIOD_WARN("Cannot read from sequence " << tagName << ": No item found");
            return EC_ItemNotFound;
        }
    }
    // get item and return it
    item = seq.getItem(0);
    if (item == NULL)
    {
        DCMIOD_ERROR("Cannot read item from " << tagName << ": No item found (internal error)");
        return EC_CorruptedData;
    }
    return EC_Normal;
}

OFCondition DcmIODUtil::checkSOPClass(DcmItem* item, const OFString& desiredSOPClass, OFString& valueFound)
{
    valueFound.clear();
    if ((item == NULL) || (item->card() == 0))
    {
        DCMIOD_TRACE("Cannot check SOP Class UID: Dataset not present or empty");
        return EC_IllegalParameter;
    }
    OFCondition result = item->findAndGetOFString(DCM_SOPClassUID, valueFound);
    if (result.bad())
    {
        DCMIOD_TRACE("No SOP Class UID in file, giving up");
        return EC_TagNotFound;
    }
    if (valueFound != desiredSOPClass)
    {
        DCMIOD_TRACE("File is not of SOP Class " << desiredSOPClass << ", but instead SOP Class is "
                                                 << dcmFindNameOfUID(valueFound.c_str(), valueFound.c_str()));
        return EC_InvalidValue;
    }
    return EC_Normal;
}

OFBool DcmIODUtil::isSequenceTag(const DcmTagKey& key, const OFString& privateCreator)
{
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry* dictRef             = NULL;
    if (privateCreator.empty())
        dictRef = globalDataDict.findEntry(key, NULL);
    else
        dictRef = globalDataDict.findEntry(key, privateCreator.c_str());
    DcmVR vr;
    if (dictRef)
    {
        vr = dictRef->getVR();
    }
    dcmDataDict.rdunlock();
    if (vr.getEVR() == EVR_SQ)
        return OFTrue;
    return OFFalse;
}

const DcmTagKey DcmIODUtil::parseTagKey(const OFString& keyString)
{
    unsigned int g, e;
    DcmTagKey resultKey = DCM_UndefinedTagKey;
    if (sscanf(keyString.c_str(), "(%x,%x)", &g, &e) == 2)
    {
        resultKey.set(OFstatic_cast(Uint16, g), OFstatic_cast(Uint16, e));
    }
    return resultKey;
}

OFCondition DcmIODUtil::decompress(DcmDataset& dset)
{
    DcmXfer xfer = dset.getOriginalXfer();
    if (xfer.isEncapsulated())
    {
        if (EC_Normal != dset.chooseRepresentation(EXS_LittleEndianExplicit, NULL))
        {

            DCMIOD_ERROR("No conversion from " << xfer.getXferName() << " to uncompressed transfer syntax possible!");
            return IOD_EC_CannotDecompress;
        }
    }

    return EC_Normal;
}

OFString DcmIODUtil::createUID(const Uint8 level)
{
    char uid[100];
    switch (level)
    {
        case 0:
            dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT);
            break;
        case 1:
            dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT);
            break;
        case 2:
            dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT);
            break;
        default:
            dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT);
    }
    return uid;
}

Uint32 DcmIODUtil::limitMaxFrames(const size_t numFramesPresent, const OFString& warning)
{
    // limit to 2^31-1 since this is the Number of Frames attribute's VR IS' maximum value
    if (numFramesPresent > 2147483647)
    {
        DCMIOD_WARN(warning);
        return 2147483647;
    }
    return OFstatic_cast(Uint32, numFramesPresent);
}

OFCondition DcmIODUtil::extractBinaryFrames(Uint8* pixData,
                                            const size_t numFrames,
                                            const size_t bitsPerFrame,
                                            OFVector<DcmIODTypes::Frame*>& results)
{
    // Will hold the bit position (0-7) that the current frame starts from. The
    // first frame will always start at bit 0.
    Uint8 bitShift = 0;
    // Compute length in bytes we need to consider for each frame.
    size_t frameLengthBytes = bitsPerFrame / 8;
    // If the number of bits is not dividable by 8, we need part of an extra
    // byte in the end. Since we like to set the unused bits to 0 in such a last
    // byte to 0, remember the number.
    size_t overlapBits = (8 - (bitsPerFrame % 8)) % 8;
    // Add an extra byte if we we fill a partial byte in the end
    if (overlapBits != 0)
        frameLengthBytes++;
    // Points to current reading position within pixData
    Uint8* readPos = pixData;
    // Loop over each frame and copy it to Frame structures
    for (size_t f = 0; f < numFrames; f++)
    {
        // Create frame with correct length and copy 1:1 from pixel data
        DcmIODTypes::Frame* frame = new DcmIODTypes::Frame();
        frame->length             = frameLengthBytes;
        frame->pixData            = new Uint8[frameLengthBytes];
        if (!frame->pixData)
        {
            delete frame;
            return EC_MemoryExhausted;
        }
        memcpy(frame->pixData, readPos, frame->length);
        // ---------------------------------------------------------
        // Remove bits in first byte from former frame if necessary:
        // ---------------------------------------------------------
        // If we have been copying too much, i.e the first bits of this frame
        // actually belong to the former frame, shift the whole frame this amount
        // of bits to the left in order to shift the superfluous bits out, i.e.
        // make frame start at byte boundary.
        if (bitShift > 0)
        {
            DcmIODUtil::alignFrameOnByteBoundary(frame->pixData, frame->length, 8 - bitShift);
        }
        // -------------------------------------------------------
        // Mask out (set 0) bits of last byte if not used by frame
        // -------------------------------------------------------
        // Adapt last byte by masking out unused bits (i.e. those belonging to next frame).
        // A reader should ignore those unused bits anyway.
        frame->pixData[frame->length - 1] = OFstatic_cast(unsigned char, (frame->pixData[frame->length - 1] << overlapBits)) >> overlapBits;
        // Store frame
        results.push_back(frame);
        // Compute the bitshift created by this frame
        bitShift = (8 - ((f + 1) * bitsPerFrame) % 8) % 8;
        // If the previous byte read has not been used completely, i.e. it contains
        // also bytes of the next frame, rewind read position to the previous byte
        // that was partially read. Otherwise skip to the next full byte.
        if (bitShift > 0)
        {

            readPos = readPos + frame->length - 1;
        }
        else
        {
            readPos = readPos + frame->length;
        }
    }
    return EC_Normal;
}

void DcmIODUtil::alignFrameOnByteBoundary(Uint8* buf, size_t bufLen, Uint8 numBits)
{
    if (numBits > 7)
    {
        DCMIOD_ERROR("Invalid input data: alignFrameOnByteBoundary() can only shift 0-7 bits");
        return;
    }
    for (size_t x = 0; x < bufLen - 1; x++)
    {
        // Shift current byte
        buf[x] = OFstatic_cast(unsigned char, buf[x]) >> numBits;
        // isolate portion of next byte that must be shifted into current byte
        Uint8 next = (buf[x + 1] << (8 - numBits));
        // Take over portion from next byte
        buf[x] |= next;
    }
    // Shift last byte manually
    buf[bufLen - 1] = OFstatic_cast(unsigned char, buf[bufLen - 1]) >> numBits;
}


void DcmIODUtil::reset_value_check_result(OFCondition& result, const OFBool checkValue, DcmElement& elem)
{
    if (!checkValue)
    {
        if ( (result == EC_ValueRepresentationViolated) ||
             (result == EC_MaximumLengthViolated) ||
             (result == EC_InvalidCharacter) ||
             (result == EC_ValueMultiplicityViolated) )
        {
            // print element to string
            OFOStringStream oss;
            oss << elem.getTag() << " " << DcmVR(elem.getVR()).getVRName() << " ";
            oss << DcmTag(elem.getTag()).getTagName() << " ";
            if (elem.getLength() > 1024)
            {
                oss << "(value too long for printing)";
            }
            {
                OFString val;
                elem.getOFString(val, 0, OFTrue);
                oss << "[" << val << "]";
            }

            DCMIOD_DEBUG("Ignoring error (" << result.text() <<") when checking element: " << oss.str().c_str());
            result = EC_Normal;
        }
    }
}
