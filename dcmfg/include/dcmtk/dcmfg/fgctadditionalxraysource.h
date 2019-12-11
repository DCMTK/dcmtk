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
 *  Purpose: Class for managing the CT Additional X-Ray Source Functional Group
 *
 */

#ifndef FGCTADDITIONALXRAYSOURCE_H
#define FGCTADDITIONALXRAYSOURCE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dctk.h" // TODO: include only needed VRs
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/dcmfg/fgdefine.h"

/** Class representing the CT Additional X-Ray Source Functional Group Macro.
 */
class DCMTK_DCMFG_EXPORT FGCTAdditionalXRaySource : public FGBase
{
public:
    /** Constructor, creates empty CT Additional X-Ray Source Functional Group
     */
    FGCTAdditionalXRaySource();

    /** Destructor, frees memory
     */
    virtual ~FGCTAdditionalXRaySource();

    /** Returns a deep copy of this object
     *  @return  Deep copy of this object
     */
    virtual FGBase* clone() const;

    /** Get shared type of this functional group (can be both, per-frame and
     *  shared)
     *  @return Always returns EFGS_BOTH
     */
    virtual DcmFGTypes::E_FGSharedType getSharedType() const
    {
        return DcmFGTypes::EFGS_BOTH;
    }

    /** Check whether functional group contains valid data
     *  @return EC_Normal if data is valid, error otherwise
     */
    virtual OFCondition check() const;

    /** Clears all data
     */
    virtual void clearData();

    /** Read functional group from given item, i.e.\ read CT Additional X-Ray Source Sequence
     *  @param  item The item to read from
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& item);

    /** Write functional group to given item, i.e.\ write CT Additional X-Ray Source Sequence
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

    /** Class representing an item of the "CT Additional X-Ray Source" Functional Group Macro.
     */
    class DCMTK_DCMFG_EXPORT FGCTAdditionalXRaySourceItem
    {
    public:
        /** Constructor, creates empty CT Additional X-Ray Source Functional Group
         */
        FGCTAdditionalXRaySourceItem();

        /** Destructor, frees memory
         */
        virtual ~FGCTAdditionalXRaySourceItem();

        /** Returns a deep copy of this object
         *  @return  Deep copy of this object
         */
        virtual FGCTAdditionalXRaySourceItem* clone() const;

        /** Clears all data
         */
        virtual void clearData();

        /** Check whether functional group contains valid data
         *  @return EC_Normal if data is valid, error otherwise
         */
        virtual OFCondition check() const;

        /** Read functional group from given item, i.e.\ read CT Additional X-Ray Source Sequence
         *  @param  item The item to read from
         *  @return EC_Normal if reading was successful, error otherwise
         */
        virtual OFCondition read(DcmItem& item);

        /** Write functional group to given item, i.e.\ write CT Additional X-Ray Source Sequence
         *  @param  item The item to write to
         *  @return EC_Normal if writing was successful, error otherwise
         */
        virtual OFCondition write(DcmItem& item);

