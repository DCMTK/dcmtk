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
 *  Purpose: Class for managing the Ultrasound Frame of Reference Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modusfor.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmiod/iodutil.h"

const OFString IODUSFoRModule::m_ModuleName = "UltrasoundFrameOfReferenceModule";

IODUSFoRModule::IODUSFoRModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
{
    // reset element rules
    resetRules();
}

IODUSFoRModule::IODUSFoRModule()
    : IODModule()
{
    resetRules();
}

void IODUSFoRModule::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_VolumeFrameOfReferenceUID, "1", "1", getName(), DcmIODTypes::IE_FOR), OFTrue);
    m_Rules->addRule(new IODRule(DCM_UltrasoundAcquisitionGeometry, "1", "1", getName(), DcmIODTypes::IE_FOR), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ApexPosition, "3", "1C", getName(), DcmIODTypes::IE_FOR), OFTrue);
    m_Rules->addRule(new IODRule(DCM_VolumeToTransducerRelationship, "1", "1C", getName(), DcmIODTypes::IE_FOR),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_VolumeToTransducerMappingMatrix, "16", "1", getName(), DcmIODTypes::IE_FOR),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_PatientFrameOfReferenceSource, "1", "1C", getName(), DcmIODTypes::IE_FOR), OFTrue);
    m_Rules->addRule(new IODRule(DCM_TableFrameOfReferenceUID, "1", "1C", getName(), DcmIODTypes::IE_FOR), OFTrue);
    m_Rules->addRule(new IODRule(DCM_VolumeToTableMappingMatrix, "16", "1C", getName(), DcmIODTypes::IE_FOR), OFTrue);
}

OFString IODUSFoRModule::getName() const
{
    return m_ModuleName;
}

IODUSFoRModule::~IODUSFoRModule()
{
    // Nothing to do
}

OFCondition IODUSFoRModule::getVolumeFrameOfReferenceUID(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_VolumeFrameOfReferenceUID, *m_Item, value, pos);
}

OFCondition IODUSFoRModule::getUltrasoundAcquisitionGeometry(OFString& value, const long signed int pos) const
{
    return DcmIODUtil::getStringValueFromItem(DCM_UltrasoundAcquisitionGeometry, *m_Item, value, pos);
}

OFCondition IODUSFoRModule::getApexPosition(OFVector<Float64>& value)
{
    return DcmIODUtil::getFloat64ValuesFromItem(DCM_ApexPosition, *m_Item, value);
}

OFCondition IODUSFoRModule::getApexPosition(Float64& value, const unsigned long pos) const
{
    return DcmIODUtil::getFloat64ValueFromItem(DCM_ApexPosition, *m_Item, value, pos);
}

OFCondition IODUSFoRModule::getVolumetoTransducerRelationship(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_UltrasoundAcquisitionGeometry, *m_Item, value, pos);
}

OFCondition IODUSFoRModule::getVolumetoTransducerMappingMatrix(Float64& value, const unsigned long pos) const
{
    return DcmIODUtil::getFloat64ValueFromItem(DCM_VolumeToTransducerMappingMatrix, *m_Item, value, pos);
}

OFCondition IODUSFoRModule::getVolumetoTransducerMappingMatrix(OFVector<Float64>& value)
{
    return DcmIODUtil::getFloat64ValuesFromItem(DCM_VolumeToTransducerMappingMatrix, *m_Item, value);
}

OFCondition IODUSFoRModule::getPatientFrameofReferenceSource(OFString& value, const long signed int pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_PatientFrameOfReferenceSource, *m_Item, value, pos);
}

OFCondition IODUSFoRModule::getTableFrameofReferenceUID(OFString& value, const long signed int pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_TableFrameOfReferenceUID, *m_Item, value, pos);
}

OFCondition IODUSFoRModule::getVolumetoTableMappingMatrix(Float64& value, const unsigned long pos) const
{
    return DcmIODUtil::getFloat64ValueFromItem(DCM_VolumeToTableMappingMatrix, *m_Item, value, pos);
}

OFCondition IODUSFoRModule::getVolumetoTableMappingMatrix(OFVector<Float64>& value)
{
    return DcmIODUtil::getFloat64ValuesFromItem(DCM_VolumeToTableMappingMatrix, *m_Item, value);
}

OFCondition IODUSFoRModule::setVolumeFrameOfReferenceUID(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_VolumeFrameOfReferenceUID, value);
    return result;
}

OFCondition IODUSFoRModule::setUltrasoundAcquisitionGeometry(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_UltrasoundAcquisitionGeometry, value);
    return result;
}

OFCondition
IODUSFoRModule::setApexPosition(const Float64& xValue, const Float64& yValue, const Float64& zValue, const OFBool)
{

    DcmElement* elem = DcmItem::newDicomElement(DCM_ApexPosition);

    OFCondition result = elem->putFloat64(xValue, 0);
    if (result.good())
        result = elem->putFloat64(yValue, 1);
    if (result.good())
        result = elem->putFloat64(zValue, 2);
    m_Item->insert(elem);

    return result;
}

OFCondition IODUSFoRModule::setVolumetoTransducerRelationship(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_VolumeToTransducerRelationship, value);
    return result;
}

OFCondition IODUSFoRModule::setVolumetoTransducerMappingMatrix(const OFVector<Float64>& value, const OFBool checkValue)
{
    size_t vm = value.size();
    if (checkValue && (vm != 16))
    {
        return EC_ValueMultiplicityViolated;
    }

    OFCondition result;

    DcmElement* elem = NULL;
    result           = DcmItem::newDicomElement(elem, DCM_VolumeToTransducerMappingMatrix);
    if (result.good())
    {
        for (size_t n = 0; result.good() && n < vm; n++)
        {
            result = elem->putFloat64(value[n], OFstatic_cast(unsigned long, n));
        }
    }
    if (result.good())
    {
        result = m_Item->insert(elem);
        if (result.bad())
            delete elem;
    }
    return result;
}

OFCondition IODUSFoRModule::setPatientFrameOfReferenceSource(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PatientFrameOfReferenceSource, value);
    return result;
}

OFCondition IODUSFoRModule::setTableFrameofReferenceUID(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_TableFrameOfReferenceUID, value);
    return result;
}

OFCondition IODUSFoRModule::setVolumeToTableMappingMatrix(const OFVector<Float64>& value, const OFBool checkValue)
{
    size_t vm = value.size();
    if (checkValue && (vm != 16))
    {
        return EC_ValueMultiplicityViolated;
    }

    OFCondition result;

    DcmElement* elem = NULL;
    result           = DcmItem::newDicomElement(elem, DCM_VolumeToTableMappingMatrix);
    if (result.good())
    {
        for (size_t n = 0; result.good() && (n < vm); n++)
        {
            result = elem->putFloat64(value[n], OFstatic_cast(unsigned long, n));
        }
    }
    if (result.good())
    {
        result = m_Item->insert(elem);
        if (result.bad())
            delete elem;
    }
    return result;
}
