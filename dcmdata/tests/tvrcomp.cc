/*
 *
 *  Copyright (C) 2015-2020, OFFIS e.V.
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
 *  Author:  Michael Onken
 *
 *  Purpose: tests for compare() operator of VR classes
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvras.h"
#include "dcmtk/dcmdata/dcvrae.h"
#include "dcmtk/dcmdata/dcchrstr.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrlt.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrst.h"
#include "dcmtk/dcmdata/dcvrur.h"
#include "dcmtk/dcmdata/dcvrut.h"
#include "dcmtk/dcmdata/dcvras.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmdata/dcvrod.h"
#include "dcmtk/dcmdata/dcvrof.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrfd.h"
#include "dcmtk/dcmdata/dcvrfl.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcvrol.h"
#include "dcmtk/dcmdata/dcvrpobw.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmdata/dcvrsl.h"
#include "dcmtk/dcmdata/dcvrss.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvruc.h"
#include "dcmtk/dcmdata/dcvrul.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcvrpobw.h"
#include "dcmtk/dcmdata/dcovlay.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcpixseq.h"
#include "dcmtk/dcmdata/dcdict.h"


template <typename StringType>
static void checkStringBased(
    const DcmTagKey& key,
    const OFString& shortVal,
    const OFString& longVal)
{
    // Start with equal values
    StringType obj1(key);
    StringType obj2(key);
    StringType objOtherTag(DCM_UndefinedTagKey);
    obj1.putOFStringArray(shortVal);
    obj2.putOFStringArray(shortVal);

    // Check equality
    OFCHECK_EQUAL(obj1.compare(obj2), 0);
    OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
    // Reverse test should yield same result
    OFCHECK_EQUAL(obj2.compare(obj1), 0);
    OFCHECK( (obj2 >= obj1) && (obj1 >= obj2) );

    // Check differing tags (DCM_UndefinedTagKey always smaller than any other key)
    objOtherTag.putOFStringArray(shortVal /* same value*/);
    OFCHECK(obj1.compare(objOtherTag) < 0);
    OFCHECK(obj1 < objOtherTag);
    OFCHECK(obj1 <= objOtherTag);
    // Reverse test should yield opposite result
    OFCHECK(objOtherTag.compare(obj1) > 0);
    OFCHECK(objOtherTag > obj1);
    OFCHECK(objOtherTag >= obj1);

    // Check second string longer
    obj2.putOFStringArray(longVal);
    OFCHECK(obj1.compare(obj2) < 0);
    OFCHECK(obj1 < obj2);
    OFCHECK(obj1 <= obj2);
    // Reverse test should yield opposite result
    OFCHECK(obj2.compare(obj1) > 0);
    OFCHECK(obj2 > obj1);
    OFCHECK(obj2 >= obj1);

    // Test different number of values
    OFString twoValues, threeValues;
    twoValues = shortVal + "\\"; twoValues += shortVal;
    threeValues = twoValues + "\\"; threeValues += shortVal;
    obj1.putOFStringArray(twoValues);
    obj2.putOFStringArray(threeValues);
    OFCHECK(obj1.compare(obj2) < 0);
    OFCHECK(obj1 < obj2);
    OFCHECK(obj1 <= obj2);
    // Reverse test should yield opposite result
    OFCHECK(obj2.compare(obj1) > 0);
    OFCHECK(obj2 > obj1);
    OFCHECK(obj2 >= obj1);
}


