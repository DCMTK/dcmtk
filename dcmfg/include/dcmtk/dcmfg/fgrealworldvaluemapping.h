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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing Real World Value Mapping Functional Group
 *
 */

#ifndef FGREALWORLDVALUEMAPPING_H
#define FGREALWORLDVALUEMAPPING_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmiod/iodmacro.h"

/** Class representing the Real World Value Mapping Functional Group that
 *  specifies the mapping of stored values to associated Real World values
 */
class DCMTK_DCMFG_EXPORT FGRealWorldValueMapping: public FGBase
{
public:

  // Forward declaration
  class RWVMItem;

  /** Constructor, creates empty functional group
   */
  FGRealWorldValueMapping();

  /** Destructor, frees memory
   */
  virtual ~FGRealWorldValueMapping();

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Returns that this functional group can be used as both, shared
   *  and per-frame
   *  @returns Always returns DcmFGTypes::EFGS_BOTH
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  /** Clear all data
   */
  virtual void clearData();

  /** Check whether data of functional group is valid
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Read functional group from item, must contain the Real World Value Mapping
   *  Sequence
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Writes this functional group to item, i.e.\ will write the Real World Value
   *  Mapping Sequence
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  // --- get() functionality ---

  /** Return references to the various items inside the Real World Value Mapping
   *  Sequence
   *  @return Reference to Real World Value Mapping Sequence items
   */
  virtual OFVector< RWVMItem* >& getRealWorldValueMapping();

  /** Comparison operator that compares the normalized value of this object
   *  with a given object of the same type, i.e.\ the elements within both
   *  functional groups (this and rhs parameter) are compared by value!
   *  Both objects (this and rhs) need to have the same type (i.e.\ both
   *  FGUnknown) to be comparable. This function is used in order
   *  to decide whether a functional group already exists, or is new. This
   *  is used in particular to find out whether a given functional group
   *  can be shared (i.e.\ the same information already exists as shared
   *  functional group) or is different from the same shared group. In that
   *  case the shared functional group must be distributed into per-frame
   *  functional groups, instead. The exact implementation for implementing
   *  the comparison is not relevant. However, it must be a comparison
   *  by value.
   *  @param  rhs the right hand side of the comparison
   *  @return 0 if the object values are equal.
   *          -1 if either the value of the first component that does not match
   *          is lower in the this object, or all compared components match
   *          but this component is shorter. Also returned if this type and
   *          rhs type (DcmFGTypes::E_FGType) do not match.
   *          1 if either the value of the first component that does not match
   *          is greater in this object, or all compared components match
   *          but this component is longer.
   */
  virtual int compare(const FGBase& rhs) const;

private:

  /* Content of Real World Value Mapping Macro */

  /// Items (1-n) of Real World Value Mapping Sequence
  OFVector<RWVMItem*> m_Items;
};

/** Class representing the Real World Value Mapping Item Macro:
 * Attribute name: (VR, VM, Requirement Type)
 * Real World Value First Value Mapped: (US or SS, 1, 1)
 * Real World Value Last Value Mapped: (US or SS, 1, 1)
 * Real World Value Intercept: (FD, 1, 1C)
 * Real World Value Slope: (FD, 1, 1C)
 * Real World Value LUT Data (FD, 1-n, 1C)
 * LUT Explanation (LO, 1, 1)
 * LUT Label (SH, 1, 1)
 * Measurement Units Code Sequence (SQ, 1, 1)
 * Not yet supported: Quantity Definition Sequence (SQ, 1, 3)
 */
class DCMTK_DCMFG_EXPORT FGRealWorldValueMapping::RWVMItem : public IODComponent
{
public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  RWVMItem(OFshared_ptr<DcmItem> item,
           OFshared_ptr<IODRules> rules,
           IODComponent* parent = NULL);

  /** Constructor
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  RWVMItem(IODComponent* parent = NULL);

  /** Clone this class (perform deep copy)
   *  @return Clone of this class or NULL (e.g.\ if memory exhausted)
   */
  RWVMItem* clone();

  /** Virtual destructor, frees memory
   */
  virtual ~RWVMItem();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of component
   *  @return Name of the module
   */
  virtual OFString getName() const;

  /** Comparison operator that compares the normalized value of this object
   *  with a given object of the same type, i.e.\ the elements within both
   *  functional groups (this and rhs parameter) are compared by value!
   *  Both objects (this and rhs) need to have the same type (i.e.\ both
   *  FGUnknown) to be comparable. This function is used in order
   *  to decide whether a functional group already exists, or is new. This
   *  is used in particular to find out whether a given functional group
   *  can be shared (i.e.\ the same information already exists as shared
   *  functional group) or is different from the same shared group. In that
   *  case the shared functional group must be distributed into per-frame
   *  functional groups, instead. The exact implementation for implementing
   *  the comparison is not relevant. However, it must be a comparison
   *  by value.
   *  @param  rhs the right hand side of the comparison
   *  @return 0 if the object values are equal.
   *          -1 if either the value of the first component that does not match
   *          is lower in the this object, or all compared components match
   *          but this component is shorter. Also returned if this type and
   *          rhs type (DcmFGTypes::E_FGType) do not match.
   *          1 if either the value of the first component that does not match
   *          is greater in this object, or all compared components match
   *          but this component is longer.
   */
  virtual int compare(const IODComponent& rhs) const;

