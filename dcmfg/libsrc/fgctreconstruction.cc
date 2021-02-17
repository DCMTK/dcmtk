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

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgctreconstruction.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

// Constructor
FGCTReconstruction::FGCTReconstruction()
    : FGBase(DcmFGTypes::EFG_CTRECONSTRUCTION)
    , m_ReconstructionAlgorithm(DCM_ReconstructionAlgorithm)
    , m_ConvolutionKernel(DCM_ConvolutionKernel)
    , m_ConvolutionKernelGroup(DCM_ConvolutionKernelGroup)
    , m_ReconstructionDiameter(DCM_ReconstructionDiameter)
    , m_ReconstructionFieldOfView(DCM_ReconstructionFieldOfView)
    , m_ReconstructionPixelSpacing(DCM_ReconstructionPixelSpacing)
    , m_ReconstructionAngle(DCM_ReconstructionAngle)
    , m_ImageFilter(DCM_ImageFilter)
{
}

FGCTReconstruction::~FGCTReconstruction()
{
    // nothing to do
}

FGBase* FGCTReconstruction::clone() const
{
    FGCTReconstruction* copy = new FGCTReconstruction();
    if (copy)
    {
        copy->m_ReconstructionAlgorithm    = this->m_ReconstructionAlgorithm;
        copy->m_ConvolutionKernel          = this->m_ConvolutionKernel;
        copy->m_ConvolutionKernelGroup     = this->m_ConvolutionKernelGroup;
        copy->m_ReconstructionDiameter     = this->m_ReconstructionDiameter;
        copy->m_ReconstructionFieldOfView  = this->m_ReconstructionFieldOfView;
        copy->m_ReconstructionPixelSpacing = this->m_ReconstructionPixelSpacing;
        copy->m_ReconstructionAngle        = this->m_ReconstructionAngle;
        copy->m_ImageFilter                = this->m_ImageFilter;
    }
    return copy;
}

void FGCTReconstruction::clearData()
{
    m_ReconstructionAlgorithm.clear();
    m_ConvolutionKernel.clear();
    m_ConvolutionKernelGroup.clear();
    m_ReconstructionDiameter.clear();
    m_ReconstructionFieldOfView.clear();
    m_ReconstructionPixelSpacing.clear();
    m_ReconstructionAngle.clear();
    m_ImageFilter.clear();
}

OFCondition FGCTReconstruction::check() const
{
    // Maybe add checks later
    return EC_Normal;
}

/// Read CT Reconstruction Sequence from given item
OFCondition FGCTReconstruction::read(DcmItem& item)
{
    clearData();

    DcmItem* seqItem   = NULL;
    OFCondition result = getItemFromFGSequence(item, DCM_CTReconstructionSequence, 0, seqItem);
    if (result.bad())
        return result;

    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_ReconstructionAlgorithm, "1", "1C", "CTReconstructionMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_ConvolutionKernel, "1-n", "1C", "CTReconstructionMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_ConvolutionKernelGroup, "1", "1C", "CTReconstructionMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_ReconstructionDiameter, "1", "1C", "CTReconstructionMacro");
    DcmIODUtil::getAndCheckElementFromDataset(
        *seqItem, m_ReconstructionFieldOfView, "2", "1C", "CTReconstructionMacro");
    DcmIODUtil::getAndCheckElementFromDataset(
        *seqItem, m_ReconstructionPixelSpacing, "2", "1C", "CTReconstructionMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_ReconstructionAngle, "1", "1C", "CTReconstructionMacro");
    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_ImageFilter, "1", "1C", "CTReconstructionMacro");

    return EC_Normal;
}

/// Writes single CT Reconstruction Sequence into given item
OFCondition FGCTReconstruction::write(DcmItem& item)
{
    DcmItem* seqItem   = NULL;
    OFCondition result = createNewFGSequence(item, DCM_CTReconstructionSequence, 0, seqItem);
    if (result.bad())
        return result;

    // --- write frame content macro attributes ---
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_ReconstructionAlgorithm, "1", "1C", "CTReconstructionMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_ConvolutionKernel, "1-n", "1C", "CTReconstructionMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_ConvolutionKernelGroup, "1", "1C", "CTReconstructionMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_ReconstructionDiameter, "1", "1C", "CTReconstructionMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_ReconstructionFieldOfView, "2", "1C", "CTReconstructionMacro");
    DcmIODUtil::copyElementToDataset(
        result, *seqItem, m_ReconstructionPixelSpacing, "2", "1C", "CTReconstructionMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_ReconstructionAngle, "1", "1C", "CTReconstructionMacro");
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_ImageFilter, "1", "1C", "CTReconstructionMacro");

    return result;
}

