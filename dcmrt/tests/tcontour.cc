/*
 *
 *  Copyright (C) 2021, J. Riesmeier, Oldenburg, Germany
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
 *  Module:  dcmrt
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for adding contour data to an RT Structure Set
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmrt/drtstrct.h"


OFTEST(dcmrt_contour)
{
    DRTStructureSetIOD structureSet;
    DRTROIContourSequence &roiContourSequence = structureSet.getROIContourSequence();
    DRTROIContourSequence::Item *roiContourSequenceItem = NULL;

    // add new item
    OFCHECK(roiContourSequence.addItem(roiContourSequenceItem).good());
    if (roiContourSequenceItem != NULL)
    {
        DRTContourSequence &contourSequence = roiContourSequenceItem->getContourSequence();
        DRTContourSequence::Item *contourSequenceItem = NULL;
        // also add a new item to the nested sub-sequence
        OFCHECK(contourSequence.addItem(contourSequenceItem).good());
        if (roiContourSequenceItem != NULL)
        {
            // set contour data (dummy values)
            OFCHECK(contourSequenceItem->setContourData("100\\200\\146\\101\\201\\147").good());
        }
    }

    // check if contour data was set correctly
    OFVector<Float64> contourData;
    OFCHECK(structureSet.getROIContourSequence().getItem(0).getContourSequence().getItem(0).getContourData(contourData).good());
    OFCHECK_EQUAL(contourData.size(), 6);
    if (contourData.size() >= 6)
    {
        OFCHECK_EQUAL(contourData.at(0), 100);
        OFCHECK_EQUAL(contourData.at(1), 200);
        OFCHECK_EQUAL(contourData.at(2), 146);
        OFCHECK_EQUAL(contourData.at(3), 101);
        OFCHECK_EQUAL(contourData.at(4), 201);
        OFCHECK_EQUAL(contourData.at(5), 147);
    }
}
