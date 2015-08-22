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
 *  Purpose: Class representing IODs by exposing common DICOM module attributes
 *
 */

#ifndef IODCOMMN_H
#define IODCOMMN_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modpatient.h"
#include "dcmtk/dcmiod/modpatientstudy.h"
#include "dcmtk/dcmiod/modgeneralstudy.h"
#include "dcmtk/dcmiod/modequipment.h"
#include "dcmtk/dcmiod/modgeneralseries.h"
#include "dcmtk/dcmiod/modfor.h"
#include "dcmtk/dcmiod/modsopcommon.h"
#include "dcmtk/dcmiod/modcommoninstanceref.h"

/** Interface class to those modules and data attributes  are common for many
 *  DICOM IODs. The class offers a dedicated API for getting and settings those
 *  attributes. Modules included: Patient Module, General Study Module, General
 *  Equipment Module, General Series Module, Frame of Reference Module, SOP
 *  Common Module.
 */
class DCMTK_DCMIOD_EXPORT DcmIODCommon
{

public:

  /** Constructor
   */
  DcmIODCommon();

  /** Get rules handled by this IOD
   *  @return The rules
   */
  OFshared_ptr<IODRules> getRules();

 /** Get item managed by this IOD
  *  @return The item
  */
  OFshared_ptr<DcmItem> getData();

  /** Get Patient Module
   *  @return Reference to Patient Module
   */
  IODPatientModule& getPatient();

  /** Get Patient Study Module
   *  @return Reference to Patient Study Module
   */
  IODPatientStudyModule& getPatientStudy();

  /** Get General Study Module
   *  @return Reference to General Study Module
   */
  IODGeneralStudyModule& getStudy();

  /** Get General Equipment Module
   *  @return Reference to General Equipment Module
   */
  IODGeneralEquipmentModule& getEquipment();

  /** Get Series Module
   *  @return Reference to General Series Module
   */
  IODGeneralSeriesModule& getSeries();

  /** Get Frame of Reference Module
   *  @return Reference to Frame of Reference Module
   */
  IODFoRModule& getFrameOfReference();

  /** Get SOP Common Module
   *  @return Reference to SOP Common Module
   */
  IODSOPCommonModule& getSOPCommon();

  /** Get Common Instance Reference Module
   *  @return Common Instance Reference Module
   */
  IODCommonInstanceReferenceModule& getCommonInstanceReference();

  /** Destructor
    */
  virtual ~DcmIODCommon();

  /** Clear (removes) all attributes handled by the modules of this IOD.
   *  IOD Rules are not reset.
   */
  virtual void clearData();

  /** Create new study.
   *  After generating a new Study Instance UID the method createNewSeries() is
   *  called, i.e.\ also a new Series Instance UID and SOP instance UID are
   *  generated.  This is a requirement of the DICOM standard. All other
   *  study-related attributes from the study-level modules managed by this class
   *  are cleared (as well as Series and Instance level attributes).
   *  @param  clearEquipment  If OFTrue (default), also the equipment information is
   *          cleared (General Equipment Module))
   */
  virtual void createNewStudy(const OFBool clearEquipment = OFTrue);

  /** Create a new series.
   *  After generating a new Series Instance UID the method
   *  createNewSOPInstance() is called, i.e.\ Series-related attributes managed
   *  by the series-level modules of this class are cleared, as well as
   *  instance-level data.
   *  @param  clearFoR If OFTrue (default), also the frame of reference is
   *          cleared (FoR Module)
   */
  virtual void createNewSeries(const OFBool clearFoR = OFTrue);

  /** Create a new SOP instance.
   *  Generate a new SOP Instance UID and set the Instance Creation Date/Time
   *  to the current date and time.
   *  It could be used explicitly from the calling application if a new UID
   *  should be created (this is the case if the Study Instance UID or Series
   *  Instance UID has changed as well as any other attribute on the instance
   *  level, e.g.\ image pixel data). Also clears other instance related data.
   */
  virtual void createNewSOPInstance();

  /** Make sure that the IOD contains a SOP Instance, Series Instance and Study
   *  Instance UID. They are created if empty. Invalid UIDs are corrected if
   *  desired.
   */
  virtual void ensureInstanceUIDs(const OFBool correctInvalid = OFFalse);

  /** Read data into this class.
   *  Please note that the current content is also deleted if the reading
   *  process fails. If the log stream is set and valid the reason for any
   *  error might be obtained from the error/warning output. The reading
   *  process will ignore missing attributes or values as possible in order
   *  to read as as much as possible.
   *  @param  dataset DICOM dataset from which the document should be read
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition read(DcmItem &dataset);

  /** Import common module attributes from dataset but only read Patient, Study,
   *  Series and/or Frame of Reference level portions. The current content
   *  is not deleted before reading. If the log stream is set and valid the
   *  reason for any error might be obtained from the error/warning output.
   *  @param  dataset     Reference to DICOM dataset from which the document
   *          should be read
   *  @param  readPatient Read Patient-level information if OFTrue
   *  @param  readStudy   Read Study-level information if OFTrue, including
   *          equipment module
   *  @param  readFoR     Read Frame of Reference information if OFTrue. See
   *          also readSeries parameter.
   *  @param  readSeries  Read Series-level information if OFTrue, always
   *                      includes Frame of Reference, i.e.\ readFoR is
   *                      considered to be OFTrue
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition import(DcmItem& dataset,
                             OFBool readPatient,
                             OFBool readStudy,
                             OFBool readFoR = OFFalse,
                             OFBool readSeries = OFFalse);

  /** Write the attributes managed by this class to DICOM dataset.
   *  @param dataset  Reference to DICOM dataset to which the current document
   *                  should be  written. The dataset is not cleared
   *                  before writing to it.
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition write(DcmItem &dataset);


private:

  /// The data hold by this class (shared with modules below)
  OFshared_ptr<DcmItem> m_Item;

  /// The rules hold by this class (shared with modules below)
  OFshared_ptr<IODRules> m_Rules;

  /// Patient Module
  IODPatientModule m_Patient;

  /// Patient Study Module
  IODPatientStudyModule m_PatientStudy;

  /// General Study Module
  IODGeneralStudyModule m_Study;

  /// General Equipment Module
  IODGeneralEquipmentModule m_Equipment;

  /// General Series Module
  IODGeneralSeriesModule m_Series;

  /// Frame of Reference Module
  IODFoRModule m_FrameOfReference;

  /// SOP Common Module
  IODSOPCommonModule m_SOPCommon;

  /// Common Instance Reference Module
  IODCommonInstanceReferenceModule m_CommonInstanceReferenceModule;

  /// Collects all modules of this class for convenience (iteration)
  OFVector<IODModule*> m_Modules;
};

#endif // IODCOMMN_H
