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
 *  Purpose: Class for managing the Enhanced US Image Module
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modenhusimage.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcvrds.h"

const OFString IODEnhUSImageModule::m_ModuleName = "EnhancedUSImageModule";

IODEnhUSImageModule::IODEnhUSImageModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODModule(item, rules)
    , m_MandatoryViewAndSliceProgressionDirection()
    , m_Anatomy("1" /* mandatory version */)
    , m_TransducerScanPattern()
    , m_TransducerGeometry()
    , m_TransducerBeamSteering()
    , m_TransducerApplication()
{
    // reset element rules
    resetRules();
}

IODEnhUSImageModule::IODEnhUSImageModule()
    : IODModule()
    , m_MandatoryViewAndSliceProgressionDirection()
    , m_Anatomy("1" /* mandatory version */)
    , m_TransducerScanPattern()
    , m_TransducerGeometry()
    , m_TransducerBeamSteering()
    , m_TransducerApplication()
{
    // reset element rules
    resetRules();
}

OFString IODEnhUSImageModule::getName() const
{
    return m_ModuleName;
}

void IODEnhUSImageModule::resetRules()
{
    // parameters are tag, VM, type. Overwrite old rules if any.
    m_Rules->addRule(new IODRule(DCM_ImageType, "4", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_SamplesPerPixel, "1", "1", getName(), DcmIODTypes::IE_IMAGE, "1"), OFTrue);
    m_Rules->addRule(
        new IODRule(DCM_PhotometricInterpretation, "1", "1", getName(), DcmIODTypes::IE_IMAGE, "MONOCHROME2"), OFTrue);
    m_Rules->addRule(new IODRule(DCM_Rows, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_Columns, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_BitsAllocated, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_BitsStored, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_HighBit, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PixelRepresentation, "1", "1", getName(), DcmIODTypes::IE_IMAGE, "0"), OFTrue);
    m_Rules->addRule(new IODRule(DCM_DimensionOrganizationType, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_AcquisitionDateTime, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_AcquisitionDuration, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PixelSpacing, "2", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_PositionMeasuringDeviceUsed, "1", "1C", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_LossyImageCompression, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_LossyImageCompressionRatio, "1-n", "1C", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_LossyImageCompressionMethod, "1-n", "1C", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_PresentationLUTShape, "1", "1", getName(), DcmIODTypes::IE_IMAGE, "IDENTITY"),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_RescaleSlope, "1", "1", getName(), DcmIODTypes::IE_IMAGE, "1"), OFTrue);
    m_Rules->addRule(new IODRule(DCM_RescaleIntercept, "1", "1", getName(), DcmIODTypes::IE_IMAGE, "0"), OFTrue);
    m_Rules->addRule(new IODRule(DCM_BurnedInAnnotation, "1", "1", getName(), DcmIODTypes::IE_IMAGE, "NO"), OFTrue);
    m_Rules->addRule(new IODRule(DCM_RecognizableVisualFeatures, "1", "3", getName(), DcmIODTypes::IE_IMAGE, "NO"),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_TransducerScanPatternCodeSequence, "1", "1", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_TransducerGeometryCodeSequence, "1", "1", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_TransducerBeamSteeringCodeSequence, "1", "1", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_TransducerApplicationCodeSequence, "1", "1", getName(), DcmIODTypes::IE_IMAGE),
                     OFTrue);
    m_Rules->addRule(new IODRule(DCM_ProcessingFunction, "1", "3", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_MechanicalIndex, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_BoneThermalIndex, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_CranialThermalIndex, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_SoftTissueThermalIndex, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_DepthsOfFocus, "1-n", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
    m_Rules->addRule(new IODRule(DCM_DepthOfScanField, "1", "1", getName(), DcmIODTypes::IE_IMAGE), OFTrue);
}

IODEnhUSImageModule::~IODEnhUSImageModule()
{
    DcmIODUtil::freeContainer(m_TransducerBeamSteering);
}

OFCondition IODEnhUSImageModule::read(DcmItem& source, const OFBool clearOldData)
{
    if (clearOldData)
    {
        DcmIODUtil::freeContainer(m_TransducerBeamSteering);
    }
    m_MandatoryViewAndSliceProgressionDirection.read(source, clearOldData);
    m_Anatomy.read(source, clearOldData);
    DcmIODUtil::readSingleItem(source,
                               DCM_TransducerApplicationCodeSequence,
                               m_TransducerApplication,
                               m_Rules->getByTag(DCM_TransducerApplicationCodeSequence));
    DcmIODUtil::readSingleItem(source,
                               DCM_TransducerGeometryCodeSequence,
                               m_TransducerGeometry,
                               m_Rules->getByTag(DCM_TransducerGeometryCodeSequence));
    DcmIODUtil::readSingleItem(source,
                               DCM_TransducerScanPatternCodeSequence,
                               m_TransducerScanPattern,
                               m_Rules->getByTag(DCM_TransducerScanPatternCodeSequence));
    DcmIODUtil::readSubSequence(source,
                                DCM_TransducerBeamSteeringCodeSequence,
                                m_TransducerBeamSteering,
                                m_Rules->getByTag(DCM_TransducerBeamSteeringCodeSequence));
    IODComponent::read(source, clearOldData);
    return EC_Normal;
}

OFCondition IODEnhUSImageModule::write(DcmItem& destination)
{
    OFCondition result;

    // Copy sequences to source (which then will be later copied to destination by IODComponent::write()
    result = m_MandatoryViewAndSliceProgressionDirection.write(destination);
    if (result.good())
        result = m_Anatomy.write(destination);
    DcmIODUtil::writeSingleItem(result,
                                DCM_TransducerScanPatternCodeSequence,
                                m_TransducerScanPattern,
                                *m_Item,
                                m_Rules->getByTag(DCM_TransducerScanPatternCodeSequence));
    DcmIODUtil::writeSingleItem(result,
                                DCM_TransducerGeometryCodeSequence,
                                m_TransducerGeometry,
                                *m_Item,
                                m_Rules->getByTag(DCM_TransducerGeometryCodeSequence));
    DcmIODUtil::writeSingleItem(result,
                                DCM_TransducerApplicationCodeSequence,
                                m_TransducerApplication,
                                *m_Item,
                                m_Rules->getByTag(DCM_TransducerApplicationCodeSequence));
    DcmIODUtil::writeSubSequence(result,
                                 DCM_TransducerBeamSteeringCodeSequence,
                                 m_TransducerBeamSteering,
                                 *m_Item,
                                 m_Rules->getByTag(DCM_TransducerBeamSteeringCodeSequence));
    if (result.good())
        result = IODComponent::write(destination);
    return result;
}

// --- get attributes (C++ string) ---

OFCondition IODEnhUSImageModule::getImageType(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ImageType, *m_Item, value, pos);
}

