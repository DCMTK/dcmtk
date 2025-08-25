/*
 *
 *  Copyright (C) 2022-2025, OFFIS e.V.
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
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Test for creating and reading Segmentation with many frames/segments
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmseg/segtypes.h" /* for DCMSEG_DEBUG */

// DCMTK's original OFMap implementation is too slow for this test...
#ifdef HAVE_STL_MAP
#include "dcmtk/dcmseg/segdoc.h"
#include "dcmtk/dcmseg/segment.h"

#include "dcmtk/dcmfg/fgfracon.h"
#include "dcmtk/dcmfg/fgpixmsr.h"
#include "dcmtk/dcmfg/fgplanor.h"
#include "dcmtk/dcmfg/fgplanpo.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/ofstd/ofstd.h"

static const Uint8 NUM_ROWS             = 5;
static const Uint8 NUM_COLS             = 5;
static const Uint8 NUM_THREADS          = 16; // Use 16 threads for writing and reading

// Restrict to 1.000.000 Frames since the theoretical 2^31-1 number of frames
// results in too much memory usage and waiting time

static const Uint32 NUM_FRAMES           = 1000000;
static const size_t NUM_SEGS             = DCM_SEG_MAX_SEGMENTS;

static const Uint8 NUM_PIXELS_PER_FRAME = NUM_COLS * NUM_ROWS;

static DcmSegmentation* create();
static void setGenericValues(DcmSegmentation* seg);
static void addSharedFGs(DcmSegmentation* seg);
static void addSegments(DcmSegmentation* seg);
static void addFrames(DcmSegmentation* seg);
static void addDimensions(DcmSegmentation* seg);
static void checkCreatedObject(DcmDataset& seg);

OFTEST_FLAGS(dcmseg_bigdim, EF_Slow)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    // Creation
    DcmSegmentation* seg = create();
    setGenericValues(seg);
    addSharedFGs(seg);
    addSegments(seg);
    addFrames(seg);
    addDimensions(seg);

    // Write to dataset and compare its dump with expected result
    DcmFileFormat dcmff;
    DcmDataset* ds = dcmff.getDataset();
    seg->setCheckDimensionsOnWrite(OFFalse);
    seg->setCheckFGOnWrite(OFFalse);
    seg->getFunctionalGroups().setUseThreads(NUM_THREADS);
    OFCondition result = seg->writeDataset(*ds);
    OFCHECK(result.good());

    // Save to disk, and re-load to test import
    OFTempFile tf;
    OFString temp_fn = tf.getFilename();
    OFCHECK(!temp_fn.empty());
    OFCHECK(dcmff.saveFile(temp_fn.c_str(), EXS_LittleEndianExplicit).good());
    tf.stealFile();

    // Read object from dataset into DcmSegmentation object, write again to dataset and
    // check whether object after writing is identical to object after writing.
    // the same expected result
    delete seg;
    seg = NULL;
    DcmSegmentation::LoadingFlags flags;
    flags.m_numThreads = NUM_THREADS; // Use 16 threads for reading
    DcmSegmentation::loadFile(temp_fn, seg, flags).good();
    OFCHECK(seg != OFnullptr);
    if (seg)
    {
        DcmDataset dset;
        seg->setCheckDimensionsOnWrite(OFFalse);
        seg->setCheckFGOnWrite(OFFalse);
        seg->getFunctionalGroups().setUseThreads(NUM_THREADS);
        OFCHECK(seg->writeDataset(dset).good());
        checkCreatedObject(dset);
        delete seg;
    }
}

static DcmSegmentation* create()
{
    IODGeneralEquipmentModule::EquipmentInfo eq("Open Connections", "OC CT", "4711", "0.1");
    ContentIdentificationMacro ci("1", "LABEL", "DESCRIPTION", "Doe^John");
    DcmSegmentation* seg = NULL;
    OFCondition result;
    DcmSegmentation::createFractionalSegmentation(seg, NUM_ROWS, NUM_COLS, DcmSegTypes::SFT_OCCUPANCY, 255, eq, ci);
    OFCHECK(result.good());
    OFCHECK(seg != OFnullptr);
    seg->setCheckFGOnWrite(OFFalse);
    seg->setCheckDimensionsOnWrite(OFFalse);
    return seg;
}

