/*
 *
 *  Copyright (C) 2019-2022, J. Riesmeier, Oldenburg, Germany
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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for class DcmSequence and DcmPixelSequence
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"

#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcpixseq.h"
#include "dcmtk/dcmdata/dcdeftag.h"


#define NUMBER_OF_ITEMS 99999


OFTEST(dcmdata_sequenceInsert)
{
    DcmSequenceOfItems sequence(DCM_OtherPatientIDsSequence);
    /* add a large number of items to the sequence */
    unsigned long counter = 0;
    for (unsigned long i = 0; i < NUMBER_OF_ITEMS; ++i)
    {
        if (sequence.insert(new DcmItem()).good())
            ++counter;
    }
    /* check whether that worked (for-loop shouldn't take too long) */
    OFCHECK_EQUAL(counter, NUMBER_OF_ITEMS);
    /* access specific items (performance should be no issue) */
    OFCHECK(sequence.getItem(0) != NULL);
    OFCHECK(sequence.getItem(2) != NULL);
    OFCHECK(sequence.getItem(NUMBER_OF_ITEMS) == NULL);
    OFCHECK(sequence.getItem(NUMBER_OF_ITEMS - 1) != NULL);
    OFCHECK(sequence.getItem(NUMBER_OF_ITEMS - 2) != NULL);
}


OFTEST(dcmdata_pixelSequenceInsert)
{
    DcmPixelItem *pixelItem = NULL;
    DcmPixelSequence pixelSequence(DCM_PixelData);
    /* add a large number of items to the sequence */
    unsigned long counter = 0;
    for (unsigned long i = 0; i < NUMBER_OF_ITEMS; ++i)
    {
        if (pixelSequence.insert(new DcmPixelItem(DCM_PixelItemTag)).good())
            ++counter;
    }
    /* check whether that worked (for-loop shouldn't take too long) */
    OFCHECK_EQUAL(counter, NUMBER_OF_ITEMS);
    /* access specific items (performance should be no issue) */
    OFCHECK(pixelSequence.getItem(pixelItem, 0).good());
    OFCHECK(pixelSequence.getItem(pixelItem, 2).good());
    OFCHECK(pixelSequence.getItem(pixelItem, NUMBER_OF_ITEMS).bad());
    OFCHECK(pixelSequence.getItem(pixelItem, NUMBER_OF_ITEMS - 1).good());
    OFCHECK(pixelSequence.getItem(pixelItem, NUMBER_OF_ITEMS - 2).good());
}


OFTEST(dcmdata_findAndGetSequenceItem)
{
    DcmDataset dataset;
    DcmItem *item = NULL;
    /* first, create a dataset with a sequence and a pixel sequence */
    OFCHECK(dataset.insertSequenceItem(DCM_OtherPatientIDsSequence, new DcmItem()).good());
    OFCHECK(dataset.insertSequenceItem(DCM_PixelData, new DcmItem()).good());
    /* try to retrieve the item from the sequence elements */
    OFCHECK(dataset.findAndGetSequenceItem(DCM_OtherPatientIDsSequence, item).good());
    OFCHECK(dataset.findAndGetSequenceItem(DCM_PixelData, item).good());
    /* the following should fail */
    OFCHECK(dataset.findAndGetSequenceItem(DCM_OtherPatientIDsSequence, item, 1).bad());
    OFCHECK(dataset.findAndGetSequenceItem(DCM_PixelData, item, 1).bad());
    /* then, try to create the items by some other means */
    OFCHECK(dataset.findOrCreateSequenceItem(DCM_OtherPatientIDsSequence, item, 1).good());
    OFCHECK(dataset.findOrCreateSequenceItem(DCM_PixelData, item, 1).good());
    /* now, it should work */
    OFCHECK(dataset.findAndGetSequenceItem(DCM_OtherPatientIDsSequence, item, 1).good());
    OFCHECK(dataset.findAndGetSequenceItem(DCM_PixelData, item, 1).good());
}
