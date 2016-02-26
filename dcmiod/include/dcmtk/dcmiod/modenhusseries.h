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
 *  Purpose: Class for managing the Enhanced US Series Module
 *
 */

#ifndef MODENHUSSERIES_H
#define MODENHUSSERIES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofoption.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing the Enhanced Ultrasound Series Module:
 *
 * Notation: "Attribute name: (VR, VM, Requirement Type)"
 * Modality: (CS, 1, 1)
 * Referenced Performed Procedure Step Sequence: (SQ, 1, 1C)
 * > SOP Instance Reference Macro
 * Performed Protocol Code Sequence: (SQ, 1, 1C)
 * > Code Sequence Macro
 * Not yet explicitly supported:
 * ----------------------------------------------
 * > Protocol Context Sequence: (SQ, 1-n, 3)
 * >> Content Item Macro
 * >> Content Item Modifier Sequence: (SQ, 1-n, 3)
 * >>> Content Item Macro
 * ----------------------------------------------
 * Performed Protocol Type: (CS, 1, 1C)
 */
class DCMTK_DCMIOD_EXPORT IODEnhancedUSSeriesModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODEnhancedUSSeriesModule(OFshared_ptr<DcmItem> item,
                            OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODEnhancedUSSeriesModule();

  /** Destructor
   */
  virtual ~IODEnhancedUSSeriesModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module ("EnhancedUSSeriesModule")
   *  @return Name of the module ("EnhancedUSSeriesModule")
   */
  virtual OFString getName() const;

  /** Read data into this module from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data in this module is cleared first (default: OFTrue)
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write this module to given item
   *  @param  destination The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

   /** Get Modality, always returns "US"
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getModality(OFString &value,
                                  const signed long pos = 0) const;


  /** Get Referenced Performed Procedure Step
   *  @return Reference to the Referenced Performed Procedure Step information
   */
  virtual SOPInstanceReferenceMacro& getReferencedPPS();

  /** Get Performed Protocol Code
   *  @return Reference to the Performed Protocol information
   */
  virtual CodeSequenceMacro& getPerformedProtocolCode();

  /** Get Performed Protocol Type
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPerformedProtocolType(OFString &value,
                                               const signed long pos = 0) const;

private:

  /// The name of this module ("EnhancedUSSeriesModule")
  static const OFString m_ModuleName;

  /// Referenced Performed Procedure Step Sequence
  SOPInstanceReferenceMacro m_ReferencedPerformedProcedureStep;

  /// Performed Protocol Code Sequence (so far, without optional data)
  CodeSequenceMacro m_PerformedProtocolCode;
};

#endif // MODENHUSSERIES_H
