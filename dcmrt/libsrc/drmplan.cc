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
 *  Purpose: Mid-Level API for RT Plan objects
 *
 */

#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */
#include "dcmtk/dcmrt/drmplan.h"
#include "dcmtk/dcmrt/drmsrch.h"

OFCondition DRTPlan::loadFile(const OFFilename &filename,
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

DRTFractionGroupSequence::Item& DRTPlan::getFractionGroup(Sint32 fractionGroupNumber)
{
    // First some typedefs to make the following more readable
    typedef DRTFractionGroupSequence seq;
    typedef DRTSearch::searchPropertyEqualHelper<seq, seq::Item, Sint32, const unsigned long, 0> helper;
    return helper::search(getFractionGroupSequence(), fractionGroupNumber,
            &seq::Item::getFractionGroupNumber);
}

DRTBeamSequence::Item& DRTPlan::getBeam(Sint32 beamNumber)
{
    // First some typedefs to make the following more readable
    typedef DRTBeamSequence seq;
    typedef DRTSearch::searchPropertyEqualHelper<seq, seq::Item, Sint32, const unsigned long, 0> helper;
    return helper::search(getBeamSequence(), beamNumber,
            &seq::Item::getBeamNumber);
}

DRTDoseReferenceSequence::Item& DRTPlan::getDoseReference(Sint32 referenceNumber)
{
    // First some typedefs to make the following more readable
    typedef DRTDoseReferenceSequence seq;
    typedef DRTSearch::searchPropertyEqualHelper<seq, seq::Item, Sint32, const unsigned long, 0> helper;
    return helper::search(getDoseReferenceSequence(), referenceNumber,
            &seq::Item::getDoseReferenceNumber);
}

DRTToleranceTableSequence::Item& DRTPlan::getToleranceTable(Sint32 tableNumber)
{
    // First some typedefs to make the following more readable
    typedef DRTToleranceTableSequence seq;
    typedef DRTSearch::searchPropertyEqualHelper<seq, seq::Item, Sint32, const unsigned long, 0> helper;
    return helper::search(getToleranceTableSequence(), tableNumber,
            &seq::Item::getToleranceTableNumber);
}

DRTPatientSetupSequence::Item& DRTPlan::getPatientSetup(Sint32 setupNumber)
{
    // First some typedefs to make the following more readable
    typedef DRTPatientSetupSequence seq;
    typedef DRTSearch::searchPropertyEqualHelper<seq, seq::Item, Sint32, const unsigned long, 0> helper;
    return helper::search(getPatientSetupSequence(), setupNumber,
            &seq::Item::getPatientSetupNumber);
}

DRTApplicationSetupSequence::Item& DRTPlan::getApplicationSetup(Sint32 setupNumber)
{
    // First some typedefs to make the following more readable
    typedef DRTApplicationSetupSequence seq;
    typedef DRTSearch::searchPropertyEqualHelper<seq, seq::Item, Sint32, const unsigned long, 0> helper;
    return helper::search(getApplicationSetupSequence(), setupNumber,
            &seq::Item::getApplicationSetupNumber);
}
