/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
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
 *  Purpose: Class for managing the Image Pixel Module
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modimagepixel.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmiod/iodutil.h"

const OFString IODImagePixelModule::m_ModuleName = "ImagePixelModule";


IODImagePixelModule::IODImagePixelModule(OFshared_ptr<DcmItem> item,
                                         OFshared_ptr<IODRules> rules)
: IODModule(item, rules)
{
  // reset element rules
  resetRules();
}


OFString IODImagePixelModule::getName() const
{
  return m_ModuleName;
}


IODImagePixelModule::IODImagePixelModule()
: IODModule()
{
  resetRules();
}


void IODImagePixelModule::resetRules()
{
  // parameters are tag, VM, type. Overwrite old rules if any.
  m_Rules->addRule(new IODRule(DCM_SamplesPerPixel, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_PhotometricInterpretation, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_Rows, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_Columns, "1", "1n", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_BitsAllocated, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_BitsStored, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_HighBit, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_PixelRepresentation, "1", "1", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_PlanarConfiguration, "1", "1C", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_PixelAspectRatio, "2", "1C", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_ICCProfile, "1", "3", m_ModuleName, DcmIODTypes::IE_IMAGE), OFTrue);
}


IODImagePixelModule::~IODImagePixelModule()
{
}


OFCondition IODImagePixelModule::getSamplesPerPixel(Uint16 &value,
                                         const signed long pos)
{
  return m_Item->findAndGetUint16(DCM_SamplesPerPixel, value, pos);
}


OFCondition IODImagePixelModule::getPhotometricInterpretation(OFString&value,
                                                      const signed long pos)
{
  return DcmIODUtil::getStringValueFromItem(DCM_PhotometricInterpretation, *m_Item, value, pos);
}


OFCondition IODImagePixelModule::getRows(Uint16& value,
                                 const signed long pos)
{
  return m_Item->findAndGetUint16(DCM_Rows, value, pos);
}


OFCondition IODImagePixelModule::getColumns(Uint16& value,
                                    const signed long pos)
{
  return m_Item->findAndGetUint16(DCM_Columns, value, pos);
}


OFCondition IODImagePixelModule::getBitsAllocated(Uint16& value,
                                          const signed long pos)
{
  return m_Item->findAndGetUint16(DCM_BitsAllocated, value, pos);
}


OFCondition IODImagePixelModule::getBitsStored(Uint16& value,
                                       const signed long pos)
{
  return m_Item->findAndGetUint16(DCM_BitsStored, value, pos);
}


OFCondition IODImagePixelModule::getHighBit(Uint16& value,
                                    const signed long pos)
{
  return m_Item->findAndGetUint16(DCM_HighBit, value, pos);
}


OFCondition IODImagePixelModule::getPixelRepresentation(Uint16& value,
                                                  const signed long pos)
{
  return m_Item->findAndGetUint16(DCM_PixelRepresentation, value, pos);
}


OFCondition IODImagePixelModule::getPlanarConfiguration(Uint16& value,
                                                const signed long pos)
{
  return m_Item->findAndGetUint16(DCM_PlanarConfiguration, value, pos);
}


OFCondition IODImagePixelModule::getPixelAspectRatio(Uint16& value,
                                            const signed long pos)
{
  return m_Item->findAndGetUint16(DCM_PixelAspectRatio, value, pos);
}


OFCondition IODImagePixelModule::getICCProfile(OFVector<Uint8>& values)
{
  DcmElement* elem = NULL;
  OFCondition result = m_Item->findAndGetElement(DCM_ICCProfile, elem);
  if (result.good())
  {
    DcmOtherByteOtherWord* ob = OFstatic_cast(DcmOtherByteOtherWord*, elem);
    if (ob)
      return DcmIODUtil::copyFromUint8Array<OFVector<Uint8> > (ob, values);
    else
      return EC_InternalError;
  }
  else
    return EC_TagNotFound;
}


