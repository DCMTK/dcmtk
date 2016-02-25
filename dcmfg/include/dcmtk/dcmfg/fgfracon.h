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
 *  Purpose: Class for managing the Frame Content Functional Group
 *
 */

#ifndef FGFRACON_H
#define FGFRACON_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fgbase.h"

/** Class representing the "Frame Content" Functional Group Macro. Can only be
 *  used as per-frame functional group (never shared).
 */
class DCMTK_DCMFG_EXPORT FGFrameContent : public FGBase
{
public:

  /** Constructor, creates empty functional group
   */
  FGFrameContent();

  /** Virtual destructor
   */
  virtual ~FGFrameContent();

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Returns shared functional group type (always "per-frame")
   *  @return The functional group type (DcmFGTypes::EFGS_ONLYPERFRAME)
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_ONLYPERFRAME;}

  /** Clear all data
   */
  virtual void clearData();

  /** Check whether the current content of this group is consistent and complete
   *  @return EC_Normal, if no errors are found, error otherwise
   */
  virtual OFCondition check() const;

  /** Read Frame Content Sequence from given item
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Writes the content of this class into Frame Content Sequence
   *  (newly cerated) into given item
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

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

  // --- get() functionality ---

  /** Get Frame Acquisition Number
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *          an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getFrameAcquisitionNumber(Uint16& value,
                                                const signed long pos = 0);

  /** Get Frame Reference Date Time
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *          an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getFrameReferenceDateTime(OFString& value,
                                                const signed long pos = 0);
  /** Get Frame Acquisition Date Time
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *          an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getFrameAcquisitionDateTime(OFString& value,
                                                  const signed long pos = 0);

  /** Get Frame Acquisition Duration
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *           an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getFrameAcquisitionDuration(Float64& value,
                                                  const unsigned long pos = 0);

  /** Get Cardiac Cycle Position
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *          an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getCardiacCyclePosition(OFString& value,
                                              const signed long pos = 0);

  /** Get Respiratory Cycle Position
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *          an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getRespiratoryCyclePosition(OFString& value,
                                                  const signed long pos = 0);

  /** Get Dimension Index Value
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *          an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getDimensionIndexValues(Uint32& value,
                                              const signed long pos = 0);

  /** Get Temporal Position Index
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *          an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getTemporalPositionIndex(Uint32& value,
                                               const signed long pos = 0);

  /** Get Stack ID
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *          an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getStackID(OFString& value,
                                 const signed long pos = 0);

  /** Get In-Stack Position Number
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *          an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getInStackPositionNumber(Uint32& value,
                                               const signed long pos = 0);

  /** Get Frame Comments
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *          an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getFrameComments(OFString& value,
                                       const signed long pos = 0);

  /** Get Frame Label
   *  @param  value Reference to variable that should hold the result
   *  @param  pos Position of the value inside the DICOM element. If 0, the first
   *          value is returned. If the no value at the given position exists,
   *          an error is returned.
   *  @return EC_Normal, if value could be returned, error otherwise
   */
  virtual OFCondition getFrameLabel(OFString& value,
                                    const signed long pos = 0);

  // --- set() functionality ---

  /** Set Frame Acquisition Number
   *  @param  value Value that should be set
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setFrameAcquisitionNumber(const Uint16& value,
                                                const OFBool checkValue = OFTrue);

  /** Set Frame Frame Reference Date Time
   *  @param  value Value that should be set
   *  @param  checkValue If OFTrue, basic checks are performed whether the value is
   *          valid for this attribute
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setFrameReferenceDateTime(const OFString& value,
                                                const OFBool checkValue = OFTrue);

  /** Set Frame Acquisition Date Time
   *  @param  value Value that should be set
   *  @param  checkValue If OFTrue, basic checks are performed whether the value is
   *          valid for this attribute
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setFrameAcquisitionDateTime(const OFString& value,
                                                  const OFBool checkValue = OFTrue);

  /** Set Frame Acquisition Duration
   *  @param  value Value that should be set
   *  @param  checkValue If OFTrue, basic checks are performed whether the value is
   *          valid for this attribute
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setFrameAcquisitionDuration(const Float64& value,
                                                  const OFBool checkValue = OFTrue);

  /** Set Cardiac Cycle Position
   *  @param  value Value that should be set
   *  @param  checkValue If OFTrue, basic checks are performed whether the value is
   *          valid for this attribute
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setCardiacCyclePosition(const OFString& value,
                                              const OFBool checkValue = OFTrue);

  /** Set Respiratory Cycle Position
   *  @param  value Value that should be set
   *  @param  checkValue If OFTrue, basic checks are performed whether the value is
   *          valid for this attribute
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setRespiratoryCyclePosition(const OFString& value,
                                                  const OFBool checkValue = OFTrue);

  /** Set one of the Dimension Index Values
   *  @param  value Value that should be set (lowest index starts with 1)
   *  @param  pos The position of the value that should be set (lowest index
   *          starts with 0), i.e.\ select the 1st, 2nd, ..nth dimension
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setDimensionIndexValues(const Uint32& value,
                                              const unsigned int pos,
                                              const OFBool checkValue = OFTrue);

  /** Set Temporal Position Index
   *  @param  value Value that should be set
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setTemporalPositionIndex(const Uint32& value,
                                               const OFBool checkValue = OFTrue);

  /** Set Stack ID
   *  @param  value Value that should be set
   *  @param  checkValue If OFTrue, basic checks are performed whether the value is
   *          valid for this attribute
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setStackID(const OFString& value,
                                 const OFBool checkValue = OFTrue);

  /** Set In-Stack Position Number
   *  @param  value Value that should be set
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setInStackPositionNumber(const Uint32& value,
                                               const OFBool checkValue = OFTrue);

  /** Set Frame Comments
   *  @param  value Value that should be set
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setFrameComments(const OFString& value,
                                       const OFBool checkValue = OFTrue);

  /** Set Frame Label
   *  @param  value Value that should be set
   *  @param  checkValue If OFTrue, basic checks are performed whether the value is
   *          valid for this attribute
   *  @return EC_Normal, if value was set, error otherwise
   */
  virtual OFCondition setFrameLabel(const OFString& value,
                                    const OFBool checkValue = OFTrue);

