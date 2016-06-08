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
 *  Module: dcmtract
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class representing the Tractography Results Module
 *
 */

#ifndef TRCMODTRACTRESULTS_H
#define TRCMODTRACTRESULTS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmiod/iodmacro.h"      // for Content Identification Macro
#include "dcmtk/dcmiod/iodreferences.h"
#include "dcmtk/dcmtract/trctrackset.h"


/** Class representing the Tractography Results Module
 */
class TrcTractographyResultsModule : public IODModule
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          a new set of rules and adds its own rules to it.
   */
  TrcTractographyResultsModule(OFshared_ptr<DcmItem> item,
                               OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  TrcTractographyResultsModule();

  /** Destructor
   */
  virtual ~TrcTractographyResultsModule();

  /** Read data into this module from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data in this module is cleared first
   *          (default: OFTrue)
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write this module to given item
   *  @param  destination The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Clear (removes) all attributes handled by the modules of this component.
   *  Rules are not reset.
   */
  virtual void clearData();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of macro
   *  @return Name of the module ("TractographyResultsModule")
   */
  virtual OFString getName() const;

  /** Get track sets
   *  @return Reference to track sets
   */
  virtual OFVector<TrcTrackSet*>& getTrackSets();

  /** Get Content Identification Macro
   *  @return Reference to Content Identification Macro
   */
  virtual ContentIdentificationMacro& getContentIdentification();

  /** Return references to images that contributed to the Tractography Results
   *  object. The references is populated from the content of the Referenced
   *  Instance Sequence (and will be used for populating it when writing)
   */
  virtual IODReferences& getReferencedInstances();

  /** Get Content Date
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getContentDate(OFString &value,
                                     const signed long pos = 0);

  /** Get Content Time
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getContentTime(OFString &value,
                                     const signed long pos = 0);

  /** Set Content Date
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (DA) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setContentDate(const OFString &value,
                                     const OFBool checkValue = OFTrue);

  /** Set Content Time
   *  @param  value Value to be set (single value only).
   *  @param  checkValue Check 'value' for conformance with VR (TM) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setContentTime(const OFString &value,
                                     const OFBool checkValue = OFTrue);

  /** Add reference to image which was used to create this Tractography Results
   *  object. Alternatively, getReferencedInstances() can be used.
   *  @param  imageRef The image reference to add
   *  @return EC_Normal if adding was successful, error otherwise
   */
  virtual OFCondition addImageReference(const IODReference& imageRef);

  /** Check whether this component's data satisfies the underlying
   *  rules
   *  @param  quiet If OFTrue, not error / warning messages will be produced. Only
   *                the returned error code will indicate error or OK. Per default,
   *                logging output is produced (OFFalse).
   *  @result EC_Normal if rules are satisfied, error otherwise
   */
  virtual OFCondition check(const OFBool quiet = OFFalse);

protected:

  /** Check whether color information is valid, i.e. coloring is either
   *  (for each Track Set): Per Track Set, per Track or Data Point-specific.
   *  @result EC_Normal if rules are satisfied, error otherwise
   */
  virtual OFCondition checkColoring();

private:

  /// Content Identification for this Tractography Results object
  ContentIdentificationMacro m_ContentIdentification;

  /// Content Identification for this Tractography Results object
  OFVector<TrcTrackSet*> m_TrackSets;

  /// Referenced instances, represents content of Referenced Instance Sequence
  IODReferences m_ReferencedInstances;

  /// The module's name ("TractographyResultsModule")
  static const OFString m_ModuleName;

};

#endif // TRCMODTRACTRESULTS_H