OFCondition IODEnhUSImageModule::getSamplesPerPixel(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_SamplesPerPixel, value, pos);
}

OFCondition IODEnhUSImageModule::getPhotometricInterpretation(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_PhotometricInterpretation, *m_Item, value, pos);
}

OFCondition IODEnhUSImageModule::getRows(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_Rows, value, pos);
}

OFCondition IODEnhUSImageModule::getColumns(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_Columns, value, pos);
}

OFCondition IODEnhUSImageModule::getBitsAllocated(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_BitsAllocated, value, pos);
}

OFCondition IODEnhUSImageModule::getBitsStored(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_BitsStored, value, pos);
}

OFCondition IODEnhUSImageModule::getHighBit(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_HighBit, value, pos);
}

OFCondition IODEnhUSImageModule::getPixelRepresentation(Uint16& value, const unsigned long pos)
{
    return m_Item->findAndGetUint16(DCM_PixelRepresentation, value, pos);
}

OFCondition IODEnhUSImageModule::getDimensionOrganizationType(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_DimensionOrganizationType, *m_Item, value, pos);
}

OFCondition IODEnhUSImageModule::getAcquisitionDateTime(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_AcquisitionDateTime, *m_Item, value, pos);
}

OFCondition IODEnhUSImageModule::getAcquisitionDuration(Float64& value, const unsigned long pos)
{
    return m_Item->findAndGetFloat64(DCM_AcquisitionDuration, value, pos);
}

OFCondition IODEnhUSImageModule::getPixelSpacing(Float64& value, const unsigned long pos)
{
    return m_Item->findAndGetFloat64(DCM_PixelSpacing, value, pos);
}

OFCondition IODEnhUSImageModule::getPositionMeasuringDevice(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_PositionMeasuringDeviceUsed, *m_Item, value, pos);
}

OFCondition IODEnhUSImageModule::getLossyImageCompression(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_LossyImageCompression, *m_Item, value, pos);
}

OFCondition IODEnhUSImageModule::getLossyImageCompressionRatio(Float64& value, const unsigned long pos)
{
    return m_Item->findAndGetFloat64(DCM_LossyImageCompressionRatio, value, pos);
}

OFCondition IODEnhUSImageModule::getLossyImageCompressionMethod(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_LossyImageCompressionMethod, *m_Item, value, pos);
}

OFCondition IODEnhUSImageModule::getPresentationLUTShape(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_PresentationLUTShape, *m_Item, value, pos);
}

OFCondition IODEnhUSImageModule::getRescaleSlope(Float64& value, const unsigned long pos)
{
    return m_Item->findAndGetFloat64(DCM_RescaleSlope, value, pos);
}

