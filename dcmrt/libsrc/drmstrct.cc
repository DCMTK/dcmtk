/*
 *
 *  Copyright (C) 2012, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmrt
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Mid-Level API for RT Structure Set objects
 *
 */

#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */
#include "dcmtk/dcmrt/drmstrct.h"
#include "dcmtk/dcmrt/drmsrch.h"

OFCondition DRTStructureSet::loadFile(const OFFilename &filename,
                                      const E_FileReadMode readMode,
                                      const E_TransferSyntax readXfer)
{
    DcmFileFormat format;
    OFCondition cond = format.loadFile(filename, readXfer, EGL_noChange,
                                       DCM_MaxReadLength, readMode);
    if (cond.good())
        cond = read(*format.getDataset());
    return cond;
}

DRTReferencedFrameOfReferenceSequence::Item& DRTStructureSet::getFrameOfReference(const OFString& uid)
{
    // First some typedefs to make the following more readable
    typedef DRTReferencedFrameOfReferenceSequence seq;
    typedef DRTSearch::searchPropertyEqualHelper<seq, seq::Item, OFString, const long, 0> helper;
    return helper::search(getReferencedFrameOfReferenceSequence(), uid,
            &seq::Item::getFrameOfReferenceUID);
}

DRTStructureSetROISequence::Item& DRTStructureSet::getROI(Sint32 roiNumber)
{
    // First some typedefs to make the following more readable
    typedef DRTStructureSetROISequence seq;
    typedef DRTSearch::searchPropertyEqualHelper<seq, seq::Item, Sint32, const unsigned long, 0> helper;
    return helper::search(getStructureSetROISequence(), roiNumber,
            &seq::Item::getROINumber);
}

DRTRTROIObservationsSequence::Item& DRTStructureSet::getObservation(Sint32 observationNumber)
{
    // First some typedefs to make the following more readable
    typedef DRTRTROIObservationsSequence seq;
    typedef DRTSearch::searchPropertyEqualHelper<seq, seq::Item, Sint32, const unsigned long, 0> helper;
    return helper::search(getRTROIObservationsSequence(), observationNumber,
            &seq::Item::getObservationNumber);
}

DRTRTROIObservationsSequence::Item& DRTStructureSet::getObservationByROINumber(Sint32 roiNumber)
{
    // First some typedefs to make the following more readable
    typedef DRTRTROIObservationsSequence seq;
    typedef DRTSearch::searchPropertyEqualHelper<seq, seq::Item, Sint32, const unsigned long, 0> helper;
    return helper::search(getRTROIObservationsSequence(), roiNumber,
            &seq::Item::getReferencedROINumber);
}

OFCondition DRTStructureSet::getContoursForROINumber(OFVector<DRTROIContourSequence::Item*> result, Sint32 roiNumber)
{
    DRTROIContourSequence& seq = getROIContourSequence();
    OFCondition cond = seq.gotoFirstItem();

    // tbd: This method could easily be turned into a template if needed,
    // to avoid code duplication

    // Go through all items in the sequence...
    result.clear();
    while (cond.good()) {
        Sint32 number;
        DRTROIContourSequence::Item& item = seq.getCurrentItem();
        cond = item.getReferencedROINumber(number);
        if (cond.bad())
            return cond;
        // ...looking for items with the wanted roiNumber
        if (number == roiNumber)
            result.push_back(&item);
        cond = seq.gotoNextItem();
    }

    return EC_Normal;
}

OFCondition DRTStructureSet::getContoursForROINumber(OFList<DRTROIContourSequence::Item*> result, Sint32 roiNumber)
{
    DRTROIContourSequence& seq = getROIContourSequence();
    OFCondition cond = seq.gotoFirstItem();

    // tbd: This method could easily be turned into a template if needed,
    // to avoid code duplication

    // Go through all items in the sequence...
    result.clear();
    while (cond.good()) {
        Sint32 number;
        DRTROIContourSequence::Item& item = seq.getCurrentItem();
        cond = item.getReferencedROINumber(number);
        if (cond.bad())
            return cond;
        // ...looking for items with the wanted roiNumber
        if (number == roiNumber)
            result.push_back(&item);
        cond = seq.gotoNextItem();
    }

    return EC_Normal;
}
