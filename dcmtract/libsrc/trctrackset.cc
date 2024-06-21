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
 *  Purpose: Class representing a Track Set from a Tractography Results IOD
 *
 */
#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofutil.h"
#include "dcmtk/dcmdata/dcvrut.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmtract/trctrackset.h"
#include "dcmtk/dcmtract/trctrack.h"
#include "dcmtk/dcmtract/trcmeasurement.h"
#include "dcmtk/dcmtract/trcstatistic.h"
#include "dcmtk/dcmtract/trctypes.h"

// default constructor (protected, instance creation via create() function)
TrcTrackSet::TrcTrackSet()
: IODComponent(),
  m_Tracks(),
  m_Anatomy("3", "1"),
  m_Measurements(),
  m_TrackStatistics(),
  m_TrackSetStatistics(),
  m_DiffusionAcquisitionCode(),
  m_DiffusionModelCode(),
  m_TrackingAlgorithmIdentification()
{
  TrcTrackSet::resetRules();
}


void TrcTrackSet::resetRules()
{

  // Set rules for the attributes within this component
  getRules()->addRule(new IODRule(DCM_TrackSetNumber, "1","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_TrackSetLabel, "1","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_TrackSetDescription, "1","3", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_TrackSetAnatomicalTypeCodeSequence, "1","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_TrackSequence, "1-n","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_RecommendedDisplayCIELabValue, "3","1C", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_RecommendedLineThickness, "1","3", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_MeasurementsSequence, "1-n","3", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_TrackStatisticsSequence, "1-n","3", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_TrackSetStatisticsSequence, "1-n","3", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_DiffusionAcquisitionCodeSequence, "1","3", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_DiffusionModelCodeSequence, "1","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_TrackingAlgorithmIdentificationSequence, "1-n","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
}


OFCondition TrcTrackSet::create(const OFString& trackSetLabel,
                                const OFString& trackSetDescription,
                                const CodeWithModifiers& trackSetAnatomy,
                                const CodeSequenceMacro& trackSetDiffusionModelCode,
                                const AlgorithmIdentificationMacro& trackSetAlgorithmIdentification,
                                TrcTrackSet*& trackSet)
{
  // Track Set Number is set within TrcTractographyResults::addTrackSet()
  trackSet = new TrcTrackSet();
  if (!trackSet)
    return EC_MemoryExhausted;

  OFCondition result = trackSet->setTrackSetLabel(trackSetLabel);
  if (result.good()) result = trackSet->setTrackSetDescription(trackSetDescription);

  if (result.good())
  {
    result = OFconst_cast(CodeWithModifiers*, &trackSetAnatomy)->check();
    if (result.good())
    {
      trackSet->m_Anatomy = trackSetAnatomy;
    }
  }

  if (result.good())
  {
    result = OFconst_cast(CodeSequenceMacro*, &trackSetDiffusionModelCode)->check();
    if (result.good())
    {
      trackSet->m_DiffusionModelCode = trackSetDiffusionModelCode;
    }
  }

  if (result.good())
  {
    result = OFconst_cast(AlgorithmIdentificationMacro*, &trackSetAlgorithmIdentification)->check();
    if (result.good())
    {
      // add deep copy (via IODComponent) of input param
      trackSet->m_TrackingAlgorithmIdentification.push_back(new AlgorithmIdentificationMacro(trackSetAlgorithmIdentification));
    }
  }

  return result;
}


OFCondition TrcTrackSet::read(DcmItem& source,
                              const OFBool clearOldData)
{
  if (clearOldData)
    this->clearData();
  // Read as much as possible. In order to return success at minimum the tracks
  // must be readable.
  IODComponent::read(source, OFFalse /* we cleared data before */);
  DcmIODUtil::readSingleItem(source, DCM_TrackSetAnatomicalTypeCodeSequence, m_Anatomy, getRules()->getByTag(DCM_TrackSetAnatomicalTypeCodeSequence));
  DcmIODUtil::readSingleItem(source, DCM_DiffusionAcquisitionCodeSequence, m_DiffusionAcquisitionCode, getRules()->getByTag(DCM_DiffusionAcquisitionCodeSequence));
  DcmIODUtil::readSingleItem(source, DCM_DiffusionModelCodeSequence, m_DiffusionModelCode, getRules()->getByTag(DCM_DiffusionModelCodeSequence));
  DcmIODUtil::readSubSequence(source, DCM_TrackingAlgorithmIdentificationSequence, m_TrackingAlgorithmIdentification, getRules()->getByTag(DCM_TrackingAlgorithmIdentificationSequence));
  readTrackStatistics(source);
  readTrackSetStatistics(source);
  OFCondition result = readTracks(source);
  DCMTRACT_DEBUG("Found " << m_Tracks.size() << " Tracks in Track Set");
  if (result.good())
  {
    readMeasurements(source);
  }
  else
  {
    DCMTRACT_ERROR("Could not read tracks from Track Set");
  }
  return result;
}


OFCondition TrcTrackSet::write(DcmItem& destination)
{
  OFCondition result;
  DcmIODUtil::writeSingleItem(result, DCM_TrackSetAnatomicalTypeCodeSequence, m_Anatomy, getData(), getRules()->getByTag(DCM_TrackSetAnatomicalTypeCodeSequence));
  DcmIODUtil::writeSingleItem(result, DCM_DiffusionAcquisitionCodeSequence, m_DiffusionAcquisitionCode, getData(), getRules()->getByTag(DCM_DiffusionAcquisitionCodeSequence));
  DcmIODUtil::writeSingleItem(result, DCM_DiffusionModelCodeSequence, m_DiffusionModelCode, getData(), getRules()->getByTag(DCM_DiffusionModelCodeSequence));
  DcmIODUtil::writeSubSequence(result, DCM_TrackingAlgorithmIdentificationSequence, m_TrackingAlgorithmIdentification, getData(), getRules()->getByTag(DCM_TrackingAlgorithmIdentificationSequence));
  writeTrackStatistics(result, getData());
  writeTrackSetStatistics(result, getData());
  writeMeasurements(result, getData());
  writeTracks(result, getData());
  if (result.good()) result = IODComponent::write(destination);
  return result;
}


const OFVector<TrcTrack *>& TrcTrackSet::getTracks()
{
  return m_Tracks;
}


void TrcTrackSet::clearData()
{
  m_Anatomy.clearData();
  DcmIODUtil::freeContainer(m_Tracks);
  DcmIODUtil::freeContainer(m_Measurements);
  DcmIODUtil::freeContainer(m_TrackStatistics);
  DcmIODUtil::freeContainer(m_TrackSetStatistics);
  DcmIODUtil::freeContainer(m_TrackingAlgorithmIdentification);
  m_DiffusionAcquisitionCode.clearData();
  m_DiffusionModelCode.clearData();
  IODComponent::clearData();
}


TrcTrackSet::~TrcTrackSet()
{
  clearData();
}


OFString TrcTrackSet::getName() const
{
  return "TrackSetSequenceItem";
}


void TrcTrackSet::inventMissing()
{
  IODComponent::inventMissing();
  // Note: We could also add code to automatic default coloring
}


OFCondition TrcTrackSet::getTrackSetNumber(Uint16& value,
                                           const unsigned long pos) const
{
  return m_Item->findAndGetUint16(DCM_TrackSetNumber, value, pos);
}


OFCondition TrcTrackSet::getTrackSetLabel(OFString& value,
                                          const long signed int pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_TrackSetLabel, *m_Item, value, pos);
}


OFCondition TrcTrackSet::getTrackSetDescription(OFString& value,
                                                const long signed int pos) const
{
  return DcmIODUtil::getStringValueFromItem(DCM_TrackSetDescription, *m_Item, value, pos);
}


OFCondition TrcTrackSet::getRecommendedDisplayCIELabValue(Uint16& L,
                                                          Uint16& a,
                                                          Uint16& b)
{
  Uint16 myL, mya, myb;
  myL = mya = myb = 0;
  DcmElement *elem = NULL;
  if (m_Item->findAndGetElement(DCM_RecommendedDisplayCIELabValue, elem).good() )
  {
    if (elem->getUint16(myL, 0).good())
    {
      L = myL;
      if (elem->getUint16(mya, 1).good())
      {
        a = mya;
        if (elem->getUint16(myb, 2).good())
        {
          b = myb;
          return EC_Normal;
        }
      }
    }
  }
  return IOD_EC_InvalidElementValue;
}


OFCondition TrcTrackSet::getRecommendedLineThickness(Float32& value,
                                                     const long unsigned int pos)
{
  return m_Item->findAndGetFloat32(DCM_RecommendedLineThickness, value, pos);
}


OFCondition TrcTrackSet::getLaterality(TrcTypes::E_TrackSetLaterality& laterality)
{
  OFCondition result;
  OFString value, designator;
  CodeSequenceMacro* mod = m_Anatomy.getModifier(0);
  if (mod)
  {
    mod->getCodingSchemeDesignator(designator);
    mod->getCodingSchemeDesignator(value);

    if (value.empty() && designator.empty())
    {
      laterality = TrcTypes::LAT_UNKNOWN;
    }
    else if (designator == "SRT") // all laterality codes are from SNOMED
    {
      if (value == "G-A100")
      {
        laterality = TrcTypes::LAT_RIGHT;
      }
      else if (value == "G-A101")
      {
        laterality = TrcTypes::LAT_LEFT;
      }
      else if (value == "G-A102")
      {
        laterality = TrcTypes::LAT_RIGHT_AND_LEFT;
      }
      else if (value == "G-A103")
      {
        laterality = TrcTypes::LAT_UNILATERAL;
      }
      else
      {
        laterality = TrcTypes::LAT_ERROR;
        result = IOD_EC_InvalidLaterality;
      }
    }
    else
    {
      laterality = TrcTypes::LAT_ERROR;
      result = IOD_EC_InvalidLaterality;
    }
  }
  else
  {
    laterality = TrcTypes::LAT_UNKNOWN;
  }
  return result;
}


CodeSequenceMacro& TrcTrackSet::getDiffusionAcquisitionCode()
{
  return m_DiffusionAcquisitionCode;
}


CodeSequenceMacro& TrcTrackSet::getDiffusionModelCode()
{
  return m_DiffusionModelCode;
}


OFVector<AlgorithmIdentificationMacro*>& TrcTrackSet::getTrackingAlgorithmIdentification()
{
  return m_TrackingAlgorithmIdentification;
}


CodeWithModifiers& TrcTrackSet::getTrackSetAnatomy()
{
  return m_Anatomy;
}


const OFVector< TrcTrackSetStatistic* >& TrcTrackSet::getTrackSetStatistics()
{
  return m_TrackSetStatistics;
}


const OFVector< TrcTracksStatistic* >& TrcTrackSet::getTrackStatistics()
{
  return m_TrackStatistics;
}


size_t TrcTrackSet::getNumberOfTracks()
{
  return m_Tracks.size();
}


size_t TrcTrackSet::getNumberOfTrackSetStatistics()
{
  return m_TrackSetStatistics.size();
}


size_t TrcTrackSet::getNumberOfTrackStatistics()
{
  return m_TrackStatistics.size();
}


size_t TrcTrackSet::getNumberOfMeasurements()
{
  return m_Measurements.size();
}


OFCondition TrcTrackSet::findMeasurementsByType(const CodeSequenceMacro& type,
                                                OFVector< size_t >& measurementNumbers)
{
  OFVector<TrcMeasurement*>::const_iterator it = m_Measurements.begin();
  for (size_t m = 0; it != m_Measurements.end(); m++)
  {
    if ( (*it)->getType().compare(type) == 0 )
    {
      measurementNumbers.push_back(m);
    }
    it++;
  }
  return EC_Normal;
}


OFCondition TrcTrackSet::getMeasurement(const size_t measurementIndex,
                                        TrcMeasurement*& measurement)
{
  if (measurementIndex > m_Measurements.size() - 1)
  {
    return TRC_EC_NoSuchMeasurement;
  }

  measurement = m_Measurements[measurementIndex];

  return EC_Normal;
}



void TrcTrackSet::getMeasurementInfos(OFVector< OFPair< CodeSequenceMacro, CodeSequenceMacro > >& typesAndUnits)
{
  OFVector<TrcMeasurement*>::const_iterator it = m_Measurements.begin();
  while (it != m_Measurements.end())
  {
    typesAndUnits.push_back( OFMake_pair( (*it)->getType(), (*it)->getUnits() ) );
    it++;
  }
}



OFCondition TrcTrackSet::addTrack(const Float32* pointData,
                                  const size_t numPoints,
                                  const Uint16* recommendedCIELabColors,
                                  const size_t numColors,
                                  TrcTrack*& result)
{
  OFCondition cond = TrcTrack::create(pointData, numPoints, recommendedCIELabColors, numColors, result);
  if (cond.good())
  {
    m_Tracks.push_back(result);
  }
  return cond;
}


OFCondition TrcTrackSet::addMeasurement(const CodeSequenceMacro& measurementCode,
                                        const CodeSequenceMacro& measurementUnitCodes,
                                        TrcMeasurement*& measurement)
{
  OFCondition result =  TrcMeasurement::create(measurementCode, measurementUnitCodes, measurement);
  if (result.good())
  {
    m_Measurements.push_back(measurement);
  }
  return result;
}


OFCondition TrcTrackSet::addTrackStatistics(const CodeSequenceMacro& typeCode,
                                            const CodeSequenceMacro& typeModifierCode,
                                            const CodeSequenceMacro& unitsCode,
                                            const Float32* values,
                                            const size_t numValues,
                                            TrcTracksStatistic*& statistic)
{
  statistic = new TrcTracksStatistic();
  if (!statistic)
    return EC_MemoryExhausted;

  if (numValues != m_Tracks.size())
  {
    DCMTRACT_ERROR("There must be as many track statistic values as number of tracks (" << m_Tracks.size() << ")");
    return TRC_EC_InvalidStatisticData;
  }
  OFCondition result = statistic->set(typeCode, typeModifierCode, unitsCode, values, numValues);
  if (result.good())
  {
    m_TrackStatistics.push_back(statistic);
  }
  else
  {
    delete statistic;
    statistic = NULL;
  }
  return result;
}


OFCondition TrcTrackSet::addTrackSetStatistic(const CodeSequenceMacro& typeCode,
                                              const CodeSequenceMacro& typeModifierCode,
                                              const CodeSequenceMacro& unitsCode,
                                              const Float64 value,
                                              TrcTrackSetStatistic*& statistic)
{
  statistic = new TrcTrackSetStatistic();
  if (!statistic)
    return EC_MemoryExhausted;

  OFCondition result = statistic->set(typeCode, typeModifierCode, unitsCode, value);
  if (result.good())
  {
    m_TrackSetStatistics.push_back(statistic);
  }
  else
  {
    delete statistic;
    statistic = NULL;
  }
  return result;
}



OFCondition TrcTrackSet::setTrackSetLabel(const OFString& value,
                                          const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmLongString::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_TrackSetLabel, value);
  return result;
}


