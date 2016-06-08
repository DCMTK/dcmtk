/*
 *
 *  Copyright (C) 2015-2016, Open Connections GmbH
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
 *  Purpose: Class for managing the Common Instance Reference Module
 *
 */

#ifndef MODCOMMONINSTANCEREF_H
#define MODCOMMONINSTANCEREF_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmiod/iodreferences.h"


/** Class representing the Common Instance Reference Module:
 *
 * Referenced Series Sequence: (SQ, VM 1-n, Type 1C)
 * > Series Instance UID: (UI, 1, 1)
 * > Referenced Instance Sequence: (SQ, 1-n, 1)
 * >> SOP Instance Reference Macro
 * Studies Containing Other Referenced Instances Sequence: (SQ, 1-n, 1C)
 * > Study Instance UID (UI, 1, 1)
 * > Series and Instance Reference Macro
 *
 */
class DCMTK_DCMIOD_EXPORT IODCommonInstanceReferenceModule : public IODModule
{

public:

  // Forward declaration (class defined later in this file)
  class StudiesOtherInstancesItem;

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODCommonInstanceReferenceModule(OFshared_ptr<DcmItem> item,
                                   OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODCommonInstanceReferenceModule();

  /** Clears all data belonging to this module (rules are kept)
   */
  virtual void clearData();

  /** Destructor
   */
  virtual ~IODCommonInstanceReferenceModule();

  /** Add references
   *  @result EC_Normal if successful, error otherwise
   */
  virtual size_t addReferences(const IODReferences& references,
                               const OFString& studyInstanceUID,
                               const OFBool clearOldData = OFTrue);


  /** Read data of this module from given source item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data is cleared before reading, otherwise
   *          it is overwriten/amended.
   *  @result EC_Normal if successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write data of this module into given destination item
   *  @param  destination The item to write to
   *  @result EC_Normal if successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module ("CommonInstanceReferenceModule")
   *  @return Name of the module ("CommonInstanceReferenceModule")
   */
  virtual OFString getName() const;

  /** Return reference to list of Referenced Series items
   *  @return Reference to list of Reference Series Items
   */
  OFVector<IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem*>& getReferencedSeriesItems();

  /** Return reference to content of Studies Containing Other Referenced Instances Sequence
   *  @return Reference to content of Studies Containing Other Referenced Instances Sequence
   */
  OFVector<StudiesOtherInstancesItem*>& getStudiesContainingOtherReferences();

protected:

  virtual OFCondition addSeriesReference(
    OFVector<IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem*>& container,
    const IODReference& ref);

private:

  /// Vector with all items of the Referenced Series Sequence
  OFVector<IODSeriesAndInstanceReferenceMacro::ReferencedSeriesItem*> m_ReferenceSeriesItems;

  /// Name of this component ("CommonInstanceReferenceModule")
  static const OFString m_ComponentName;

  /// Items of Studies Containing Other Referenced Instances Sequence
  OFVector<StudiesOtherInstancesItem*> m_StudiesContainingOtherReferencedInstancesSequence;
};


/** Class representing items from the Studies Containing Other Referenced Instances Sequence,
 *  as used within the Common Instance Reference Module
 *
 * Studies Containing Other Referenced Instances Sequence: (SQ, 1-n, 1C)
 * > Study Instance UID (UI, 1, 1)
 * > Series and Instance Reference Macro
 *
 */
class DCMTK_DCMIOD_EXPORT IODCommonInstanceReferenceModule::StudiesOtherInstancesItem : public IODComponent
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  StudiesOtherInstancesItem(OFshared_ptr<DcmItem> item,
                            OFshared_ptr<IODRules> rules,
                            IODComponent* parent = NULL);

  /** Constructor
   *  @param  parent The parent component of this module, might be NULL
   */
  StudiesOtherInstancesItem(IODComponent* parent = NULL);

  /** Destructor
   */
  virtual ~StudiesOtherInstancesItem();

  /** Clear (removes) all attributes handled by the modules of this component.
   *  Rules are not reset.
   */
  virtual void clearData();

  /** Read data from source item into this module
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data is cleared first, otherwise it is
   *          overwritten/amended
   *  @result EC_Normal if successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write this module's data into given destination item
   *  @param  destination Item to write to
   *  @result EC_Normal if successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of module ("StudiesContainingOtherReferencedInstancesSequence")
   *  @return Name of the module ("StudiesContainingOtherReferencedInstancesSequence")
   */
  virtual OFString getName() const;

  /** Get Study Instance UID
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getStudyInstanceUID(OFString &value,
                                          const signed long pos = 0) const;

  /** Set Study Instance UID
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (PN) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setStudyInstanceUID(const OFString& value,
                                          const OFBool checkValue = OFTrue);


  /** Get Series And Instance Reference Macro
  *  @return Reference to the Series And Instance Reference Macro structure
  */
  virtual IODSeriesAndInstanceReferenceMacro& getReferencedSeriesAndInstanceReferences();

private:

  /// The name of this component ("StudiesContainingOtherReferencedInstancesSequence")
  static const OFString m_ComponentName;

  /// The Series and Instance Reference Macro used in this item
  IODSeriesAndInstanceReferenceMacro m_ReferencedSeriesAndInstance;
};


#endif // MODCOMMONINSTANCEREF_H