static void checkAttributeTags()
{
    // Start with equal values
    DcmAttributeTag obj1(DCM_FrameIncrementPointer);
    DcmAttributeTag obj2(DCM_FrameIncrementPointer);
    DcmAttributeTag objOtherTag(DCM_UndefinedTagKey);
    obj1.putTagVal(DCM_PatientName);
    obj2.putTagVal(DCM_PatientName);

    // Check equality
    OFCHECK_EQUAL(obj1.compare(obj2), 0);
    OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
    // Reverse test should yield same result
    OFCHECK_EQUAL(obj2.compare(obj1), 0);
    OFCHECK( (obj2 >= obj1) && (obj1 >= obj2) );

    // Check differing tags (DCM_UndefinedTagKey always smaller than any other key)
    objOtherTag.putTagVal(DCM_PatientName);
    OFCHECK(obj1.compare(objOtherTag) < 0);
    OFCHECK(obj1 < objOtherTag);
    OFCHECK(obj1 <= objOtherTag);
    // Reverse test should yield opposite result
    OFCHECK(objOtherTag.compare(obj1) > 0);
    OFCHECK(objOtherTag > obj1);
    OFCHECK(objOtherTag >= obj1);

    // Check second attribute larger
    obj2.putTagVal(DCM_PatientID);
    OFCHECK(obj1.compare(obj2) < 0);
    OFCHECK(obj1 < obj2);
    OFCHECK(obj1 <= obj2);
    // Reverse test should yield opposite result
    OFCHECK(obj2.compare(obj1) > 0);
    OFCHECK(obj2 > obj1);
    OFCHECK(obj2 >= obj1);

    // Test different VM
    obj1.putTagVal(DCM_PatientName, 1); // VM = 2
    obj2.putTagVal(DCM_PatientName, 0);
    obj2.putTagVal(DCM_PatientName, 1);
    obj2.putTagVal(DCM_PatientName, 2); // VM = 3

    OFCHECK(obj1.compare(obj2) < 0);
    OFCHECK(obj1 < obj2);
    OFCHECK(obj1 <= obj2);
    // Reverse test should yield opposite result
    OFCHECK(obj2.compare(obj1) > 0);
    OFCHECK(obj2 > obj1);
    OFCHECK(obj2 >= obj1);
}


static void checkOtherByteOtherWord()
{
  // Start with equal values
  DcmOtherByteOtherWord obj1(DCM_PixelData);
  DcmOtherByteOtherWord obj2(DCM_PixelData);
  DcmOtherByteOtherWord objOtherTag(DCM_UndefinedTagKey);

  // Check equality
  Uint8 testData[1] = {100};
  obj1.putUint8Array(testData, 1);
  obj2.putUint8Array(testData, 1);
  OFCHECK_EQUAL(obj1.compare(obj2), 0);
  OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
  // Reverse test should yield same result
  OFCHECK_EQUAL(obj2.compare(obj1), 0);
  OFCHECK( (obj2 >= obj1) && (obj1 >= obj2) );

  // Check differing tags (DCM_UndefinedTagKey always smaller than any other key)
  objOtherTag.putUint8Array(testData, 1);
  OFCHECK(obj1.compare(objOtherTag) < 0);
  OFCHECK(obj1 < objOtherTag);
  OFCHECK(obj1 <= objOtherTag);
  // Reverse test should yield opposite result
  OFCHECK(objOtherTag.compare(obj1) > 0);
  OFCHECK(objOtherTag > obj1);
  OFCHECK(objOtherTag >= obj1);

  // Check second attribute larger
  Uint8 biggerTestData[1] = {101};
  obj2.putUint8Array(biggerTestData, 1);
  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);

  // Test different VM
  Uint8 largeVMTestData[2] = {100, 101};
  Uint8 evenLargerVMTestData[3] = {100, 101, 102};
  obj1.putUint8Array(largeVMTestData, 2);      // VM = 2
  obj2.putUint8Array(evenLargerVMTestData, 3); // VM = 3

  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);
}