OFCondition TrcTrackSet::setTrackSetDescription(const OFString& value,
                                                const OFBool checkValue)
{
  OFCondition result = (checkValue) ? DcmUnlimitedText::checkStringValue(value, "1") : EC_Normal;
  if (result.good())
    result = m_Item->putAndInsertOFStringArray(DCM_TrackSetDescription, value);
  return result;
}


OFCondition TrcTrackSet::setRecommendedDisplayCIELabValue(const Uint16 L,
                                                          const Uint16 a,
                                                          const Uint16 b)
{
  DcmElement* elem = DcmItem::newDicomElement(DCM_RecommendedDisplayCIELabValue);
  if (elem)
  {
    if (elem->putUint16(L, 0).good())
    {
      if (elem->putUint16(a, 1).good())
      {
        if (elem->putUint16(b, 2).good())
        {
          if (m_Item->insert(elem, OFTrue /* replace old */).good())
          {
            return EC_Normal;
          }
        }
      }
    }
  }
  else
  {
    return EC_MemoryExhausted;
  }
  return EC_InternalError;
}


OFCondition TrcTrackSet::setRecommendedLineThickness(const Float32& value,
                                                     const OFBool checkValue)
{
  (void)checkValue;
  return m_Item->putAndInsertFloat32(DCM_RecommendedLineThickness, value, 0);
}