int FGCTReconstruction::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGCTReconstruction* myRhs = OFstatic_cast(const FGCTReconstruction*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all elements
    result = m_ReconstructionAlgorithm.compare(myRhs->m_ReconstructionAlgorithm);
    if (result == 0)
        result = m_ConvolutionKernel.compare(myRhs->m_ConvolutionKernel);
    if (result == 0)
        result = m_ConvolutionKernelGroup.compare(myRhs->m_ConvolutionKernelGroup);
    if (result == 0)
        result = m_ReconstructionDiameter.compare(myRhs->m_ReconstructionDiameter);
    if (result == 0)
        result = m_ReconstructionFieldOfView.compare(myRhs->m_ReconstructionFieldOfView);
    if (result == 0)
        result = m_ReconstructionPixelSpacing.compare(myRhs->m_ReconstructionPixelSpacing);
    if (result == 0)
        result = m_ReconstructionAngle.compare(myRhs->m_ReconstructionAngle);
    if (result == 0)
        result = m_ImageFilter.compare(myRhs->m_ImageFilter);

    return result;
}

// --- get() functionality ---

OFCondition FGCTReconstruction::getReconstructionAlgorithm(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ReconstructionAlgorithm, value, pos);
}

OFCondition FGCTReconstruction::getConvolutionKernel(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ConvolutionKernel, value, pos);
}

OFCondition FGCTReconstruction::getConvolutionKernelGroup(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ConvolutionKernelGroup, value, pos);
}

OFCondition FGCTReconstruction::getReconstructionDiameter(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ReconstructionDiameter, value, pos);
}

OFCondition FGCTReconstruction::getReconstructionDiameter(Float64& value, const unsigned long pos)
{
    return m_ReconstructionDiameter.getFloat64(value, pos);
}

OFCondition FGCTReconstruction::getReconstructionFieldOfView(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ReconstructionFieldOfView, value, pos);
}

OFCondition FGCTReconstruction::getReconstructionFieldOfView(OFVector<Float64>& values)
{
    return DcmIODUtil::getFloat64ValuesFromElement(m_ReconstructionFieldOfView, values);
}

OFCondition FGCTReconstruction::getReconstructionPixelSpacing(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ReconstructionPixelSpacing, value, pos);
}

OFCondition FGCTReconstruction::getReconstructionPixelSpacing(OFVector<Float64>& values)
{
    return DcmIODUtil::getFloat64ValuesFromElement(m_ReconstructionPixelSpacing, values);
}

OFCondition FGCTReconstruction::getReconstructionAngle(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ReconstructionAngle, value, pos);
}

OFCondition FGCTReconstruction::getReconstructionAngle(Float64& value, const unsigned long pos)
{
    return m_ReconstructionAngle.getFloat64(value, pos);
}

OFCondition FGCTReconstruction::getImageFilter(OFString& value, const long pos)
{
    return DcmIODUtil::getStringValueFromElement(m_ImageFilter, value, pos);
}

// --- set() functionality ---

OFCondition FGCTReconstruction::setReconstructionAlgorithm(const OFString& value, const bool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_ReconstructionAlgorithm.putString(value.c_str());
    return result;
}

OFCondition FGCTReconstruction::setConvolutionKernel(const OFString& value, const bool checkValue)
{
    OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1-n") : EC_Normal;
    if (result.good())
        result = m_ConvolutionKernel.putString(value.c_str());
    return result;
}

OFCondition FGCTReconstruction::setConvolutionKernelGroup(const OFString& value, const bool checkValue)
{
    OFCondition result = (checkValue) ? DcmCodeString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_ConvolutionKernelGroup.putString(value.c_str());
    return result;
}

OFCondition FGCTReconstruction::setReconstructionDiameter(const Float64 value, const bool checkValue)
{
    (void)checkValue;
    return m_ReconstructionDiameter.putFloat64(value);
}

OFCondition
FGCTReconstruction::setReconstructionFieldOfView(const Float64 value1, const Float64 value2, const bool checkValue)
{
    (void)checkValue;
    OFCondition result = m_ReconstructionFieldOfView.putFloat64(value1, 0);
    if (result.good())
        result = m_ReconstructionFieldOfView.putFloat64(value2, 1);
    return result;
}

OFCondition
FGCTReconstruction::setReconstructionPixelSpacing(const Float64 value1, const Float64 value2, const bool checkValue)
{
    (void)checkValue;
    OFCondition result = m_ReconstructionPixelSpacing.putFloat64(value1, 0);
    if (result.good())
        result = m_ReconstructionPixelSpacing.putFloat64(value2, 1);
    return result;
}

OFCondition FGCTReconstruction::setReconstructionAngle(const Float64 value, const bool checkValue)
{
    (void)checkValue;
    return m_ReconstructionAngle.putFloat64(value);
}

OFCondition FGCTReconstruction::setImageFilter(const OFString& value, const bool checkValue)
{
    OFCondition result = (checkValue) ? DcmShortString::checkStringValue(value, "1") : EC_Normal;
    if (result.good())
        result = m_ImageFilter.putString(value.c_str());
    return result;
}
