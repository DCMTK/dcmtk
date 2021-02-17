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
 *  Purpose: Class for managing the CT Table Dynamics Functional Group
 *
 */

#ifndef FGCTTABLEDYNAMICS_H
#define FGCTTABLEDYNAMICS_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofvector.h"

/** Class representing the "CT Table Dynamics" Functional Group Macro.
 */
class DCMTK_DCMFG_EXPORT FGCTTableDynamics : public FGBase
{

public:
    /** Class representing an item of the "CT Table Dynamics" Functional Group Macro.
     */
    class DCMTK_DCMFG_EXPORT FGCTTableDynamicsItem
    {

    public:
        /** Constructor, creates empty functional group item
         */
        FGCTTableDynamicsItem();

        /** Virtual destructor
         */
        virtual ~FGCTTableDynamicsItem();

        /** Returns a deep copy of this object
         *  @return  Deep copy of this object
         */
        virtual FGCTTableDynamicsItem* clone() const;

        /** Clear all data
         */
        virtual void clearData();

        /** Check whether the current content of this group item is consistent and complete
         *  @return EC_Normal, if no errors are found, error otherwise
         */
        virtual OFCondition check() const;

        /** Read CT Table Dynamics Sequence from given item
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
        virtual int compare(const FGCTTableDynamicsItem& rhs) const;

        // --- get() functionality ---

        /** Get Table Speed
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getTableSpeed(OFString& value, const signed long pos = 0);

        /** Get Tube Angle
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Position of the value inside the DICOM element. If 0, the first
         *          value is returned. If the no value at the given position exists,
         *          an error is returned.
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getTableSpeed(Float64& value, const unsigned long pos = 0);

        /** Get Table Feed per Rotation
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getTableFeedPerRotation(OFString& value, const signed long pos = 0);

        /** Get Table Feed per Rotation
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Position of the value inside the DICOM element. If 0, the first
         *          value is returned. If the no value at the given position exists,
         *          an error is returned.
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getTableFeedPerRotation(Float64& value, const unsigned long pos = 0);

        /** Get Spiral Pitch Factor
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Index of the value to get (0..vm-1), -1 for all components
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getSpiralPitchFactor(OFString& value, const signed long pos = 0);

        /** Get Spiral Pitch Factor
         *  @param  value Reference to variable that should hold the result
         *  @param  pos Position of the value inside the DICOM element. If 0, the first
         *          value is returned. If the no value at the given position exists,
         *          an error is returned.
         *  @return EC_Normal, if value could be returned, error otherwise
         */
        virtual OFCondition getSpiralPitchFactor(Float64& value, const unsigned long pos = 0);

        // --- set() functionality ---

        /** Set Table Speed
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setTableSpeed(const Float64& value, const OFBool checkValue = OFTrue);

        /** Set Table Feed per Rotation
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setTableFeedPerRotation(const Float64& value, const OFBool checkValue = OFTrue);

        /** Set Spiral Pitch Factor
         *  @param  value Value that should be set
         *  @param  checkValue If OFTrue, basic checks are performed whether the value is
         *          valid for this attribute
         *  @return EC_Normal, if value was set, error otherwise
         */
        virtual OFCondition setSpiralPitchFactor(const Float64& value, const OFBool checkValue = OFTrue);

    private:
        /* Content of CT Table Dynamics Macro (items) */

        /// Acquisition Type (FD, VM 1, Required type 1C)
        DcmFloatingPointDouble m_TableSpeed;

        /// Table Feed Rotation (FD, 1, 1C)
        DcmFloatingPointDouble m_TableFeedPerRotation;

        /// Spiral Pitch Factor (FD, 1, 1C)
        DcmFloatingPointDouble m_SpiralPitchFactor;
    };

    /** Constructor, creates empty functional group
     */
    FGCTTableDynamics();

    /** Virtual destructor
     */
    virtual ~FGCTTableDynamics();

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

    /** Read CT Table Dynamics Sequence from given item
     *  @param  item The item to read from
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& item);

    /** Writes the content of this class into CT Table Dynamics Sequence
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

    OFVector<FGCTTableDynamicsItem*>& getCTTableDynamicsItems();

private:
    /// Items of this functional group macro
    OFVector<FGCTTableDynamicsItem*> m_Items;
};

#endif // FGCTTABLEDYNAMICS_H
