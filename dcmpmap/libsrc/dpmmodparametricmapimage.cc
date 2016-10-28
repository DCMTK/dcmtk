/*
 *
 *  Copyright (C) 2016, Open Connections GmbH
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
 *  Purpose: Class for managing the Parametric Map Image Module
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmpmap/dpmmodparametricmapimage.h"

const OFString DPMParametricMapImageModule::m_ModuleName = "DPMParametricMapImageModule";

DPMParametricMapImageModule::DPMParametricMapImageModule()
: IODModule()
{
  resetRules();
}

DPMParametricMapImageModule::DPMParametricMapImageModule(OFshared_ptr<DcmItem> item,
                                                         OFshared_ptr<IODRules> rules)
: IODModule(item, rules)
{
  // reset element rules
  resetRules();
}

DPMParametricMapImageModule::~DPMParametricMapImageModule()
{

}

OFString DPMParametricMapImageModule::getName() const
{
  return m_ModuleName;
}

void DPMParametricMapImageModule::resetRules()
{
  // parameters are tag, VM, type. Overwrite old rules if any.
  m_Rules->addRule(new IODRule(DCM_ImageType, "2-n","1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_SamplesPerPixel, "1","1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_PhotometricInterpretation, "1","1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_BitsAllocated, "1","1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_BitsStored, "1","1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_HighBit, "1","1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_PresentationLUTShape, "1","1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_LossyImageCompression, "1","1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_LossyImageCompressionRatio, "1-n","1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_LossyImageCompressionMethod, "1-n","1C", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_BurnedInAnnotation, "1","1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_RecognizableVisualFeatures, "1","1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
  m_Rules->addRule(new IODRule(DCM_ContentQualification, "1","1", getName(), DcmIODTypes::IE_SERIES), OFTrue);
}

OFCondition DPMParametricMapImageModule::read(DcmItem& source,
                                              const OFBool clearOldData)
{
  if (clearOldData)
    clearData();

  IODComponent::read(source, OFFalse /* data already cleared */);

  return EC_Normal;
}

OFCondition DPMParametricMapImageModule::write(DcmItem& destination)
{
  OFCondition result = EC_Normal;

  result = IODComponent::write(destination);

  return result;
}

OFCondition DPMParametricMapImageModule::getImageType(OFString &value,
                                                      const signed long pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_ImageType, *m_Item, value, pos);
}

OFCondition DPMParametricMapImageModule::getSamplesPerPixel(Uint16 &value,
                                                            const signed long pos) const
{
  return m_Item->findAndGetUint16(DCM_SamplesPerPixel, value, pos);
}

OFCondition DPMParametricMapImageModule::getPhotometricInterpretation(OFString &value,
                                                                      const signed long pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_PhotometricInterpretation, *m_Item, value, pos);
}

OFCondition DPMParametricMapImageModule::getBitsAllocated(Uint16 &value,
                                                          const signed long pos) const
{
  return m_Item->findAndGetUint16(DCM_BitsAllocated, value, pos);
}

OFCondition DPMParametricMapImageModule::getBitsStored(Uint16 &value,
                                                       const signed long pos) const
{
  return m_Item->findAndGetUint16(DCM_BitsStored, value, pos);
}

OFCondition DPMParametricMapImageModule::getHighBit(Uint16 &value,
                                                    const signed long pos) const
{
  return m_Item->findAndGetUint16(DCM_HighBit, value, pos);
}

OFCondition DPMParametricMapImageModule::getPresentationLUTShape(OFString &value,
                                                                 const signed long pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_PresentationLUTShape, *m_Item, value, pos);
}

OFCondition DPMParametricMapImageModule::getLossyImageCompression(OFString &value,
                                                                  const signed long pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_LossyImageCompression, *m_Item, value, pos);
}

OFCondition DPMParametricMapImageModule::getLossyImageCompressionRatio(OFString &value,
                                                                       const signed long pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_LossyImageCompressionRatio, *m_Item, value, pos);
}

OFCondition DPMParametricMapImageModule::getLossyImageCompressionMethod(OFString &value,
                                                                        const signed long pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_LossyImageCompressionMethod, *m_Item, value, pos);
}

OFCondition DPMParametricMapImageModule::getBurnedInAnnotation(OFString &value,
                                                               const signed long pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_BurnedInAnnotation, *m_Item, value, pos);
}

OFCondition DPMParametricMapImageModule::getRecognizableVisualFeatures(OFString &value,
                                                                       const signed long pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_RecognizableVisualFeatures, *m_Item, value, pos);
}

OFCondition DPMParametricMapImageModule::getContentQualification(OFString &value,
                                                                 const signed long pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_ContentQualification, *m_Item, value, pos);
}

OFCondition DPMParametricMapImageModule::setImageType(const OFString &value,
                                                      const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "2-n") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_ImageType, value);
  return result;
}

OFCondition DPMParametricMapImageModule::setSamplesPerPixel(const Uint16 &value,
                                                            const OFBool checkValue)
{
  if (checkValue && (value != 1))
    return IOD_EC_InvalidElementValue;

  return m_Item->putAndInsertUint16(DCM_SamplesPerPixel, value);
}

OFCondition DPMParametricMapImageModule::setPhotometricInterpretation(const OFString &value,
                                                                      const OFBool checkValue)
{
  if (checkValue && (value != "MONOCHROME2"))
    return IOD_EC_InvalidElementValue;

  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_PhotometricInterpretation, value);
  return result;
}

OFCondition DPMParametricMapImageModule::setBitsAllocated(const Uint16 &value,
                                                          const OFBool checkValue)
{
  if (checkValue && (value != 16) && (value != 32) && (value != 64))
    return IOD_EC_InvalidElementValue;

  return m_Item->putAndInsertUint16(DCM_BitsAllocated, value);
}

OFCondition DPMParametricMapImageModule::setBitsStored(const Uint16 &value,
                                                       const OFBool checkValue)
{
  if (checkValue && (value != 16) && (value != 32) && (value != 64))
    return IOD_EC_InvalidElementValue;

  return m_Item->putAndInsertUint16(DCM_BitsStored, value);
}

OFCondition DPMParametricMapImageModule::setHighBit(const Uint16 &value,
                                                    const OFBool checkValue)
{
  if (checkValue && (value != 15))
    return IOD_EC_InvalidElementValue;

  return m_Item->putAndInsertUint16(DCM_HighBit, value);
}

OFCondition DPMParametricMapImageModule::setPresentationLUTShape(const OFString &value,
                                                                 const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_PresentationLUTShape, value);
  return result;
}

OFCondition DPMParametricMapImageModule::setLossyImageCompression(const OFString &value,
                                                                  const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_LossyImageCompression, value);
  return result;
}

OFCondition DPMParametricMapImageModule::setLossyImageCompressionRatio(const OFString &value,
                                                                       const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1-n") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_LossyImageCompressionRatio, value);
  return result;
}

OFCondition DPMParametricMapImageModule::setLossyImageCompressionMethod(const OFString &value,
                                                                        const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1-n") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_LossyImageCompressionMethod, value);
  return result;
}

OFCondition DPMParametricMapImageModule::setBurnedInAnnotation(const OFString &value,
                                                               const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_BurnedInAnnotation, value);
  return result;
}

OFCondition DPMParametricMapImageModule::setRecognizableVisualFeatures(const OFString &value,
                                                                       const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_RecognizableVisualFeatures, value);
  return result;
}

OFCondition DPMParametricMapImageModule::setContentQualification(const OFString &value,
                                                                 const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_ContentQualification, value);
  return result;
}