OFCondition TrcTrackSet::readTracks(DcmItem& source)
{
  return DcmIODUtil::readSubSequence(source, DCM_TrackSequence, m_Tracks, getRules()->getByTag(DCM_TrackSequence));
}


OFCondition TrcTrackSet::readMeasurements(DcmItem& source)
{
  DcmIODUtil::readSubSequence(source, DCM_MeasurementsSequence, m_Measurements, getRules()->getByTag(DCM_MeasurementsSequence));
  if (checkMeasurements().bad())
  {
    DCMTRACT_WARN("Ignoring missing or superfluous Measurements");
  }
  return EC_Normal;
}


OFCondition TrcTrackSet::setLaterality(const TrcTypes::E_TrackSetLaterality value,
                                       const OFBool checkValue)
{
  (void)checkValue;
  CodeSequenceMacro anatomy;
  switch(value)
  {
    case TrcTypes::LAT_RIGHT: anatomy.set("G-A100", "SRT", "Right"); break;
    case TrcTypes::LAT_LEFT: anatomy.set("G-A101", "SRT", "Left"); break;
    case TrcTypes::LAT_RIGHT_AND_LEFT: anatomy.set("G-A102", "SRT", "Right and left"); break;
    case TrcTypes::LAT_UNILATERAL: anatomy.set("G-A103", "SRT", "Unilateral"); break;
    default: return IOD_EC_InvalidLaterality;
  }

  CodeSequenceMacro* mod = m_Anatomy.getModifier(0);
  if (!mod)
  {
    mod = new CodeSequenceMacro();
    if (!mod)
    {
      return EC_MemoryExhausted;
    }
    *mod = anatomy;
    m_Anatomy.addModifier(*mod);
  }
  else
  {
    *mod = anatomy;
  }

  return EC_Normal;;
}


