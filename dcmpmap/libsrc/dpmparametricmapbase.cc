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
 *  Module: dcmpmap
 *
 *  Author: Jan Schlamelcher
 *
 *  Purpose: Generated base class representing the Parametric Map IOD
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmpmap/dpmtypes.h"
#include "dcmtk/dcmpmap/dpmparametricmapbase.h"
#include "dcmtk/dcmdata/dcuid.h"

DPMParametricMapBase::DPMParametricMapBase()
: DPMParametricMapBase::IODImage()
, m_FGInterface()
, m_DPMParametricMapSeriesModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_IODEnhGeneralEquipmentModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_DPMParametricMapImageModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_IODMultiFrameFGModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_IODMultiframeDimensionModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_IODAcquisitionContextModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_IODCommonInstanceReferenceModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
{
}


template<typename ImagePixel>
DPMParametricMapBase::DPMParametricMapBase(OFin_place_type_t(ImagePixel))
: DPMParametricMapBase::IODImage(OFin_place<ImagePixel>)
, m_FGInterface()
, m_DPMParametricMapSeriesModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_IODEnhGeneralEquipmentModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_DPMParametricMapImageModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_IODMultiFrameFGModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_IODMultiframeDimensionModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_IODAcquisitionContextModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
, m_IODCommonInstanceReferenceModule(DPMParametricMapBase::IODImage::getData(), DPMParametricMapBase::IODImage::getRules())
{
}


DPMParametricMapBase::~DPMParametricMapBase()
{
}


void DPMParametricMapBase::clearData()
{
  DPMParametricMapBase::IODImage::clearData();
}


OFCondition DPMParametricMapBase::read(DcmItem& dataset)
{
  OFString sopClass;
  if(DcmIODUtil::checkSOPClass(&dataset, UID_ParametricMapStorage, sopClass).bad())
  {
    DCMPMAP_ERROR("Given file does not seem to be a Parametric Map storage object since SOP class is: " << sopClass);
    return IOD_EC_WrongSOPClass;
  }

  DPMParametricMapBase::IODImage::read(dataset);
  m_DPMParametricMapSeriesModule.read(dataset);
  m_IODEnhGeneralEquipmentModule.read(dataset);
  m_DPMParametricMapImageModule.read(dataset);
  m_IODMultiFrameFGModule.read(dataset);
  m_FGInterface.read(dataset);
  m_IODMultiframeDimensionModule.read(dataset);
  m_IODAcquisitionContextModule.read(dataset);
  m_IODCommonInstanceReferenceModule.read(dataset);

  return EC_Normal;
}


OFCondition DPMParametricMapBase::saveFile(const OFString& filename,
                                           const E_TransferSyntax writeXfer)
{
  DcmFileFormat dcmff;
  OFCondition result;
  if((result = write(*dcmff.getDataset())).good())
    result = dcmff.saveFile(filename.c_str(), writeXfer);
  if(result.bad())
    DCMPMAP_ERROR("Cannot save Parametric Map object to file " << filename << ": " << result.text());
  return result;
}


OFCondition DPMParametricMapBase::writeDataset(DcmItem& dataset)
{
  return write(dataset);
}


OFCondition DPMParametricMapBase::write(DcmItem& dataset)
{
  if(!check())
    return IOD_EC_InvalidObject;

  OFCondition result;
  if((result = m_DPMParametricMapSeriesModule.write(dataset)).good())
  if((result = m_IODEnhGeneralEquipmentModule.write(dataset)).good())
  if((result = m_DPMParametricMapImageModule.write(dataset)).good())
  if((result = m_IODMultiFrameFGModule.write(dataset)).good())
  if((result = m_FGInterface.write(dataset)).good())
  if((result = m_IODMultiframeDimensionModule.write(dataset)).good())
  if((result = m_IODAcquisitionContextModule.write(dataset)).good())
  if((result = m_IODCommonInstanceReferenceModule.write(dataset)).good())
    return DPMParametricMapBase::IODImage::write(dataset);

  return result;
}


