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
 *  Purpose: Class for managing the General Image Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modgeneralimage.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrui.h"

#include "dcmtk/dcmiod/iodutil.h"

const OFString IODGeneralImageModule::m_ModuleName = "GeneralImageModule";

IODGeneralImageModule::IODGeneralImageModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
{
    // reset element rules
    resetRules();
}

IODGeneralImageModule::IODGeneralImageModule()
    : IODModule()
{
    resetRules();
}

IODGeneralImageModule::~IODGeneralImageModule()
{
}

void IODGeneralImageModule::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_InstanceNumber, "1", "2", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PatientOrientation, "2", "2C", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ContentDate, "1", "2C", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ContentTime, "1", "2C", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ImageType, "2-n", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_AcquisitionNumber, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_AcquisitionDate, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_AcquisitionTime, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_AcquisitionDateTime, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_ImageComments, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_BurnedInAnnotation, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_RecognizableVisualFeatures, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_LossyImageCompression, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_LossyImageCompressionRatio, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_LossyImageCompressionMethod, "1-n", "3", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_PresentationLUTShape, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_IrradiationEventUID, "1-n", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
}

OFString IODGeneralImageModule::getName() const
{
    return "GeneralImageModule";
}

OFCondition IODGeneralImageModule::getInstanceNumber(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_InstanceNumber, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getPatientOrientation(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_PatientOrientation, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getContentDate(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ContentDate, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getContentTime(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ContentTime, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getImageType(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ImageType, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getAcquisitionNumber(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_AcquisitionNumber, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getAcquisitionDate(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_AcquisitionDate, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getAcquisitionTime(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_AcquisitionTime, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getAcquisitionDateTime(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_AcquisitionDateTime, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getImageComments(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ImageComments, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getBurnedInAnnotation(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_BurnedInAnnotation, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getRecognizableVisualFeatures(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_RecognizableVisualFeatures, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getLossyImageCompression(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_LossyImageCompression, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getLossyImageCompressionRatio(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_LossyImageCompressionRatio, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getLossyImageCompressionMethod(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_LossyImageCompressionMethod, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getPresentationLUTShape(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_PresentationLUTShape, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::getIrradiationEventUID(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_IrradiationEventUID, *m_Item, value, pos);
}

OFCondition IODGeneralImageModule::setInstanceNumber(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_InstanceNumber, value);
    return result;
}

OFCondition IODGeneralImageModule::setPatientOrientation(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "2") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PatientOrientation, value);
    return result;
}

OFCondition IODGeneralImageModule::setContentDate(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ContentDate, value);
    return result;
}

OFCondition IODGeneralImageModule::setContentTime(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ContentTime, value);
    return result;
}

OFCondition IODGeneralImageModule::setImageType(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "2-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ImageType, value);
    return result;
}

OFCondition IODGeneralImageModule::setAcquisitionNumber(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_AcquisitionNumber, value);
    return result;
}

OFCondition IODGeneralImageModule::setAcquisitionDate(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_AcquisitionDate, value);
    return result;
}

OFCondition IODGeneralImageModule::setAcquisitionTime(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_AcquisitionTime, value);
    return result;
}

OFCondition IODGeneralImageModule::setAcquisitionDateTime(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_AcquisitionDateTime, value);
    return result;
}

OFCondition IODGeneralImageModule::setImageComments(const OFString& value, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertOFStringArray(DCM_ImageComments, value);
}

OFCondition IODGeneralImageModule::setBurnedInAnnotation(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_BurnedInAnnotation, value);
    return result;
}

OFCondition IODGeneralImageModule::setRecognizableVisualFeatures(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_RecognizableVisualFeatures, value);
    return result;
}

OFCondition IODGeneralImageModule::setLossyImageCompression(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_LossyImageCompression, value);
    return result;
}

OFCondition IODGeneralImageModule::setLossyImageCompressionRatio(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_LossyImageCompressionRatio, value);
    return result;
}

OFCondition IODGeneralImageModule::setLossyImageCompressionMethod(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_LossyImageCompressionMethod, value);
    return result;
}

OFCondition IODGeneralImageModule::setLossyImageCompressionFlag(const OFString& ratios,
                                                                const OFString& methods,
                                                                const OFBool checkValues)
{
    OFCondition result = setLossyImageCompression("01", checkValues);
    if (result.good() || !checkValues)
        result = setLossyImageCompressionMethod(methods, checkValues);
    if (result.good() || !checkValues)
        result = setLossyImageCompressionRatio(ratios, checkValues);

    if (checkValues)
        return result;
    else
        return EC_Normal;
}

OFCondition IODGeneralImageModule::setPresentationLUTShape(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PresentationLUTShape, value);
    return result;
}

OFCondition IODGeneralImageModule::setIrradiationEventUID(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmUniqueIdentifier::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_IrradiationEventUID, value);
    return result;
}
