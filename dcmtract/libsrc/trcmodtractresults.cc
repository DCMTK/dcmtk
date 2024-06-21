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
 *  Module: dcmtract
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class representing the Tractography Results Module
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmtract/trcmodtractresults.h"
#include "dcmtk/dcmtract/trctrack.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrtm.h"

const OFString TrcTractographyResultsModule::m_ModuleName = "TractographyResultsModule";

TrcTractographyResultsModule::TrcTractographyResultsModule(
  OFshared_ptr<DcmItem> item,
  OFshared_ptr<IODRules> rules)
: IODModule(item, rules),
  m_ContentIdentification(),
  m_TrackSets(),
  m_ReferencedInstances()
{
  // reset element rules
  resetRules();
}


TrcTractographyResultsModule::TrcTractographyResultsModule()
: IODModule(),
  m_ContentIdentification(),
  m_TrackSets(),
  m_ReferencedInstances()
{
  resetRules();
}


TrcTractographyResultsModule::~TrcTractographyResultsModule()
{
  DcmIODUtil::freeContainer(m_TrackSets);
}


OFCondition TrcTractographyResultsModule::read(DcmItem& source,
                                               const OFBool clearOldData)
{
  if (clearOldData)
    clearData();

  IODModule::read(source, OFFalse /* data already cleared above */);
  DcmIODUtil::readSubSequence(source, DCM_TrackSetSequence, m_TrackSets, m_Rules->getByTag(DCM_TrackSetSequence));
  DCMTRACT_DEBUG("Found " << m_TrackSets.size() << " Track Sets");
  m_ContentIdentification.read(source, clearOldData);
  m_ReferencedInstances.readTractographyReferencedInstanceSequence(source);
  DCMTRACT_DEBUG("Found " << m_ReferencedInstances.size() << " references to other DICOM objects");
  checkColoring();
  return EC_Normal;
}


OFCondition TrcTractographyResultsModule::write(DcmItem& destination)
{
  OFCondition result = EC_Normal;

  // Re-create dimension organization data
  DcmIODUtil::writeSubSequence(result, DCM_TrackSetSequence, m_TrackSets, *m_Item, m_Rules->getByTag(DCM_TrackSetSequence));
  if (result.good())
  {
    result = m_ReferencedInstances.writeTractographyReferencedInstanceSequence(getData());
  }
  if (result.good())
  {
    result = m_ContentIdentification.write(destination);
  }
  if (result.good())
  {
    result = IODComponent::write(destination);
  }
  return result;
}


void TrcTractographyResultsModule::clearData()
{
  DcmIODUtil::freeContainer(m_TrackSets);
  m_ContentIdentification.clearData();
  IODModule::clearData();
}