static void checkPolymorphOtherByteOtherWord()
{
  // Start comparing OB with OB
  DcmPolymorphOBOW obj1_ob(DCM_PixelData);
  DcmPolymorphOBOW obj2_ob(DCM_PixelData);
  obj1_ob.setVR(EVR_OB);
  obj2_ob.setVR(EVR_OB);

  OFCHECK(obj1_ob.compare(obj2_ob) == 0);
  OFCHECK(obj1_ob >= obj2_ob);
  OFCHECK(obj1_ob <= obj2_ob);
  OFCHECK(obj2_ob >= obj1_ob);
  OFCHECK(obj2_ob <= obj2_ob);

  // Compare with same values (still equal)
  Uint8 testData_100[1] = {100};
  OFCHECK(obj1_ob.putUint8Array(testData_100, 1).good());
  OFCHECK(obj2_ob.putUint8Array(testData_100, 1).good());
  OFCHECK(obj1_ob.compare(obj2_ob) == 0);
  OFCHECK(obj1_ob >= obj2_ob);
  OFCHECK(obj1_ob <= obj2_ob);
  OFCHECK(obj2_ob >= obj1_ob);
  OFCHECK(obj2_ob <= obj2_ob);
  OFCHECK(!(obj1_ob > obj2_ob));
  OFCHECK(!(obj1_ob < obj2_ob));


  // Compare with different values (single value, obj1 < obj2)
  Uint8 testData_200[1] = {200};
  OFCHECK(obj1_ob.putUint8Array(testData_100, 1).good());
  OFCHECK(obj2_ob.putUint8Array(testData_200, 1).good());
  OFCHECK(obj1_ob.compare(obj2_ob) == -1);
  OFCHECK(obj1_ob < obj2_ob);
  OFCHECK(obj1_ob <= obj2_ob);

  // Compare with different number of values (less values, i.e. obj1, means smaller)
  Uint8 testData_100_200[2] = {100,200};
  OFCHECK(obj1_ob.putUint8Array(testData_100, 1).good());
  OFCHECK(obj2_ob.putUint8Array(testData_100_200, 2).good());
  OFCHECK(obj1_ob.compare(obj2_ob) < 0);
  OFCHECK(obj1_ob < obj2_ob);
  OFCHECK(obj1_ob <= obj2_ob);

  // Compare values (same multiple values means equal)
  OFCHECK(obj1_ob.putUint8Array(testData_100_200, 2).good());
  OFCHECK(obj2_ob.putUint8Array(testData_100_200, 2).good());
  OFCHECK(obj1_ob.compare(obj2_ob) == 0);
  OFCHECK(!(obj1_ob < obj2_ob));
  OFCHECK(!(obj2_ob < obj2_ob));
  OFCHECK(obj1_ob <= obj2_ob);

  // Compare values (same number of values but different values means not equal, obj1 < obj2)
  Uint8 testData_100_201[2] = {100,201};
  OFCHECK(obj1_ob.putUint8Array(testData_100_200, 2).good());
  OFCHECK(obj2_ob.putUint8Array(testData_100_201, 2).good());
  OFCHECK(obj1_ob.compare(obj2_ob) == -1);
  OFCHECK(obj1_ob < obj2_ob);
  OFCHECK(!(obj2_ob < obj2_ob));
  OFCHECK(obj1_ob <= obj2_ob);
}


