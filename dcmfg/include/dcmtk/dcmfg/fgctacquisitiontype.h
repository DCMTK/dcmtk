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
 *  Purpose: Class for managing the CT Acquisition Type Functional Group
 *
 */

#ifndef FGCTACQUISITIONTYPE_H
#define FGCTACQUISITIONTYPE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofvector.h"

/** Class representing the "CT Acquisition Type" Functional Group Macro.
 */
class DCMTK_DCMFG_EXPORT FGCTAcquisitionType : public FGBase
{
public:
    static const OFString DT_AcquisitionType_Sequenced;
    static const OFString DT_AcquisitionType_Spiral;
    static const OFString DT_AcquisitionType_ConstantAngle;
    static const OFString DT_AcquisitionType_Stationary;
    static const OFString DT_AcquisitionType_Free;

    enum E_ConstantVolumeFlag
    {
        E_ConstVol_Yes,
        E_ConstVol_No,
        E_ConstVol_Empty,
        E_ConstVol_Invalid
    };

    enum E_FluoroscopyFlag
    {
        E_Fluoroscopy_Yes,
        E_Fluoroscopy_No,
        E_Fluoroscopy_Empty,
        E_Fluoroscopy_Invalid
    };

    /** Constructor, creates empty functional group
     */
    FGCTAcquisitionType();

    /** Virtual destructor
     */
    virtual ~FGCTAcquisitionType();

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

    /** Read CT Acquisition Type Sequence from given item
     *  @param  item The item to read from
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& item);

    /** Writes the content of this class into CT Acquisition Type Sequence
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

    /** Get Acquisition Type
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getAcquisitionType(OFString& value, const signed long pos = 0);

    /** Get Tube Angle
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Position of the value inside the DICOM element. If 0, the first
     *          value is returned. If the no value at the given position exists,
     *          an error is returned.
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getTubeAngle(Float64& value, const unsigned long pos = 0);

    /** Get Constant Value Flag
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getConstantVolumeFlag(OFString& value, const signed long pos = 0);

    /** Get Constant Value Flag
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getConstantVolumeFlag(E_ConstantVolumeFlag& value, const signed long pos = 0);

    /** Get Fluoroscopy Flag
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getFluoroscopyFlag(OFString& value, const signed long pos = 0);

    /** Get Fluoroscopy Flag
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getFluoroscopyFlag(E_FluoroscopyFlag& value, const signed long pos = 0);

    // --- set() functionality ---

    /** Set Acquisition Type
     *  @param  value Value that should be set
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setAcquisitionType(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Tube Angle
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setTubeAngle(const Float64& value, const OFBool checkValue = OFTrue);

    /** Set Constant Volume Flag
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setConstantVolumeFlag(const E_ConstantVolumeFlag& value, const OFBool checkValue = OFTrue);

    /** Set Constant Volume Flag
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setConstantVolumeFlag(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Fluoroscopy Flag
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setFluoroscopyFlag(const E_FluoroscopyFlag& value, const OFBool checkValue = OFTrue);

    /** Set Fluoroscopy Flag
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setFluoroscopyFlag(const OFString& value, const OFBool checkValue = OFTrue);

    static E_ConstantVolumeFlag constVolFlag2Enum(const OFString& str);

    static OFBool constVolFlag2Str(const E_ConstantVolumeFlag& eval, OFString& result);

    static E_FluoroscopyFlag fluoroscopyFlag2Enum(const OFString& str);

    static OFBool fluoroscopyFlag2Str(const E_FluoroscopyFlag& eval, OFString& result);

private:
    /* Content of CT Acquisition Type Macro */

    /// Acquisition Type (CS, VM 1, Required type 1)
    DcmCodeString m_AcquisitionType;

    /// Tube Angle (FD, 1, 1)
    DcmFloatingPointDouble m_TubeAngle;

    /// Constant Volume Flag (CS, 1, 1)
    DcmCodeString m_ConstantVolumeFlag;

    /// Fluoroscopy Flag (CS, 1, 1)
    DcmCodeString m_FluoroscopyFlag;
};

#endif // FGCTACQUISITIONTYPE_H
