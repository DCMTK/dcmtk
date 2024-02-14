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
 *  Purpose: Track Set's Measurement from a Tractography Results IOD
 *
 */
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmtract/trcmeasurement.h"
#include "dcmtk/dcmtract/trctypes.h"

// default constructor (protected, instance creation via create() function)
TrcMeasurement::TrcMeasurement()
: IODComponent(),
  m_Type(),
  m_Units(),
  m_Values()
{
  TrcMeasurement::resetRules();
  // Reserve space for 100 tracks, will grow if more are added
  m_Values.reserve(100);
}


void TrcMeasurement::resetRules()
{
  // Set rules for the attributes within this component
  getRules()->addRule(new IODRule(DCM_ConceptNameCodeSequence, "1","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_MeasurementUnitsCodeSequence, "1","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_MeasurementValuesSequence, "1-n","1", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
}

OFCondition TrcMeasurement::create(const CodeSequenceMacro& typeCode,
                                   const CodeSequenceMacro& unitCode,
                                   TrcMeasurement*& measurement)
{
  measurement = new TrcMeasurement();
  if (!measurement)
    return EC_MemoryExhausted;

  OFCondition result = OFconst_cast(CodeSequenceMacro*, &typeCode)->check();
  if (result.good())
  {
    measurement->m_Type = typeCode;
    result = OFconst_cast(CodeSequenceMacro*, &unitCode)->check();
    if (result.good())
    {
      measurement->m_Units = unitCode;
    }
  }
  if (result.bad())
  {
    delete measurement;
    measurement = NULL;
  }
  return result;
}


OFCondition TrcMeasurement::get(const size_t trackNumber,
                                CodeSequenceMacro& type,
                                CodeSequenceMacro& units,
                                const Float32*& measurementData,
                                long unsigned int& numMeasurementData,
                                const Uint32*& pointIndices)
{
  type = m_Type;
  units = m_Units;
  if (m_Values.size() >= trackNumber + 1)
  {
    return TRC_EC_NoSuchMeasurement;
  }
  TrcMeasurement::Values* v = m_Values[trackNumber];
  if (v == NULL)
  {
    return TRC_EC_NoSuchMeasurement;
  }
  OFCondition result = v->get(measurementData, numMeasurementData, pointIndices);
  return result;
}


OFCondition TrcMeasurement::setTrackValues(const size_t trackNumber,
                                           const Float32* measurementValues,
                                           const long unsigned int& numValues)
{
  if (trackNumber +1 > m_Values.size())
  {
    m_Values.resize(trackNumber+1, NULL);
  }
  else if (m_Values[trackNumber])
  {
    delete m_Values[trackNumber];
    m_Values[trackNumber] = NULL;
  }

  TrcMeasurement::Values* v = NULL;
  OFCondition result = TrcMeasurement::Values::create(measurementValues, numValues, v);
  if (result.good())
    m_Values[trackNumber] = v;

  return result;
}


OFCondition TrcMeasurement::setTrackValues(const size_t trackNumber,
                                           const Float32* measurementValues,
                                           const long unsigned int& numValues,
                                           const Uint32* pointIndices)
{
  if (trackNumber +1 > m_Values.size())
  {
    m_Values.resize(trackNumber+1, NULL);
  }
  else if (m_Values[trackNumber])
  {
    delete m_Values[trackNumber];
    m_Values[trackNumber] = NULL;
  }

  TrcMeasurement::Values* v = NULL;
  OFCondition result = TrcMeasurement::Values::createWithIndices(
    measurementValues,
    numValues,
    pointIndices,
    v);

  if (result.good())
  {
    m_Values[trackNumber] = v;
  }
  return result;
}



OFCondition TrcMeasurement::read(DcmItem& source,
                                 const OFBool clearOldData)
{
  if (clearOldData)
    this->clearData();
  // Read as much as possible. In order to return success at minimum the tracks
  // must be readable.
  IODComponent::read(source, OFFalse /* we cleared data before */);
  DcmIODUtil::readSingleItem(source, DCM_ConceptNameCodeSequence, m_Type, getRules()->getByTag(DCM_ConceptNameCodeSequence));
  DcmIODUtil::readSingleItem(source, DCM_MeasurementUnitsCodeSequence, m_Units, getRules()->getByTag(DCM_MeasurementUnitsCodeSequence));
  DcmIODUtil::readSubSequence(source, DCM_MeasurementValuesSequence, m_Values, getRules()->getByTag(DCM_MeasurementValuesSequence));
  return EC_Normal;
}


OFCondition TrcMeasurement::write(DcmItem& destination)
{
  if (!checkValuesComplete())
  {
    return TRC_EC_MeasurementDataMissing;
  }
  OFCondition result;
  DcmIODUtil::writeSingleItem(result, DCM_ConceptNameCodeSequence, m_Type, getData(), getRules()->getByTag(DCM_ConceptNameCodeSequence));
  DcmIODUtil::writeSingleItem(result, DCM_MeasurementUnitsCodeSequence, m_Units, getData(), getRules()->getByTag(DCM_MeasurementUnitsCodeSequence));
  DcmIODUtil::writeSubSequence(result, DCM_MeasurementValuesSequence, m_Values, getData(), getRules()->getByTag(DCM_MeasurementValuesSequence));
  if (result.good()) result = result = IODComponent::write(destination);
  return result;
}


void TrcMeasurement::clearData()
{
  m_Type.clearData();
  m_Units.clearData();
  DcmIODUtil::freeContainer(m_Values);
  IODComponent::clearData();
}


TrcMeasurement::~TrcMeasurement()
{
  DcmIODUtil::freeContainer(m_Values);
}


OFString TrcMeasurement::getName() const
{
  return "MeasurementsSequenceItem";
}


CodeSequenceMacro& TrcMeasurement::getType()
{
  return m_Type;
}


CodeSequenceMacro& TrcMeasurement::getUnits()
{
  return m_Units;
}


OFVector<TrcMeasurement::Values*>& TrcMeasurement::getValues()
{
  return m_Values;
}


OFBool TrcMeasurement::checkValuesComplete()
{
  OFBool complete = OFTrue;
  size_t count = m_Values.size();
  for (size_t v = 0; v < count; v++)
  {
    if (m_Values[v] == NULL)
    {
      DCMTRACT_ERROR("Measurement values for track #" << v << " are missing");
      complete = OFFalse;
    }
  }
  return complete;
}


// ------------------------Class TrcMeasurement::Values ----------------------


OFCondition TrcMeasurement::Values::create(const Float32* dataValues,
                                           const unsigned long numValues,
                                           TrcMeasurement::Values*& values)
{
  values = new TrcMeasurement::Values();
  if (!values)
   return EC_MemoryExhausted;

  if ( (numValues == 0) || !dataValues )
    return EC_IllegalParameter;

  OFCondition result = values->getData().putAndInsertFloat32Array(DCM_FloatingPointValues, dataValues, numValues);
  if (result.bad())
  {
    delete values;
    values = NULL;
  }
  return result;
}


OFCondition TrcMeasurement::Values::createWithIndices(const Float32* dataValues,
                                                      const unsigned long numValues,
                                                      const Uint32* trackPointIndices,
                                                      TrcMeasurement::Values*& values)
{
  values = new TrcMeasurement::Values();
  if (!values)
    return EC_MemoryExhausted;

  if ( (numValues == 0) || !dataValues || !trackPointIndices)
    return EC_IllegalParameter;

  OFCondition result = values->getData().putAndInsertFloat32Array(DCM_FloatingPointValues, dataValues, numValues);
  if (result.good())
  {
    result = values->getData().putAndInsertUint32Array(DCM_TrackPointIndexList, trackPointIndices, numValues);
  }

  if (result.bad())
  {
    delete values;
    values = NULL;
  }
  return result;
}


OFCondition TrcMeasurement::Values::get(const Float32*& dataValues,
                                        unsigned long& numValues,
                                        const Uint32*& trackPointIndices)
{
  OFCondition result = m_Item->findAndGetFloat32Array(DCM_FloatingPointValues, dataValues, &numValues);
  unsigned long numIndices = numValues;
  if (result.good())
  {
    result = m_Item->findAndGetUint32Array(DCM_TrackPointIndexList, trackPointIndices, &numIndices);
    if (result.bad())
    {
      trackPointIndices = NULL;
      numIndices = numValues;
      result = EC_Normal; // no track point indices mean that measurements cover all points
    }
  }
  // The number of point indices must be equal to the number of measurement
  // values. If not, print warning and use the smaller number.
  if ( numIndices != numValues)
  {
    char buf[200];
    sprintf(buf, "Number of measurement values for track (%lu) differs from number of track data indices (%lu), using smaller value", numValues, numIndices);
    DCMTRACT_WARN(buf);
    if (numValues > numIndices)
      numValues = numIndices;
  }
  return result;
}


OFString TrcMeasurement::Values::getName() const
{
  return "MeasurementValuesSequenceItem";
}


TrcMeasurement::Values::Values()
: IODComponent()
{
  resetRules();
}


void TrcMeasurement::Values::resetRules()
{
  // Both bulk data attribute are VM 1 by DICOM definition
  getRules()->addRule(new IODRule(DCM_FloatingPointValues, "1","1",  getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
  getRules()->addRule(new IODRule(DCM_TrackPointIndexList, "1","1C", getName(), DcmIODTypes::IE_INSTANCE), OFTrue);
}


TrcMeasurement::Values::~Values()
{
 // nothing to do yet
}





