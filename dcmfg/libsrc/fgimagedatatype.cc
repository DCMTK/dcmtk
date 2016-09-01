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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing Image Data Type Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmfg/fgimagedatatype.h"
#include "dcmtk/dcmfg/fgtypes.h"

// Constructor
FGImageDataType::FGImageDataType() :
  FGBase(DcmFGTypes::EFG_IMAGEDATATYPE),
  m_DataType(DCM_DataType),
  m_AliasedDataType(DCM_AliasedDataType),
  m_ZeroVelocityPixelValueUS(DCM_ZeroVelocityPixelValue),
  m_ZeroVelocityPixelValueSS(DCM_ZeroVelocityPixelValue)
{
}


FGImageDataType::~FGImageDataType()
{
  // nothing to do
}


FGBase* FGImageDataType::clone() const
{
  FGImageDataType* copy = new FGImageDataType();
  if (copy)
  {
    copy->m_DataType = this->m_DataType;
    copy->m_AliasedDataType= this->m_AliasedDataType;
    if (! OFconst_cast(DcmSignedShort*, &this->m_ZeroVelocityPixelValueSS)->isEmpty())
    {
      copy->m_ZeroVelocityPixelValueSS = this->m_ZeroVelocityPixelValueSS;
    }
    else if (! OFconst_cast(DcmUnsignedShort*, &this->m_ZeroVelocityPixelValueUS)->isEmpty())
    {
      copy->m_ZeroVelocityPixelValueUS = this->m_ZeroVelocityPixelValueUS;
    }
    // both emtpy? nothing to do then
  }
  return copy;
}


void FGImageDataType::clearData()
{
  m_DataType.clear();
  m_AliasedDataType.clear();
  m_ZeroVelocityPixelValueSS.clear();
  m_ZeroVelocityPixelValueUS.clear();
}


OFCondition FGImageDataType::check() const
{
   // For now, checks in read() and write() are sufficient
  return EC_Normal;
}


// --- get() functionality ---

OFCondition FGImageDataType::getDataType(OFString& value,
                                         const long signed int pos)
{
  return DcmIODUtil::getStringValueFromElement(m_DataType, value, pos);
}


OFCondition FGImageDataType::getAliasedDataType(OFString& value, const long signed int pos)
{
  return DcmIODUtil::getStringValueFromElement(m_AliasedDataType, value, pos);
}


OFCondition FGImageDataType::getZeroVelocityPixelValue(Sint32& value,
                                                       const long signed int pos)
{
  OFCondition result;
  if (!m_ZeroVelocityPixelValueSS.isEmpty())
  {
    Sint16 val = 0;
    if ( (result = m_ZeroVelocityPixelValueSS.getSint16(val, pos)).good())
    {
      value = val;
    }
  }
  else if (!m_ZeroVelocityPixelValueUS.isEmpty())
  {
    Uint16 val = 0;
    if ( (result = m_ZeroVelocityPixelValueUS.getUint16(val, pos)).good())
    {
      value = val;
    }
  }
  else
    return EC_TagNotFound;

  return EC_Normal;
}


// --- set() functionality ---


OFCondition FGImageDataType::setDataType(const OFString& value,
                                         const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_DataType.putOFStringArray(value);
  return result;
}


OFCondition FGImageDataType::setAliasedDataType(const OFString& value,
                                                const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_AliasedDataType.putOFStringArray(value);
  return result;
}


OFCondition FGImageDataType::setZeroVelocityPixelValueSS(const Sint16 value)
{
  return m_ZeroVelocityPixelValueSS.putSint16(value);
}


OFCondition FGImageDataType::setZeroVelocityPixelValueUS(const Uint16 value)
{
  return m_ZeroVelocityPixelValueUS.putUint16(value);
}


