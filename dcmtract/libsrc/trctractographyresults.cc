  /*
 *
 *  Copyright (C) 2016-2017, Open Connections GmbH
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
 *  Purpose: Class representing a Segmentation object
 *
 */
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmiod/modcommoninstanceref.h"
#include "dcmtk/dcmtract/trctractographyresults.h"

// default constructor (protected, instance creation via create() function)
TrcTractographyResults::TrcTractographyResults()
: DcmIODCommon(),
  m_EnhancedGeneralEquipmentModule(DcmIODCommon::getData(), DcmIODCommon::getRules()),
  m_TractographyResultsModule(DcmIODCommon::getData(), DcmIODCommon::getRules())
{
  TrcTractographyResults::initIODRules();
}


void TrcTractographyResults::initIODRules()
{
  // We do not explicitly implement Tractography Results Module but just change
  // existing rules from the General Series Module
  getRules()->addRule(new IODRule(DCM_ReferencedPerformedProcedureStepSequence, "1","1C", "TractographyResultsSeries", DcmIODTypes::IE_SERIES), OFTrue);
  getRules()->addRule(new IODRule(DCM_SeriesNumber, "1","1", "TractographyResultsSeries", DcmIODTypes::IE_SERIES), OFTrue);
}


TrcTractographyResults::~TrcTractographyResults()
{
  // nothing to do
}


// static method for loading tractography results objects
OFCondition TrcTractographyResults::loadFile(
  const OFString& filename,
  TrcTractographyResults*& trc)
{
  DcmFileFormat dcmff;
  DcmDataset *dataset = NULL;
  OFCondition result = loadFile(dcmff, filename, dataset);
  if (result.bad())
    return result;

  return loadDataset(*dataset, trc);
}


// static method for loading tractography results objects
OFCondition TrcTractographyResults::loadDataset(
  DcmDataset& dataset,
  TrcTractographyResults*& tract)
{
  DcmXfer xfer = dataset.getOriginalXfer();
  // If the original transfer is encapsulated and we do not already have an uncompressed version, decompress or reject the file
  if (xfer.isEncapsulated())
  {
    DCMTRACT_ERROR("Cannot load dataset since it is compressed, transfer syntax: " << xfer.getXferName());
    return IOD_EC_CannotDecompress;
  }

  tract = new TrcTractographyResults();
  if (tract == NULL)
  {
    return EC_MemoryExhausted;
  }

  return tract->read(dataset);
}


OFCondition TrcTractographyResults::create(const ContentIdentificationMacro& contentIdentification,
                                           const OFString& contentDate,
                                           const OFString& contentTime,
                                           const IODEnhGeneralEquipmentModule::EquipmentInfo& equipment,
                                           const IODReferences& imageReferences,
                                           TrcTractographyResults*& result)
{
  result = new TrcTractographyResults();
  if (!result)
    return EC_MemoryExhausted;

  OFCondition cond;
  result->m_TractographyResultsModule.getContentIdentification() = contentIdentification;
  cond = result->m_TractographyResultsModule.getContentIdentification().check();
  if (cond.bad())
  {
    return cond;
  }

  OFVector<IODReference*>::const_iterator it = imageReferences.get().begin();
  while ( cond.good() && (it != imageReferences.get().end()) )
  {
    cond = result->m_TractographyResultsModule.addImageReference(**it);
    it++;
  }
  if (cond.good())
  {
    if (result->getCommonInstanceReference().addReferences(imageReferences, "") != imageReferences.size())
    {
      cond = IOD_EC_InvalidReference;
    }
  }
  if (cond.good())
    result->m_References = imageReferences;
  if (cond.good()) cond = result->m_TractographyResultsModule.setContentDate(contentDate);
  if (cond.good()) cond = result->m_TractographyResultsModule.setContentTime(contentTime);
  if (cond.good()) cond = result->m_EnhancedGeneralEquipmentModule.set(equipment);

  if (cond.bad())
  {
    delete result;
    result = NULL;
  }
  return cond;
}


OFCondition TrcTractographyResults::addTrackSet(const OFString& trackSetLabel,
                                                const OFString& trackSetDescription,
                                                const CodeWithModifiers& anatomyCode,
                                                const CodeSequenceMacro& diffusionModelCode,
                                                const CodeSequenceMacro& algoIdentCode,
                                                TrcTrackSet*& trackSet)
{
  OFCondition result = TrcTrackSet::create(trackSetLabel, trackSetDescription, anatomyCode, diffusionModelCode, algoIdentCode, trackSet);
  if (result.good())
  {
    result = trackSet->getData().putAndInsertUint32(DCM_TrackSetNumber, OFstatic_cast(Uint32, m_TractographyResultsModule.getTrackSets().size() + 1));
    if (result.good())
    {
      m_TractographyResultsModule.getTrackSets().push_back(trackSet);
    }
    else
    {
      delete trackSet;
      trackSet = NULL;
    }
  }
  return result;
}


OFCondition TrcTractographyResults::read(DcmItem &dataset)
{

  OFString sopClass;
  if (DcmIODUtil::checkSOPClass(&dataset, UID_TractographyResultsStorage, sopClass).bad())
  {
    DCMTRACT_ERROR("Given file does not seem to be a tractography results storage object since SOP class is: " << sopClass);
    return IOD_EC_WrongSOPClass;
  }

  // Read attributes in base classes
  DcmIODCommon::read(dataset);

  // Read modules supported by this class
  m_EnhancedGeneralEquipmentModule.read(dataset);
  return m_TractographyResultsModule.read(dataset);
}