static void checkDcmPixelDataNative()
{
  // Testing DcmPixelData having uncompressed pixel data.
  // The tests here finally boil down in the usage of
  // DcmPolymorphOBOW::compare() which is called within
  // DcmPixelData::compare()

  // -------------------------------
  // Start with comparing OB with OB
  // -------------------------------

  DcmPixelData obj1_ob(DCM_PixelData);
  DcmPixelData obj2_ob(DCM_PixelData);
  obj1_ob.setVR(EVR_OB);
  obj2_ob.setVR(EVR_OB);

  // Compare empty
  OFCHECK(obj1_ob.compare(obj2_ob) == 0);
  OFCHECK(obj1_ob >= obj2_ob);
  OFCHECK(obj2_ob >= obj1_ob);

  // Compare with same values (still equal), uncompressed
  Uint8 testData_100[1] = {100};
  OFCHECK(obj1_ob.putUint8Array(testData_100, 1).good());
  OFCHECK(obj2_ob.putUint8Array(testData_100, 1).good());
  OFCHECK(obj1_ob.compare(obj2_ob) == 0);
  OFCHECK(obj1_ob >= obj2_ob);
  OFCHECK(obj2_ob >= obj1_ob);
  OFCHECK(!(obj1_ob > obj2_ob));
  OFCHECK(!(obj1_ob < obj2_ob));

  // Compare with different values (single value, obj1 < obj2)
  Uint8 testData_200[1] = {200};
  OFCHECK(obj1_ob.putUint8Array(testData_100, 1).good());
  OFCHECK(obj2_ob.putUint8Array(testData_200, 1).good());
  OFCHECK(obj1_ob.compare(obj2_ob) == -1);
  OFCHECK(obj1_ob < obj2_ob);
  OFCHECK(obj1_ob <= obj2_ob);

  // Compare with different number of values (less values, i.e. obj1, means smaller)
  Uint8 testData_100_200[2] = {100,200};
  OFCHECK(obj1_ob.putUint8Array(testData_100, 1).good());
  OFCHECK(obj2_ob.putUint8Array(testData_100_200, 2).good());
  OFCHECK(obj1_ob.compare(obj2_ob) < 0);
  OFCHECK(obj1_ob < obj2_ob);
  OFCHECK(obj1_ob <= obj2_ob);

  // Compare values (same multiple values means equal)
  OFCHECK(obj1_ob.putUint8Array(testData_100_200, 2).good());
  OFCHECK(obj2_ob.putUint8Array(testData_100_200, 2).good());
  OFCHECK(obj1_ob.compare(obj2_ob) == 0);
  OFCHECK(!(obj1_ob < obj2_ob));
  OFCHECK(!(obj2_ob < obj2_ob));
  OFCHECK(obj1_ob <= obj2_ob);

  // Compare values (same number of values but different values means not equal, obj1 < obj2)
  Uint8 testData_100_201[2] = {100,201};
  OFCHECK(obj1_ob.putUint8Array(testData_100_200, 2).good());
  OFCHECK(obj2_ob.putUint8Array(testData_100_201, 2).good());
  OFCHECK(obj1_ob.compare(obj2_ob) == -1);
  OFCHECK(obj1_ob < obj2_ob);
  OFCHECK(!(obj2_ob < obj2_ob));
  OFCHECK(obj1_ob <= obj2_ob);

  // -------------------------------
  // Continue comparing OW with OW
  // -------------------------------

  DcmPixelData obj1_ow(DCM_PixelData);
  DcmPixelData obj2_ow(DCM_PixelData);
  obj1_ow.setVR(EVR_OW);
  obj2_ow.setVR(EVR_OW);

  // Compare empty (equal)
  OFCHECK(obj1_ow.compare(obj2_ow) == 0);
  OFCHECK(obj1_ow >= obj2_ow);
  OFCHECK(obj2_ow >= obj1_ow);

  // Compare with same values (still equal), uncompressed
  Uint16 testData_1000[1] = {0x1000};
  OFCHECK(obj1_ow.putUint16Array(testData_1000, 1).good());
  OFCHECK(obj2_ow.putUint16Array(testData_1000, 1).good());
  OFCHECK(obj1_ow.compare(obj2_ow) == 0);
  OFCHECK(obj1_ow >= obj2_ow);
  OFCHECK(obj2_ow >= obj1_ow);

  // Compare with different values (single value)
  Uint16 testData_2000[1] = {0x2000};
  OFCHECK(obj1_ow.putUint16Array(testData_1000, 1).good());
  OFCHECK(obj2_ow.putUint16Array(testData_2000, 1).good());
  OFCHECK(obj1_ow.compare(obj2_ow) == -1);
  OFCHECK(obj1_ow < obj2_ow);

  // Compare with different number of values (less values, i.e. obj1, means smaller)
  Uint16 testData_1000_2000[2] = {0x1000,0x2000};
  OFCHECK(obj1_ow.putUint16Array(testData_1000, 1).good());
  OFCHECK(obj2_ow.putUint16Array(testData_1000_2000, 2).good());
  OFCHECK(obj1_ow.compare(obj2_ow) < 0);
  OFCHECK(obj1_ow < obj2_ow);

  // Compare values (same multiple values means equal)
  OFCHECK(obj1_ow.putUint16Array(testData_1000_2000, 2).good());
  OFCHECK(obj2_ow.putUint16Array(testData_1000_2000, 2).good());
  OFCHECK(obj1_ow.compare(obj2_ow) == 0);
  OFCHECK(!(obj1_ow < obj2_ow));
  OFCHECK(!(obj2_ow < obj2_ow));
  OFCHECK(obj1_ow <= obj2_ow);

  // Compare values (same number of values but different values means not equal, obj1 < obj2)
  Uint16 testData_1000_2001[2] = {0x1000,0x2001};
  OFCHECK(obj1_ow.putUint16Array(testData_1000_2000, 2).good());
  OFCHECK(obj2_ow.putUint16Array(testData_1000_2001, 2).good());
  OFCHECK(obj1_ow.compare(obj2_ow) == -1);
  OFCHECK(obj1_ow < obj2_ow);
  OFCHECK(!(obj2_ow < obj2_ow));
  OFCHECK(obj1_ow <= obj2_ow);


  // Continue with comparison of OB with OW
  Uint8 testData8_10[1] = {0x10};
  Uint16 testData16_0010[1] = {0x0010};
  obj1_ob.putUint8Array(testData8_10, 1);
  obj2_ow.putUint16Array(testData16_0010, 1);
  // OB and OW with same value should be equal
  // (comparison takes place bytewise in little endian)
  OFCHECK(obj1_ob.compare(obj2_ow) == 0);
  OFCHECK(obj1_ob >= obj2_ow);
  OFCHECK(obj2_ow >= obj1_ob);

  // Compare with different values
  Uint16 testData16_0011[1] = {0x0011};
  OFCHECK(obj1_ob.putUint8Array(testData8_10, 1).good());
  OFCHECK(obj2_ow.putUint16Array(testData16_0011, 1).good());
  OFCHECK(obj1_ob.compare(obj2_ow) < 0);
  OFCHECK(obj1_ob < obj2_ow);

  // Compare with different number of values (less values, i.e. obj1, means smaller)
  Uint16 testData16_0010_0020[2] = {0x0010,0x0020};
  OFCHECK(obj1_ob.putUint8Array(testData8_10, 1).good());
  OFCHECK(obj2_ow.putUint16Array(testData16_0010_0020, 2).good());
  OFCHECK(obj1_ow.compare(obj2_ow) < 0);
  OFCHECK(obj1_ow < obj2_ow);
}


