/*
 *
 *  Copyright (C) 2016-2024, Open Connections GmbH
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
 *  Module: dcmpmap
 *
 *  Author: Jan Schlamelcher
 *
 *  Purpose: Class for managing the Content Item Macro
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrut.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/ofstd/ofstream.h"

#include "dcmtk/dcmiod/iodcontentitemmacro.h"
#include "dcmtk/dcmiod/iodutil.h"

const OFString ContentItemMacro::ReferencedSOPSequenceItem::m_ComponentName = "ReferencedSOPSequenceItem";

ContentItemMacro::ReferencedSOPSequenceItem::ReferencedSOPSequenceItem(OFshared_ptr<DcmItem> item,
                                                                       OFshared_ptr<IODRules> rules,
                                                                       IODComponent* parent)
    : IODComponent(item, rules, parent)
    , m_SOPInstanceReferenceMacro()
{
    // reset element rules
    resetRules();
}

ContentItemMacro::ReferencedSOPSequenceItem::ReferencedSOPSequenceItem(IODComponent* parent)
    : IODComponent(parent)
    , m_SOPInstanceReferenceMacro()
{
    // reset element rules
    resetRules();
}

ContentItemMacro::ReferencedSOPSequenceItem::ReferencedSOPSequenceItem(const ReferencedSOPSequenceItem& rhs)
    : IODComponent(rhs)
    , m_SOPInstanceReferenceMacro()
{
}

ContentItemMacro::ReferencedSOPSequenceItem::~ReferencedSOPSequenceItem()
{
    // nothing to do
}

OFString ContentItemMacro::ReferencedSOPSequenceItem::getName() const
{
    return m_ComponentName;
}

void ContentItemMacro::ReferencedSOPSequenceItem::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_ReferencedFrameNumber, "1-n", "1C", getName(), DcmIODTypes::IE_UNDEFINED), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ReferencedSegmentNumber, "1-n", "1C", getName(), DcmIODTypes::IE_UNDEFINED),
                     OFTrue);
}

OFCondition ContentItemMacro::ReferencedSOPSequenceItem::read(DcmItem& source, const OFBool clearOldData)
{
    if (clearOldData)
        clearData();

    IODComponent::read(source, OFFalse /* data already cleared */);
    return EC_Normal;
}

OFCondition ContentItemMacro::ReferencedSOPSequenceItem::write(DcmItem& destination)
{
    OFCondition result = EC_Normal;

    result = IODComponent::write(destination);

    return result;
}

SOPInstanceReferenceMacro& ContentItemMacro::ReferencedSOPSequenceItem::getSOPInstanceReferenceMacro()
{
    return m_SOPInstanceReferenceMacro;
}

OFCondition ContentItemMacro::ReferencedSOPSequenceItem::getReferencedFrameNumber(OFString& value,
                                                                                  const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_ReferencedFrameNumber, *m_Item, value, pos);
}

OFCondition ContentItemMacro::ReferencedSOPSequenceItem::getReferencedSegmentNumber(Uint16& value,
                                                                                    const unsigned long pos) const
{
    return m_Item->findAndGetUint16(DCM_ReferencedSegmentNumber, value, pos);
}

OFCondition ContentItemMacro::ReferencedSOPSequenceItem::setReferencedFrameNumber(const OFString& value,
                                                                                  const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmIntegerString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ReferencedFrameNumber, value);
    return result;
}

OFCondition ContentItemMacro::ReferencedSOPSequenceItem::setReferencedSegmentNumber(const Uint16 value,
                                                                                    const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertUint16(DCM_ReferencedSegmentNumber, value);
}

const OFString ContentItemMacro::m_ModuleName = "ContentItemMacro";

ContentItemMacro::ContentItemMacro()
    : IODComponent()
    , m_ConceptNameCodeSequence()
    , m_ConceptCodeSequence()
    , m_MeasurementUnitsCodeSequence()
    , m_ReferencedSOPSequence()
{
    resetRules();
}

