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
 *  Purpose: Class for managing Frame VOI LUT Functional Group
 *
 */

#ifndef FGFRAMEVOILUT_H
#define FGFRAMEVOILUT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmfg/fgbase.h"

/** Class representing the "Frame VOI LUT" Functional Group Macro
 */
class DCMTK_DCMFG_EXPORT FGFrameVOILUT : public FGBase
{
public:

  /** Constructor creating empty functional group
   */
  FGFrameVOILUT();

  /** Virtual destructor
   */
  virtual ~FGFrameVOILUT();

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Get shared type of this kind functional group (can be both, per-frame or
   *  shared)
   *  @return Always returns DcmFGTypes::EFGS_BOTH
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  /** Clear old values
   */
  virtual void clearData();

  /** Check whether functional group contains valid data
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Read Frame VOI LUT Sequence from given item
   *  @param  item The item to read from
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& item);

  /** Write Frame VOI LUT Sequence to given item
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

  // --- get() functionality ---

   /** Get Window Center
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1)
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getWindowCenter(Float64& value,
                                      const unsigned long pos = 0);

   /** Get Window Width
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1)
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getWindowWidth(Float64& value,
                                     const unsigned long pos = 0);

   /** Get Window Center and Window Width Explanation
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1)
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getWindowCenterAndWindowWidthExplanation(OFString& value,
                                                               const unsigned long pos = 0);

   /** Get tripe of information: Window Center, Window With, Window Center and
    *  Window Width Explanation
    *  @param  windowCenter Reference to variable in which Window Center should
    *          be stored
    *  @param  windowWidth Reference to variable in which Window Width should
    *          be stored
    *  @param  explanation Reference to variable in which Window Center and
    *          Window Width Explanation should be stored
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getCenterWidthExplanation(Float64& windowCenter,
                                                Float64& windowWidth,
                                                OFString& explanation);

   /** Get VOI LUT Function
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1)
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getVOILUTFunction(OFString& value,
                                        const unsigned long pos = 0);


  // --- set() functionality ---

  /** Set Window Center
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setWindowCenter(const OFString& value,
                                      const OFBool checkValue = OFTrue);

  /** Set Window Width
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setWindowWidth(const OFString& value,
                                     const OFBool checkValue = OFTrue);

  /** Set Window Center and Window Width Explanation
   *  @param  windowCenter Value for Window Center
   *  @param  windowWidth Value for Window Width
   *  @param  explanation Value for Window Center and Window Width Explanation
   *          should be stored
   *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1-n)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setCenterWidthExplanation(const Float64& windowCenter,
                                                const Float64& windowWidth,
                                                const OFString& explanation = "",
                                                const OFBool checkValue = OFTrue);

  /** Set VOI LUT Function
   *  @param  value Value for VOI LUT Function
   *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setVOILUTFunction(const OFString& value,
                                        const OFBool checkValue = OFTrue);

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

  /* Content of Frame VOI LUT Macro */

  /// Window Center (DS, VM 1-n, Required type 1)
  DcmDecimalString m_WindowCenter;

  /// Window Width (DS, VM 1-n, Required type 1)
  DcmDecimalString m_WindowWidth;

  /// Window Center & Window Width Explanation (LO, VM 1-n, Required type 3)
  DcmLongString m_WindowCenterWindowWidthExplanation;

  /// VOI LUT Function (CS, VM 1, Requirement Type 3)
  DcmCodeString m_VOILUTFunction;
};

#endif // FGFRAMEVOILUT_H
