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
 *  Purpose: Class for managing the CT Reconstruction Functional Group
 *
 */

#ifndef FGCTRECONSTRUCTION_H
#define FGCTRECONSTRUCTION_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofvector.h"

/** Class representing the "CT Reconstruction" Functional Group Macro.
 */
class DCMTK_DCMFG_EXPORT FGCTReconstruction : public FGBase
{
public:
    /** Constructor, creates empty functional group
     */
    FGCTReconstruction();

    /** Virtual destructor
     */
    virtual ~FGCTReconstruction();

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

    /** Read CT Reconstruction Sequence from given item
     *  @param  item The item to read from
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& item);

    /** Writes the content of this class into CT Reconstruction Sequence
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

    /** Get Reconstruction Algorithm
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getReconstructionAlgorithm(OFString& value, const signed long pos = 0);

    /** Get Convolution Kernel
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getConvolutionKernel(OFString& value, const signed long pos = 0);

    /** Get Convolution Kernel Group
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getConvolutionKernelGroup(OFString& value, const signed long pos = 0);

    /** Get Reconstruction Diameter
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getReconstructionDiameter(OFString& value, const signed long pos = 0);

    /** Get Reconstruction Diameter
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getReconstructionDiameter(Float64& value, const unsigned long pos = 0);

    /** Get Reconstruction Field of View
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getReconstructionFieldOfView(OFString& value, const signed long pos = 0);

    /** Get Reconstruction Field of View
     *  @param  values Reference to variable that should hold the result
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getReconstructionFieldOfView(OFVector<Float64>& values);

    /** Get Reconstruction Pixel Spacing
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getReconstructionPixelSpacing(OFString& value, const signed long pos = 0);

    /** Get Reconstruction Pixel Spacing
     *  @param  values Reference to variable that should hold the result
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getReconstructionPixelSpacing(OFVector<Float64>& values);

    /** Get Reconstruction Angle
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getReconstructionAngle(OFString& value, const signed long pos = 0);

    /** Get Reconstruction Angle
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1)
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getReconstructionAngle(Float64& value, const unsigned long pos = 0);

    /** Get Image Filter
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getImageFilter(OFString& value, const signed long pos = 0);

    // --- set() functionality ---

    /** Set Reconstruction Algorithm
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setReconstructionAlgorithm(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Convolution Kernel
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setConvolutionKernel(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Convolution Kernel Group
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setConvolutionKernelGroup(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Reconstruction Diameter
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setReconstructionDiameter(const Float64 value, const OFBool checkValue = OFTrue);

    /** Set Reconstruction Field of View
     *  @param  value1 First value that should be set
     *  @param  value2 Second value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition
    setReconstructionFieldOfView(const Float64 value1, const Float64 value2, const OFBool checkValue = OFTrue);

    /** Set Reconstruction Pixel Spacing
     *  @param  value1 First value that should be set
     *  @param  value2 Second value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition
    setReconstructionPixelSpacing(const Float64 value1, const Float64 value2, const OFBool checkValue = OFTrue);
    /** Set Reconstruction Angle
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setReconstructionAngle(const Float64 value, const OFBool checkValue = OFTrue);

    /** Set Image Filter
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setImageFilter(const OFString& value, const OFBool checkValue = OFTrue);

private:
    /* Content of CT Reconstruction Macro */

    /// Reconstruction Algorithm (CS, VM 1, Required type 1C)
    DcmCodeString m_ReconstructionAlgorithm;

    /// Convolution Kernel (SH, 1-n, 1C)
    DcmShortString m_ConvolutionKernel;

    /// Convolution Kernel Group (CS, 1, 1C)
    DcmCodeString m_ConvolutionKernelGroup;

    /// Reconstruction Diameter (DS, 1, 1C)
    DcmDecimalString m_ReconstructionDiameter;

    /// Reconstruction Field of View (FD, 2, 1C)
    DcmFloatingPointDouble m_ReconstructionFieldOfView;

    /// Reconstruction Pixel Spacing (FD, 2, 1C)
    DcmFloatingPointDouble m_ReconstructionPixelSpacing;

    /// Reconstruction Angle (FD, 1, 1C)
    DcmFloatingPointDouble m_ReconstructionAngle;

    /// Image Filter (SH, 1, 1C)
    DcmShortString m_ImageFilter;
};

#endif // FGCTRECONSTRUCTION_H