ContentItemMacro::ContentItemMacro(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODComponent(item, rules)
    , m_ConceptNameCodeSequence()
    , m_ConceptCodeSequence()
    , m_MeasurementUnitsCodeSequence()
    , m_ReferencedSOPSequence()
{
    // reset element rules
    resetRules();
}

ContentItemMacro::ContentItemMacro(const ContentItemMacro& rhs)
    : IODComponent(rhs)
    , m_ConceptNameCodeSequence()
    , m_ConceptCodeSequence()
    , m_MeasurementUnitsCodeSequence()
    , m_ReferencedSOPSequence()
{
    if (this == &rhs)
        return;

    OFVector<CodeSequenceMacro*>::const_iterator it = rhs.m_ConceptCodeSequence.begin();
    while (it != rhs.m_ConceptCodeSequence.end())
    {
        CodeSequenceMacro* macro = new CodeSequenceMacro(**it);
        m_ConceptCodeSequence.push_back(macro);
        it++;
    }

    it = rhs.m_ConceptNameCodeSequence.begin();
    while (it != rhs.m_ConceptNameCodeSequence.end())
    {
        CodeSequenceMacro* macro = new CodeSequenceMacro(**it);
        m_ConceptNameCodeSequence.push_back(macro);
        it++;
    }

    it = rhs.m_MeasurementUnitsCodeSequence.begin();
    while (it != rhs.m_MeasurementUnitsCodeSequence.end())
    {
        CodeSequenceMacro* macro = new CodeSequenceMacro(**it);
        m_MeasurementUnitsCodeSequence.push_back(macro);
        it++;
    }

    OFVector<ReferencedSOPSequenceItem*>::const_iterator m = rhs.m_ReferencedSOPSequence.begin();
    while (m != rhs.m_ReferencedSOPSequence.end())
    {
        ReferencedSOPSequenceItem* item = new ReferencedSOPSequenceItem(**m);
        m_ReferencedSOPSequence.push_back(item);
        m++;
    }
}

ContentItemMacro::~ContentItemMacro()
{
    DcmIODUtil::freeContainer(m_ConceptNameCodeSequence);
    DcmIODUtil::freeContainer(m_ConceptCodeSequence);
    DcmIODUtil::freeContainer(m_MeasurementUnitsCodeSequence);
    DcmIODUtil::freeContainer(m_ReferencedSOPSequence);
}

OFString ContentItemMacro::getName() const
{
    return m_ModuleName;
}

void ContentItemMacro::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_ValueType, "1", "1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ConceptNameCodeSequence, "1", "1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_DateTime, "1", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_Date, "1", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_Time, "1", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PersonName, "1", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_UID, "1", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_TextValue, "1", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ConceptCodeSequence, "1", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_NumericValue, "1-n", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_FloatingPointValue, "1-n", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_RationalNumeratorValue, "1-n", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_RationalDenominatorValue, "1-n", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
    m_Rules->addRule(new IODRule(DCM_MeasurementUnitsCodeSequence, "1", "1C", getName(), DcmIODTypes::IE_SERIES),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_ReferencedSOPSequence, "1", "1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
}

OFCondition ContentItemMacro::read(DcmItem& source, const OFBool clearOldData)
{
    if (clearOldData)
        clearData();

    IODComponent::read(source, OFFalse /* data already cleared */);
    DcmIODUtil::readSubSequence(
        source, DCM_ConceptNameCodeSequence, m_ConceptNameCodeSequence, m_Rules->getByTag(DCM_ConceptNameCodeSequence));
    DcmIODUtil::readSubSequence(
        source, DCM_ConceptCodeSequence, m_ConceptCodeSequence, m_Rules->getByTag(DCM_ConceptCodeSequence));
    DcmIODUtil::readSubSequence(source,
                                DCM_MeasurementUnitsCodeSequence,
                                m_MeasurementUnitsCodeSequence,
                                m_Rules->getByTag(DCM_MeasurementUnitsCodeSequence));
    DcmIODUtil::readSubSequence(
        source, DCM_ReferencedSOPSequence, m_ReferencedSOPSequence, m_Rules->getByTag(DCM_ReferencedSOPSequence));

    return EC_Normal;
}

OFCondition ContentItemMacro::write(DcmItem& destination)
{
    OFCondition result = EC_Normal;

    if (CodeSequenceMacro* pConceptNameCodeSequence = getConceptNameCodeSequence())
        DcmIODUtil::writeSingleItem(result,
                                    DCM_ConceptNameCodeSequence,
                                    *pConceptNameCodeSequence,
                                    *m_Item,
                                    m_Rules->getByTag(DCM_ConceptNameCodeSequence));
    if (CodeSequenceMacro* pConceptCodeSequence = getConceptCodeSequence())
        DcmIODUtil::writeSingleItem(result,
                                    DCM_ConceptCodeSequence,
                                    *pConceptCodeSequence,
                                    *m_Item,
                                    m_Rules->getByTag(DCM_ConceptCodeSequence));
    if (CodeSequenceMacro* pMeasurementUnitsCodeSequence = getMeasurementUnitsCodeSequence())
        DcmIODUtil::writeSingleItem(result,
                                    DCM_MeasurementUnitsCodeSequence,
                                    *pMeasurementUnitsCodeSequence,
                                    *m_Item,
                                    m_Rules->getByTag(DCM_MeasurementUnitsCodeSequence));
    if (ReferencedSOPSequenceItem* pReferencedSOPSequence = getReferencedSOPSequence())
        DcmIODUtil::writeSingleItem(result,
                                    DCM_ReferencedSOPSequence,
                                    *pReferencedSOPSequence,
                                    *m_Item,
                                    m_Rules->getByTag(DCM_ReferencedSOPSequence));

    if (result.good())
    {
        result = IODComponent::write(destination);
    }

    return result;
}

OFCondition ContentItemMacro::getValueType(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_ValueType, *m_Item, value, pos);
}