static void checkDcmPixelDataEncapsulatedOB()
{
  DcmPixelData obj1_ob(DCM_PixelData);
  DcmPixelData obj2_ob(DCM_PixelData);
  obj1_ob.setVR(EVR_OB);
  obj2_ob.setVR(EVR_OB);
  Uint8 testData8_1010[2] = {0x10,0x10};

  // (Compare empty already om checkDcmPixelDataNative())

  // --------------------------------------------
  // Compare with same current representation
  // --------------------------------------------

  DcmPixelSequence *pixseq1 = new DcmPixelSequence(DCM_PixelData);
  OFCHECK(pixseq1 != NULL);
  DcmOffsetList offsetList;
  DcmPixelItem *offsetItem = new DcmPixelItem(DCM_PixelItemTag);
  OFCHECK(offsetItem != NULL);
  // First item is always offset table
  OFCHECK(pixseq1->insert(offsetItem).good());
  OFCHECK(pixseq1->storeCompressedFrame(offsetList, testData8_1010, 2, 0).good());
  obj1_ob.putOriginalRepresentation(EXS_JPEGProcess14SV1, NULL, pixseq1);

  // Compare objects with two equal single frames
  DcmPixelSequence *pixseq2 = new DcmPixelSequence(DCM_PixelData);
  OFCHECK(pixseq2 != NULL);
  offsetItem = new DcmPixelItem(DCM_PixelItemTag);
  OFCHECK(offsetItem != NULL);
  OFCHECK(pixseq2->insert(offsetItem).good());
  OFCHECK(pixseq2->storeCompressedFrame(offsetList, testData8_1010, 2, 0).good());
  obj2_ob.putOriginalRepresentation(EXS_JPEGProcess14SV1, NULL, pixseq2);
  OFCHECK(obj1_ob.compare(obj2_ob) == 0);
  OFCHECK(obj1_ob >= obj2_ob);

  // Compare object with single frame with one with no frame
  DcmPixelItem* remove = NULL;
  OFCHECK(pixseq2->remove(remove, 1).good()); // remove frame 1
  OFCHECK(obj1_ob.compare(obj2_ob) > 0 );

  // Compare object with single frame, and with two frames
  OFCHECK(pixseq2->insert(remove, 1).good()); // insert frame 1 back in again
  OFCHECK(pixseq2->storeCompressedFrame(offsetList, testData8_1010, 2, 0).good());
  OFCHECK(obj1_ob.compare(obj2_ob) < 0); //1 frames < 2 frames

  // Compare objects, both having two frames, with the second frame being different
  Uint8 testData8_0505[2] = {0x05,0x05};
  OFCHECK(pixseq1->storeCompressedFrame(offsetList, testData8_0505, 2, 0).good());
  OFCHECK(obj1_ob.compare(obj2_ob) < 0);

  // --------------------------------------------
  // Compare with different current representation
  // --------------------------------------------

  DcmPixelData obj3_ob(DCM_PixelData);
  obj3_ob.setVR(EVR_OB);
  DcmPixelSequence *pixseq3 = new DcmPixelSequence(DCM_PixelData);
  OFCHECK(pixseq3 != NULL);
  offsetItem = new DcmPixelItem(DCM_PixelItemTag);
  OFCHECK(offsetItem != NULL);
  OFCHECK(pixseq3->insert(offsetItem).good());
  // Add same data as in pixseq1
  OFCHECK(pixseq3->storeCompressedFrame(offsetList, testData8_1010, 2, 0).good());
  OFCHECK(pixseq3->storeCompressedFrame(offsetList, testData8_0505, 2, 0).good());
  // But insert and pretend that is RLE
  obj3_ob.putOriginalRepresentation(EXS_RLELossless, NULL, pixseq3);
  // compare JPEG (pixseq1), with RLE (pixseq3)
  OFCHECK(obj1_ob.compare(obj3_ob) == 1); // different current representation returns 1
}


