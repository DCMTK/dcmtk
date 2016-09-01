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
 *  Module: dcmpmap
 *
 *  Author: Jan Schlamelcher
 *
 *  Purpose: Class for managing the Parametric Map Series Module
 *
 */

#ifndef DPMMODPARAMETRICMAPSERIES_H
#define DPMMODPARAMETRICMAPSERIES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmpmap/dpmdef.h"
#include "dcmtk/dcmiod/iodmacro.h"

/** Class representing the Parametric Map Series Module:
 *
 *  Modality: (CS, 1, 1)
 *  SeriesNumber: (IS, 1, 1)
 *  ReferencedPerformedProcedureStepSequence: (SQ, 1, 1C)
 */
class DCMTK_DCMPMAP_EXPORT DPMParametricMapSeriesModule : public IODModule
{
public:
  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  DPMParametricMapSeriesModule(OFshared_ptr<DcmItem> item,
                               OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  DPMParametricMapSeriesModule();

  /** Destructor
   */
  virtual ~DPMParametricMapSeriesModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module ("DPMParametricMapSeriesModule")
   *  @return Name of the module ("DPMParametricMapSeriesModule")
   */
  virtual OFString getName() const;

  /** Read attributes from given item into this class
   *  @param source  The source to read from
   *  @param clearOldData If OFTrue, old data is cleared before reading. Otherwise
   *         old data is overwritten (or amended)
   *  @result EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write attributes from this class into given item
   *  @param  destination The item to write to
   *  @result EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Get Modality
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

  /** Get reference to the Referenced Performed Procedure Step Sequence
   *  @return a reference to the Referenced Performed Procedure Step Sequence
   */
  virtual SOPInstanceReferenceMacro* getReferencedPerformedProcedureStepSequence();

  /** Get a reference to the entire Referenced Performed Procedure Step Sequence, including items
   *  exceeding the value multiplicity restriction of "1"
   *  @return a reference to the entire Referenced Performed Procedure Step Sequence
   */
  virtual OFVector<SOPInstanceReferenceMacro>& getEntireReferencedPerformedProcedureStepSequence();

  /** Set Modality
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setModality(const OFString &value,
                                  const OFBool checkValue = OFTrue);

  /** Set Series Number
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (IS) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setSeriesNumber(const OFString &value,
                                      const OFBool checkValue = OFTrue);

private:
  /// The name of this module ("DPMParametricMapSeriesModule")
  static const OFString m_ModuleName;

  /// ReferencedPerformedProcedureStepSequence
  OFVector<SOPInstanceReferenceMacro> m_ReferencedPerformedProcedureStepSequence;
};

#endif // DPMMODPARAMETRICMAPSERIES_H