OFCondition IODEnhUSImageModule::getRescaleIntercept(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_RescaleSlope, *m_Item, value, pos);
}

GeneralAnatomyMacro& IODEnhUSImageModule::getAnatomy()
{
    return m_Anatomy;
}

MandatoryViewAndSliceProgressionDirectionMacro& IODEnhUSImageModule::getMandatoryViewAndSliceProgressionDirection()
{
    return m_MandatoryViewAndSliceProgressionDirection;
}

OFCondition IODEnhUSImageModule::getBurnedInAnnotation(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_BurnedInAnnotation, *m_Item, value, pos);
}

OFCondition IODEnhUSImageModule::getRecognizableVisibleFeatures(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_RecognizableVisualFeatures, *m_Item, value, pos);
}

CodeSequenceMacro& IODEnhUSImageModule::getTransducerScanPattern()
{
    return m_TransducerScanPattern;
}

CodeSequenceMacro& IODEnhUSImageModule::getTransducerGeometry()
{
    return m_TransducerGeometry;
}

OFVector<CodeSequenceMacro*>& IODEnhUSImageModule::getTransducerBeamSteering()
{
    return m_TransducerBeamSteering;
}

CodeSequenceMacro& IODEnhUSImageModule::getTransducerApplication()
{
    return m_TransducerApplication;
}

OFCondition IODEnhUSImageModule::getProcessingFunction(OFString& value, const signed long pos)
{
    return DcmIODUtil::getStringValueFromItem(DCM_ProcessingFunction, *m_Item, value, pos);
}

OFCondition IODEnhUSImageModule::getMechanicalIndex(Float64& value, const unsigned long pos)
{
    return m_Item->findAndGetFloat64(DCM_MechanicalIndex, value, pos);
}

OFCondition IODEnhUSImageModule::getBoneThermalIndex(Float64& value, const unsigned long pos)
{
    return m_Item->findAndGetFloat64(DCM_BoneThermalIndex, value, pos);
}

OFCondition IODEnhUSImageModule::getCranialThermalIndex(Float64& value, const unsigned long pos)
{
    return m_Item->findAndGetFloat64(DCM_CranialThermalIndex, value, pos);
}

OFCondition IODEnhUSImageModule::getSoftTissueThermalIndex(Float64& value, const unsigned long pos)
{
    return m_Item->findAndGetFloat64(DCM_SoftTissueThermalIndex, value, pos);
}

OFCondition IODEnhUSImageModule::getDepthsOfFocus(Float64& value, const unsigned long pos)
{
    return m_Item->findAndGetFloat64(DCM_DepthsOfFocus, value, pos);
}

OFCondition IODEnhUSImageModule::getDepthsOfScanField(Sint32& value, const unsigned long pos)
{
    return m_Item->findAndGetSint32(DCM_DepthOfScanField, value, pos);
}

// --- set attributes ---

OFCondition IODEnhUSImageModule::setImageType(const DcmIODTypes::IOD_ENHUSIMAGETYPE pixelDataChar,
                                              const OFString& imageFlavor,
                                              const OFString& derivedPixelContrast,
                                              const OFBool checkValue)
{
    OFString value;
    switch (pixelDataChar)
    {
        case (DcmIODTypes::IMAGETYPE_ORIGINAL):
            value = "ORIGINAL\\PRIMARY\\";
            break;
        case (DcmIODTypes::IMAGETYPE_DERIVED):
            value = "DERIVED\\PRIMARY\\";
            break;
        default:
            return IOD_EC_InvalidElementValue;
    }
    value += imageFlavor;
    value += "\\";
    value += derivedPixelContrast;

    OFCondition result;
    if (checkValue)
    {
        result = DcmCodeString::checkStringValue(value, "2");
    }
    if (result.good())
    {
        result = m_Item->putAndInsertOFStringArray(DCM_ImageType, value);
    }
    return result;
}

OFCondition IODEnhUSImageModule::setRows(const Uint16 value, const OFBool checkValue)
{
    if (checkValue && (value == 0))
        return IOD_EC_InvalidElementValue;

    return m_Item->putAndInsertUint16(DCM_Rows, value);
}

OFCondition IODEnhUSImageModule::setColumns(const Uint16 value, const OFBool checkValue)
{
    if (checkValue && (value == 0))
        return IOD_EC_InvalidElementValue;

    return m_Item->putAndInsertUint16(DCM_Columns, value);
}

OFCondition IODEnhUSImageModule::setBitsAllocated(const Uint16 value, const OFBool checkValue)
{
    if (checkValue && (value != 8) && (value != 16))
        return IOD_EC_InvalidElementValue;

    return m_Item->putAndInsertUint16(DCM_BitsAllocated, value);
}

