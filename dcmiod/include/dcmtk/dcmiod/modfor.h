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
 *  Purpose: Class for managing the Frame of Reference Module
 *
 */

#ifndef MODFOR_H
#define MODFOR_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing the Frame of Reference Module:
 *
 *  Frame of Reference: (UI, 1, 1)
 *  Position Reference Indicator: (LO, 2, 1)
 *
 */
class DCMTK_DCMIOD_EXPORT IODFoRModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODFoRModule(OFshared_ptr<DcmItem> item,
               OFshared_ptr<IODRules> rules);
  /** Constructor
   */
  IODFoRModule();

  /** Destructor
   */
  virtual ~IODFoRModule();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module
   *  @return Name of the module ("FrameOfReferenceModule")
   */
  virtual OFString getName() const;

  /** Get Frame of Reference UID
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getFrameOfReferenceUID(OFString &value,
                                             const signed long pos = 0) const;

  /** Get Position Reference Indicator
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getPositionReferenceIndicator(OFString &value,
                                                    const signed long pos = 0) const;

  /** Set Frame of Reference UID
   *  @param  value Value to be set (single value only).  If an empty string is passed,
   *          the value "1" is set when displaying or writing the document since
   *          the corresponding DICOM attribute is mandatory.
   *  @param  checkValue Check 'value' for conformance with VR (UI) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setFrameOfReferenceUID(const OFString &value,
                                             const OFBool checkValue = OFTrue);

  /** Set Position Reference Indicator
   *  @param  value Value to be set (single value only).  If an empty string is passed,
   *          the value "1" is set when displaying or writing the document since
   *          the corresponding DICOM attribute is mandatory.
   *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setPositionReferenceIndicator(const OFString &value,
                                                    const OFBool checkValue = OFTrue);


private:

  /// Module name "FrameOfReferenceModule"
  static const OFString m_ModuleName;

};

#endif // MODFOR_H
