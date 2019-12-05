/*
 *
 *  Copyright (C) 2019, Open Connections GmbH
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
 *  Purpose: Class for managing the CT Position Functional Group
 *
 */

#ifndef FGCTPOSITION_H
#define FGCTPOSITION_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofvector.h"

/** Class representing the "CT Position" Functional Group Macro.
 */
class DCMTK_DCMFG_EXPORT FGCTPosition : public FGBase
{
public:
    /** Constructor, creates empty functional group
     */
    FGCTPosition();

    /** Virtual destructor
     */
    virtual ~FGCTPosition();

    /** Returns a deep copy of this object
     *  @return  Deep copy of this object
     */
    virtual FGBase* clone() const;

    /** Returns shared functional group type
     *  @return The functional group type (DcmFGTypes::EFGS_BOTH)
     */
    virtual DcmFGTypes::E_FGSharedType getSharedType() const
    {
        return DcmFGTypes::EFGS_BOTH;
    }

    /** Clear all data
     */
    virtual void clearData();

    /** Check whether the current content of this group is consistent and complete
     *  @return EC_Normal, if no errors are found, error otherwise
     */
    virtual OFCondition check() const;

    /** Read CT Position Sequence from given item
     *  @param  item The item to read from
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& item);

    /** Writes the content of this class into CT Position Sequence
     *  (newly created) into given item
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

    /** Get Table Speed
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getTablePosition(OFString& value, const signed long pos = 0);

    /** Get Table Speed
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Position of the value inside the DICOM element. If 0, the first
     *          value is returned. If the no value at the given position exists,
     *          an error is returned.
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getTablePosition(Float64& value, const unsigned long pos = 0);

    /** Get Data Collection Center (Patient)
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getDataCollectionCenterPatient(OFString& value, const signed long pos = 0);

    /** Get Data Collection Center (Patient)
     *  @param  values Reference to variable that should hold the result
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getDataCollectionCenterPatient(OFVector<Float64>& values);

    /** Get Reconstruction Target Center (Patient)
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getReconstructionTargetCenterPatient(OFString& value, const signed long pos = 0);

    /** Get Reconstruction Target Center (Patient)
     *  @param  values Reference to variable that should hold the result
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getReconstructionTargetCenterPatient(OFVector<Float64>& values);

    // --- set() functionality ---

    /** Set Table Position
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setTablePosition(const Float64 value, const OFBool checkValue = OFTrue);

    /** Set Data Collection Center (Patient)
     *  @param  values Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setDataCollectionCenterPatient(const OFVector<Float64>& values,
                                                       const OFBool checkValue = OFTrue);

    /** Set Reconstruction Target Center (Patient)
     *  @param  values Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setReconstructionTargetCenterPatient(const OFVector<Float64>& values,
                                                             const OFBool checkValue = OFTrue);

private:
    /* Content of CT Position Macro */

    /// Table Position (FD, VM 1, Required type 1C)
    DcmFloatingPointDouble m_TablePosition;

    /// Data Collection Center (Patient) (FD, 3, 1C)
    DcmFloatingPointDouble m_DataCollectionCenterPatient;

    /// Reconstruction Target Center (Patient) (FD, 3, 1C)
    DcmFloatingPointDouble m_ReconstructionTargetCenterPatient;
};

#endif // FGCTPOSITION_H
