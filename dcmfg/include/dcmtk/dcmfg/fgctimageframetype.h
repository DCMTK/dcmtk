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
 *  Purpose: Class for managing the CT Image Frame Type Functional Group
 *
 */

#ifndef FGCTIMAGEFRAMETYPE_H
#define FGCTIMAGEFRAMETYPE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofvector.h"

/** Class representing the "CT Image Frame Type" Functional Group Macro. Can only be
 *  used as per-frame functional group (never shared).
 */
class DCMTK_DCMFG_EXPORT FGCTImageFrameType : public FGBase
{
public:
    static const OFString DT_VolBasedCalcTechnique_MaxIp;
    static const OFString DT_VolBasedCalcTechnique_MinIp;
    static const OFString DT_VolBasedCalcTechnique_VolumeRender;
    static const OFString DT_VolBasedCalcTechnique_SurfaceRender;
    static const OFString DT_VolBasedCalcTechnique_Mpr;
    static const OFString DT_VolBasedCalcTechnique_CurvedMpr;
    static const OFString DT_VolBasedCalcTechnique_None;
    static const OFString DT_VolBasedCalcTechnique_Mixed;

    enum E_PixelPresentation
    {
        E_PixelPres_Color,
        E_PixelPres_Monochrome,
        E_PixelPres_Mixed,
        E_PixelPres_TrueColor,
        E_PixelPres_Empty,
        E_PixelPres_Invalid
    };

    enum E_VolumetricProperties
    {
        E_VolProp_Volume,
        E_VolProp_Sampled,
        E_VolProp_Distorted,
        E_VolProp_Mixed,
        E_VolProp_Empty,
        E_VolProp_Invalid
    };

    /** Constructor, creates empty functional group
     */
    FGCTImageFrameType();

    /** Virtual destructor
     */
    virtual ~FGCTImageFrameType();

    /** Returns a deep copy of this object
     *  @return  Deep copy of this object
     */
    virtual FGBase* clone() const;

    /** Returns shared functional group type (always "per-frame")
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

    /** Read CT Image Frame Type Sequence from given item
     *  @param  item The item to read from
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& item);

    /** Writes the content of this class into CT Image Frame Type Sequence
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

    /** Get Frame Type
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getFrameType(OFString& value, const signed long pos = 0);

    /** Get Pixel Presentation
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getPixelPresentation(OFString& value, const signed long pos = 0);

    /** Get Pixel Presentation
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Position of the value inside the DICOM element. If 0, the first
     *          value is returned. If the no value at the given position exists,
     *          an error is returned.
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getPixelPresentation(E_PixelPresentation& value, const signed long pos = 0);

    /** Get Volumetric Properties
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getVolumetricProperties(OFString& value, const signed long pos = 0);

    /** Get Volumetric Properties
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Position of the value inside the DICOM element. If 0, the first
     *          value is returned. If the no value at the given position exists,
     *          an error is returned.
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getVolumetricProperties(E_VolumetricProperties& value, const signed long pos = 0);

    /** Get Volume Based Calculation Technique
     *  @param  value Reference to variable that should hold the result
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal, if value could be returned, error otherwise
     */
    virtual OFCondition getVolumeBasedCalculationTechnique(OFString& value, const signed long pos = 0);

    // --- set() functionality ---

    /** Set Frame Type
     *  @param  value Value that should be set
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setFrameType(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Pixel Presentation
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setPixelPresentation(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Pixel Presentation
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setPixelPresentation(const E_PixelPresentation& value, const OFBool checkValue = OFTrue);

    /** Set Volumetric Properties
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setVolumetricProperties(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set Volumetric Properties
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setVolumetricProperties(const E_VolumetricProperties& value, const OFBool checkValue = OFTrue);

    /** Set Volume Based Calculation Technique
     *  @param  value Value that should be set
     *  @param  checkValue If OFTrue, basic checks are performed whether the value is
     *          valid for this attribute
     *  @return EC_Normal, if value was set, error otherwise
     */
    virtual OFCondition setVolumeBasedCalculationTechnique(const OFString& value, const OFBool checkValue = OFTrue);

    static E_PixelPresentation pixelPres2Enum(const OFString& str);

    static OFBool pixelPres2Str(const E_PixelPresentation& eval, OFString& result);

    static E_VolumetricProperties volProps2Enum(const OFString& str);

    static OFBool volProps2Str(const E_VolumetricProperties& eval, OFString& result);

private:
    /* Content of CT Image Frame Type Macro */

    /// Frame Type (CS, VM 4, Required type 1)
    DcmCodeString m_FrameType;

    /// Pixel Presentation (CS, 1, 1)
    DcmCodeString m_PixelPresentation;

    /// Volumetric Properties (CS, 1, 1)
    DcmCodeString m_VolumetricProperties;

    /// Volume Based Calculation Technique (CS, 1, 1)
    DcmCodeString m_VolumeBasedCalculationTechnique;
};

#endif // FGCTIMAGEFRAMETYPE_H