OFCondition TrcTractographyResults::write(DcmItem &dataset)
{
  if (!check())
  {
    return IOD_EC_InvalidObject;
  }

  OFCondition result;

  // -- Set constant default values written by external modules --
  getSOPCommon().setSOPClassUID(UID_TractographyResultsStorage);

  // -- Extra Series level data --

  // Tractography results always set Modality to "MR"
  if (result.good()) result = getSeries().setModality("MR");

  if (result.good()) result = m_EnhancedGeneralEquipmentModule.write(dataset);

  // -- Extra Instance level data --
  if (result.good()) result = m_TractographyResultsModule.write(dataset);

  // -- Write common multi frame image IOD attributes --

  // Patient Module
  // General Study Module
  // General Series Module
  // Frame of Reference Module
  // General Equipment Module
  // SOP Common Module
  // Common Instance Reference Module
  if (result.good()) result = DcmIODCommon::write(dataset);

  return result;
}


OFCondition TrcTractographyResults::importPatientStudyFoR(
  const OFString& filename,
  const OFBool usePatient,
  const OFBool useStudy,
  const OFBool useSeries,
  const OFBool useFoR,
  const OFBool updateCommonInstanceReferences)
{
  DcmFileFormat dcmff;
  OFCondition result = dcmff.loadFile(filename.c_str());
  if ( result.good() )
  {
    DcmDataset *dset = dcmff.getDataset();
    if (dset != NULL)
    {
      result = import(*dset, usePatient, useStudy, useSeries, useFoR);
      if (result.good() && updateCommonInstanceReferences)
      {
        getCommonInstanceReference().clearData();
        getCommonInstanceReference().addReferences(m_References, "");
      }
    }
    else
    {
      DCMTRACT_ERROR("Unable to get dataset from file for copying patient, study, series and/or frame of reference information");
      result = EC_IllegalCall;
    }
  }
  return result;
}


ContentIdentificationMacro& TrcTractographyResults::getContentIdentification()
{
  return m_TractographyResultsModule.getContentIdentification();
}


OFCondition TrcTractographyResults::saveFile(
  const OFString& filename,
  const E_TransferSyntax writeXfer)
{
  if ( (writeXfer != EXS_LittleEndianExplicit) &&
       (writeXfer != EXS_BigEndianExplicit) &&
       (writeXfer != EXS_LittleEndianImplicit))
  {
    DcmXfer ts(writeXfer);
    DCMTRACT_ERROR("Cannot write transfer syntax: " << ts.getXferName() << " (can only write uncompressed)");
    return EC_CannotChangeRepresentation;
  }
  DcmFileFormat dcmff;
  OFCondition result = write( *(dcmff.getDataset()) );
  if (result.good())
  {
    result = dcmff.saveFile(filename.c_str(), writeXfer);
  }
  if (result.bad())
  {
    DCMTRACT_ERROR("Cannot save tractography results object to file " << filename << ": " << result.text());
  }

  return result;
}


OFVector< TrcTrackSet* >& TrcTractographyResults::getTrackSets()
{
  return m_TractographyResultsModule.getTrackSets();
}


size_t TrcTractographyResults::getNumberOfTrackSets()
{
  return m_TractographyResultsModule.getTrackSets().size();
}


OFCondition TrcTractographyResults::writeDataset(DcmItem& dataset)
{
  return write(dataset);
}


OFCondition TrcTractographyResults::getContentDate(OFString& value,
                                                   const long pos)
{
  return m_TractographyResultsModule.getContentDate(value, pos);
}


OFCondition TrcTractographyResults::getContentTime(OFString& value,
                                                   const long pos)
{
  return m_TractographyResultsModule.getContentTime(value, pos);
}


IODEnhGeneralEquipmentModule& TrcTractographyResults::getEquipment()
{
  return m_EnhancedGeneralEquipmentModule;
}


IODReferences& TrcTractographyResults::getReferencedInstances()
{
  return m_TractographyResultsModule.getReferencedInstances();
}


// -- private helpers --

void TrcTractographyResults::clearData()
{
  DcmIODCommon::clearData();
  m_EnhancedGeneralEquipmentModule.clearData();
  m_TractographyResultsModule.clearData();
  m_References.clearData();
}


OFCondition TrcTractographyResults::loadFile(
  DcmFileFormat& dcmff,
  const OFString& filename,
  DcmDataset*& dset)
{
  dset = NULL;
  OFCondition result = dcmff.loadFile(filename.c_str());
  if (result.bad())
  {
    DCMTRACT_ERROR("Could not load file " << filename << ": " << result.text());
    return result;
  }
  dset = dcmff.getDataset();
  if (dset == NULL)
  {
    DCMTRACT_ERROR("Could not load file " << filename << ": No dataset");
    return IOD_EC_InvalidObject;
  }
  return result;
}


OFBool TrcTractographyResults::check()
{
  if (m_TractographyResultsModule.check().good())
  {
    return OFTrue;
  }
  return OFFalse;
}
