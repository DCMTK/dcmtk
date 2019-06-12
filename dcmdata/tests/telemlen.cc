/*
 *
 *  Copyright (C) 2011-2019, OFFIS e.V.
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
 *  Purpose: test program for DcmElement::calcElementLength
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcostrmb.h"


static void testElementLengthForElem(DcmElement& elem, const char *type,
                                     offile_off_t len)
{
    const int maxSize = 100;
    Uint8 buf[maxSize];
    DcmOutputBufferStream stream(buf, maxSize);
    void *unused;
    offile_off_t length;
    OFCondition cond;

    elem.transferInit();
    cond = elem.write(stream, EXS_LittleEndianExplicit, EET_ExplicitLength, NULL);
    elem.transferEnd();

    stream.flushBuffer(unused, length);
    if (cond.bad())
        OFCHECK_FAIL("Failed to write element " << type << " to stream: " << cond.text());
    if (length != len)
    {
        OFCHECK_FAIL("Expected length (" << OFstatic_cast(size_t, len)
                       << "), but got (" << OFstatic_cast(size_t, length)
                       << ") while writing " << type);
    }

    length = elem.calcElementLength(EXS_LittleEndianExplicit, EET_ExplicitLength);
    if (length != len)
    {
        OFCHECK_FAIL("Expected length (" << OFstatic_cast(size_t, len)
                       << "), but got (" << OFstatic_cast(size_t, length)
                       << ") while handling " << type);
    }
}

static void testElementLengthForVR(DcmEVR vr, offile_off_t len)
{
    const DcmTag tag(DCM_PatientName, vr);

    // Test all the calcElementLength and write implementations
    DcmUnlimitedText ut(tag);
    DcmByteString bs(tag);
    DcmPixelData pd(tag);
    DcmPolymorphOBOW po(tag);
    DcmSequenceOfItems sq(tag);
    DcmOtherByteOtherWord oo(tag);

    // Make sure the PixelData assumes it is un-encapsulated, else
    // calcElementLength() will fail (Yes, this is a hack)
    pd.putUint8Array(NULL, 0);
    pd.setVR(vr);

    testElementLengthForElem(ut, "UnlimitedText", len);
    testElementLengthForElem(bs, "ByteString", len);
    testElementLengthForElem(pd, "PixelData", len);
    testElementLengthForElem(po, "PolymorphOBOW", len);
    testElementLengthForElem(sq, "SequenceOfItems", len);
    testElementLengthForElem(oo, "OtherByteOtherWord", len);
}

// These don't make sense to be tested: EVR_item, EVR_metainfo,
// EVR_dataset, EVR_fileFormat, EVR_dicomDir, EVR_dirRecord, EVR_pixelSQ

OFTEST(dcmdata_elementLength_pixelItem)
{
    // EVR_pixelItem only makes sense for class DcmPixelItem.
    // In fact, it enforces that VR in the constructor.
    const DcmTag tag(DCM_PatientName, EVR_pixelItem);
    DcmPixelItem pi(tag);
    testElementLengthForElem(pi, "PixelItem", 8);
}

OFTEST(dcmdata_elementLength_pixelSequence)
{
    // DcmPixelSequence forces EVR_OB, no point in trying other VRs.
    const DcmTag tag(DCM_PatientName, EVR_OB);
    DcmPixelSequence ps(tag);
    testElementLengthForElem(ps, "PixelSeq", 20);
}

/* Test extended length VRs */
#define TEST_VR(vr) OFTEST(dcmdata_elementLength_ ## vr) { testElementLengthForVR(vr, 12); }
TEST_VR(EVR_OB)
TEST_VR(EVR_OD)
TEST_VR(EVR_OF)
TEST_VR(EVR_OL)
TEST_VR(EVR_OV)
TEST_VR(EVR_OW)
TEST_VR(EVR_SV)
TEST_VR(EVR_UV)
TEST_VR(EVR_SQ)
TEST_VR(EVR_UC)
TEST_VR(EVR_UR)
TEST_VR(EVR_UT)
TEST_VR(EVR_ox)
TEST_VR(EVR_lt)
TEST_VR(EVR_UNKNOWN)
TEST_VR(EVR_UN)
/* Converted to UN during write */
TEST_VR(EVR_na)
TEST_VR(EVR_UNKNOWN2B)
#undef TEST_VR

/* Test "short" length VRs */
#define TEST_VR(vr) OFTEST(dcmdata_elementLength_ ## vr) { testElementLengthForVR(vr, 8); }
TEST_VR(EVR_AE)
TEST_VR(EVR_AS)
TEST_VR(EVR_AT)
TEST_VR(EVR_CS)
TEST_VR(EVR_DA)
TEST_VR(EVR_DS)
TEST_VR(EVR_DT)
TEST_VR(EVR_FL)
TEST_VR(EVR_FD)
TEST_VR(EVR_IS)
TEST_VR(EVR_LO)
TEST_VR(EVR_PN)
TEST_VR(EVR_SH)
TEST_VR(EVR_SL)
TEST_VR(EVR_SS)
TEST_VR(EVR_ST)
TEST_VR(EVR_TM)
TEST_VR(EVR_UI)
TEST_VR(EVR_UL)
TEST_VR(EVR_US)
TEST_VR(EVR_xs)
TEST_VR(EVR_LT)
TEST_VR(EVR_up)
TEST_VR(EVR_PixelData)
TEST_VR(EVR_OverlayData)
#undef TEST_VR
