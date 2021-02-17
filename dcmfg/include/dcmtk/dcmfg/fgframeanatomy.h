/*
 *
 *  Copyright (C) 2016-2019, Open Connections GmbH
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
 *  Purpose: Class for managing Frame Anatomy Functional Group
 *
 */

#ifndef FGFRAMEANATOMY_H
#define FGFRAMEANATOMY_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmfg/fgbase.h"
#include "dcmtk/dcmiod/iodmacro.h"

/** Class representing the Frame Anatomy Functional Group Macro
 */
class DCMTK_DCMFG_EXPORT FGFrameAnatomy : public FGBase
{
public:
    /** Enumerated values for attribute "Laterality"
     */
    enum LATERALITY
    {
        /// Undefined (e.g.\ value not set)
        LATERALITY_UNDEFINED,
        /// Invalid Laterality, e.g. when invalid value is read from file
        LATERALITY_INVALID,
        /// Right body part
        LATERALITY_R,
        /// Left body part
        LATERALITY_L,
        /// Unpaired body part
        LATERALITY_UNPAIRED,
        /// Both, left and right
        LATERALITY_BOTH
    };

    /** Constructor, creates empty Frame Anatomy Functional Group
     */
    FGFrameAnatomy();

    /** Virtual destructor, frees memory
     */
    virtual ~FGFrameAnatomy();

    /** Returns a deep copy of this object
     *  @return Deep copy of this object
     */
    virtual FGBase* clone() const;

    /** Returns shared type of this group (can be shared or per-frame)
     *  @return Always returns DcmFGTypes::EFGS_BOTH;
     */
    virtual DcmFGTypes::E_FGSharedType getSharedType() const
    {
        return DcmFGTypes::EFGS_BOTH;
    }

    /** Clear all data
     */
    virtual void clearData();

    /** Check whether data in functional group is valid
     *  @return EC_Normal if data is valid, error otherwise
     */
    virtual OFCondition check() const;

    /** Read functional group from given item
     *  @param  item The item to read from, must contain Frame Anatomy Sequence
     *  @return EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& item);

    /** Write functional group to given item
     *  @param  item The item to write Frame Anatomy Sequence to
     *  @return EC_Normal if writing was successful, error otherwise
     */
    virtual OFCondition write(DcmItem& item);

    // --- get() functionality ---

    /** Get Laterality
     *  @param  value Reference to variable in which the value should be stored
     *  @return EC_Normal if successful, an error code otherwise (i.e. if
     *          value is LATERALITY_INVALID).
     */
    virtual OFCondition getLaterality(LATERALITY& value);

    /** Get anatomy information
     *  @return Reference to anatomic information
     */
    virtual GeneralAnatomyMacro& getAnatomy();

    // --- set() functionality ---

    /** Set Laterality
     *  @param  value Value to be set
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setLaterality(const LATERALITY& value);

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

    static LATERALITY str2Laterality(const OFString& lat);

    static OFString laterality2Str(const LATERALITY lat);

    static OFBool isLateralityValid(const LATERALITY lat);

private:
    /* Content of Frame Anatomy Macro */

    /// Frame Laterality (DS, VM 1, Required type 1)
    LATERALITY m_FrameLaterality;

    /// General Anatomy Mandatory Macro
    GeneralAnatomyMacro m_Anatomy;
};

#endif // FGFRAMEANATOMY_H
