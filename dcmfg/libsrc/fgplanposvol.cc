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
 *  Purpose: Class for managing Plane Position (Volume) Functional Group
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmfg/fgplanposvol.h"
#include "dcmtk/dcmfg/fgtypes.h"
#include "dcmtk/dcmiod/iodutil.h"

// Constructor
FGPlanePositionVolume::FGPlanePositionVolume()
    : FGBase(DcmFGTypes::EFG_PLANEPOSITIONVOLUME)
    , m_ImagePositionVolume(DCM_ImagePositionVolume)
{
}

FGPlanePositionVolume::~FGPlanePositionVolume()
{
    // nothing to do
}

FGBase* FGPlanePositionVolume::clone() const
{
    FGPlanePositionVolume* copy = new FGPlanePositionVolume();
    if (copy)
    {
        copy->m_ImagePositionVolume = this->m_ImagePositionVolume;
    }
    return copy;
}

void FGPlanePositionVolume::clearData()
{
    m_ImagePositionVolume.clear();
}

OFCondition FGPlanePositionVolume::check() const
{
    // Checks in read() and write() are sufficient for now
    return EC_Normal;
}

// --- get() functionality ---

OFCondition FGPlanePositionVolume::getImagePositionVolume(Float64& value, const long unsigned int pos)
{
    return m_ImagePositionVolume.getFloat64(value, pos);
}

OFCondition FGPlanePositionVolume::getImagePositionVolume(Float64& valueX, Float64& valueY, Float64& valueZ)
{
    OFCondition result = m_ImagePositionVolume.getFloat64(valueX, 0);
    if (result.good())
        result = m_ImagePositionVolume.getFloat64(valueY, 1);
    if (result.good())
        result = m_ImagePositionVolume.getFloat64(valueZ, 2);
    return result;
}

OFCondition FGPlanePositionVolume::setImagePositionVolume(const Float64& valueX,
                                                          const Float64& valueY,
                                                          const Float64& valueZ,
                                                          const OFBool checkValue)
{
    (void)checkValue;
    OFCondition result = m_ImagePositionVolume.putFloat64(valueX, 0);
    if (result.good())
        result = m_ImagePositionVolume.putFloat64(valueY, 1);
    if (result.good())
        result = m_ImagePositionVolume.putFloat64(valueZ, 2);
    return result;
}

OFCondition FGPlanePositionVolume::setImagePositionVolume(const Float64& value,
                                                          const long unsigned int pos,
                                                          const OFBool checkValue)
{
    // no checks
    (void)checkValue;
    if (pos > 2)
        return EC_ValueMultiplicityViolated;

    return m_ImagePositionVolume.putFloat64(value, pos);
}

/// Read Plane Position (Volume) Sequence from given item
OFCondition FGPlanePositionVolume::read(DcmItem& item)
{
    clearData();

    DcmItem* seqItem   = NULL;
    OFCondition result = getItemFromFGSequence(item, DCM_PlanePositionVolumeSequence, 0, seqItem);
    if (result.bad())
        return result;

    DcmIODUtil::getAndCheckElementFromDataset(*seqItem, m_ImagePositionVolume, "3", "1", "PlanePositionVolume");

    return EC_Normal;
}

/// Writes single Plane Position (Volume) Sequence into given item
OFCondition FGPlanePositionVolume::write(DcmItem& item)
{
    DcmItem* seqItem   = NULL;
    OFCondition result = createNewFGSequence(item, DCM_PlanePositionVolumeSequence, 0, seqItem);
    if (result.bad())
        return result;

    // --- write frame content macro attributes ---
    DcmIODUtil::copyElementToDataset(result, *seqItem, m_ImagePositionVolume, "3", "1", "PlanePositionVolume");
    return result;
}

int FGPlanePositionVolume::compare(const FGBase& rhs) const
{
    int result = FGBase::compare(rhs);
    if (result != 0)
        return result;

    const FGPlanePositionVolume* myRhs = OFstatic_cast(const FGPlanePositionVolume*, &rhs);
    if (!myRhs)
        return -1;

    // Compare all elements
    result = m_ImagePositionVolume.compare(myRhs->m_ImagePositionVolume);
    return result;
}
