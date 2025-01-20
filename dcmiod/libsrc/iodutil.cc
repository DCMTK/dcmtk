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
#include "dcmtk/dcmiod/iodtypes.h"
#include "dcmtk/ofstd/ofstring.h"

// --- static helpers ---

OFCondition DcmIODUtil::getAndCheckElementFromDataset(
    DcmItem& dataset, DcmElement& delem, const OFString& vm, const OFString& type, const char* moduleName)
{
    DcmStack stack;
    const DcmTagKey tagKey = delem.getTag();
    OFCondition result     = dataset.search(tagKey, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    if (result.good() && stack.top()->isElement())
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
    if (result.good() && stack.top()->isElement())
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
                    resetConditionIfCheckDisabled(result, checkValue, *delem);
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
    if (xfer.isPixelDataCompressed())
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
    if (pixData == NULL)
    {
        DCMIOD_ERROR("Cannot extract frames from NULL pixel data");
        return EC_IllegalParameter;
    }
    if (numFrames == 0)
    {
        DCMIOD_ERROR("Cannot extract frames from 0 frames");
        return EC_IllegalParameter;
    }
    if (bitsPerFrame == 0)
    {
        DCMIOD_ERROR("Cannot extract frames with 0 bits per frame");
        return EC_IllegalParameter;
    }
    // pixData contains all frames in a single buffer. Each frame is bitsPerFrame bits long.
    // We need to extract each frame into a separate buffer (results vector). Every frame
    // in the input takes bitsPerFrame bits, and pixData's bytes are filled from left to right,
    // with bits filled from right to left (!). This means that the first frame's first bit
    // is the bit on the right of the first byte in pixData, and the last frame's last bit is
    // the bit on the left of the last byte in pixData. However, the last frame's last bit
    // can be in the middle of a byte. The resulting frame contains an exact copy of each
    // frame, however, the last byte of each frame is padded with 0s to make it a full byte.
    // E.g. if the last frame occupies only 3 bits of the last required byte, the last byte
    // of the frame will be padded with 5 0s on the left side of the that byte (since as said, bits
    // are filled from right to left).
    // Calculate number of bytes per frame
    size_t bytesPerFrame = bitsPerFrame / 8;
    if (bitsPerFrame % 8 != 0)
    {
        bytesPerFrame++; // If bitsPerFrame is not a multiple of 8, we need one more byte
    }
    // Reserve memory for results
    results.reserve(numFrames);
    for (size_t i = 0; i < numFrames; i++)
    {
        Uint8* frameData = new Uint8[bytesPerFrame];
        if (frameData == NULL)
        {
            DCMIOD_ERROR("Memory exhausted while extracting frames");
            return EC_MemoryExhausted;
        }
        memset(frameData, 0, bytesPerFrame); // Initialize to 0
        DcmIODTypes::Frame* frame = new DcmIODTypes::Frame();
        if (frame == NULL)
        {
            DCMIOD_ERROR("Memory exhausted while extracting frames");
            delete[] frameData;
            return EC_MemoryExhausted;
        }
        frame->pixData = frameData;
        frame->length = bytesPerFrame;
        results.push_back(frame);
    }

    // Extract frames
    size_t bitsLeftInInputByte = 8;
    size_t bitsLeftInTargetByte = 8;
    size_t bitsLeftInFrame = bitsPerFrame;
    size_t inputByteIndex = 0;
    size_t targetFrameIndex = 0;
    size_t targetByteIndex = 0;
    // Iterate over bits in input data (byte per byte, from left to right, and within byte from right to left)
    for (size_t i = 0; i < numFrames * bitsPerFrame; i++)
    {
        // Get current bit
        Uint8 bit = (pixData[inputByteIndex] >> (8 - bitsLeftInInputByte)) & 0x01;

        // Set bit in current frame to to position calculated from bitsLeftInTargetByte
        results[targetFrameIndex]->pixData[targetByteIndex] |= (bit << (8 - bitsLeftInTargetByte));

        // Move to next bit
        bitsLeftInInputByte--;
        bitsLeftInFrame--;
        bitsLeftInTargetByte--;

        // If we have processed all bits in the current byte, move to next byte
        if (bitsLeftInInputByte == 0)
        {
            bitsLeftInInputByte = 8;
            inputByteIndex++;
        }
        // If we have processed all bits in the current frame, move to next frame,
        // and also start new target frame
        if (bitsLeftInFrame == 0)
        {
            bitsLeftInFrame = bitsPerFrame;
            targetFrameIndex++;
            targetByteIndex = 0;
            bitsLeftInTargetByte = 8;
        }
        // Advance to next target byte if we have filled the current one
        if (bitsLeftInTargetByte == 0)
        {
            bitsLeftInTargetByte = 8;
            targetByteIndex++;
        }
    }
    return EC_Normal;
}


void DcmIODUtil::resetConditionIfCheckDisabled(OFCondition& result, const OFBool checkValue, DcmElement& elem)
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
