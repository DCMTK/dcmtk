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
 *  Purpose: Class for managing the CT X-Ray Details Functional Group
 *
 */

#ifndef FGCTXRAYDETAILS_H
#define FGCTXRAYDETAILS_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofvector.h"

/** Class representing the "CT X-Ray Details" Functional Group Macro.
 */
class DCMTK_DCMFG_EXPORT FGCTXRayDetails : public FGBase
{
public:
    /** Constructor, creates empty functional group
     */
    FGCTXRayDetails();

    /** Virtual destructor
     */
    virtual ~FGCTXRayDetails();

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

    /** Read CT X-Ray Details Sequence from given item
     *  @param  item The item to read from
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& item);

    /** Writes the content of this class into CT X-Ray Details Sequence
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

    /** Class representing the "CT X-Ray Details" Functional Group Macro.
     */
    class DCMTK_DCMFG_EXPORT FGCTXRayDetailsItem
    {
    public:
        /** Constructor, creates empty functional group
         */
        FGCTXRayDetailsItem();

        /** Virtual destructor
         */
        virtual ~FGCTXRayDetailsItem();

        /** Returns a deep copy of this object
         *  @return  Deep copy of this object
         */
        virtual FGCTXRayDetailsItem* clone() const;

        /** Clear all data
         */
        virtual void clearData();

        /** Check whether the current content of this group is consistent and complete
         *  @return EC_Normal, if no errors are found, error otherwise
         */
        virtual OFCondition check() const;

        /** Read CT X-Ray Details Sequence item from given item
         *  @param  item The item to read from
         *  @return EC_Normal if reading was successful, error otherwise
         */
        virtual OFCondition read(DcmItem& item);

        /** Writes the content of this class into CT X-Ray Details Sequence item
         *  given item
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
        virtual int compare(const FGCTXRayDetailsItem& rhs) const;

        // --- get() functionality ---

        /** Get Referenced Path Index
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getReferencedPathIndex(OFString& value, const signed long pos = 0);

        /** Get Referenced Path Index
         *  @param  values Reference to variable that should hold the result
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getReferencedPathIndex(OFVector<Uint16>& values);

        /** Get KVP
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getKVP(OFString& value, const signed long pos = 0);

        /** Get KVP
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getKVP(Float64& value, const unsigned long pos = 0);

        /** Get Focal Spots
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getFocalSpots(OFString& value, const signed long pos = 0);

        /** Get Focal Spots
         *  @param  values Reference to variable that should hold the result
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getFocalSpots(OFVector<Float64>& values);

        /** Get Filter Type
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getFilterType(OFString& value, const signed long pos = 0);

        /** Get Filter Material
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getFilterMaterial(OFString& value, const signed long pos = 0);

        /** Get Calcium Scoring Mass Factor Patient
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getCalciumScoringMassFactorPatient(OFString& value, const signed long pos = 0);

        /** Get Calcium Scoring Mass Factor Patient
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getCalciumScoringMassFactorPatient(Float32& value, const unsigned long pos = 0);

        /** Get Calcium Scoring Mass Factor Device
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getCalciumScoringMassFactorDevice(OFString& value, const signed long pos = 0);

        /** Get Calcium Scoring Mass Factor Device
         *  @param  values Reference to variable that should hold the result
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getCalciumScoringMassFactorDevice(OFVector<Float64>& values);

        /** Get Energy Weighting Factor
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getEnergyWeightingFactor(OFString& value, const signed long pos = 0);

        /** Get Energy Weighting Factor
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getEnergyWeightingFactor(Float32& value, const unsigned long pos = 0);

        // --- set() functionality ---

        /** Set Referenced Path Index
         *  @param  values Values that should be set
         *  @param  checkValues If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setReferencedPathIndex(const OFVector<Uint16>& values, const OFBool checkValues = OFTrue);

        /** Set KVP
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setKVP(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set Focal Spots
         *  @param  values Values that should be set
         *  @param  checkValues If OFTrue, basic checks are performed whether the values are
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setFocalSpots(const OFVector<Float64>& values, const OFBool checkValues = OFTrue);

        /** Set Filter Type
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setFilterType(const OFString& value, const OFBool checkValue = OFTrue);

        /** Set Filter Material
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setFilterMaterial(const OFString& value, const OFBool checkValue = OFTrue);

        /** Set Calcium Scoring Mass Factor Patient
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setCalciumScoringMassFactorPatient(const Float32 value, const OFBool checkValue = OFTrue);

        /** Set Calcium Scoring Mass Factor Device
         *  @param  values Values that should be set
         *  @param  checkValues If OFTrue, basic checks are performed whether the values are
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setCalciumScoringMassFactorDevice(const OFVector<Float32>& values,
                                                              const OFBool checkValues = OFTrue);

        /** Set Energy Weighting Factor
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setEnergyWeightingFactor(const Float32 value, const OFBool checkValue = OFTrue);

    private:
        /* Content of CT X-Ray Details Macro */

        /// Referenced Path Index (US, VM 1-n, Required type 1C)
        DcmUnsignedShort m_ReferencedPathIndex;

        /// KVP (DS, 1, 1C)
        DcmDecimalString m_KVP;

        /// Focal Spots (FD, 1-n, 1C)
        DcmDecimalString m_FocalSpots;

        /// Filter Type (SH, 1, 1C)
        DcmShortString m_FilterType;

        /// Filter Material (CS, 1-n, 1C)
        DcmCodeString m_FilterMaterial;

        /// Calcium Scoring Mass Factor Patient (FL, 1, 3)
        DcmFloatingPointSingle m_CalciumScoringMassFactorPatient;

        /// Calcium Scoring Mass Factor Device (FL, 3, 3)
        DcmFloatingPointSingle m_CalciumScoringMassFactorDevice;

        /// Energy Weighting Factor (FD, 1, 1C)
        DcmFloatingPointSingle m_EnergyWeightingFactor;
    };

    // --- get() functionality ---

    OFVector<FGCTXRayDetailsItem*>& getCTXRayDetailsItems();

private:
    /* Content of CT X-Ray Details Macro item */

    /// Items of CT X-Ray Details macro
    OFVector<FGCTXRayDetailsItem*> m_Items;
};

#endif // FGCTXRAYDETAILS_H