OFCondition IODEnhUSImageModule::setBitsStored(const Uint16 value, const OFBool checkValue)
{
    if (checkValue && (value != 8) && (value != 16))
        return IOD_EC_InvalidElementValue;

    return m_Item->putAndInsertUint16(DCM_BitsStored, value);
}

OFCondition IODEnhUSImageModule::setHighBit(const Uint16 value, const OFBool checkValue)
{
    if (checkValue && (value != 7) && (value != 15))
        return IOD_EC_InvalidElementValue;

    return m_Item->putAndInsertUint16(DCM_HighBit, value);
}

OFCondition IODEnhUSImageModule::setDimensionOrganizationType(const OFString& value, const OFBool checkValue)
{
    OFCondition result;
    if (checkValue)
    {
        if ((value != "3D") && (value != "3D_TEMPORAL"))
            return IOD_EC_InvalidElementValue;
    }
    return m_Item->putAndInsertOFStringArray(DCM_DimensionOrganizationType, value);
}

OFCondition IODEnhUSImageModule::setAcquisitionDateTime(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDateTime::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_AcquisitionDateTime, value);
    return result;
}

OFCondition IODEnhUSImageModule::setAcquisitionDuration(const Float64 value, const OFBool)
{
    return m_Item->putAndInsertFloat64(DCM_AcquisitionDuration, value);
}

OFCondition IODEnhUSImageModule::setPixelSpacing(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "2") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PixelSpacing, value);
    return result;
}

OFCondition IODEnhUSImageModule::setPositionMeasuringDevice(const OFString& value, const OFBool checkValue)
{
    OFCondition result;
    if (checkValue)
    {
        result = DcmCodeString::checkStringValue(value, "1");
        if (result.good())
        {
            if ((value != "RIGID") && (value != "FREEHAND"))
                result = IOD_EC_InvalidElementValue;
        }
    }
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_PositionMeasuringDeviceUsed, value);

    return result;
}

OFCondition IODEnhUSImageModule::setLossyImageCompression(const OFString& value, const OFBool checkValue)
{
    if (checkValue)
    {
        if ((value != "00") && (value != "01"))
            return IOD_EC_InvalidElementValue;
    }

    return m_Item->putAndInsertOFStringArray(DCM_LossyImageCompression, value);
}

OFCondition IODEnhUSImageModule::setLossyImageCompressionMethod(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_LossyImageCompressionMethod, value);
    return result;
}

OFCondition IODEnhUSImageModule::setLossyImageCompressionRatio(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_LossyImageCompressionRatio, value);
    return result;
}

OFCondition IODEnhUSImageModule::setRecognizableVisibleFeatures(const OFString& value, const OFBool checkValue)
{
    if (checkValue)
    {
        if ((value != "NO") && (value != "YES"))
            return IOD_EC_InvalidElementValue;
    }
    return m_Item->putAndInsertOFStringArray(DCM_RecognizableVisualFeatures, value);
}

OFCondition IODEnhUSImageModule::setProcessingFunction(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_ProcessingFunction, value);
    return result;
}

OFCondition IODEnhUSImageModule::setMechanicalIndex(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_MechanicalIndex, value);
    return result;
}

OFCondition IODEnhUSImageModule::setBoneThermalIndex(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_BoneThermalIndex, value);
    return result;
}

OFCondition IODEnhUSImageModule::setCranialThermalIndex(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_CranialThermalIndex, value);
    return result;
}

OFCondition IODEnhUSImageModule::setSoftTissueThermalIndex(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_SoftTissueThermalIndex, value);
    return result;
}

OFCondition
IODEnhUSImageModule::setDepthsOfFocus(const Float64 value, const long unsigned int pos, const OFBool checkValue)
{
    (void)checkValue;
    return m_Item->putAndInsertFloat64(DCM_DepthsOfFocus, value, pos);
}

OFCondition IODEnhUSImageModule::setDepthsOfFocus(const OFVector<Float64>& values, const OFBool checkValue)
{
    (void)checkValue;
    DcmElement* elem = DcmItem::newDicomElement(DCM_DepthsOfFocus);
    if (!elem)
        return EC_MemoryExhausted;

    OFCondition result;
    OFVector<Float64>::const_iterator it = values.begin();
    while ((it != values.end()) && result.good())
    {
        result = elem->putFloat64(*it);
        it++;
    }
    if (result.bad())
    {
        delete elem;
    }
    else
    {
        result = m_Item->insert(elem);
    }
    return result;
}

OFCondition IODEnhUSImageModule::setDepthsOfScanField(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmIntegerString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_Item->putAndInsertOFStringArray(DCM_DepthOfScanField, value);
    return result;
}
