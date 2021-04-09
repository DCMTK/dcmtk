/*
 *
 *  Copyright (C) 2019-2021, Open Connections GmbH
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
 *  Purpose: Class for managing the CT Exposure Functional Group
 *
 */

#ifndef FGCTEXPOSURE_H
#define FGCTEXPOSURE_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofvector.h"

#include <cmath>

/** Class representing the "CT Exposure" Functional Group Macro.
 */
class DCMTK_DCMFG_EXPORT FGCTExposure : public FGBase
{
public:
    /** Constructor, creates empty functional group
     */
    FGCTExposure();

    /** Copy Constructor, creates deep copy
     *  @param  rhs The object to copy from
     */
    FGCTExposure(const FGCTExposure& rhs);

    /** Virtual destructor
     */
    virtual ~FGCTExposure();

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

    /** Read CT Exposure Sequence from given item
     *  @param  item The item to read from
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& item);

    /** Writes the content of this class into CT Exposure Sequence
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

    /** Class representing the "CT Exposure" Functional Group Macro item
     */
    class DCMTK_DCMFG_EXPORT FGCTExposureItem
    {
    public:
        /** Constructor, creates empty functional group item
         */
        FGCTExposureItem();

        /** Copy Constructor, creates deep copy
         *  @param  rhs The object to copy from
         */
        FGCTExposureItem(const FGCTExposureItem& rhs);

        /** Virtual destructor
         */
        virtual ~FGCTExposureItem();

        /** Returns a deep copy of this object
         *  @return  Deep copy of this object
         */
        virtual FGCTExposureItem* clone() const;

        /** Clear all data
         */
        virtual void clearData();

        /** Check whether the current content of this group is consistent and complete
         *  @return EC_Normal, if no errors are found, error otherwise
         */
        virtual OFCondition check() const;

        /** Read CT Exposure Sequence item from given item
         *  @param  item The item to read from
         *  @return EC_Normal if reading was successful, error otherwise
         */
        virtual OFCondition read(DcmItem& item);

        /** Writes the content of this class into given item
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
        virtual int compare(const FGCTExposureItem& rhs) const;

        // --- get() functionality ---

        /** Get Referenced Path Index
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getReferencedPathIndex(OFString& value, const signed long pos = 0);

        /** Get Exposure Time in ms
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getExposureTimeInMs(OFString& value, const signed long pos = 0);

        /** Get Exposure Time in ms
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getExposureTimeInMs(Float64& value, const unsigned long pos = 0);

        /** Get X-Ray Tube Current in mA
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getXRayTubeCurrentInMa(OFString& value, const signed long pos = 0);

        /** Get X-Ray Tube Current in mA
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getXRayTubeCurrentInMa(Float64& value, const unsigned long pos = 0);

        /** Get Exposure in mAs
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getExposureInMas(OFString& value, const signed long pos = 0);

        /** Get Exposure in mAs
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getExposureInMas(Float64& value, const unsigned long pos = 0);

        /** Get Exposure Modulation Type
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getExposureModulationType(OFString& value, const signed long pos = 0);

        /** Get Estimated Dose Saving (retired)
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getEstimatedDoseSaving(OFString& value, const signed long pos = 0);

        /** Get Estimated Dose Saving (retired)
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getEstimatedDoseSaving(Float64& value, const unsigned long pos = 0);

        /** Get CTDIvol
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getCTDIVol(OFString& value, const signed long pos = 0);

        /** Get CTDIvol
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getCTDIVol(Float64& value, const unsigned long pos = 0);

        virtual OFVector<CodeSequenceMacro*>& getCTDIPhantomTypeCodeSequence();

        /** Get Water Equivalent Diameter
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getWaterEquivalentDiameter(OFString& value, const signed long pos = 0);

        /** Get Water Equivalent Diameter
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getWaterEquivalentDiameter(Float64& value, const unsigned long pos = 0);

        virtual OFVector<CodeSequenceMacro*>& getWaterEquivalentDiameterCalculationMethodCodeSequence();

        /** Get Image And Fluoroscopy Area Dose Product
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getImageAndFluoroscopyAreaDoseProduct(OFString& value, const signed long pos = 0);

        /** Get Image And Fluoroscopy Area Dose Product
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1)
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getImageAndFluoroscopyAreaDoseProduct(Float64& value, const unsigned long pos = 0);

        // --- set() functionality ---

        /** Set Referenced Path Index
         *  @param  values Values that should be set
         *  @param  checkValues If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setReferencedPathIndex(const OFVector<Uint16>& values, const OFBool checkValues = OFTrue);

        /** Set Exposure Time in ms
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setExposureTimeInMs(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set X-Ray Tube Current In Ma
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setXRayTubeCurrentInMa(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set Exposure in mAs
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setExposureInMas(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set Exposure Modulation Type
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setExposureModulationType(const OFString& value, const OFBool checkValue = OFTrue);

        /** Set Estimated Dose Saving (retired)
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setEstimatedDoseSaving(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set CTDIVol
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setCTDIVol(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set Water Equivalent Diameter
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setWaterEquivalentDiameter(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set Image and Fluoroscopy Area Dose Product
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setImageAndFluoroscopyAreaDoseProduct(const Float64 value,
                                                                  const OFBool checkValue = OFTrue);

    private:
        /* Content of CT Exposure Macro item */

        /// Referenced Path Index (US, VM 1-n, Required type 1C)
        DcmUnsignedShort m_ReferencedPathIndex;

        /// Exposure Time in ms (FD, 1, 1C)
        DcmFloatingPointDouble m_ExposureTimeInMs;

        /// X-Ray Tube Current in mA (FD, 1, 1C)
        DcmFloatingPointDouble m_XRayTubeCurrentInMa;

        /// Exposure in mAs (FD, 1, 1C)
        DcmFloatingPointDouble m_ExposureInMas;

        /// Exposure Modulation Type (CS, 1-n, 1C)
        DcmCodeString m_ExposureModulationType;

        /// Estimated Dose Saving (FD, 1, 2C), retired
        DcmFloatingPointDouble m_EstimatedDoseSaving;

        /// CTDIvol (FD, 1, 2C)
        DcmFloatingPointDouble m_CTDIVol;

        /// CTDI Phantom Type Code Sequence (SQ, 1, 3)
        OFVector<CodeSequenceMacro*> m_CTDIPhantomTypeCodeSequence;

        /// Water Equivalent Diameter (FD, 1, 3)
        DcmFloatingPointDouble m_WaterEquivalentDiameter;

        /// Water Equivalent Diameter Calculation Method Code Sequence (SQ, 1, 1C)
        OFVector<CodeSequenceMacro*> m_WaterEquivalentDiameterCalculationMethodCodeSequence;

        /// Image and Fluoroscopy Area Dose Product (DS, 1, 3)
        DcmDecimalString m_ImageAndFluoroscopyAreaDoseProduct;
    };

    OFVector<FGCTExposureItem*>& getCTExposureItems();

private:
    /* Content of CT Exposure Macro */

    /// Items of CT Exposure Macro
    OFVector<FGCTExposureItem*> m_Items;
};

#endif // FGCTEXPOSURE_H