OFCondition ContentItemMacro::getValueType(ValueType& value) const
{
    OFString str;
    OFCondition result = DcmIODUtil::getStringValueFromItem(DCM_ValueType, *m_Item, str, 0);
    if (result == EC_Normal)
    {
        if (!str.empty())
        {
            if (str == "DATE")
                value = VT_DATE;
            else if (str == "TIME")
                value = VT_TIME;
            else if (str == "DATETIME")
                value = VT_DATETIME;
            else if (str == "PNAME")
                value = VT_PNAME;
            else if (str == "UIDREF")
                value = VT_UIDREF;
            else if (str == "TEXT")
                value = VT_TEXT;
            else if (str == "CODE")
                value = VT_CODE;
            else if (str == "NUMERIC")
                value = VT_NUMERIC;
            else if (str == "COMPOSITE")
                value = VT_COMPOSITE;
            else if (str == "IMAGE")
                value = VT_IMAGE;
            else
                value = VT_UNKNOWN;
        }
        else
        {
            value = VT_EMPTY;
        }
    }
    else if (result == EC_TagNotFound)
    {
        value = VT_EMPTY;
    }
    else
    {
        DCMIOD_ERROR("Unexpected error, could not get Value Type: " << result.text());
        value = VT_UNKNOWN;
    }
    return result;
}

CodeSequenceMacro* ContentItemMacro::getConceptNameCodeSequence()
{
    return m_ConceptNameCodeSequence.empty() ? OFnullptr : *m_ConceptNameCodeSequence.begin();
}

OFVector<CodeSequenceMacro*>& ContentItemMacro::getEntireConceptNameCodeSequence()
{
    return m_ConceptNameCodeSequence;
}

OFCondition ContentItemMacro::getDateTime(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_DateTime, *m_Item, value, pos);
}

OFCondition ContentItemMacro::getDate(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_Date, *m_Item, value, pos);
}

OFCondition ContentItemMacro::getTime(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_Time, *m_Item, value, pos);
}

OFCondition ContentItemMacro::getPersonName(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_PersonName, *m_Item, value, pos);
}

OFCondition ContentItemMacro::getUID(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_UID, *m_Item, value, pos);
}

OFCondition ContentItemMacro::getTextValue(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_TextValue, *m_Item, value, pos);
}

CodeSequenceMacro* ContentItemMacro::getConceptCodeSequence()
{
    return m_ConceptCodeSequence.empty() ? OFnullptr : *m_ConceptCodeSequence.begin();
}

OFVector<CodeSequenceMacro*>& ContentItemMacro::getEntireConceptCodeSequence()
{
    return m_ConceptCodeSequence;
}

OFCondition ContentItemMacro::getNumericValue(OFString& value, const signed long pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_NumericValue, *m_Item, value, pos);
}

OFCondition ContentItemMacro::getFloatingPointValue(Float64& value, const unsigned long pos) const
{
    return m_Item->findAndGetFloat64(DCM_FloatingPointValue, value, pos);
}

OFCondition ContentItemMacro::getRationalNumeratorValue(Sint32& value, const unsigned long pos) const
{
    return m_Item->findAndGetSint32(DCM_RationalNumeratorValue, value, pos);
}

OFCondition ContentItemMacro::getRationalDenominatorValue(Uint32& value, const unsigned long pos) const
{
    return m_Item->findAndGetUint32(DCM_RationalDenominatorValue, value, pos);
}