OFCondition TrcTrackSet::checkMeasurements()
{
  size_t numTracks = m_Tracks.size();
  for (size_t i = 0; i < m_Measurements.size(); i++)
  {
    TrcMeasurement *m = m_Measurements[i];
    size_t numEntries = m->getValues().size();
    if (numEntries < numTracks)
    {
      DCMTRACT_WARN("Measurement #" << i << " misses data for " << numTracks - numEntries  << " tracks");
      return TRC_EC_MeasurementDataMissing;
    }
    else if (numEntries > numTracks)
    {
      DCMTRACT_WARN("Measurement #" << i << " has data for " << numEntries - numTracks << " too many tracks");
      return TRC_EC_MeasurementDataMissing;
    }
  }
  return EC_Normal;
}


OFCondition TrcTrackSet::readTrackSetStatistics(DcmItem& source)
{
  (void)source;
  DcmIODUtil::readSubSequence(source, DCM_TrackSetStatisticsSequence, m_TrackSetStatistics, m_Rules->getByTag(DCM_TrackSetStatisticsSequence));
  return EC_Normal;
}


OFCondition TrcTrackSet::readTrackStatistics(DcmItem& source)
{
  (void)source;
  DcmIODUtil::readSubSequence(source, DCM_TrackStatisticsSequence, m_TrackStatistics, m_Rules->getByTag(DCM_TrackStatisticsSequence));
  return EC_Normal;
}


