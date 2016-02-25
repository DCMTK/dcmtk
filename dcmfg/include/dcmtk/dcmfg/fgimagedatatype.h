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
 *  Purpose: Class for managing Image Data Type Functional Group
 *
 */

#ifndef FGIMAGEDATATYPE_H
#define FGIMAGEDATATYPE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmfg/fgbase.h"

/** Class for managing Image Data Type Functional Group that identifies the
 *  data type characteristics of this frame
 */
class DCMTK_DCMFG_EXPORT FGImageDataType : public FGBase
{
public:

  /** Constructor, creates empty Image Data Type functional group
   */
  FGImageDataType();

  /** Destructor, frees memory
   */
  virtual ~FGImageDataType();

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Get potential shared type of this group, always returns
   *  DcmFGTypes::EFGS_BOTH
   *  @return Always returns DcmFGTypes::EFGS_BOTH
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  /** Clear data
   */
  virtual void clearData();

  /** Check whether current data of this functional group is valid
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Read Image Data Type Sequence (this functional group) from given item
   * @param  item Item to read from
   * @return EC_Normal if reading works, error otherwise
   */
  virtual OFCondition read(DcmItem &item);

  /** Write Image Data Type Sequence (this functional group) to given item
   * @param  item Item to write to
   * @return EC_Normal if writing works, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  // --- get() functionality ---

  /** Get Data Type
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getDataType(OFString& value,
                                  const signed long pos = 0);

  /** Get Aliased Data Type
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getAliasedDataType(OFString& value,
                                         const signed long pos = 0);

  /** Get Zero Velocity Pixel Value
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getZeroVelocityPixelValue(Sint32& value,
                                                const signed long pos = 0);

  // --- set() functionality ---

  /** Set Data Type
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setDataType(const OFString& value,
                                  const OFBool checkValue = OFTrue);

  /** Set Aliased Data Type
   *  @param  value Permitted values: "YES" or "NO".
   *  @param  checkValue If enabled, the value is checked for conformance
   *  @return EC_Normal if value was set, error code otherwise
   */
  virtual OFCondition setAliasedDataType(const OFString& value,
                                         const OFBool checkValue = OFTrue);

  /** Set Zero Velocity Pixel Value. Only use if Zero Velocity Pixel Value is of
   *  VR US, i.e.\ Pixel Representation = 0
   *  @param  value Value to be set

   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setZeroVelocityPixelValueUS(const Uint16 value);

  /** Set Zero Velocity Pixel Value. Only use if Zero Velocity Pixel Value is of
   *  VR SS, i.e.\ Pixel Representation = 1
   *  @param  value Value to be set
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setZeroVelocityPixelValueSS(const Sint16 value);

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

  /* Content of Image Data Type Macro */

  /// Data Type (CS, VM 1, Required type 1)
  DcmCodeString m_DataType;

  /// Aliased Data Type (CS, VM 1, Required Type 1)
  DcmCodeString m_AliasedDataType;

  /// Zero Velocity Pixel Value (US or SS, VM , Required Type 1C)
  DcmUnsignedShort m_ZeroVelocityPixelValueUS;

  /// Zero Velocity Pixel Value (US or SS, VM , Required Type 1C)
  DcmSignedShort m_ZeroVelocityPixelValueSS;

};

#endif // FGIMAGEDATATYPE_H