CodeSequenceMacro* ContentItemMacro::getMeasurementUnitsCodeSequence()
{
    return m_MeasurementUnitsCodeSequence.empty() ? OFnullptr : *m_MeasurementUnitsCodeSequence.begin();
}

OFVector<CodeSequenceMacro*>& ContentItemMacro::getEntireMeasurementUnitsCodeSequence()
{
    return m_MeasurementUnitsCodeSequence;
}

ContentItemMacro::ReferencedSOPSequenceItem* ContentItemMacro::getReferencedSOPSequence()
{
    return m_ReferencedSOPSequence.empty() ? OFnullptr : *m_ReferencedSOPSequence.begin();
}

OFVector<ContentItemMacro::ReferencedSOPSequenceItem*>& ContentItemMacro::getEntireReferencedSOPSequence()
{
    return m_ReferencedSOPSequence;
}

OFCondition ContentItemMacro::setValueType(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ValueType, value);
    return result;
}

OFCondition ContentItemMacro::setValueType(const ContentItemMacro::ValueType value, const OFBool checkValue)
{
    (void)checkValue;
    OFCondition result;
    switch (value)
    {
        case VT_DATE:
        {
            result = m_Item->putAndInsertOFStringArray(DCM_ValueType, "DATE");
            break;
        }
        case VT_TIME:
        {
            result = m_Item->putAndInsertOFStringArray(DCM_ValueType, "TIME");
            break;
        }
        case VT_DATETIME:
        {
            result = m_Item->putAndInsertOFStringArray(DCM_ValueType, "DATETIME");
            break;
        }
        case VT_PNAME:
        {
            result = m_Item->putAndInsertOFStringArray(DCM_ValueType, "PNAME");
            break;
        }
        case VT_UIDREF:
        {
            result = m_Item->putAndInsertOFStringArray(DCM_ValueType, "UIDREF");
            break;
        }
        case VT_TEXT:
        {
            result = m_Item->putAndInsertOFStringArray(DCM_ValueType, "TEXT");
            break;
        }
        case VT_CODE:
        {
            result = m_Item->putAndInsertOFStringArray(DCM_ValueType, "CODE");
            break;
        }
        case VT_NUMERIC:
        {
            result = m_Item->putAndInsertOFStringArray(DCM_ValueType, "NUMERIC");
            break;
        }
        case VT_COMPOSITE:
        {
            result = m_Item->putAndInsertOFStringArray(DCM_ValueType, "COMPOSITE");
            break;
        }
        case VT_IMAGE:
        {
            result = m_Item->putAndInsertOFStringArray(DCM_ValueType, "IMAGE");
            break;
        }
        default:
        {
            result = IOD_EC_InvalidElementValue;
        }
    }
    return result;
}

OFCondition ContentItemMacro::setDateTime(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_DateTime, value);
    return result;
}

OFCondition ContentItemMacro::setDate(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_Date, value);
    return result;
}

OFCondition ContentItemMacro::setTime(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_Time, value);
    return result;
}

OFCondition ContentItemMacro::setPersonName(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmPersonName::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PersonName, value);
    return result;
}

OFCondition ContentItemMacro::setUID(const OFString& value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertOFStringArray(DCM_UID, value);
}

OFCondition ContentItemMacro::setTextValue(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUnlimitedText::checkStringValue(value) : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_TextValue, value);
    return result;
}

OFCondition ContentItemMacro::setNumericValue(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_NumericValue, value);
    return result;
}

OFCondition
ContentItemMacro::setFloatingPointValue(const Float64 value, const unsigned long pos, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertFloat64(DCM_FloatingPointValue, value, pos);
}

OFCondition
ContentItemMacro::setRationalNumeratorValue(const Sint32 value, const unsigned long pos, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertSint32(DCM_RationalNumeratorValue, value, pos);
}

OFCondition
ContentItemMacro::setRationalDenominatorValue(const Uint32 value, const unsigned long pos, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertUint32(DCM_RationalDenominatorValue, value, pos);
}