OFBool DPMParametricMapBase::check()
{
  return m_FGInterface.check();
}


FGInterface& DPMParametricMapBase::getFunctionalGroups()
{
  return m_FGInterface;
}


IODPatientModule& DPMParametricMapBase::getIODPatientModule()
{
  return DPMParametricMapBase::IODImage::getPatient();
}


IODGeneralStudyModule& DPMParametricMapBase::getIODGeneralStudyModule()
{
  return DPMParametricMapBase::IODImage::getStudy();
}


IODPatientStudyModule& DPMParametricMapBase::getIODPatientStudyModule()
{
  return DPMParametricMapBase::IODImage::getPatientStudy();
}


IODGeneralSeriesModule& DPMParametricMapBase::getIODGeneralSeriesModule()
{
  return DPMParametricMapBase::IODImage::getSeries();
}


DPMParametricMapSeriesModule& DPMParametricMapBase::getDPMParametricMapSeriesModule()
{
  return m_DPMParametricMapSeriesModule;
}


IODGeneralEquipmentModule& DPMParametricMapBase::getIODGeneralEquipmentModule()
{
  return DPMParametricMapBase::IODImage::getEquipment();
}


IODEnhGeneralEquipmentModule& DPMParametricMapBase::getIODEnhGeneralEquipmentModule()
{
  return m_IODEnhGeneralEquipmentModule;
}


IODGeneralImageModule& DPMParametricMapBase::getIODGeneralImageModule()
{
  return DPMParametricMapBase::IODImage::getGeneralImage();
}


DPMParametricMapImageModule& DPMParametricMapBase::getDPMParametricMapImageModule()
{
  return m_DPMParametricMapImageModule;
}


IODMultiFrameFGModule& DPMParametricMapBase::getIODMultiFrameFGModule()
{
  return m_IODMultiFrameFGModule;
}


IODMultiframeDimensionModule& DPMParametricMapBase::getIODMultiframeDimensionModule()
{
  return m_IODMultiframeDimensionModule;
}


IODAcquisitionContextModule& DPMParametricMapBase::getIODAcquisitionContextModule()
{
  return m_IODAcquisitionContextModule;
}


IODCommonInstanceReferenceModule& DPMParametricMapBase::getIODCommonInstanceReferenceModule()
{
  return m_IODCommonInstanceReferenceModule;
}


IODSOPCommonModule& DPMParametricMapBase::getIODSOPCommonModule()
{
  return DPMParametricMapBase::IODImage::getSOPCommon();
}

#ifdef __SUNPRO_CC
// the SunPro compiler would complain that the DCMTK_DCMPMAP_EXPORT attribute cannot be applied
// to an explicit template instantiation.
template DPMParametricMapBase::DPMParametricMapBase(OFin_place_type_t(IODImagePixelModule<Uint16>));
template DPMParametricMapBase::DPMParametricMapBase(OFin_place_type_t(IODImagePixelModule<Sint16>));
template DPMParametricMapBase::DPMParametricMapBase(OFin_place_type_t(IODFloatingPointImagePixelModule));
template DPMParametricMapBase::DPMParametricMapBase(OFin_place_type_t(IODDoubleFloatingPointImagePixelModule));
#else
template DCMTK_DCMPMAP_EXPORT DPMParametricMapBase::DPMParametricMapBase(OFin_place_type_t(IODImagePixelModule<Uint16>));
template DCMTK_DCMPMAP_EXPORT DPMParametricMapBase::DPMParametricMapBase(OFin_place_type_t(IODImagePixelModule<Sint16>));
template DCMTK_DCMPMAP_EXPORT DPMParametricMapBase::DPMParametricMapBase(OFin_place_type_t(IODFloatingPointImagePixelModule));
template DCMTK_DCMPMAP_EXPORT DPMParametricMapBase::DPMParametricMapBase(OFin_place_type_t(IODDoubleFloatingPointImagePixelModule));
#endif