private:

  /* Content of Frame Content Macro */

  /// Frame Acquisition Number (US, VM 1, Required type 3)
  DcmUnsignedShort m_FrameAcquisitonNumber;

  /// Frame Reference DateTime (DT, 1, 1C)
  DcmDateTime m_FrameReferenceDateTime;

  /// Frame Acquisition DateTime (DT, 1, 1C)
  DcmDateTime m_FrameAcquisitionDateTime;

  /// Frame Acquisition Duration (FD, 1, 1C)
  DcmFloatingPointDouble m_FrameAcquisitionDuration;

  /// Cardiac Cycle Position (CS, 1, 3)
  DcmCodeString m_CardiacCyclePosition;

  /// Respiratory Cycle Position (CS, 1, 3)
  DcmCodeString m_RespiratoryCyclePosition;

  /// Dimension Index Values (UL, 1-n, 1C)
  DcmUnsignedLong m_DimensionIndexValues;

  /// Temporal Position Index (UL, 1, 1C)
  DcmUnsignedLong m_TemporalPositionIndex;

  /// Stack ID (SH, 1, 1C)
  DcmShortString m_StackID;

  /// In-Stack Position Number (UL, 1, 1C)
  DcmUnsignedLong m_InStackPositionNumber;

  /// Frame Comments (LT, 1, 3)
  DcmLongText m_FrameComments;

  /// Frame Label (LO, 1, 3)
  DcmLongString m_FrameLabel;
};

#endif // FGFRACON_H