OFCondition TrcTrackSet::writeTracks(OFCondition& result,
                                     DcmItem& destination)
{
  if (result.good())
  {
    DcmIODUtil::writeSubSequence(result, DCM_TrackSequence, m_Tracks, destination, getRules()->getByTag(DCM_TrackSequence));
  }
  return result;
}


OFCondition TrcTrackSet::writeMeasurements(OFCondition& result,
                                           DcmItem& destination)
{
  if (result.good())
  {
    result = checkMeasurements();
    if (result.bad())
    {
      DCMTRACT_ERROR("Measurements have too much or to less data");
      return result;
    }
    DcmIODUtil::writeSubSequence(result, DCM_MeasurementsSequence, m_Measurements, destination, getRules()->getByTag(DCM_MeasurementsSequence));
  }
  return result;
}


void TrcTrackSet::writeTrackSetStatistics(OFCondition& result,
                                          DcmItem& destination)
{
  DcmIODUtil::writeSubSequence(result, DCM_TrackSetStatisticsSequence, m_TrackSetStatistics, destination, m_Rules->getByTag(DCM_TrackSetStatisticsSequence));
}


void TrcTrackSet::writeTrackStatistics(OFCondition& result,
                                       DcmItem& destination)
{
  DcmIODUtil::writeSubSequence(result, DCM_TrackStatisticsSequence, m_TrackStatistics, destination, m_Rules->getByTag(DCM_TrackStatisticsSequence));
}

