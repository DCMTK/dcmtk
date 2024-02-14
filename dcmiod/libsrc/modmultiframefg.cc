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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing the Multi-frame Functional Groups Module
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modmultiframefg.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h" // for static helpers

const OFString IODMultiFrameFGModule::m_ModuleName = "MultiframeFunctionalGroupsModule";

IODMultiFrameFGModule::IODMultiFrameFGModule(OFshared_ptr<DcmItem> data, OFshared_ptr<IODRules> rules)
    : IODModule(data, rules)
    , m_ConcatenationInfo(data, rules)
{
    // reset element rules
    resetRules();
}

OFString IODMultiFrameFGModule::getName() const
{
    return m_ModuleName;
}

IODMultiFrameFGModule::IODMultiFrameFGModule()
    : IODModule()
    , m_ConcatenationInfo()
{
    resetRules();
}

void IODMultiFrameFGModule::resetRules()
{
    m_Rules->addRule(new IODRule(DCM_InstanceNumber, "1", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ContentDate, "1", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ContentTime, "1", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_NumberOfFrames, "1", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_RepresentativeFrameNumber, "1", "3", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
    m_ConcatenationInfo.resetRules();
}

IODMultiFrameFGModule::~IODMultiFrameFGModule()
{
}

OFCondition IODMultiFrameFGModule::read(DcmItem& source, const bool clearOldData)
{
    IODModule::read(source, clearOldData);
    m_ConcatenationInfo.read(source, clearOldData);
    return EC_Normal;
}

OFCondition IODMultiFrameFGModule::write(DcmItem& destination)
{
    OFCondition result = IODModule::write(destination);
    if (result.good())
        m_ConcatenationInfo.write(destination);
    return result;
}

IODMultiFrameFGModule::ConcatenationInfo& IODMultiFrameFGModule::getConcatenationInfo()
{
    return m_ConcatenationInfo;
}

OFCondition IODMultiFrameFGModule::getInstanceNumber(Sint32& value, const unsigned int pos)
{
    return m_Item->findAndGetSint32(DCM_InstanceNumber, value, pos);
}

OFCondition IODMultiFrameFGModule::getContentDate(OFString& value, const long signed int pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ContentDate, *m_Item, value, pos);
}

OFCondition IODMultiFrameFGModule::getContentTime(OFString& value, const long signed int pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ContentTime, *m_Item, value, pos);
}

OFCondition IODMultiFrameFGModule::getNumberOfFrames(Sint32& value, const unsigned int pos)
{
    return m_Item->findAndGetSint32(DCM_NumberOfFrames, value, pos);
}

OFCondition IODMultiFrameFGModule::getRepresentativeFrameNumber(Uint16& value, const unsigned int pos)
{
    return m_Item->findAndGetUint16(DCM_RepresentativeFrameNumber, value, pos);
}

// -- setters --

OFCondition
IODMultiFrameFGModule::setConcatenationInfo(const IODMultiFrameFGModule::ConcatenationInfo& concatenationInfo)
{
    m_ConcatenationInfo = concatenationInfo;
    return EC_Normal;
}

OFCondition IODMultiFrameFGModule::setInstanceNumber(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_InstanceNumber, value);
    return result;
}

OFCondition IODMultiFrameFGModule::setContentDate(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ContentDate, value);
    return result;
}

OFCondition IODMultiFrameFGModule::setContentTime(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = result = m_Item->putAndInsertOFStringArray(DCM_ContentTime, value);
    return result;
}

OFCondition IODMultiFrameFGModule::setNumberOfFrames(const Uint32 value)
{
    if (value > 2147483647)
    {
        DCMIOD_ERROR("Cannot set Number of Frames to " << value << ": Maximum permitted value is 2147483647");
        return EC_InvalidValue;
    }
    OFCondition result;
    OFStringStream oss;
    oss << value;
    OFSTRINGSTREAM_GETSTR(oss, tempstr);
    result = m_Item->putAndInsertOFStringArray(DCM_NumberOfFrames, tempstr);
    OFSTRINGSTREAM_FREESTR(tmpstr);
    return result;
}

