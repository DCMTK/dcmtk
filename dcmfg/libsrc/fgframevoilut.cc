/*
 *
 *  Copyright (C) 2015-2024, Open Connections GmbH
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
 *  Purpose: Class for managing Frame VOI LUT Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgframevoilut.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

const OFString FGFrameVOILUT::DT_CT_WindowCenterWidthExplanation_Brain      = "BRAIN";
const OFString FGFrameVOILUT::DT_CT_WindowCenterWidthExplanation_SoftTissue = "SOFT_TISSUE";
const OFString FGFrameVOILUT::DT_CT_WindowCenterWidthExplanation_Lung       = "LUNG";
const OFString FGFrameVOILUT::DT_CT_WindowCenterWidthExplanation_Bone       = "BONE";

// Constructor
FGFrameVOILUT::FGFrameVOILUT()
    : FGBase(DcmFGTypes::EFG_FRAMEVOILUTMETA)
    , m_WindowCenter(DCM_WindowCenter)
    , m_WindowWidth(DCM_WindowWidth)
    , m_WindowCenterWindowWidthExplanation(DCM_WindowCenterWidthExplanation)
    , m_VOILUTFunction(DCM_VOILUTFunction)
{
}

FGFrameVOILUT::~FGFrameVOILUT()
{
    // nothing to do
}

FGBase* FGFrameVOILUT::clone() const
{
    FGFrameVOILUT* copy = new FGFrameVOILUT();
    if (copy)
    {
        copy->m_WindowCenter                       = this->m_WindowCenter;
        copy->m_WindowWidth                        = this->m_WindowWidth;
        copy->m_WindowCenterWindowWidthExplanation = this->m_WindowCenterWindowWidthExplanation;
        copy->m_VOILUTFunction                     = this->m_VOILUTFunction;
    }
    return copy;
}

void FGFrameVOILUT::clearData()
{
    m_WindowCenter.clear();
    m_WindowWidth.clear();
    m_WindowCenterWindowWidthExplanation.clear();
    m_VOILUTFunction.clear();
}

OFCondition FGFrameVOILUT::check() const
{
    // For now attribute-based checks in read() and write() are sufficient
    return EC_Normal;
}

// --- get() functionality ---

OFCondition FGFrameVOILUT::getWindowCenter(Float64& value, const unsigned long pos)
{
    return m_WindowCenter.getFloat64(value, pos);
}

OFCondition FGFrameVOILUT::getWindowWidth(Float64& value, const unsigned long pos)
{
    return m_WindowWidth.getFloat64(value, pos);
}

OFCondition FGFrameVOILUT::getWindowCenterAndWindowWidthExplanation(OFString& value, const unsigned long pos)
{
    return m_WindowCenterWindowWidthExplanation.getOFString(value, pos);
}

OFCondition FGFrameVOILUT::getCenterWidthExplanation(Float64& windowCenter, Float64& windowWidth, OFString& explanation)
{
    OFCondition result = m_WindowCenter.getFloat64(windowCenter);
    if (result.good())
        result = m_WindowWidth.getFloat64(windowWidth);
    if (result.good())
        result = m_WindowCenterWindowWidthExplanation.getOFString(explanation, 0);
    return result;
}

OFCondition FGFrameVOILUT::getVOILUTFunction(OFString& value, const unsigned long pos)
{
    return m_VOILUTFunction.getOFString(value, pos);
}

// --- set() functionality ---

OFCondition FGFrameVOILUT::setWindowCenter(const OFString& value, const OFBool checkValue)
{
    // no checks at the moment
    (void)checkValue;
    return m_WindowCenter.putOFStringArray(value);
}

OFCondition FGFrameVOILUT::setWindowWidth(const OFString& value, const OFBool checkValue)
{
    // no checks at the moment
    (void)checkValue;
    return m_WindowWidth.putOFStringArray(value);
}

OFCondition FGFrameVOILUT::setCenterWidthExplanation(const Float64& windowCenter,
                                                     const Float64& windowWidth,
                                                     const OFString& explanation,
                                                     const OFBool checkValue)
{
    OFCondition result = m_WindowCenter.putFloat64(windowCenter);
    if (result.good())
        result = m_WindowWidth.putFloat64(windowWidth);
    if (result.good() && !explanation.empty())
    {
        if (checkValue)
        {
            result = (checkValue) ? DcmLongString::checkStringValue(explanation, "1") : EC_Normal;
        }
        if (result.good())
        {
            result = m_WindowCenterWindowWidthExplanation.putString(explanation.c_str());
        }
    }
    return result;
}

OFCondition FGFrameVOILUT::setVOILUTFunction(const OFString& value, const OFBool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_VOILUTFunction.putString(value.c_str());
    return result;
}

/// Read Frame Content Sequence from given item
OFCondition FGFrameVOILUT::read(DcmItem& item)
{
    clearData();

    DcmItem* seqItem   = NULL;
    OFCondition result = getItemFromFGSequence(item, DCM_FrameVOILUTSequence, 0, seqItem);
    if (result.bad())
        return result;

    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_WindowCenter, "1-n", "1", "FrameVOILUTMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_WindowWidth, "1-n", "1", "FrameVOILUTMacro");
    DcmIODUtil::getAndCheckElementFromDataset(
        *seqItem, m_WindowCenterWindowWidthExplanation, "1-n", "3", "FrameVOILUTMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_VOILUTFunction, "1", "3", "FrameVOILUTMacro");

    return EC_Normal;
}

/// Writes single Frame Content Sequence into given item
OFCondition FGFrameVOILUT::write(DcmItem& item)
{
    DcmItem* seqItem   = NULL;
    OFCondition result = createNewFGSequence(item, DCM_FrameVOILUTSequence, 0, seqItem);
    if (result.bad())
        return result;

    // --- write frame content macro attributes ---
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_WindowCenter, "1-n", "1", "FrameVOILUTMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_WindowWidth, "1-n", "1", "FrameVOILUTMacro");
    DcmIODUtil::copyElementToDataset(
        result, *seqItem, m_WindowCenterWindowWidthExplanation, "1-n", "3", "FrameVOILUTMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_VOILUTFunction, "1", "3", "FrameVOILUTMacro");
    return result;
}

int FGFrameVOILUT::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGFrameVOILUT* myRhs = OFstatic_cast(const FGFrameVOILUT*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all elements
    result = m_WindowCenter.compare(myRhs->m_WindowCenter);
    if (result == 0)
        result = m_WindowWidth.compare(myRhs->m_WindowWidth);
    if (result == 0)
        result = m_WindowCenterWindowWidthExplanation.compare(myRhs->m_WindowCenterWindowWidthExplanation);
    if (result == 0)
        result = m_VOILUTFunction.compare(myRhs->m_VOILUTFunction);

    return result;
}