void TrcTractographyResultsModule::resetRules()
{
  // parameters are tag, VM, type. Overwrite old rules if any.
  m_Rules->addRule(new IODRule(DCM_ContentTime, "1", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_ContentDate, "1", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_TrackSetSequence, "1-n", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  m_Rules->addRule(new IODRule(DCM_ReferencedInstanceSequence, "1-n", "1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
}


OFString TrcTractographyResultsModule::getName() const
{
  return "TractographyResultsModule";
}


OFCondition TrcTractographyResultsModule::getContentDate(OFString &value,
                                                         const signed long pos)
{
  return DcmIODUtil::getStringValueFromItem(DCM_ContentDate, *m_Item, value, pos);
}


OFCondition TrcTractographyResultsModule::getContentTime(OFString &value,
                                                         const signed long pos)
{
  return DcmIODUtil::getStringValueFromItem(DCM_ContentTime, *m_Item, value, pos);
}


ContentIdentificationMacro& TrcTractographyResultsModule::getContentIdentification()
{
  return m_ContentIdentification;
}


IODReferences& TrcTractographyResultsModule::getReferencedInstances()
{
  return m_ReferencedInstances;
}


OFVector< TrcTrackSet* >& TrcTractographyResultsModule::getTrackSets()
{
  return m_TrackSets;
}


OFCondition TrcTractographyResultsModule::setContentDate(const OFString &value,
                                                         const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmDate::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_ContentDate, value);
  return result;
}


OFCondition TrcTractographyResultsModule::setContentTime(const OFString &value,
                                                         const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmTime::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_ContentTime, value);
  return result;
}


OFCondition TrcTractographyResultsModule::addImageReference(const IODReference& imageRef)
{
  OFCondition result = imageRef.check();
  if (result.good())
  {
    IODReference *copy = imageRef.clone();
    if (copy)
    {
      m_ReferencedInstances.add(copy);
    }
    else
    {
      result = EC_MemoryExhausted;
    }
  }
  return result;
}


OFCondition TrcTractographyResultsModule::check(const OFBool quiet)
{
  if (m_ContentIdentification.check().good())
  {
    if (m_TrackSets.size() > 0)
    {
      OFVector<TrcTrackSet*>::iterator it = m_TrackSets.begin();
      while (it != m_TrackSets.end())
      {
        if ( (*it)->getNumberOfTracks() == 0)
        {
          DCMTRACT_ERROR("Track Set does not contain any tracks");
          return TRC_EC_NoSuchTrack;
        }
        else
        {
          OFVector<TrcTrack*> tracks = (*it)->getTracks();
          OFVector<TrcTrack*>::iterator t = tracks.begin();
          while (t != tracks.end())
          {
            if ( (*t)->check(quiet).bad() )
              return TRC_EC_InvalidTrackData;
            t++;
          }
        }
        it++;
      }
    }
    else
    {
      return TRC_EC_NoSuchTrack;
    }
  }
  return checkColoring();
}


OFCondition TrcTractographyResultsModule::checkColoring()
{
  size_t tsCount = 1;
  OFVector<TrcTrackSet*>::iterator ts = m_TrackSets.begin();
  while (ts != m_TrackSets.end())
  {
    // Collect statistics how much are colored
    OFVector<TrcTrack*>::const_iterator track = (*ts)->getTracks().begin();
    OFVector<TrcTrack*>::const_iterator last = (*ts)->getTracks().end();
    size_t numPerTrack = 0;
    size_t numPerPoint = 0;
    size_t trackCount = 1;
    while ( track != last)
    {
      TrcTypes::E_TrackColorMode cmode = (*track)->getRecommendedDisplayCIELabMode();
      switch (cmode)
      {
        case TrcTypes::CM_ERROR:
          DCMTRACT_ERROR("Color definition error in Track Set #" << tsCount << "/" << m_TrackSets.size() << " Track #" << trackCount << "/" << (*ts)->getNumberOfTracks());
          return TRC_EC_InvalidColorInformation;
        case TrcTypes::CM_POINTS:
          numPerPoint++;
          break;
        case TrcTypes::CM_TRACK:
          numPerTrack++;
          break;
        case TrcTypes::CM_TRACKSET:
          // the rest that is not per point or per track
          break;
        default:
          DCMTRACT_ERROR("Internal color definition checking problem in Track Set #" << tsCount << "/" << m_TrackSets.size() << " Track #" << trackCount << "/" << (*ts)->getNumberOfTracks());
          return EC_Normal;
      }
      track++;
      trackCount++;
    }

    // Do we have tracks at all? Otherwise report error
    size_t numTracks = (*ts)->getNumberOfTracks();
    if (numTracks == 0)
    {
      DCMTRACT_ERROR("No tracks in Track Set");
      return TRC_EC_NoSuchTrack;
    }

    // Check if we have Track Set coloring
    if ((numPerPoint == 0) && (numPerTrack == 0))
    {
      Uint16 L,a,b;
      L=a=b=0;
      // Check whether we have Track Set Coloring
      OFCondition result = (*ts)->getRecommendedDisplayCIELabValue(L,a,b);
      if (result.bad())
      {
        DCMTRACT_ERROR("Neither Tracks nor Track Set has color information in Track Set #" << tsCount << "/" << m_TrackSets.size());
        return TRC_EC_InvalidColorInformation;
      }
    }
    // No Track Set coloring, check whether all Tracks have Track or point coloring
    else if (numTracks != (numPerPoint + numPerTrack))
    {
      DCMTRACT_ERROR("Some but not all tracks provide color information in Track Set #" << tsCount  << "/" << m_TrackSets.size());
      return TRC_EC_InvalidColorInformation;
    }
    ts++;
    tsCount++;
  }
  return EC_Normal;
}
