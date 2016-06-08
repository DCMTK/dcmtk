/*
 *
 *  Copyright (C) 2016, Open Connections GmbH
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
 *  Purpose: Class representing a Tractography Results object
 *
 */

#ifndef TRCTRACTOGRAPHYRESULTS_H
#define TRCTRACTOGRAPHYRESULTS_H

#include "dcmtk/config/osconfig.h"              // include OS configuration first
#include "dcmtk/ofstd/ofvector.h"               // for OFVector
#include "dcmtk/dcmiod/iodcommn.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmiod/modenhequipment.h"       // for Enhanced General Equipment Module
#include "dcmtk/dcmtract/trctypes.h"            // for Tractography Results data types
#include "dcmtk/dcmtract/trctrackset.h"         // for Track Set
#include "dcmtk/dcmtract/trcmodtractresults.h"  // for Tractography Results Module
#include "dcmtk/dcmtract/trcdef.h"

// Forward declaration
class IODReferences;

/** Class representing an object of the "Tractography Results" object IOD.
 */
class DCMTK_DCMTRACT_EXPORT TrcTractographyResults
: public DcmIODCommon
{

public:

  // -------------------- creation ----------------------------------

  /** Create new Tractography Results object. The resulting object then can
   *  be filled by using addTrackSet().
   *  @param  contentIdentification Content identification for this object
   *  @param  contentDate The date the content creation started
   *  @param  contentTime The time the content creation started
   *  @param  equipment Equipment used to produce this object
   *  @param  imageReferences References to images that played a role when
   *          creating this object. Usually these are MR images that were used
   *          to compute the tracks in this object.
   *  @param  result Returns the created object if successful, otherwise NULL
   *  @return EC_Normal if creation was successful, error otherwise
   */
  static OFCondition create(const ContentIdentificationMacro& contentIdentification,
                            const OFString& contentDate,
                            const OFString& contentTime,
                            const IODEnhGeneralEquipmentModule::EquipmentInfo& equipment,
                            const IODReferences& imageReferences,
                            TrcTractographyResults*& result);

  /** Take over general information for Patient, Study, Series and/or Frame of Reference
   *  from existing file
   *  @param  filename The filename to read from
   *  @param  usePatient If OFTrue, Patient level information is imported
   *  @param  useStudy If OFTrue, Study level information is imported
   *  @param  useSeries If OFTrue, Series level information is imported
   *  @param  useFoR If OFTrue, Frame of Reference information is imported
   *  @param  updateCommonInstanceReferences If OFTrue (default), all
   *          references in the Common Instance Reference Module will be updated
   *          using the reference data provided earlier in the create() call. The
   *          reason is that the study and series information (including UIDs)
   *          are likely to change during this call, which requires another
   *          sorting of references inside the Common Instance Reference module.
   *  @return EC_Normal if reading was successful (i.e.\ if any information could
   *          be read), otherwise an error is returned
   */
  virtual OFCondition importPatientStudyFoR(const OFString& filename,
                                            const OFBool usePatient,
                                            const OFBool useStudy,
                                            const OFBool useSeries,
                                            const OFBool useFoR = OFFalse,
                                            const OFBool updateCommonInstanceReferences = OFTrue);

  /** Add Track Set to object. If successful, the resulting Track Set can be
   *  filled by calling addTrack() (and other methods) on it.
   *  @param  trackSetLabel Label for this Track Set with a maximum of 256
   *                        characters (VR = LO)
   *  @param  trackSetDescription Text description for this Track Set with a
   *          maximum of 2^32-2 bytes (VR = UT).
   *  @param  anatomyCode  Code describing the anatomy covered by the Track Set.
   *          DICOM proposes a code from CID 7710 "Tractography Anatomic Sites"
   *          (Baseline). Laterality can be set later on the resulting
   *          Track Set.
   *  @param  diffusionModelCode Code describing the diffusion model this Track
   *          Set is based on. DICOM proposes a code from CID 7261 "Diffusion
   *          Model Value Types"
   *  @param  algoIdentCode Code describing the algorithm identification. DICOM
   *          prescribes code from CID 7262 "Diffusion Tractography Algorithm
   *          Families".
   *  @param  trackSet Returns the resulting Track Set if successful, NULL
   *          otherwise.
   *  @return EC_Normal if creation was successful, error otherwise
   */
  virtual OFCondition addTrackSet(const OFString& trackSetLabel,
                                  const OFString& trackSetDescription,
                                  const CodeWithModifiers& anatomyCode,
                                  const CodeSequenceMacro& diffusionModelCode,
                                  const CodeSequenceMacro& algoIdentCode,
                                  TrcTrackSet*& trackSet);

  // -------------------- destruction -------------------------------

  /** Destructor, frees memory
   */
  virtual ~TrcTractographyResults();

  // -------------------- loading and saving ---------------------

  /** Load Tractography Results object from item file
   *  @param  filename The file to read from
   *  @param  tractography  The resulting Tractography Results object. NULL if
   *          dataset could not be read successfully.
   *  @return EC_Normal if reading was successful, error otherwise
   */
  static OFCondition loadFile(const OFString& filename,
                              TrcTractographyResults*& tractography);

  /** Load Tractography Results object from item object.
   *  @param  dataset The dataset to read from
   *  @param  tractography  The resulting Tractography Results object. NULL if
   *          dataset could not be read successfully.
   *  @return EC_Normal if reading was successful, error otherwise
   */
  static OFCondition loadDataset(DcmDataset& dataset,
                                 TrcTractographyResults*& tractography);

  /** Save current object to given filename
   *  @param  filename The file to write to
   *  @param  writeXfer The transfer syntax to be used
   *  @return EC_Normal if writing was successful, error otherwise.
   */
  virtual OFCondition saveFile(const OFString& filename,
                       const E_TransferSyntax writeXfer = EXS_LittleEndianExplicit);

  /** Write current object to given item
   *  @param  dataset The item to write to
   *  @return EC_Normal if writing was successful, error otherwise.
   */
  virtual OFCondition writeDataset(DcmItem& dataset);


  // -------------------- access ---------------------

  /** Get Modality (overwrite from DcmIODCommon. DICOM enforces "MR" for
   *  Tractography Result objects, and thus it is always returned.
   *  @param  value  Reference to variable in which the value should be stored
   *  @param  pos    Index of the value to get. Not evaluated (here for
   *          consistency with other setter functions).
   *  @return status, EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getModality(OFString &value,
                                  const signed long pos = 0)
  {
    (void)pos;
    value = "MR";
    return EC_Normal;
  }

  /** Get Content Date (from Tractography Results Module)
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getContentDate(OFString &value,
                                     const signed long pos = 0);

  /** Get Content Time (from Tractography Results Module)
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getContentTime(OFString &value,
                                     const signed long pos = 0);

  /** Get referenced instances (from Tractography Results Module)
   *  @return Reference to instances referenced in Referenced Instance Sequence
   */
  virtual IODReferences& getReferencedInstances();

  /** Get reference to Equipment information
   *  @return Reference to information from Enhanced General Equipment Module
   */
  virtual IODEnhGeneralEquipmentModule& getEquipment();

  /** Return reference to content identification of this Tractography Results
   *  object (from Tractography Results Module)
   *  @return Reference to content identification data
   */
  virtual ContentIdentificationMacro& getContentIdentification();

  /** Returns number of Track Sets in Tractography Results object
   *  @return Number of Track Sets in object
   */
  virtual size_t getNumberOfTrackSets();

  /** Return Track Sets (from Tractography Results Module)
   *  @return Reference to Track Sets
   */
  virtual OFVector<TrcTrackSet*>& getTrackSets();

protected:

  /** Protected default constructor. Library users should the factory create..()
   *  method in order to create an object from scratch
   */
  TrcTractographyResults();

  /** Overwrites DcmIODCommon::read()
   *  @param  dataset The dataset to read from
   *  @return EC_Normal if reading succeeded, error otherwise
   */
  virtual OFCondition read(DcmItem &dataset);

  /** Overwrites DcmIODCommon::write()
   *  @param  dataset The dataset to write to
   *  @return EC_Normal if writing succeeded, error otherwise
   */
  virtual OFCondition write(DcmItem &dataset);

  /** Initialize IOD rules
   */
  virtual void initIODRules();

private:

  // Modules supported:
  //
  // Patient Module (through DcmIODCommon)
  // Patient Study Module (through DcmIODCommon)
  // General Study Module (through DcmIODCommon)
  // General Series Module (through DcmIODCommon)
  // Tractography Results Series (through this class)
  // Frame of Reference Module (through DcmIODCommon)
  // General Equipment Module (through DcmIODCommon)
  // Enhanced General Equipment Module (through IODEnhGeneralEquipmentModule)
  // Tractography Results Module (through TrcTractographyResultsModule)
  // Common Instance Reference Module (through DcmIODCommon)
  // SOP Common Module (through DcmIODCommon)

  /// Enhanced General Equipment Module
  IODEnhGeneralEquipmentModule m_EnhancedGeneralEquipmentModule;

  /// Tractography Results Module
  TrcTractographyResultsModule m_TractographyResultsModule;

  /// References used when creating Tractography Results objects. When calling
  /// create() in order to create a Tractography Results object, the references
  /// provided in the call are used to initialize those in the Common Instance
  /// Reference Module as well as the Tractography Results module's Referenced
  /// Instance Sequence. However, in the case of Common Instance Reference, the
  /// internal reference structure is distinguishes between reference within
  /// the same study as "this" object as well as other objects outside this study.
  /// If the user calls later importPatientStudyFoR() after creation, the study
  /// is most likely to change (away from the automatically created new study when
  /// calling create() to the imported study information) which requires to
  /// re-build the Common Instance Refernence Module's references. Thus, this
  /// member is used to store store the references provided by create() for
  /// this use case.
  IODReferences m_References;

  // --------------- private helper functions -------------------

  /** Clear old data
   */
  void clearData();

  /** Check whether Tractography Results object is ready for writing
   *  @return OFTrue, if object can be written, OFFalse otherwise
   */
  OFBool check();

  /** Loads file
   *  @param  dcmff The file format to load into
   *  @param  filename The filename of the file to load
   *  @param  dset Pointer to dataset after loading
   *  @return EC_Normal if loading was successful, error otherwise
   */
  static OFCondition loadFile(DcmFileFormat& dcmff,
                              const OFString& filename,
                              DcmDataset*& dset);

  // --------------- members ------------------------------------

};

#endif // TRCTRACTOGRAPHYRESULTS_H