// Read Frame Content Sequence from given item
OFCondition FGImageDataType::read(DcmItem& item)
{
  clearData();

  DcmItem* seqItem = NULL;
  OFCondition result = getItemFromFGSequence(item, DCM_ImageDataTypeSequence, 0, seqItem);
  if (result.bad())
    return result;

  DcmElement* elem = NULL;
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_DataType, "1", "1", "ImageDataTypeMacro");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_AliasedDataType, "1", "1", "ImageDataTypeMacro");
  // We do not know the VR of Zero Velocity Pixel Value (US or SS), so we cannot
  // use the regular getAndCheckElementFromDataset() method that implicitly gets
  // this information via the provided element. Instead, get element first and
  // and then check it in an extra step, afterwards access value depending on
  // the element's VR.
  seqItem->findAndGetElement(DCM_ZeroVelocityPixelValue, elem);
  DcmIODUtil::checkElementValue(elem, DCM_ZeroVelocityPixelValue, "1", "1C", EC_Normal, "ImageDataTypeMacro");
  if (elem)
  {
    if (elem->getVR() == EVR_SS)
    {
      m_ZeroVelocityPixelValueSS.copyFrom(*elem);
    }
    else if (elem->getVR() == EVR_US)
    {
      m_ZeroVelocityPixelValueUS.copyFrom(*elem);
    }
    else
    {
      Uint16 pixrep = 0;
      if (item.findAndGetUint16(DCM_PixelRepresentation, pixrep).good())
      {
        if (pixrep == 0)
        {
          m_ZeroVelocityPixelValueUS.putUint16(pixrep, 0);
        }
        else
        {
          m_ZeroVelocityPixelValueSS.putSint16(pixrep, 1);
        }
      }
      else
      {
        DCMFG_ERROR("Cannot read Zero Velocity Pixel Value");
      }
    }
  }
  return EC_Normal;
}


// Writes single Frame Content Sequence into given item
OFCondition FGImageDataType::write(DcmItem& item)
{
  DcmItem *seqItem = NULL;
  OFCondition result = createNewFGSequence(item, DCM_ImageDataTypeSequence, 0, seqItem);
  if (result.bad())
    return result;

  // --- write frame content macro attributes ---
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_DataType, "1", "1", "ImageDataTypeMacro");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_AliasedDataType, "1", "1", "ImageDataTypeMacro");
  if (!m_ZeroVelocityPixelValueSS.isEmpty())
  {
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_ZeroVelocityPixelValueSS, "1", "1C", "ImageDataTypeMacro");
  } else if (!m_ZeroVelocityPixelValueUS.isEmpty())
  {
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_ZeroVelocityPixelValueUS, "1", "1C", "ImageDataTypeMacro");
  }
  // Zero Velocity Pixel Value is required in case Data Type is TISSUE_VELOCITY,
  // FLOW_VELOCITY or DIRECTION_POWER.
  else
  {
    OFString val;
    m_DataType.getOFStringArray(val);
    if ( (val == "TISSUE_VELOCITY") || (val == "FLOW_VELOCITY") ||  (val == "DIRECTION_POWER") )
    {
      DCMFG_ERROR("Missing value for Zero Velocity Pixel Value (required if Data Type has" <<
                  "the value TISSUE_VELOCITY, FLOW_VELOCITY or DIRECTION_POWER");
      return FG_EC_InvalidData;
    }
  }

  return result;
}


int FGImageDataType::compare(const FGBase& rhs) const
{
  int result = FGBase::compare(rhs);
  if (result != 0)
    return result;

  const FGImageDataType* myRhs = OFstatic_cast(const FGImageDataType*, &rhs);

  // Compare all elements
  result = m_DataType.compare(myRhs->m_DataType);
  if (result == 0)
    result = m_AliasedDataType.compare(myRhs->m_AliasedDataType);
  if (result == 0)
    result = m_ZeroVelocityPixelValueSS.compare(myRhs->m_ZeroVelocityPixelValueSS);
  if (result == 0)
    result = m_ZeroVelocityPixelValueUS.compare(myRhs->m_ZeroVelocityPixelValueUS);

  return result;
}
