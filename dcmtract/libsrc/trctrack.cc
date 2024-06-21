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
 *  Module:  dcmtract
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class representing a Track from a Tractography Results IOD
 *
 */
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmtract/trctrack.h"
#include "dcmtk/dcmtract/trctypes.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcelem.h"

// default constructor (protected, instance creation via create() function)
TrcTrack::TrcTrack()
: IODComponent()
{
  TrcTrack::resetRules();
}


OFString TrcTrack::getName() const
{
  return "TrackSequenceItem";
}


OFCondition TrcTrack::create(const Float32* trackDataPoints,
                             const size_t numPoints,
                             const Uint16* colors,
                             const size_t numColors,
                             TrcTrack*& track /* result */)
{
  track = new TrcTrack();
  if (!track)
    return EC_MemoryExhausted;

  OFCondition result = track->setTrackData(trackDataPoints, numPoints);
  if (result.good())
  {
    result = track->setRecommendedDisplayCIELabValues(colors, numColors);
  }
  if (result.bad())
  {
    delete track;
    track = NULL;
  }
  return result;
}


void TrcTrack::resetRules()
{
  // Set rules for the attributes within this component
  getRules()->addRule(new IODRule(DCM_PointCoordinatesData, "1","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_RecommendedDisplayCIELabValueList, "1","1C", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_RecommendedDisplayCIELabValue, "3","1C", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
}


OFCondition TrcTrack::check(const OFBool quiet)
{
  OFCondition result;
  // Report errors but ignore them
  IODComponent::check(quiet);
  const Float32* data = NULL;
  unsigned long count = 0;
  // If the actual data is missing, return an error, ignore other errors
  if (m_Item->findAndGetFloat32Array(DCM_PointCoordinatesData, data, &count).good())
  {
    if (count != 0)
    {
      if (count % 3 == 0)
      {
        result = EC_Normal;
      }
      else
      {
        DCMTRACT_ERROR("Point Coordinates Data must have x,y,z coordinates for every point but has length: " << count);
        result = IOD_EC_InvalidElementValue;
      }
    }
    else
    {
      DCMTRACT_ERROR("Point Coordinates Data empty");
      result = IOD_EC_InvalidElementValue;
    }
  }
  else
  {
    DCMTRACT_ERROR("Point Coordinates Data element missing");
    result = IOD_EC_MissingAttribute;
  }
  return result;
}


OFCondition TrcTrack::read(DcmItem& source,
                           OFBool clearOldData)
{
  OFCondition result = IODComponent::read(source, clearOldData);
  if (result.good())
  {
    // fix number of values in point coordinates data if wrong
    if (!fixPointCoordinatesDataVM())
    {
      return TRC_EC_InvalidPointCoordinatesData;
    }
  }
  return result;
}


OFCondition TrcTrack::write(DcmItem& destination)
{
  return IODComponent::write(destination);
}


void TrcTrack::clearData()
{
  IODComponent::clearData();
}


TrcTrack::~TrcTrack()
{
  clearData();
}


size_t TrcTrack::getTrackData(const Float32*& data) const
{
  data = NULL;
  unsigned long count;
  if (m_Item->findAndGetFloat32Array(DCM_PointCoordinatesData, data, &count).good())
  {
    return count / 3;
  }
  return 0;
}


size_t TrcTrack::getNumDataPoints()
{
  DcmElement *elem = NULL;
  if (m_Item->findAndGetElement(DCM_PointCoordinatesData, elem).good())
  {
    Uint32 numBytes = elem->getLengthField();
    if (numBytes % 2 == 1)
    {
      DCMTRACT_WARN("Length of Point Coordinates data is odd (" << numBytes << "), assuming " << numBytes - 1);
      numBytes = numBytes -1;
    }
    // single point coordinate has 4 bytes (Float32), and point consists of 3 coordinates
    return numBytes / sizeof(Float32) / 3;
  }
  return 0;
}


OFBool TrcTrack::getRecommendedDisplayCIELabValue(Uint16& L,
                                                  Uint16& a,
                                                  Uint16& b)
{
  const Uint16* colors = NULL;
  unsigned long count = 0;
  if (m_Item->findAndGetUint16Array(DCM_RecommendedDisplayCIELabValue, colors, &count).good())
  {
    if (count < 3)
    {
      L = a = b = 0;
      return OFFalse;
    }
    else
    {
      L = colors[0];
      a = colors[1];
      b = colors[2];
      return OFTrue;
    }
  }
  else
  {
    return OFFalse;
  }
}


size_t TrcTrack::getRecommendedDisplayCIELabValueList(const Uint16*& colors)
{
  unsigned long count = 0;
  if (m_Item->findAndGetUint16Array(DCM_RecommendedDisplayCIELabValueList, colors, &count).good())
  {
    return count / 3;
  }
  return 0;
}


TrcTypes::E_TrackColorMode TrcTrack::getRecommendedDisplayCIELabMode()
{
  DcmElement *elem = NULL;
  if (m_Item->findAndGetElement(DCM_RecommendedDisplayCIELabValue, elem).good())
  {
    if (elem->getNumberOfValues() == 3)
    {
      if (m_Item->findAndGetElement(DCM_RecommendedDisplayCIELabValueList, elem).good())
      {
        DCMTRACT_DEBUG("Recommended Display CIE Lab Value List as well as Recommended Display CIE Lab Value are set, will try per track coloring");
      }
      return TrcTypes::CM_TRACK;
    }
    else
    {
      DCMTRACT_ERROR("Recommended Display CIE Lab Value is set but has more or less than 3 values");
      return TrcTypes::CM_ERROR;
    }
  }
  // RecommendedDisplayCIELabValue is not set or not set correctly,
  // try RecommendedDisplayCIELabValueList instead
  elem = NULL;
  if (m_Item->findAndGetElement(DCM_RecommendedDisplayCIELabValueList, elem).good())
  {
    size_t numBytes = elem->getLength();
    if (numBytes % 2 == 1)
    {
      DCMTRACT_WARN("Recommended Display CIE Lab Value List has odd length " << numBytes << ", assuming " << numBytes -1);
      numBytes = numBytes - 1;
    }
    if (numBytes > 0)
    {
      // divide by 2 to get number of 16 bit entries and divide by 3 since
      // every color has 3 components
      if (getNumDataPoints() == (numBytes / 2 / 3))
      {
        return TrcTypes::CM_POINTS;
      }
      else
      {
        DCMTRACT_ERROR("Number of colors in Recommended Display CIE Lab Value List does not match number of points in Point Coordinates Data");
        return TrcTypes::CM_ERROR;
      }
    }
    else
    {
      DCMTRACT_ERROR("Recommended Display CIE Lab Value List is empty and can not be used");
      return TrcTypes::CM_ERROR;
    }
  }

  return TrcTypes::CM_TRACKSET;
}



OFCondition TrcTrack::setTrackData(const Float32* trackDataPoints,
                                   const size_t numPoints)
{
  return m_Item->putAndInsertFloat32Array(DCM_PointCoordinatesData, trackDataPoints, OFstatic_cast(unsigned long, numPoints*3));
}



OFCondition TrcTrack::setRecommendedDisplayCIELabValues(const Uint16* colors,
                                                        const size_t numColors)
{
  OFCondition result;
  if ( (numColors == 0) && (colors == NULL))
  {
    m_Item->findAndDeleteElement(DCM_RecommendedDisplayCIELabValue);
    m_Item->findAndDeleteElement(DCM_RecommendedDisplayCIELabValueList);
    return EC_Normal;
  }
  else if (colors == NULL)
  {
    return TRC_EC_InvalidColorInformation;
  }
  if (numColors == 1)
  {
    return m_Item->putAndInsertUint16Array(DCM_RecommendedDisplayCIELabValue, colors, OFstatic_cast(unsigned long, numColors * 3));
  }
  else
  {
    return m_Item->putAndInsertUint16Array(DCM_RecommendedDisplayCIELabValueList, colors, OFstatic_cast(unsigned long, numColors * 3));
  }
}



OFBool TrcTrack::fixPointCoordinatesDataVM()
{
  const Float32* values = NULL;
  unsigned long numValues = 0;
  if (m_Item->findAndGetFloat32Array(DCM_PointCoordinatesData, values, &numValues).good())
  {
    if (numValues % 3 == 0)
    {
      return OFTrue;
    }
    else
    {
      if (m_Item->putAndInsertFloat32Array(DCM_PointCoordinatesData, values, numValues - (numValues % 3)).good())
      {
        DCMTRACT_WARN("Wrong number of values in track (" << numValues << "), cutting off " << numValues % 3 << " coordinates at the end");
        return OFTrue;
      }
    }
  } // else error
  return OFFalse;
}