static void setGenericValues(DcmSegmentation* seg)
{
    if (!seg)
        return;
    OFCHECK(seg->getPatient().setPatientName("Bond^James").good());
    OFCHECK(seg->getPatient().setPatientID("007").good());
    OFCHECK(seg->getPatient().setPatientBirthDate("19771007").good());
    OFCHECK(seg->getStudy().setStudyDate("20190801").good());
    OFCHECK(seg->getStudy().setStudyTime("120000").good());
    OFCHECK(seg->getStudy().setStudyID("1").good());
    OFCHECK(seg->getPatientStudy().setPatientAge("040Y").good());
    OFCHECK(seg->getSeries().setSeriesDescription("Test Description").good());
    OFCHECK(seg->getSeries().setSeriesNumber("1").good());
    OFCHECK(seg->getSeries().setPatientPosition("HFS").good());

    // Those values are usually computed automatically. UIDS are generated and date/times are set to current values.
    // But in order to compare the "old" dump with the freshly created image attributes, we set some values manually,
    // so that they are not overwritten with new, automatically created values later.
    OFCHECK(seg->getStudy().setStudyInstanceUID("1.2.276.0.7230010.3.1.2.8323329.14863.1565940357.864811").good());
    OFCHECK(seg->getFrameOfReference().setFrameOfReferenceUID("2.25.30853397773651184949181049330553108086").good());
    OFCHECK(seg->getSeries().setSeriesInstanceUID("1.2.276.0.7230010.3.1.3.8323329.14863.1565940357.864812").good());
    OFCHECK(seg->getSOPCommon().setSOPInstanceUID("1.2.276.0.7230010.3.1.4.8323329.14863.1565940357.864813").good());
    OFCHECK(seg->getGeneralImage().setContentDate("20190927").good());
    OFCHECK(seg->getGeneralImage().setContentTime("153857").good());
}

static void addSharedFGs(DcmSegmentation* seg)
{
    if (!seg)
        return;

    FGPixelMeasures meas;
    OFCHECK(meas.setPixelSpacing("0.1\\0.1").good());
    OFCHECK(meas.setSliceThickness("1.0").good());
    OFCHECK(meas.setSpacingBetweenSlices("0.05").good());

    FGPlanePosPatient planpo;
    OFCHECK(planpo.setImagePositionPatient("0.0", "0.0", "0.0").good());

    FGPlaneOrientationPatient planor;
    OFCHECK(planor.setImageOrientationPatient("1.0", "0.0", "0.0", "0.0", "1.0", "0.0").good());

    OFCHECK(seg->addForAllFrames(meas).good());
    OFCHECK(seg->addForAllFrames(planpo).good());
    OFCHECK(seg->addForAllFrames(planor).good());
}



static void addSegments(DcmSegmentation* seg)
{
    for (Uint16 s = 0; s < NUM_SEGS; s++)
    {
        DcmSegment* segment = NULL;
        CodeSequenceMacro category("85756007", "SCT", "Tissue");
        CodeSequenceMacro propType("51114001", "SCT", "Artery");
        char buf[100];
        OFStandard::snprintf(buf, sizeof(buf), "SEGLABEL_%hu", s);
        OFCHECK(DcmSegment::create(segment, buf, category, propType, DcmSegTypes::SAT_AUTOMATIC, "OC_DUMMY")
                    .good());
        OFCHECK(segment != OFnullptr);
        Uint16 dontCare = 0;
        OFCHECK(seg->addSegment(segment, dontCare).good());
    }
}