OFString ContentItemMacro::toString()
{
    ValueType vt;
    getValueType(vt);
    OFStringStream oss;
    if (getConceptNameCodeSequence())
    {
        oss << getConceptNameCodeSequence()->toString() << ": ";
    }
    else
    {
        oss << "<no name>: ";
    }
    switch (vt)
    {
        case VT_CODE:
            oss << "CODE: " << getConceptCodeSequence()->toString();
            break;
        case VT_COMPOSITE:
        {
            OFString sopClass, sopInstance, frameNumber;
            if (getReferencedSOPSequence())
            {
                getReferencedSOPSequence()->getSOPInstanceReferenceMacro().getReferencedSOPClassUID(sopClass);
                getReferencedSOPSequence()->getSOPInstanceReferenceMacro().getReferencedSOPClassUID(sopInstance);
                getReferencedSOPSequence()->getReferencedFrameNumber(frameNumber);
                oss << "COMPOSITE: " << sopClass << " / " << sopInstance;
                if (!frameNumber.empty())
                {
                    oss << " / Frames: " << frameNumber;
                }
                Uint16 val, pos;
                val = pos = 0;
                if (getReferencedSOPSequence()->getReferencedSegmentNumber(val, pos).good())
                {
                    oss << " / Segments: ";
                    while (getReferencedSOPSequence()->getReferencedSegmentNumber(val, pos).good())
                    {
                        oss << val << " ";
                        pos++;
                    }
                }
            }
            else
            {
                oss << "COMPOSITE: <None>";
            }
            break;
        }
        case VT_DATE:
        {
            OFString date;
            getDate(date);
            oss << "DATE: " << date;
            break;
        }
        case VT_DATETIME:
        {
            OFString datetime;
            getDateTime(datetime);
            oss << "DATETIME: " << datetime;
            break;
        }
        case VT_IMAGE:
        {
            OFString sopClass, sopInstance, frameNumber;
            if (getReferencedSOPSequence())
            {
                getReferencedSOPSequence()->getSOPInstanceReferenceMacro().getReferencedSOPClassUID(sopClass);
                getReferencedSOPSequence()->getSOPInstanceReferenceMacro().getReferencedSOPClassUID(sopInstance);
                getReferencedSOPSequence()->getReferencedFrameNumber(frameNumber);
                oss << "IMAGE: " << sopClass << " / " << sopInstance;
                if (!frameNumber.empty())
                {
                    oss << " / Frames: " << frameNumber;
                }
            }
            else
            {
                oss << "IMAGE: <None>";
            }
            break;
        }
        case VT_NUMERIC:
        {
            OFString val;
            getNumericValue(val);
            oss << "NUMERIC: " << val;
            if (getMeasurementUnitsCodeSequence())
            {
                oss << ", Units: " << getMeasurementUnitsCodeSequence()->toString() << ")";
            }
            Uint16 pos   = 0;
            Float64 fl64 = 0;
            if (getFloatingPointValue(fl64, pos).good())
            {
                oss << ", Float value(s): ";
                do
                {
                    oss << val << " ";
                    pos++;
                } while (getFloatingPointValue(fl64, pos).good());
            }
            else
            {
                oss << ", Float value(s): <none>";
            }
            pos         = 0;
            Sint32 si32 = 0;
            if (getRationalNumeratorValue(si32, pos).good())
            {
                oss << ", Numerator value(s): ";
                do
                {
                    oss << val << " ";
                    pos++;
                } while (getRationalNumeratorValue(si32, pos).good());
            }
            pos         = 0;
            Uint32 ui32 = 0;
            if (getRationalDenominatorValue(ui32, pos).good())
            {
                oss << ", Denominator value(s): ";
                do
                {
                    oss << val << " ";
                    pos++;
                } while (getRationalDenominatorValue(ui32, pos).good());
            }

            break;
        }
        case VT_PNAME:
        {
            OFString val;
            getPersonName(val);
            oss << "PNAME: " << val;
            break;
        }
        case VT_TEXT:
        {
            OFString val;
            getTextValue(val);
            oss << "TEXT: " << val;
            break;
        }
        case VT_TIME:
        {
            OFString val;
            getTime(val);
            oss << "TIME: " << val;
            break;
        }
        case VT_UIDREF:
        {
            OFString val;
            getUID(val);
            oss << "UIDREF: " << val;
            break;
        }
        case VT_EMPTY:
        {
            oss << "<None>";
            break;
        }
        case VT_UNKNOWN:
        {
            OFString val;
            getValueType(val);
            oss << "<Unknown: " << val << ">";
            break;
        }
        default:
        {
            oss << "<Internal error>";
        }
    }
    OFSTRINGSTREAM_GETOFSTRING(oss, val);
    return val;
}
