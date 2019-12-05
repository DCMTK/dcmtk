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
 *  Purpose: Class for managing the CT Acquisition Details Functional Group
 *
 */

#ifndef FGCTACQUISITIONDETAILS_H
#define FGCTACQUISITIONDETAILS_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofvector.h"

/** Class representing the "CT Acquisition Details" Functional Group Macro.
 */
class DCMTK_DCMFG_EXPORT FGCTAcquisitionDetails : public FGBase
{
public:
    enum E_RotationDirection
    {
        E_RotationDirection_CC,
        E_RotationDirection_CW,
        E_RotationDirection_Empty,
        E_RotationDirection_Invalid
    };

    /** Constructor, creates empty functional group
     */
    FGCTAcquisitionDetails();

    /** Class representing an item of the "CT Acquisition Details " Functional Group Macro.
     */
    class DCMTK_DCMFG_EXPORT FGCTAcquisitionDetailsItem
    {
    public:
        /** Constructor
         */
        FGCTAcquisitionDetailsItem();

        /** Virtual destructor
         */
        virtual ~FGCTAcquisitionDetailsItem();

        /** Returns a deep copy of this object
         *  @return  Deep copy of this object
         */
        virtual FGCTAcquisitionDetailsItem* clone() const;

        /** Clear all data
         */
        virtual void clearData();

        /** Check whether the current content of this group is consistent and complete
         *  @return EC_Normal, if no errors are found, error otherwise
         */
        virtual OFCondition check() const;

        /** Read CT Acquisition Details Sequence item from given dataitem
         *  @param  item The item to read from
         *  @return EC_Normal if reading was successful, error otherwise
         */
        virtual OFCondition read(DcmItem& item);

        /** Writes the content of this class into CT Acquisition Details Sequence item
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
        virtual int compare(const FGCTAcquisitionDetailsItem& rhs) const;

        // --- get() functionality ---

        /** Get Referenced Path Index
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Position of the value inside the DICOM element. If 0, the first
         *          value is returned. If the no value at the given position exists,
         *          an error is returned.
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getReferencedPathIndex(Uint16& value, const unsigned long pos = 0);

        /** Get Referenced Path Index
         *  @param  values Reference to variable that should hold the result
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getReferencedPathIndex(OFVector<Uint16>& values);

        /** Get Rotation Direction
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getRotationDirection(OFString& value, const signed long pos = 0);

        /** Get Rotation Direction
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Position of the value inside the DICOM element. If 0, the first
         *          value is returned. If the no value at the given position exists,
         *          an error is returned.
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getRotationDirection(E_RotationDirection& value, const signed long pos = 0);

        /** Get Revolution Time
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Position of the value inside the DICOM element. If 0, the first
         *          value is returned. If the no value at the given position exists,
         *          an error is returned.
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getRevolutionTime(Float64& value, const unsigned long pos = 0);

        /** Get Single Collimation Width
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Position of the value inside the DICOM element. If 0, the first
         *          value is returned. If the no value at the given position exists,
         *          an error is returned.
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getSingleCollimationWidth(Float64& value, const unsigned long pos = 0);

        /** Get Total Collimation Width
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Position of the value inside the DICOM element. If 0, the first
         *          value is returned. If the no value at the given position exists,
         *          an error is returned.
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getTotalCollimationWidth(Float64& value, const unsigned long pos = 0);

        /** Get Table Height
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Position of the value inside the DICOM element. If 0, the first
         *          value is returned. If the no value at the given position exists,
         *          an error is returned.
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getTableHeight(Float64& value, const unsigned long pos = 0);

        /** Get Gantry Detector Tilt
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Position of the value inside the DICOM element. If 0, the first
         *          value is returned. If the no value at the given position exists,
         *          an error is returned.
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getGantryDetectorTilt(Float64& value, const unsigned long pos = 0);

        /** Get Data Collection Diameter
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Position of the value inside the DICOM element. If 0, the first
         *          value is returned. If the no value at the given position exists,
         *          an error is returned.
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getDataCollectionDiameter(Float64& value, const unsigned long pos = 0);

        // --- set() functionality ---

        /** Set Referenced Path Index
         *  @param  values Values that should be set
         *  @param  checkValues Check 'values'. Not evaluated (here for consistency
         *          with other setter functions).
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setReferencedPathIndex(const OFVector<Uint16>& values, const OFBool checkValues = OFTrue);

        /** Set Rotation Direction
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setRotationDirection(const E_RotationDirection& value, const OFBool checkValue = OFTrue);

        /** Set Revolution Time
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setRevolutionTime(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set Single Collimation Width
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setSingleCollimationWidth(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set Total Collimation Width
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setTotalCollimationWidth(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set Table Height
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setTableHeight(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set Gantry/Detector Tilt
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setGantryDetectorTilt(const Float64 value, const OFBool checkValue = OFTrue);

        /** Set Data Collection Diameter
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setDataCollectionDiameter(const Float64 value, const OFBool checkValue = OFTrue);

        static E_RotationDirection rotaDir2Enum(const OFString& str);

        static OFBool rotaDir2Str(const E_RotationDirection& rota, OFString& result);

    private:
        /* Content of CT Acquisition Details Macro (item) */

        /// Referenced Path Index (US, VM 1-n, Required type 1C)
        DcmUnsignedShort m_ReferencedPathIndex;

        /// Rotation Direction (CS, 1, 1C)
        DcmCodeString m_RotationDirection;

        /// Revolution Time (FD, 1, 1C)
        DcmFloatingPointDouble m_RevolutionTime;

        /// Single Collimation Width (FD, 1, 1C)
        DcmFloatingPointDouble m_SingleCollimationWidth;

        /// Total Collimation Width  (FD, 1, 1C)
        DcmFloatingPointDouble m_TotalCollimationWidth;

        /// Table Height (DS, 1, 1C)
        DcmDecimalString m_TableHeight;

        /// Gantry/Detector Tilt (DS, 1, 1C)
        DcmDecimalString m_GantryDetectorTilt;

        /// Data Collection Diameter (DS, 1, 1C)
        DcmDecimalString m_DataCollectionDiameter;
    };

    /** Virtual destructor
     */
    virtual ~FGCTAcquisitionDetails();

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

    /** Read CT Acquisition Details Sequence from given item
     *  @param  item The item to read from
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& item);

    /** Writes the content of this class into CT Acquisition Details Sequence
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

    OFVector<FGCTAcquisitionDetailsItem*>& getCTAcquisitionDetailsItems();

private:
    /* Content of CT Acquisition Details Macro */

    OFVector<FGCTAcquisitionDetailsItem*> m_Items;
};

#endif // FGCTACQUISITIONDETAILS_H
