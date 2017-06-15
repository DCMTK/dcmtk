/*
 *
 *  Copyright (C) 2012-2017, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: test program for parent element handling
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dctmacro.h"


/* To switch between big and little endian, just change these macros */
#define TRANSFER_SYNTAX EXS_LittleEndianExplicit
#define ENDIAN_UINT16(w) LITTLE_ENDIAN_UINT16(w)
#define ENDIAN_UINT32(w) LITTLE_ENDIAN_UINT32(w)


// This checks if each object's parent is set correctly. This should be called
// as testParent(fileFormat); because other parameters are just used internally.
static void testParents(DcmObject& item, DcmObject *root = NULL, DcmObject *parent = NULL)
{
    OFCHECK_EQUAL(item.getParent(), parent);

    // DcmFileFormat doesn't count as root, but DcmMetaInfo and DcmDataset do.
    // Thus we need some magic to check against them as root.
    if (root != NULL)
    {
        DcmItem *pItem = item.getParentItem();
        OFCHECK(pItem == parent || pItem == parent->getParent());
        if (pItem)
        {
            DcmEVR ident = pItem->ident();
            OFCHECK(ident == EVR_item || ident == EVR_dataset
                || ident == EVR_metainfo || ident == EVR_dirRecord);
        }

        OFCHECK_EQUAL(item.getRootItem(), root);
    }
    else
    {
        OFCHECK(item.getRootItem() == NULL);
        OFCHECK(item.getParentItem() == NULL);
    }

    // iterate over all children and check them, too
    DcmObject *obj = NULL;
    while ((obj = item.nextInContainer(obj)) != NULL)
        testParents(*obj, root == NULL ? obj : root, &item);
}

OFTEST(dcmdata_elementParent)
{
    DcmFileFormat ff;
    DcmItem *item = NULL;
    DcmElement *elem = NULL;
    DcmSequenceOfItems *seq = NULL;
    OFCondition cond = EC_Normal;

    testParents(ff);

    // Some test dataset that we use to verify that reading works
    const Uint8 testData[] = {
        TAG_AND_LENGTH(DCM_DirectoryRecordSequence, 'S', 'Q', UNDEFINED_LENGTH),
          ITEM(UNDEFINED_LENGTH),
            TAG_AND_LENGTH_SHORT(DCM_OffsetOfTheNextDirectoryRecord, 'U', 'L', 4),
            0x42, 0x00, 0x00, 0x00,
          ITEM_END,
        SEQUENCE_END,
        TAG_AND_LENGTH_SHORT(DCM_PatientName, 'P', 'N', 4),
        'T', 'e', 's', 't',
        TAG_AND_LENGTH(DCM_ConceptNameCodeSequence, 'S', 'Q', UNDEFINED_LENGTH),
          ITEM(UNDEFINED_LENGTH),
            TAG_AND_LENGTH_SHORT(DCM_CodeValue, 'S', 'H', 6),
            'D', 'T', '.', '0', '6', ' ',
          ITEM_END,
        SEQUENCE_END,
        TAG_AND_LENGTH(DCM_VerifyingObserverSequence, 'S', 'Q', UNDEFINED_LENGTH),
          ITEM(UNDEFINED_LENGTH),
            TAG_AND_LENGTH_SHORT(DCM_VerificationDateTime, 'D', 'T', 8),
            '2', '0', '1', '2', '0', '4', '2', '3',
            TAG_AND_LENGTH(DCM_VerifyingObserverIdentificationCodeSequence, 'S', 'Q', UNDEFINED_LENGTH),
              ITEM(UNDEFINED_LENGTH),
                TAG_AND_LENGTH_SHORT(DCM_CodeValue, 'S', 'H', 10),
                'U', 'S', 'R', '.', '0', '0', '0', '0', '2', ' ',
              ITEM_END,
            SEQUENCE_END,
          ITEM_END,
        SEQUENCE_END,
        TAG_AND_LENGTH(DCM_PixelData, 'O', 'B', UNDEFINED_LENGTH),
          ITEM(8), 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
          ITEM(8), 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
          ITEM(2), 0x10, 0x2a,
        SEQUENCE_END
    };

    cond = readDataset(*ff.getDataset(), testData, sizeof(testData), EXS_LittleEndianExplicit);
    OFCHECK(cond.good());
    testParents(ff);

    // Now test the insert/remove methods directly
    DcmDataset *dset = ff.getAndRemoveDataset();
    OFCHECK(dset->getParent() == NULL);
    delete dset;
    testParents(ff);
    dset = ff.getDataset();

    elem = new DcmPersonName(DCM_PatientName);
    OFCHECK(elem->getParent() == NULL);
    cond = dset->insert(elem);
    OFCHECK(cond.good());
    OFCHECK(elem->getParent() == dset);
    testParents(ff);

    seq = new DcmSequenceOfItems(DCM_OtherPatientIDsSequence);
    OFCHECK(seq->getParent() == NULL);
    cond = dset->insert(seq);
    OFCHECK(cond.good());
    OFCHECK(seq->getParent() == dset);
    testParents(ff);

    item = new DcmItem(DCM_Item);
    OFCHECK(item->getParent() == NULL);
    cond = seq->insert(item);
    OFCHECK(cond.good());
    OFCHECK(item->getParent() == seq);
    testParents(ff);

    elem = new DcmLongString(DCM_RETIRED_OtherPatientIDs);
    OFCHECK(elem->getParent() == NULL);
    cond = item->insert(elem);
    OFCHECK(cond.good());
    OFCHECK(elem->getParent() == item);
    testParents(ff);

    item = new DcmItem(DCM_Item);
    OFCHECK(item->getParent() == NULL);
    cond = seq->insert(item);
    OFCHECK(cond.good());
    OFCHECK(item->getParent() == seq);
    testParents(ff);

    elem = new DcmLongString(DCM_RETIRED_OtherPatientIDs);
    OFCHECK(elem->getParent() == NULL);
    cond = item->insert(elem);
    OFCHECK(cond.good());
    OFCHECK(elem->getParent() == item);
    testParents(ff);

    elem = new DcmPersonName(DCM_PatientName);
    OFCHECK(elem->getParent() == NULL);
    cond = item->insert(elem);
    OFCHECK(cond.good());
    OFCHECK(elem->getParent() == item);
    testParents(ff);

    elem = item->remove(1);
    OFCHECK_EQUAL(elem->getTag(), DCM_RETIRED_OtherPatientIDs);
    OFCHECK(elem->getParent() == NULL);
    delete elem;
    testParents(ff);

    elem = item->remove(DCM_PatientName);
    OFCHECK_EQUAL(elem->getTag(), DCM_PatientName);
    OFCHECK(elem->getParent() == NULL);
    delete elem;
    testParents(ff);
}
