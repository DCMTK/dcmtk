/*
 *
 *  Copyright (C) 2016-2018, Open Connections GmbH
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
 *  Module: dcmfg
 *
 *  Author: Jan Schlamelcher
 *
 *  Purpose: Class for managing the (Identity) Pixel Value Transformation FG
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmfg/fgpixeltransform.h"

FGPixelValueTransformation::FGPixelValueTransformation()
: FGBase(DcmFGTypes::EFG_UNDEFINED)
, m_RescaleIntercept(DCM_RescaleIntercept)
, m_RescaleSlope(DCM_RescaleSlope)
, m_RescaleType(DCM_RescaleType)
, m_UseAsIdentityPixelValueTransformationFG(OFFalse)
{
  m_RescaleIntercept.putOFStringArray("0");
  m_RescaleSlope.putOFStringArray("1");
  m_RescaleType.putOFStringArray("US");
}


void FGPixelValueTransformation::setUseAsIdentityPixelValueTransformation()
{
  m_UseAsIdentityPixelValueTransformationFG = OFTrue;
}



FGPixelValueTransformation::~FGPixelValueTransformation()
{

}


OFCondition FGPixelValueTransformation::check() const
{
  if (m_UseAsIdentityPixelValueTransformationFG)
  {
    Float64 rs, ri;
    rs = ri = 0;
    OFString rt;
    OFBool ok = OFTrue;
    if (OFconst_cast(DcmDecimalString*, &m_RescaleIntercept)->getFloat64(ri).good())
    {
      if (ri != 0)
      {
        DCMFG_ERROR("Rescale Intercept in Identity Pixel Value Transformation FG must be 0 but is set to " << ri);
        ok = OFFalse;
      }
    }
    else
    {
      DCMFG_ERROR("Invalid or no value for Rescale Intercept in Identity Pixel Value Transformation FG (0 is the only valid value");
      ok = OFFalse;
    }

    if (OFconst_cast(DcmDecimalString*, &m_RescaleSlope)->getFloat64(rs).good())
    {
      if (rs != 1)
      {
        DCMFG_ERROR("Rescale Slope in Identity Pixel Value Transformation FG must be 1 but is set to " << rs);
        ok = OFFalse;
      }
    }
    else
    {
      DCMFG_ERROR("Invalid or no value for Rescale Slope in Identity Pixel Value Transformation FG (1 is the only valid value");
      ok = OFFalse;
    }

    if (OFconst_cast(DcmLongString*, &m_RescaleType)->getOFStringArray(rt).good())
    {
      if (rt != "US")
      {
        DCMFG_ERROR("Rescale Type in Identity Pixel Value Transformation FG must be \"US\" but is set to " << rt);
        ok = OFFalse;
      }
    }
    else
    {
      DCMFG_ERROR("Invalid or no value for Rescale Type in Identity Pixel Value Transformation FG (\"US\" is the only valid value");
      ok = OFFalse;
    }
    if (!ok)
    {
      return FG_EC_InvalidData;
    }
  }
  return EC_Normal;
}


void FGPixelValueTransformation::clearData()
{
  m_RescaleIntercept.clear();
  m_RescaleSlope.clear();
  m_RescaleType.clear();
}


FGBase* FGPixelValueTransformation::clone() const
{
  if(FGPixelValueTransformation* copy = new FGPixelValueTransformation)
  {
    copy->m_RescaleIntercept = m_RescaleIntercept;
    copy->m_RescaleSlope = m_RescaleSlope;
    copy->m_RescaleType = m_RescaleType;
    return copy;
  }
  return OFnullptr;
}


OFCondition FGPixelValueTransformation::read(DcmItem& item)
{
  clearData();

  DcmItem* seqItem;
  OFCondition result;

  seqItem = OFnullptr;
  result = getItemFromFGSequence(item, DCM_PixelValueTransformationSequence, 0, seqItem);
  if(result.bad())
    return result;
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_RescaleIntercept, "1", "1", "Identity Pixel Value Transformation");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_RescaleSlope, "1", "1", "Identity Pixel Value Transformation");
  DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_RescaleType, "1", "1", "Identity Pixel Value Transformation");

  return EC_Normal;
}


OFCondition FGPixelValueTransformation::write(DcmItem& item)
{
  DcmItem* seqItem;
  DCMFG_DEBUG("Identity Pixel Value Transformation Macro: Fixing values for Rescale Slope, Intercept and Type to enumerated values '1', '0' and 'US'");
  m_RescaleSlope.putOFStringArray("1");
  m_RescaleIntercept.putOFStringArray("0");
  m_RescaleType.putOFStringArray("US");

  seqItem = OFnullptr;
  OFCondition result = createNewFGSequence(item, DCM_PixelValueTransformationSequence, 0, seqItem);
  if(result.bad())
    return result;
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_RescaleIntercept, "1", "1", "Identity Pixel Value Transformation");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_RescaleSlope, "1", "1", "Identity Pixel Value Transformation");
  DcmIODUtil::copyElementToDataset(result, *seqItem, m_RescaleType, "1", "1", "Identity Pixel Value Transformation");

  return result;
}


int FGPixelValueTransformation::compare(const FGBase& rhs) const
{
  int result = FGBase::compare(rhs);
  if(result == 0)
  {
    const FGPixelValueTransformation* myRhs = OFstatic_cast(const FGPixelValueTransformation*, &rhs);
    if (!myRhs)
      return -1;

    // Compare all elements
    result = m_RescaleIntercept.compare(myRhs->m_RescaleIntercept);
    if (result == 0) result = m_RescaleSlope.compare(myRhs->m_RescaleSlope);
    if (result == 0) result = m_RescaleType.compare(myRhs->m_RescaleType);
  }

  return result;
}


OFCondition FGPixelValueTransformation::getRescaleIntercept(OFString &value,
                                                                    const signed long pos) const
{
  return DcmIODUtil::getStringValueFromElement(m_RescaleIntercept, value, pos);
}


OFCondition FGPixelValueTransformation::getRescaleSlope(OFString &value,
                                                                const signed long pos) const
{
  return DcmIODUtil::getStringValueFromElement(m_RescaleSlope, value, pos);
}


OFCondition FGPixelValueTransformation::getRescaleType(OFString &value,
                                                               const signed long pos) const
{
  return DcmIODUtil::getStringValueFromElement(m_RescaleType, value, pos);
}


OFCondition FGPixelValueTransformation::setRescaleIntercept(const OFString &value,
                                                                    const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_RescaleIntercept.putString(value.c_str());
  return result;
}


OFCondition FGPixelValueTransformation::setRescaleSlope(const OFString &value,
                                                                const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmDecimalString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_RescaleSlope.putString(value.c_str());
  return result;
}


OFCondition FGPixelValueTransformation::setRescaleType(const OFString &value,
                                                               const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_RescaleType.putString(value.c_str());
  return result;
}

