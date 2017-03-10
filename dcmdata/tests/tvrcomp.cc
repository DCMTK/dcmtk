/*
 *
 *  Copyright (C) 2015-2017, OFFIS e.V.
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
 *  Purpose: tests for compare() as well as < and > operators of VR classes
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcvras.h"
#include "dcmtk/dcmdata/dcvrae.h"
#include "dcmtk/dcmdata/dcchrstr.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrlt.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrst.h"
#include "dcmtk/dcmdata/dcvrut.h"
#include "dcmtk/dcmdata/dcvras.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrfd.h"
#include "dcmtk/dcmdata/dcvrfl.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcvrsl.h"
#include "dcmtk/dcmdata/dcvrss.h"
#include "dcmtk/dcmdata/dcvrul.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcvrpobw.h"
#include "dcmtk/dcmdata/dcovlay.h"
#include "dcmtk/dcmdata/dcpixel.h"


template <typename ByteStringType>
static void checkByteString(const DcmTagKey& key)
{
  // Start with equal values
  OFString val1 = "TEST";
  OFString val2 = "TEST";
  ByteStringType obj1(key);
  ByteStringType obj2(key);
  ByteStringType objOtherTag(DCM_UndefinedTagKey);

  // Check equality
  obj1.putOFStringArray("TEST");
  obj2.putOFStringArray("TEST");
  OFCHECK_EQUAL(obj1.compare(obj2), 0);
  OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
  // Reverse test should yield same result
  OFCHECK_EQUAL(obj2.compare(obj1), 0);
  OFCHECK( (obj2 >= obj1) && (obj1 >= obj2) );

  // Check differing tags (DCM_UndefinedTagKey always smaller than any other key)
  objOtherTag.putOFStringArray("TEST" /* same value*/);
  OFCHECK(obj1.compare(objOtherTag) < 0);
  OFCHECK(obj1 < objOtherTag);
  OFCHECK(obj1 <= objOtherTag);
  // Reverse test should yield opposite result
  OFCHECK(objOtherTag.compare(obj1) > 0);
  OFCHECK(objOtherTag > obj1);
  OFCHECK(objOtherTag >= obj1);

  // Check second string longer
  obj2.putOFStringArray("TEST_LONG");
  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);

  // Test different VM
  obj1.putOFStringArray("TEST\\TEST");
  obj2.putOFStringArray("TEST\\TEST\\TEST");
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

  // Check equality
  obj1.putTagVal(DCM_PatientName);
  obj2.putTagVal(DCM_PatientName);
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


static void checkFloatingPointDouble()
{
  // Start with equal values
  DcmFloatingPointDouble obj1(DCM_RealWorldValueLUTData);
  DcmFloatingPointDouble obj2(DCM_RealWorldValueLUTData);
  DcmFloatingPointDouble objOtherTag(DCM_UndefinedTagKey);

  // Check equality
  obj1.putFloat64(10.0);
  obj2.putFloat64(10.0);
  OFCHECK_EQUAL(obj1.compare(obj2), 0);
  OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
  // Reverse test should yield same result
  OFCHECK_EQUAL(obj2.compare(obj1), 0);
  OFCHECK( (obj2 >= obj2) && (obj1 >= obj1) );

  // Check differing tags (DCM_UndefinedTagKey always smaller than any other key)
  objOtherTag.putFloat64(10.0);
  OFCHECK(obj1.compare(objOtherTag) < 0);
  OFCHECK(obj1 < objOtherTag);
  OFCHECK(obj1 <= objOtherTag);
  // Reverse test should yield opposite result
  OFCHECK(objOtherTag.compare(obj1) > 0);
  OFCHECK(objOtherTag > obj1);
  OFCHECK(objOtherTag >= obj1);

  // Check second attribute larger
  obj2.putFloat64(100.50);
  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);

  // Test different VM
  obj1.putFloat64(10.0);     // VM = 2
  obj2.putFloat64(10.0, 0);
  obj2.putFloat64(10.0, 1);
  obj2.putFloat64(10.0, 2); // VM = 3

  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);
}


static void checkFloatingPointSingle()
{
  // Start with equal values
  DcmFloatingPointSingle obj1(DCM_CornealPointLocation);
  DcmFloatingPointSingle obj2(DCM_CornealPointLocation);
  DcmFloatingPointSingle objOtherTag(DCM_UndefinedTagKey);

  // Check equality
  obj1.putFloat32(10.0);
  obj2.putFloat32(10.0);
  OFCHECK_EQUAL(obj1.compare(obj2), 0);
  OFCHECK( (obj1 <= obj2) && (obj2 <= obj1) );
  // Reverse test should yield same result
  OFCHECK_EQUAL(obj2.compare(obj1), 0);
  OFCHECK( (obj2 >= obj1) && (obj1 >= obj2) );

  // Check differing tags (DCM_UndefinedTagKey always smaller than any other key)
  objOtherTag.putFloat32(10.0);
  OFCHECK(obj1.compare(objOtherTag) < 0);
  OFCHECK(obj1 < objOtherTag);
  OFCHECK(obj1 <= objOtherTag);
  // Reverse test should yield opposite result
  OFCHECK(objOtherTag.compare(obj1) > 0);
  OFCHECK(objOtherTag > obj1);
  OFCHECK(objOtherTag >= obj1);

  // Check second attribute larger
  obj2.putFloat32(100.50);
  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);

  // Test different VM
  obj1.putFloat32(10.0);     // VM = 2
  obj2.putFloat32(10.0, 0);
  obj2.putFloat32(10.0, 1);
  obj2.putFloat32(10.0, 2); // VM = 3

  OFCHECK(obj1.compare(obj2) < 0);
  OFCHECK(obj1 < obj2);
  OFCHECK(obj1 <= obj2);
  // Reverse test should yield opposite result
  OFCHECK(obj2.compare(obj1) > 0);
  OFCHECK(obj2 > obj1);
  OFCHECK(obj2 >= obj1);
}

template <typename BinaryOBOWType>
static void checkOtherByteOtherWord(const DcmTagKey& tagKey)
{
  // Start with equal values
  BinaryOBOWType obj1(tagKey);
  BinaryOBOWType obj2(tagKey);
  BinaryOBOWType objOtherTag(DCM_UndefinedTagKey);

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
  // Check the different VRs implemented by DcmByteString.
  // We gracefully ignore the maximum VM here since there is not
  // even an attribute (e.g.) at the moment which is of VR
  // AS and has a VM >1.
  checkByteString<DcmAgeString>(DCM_PatientAge);
  checkByteString<DcmApplicationEntity>(DCM_RetrieveAETitle);
  checkByteString<DcmLongString>(DCM_SeriesDescription);
  checkByteString<DcmLongText>(DCM_AdditionalPatientHistory);
  checkByteString<DcmPersonName>(DCM_OtherPatientNames);
  checkByteString<DcmShortText>(DCM_RTPlanDescription);
  checkByteString<DcmUnlimitedText>(DCM_RetrieveURL);
  checkAttributeTags();
  checkFloatingPointDouble();
  checkFloatingPointSingle();
  checkOtherByteOtherWord<DcmPixelItem>(DCM_PixelData);
  checkOtherByteOtherWord<DcmPolymorphOBOW>(DCM_PixelData);
  checkOtherByteOtherWord<DcmOverlayData>(DCM_OverlayData);
  checkOtherByteOtherWord<DcmPixelData>(DCM_PixelData);
  checkSignedLong();
  checkSignedShort();
  checkUnsignedLong();
  checkUnsignedShort();
  checkDcmItemAndSequences();
}