static void checkSignedLong()
{
  // Start with equal values
  DcmSignedLong obj1(DCM_SelectorSLValue);
  DcmSignedLong obj2(DCM_SelectorSLValue);
  DcmSignedLong objOtherTag(DCM_UndefinedTagKey);

  // Check equality
  obj1.putSint32(-10);
  obj2.putSint32(-10);
  OFCHECK_EQUAL(obj1.compare(obj2), 0);
  OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
  // Reverse test should yield same result
  OFCHECK_EQUAL(obj2.compare(obj1), 0);
  OFCHECK( (obj2 >= obj1) && (obj1 >= obj2) );

  // Check differing tags (DCM_UndefinedTagKey always smaller than any other key)
  objOtherTag.putSint32(-10);
  OFCHECK(obj1.compare(objOtherTag) < 0);
  OFCHECK(obj1 < objOtherTag);
  // Reverse test should yield opposite result
  OFCHECK(objOtherTag.compare(obj1) > 0);
  OFCHECK(objOtherTag > obj1);
  OFCHECK(objOtherTag >= obj1);

  // Check second attribute larger
  obj2.putSint32(-5);
  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);

  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);

  // Test different VM
  obj1.putSint32(-10);     // VM = 2
  obj2.putSint32(-10, 0);
  obj2.putSint32(-10, 1);
  obj2.putSint32(-10, 2); // VM = 3

  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);
}


static void checkSignedShort()
{
  // Start with equal values
  DcmSignedShort obj1(DCM_SelectorSSValue);
  DcmSignedShort obj2(DCM_SelectorSSValue);
  DcmSignedShort objOtherTag(DCM_UndefinedTagKey);

  // Check equality
  obj1.putSint16(-10);
  obj2.putSint16(-10);
  OFCHECK_EQUAL(obj1.compare(obj2), 0);
  OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
  // Reverse test should yield same result
  OFCHECK_EQUAL(obj2.compare(obj1), 0);
  OFCHECK( (obj2 >= obj1) && (obj1 >= obj2) );

  // Check differing tags (DCM_UndefinedTagKey always smaller than any other key)
  objOtherTag.putSint16(-10);
  OFCHECK(obj1.compare(objOtherTag) < 0);
  OFCHECK(obj1 < objOtherTag);
  OFCHECK(obj1 <= objOtherTag);
  // Reverse test should yield opposite result
  OFCHECK(objOtherTag.compare(obj1) > 0);
  OFCHECK(objOtherTag > obj1);
  OFCHECK(objOtherTag >= obj1);

  // Check second attribute larger
  obj2.putSint16(-5);
  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);

  // Test different VM
  obj1.putSint16(-10);     // VM = 2
  obj2.putSint16(-10, 0);
  obj2.putSint16(-10, 1);
  obj2.putSint16(-10, 2); // VM = 3

  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);
}


static void checkUnsignedLong()
{
  // Start with equal values
  DcmUnsignedLong obj1(DCM_SelectorULValue);
  DcmUnsignedLong obj2(DCM_SelectorULValue);
  DcmUnsignedLong objOtherTag(DCM_UndefinedTagKey);

  // Check equality
  obj1.putUint32(10);
  obj2.putUint32(10);
  OFCHECK_EQUAL(obj1.compare(obj2), 0);
  OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
  // Reverse test should yield same result
  OFCHECK_EQUAL(obj2.compare(obj1), 0);
  OFCHECK( (obj2 >= obj1) && (obj1 >= obj2) );

  // Check differing tags (DCM_UndefinedTagKey always smaller than any other key)
  objOtherTag.putUint32(10);
  OFCHECK(obj1.compare(objOtherTag) < 0);
  OFCHECK(obj1 < objOtherTag);
  OFCHECK(obj1 <= objOtherTag);
  // Reverse test should yield opposite result
  OFCHECK(objOtherTag.compare(obj1) > 0);
  OFCHECK(objOtherTag > obj1);
  OFCHECK(objOtherTag >= obj1);

  // Check second attribute larger
  obj2.putUint32(10000);
  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);

  // Test different VM
  obj1.putUint32(10);     // VM = 2
  obj2.putUint32(10, 0);
  obj2.putUint32(10, 1);
  obj2.putUint32(10, 2); // VM = 3

  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);
}


