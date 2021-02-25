/*
 *
 *  Copyright (c) 2008-2021, OFFIS e.V. and ICSMED AG, Oldenburg, Germany
 *  Copyright (C) 2013-2021, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class DRTTypes
 *
 *  Generated manually based on dsrtypes.cc
 *  File created on 2008-12-05
 *  Last modified on 2016-02-12 by Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmrt/drttypes.h"

#include "dcmtk/ofstd/ofstd.h"

/*------------------------*
 *  constant definitions  *
 *------------------------*/

// conditions
makeOFConditionConst(RT_EC_InvalidValue,     OFM_dcmrt, 1, OF_error, "Invalid Value");
makeOFConditionConst(RT_EC_UnsupportedValue, OFM_dcmrt, 2, OF_error, "Unsupported Value");
makeOFConditionConst(RT_EC_InvalidObject,    OFM_dcmrt, 3, OF_error, "Invalid Object");
// error code 4 is reserved for medium-level API error messages


/*--------------------*
 *  global variables  *
 *--------------------*/

OFLogger DCM_dcmrtLogger = OFLog::getLogger("dcmtk.dcmrt");


/*------------------*
 *  implementation  *
 *------------------*/

DRTTypes::~DRTTypes()
{
}


OFCondition DRTTypes::addElementToDataset(OFCondition &result,
                                          DcmItem &dataset,
                                          DcmElement *element,
                                          const OFString &vm,
                                          const OFString &type,
                                          const char *moduleName)
{
    if (element != NULL)
    {
        OFBool triedToInsert = OFFalse;
        if (result.good())
        {
            if ((type == "2") || !element->isEmpty())
            {
                triedToInsert = OFTrue;
                /* insert non-empty element or empty "type 2" element */
                result = dataset.insert(element, OFTrue /*replaceOld*/);
                if (DCM_dcmrtLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
                    checkElementValue(*element, vm, type, result, moduleName);
            }
            else if (type == "1")
            {
                /* empty element value not allowed for "type 1" */
                result = RT_EC_InvalidValue;
                if (DCM_dcmrtLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
                    checkElementValue(*element, vm, type, result, moduleName);
            }
        }
        /* delete element if not inserted into the dataset */
        if (result.bad() || !triedToInsert)
            delete element;
    } else
        result = EC_MemoryExhausted;
    return result;
}


OFCondition DRTTypes::getElementFromDataset(DcmItem &dataset,
                                            DcmElement &element)
{
    DcmStack stack;
    OFCondition result = dataset.search(element.getTag(), stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    if (result.good())
    {
        /* copy object from search stack */
        result = element.copyFrom(*stack.top());
    }
    if (result.bad())
        element.clear();
    return result;
}


OFCondition DRTTypes::getStringValueFromElement(const DcmElement &element,
                                                OFString &stringValue,
                                                const signed long pos)
{
    OFCondition result = EC_Normal;
    if (pos < 0)
        result = OFconst_cast(DcmElement &, element).getOFStringArray(stringValue);
    else
        result = OFconst_cast(DcmElement &, element).getOFString(stringValue, OFstatic_cast(unsigned long, pos));
    if (result.bad())
        stringValue.clear();
    return result;
}


OFCondition DRTTypes::getStringValueFromDataset(DcmItem &dataset,
                                                const DcmTagKey &tagKey,
                                                OFString &stringValue)
{
    return dataset.findAndGetOFString(tagKey, stringValue, 0, OFFalse /*searchIntoSub*/);
}


OFCondition DRTTypes::putStringValueToDataset(DcmItem &dataset,
                                              const DcmTag &tag,
                                              const OFString &stringValue,
                                              const OFBool allowEmpty)
{
    OFCondition result = EC_Normal;
    if (allowEmpty || !stringValue.empty())
        result = dataset.putAndInsertOFStringArray(tag, stringValue, OFTrue /*replaceOld*/);
    return result;
}


OFBool DRTTypes::checkElementValue(DcmElement &element,
                                   const OFString &vm,
                                   const OFString &type,
                                   const OFCondition &searchCond,
                                   const char *moduleName)
{
    OFBool result = OFTrue;
    DcmTag tag = element.getTag();
    const OFString tagName = tag.getTagName();
    const OFString module = (moduleName == NULL) ? "RT object" : moduleName;
    /* NB: type 1C and 2C cannot be checked, assuming to be optional = type 3 */
    if (((type == "1") || (type == "2")) && searchCond.bad())
    {
        DCMRT_WARN(tagName << " " << tag << " absent in " << module << " (type " << type << ")");
        result = OFFalse;
    }
    else if (element.isEmpty(OFTrue /*normalize*/))
    {
        /* however, type 1C should never be present with empty value */
        if (((type == "1") || (type == "1C")) && searchCond.good())
        {
            DCMRT_WARN(tagName << " " << tag << " empty in " << module << " (type " << type << ")");
            result = OFFalse;
        }
    } else {
        const OFCondition checkResult = element.checkValue(vm, OFTrue /*oldFormat*/);
        if (checkResult == EC_InvalidCharacter)
        {
            DCMRT_WARN(tagName << " " << tag << " contains invalid character(s) in " << module);
            result = OFFalse;
        }
        else if (checkResult == EC_ValueRepresentationViolated)
        {
            DCMRT_WARN(tagName << " " << tag << " violates VR definition in " << module);
            result = OFFalse;
        }
        else if (checkResult == EC_ValueMultiplicityViolated)
        {
            const OFString vmText = (element.getVR() == EVR_SQ) ? " #items" : " VM";
            DCMRT_WARN(tagName << " " << tag << vmText << " != " << vm << " in " << module);
            result = OFFalse;
        }
        else if (checkResult == EC_MaximumLengthViolated)
        {
            DCMRT_WARN(tagName << " " << tag << " violates maximum VR length in " << module);
            result = OFFalse;
        }
        else if (checkResult.bad())
        {
            DCMRT_DEBUG("INTERNAL ERROR while checking value of " << tagName << " " << tag << " in " << module);
        }
    }
    return result;
}


OFCondition DRTTypes::getAndCheckElementFromDataset(DcmItem &dataset,
                                                    DcmElement &element,
                                                    const OFString &vm,
                                                    const OFString &type,
                                                    const char *moduleName)
{
    OFCondition result = getElementFromDataset(dataset, element);
    if (!checkElementValue(element, vm, type, result, moduleName))
        result = RT_EC_InvalidValue;
    return result;
}


OFCondition DRTTypes::getAndCheckStringValueFromDataset(DcmItem &dataset,
                                                        const DcmTagKey &tagKey,
                                                        OFString &stringValue,
                                                        const OFString &vm,
                                                        const OFString &type,
                                                        const char *moduleName)
{
    DcmStack stack;
    OFCondition result = dataset.search(tagKey, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    if (result.good())
    {
        DcmElement *element = OFstatic_cast(DcmElement *, stack.top());
        if (element != NULL)
        {
            if (checkElementValue(*element, vm, type, result, moduleName))
                result = element->getOFString(stringValue, 0);
            else
                result = RT_EC_InvalidValue;
        } else
            result = EC_CorruptedData;
    } else {
        if ((type == "1") || (type == "2"))
        {
            const OFString tagName = DcmTag(tagKey).getTagName();
            const OFString module = (moduleName == NULL) ? "RT object" : moduleName;
            DCMRT_WARN(tagName << " " << tagKey << " absent in " << module << " (type " << type << ")");
        }
    }
    if (result.bad())
        stringValue.clear();
    return result;
}