OFCondition IODMultiFrameFGModule::setRepresentativeFrameNumber(const Uint16 value)
{
    return m_Item->putAndInsertUint16(DCM_RepresentativeFrameNumber, value);
}

IODMultiFrameFGModule::ConcatenationInfo::ConcatenationInfo(OFshared_ptr<DcmItem> data, OFshared_ptr<IODRules> rules)
    : IODComponent(data, rules)
{

    // reset element rules
    resetRules();
}

IODMultiFrameFGModule::ConcatenationInfo::ConcatenationInfo(IODComponent* parent)
    : IODComponent(parent)
{
    // reset element rules
    resetRules();
}

IODMultiFrameFGModule::ConcatenationInfo::~ConcatenationInfo()
{
    // Nothing to do
}

OFString IODMultiFrameFGModule::ConcatenationInfo::getName() const
{
    return m_ModuleName;
}

void IODMultiFrameFGModule::ConcatenationInfo::resetRules()
{
    // Concatenation attributes
    m_Rules->addRule(new IODRule(DCM_ConcatenationFrameOffsetNumber, "1", "1C", m_ModuleName, DcmIODTypes::IE_INSTANCE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_ConcatenationUID, "1", "1C", m_ModuleName, DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(
        new IODRule(DCM_SOPInstanceUIDOfConcatenationSource, "1", "1C", m_ModuleName, DcmIODTypes::IE_INSTANCE),
        OFTrue);
    m_Rules->addRule(new IODRule(DCM_InConcatenationNumber, "1", "1C", m_ModuleName, DcmIODTypes::IE_INSTANCE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_InConcatenationTotalNumber, "1", "3", m_ModuleName, DcmIODTypes::IE_INSTANCE),
                     OFTrue);
}

OFCondition IODMultiFrameFGModule::ConcatenationInfo::getConcatenationFrameOffsetNumber(Uint32& value,
                                                                                        const unsigned int pos)
{
    return m_Item->findAndGetUint32(DCM_ConcatenationFrameOffsetNumber, value, pos);
}

OFCondition IODMultiFrameFGModule::ConcatenationInfo::getConcatenationUID(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ConcatenationUID, *m_Item, value, pos);
}

OFCondition IODMultiFrameFGModule::ConcatenationInfo::getSOPInstanceUIDOfConcatenationSource(OFString& value,
                                                                                             const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_SOPInstanceUIDOfConcatenationSource, *m_Item, value, pos);
}

OFCondition IODMultiFrameFGModule::ConcatenationInfo::getInConcatenationNumber(Uint16& value, const unsigned int pos)
{
    return m_Item->findAndGetUint16(DCM_InConcatenationNumber, value, pos);
}

OFCondition IODMultiFrameFGModule::ConcatenationInfo::getInConcatenationTotalNumber(Uint16& value,
                                                                                    const unsigned int pos)
{
    return m_Item->findAndGetUint16(DCM_InConcatenationTotalNumber, value, pos);
}

OFCondition IODMultiFrameFGModule::ConcatenationInfo::setConcatenationFrameOffsetNumber(const Uint32 value)
{
    return m_Item->putAndInsertUint32(DCM_ConcatenationFrameOffsetNumber, value);
}

OFCondition IODMultiFrameFGModule::ConcatenationInfo::setConcatenationUID(const OFString& value,
                                                                          const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ConcatenationUID, value);
    return result;
}

OFCondition IODMultiFrameFGModule::ConcatenationInfo::setSOPInstanceUIDOfConcatenationSource(const OFString& value,
                                                                                             const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_SOPInstanceUIDOfConcatenationSource, value);
    return result;
}

OFCondition IODMultiFrameFGModule::ConcatenationInfo::setInConcatenationNumber(const Uint16 value)
{
    return m_Item->putAndInsertUint16(DCM_InConcatenationNumber, value);
}

OFCondition IODMultiFrameFGModule::ConcatenationInfo::setInConcatenationTotalNumber(const Uint16 value)
{
    return m_Item->putAndInsertUint16(DCM_InConcatenationTotalNumber, value);
}