static void checkUnsignedShort()
{
  // Start with equal values
  DcmUnsignedShort obj1(DCM_SelectorUSValue);
  DcmUnsignedShort obj2(DCM_SelectorUSValue);
  DcmUnsignedShort objOtherTag(DCM_UndefinedTagKey);

  // Check equality
  obj1.putUint16(10);
  obj2.putUint16(10);
  OFCHECK_EQUAL(obj1.compare(obj2), 0);
  OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
  // Reverse test should yield same result
  OFCHECK_EQUAL(obj2.compare(obj1), 0);
  OFCHECK( (obj2 >= obj2) && (obj1 >= obj2) );

  // Check differing tags (DCM_UndefinedTagKey always smaller than any other key)
  objOtherTag.putUint16(10);
  OFCHECK(obj1.compare(objOtherTag) < 0);
  OFCHECK(obj1 < objOtherTag);
  OFCHECK(obj1 <= objOtherTag);
  // Reverse test should yield opposite result
  OFCHECK(objOtherTag.compare(obj1) > 0);
  OFCHECK(objOtherTag > obj1);
  OFCHECK(objOtherTag >= obj1);

  // Check second attribute larger
  obj2.putUint16(10000);
  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);

  // Test different VM
  obj1.putUint16(10);     // VM = 2
  obj2.putUint16(10, 0);
  obj2.putUint16(10, 1);
  obj2.putUint16(10, 2); // VM = 3

  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);
}



static void checkOtherLong()
{
  // Start with equal values
  DcmOtherLong obj1(DCM_LongTrianglePointIndexList);
  DcmOtherLong obj2(DCM_LongTrianglePointIndexList);
  DcmOtherLong objOtherTag(DCM_UndefinedTagKey);

  // Check equality
  obj1.putUint32(100000);
  obj2.putUint32(100000);
  OFCHECK_EQUAL(obj1.compare(obj2), 0);
  OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
  // Reverse test should yield same result
  OFCHECK_EQUAL(obj2.compare(obj1), 0);
  OFCHECK( (obj2 >= obj2) && (obj1 >= obj2) );

  // Check differing tags (DCM_UndefinedTagKey always smaller than any other key)
  objOtherTag.putUint32(100000);
  OFCHECK(obj1.compare(objOtherTag) < 0);
  OFCHECK(obj1 < objOtherTag);
  OFCHECK(obj1 <= objOtherTag);
  // Reverse test should yield opposite result
  OFCHECK(objOtherTag.compare(obj1) > 0);
  OFCHECK(objOtherTag > obj1);
  OFCHECK(objOtherTag >= obj1);

  // Check second attribute larger
  obj2.putUint32(200000);
  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);

  // Test different VM
  obj1.putUint32(100000);     // VM = 2
  obj2.putUint32(100000, 0);
  obj2.putUint32(100000, 1);
  obj2.putUint32(100000, 2); // VM = 3

  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);
}



static void checkDcmItemAndSequences()
{
  // Start with equal values
  DcmItem obj1;
  DcmItem obj2;

  obj1.putAndInsertUint16(DCM_Rows, 10);
  obj1.putAndInsertUint16(DCM_Columns, 10);
  obj2.putAndInsertUint16(DCM_Rows, 10);
  obj2.putAndInsertUint16(DCM_Columns, 10);

  // Check equality
  OFCHECK_EQUAL(obj1.compare(obj2), 0);
  OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
  // Reverse test should yield same result
  OFCHECK_EQUAL(obj2.compare(obj1), 0);
  OFCHECK( (obj2 >= obj2) && (obj1 >= obj1) );

  // Check values in second item larger. Make sure that the first value remains equal.
  obj2.putAndInsertUint16(DCM_Columns, 100);
  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);

  // Test different VM (i.e. item cardinality):
  // obj1 has VM = 2
  // Make obj2 VM = 3 by adding another tag
  obj2.insertEmptyElement(DCM_PixelData);

  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2 );
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1 );
  OFCHECK(obj2 >= obj1 );

  // Check sequences

  DcmSequenceOfItems seq1(DCM_AnatomicRegionModifierSequence);
  obj2.findAndDeleteElement(DCM_PixelData); // reset obj2
  seq1.insert(new DcmItem(obj1));
  seq1.insert(new DcmItem(obj2));
  DcmSequenceOfItems seq2(DCM_AnatomicRegionModifierSequence);
  seq2.insert(new DcmItem(obj1));
  seq2.insert(new DcmItem(obj2));

  // Check equality
  OFCHECK_EQUAL(seq1.compare(seq2), 0);
  OFCHECK( (seq1 <= seq2) && (seq2 <= seq1) );
  // Reverse test should yield same result
  OFCHECK_EQUAL(seq2.compare(seq1), 0);
  OFCHECK( (seq2 >= seq1) && (seq1 >= seq2) );

  // Test different VM (i.e. sequence cardinality):
  // seq1 has VM = 2
  // Make seq2 VM = 3 by adding another item
  seq2.insertAtCurrentPos(new DcmItem());;

  OFCHECK(seq1.compare(seq2) < 0);
  OFCHECK(seq1 < seq2);
  OFCHECK(seq1 <= seq2);
  // Reverse test should yield opposite result
  OFCHECK(seq2.compare(seq1) > 0);
  OFCHECK(seq2 > seq1);
  OFCHECK(seq2 >= seq1);
}