static void addFrames(DcmSegmentation* seg)
{
    if (!seg)
        return;

    FGFrameContent* fg     = new FGFrameContent();
    OFCHECK(fg);
    fg->setStackID("1");
    if (fg)
    {
        for (Uint32 frameNo = 1; frameNo <= NUM_FRAMES; frameNo++)
        {
            // Frame Acqusition Number is set to the current frame number. Since attribute is 16 bits
            // only (but optional), this is only done as long as frame number is within 16 bit range.
            if (frameNo <= DCM_SEG_MAX_SEGMENTS)
                OFCHECK(fg->setFrameAcquisitionNumber(OFstatic_cast(Uint16, frameNo)).good());
            OFCHECK(fg->setFrameReferenceDateTime("20190816092557").good());
            OFCHECK(fg->setFrameAcquisitionDateTime("20190816092557").good());
            OFCHECK(fg->setFrameAcquisitionDuration(0.001).good());
            OFCHECK(fg->setInStackPositionNumber(frameNo).good());
            OFCHECK(fg->setDimensionIndexValues(1, 0).good());
            OFCHECK(fg->setDimensionIndexValues(frameNo, 1).good());
            OFVector<FGBase*> groups;
            groups.push_back(fg);

            Uint8* data = new Uint8[NUM_PIXELS_PER_FRAME];
            for (Uint8 i = 0; i < NUM_PIXELS_PER_FRAME; ++i)
            {
                data[i] = i;
            }
            Uint16 segmentNumber = OFstatic_cast(Uint16, ((frameNo-1) % (NUM_SEGS)) +1); // segment numbers start at 1
            OFVector<FGBase*> perFrameFGs;
            perFrameFGs.push_back(fg);
            OFCHECK(seg->addFrame(data, segmentNumber, perFrameFGs).good());
            delete[] data;
        }
    }
    delete fg;
}

static void addDimensions(DcmSegmentation* seg)
{
    if (!seg)
        return;
    IODMultiframeDimensionModule& dims = seg->getDimensions();
    OFCHECK(dims.addDimensionIndex(
                    DCM_StackID, "2.25.30855560781715986879861690673941231222", DCM_FrameContentSequence, "STACK_DIM")
                .good());
    OFCHECK(dims.addDimensionIndex(DCM_InStackPositionNumber,
                                   "2.25.30855560781715986879861690673941231222",
                                   DCM_FrameContentSequence,
                                   "STACK_DIM")
                .good());
    OFunique_ptr<IODMultiframeDimensionModule::DimensionOrganizationItem> org(
        new IODMultiframeDimensionModule::DimensionOrganizationItem);
    if (org)
    {
        org->setDimensionOrganizationUID("2.25.30855560781715986879861690673941231222");
        dims.getDimensionOrganizationSequence().push_back(org.release());
    }
}


static void checkCreatedObject(DcmDataset& dset)
{
    OFString str;
    Sint32 sint32;

    dset.findAndGetSint32(DCM_NumberOfFrames, sint32);
    OFCHECK(NUM_FRAMES == sint32);

    DcmElement *elem = NULL;
    OFCHECK(dset.findAndGetElement(DCM_PixelData, elem).good());
    if (elem != OFnullptr)
    {
        unsigned long count = 0;
        const Uint8* pixData = NULL;
        OFCHECK(dset.findAndGetUint8Array(DCM_PixelData, pixData, &count).good());
        OFCHECK(count == NUM_FRAMES * NUM_PIXELS_PER_FRAME);
        for (size_t p = 0; p < count; p++)
        {
            OFCHECK(pixData[p] == p % (NUM_PIXELS_PER_FRAME));
        }
    }
    DcmSequenceOfItems *seq = NULL;
    OFCHECK(dset.findAndGetSequence(DCM_PerFrameFunctionalGroupsSequence, seq).good());
    if (seq != NULL)
    {
        size_t numFrames = seq->card();
        OFOStringStream oss;
        oss << "Expected " << NUM_FRAMES << " frames, but got " << numFrames;
        OFCHECK_MSG(numFrames == NUM_FRAMES, oss.str().c_str());
        DcmItem* item = seq->getItem(0);
        for (size_t n = 0; (n < numFrames) && (item != NULL); n++)
        {
            DcmItem* fgItem = NULL;
            OFCHECK(item->findAndGetSequenceItem(DCM_SegmentIdentificationSequence, fgItem, 0).good());
            if (item != NULL)
            {
                Uint16 segNum = 0;
                OFCHECK(fgItem->findAndGetUint16(DCM_ReferencedSegmentNumber, segNum).good());
                OFCHECK(segNum == ((n % NUM_SEGS) + 1));

            }
            item = OFstatic_cast(DcmItem*, seq->nextInContainer(item));
        }
    }
}
#else

// Dummy version of the test case. Needed to prevent ctest test failure.
OFTEST(dcmseg_bigdim)
{
    DCMSEG_DEBUG("Will not run dcmseg_bigdim test: std::map support (HAVE_STL_MAP) must be defined to complete the test in acceptable time");
}

#endif // HAVE_STL_MAP
