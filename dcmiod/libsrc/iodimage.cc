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
 *  Purpose: Class repres. image IOD by exposing common image IOD attributes
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/iodimage.h"
#include "dcmtk/dcmiod/iodutil.h"

DcmIODImage::DcmIODImage()
: m_GeneralImage(getData(), getRules()),
  m_ImagePixel(getData(), getRules())
{
  // Nothing to do
}


IODGeneralImageModule& DcmIODImage::getGeneralImage()
{
  return m_GeneralImage;
}


IODImagePixelModule& DcmIODImage::getImagePixel()
{
  return m_ImagePixel;
}


DcmIODImage::~DcmIODImage()
{
  // IOD rules created in DcmIODImage are cleared in DcmIODCommon::~DcmIODCommon()
}


void DcmIODImage::clearData()
{
  // clear all attributes from base classes
  DcmIODCommon::clearData();
  m_GeneralImage.clearData();
  m_ImagePixel.clearData();
}


OFCondition DcmIODImage::read(DcmItem &dataset)
{
  // re-initialize object
  DcmIODImage::clearData();

  // read from base classes
  DcmIODCommon::read(dataset);
  m_GeneralImage.read(dataset, OFTrue /* clear old data */);
  m_ImagePixel.read(dataset, OFTrue /* clear old data */);

  return EC_Normal;
}


OFCondition DcmIODImage::write(DcmItem &dataset)
{
  OFCondition result = EC_Normal;

  // Write base class attributes to dataset
  result = DcmIODCommon::write(dataset);

  // Write extra modules to dataset
  if (result.good())
    result = m_GeneralImage.write(dataset);
  if (result.good())
    result = m_ImagePixel.write(dataset);

  return result;
}