OFTEST(dcmdata_VRCompare)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    // Check the different String-based VRs (in the sense that the method
    // putAndInsertOFStringArray() can be used by the test method for initializing
    // the test values.
    // This applies to: AE, AS, CS, DA, DS, DT, FD, FL, IS, LO, LT, OD, OF, PN,
    // SH, ST, TM, UC, UI, UR, UT.
    // Sometimes the maximum length of the VR is exceed deliberately to perform
    // the test, which is even useful since on that level DCMTK allows invalid
    // values in order to handle incorrect datasets.
    checkStringBased<DcmApplicationEntity>(DCM_RetrieveAETitle,"AE1", "AE1_CT");
    checkStringBased<DcmAgeString>(DCM_PatientAge, "008Y", "008YY");
    checkStringBased<DcmCodeString>(DCM_BlendingMode, "SOME", "SOME_LONGER");
    checkStringBased<DcmDate>(DCM_StudyDate, "19771212", "1977121200");
    checkStringBased<DcmDecimalString>(DCM_ProcedureStepProgress, "0.0", "0.01");
    checkStringBased<DcmDateTime>(DCM_SOPAuthorizationDateTime, "19771212235900", "19771212235900.123456");
    checkStringBased<DcmFloatingPointDouble>(DCM_BeamDeliveryDurationLimit, "10", "10.5");
    checkStringBased<DcmFloatingPointSingle>(DCM_IsocenterToWedgeTrayDistance, "10", "10.5");
    checkStringBased<DcmIntegerString>(DCM_NumberOfWedges, "100", "1000");
    checkStringBased<DcmLongString>(DCM_SeriesDescription, "Description", "Description Long");
    checkStringBased<DcmLongText>(DCM_AdditionalPatientHistory, "Text", "Text Long");
    checkStringBased<DcmOtherDouble>(DCM_DoubleFloatPixelData, "10", "10.5");
    checkStringBased<DcmOtherFloat>(DCM_FloatPixelData, "10", "10.5");
    checkStringBased<DcmPersonName>(DCM_OtherPatientNames, "Bond^James", "Bond^James^T.");
    checkStringBased<DcmShortString>(DCM_WedgeID, "WEDGE1", "WEDGE1_LONG");
    checkStringBased<DcmShortText>(DCM_RTPlanDescription, "Text", "Text Long");
    checkStringBased<DcmTime>(DCM_StudyTime, "235959", "235959.123456");
    checkStringBased<DcmUnlimitedCharacters>(DCM_LongCodeValue, "Code_0815", "Code_0815_4711");
    checkStringBased<DcmUniqueIdentifier>(DCM_SOPInstanceUID, "1.2.3.4", "1.2.3.4.5");
    checkStringBased<DcmUniversalResourceIdentifierOrLocator>(DCM_RetrieveURL, "http://wwww.dcmtk.org", "http://www.dcmtk.org/dcmtk.php.en");
    checkStringBased<DcmUnlimitedText>(DCM_TextValue, "Some (very long) text...", "Some (very long) text... hmm, not really.");

    // Check the rest
    checkAttributeTags();
    checkOtherByteOtherWord();
    checkPolymorphOtherByteOtherWord();
    checkOtherLong();
    checkSignedLong();
    checkSignedShort();
    checkDcmItemAndSequences();
    checkUnsignedLong();
    checkUnsignedShort();
    // Special VR helper classes:
    checkDcmPixelDataNative();
    checkDcmPixelDataEncapsulatedOB();
    // UN is not represented by its own class but is handled by OB/OW VR

    // The following, recently introduced VRs are not tested yet: OV, SV, UV.
}