  /** Read attributes from given item into this class
   *  @param source  The source to read from
   *  @param clearOldData If OFTrue, old data is cleared before reading.
   *         Otherwise old data is overwritten (or amended)
   *  @result EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write attributes from this class into given item
   *  @param  destination The item to write to
   *  @result EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  // --- get() functionality ---

  /** Get Real World Value First Value Mapped
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRealWorldValueFirstValueMapped(Sint32& value,
                                                        const unsigned long pos = 0) const;

  /** Get Real World Value First Value Mapped
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRealWorldValueLastValueMapped(Sint32 &value,
                                                       const unsigned long pos = 0) const;

  /** Get Real World Value LUT Data
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRealWorldValueLUTData(Float64 &value,
                                               const unsigned long pos = 0) const;

  /** Get Real World Value LUT Data
   *  @param  values Reference to variable in which the values should be stored
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRealWorldValueLUTData(OFVector<Float64>& values) const;

  /** Get LUT Explanation
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getLUTExplanation(OFString &value,
                                        const signed long pos = 0) const;

  /** Get LUT Label
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getLUTLabel(OFString &value,
                                  const signed long pos = 0) const;

  /** Get Measurement Units Code Sequence (content)
   *  @return Reference to Measurement Units Code
   */
  virtual CodeSequenceMacro& getMeasurementUnitsCode();

  // --- set() functionality ---

  /** Set Real World Value First Value Mapped
   *  @param  value Value to be set. If Pixel Representation is 0 (unsigned
   *          pixel data) value  must be 0 < value < 2^16. Otherwise use
   *          setRealWorldValueFirstValueMappedSigned().
   *  @param  checkValue Check 'value' for conformance with VR (US and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRealWorldValueFirstValueMappedUnsigned(const Uint16 &value,
                                                                const OFBool checkValue = OFTrue);

  /** Set Real World Value First Value Mapped
   *  @param  value Value to be set. If Pixel Representation is -1 (signed
   *          pixel data) value  must be -2^15  < value < 2^15-1. Otherwise use
   *          setRealWorldValueFirstValueMappedUnSigned().
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRealWorldValueFirstValueMappeSigned(const Sint16 &value,
                                                             const OFBool checkValue = OFTrue);

  /** Set Real World Value Last Value Mapped
   *  @param  value Value to be set. If Pixel Representation is 0 (unsigned
   *          pixel data) value  must be 0 < value < 2^16. Otherwise use
   *          setRealWorldValueLastValueMappedSigned().
   *  @param  checkValue Check 'value' for conformance with VR (US and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRealWorldValueLastValueMappedUnsigned(const Uint16 &value,
                                                               const OFBool checkValue = OFTrue);

  /** Set Real World Value Last Value Mapped
   *  @param  value Value to be set. If Pixel Representation is -1 (signed
   *          pixel data) value  must be -2^15  < value < 2^15-1. Otherwise use
   *          setRealWorldValueLastValueMappedUnSigned().
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRealWorldValueLastValueMappedSigned(const Sint16 &value,
                                                        const OFBool checkValue = OFTrue);

  /** Set Real World Value Intercept
   *  @param  value Value to be set
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRealWorldValueIntercept(const Float64& value,
                                                 const OFBool checkValue = OFTrue);

  /** Set Real World Value Slope
   *  @param  value Value to be set
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRealWorldValueSlope(const Float64& value,
                                             const OFBool checkValue = OFTrue);

  /** Set Real World Value LUT Data
   *  @param  value Values to be set
   *  @param  checkValue Check 'value' for conformance with VR (FD) and VM (1-n) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRealWorldValueLUTData(const OFVector<Float64>& value,
                                               const OFBool checkValue = OFTrue);

  /** Set LUT Explanation
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLUTExplanation(const OFString &value,
                                        const OFBool checkValue = OFTrue);

  /** Set LUT Label
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance with VR (SH) and VM (1) if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLUTLabel(const OFString &value,
                                  const OFBool checkValue = OFTrue);

protected:

  static OFCondition getUSorSS(DcmItem& item,
                               const DcmTagKey& key,
                               const unsigned long pos,
                               Sint32& value);

private:

  /// The name of this module ("RealWorldValueMappingItemMacro")
  static const OFString m_ModuleName;

  /// Measurement Units Code Sequence
  CodeSequenceMacro m_MeasurementUnitsCode;

  /// TODO Content Item Macro for Quantity Definition Sequence
};

#endif // FGREALWORLDVALUEMAPPING_H
