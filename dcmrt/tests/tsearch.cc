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
 *  Purpose: test program for the search helper classes
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmrt/drmplan.h"


OFTEST(dcmrt_search)
{
    DRTPlan plan;
    DRTBeamSequence &seq = plan.getBeamSequence();
    DRTBeamSequence::Item *item1, *item2, *item3;
    // Get the empty default item
    DRTBeamSequence::Item &empty = seq.getCurrentItem();

    // Add some elements
    OFCHECK(seq.addItem(item1).good());
    OFCHECK(item1->setBeamNumber("42").good());
    OFCHECK(seq.addItem(item2).good());
    OFCHECK(item2->setBeamNumber("1337").good());
    OFCHECK(seq.addItem(item3).good());
    OFCHECK(item3->setBeamNumber("21").good());

    // Check if finding works correctly
    OFCHECK_EQUAL(&plan.getBeam(1337), item2);
    OFCHECK_EQUAL(&plan.getBeam(42), item1);
    OFCHECK_EQUAL(&plan.getBeam(21), item3);
    OFCHECK_EQUAL(&plan.getBeam(100), &empty);
}
