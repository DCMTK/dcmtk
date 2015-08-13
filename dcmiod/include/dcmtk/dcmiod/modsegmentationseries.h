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
 *  Purpose: Class for managing the Segmentation Series Module
 *
 */

#ifndef MODSEGMENTATIONSERIES_H
#define MODSEGMENTATIONSERIES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing the Segmentation Series Module:
 *
 *  Modality: (CS, 1, 1)
 *  Series Number: (IS, 1, 2)
 *  ReferencedPerformedProcedureStepSequence: (SQ, 1, 1C)
 *  -> Referenced SOP Class UID: (UI, 1, 1)
 *  -> Referenced SOP Instance UID: (UI, 1, 1)
 */
class DCMTK_DCMIOD_EXPORT IODSegmentationSeriesModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODSegmentationSeriesModule(OFshared_ptr<DcmItem> item,
                              OFshared_ptr<IODRules> rules);
  /** Constructor
   */
  IODSegmentationSeriesModule();

  /** Destructor
   */
  virtual ~IODSegmentationSeriesModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of this module
   *  @return Name of the module ("SegmentationSeriesModule")
   */
  virtual OFString getName() const;

   /** Get Modality (always returns "SEG")
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getModality(OFString &value,
                                  const signed long pos = 0) const;

   /** Get Series Number
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getSeriesNumber(OFString &value,
                                      const signed long pos = 0) const;

   /** Get Referenced PPS SOP Class UID
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getPPSSOPClassUID(OFString &value,
                                        const signed long pos = 0) const;

   /** Get Referenced PPS SOP Instance UID
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getPPSSOPInstanceUID(OFString &value,
                                           const signed long pos = 0) const;

  /** Set Series Number
   *  @param  value Value to be set (single value only).  If an empty string is passed,
   *          the value "1" is set when displaying or writing the document since
   *          the corresponding DICOM attribute is mandatory.
   *  @param  checkValue Check 'value' for conformance with VR (IS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSeriesNumber(const OFString &value,
                                      const OFBool checkValue = OFTrue);

  /** Set Referenced PPS SOP Class UID
   *  @param  value Value to be set (single value only).  If an empty string is passed,
   *          the value "1" is set when displaying or writing the document since
   *          the corresponding DICOM attribute is mandatory.
   *  @param  checkValue Check 'value' for conformance with VR (IS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPPSSOPClassUID(const OFString &value,
                                        const OFBool checkValue = OFTrue);

  /** Set Referenced PPS SOP Instance UID
   *  @param  value Value to be set (single value only).  If an empty string is passed,
   *          the value "1" is set when displaying or writing the document since
   *          the corresponding DICOM attribute is mandatory.
   *  @param  checkValue Check 'value' for conformance with VR (IS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPPSSOPInstanceUID(const OFString &value,
                                           const OFBool checkValue = OFTrue);

private:

  /// Name of this module ("SegmentationSeriesModule")
  static const OFString m_ModuleName;

};

#endif // MODSEGMENTATIONSERIES_H