OFCondition IODImagePixelModule::setSamplesPerPixel(const Uint16 value,
                                                    const OFBool checkValue)
{
  OFCondition result = EC_Normal;

  // only values 1, 3 and 4 (retired) are permitted
  if (checkValue)
  {
    if ( (value == 0) || (value == 2) || (value > 4) )
    {
      DCMIOD_ERROR("Value " << value << " not permitted for attribute Samples Per Pixel");
      result = EC_InvalidValue;
    }
    else if (value == 4)
    {
      DCMIOD_WARN("Value " << value << " is retired for attribute Samples Per Pixel");
    }
  }

  if (result.good() )
    result = m_Item->putAndInsertUint16(DCM_SamplesPerPixel, value);

  // TODO: Check consistency with photometric interpretation
  return result;
}


OFCondition IODImagePixelModule::setPhotometricInterpretation(const OFString& value,
                                                              const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_PhotometricInterpretation, value);
  return result;
}


OFCondition IODImagePixelModule::setRows(const Uint16 value,
                                         const OFBool checkValue)
{
  (void)checkValue;
  return m_Item->putAndInsertUint16(DCM_Rows, value);
}


OFCondition IODImagePixelModule::setColumns(const Uint16 value,
                                            const OFBool checkValue)
{
  (void)checkValue;
  return m_Item->putAndInsertUint16(DCM_Columns, value);
}


OFCondition IODImagePixelModule::setBitsAllocated(const Uint16 value,
                                                  const OFBool checkValue)
{
  (void)checkValue;
  return m_Item->putAndInsertUint16(DCM_BitsAllocated, value);
}


OFCondition IODImagePixelModule::setBitsStored(const Uint16 value,
                                               const OFBool checkValue)
{
  (void)checkValue;
  return m_Item->putAndInsertUint16(DCM_BitsStored, value);
}


OFCondition IODImagePixelModule::setHighBit(const Uint16 value,
                                            const OFBool checkValue)
{
  (void)checkValue;
  return m_Item->putAndInsertUint16(DCM_HighBit, value);
}


OFCondition IODImagePixelModule::setPixelRepresentation(const Uint16 value,
                                                        const OFBool checkValue)
{
  OFCondition result = EC_Normal;
  if (checkValue)
  {
    if ( (value != 0) && (value != 1) )
    {
      DCMIOD_ERROR("Value " << value << " not permitted for attribute Pixel Representation");
      result = EC_InvalidValue;
    }
  }
  if ( result.good() )
  {
   result = m_Item->putAndInsertUint16(DCM_PixelRepresentation, value);
  }
  return result;
}


OFCondition IODImagePixelModule::setPlanarConfiguration(const Uint16 value,
                                                        const OFBool checkValue)
{
  OFCondition result = EC_Normal;
  if (checkValue)
  {
    if ( (value != 0) && (value != 1) )
    {
      DCMIOD_ERROR("Value " << value << " not permitted for attribute Planar Configuration");
      result = EC_InvalidValue;
    }
  }
  if ( result.good() )
  {
    result = m_Item->putAndInsertUint16(DCM_PlanarConfiguration, value);
  }
  return result;
}


OFCondition IODImagePixelModule::setPixelAspectRatio(const OFString& verticalPixelSize,
                                                     const OFString& horizontalPixelSize,
                                                     const OFBool checkValue)
{
  OFString concat = verticalPixelSize;
  concat += "\\"; concat += horizontalPixelSize;
  OFCondition cond;
  if (checkValue)
  {
    cond = DcmIntegerString::checkStringValue(concat, "2");
    // check for unsignedness, too?
  }
  if (cond.good()) m_Item->putAndInsertOFStringArray(DCM_PixelAspectRatio, concat);
  return cond;
}


OFCondition IODImagePixelModule::setICCProfile(const Uint8* values,
                                               const size_t length)
{
  return m_Item->putAndInsertUint8Array(DCM_ICCProfile, values, length);
}