        /** Get KVP
         *  Peak kilo voltage output of the X-Ray generator used.
         *  @param  value Reference to variable in which the value should be stored
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition getKVP(OFString& value, const signed long pos = 0) const;

        /** Get XRayTubeCurrentInmA
         *  Nominal X-Ray tube current in milliamperes.
         *  @param  value Reference to variable in which the value should be stored
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition getXRayTubeCurrentInmA(Float64& value, const unsigned long pos = 0) const;

        /** Get DataCollectionDiameter
         *  The diameter in mm of the region over which data were collected.
         *  @param  value Reference to variable in which the value should be stored
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition getDataCollectionDiameter(OFString& value, const signed long pos = 0) const;

        /** Get FocalSpots
         *  Used nominal size of the focal spot in mm.
         *  @param  value Reference to variable in which the value should be stored
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition getFocalSpots(OFString& value, const signed long pos = 0) const;

        /** Get FilterType
         *  Type of filter(s) inserted into the X-Ray beam. See
         *  @param  value Reference to variable in which the value should be stored
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition getFilterType(OFString& value, const signed long pos = 0) const;

        /** Get FilterMaterial
         *  The X-Ray absorbing material used in the filter.
         *  @param  value Reference to variable in which the value should be stored
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition getFilterMaterial(OFString& value, const signed long pos = 0) const;

        /** Get ExposureInmAs
         *  The exposure expressed in milliampere seconds, for example calculated from exposure time and X-Ray tube
         * current.
         *  @param  value Reference to variable in which the value should be stored
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition getExposureInmAs(Float64& value, const unsigned long pos = 0) const;

        /** Get EnergyWeightingFactor
         *  The weighting factor of the data from this additional source in a multiple energy composition image. This
         * factor incorporates the effects ofRequired if Required if Frame Type (0008,9007) Value 4 of this frame is
         * ENERGY_PROP_WT. May be present otherwise.
         *  @param  value Reference to variable in which the value should be stored
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition getEnergyWeightingFactor(Float32& value, const unsigned long pos = 0) const;

        /** Set KVP
         *  Peak kilo voltage output of the X-Ray generator used.
         *  @param  value Value to be set (single value only) or "" for no value
         *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setKVP(const OFString& value, const OFBool checkValue = OFTrue);

        /** Set KVP
         *  Peak kilo voltage output of the X-Ray generator used.
         *  @param  value Value to be set
         *  @param  checkValue Check 'value' if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setKVP(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set XRayTubeCurrentInmA
         *  Nominal X-Ray tube current in milliamperes.
         *  @param  value Value to be set (single value only) or "" for no value
         *  @param  checkValue Check 'value' for conformance with VR (FD) and VM (1) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setXRayTubeCurrentInmA(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set DataCollectionDiameter
         *  The diameter in mm of the region over which data were collected.
         *  @param  value Value to be set (single value only) or "" for no value
         *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setDataCollectionDiameter(const OFString& value, const OFBool checkValue = OFTrue);

        /** Set DataCollectionDiameter
         *  The diameter in mm of the region over which data were collected.
         *  @param  value Value to be set (single value only) or "" for no value
         *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setDataCollectionDiameter(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set FocalSpots
         *  Used nominal size of the focal spot in mm.
         *  @param  value Value to be set (single value only) or "" for no value
         *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1-n) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setFocalSpots(const OFString& value, const OFBool checkValue = OFTrue);

        /** Set FocalSpots
         *  Used nominal size of the focal spot in mm.
         *  @param  values Values to be set
         *  @param  checkValue Check 'value' for conformance with VR (DS) and VM (1-n) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setFocalSpots(const OFVector<Float64>& values, const OFBool checkValue = OFTrue);

        /** Set FilterType
         *  Type of filter(s) inserted into the X-Ray beam. See
         *  @param  value Value to be set (single value only) or "" for no value
         *  @param  checkValue Check 'value' for conformance with VR (SH) and VM (1) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setFilterType(const OFString& value, const OFBool checkValue = OFTrue);

        /** Set FilterMaterial
         *  The X-Ray absorbing material used in the filter.
         *  @param  value Value to be set (single value only) or "" for no value
         *  @param  checkValue Check 'value' for conformance with VR (CS) and VM (1-n) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setFilterMaterial(const OFString& value, const OFBool checkValue = OFTrue);

        /** Set ExposureInmAs
         *  The exposure expressed in milliampere seconds, for example calculated from exposure time and X-Ray tube
         * current.
         *  @param  value Value to be set (single value only) or "" for no value
         *  @param  checkValue Check 'value' for conformance with VR (FD) and VM (1) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setExposureInmAs(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set EnergyWeightingFactor
         *  The weighting factor of the data from this additional source in a multiple energy composition image. This
         * factor incorporates the effects ofRequired if Required if Frame Type (0008,9007) Value 4 of this frame is
         * ENERGY_PROP_WT. May be present otherwise.
         *  @param  value Value to be set (single value only) or "" for no value
         *  @param  checkValue Check 'value' for conformance with VR (FL) and VM (1) if enabled
         *  @return EC_Normal if successful, an error code otherwise
         */
        virtual OFCondition setEnergyWeightingFactor(const Float32 value, const OFBool checkValue = OFTrue);

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
         *          but this component is shorter.
         *          1 if either the value of the first component that does not match
         *          is greater in this object, or all compared components match
         *          but this component is longer.
         */
        virtual int compare(const FGCTAdditionalXRaySourceItem& rhs) const;

    protected:
        /* Content of CT Additional X-Ray Source Functional Group Macro */

        /// KVP (DS, VM 1, Required type 1)
        /// Peak kilo voltage output of the X-Ray generator used.
        DcmDecimalString m_KVP;

        /// XRayTubeCurrentInmA (FD, VM 1, Required type 1)
        /// Nominal X-Ray tube current in milliamperes.
        DcmFloatingPointDouble m_XRayTubeCurrentInmA;

        /// DataCollectionDiameter (DS, VM 1, Required type 1)
        /// The diameter in mm of the region over which data were collected.
        DcmDecimalString m_DataCollectionDiameter;

        /// FocalSpots (DS, VM 1-n, Required type 1)
        /// Used nominal size of the focal spot in mm.
        DcmDecimalString m_FocalSpots;

        /// FilterType (SH, VM 1, Required type 1)
        /// Type of filter(s) inserted into the X-Ray beam. See
        DcmShortString m_FilterType;

        /// FilterMaterial (CS, VM 1-n, Required type 1)
        /// The X-Ray absorbing material used in the filter.
        DcmCodeString m_FilterMaterial;

        /// ExposureInmAs (FD, VM 1, Required type 1)
        /// The exposure expressed in milliampere seconds, for example calculated from exposure time and X-Ray tube
        /// current.
        DcmFloatingPointDouble m_ExposureInmAs;

        /// EnergyWeightingFactor (FL, VM 1, Required type 1C)
        /// The weighting factor of the data from this additional source in a multiple energy composition image. This
        /// factor incorporates the effects ofRequired if Required if Frame Type (0008,9007) Value 4 of this frame is
        /// ENERGY_PROP_WT. May be present otherwise.
        DcmFloatingPointSingle m_EnergyWeightingFactor;
    };

    virtual OFVector<FGCTAdditionalXRaySourceItem*>& getCTAdditionalXRaySourceItems();

protected:
    OFVector<FGCTAdditionalXRaySourceItem*> m_Items;
};

#endif // FGCTADDITIONALXRAYSOURCE_H
