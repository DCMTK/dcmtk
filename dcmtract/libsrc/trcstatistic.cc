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
 *  Purpose: Statistics from a Tractography Results IOD
 *
 */
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmtract/trcstatistic.h"
#include "dcmtk/dcmtract/trctypes.h"


// ------------------------------ class TrcTStatistic --------------------------

// default constructor (protected, instance creation via create() function)
TrcStatistic::TrcStatistic()
: IODComponent(),
  m_Type(),
  m_TypeModifier(),
  m_Units(),
  m_Values()
{
  // This is an abstract class so it is enough to call resetRules() is called
  // from derived classes
}


void TrcStatistic::resetRules()
{
  // Set rules for the attributes within this component
  getRules()->addRule(new IODRule(DCM_ConceptNameCodeSequence, "1","1",getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_ModifierCodeSequence, "1","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_MeasurementUnitsCodeSequence, "1","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
}


OFCondition TrcStatistic::read(DcmItem& source,
                               const OFBool clearOldData)
{
  if (clearOldData)
    this->clearData();
  // Read as much as possible. In order to return success at minimum the tracks
  // must be readable.
  IODComponent::read(source, OFFalse /* we cleared data before */);
  DcmIODUtil::readSingleItem(source, DCM_ConceptNameCodeSequence, m_Type, getRules()->getByTag(DCM_ConceptNameCodeSequence));
  DcmIODUtil::readSingleItem(source, DCM_MeasurementUnitsCodeSequence, m_Units, getRules()->getByTag(DCM_MeasurementUnitsCodeSequence));
  DcmIODUtil::readSingleItem(source, DCM_ModifierCodeSequence, m_TypeModifier, getRules()->getByTag(DCM_ModifierCodeSequence));
  return EC_Normal;
}


OFCondition TrcStatistic::write(DcmItem& destination)
{
  OFCondition result;
  DcmIODUtil::writeSingleItem(result, DCM_ConceptNameCodeSequence, m_Type, getData(), getRules()->getByTag(DCM_ConceptNameCodeSequence));
  DcmIODUtil::writeSingleItem(result, DCM_MeasurementUnitsCodeSequence, m_Units, getData(), getRules()->getByTag(DCM_MeasurementUnitsCodeSequence));
  DcmIODUtil::writeSingleItem(result, DCM_ModifierCodeSequence, m_TypeModifier, getData(), getRules()->getByTag(DCM_ModifierCodeSequence));
  if (result.good()) result = IODComponent::write(destination);
  return result;
}


void TrcStatistic::clearData()
{
  m_Type.clearData();
  m_Units.clearData();
  m_TypeModifier.clearData();
  IODComponent::clearData();
}

OFCondition TrcStatistic::get(CodeSequenceMacro& typeCode,
                              CodeSequenceMacro& typeModifierCode,
                              CodeSequenceMacro& unitsCode)
{
  typeCode = m_Type;
  typeModifierCode = m_TypeModifier;
  unitsCode = m_Units;
  return EC_Normal;
}


OFCondition TrcStatistic::setCommon(const CodeSequenceMacro& typeCode,
                                    const CodeSequenceMacro& typeModifierCode,
                                    const CodeSequenceMacro& unitsCode)
{
  OFCondition result;
  result = OFconst_cast(CodeSequenceMacro*, &typeCode)->check(OFTrue /* quiet */);
  if (result.good())
  {
    m_Type = typeCode;
  }
  if (result.good())
  {
    result = OFconst_cast(CodeSequenceMacro*, &typeModifierCode)->check(OFTrue /* quiet */);
    if (result.good())
    {
      m_TypeModifier = typeModifierCode;
    }
  }
  if (result.good())
  {
    result = OFconst_cast(CodeSequenceMacro*, &unitsCode)->check(OFTrue /* quiet */);
    if (result.good())
    {
      m_Units = unitsCode;
    }
  }
  return result;
}


TrcStatistic::~TrcStatistic()
{
  // nothing to do
}



// ----------------------------- class TrcTrackStatistic -----------------------

TrcTracksStatistic::TrcTracksStatistic()
: TrcStatistic()
{
  resetRules();
}


TrcTracksStatistic::~TrcTracksStatistic()
{
// nothing to do
}

OFCondition TrcTracksStatistic::create(const CodeSequenceMacro& typeCode,
                                       const CodeSequenceMacro& typeModifierCode,
                                       const CodeSequenceMacro& unitsCode,
                                       const Float32* statisticValues,
                                       const size_t numValues,
                                       TrcTracksStatistic*& statistic)
{
  statistic = new TrcTracksStatistic();
  if (!statistic)
    return EC_MemoryExhausted;

  //OFCondition result = OFstatic_cast(TrcStatistic*,statistic)->set(typeCode, typeModifierCode, unitsCode);
  OFCondition result = statistic->TrcStatistic::setCommon(typeCode, typeModifierCode, unitsCode);
  if (result.good())
  {
    result = statistic->getData().putAndInsertFloat32Array(DCM_FloatingPointValues, statisticValues, OFstatic_cast(unsigned long, numValues));
    if (result.bad())
    {
      result = TRC_EC_InvalidStatisticData;
    }
  }
  if (result.bad())
  {
    delete statistic;
    statistic = NULL;
  }
  return result;
}


OFCondition TrcTracksStatistic::read(DcmItem& source,
                                     const OFBool clearOldData)
{
  return TrcStatistic::read(source, clearOldData);
}


OFCondition TrcTracksStatistic::write(DcmItem& destination)
{
  OFCondition result;
  result = TrcStatistic::write(destination);
  if (result.good())
  {
    result = IODComponent::write(destination);
  }
  return result;
}


OFCondition TrcTracksStatistic::get(CodeSequenceMacro& typeCode,
                                    CodeSequenceMacro& typeModifierCode,
                                    CodeSequenceMacro& unitsCode,
                                    const Float32*& statisticValues,
                                    unsigned long& numValues)
{
  OFCondition result = TrcStatistic::get(typeCode, typeModifierCode, unitsCode);

  if (result.good())
  {
    result = m_Item->findAndGetFloat32Array(DCM_FloatingPointValues, statisticValues, &numValues);
  }
  return result;
}


OFString TrcTracksStatistic::getName() const
{
  return "TrackStatisticsSequenceItem";
}


void TrcTracksStatistic::resetRules()
{
  TrcStatistic::resetRules();
  m_Rules->addRule(new IODRule(DCM_FloatingPointValues, "1","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
}


OFCondition TrcTracksStatistic::set(const CodeSequenceMacro& typeCode,
                                    const CodeSequenceMacro& typeModifierCode,
                                    const CodeSequenceMacro& unitsCode,
                                    const Float32* statisticValues,
                                    const size_t numValues)
{
  if ( !statisticValues || (numValues == 0))
  {
    return TRC_EC_InvalidStatisticData;
  }
  OFCondition result = TrcStatistic::setCommon(typeCode, typeModifierCode, unitsCode);
  if (result.good())
  {
    result = m_Item->putAndInsertFloat32Array(DCM_FloatingPointValues, statisticValues, OFstatic_cast(unsigned long, numValues));
  }
  return result;
}


// --------------------------- class TrcTrackSetStatistic ----------------------

TrcTrackSetStatistic::TrcTrackSetStatistic()
: TrcStatistic()
{
  resetRules();
}


TrcTrackSetStatistic::~TrcTrackSetStatistic()
{
  // nothing to do
}


OFCondition TrcTrackSetStatistic::create(const CodeSequenceMacro& typeCode,
                                         const CodeSequenceMacro& typeModifierCode,
                                         const CodeSequenceMacro& unitsCode,
                                         const Float64 statisticValue,
                                         TrcTrackSetStatistic *&statistic)
{
  statistic = new TrcTrackSetStatistic();
  if (!statistic)
    return EC_MemoryExhausted;

  OFCondition result = statistic->set(typeCode, typeModifierCode, unitsCode, statisticValue);
  if (result.bad())
  {
    delete statistic;
    statistic = NULL;
  }
  return result;
}


OFCondition TrcTrackSetStatistic::read(DcmItem& source,
                                       const OFBool clearOldData)
{
  OFCondition result = IODComponent::read(source, clearOldData);
  if (result.good())
  {
    TrcStatistic::read(source, clearOldData);
  }
  return result;
}


OFCondition TrcTrackSetStatistic::write(DcmItem& destination)
{
  OFCondition result;
  result = TrcStatistic::write(destination);
  if (result.good())
  {
    result = IODComponent::write(destination);
  }
  return result;
}



OFCondition TrcTrackSetStatistic::get(CodeSequenceMacro& typeCode,
                                      CodeSequenceMacro& typeModifierCode,
                                      CodeSequenceMacro& unitsCode,
                                      Float64& statisticValue)
{
  OFCondition result = TrcStatistic::get(typeCode, typeModifierCode, unitsCode);
  if (result.good())
  {
    result = m_Item->findAndGetFloat64(DCM_FloatingPointValue, statisticValue, 0);
    if (result.bad())
    {
      result = TRC_EC_InvalidStatisticData;
    }
  }
  return result;
}


OFString TrcTrackSetStatistic::getName() const
{
  return "TrackSetStatisticsSequenceItem";
}


void TrcTrackSetStatistic::resetRules()
{
 TrcStatistic::resetRules();
 m_Rules->addRule(new IODRule(DCM_FloatingPointValue, "1","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
}


OFCondition TrcTrackSetStatistic::set(const CodeSequenceMacro& typeCode,
                                      const CodeSequenceMacro& typeModifierCode,
                                      const CodeSequenceMacro& unitsCode,
                                      const Float64 statisticValue)
{
  OFCondition result = TrcStatistic::setCommon(typeCode, typeModifierCode, unitsCode);
  if (result.good())
  {
    result = m_Item->putAndInsertFloat64(DCM_FloatingPointValue, statisticValue, 0);
    if (result.bad())
    {
      result = TRC_EC_InvalidStatisticData;
    }
  }
  return result;
}
