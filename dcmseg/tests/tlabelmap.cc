/*
 *
 *  Copyright (C) 2024-2025, Open Connections GmbH
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
 *  Module:  dcmseg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Test for creating, writing and reading labelmap segmentations
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmiod/modpalettecolorlut.h"
#include "dcmtk/dcmseg/segdoc.h"
#include "dcmtk/dcmseg/segment.h"
#include "dcmtk/dcmseg/segtypes.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmfg/fgfracon.h"
#include "dcmtk/dcmfg/fgpixmsr.h"
#include "dcmtk/dcmfg/fgplanor.h"
#include "dcmtk/dcmfg/fgplanpo.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftypes.h"

// Dimensions are 510 x 257 = 65535 pixels per frame.
// This allows to count through 16 bit values from 0 to 65534 for each frame. We are not using value
// up to 65535 since that would result in 65536 values; since each value represents a segment requiring
// an entry in the Segment Sequence, we would exceed the maximum number of segments (65535) allowed in
// a segmentation object (Segment Number is of VR US).
// For 8 bit, the pixel are counted through repeatedly from 0 to 255. This does not match exactly on the
// number of pixels (510*257) per frame but should be sufficient for testing purposes. 255 is chosen
// because this will also exercise the maximum number of palette entries (256) in the palette color LUT.
static const Uint16 NUM_ROWS             = 510;
static const Uint16 NUM_COLS             = 257;
// Two frames to exercise multi-frame
static const Uint16 NUM_FRAMES            = 2;
static const size_t NUM_PIXELS_PER_FRAME = NUM_COLS * NUM_ROWS;


// set to true to keep temporary files for debugging
static const OFBool keepTempFiles = OFFalse;

// For 8 bit, pixel values 0 to 255 are permitted.
// For 16 bit, pixel values 0 to 65534 are permitted, since for labelmaps the pixel
// value represents the segment number as well, and DICOM permits a maximum segment number
// of 65535.

// Since the number of segments in the 16 bit case is very large and generation takes
// some time, the 16 bits are marked as slow.

static DcmSegmentation* createLabelMap(const Uint8 bits_allocated, const DcmSegTypes::E_SegmentationLabelmapColorModel cm);
static void setGenericValues(DcmSegmentation* seg);
static void addSharedFGs(DcmSegmentation* seg);
static void addPaletteAndICCProfile(DcmSegmentation* seg, const Uint8 bitsAllocated);
template<typename T>
static void addFrames(DcmSegmentation* seg, const Uint8 bitsAllocated);
static void addSegments(DcmSegmentation *seg, const Uint8 bitsAllocated);
static void addDimensions(DcmSegmentation* seg);
static void checkCreatedObject(DcmDataset* dset, const Uint8 bits_allocated, const DcmSegTypes::E_SegmentationLabelmapColorModel cm);
static void checkPalette(DcmDataset* dset, const Uint8 bits_allocated);


// Concatenations tests: TBD
// static void writeAndCheckConcatenation(DcmSegmentation* seg, OFList<OFFilename>& concats);
// static void loadAndCheckConcatenation(const OFList<OFFilename>& concats);
// static void checkConcatenationInstance(size_t numInstance, DcmSegmentation* srcInstance, DcmDataset* concatInstance);

OFTEST(dcmseg_labelmap_8bit_mono2)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    // Creation
    DcmSegmentation* seg = createLabelMap(8, DcmSegTypes::SLCM_MONOCHROME2);
    setGenericValues(seg);
    addSharedFGs(seg);
    addFrames<Uint8>(seg, 8);
    addSegments(seg, 8);
    addDimensions(seg);

    // Write to dataset and compare its dump with expected result
    DcmFileFormat dcmff;
    DcmDataset* created_ds = dcmff.getDataset();
    OFCHECK(seg->writeDataset(*created_ds).good());
    checkCreatedObject(created_ds, 8, DcmSegTypes::SLCM_MONOCHROME2);

    // Save to disk, and re-load to test import
    OFTempFile tf;
    OFString temp_fn = tf.getFilename();
    OFCHECK(!temp_fn.empty());
    OFCHECK(seg->saveFile(temp_fn.c_str(), EXS_LittleEndianExplicit).good());

    // Read object from dataset into DcmSegmentation object, write again to dataset and
    // check whether object after writing is identical to object after writing.
    // the same expected result
    delete seg;
    seg = NULL;
    DcmDataset loaded_ds;
    DcmSegmentation::loadFile(temp_fn, seg).good();
    OFCHECK(seg != OFnullptr);
    if (seg)
    {
        OFCHECK(seg->writeDataset(loaded_ds).good());
        checkCreatedObject(&loaded_ds, 8, DcmSegTypes::SLCM_MONOCHROME2);
    }
    if (keepTempFiles)
    {
        std::cout << "Keeping temporary file: " << temp_fn << " from test dcmseg_labelmap_8bit_mono2" << std::endl;
        tf.stealFile();
    }
    delete seg;
}


OFTEST_FLAGS(dcmseg_labelmap_16bit_mono2, EF_Slow)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    // Creation
    DcmSegmentation* seg_object = createLabelMap(16, DcmSegTypes::SLCM_MONOCHROME2);
    setGenericValues(seg_object);
    addSharedFGs(seg_object);
    addFrames<Uint16>(seg_object, 16);
    addSegments(seg_object, 16);
    addDimensions(seg_object);

    // Write to dataset and compare its dump with expected result
    DcmFileFormat dcmff;
    DcmDataset* dset_created = dcmff.getDataset();
    OFCHECK(seg_object->writeDataset(*dset_created).good());
    checkCreatedObject(dset_created, 16, DcmSegTypes::SLCM_MONOCHROME2);

    // Save to disk, and re-load to test import
    OFTempFile tf;
    OFString temp_fn = tf.getFilename();
    OFCHECK(!temp_fn.empty());
    OFCHECK(seg_object->saveFile(temp_fn.c_str(), EXS_LittleEndianExplicit).good());

    // Read object from dataset into DcmSegmentation object, write again to dataset and
    // check whether object after writing is identical to object after writing.
    // the same expected result
    delete seg_object;
    seg_object = NULL;
    DcmSegmentation::loadFile(temp_fn, seg_object).good();
    OFCHECK(seg_object != OFnullptr);
    DcmDataset dset_loaded;
    if (seg_object)
    {
        OFCHECK(seg_object->writeDataset(dset_loaded).good());
        checkCreatedObject(&dset_loaded, 16, DcmSegTypes::SLCM_MONOCHROME2);
    }

    delete seg_object;
    if (keepTempFiles)
    {
        std::cout << "Keeping temporary file: " << temp_fn << " from test dcmseg_labelmap_16bit_mono2" << std::endl;
        tf.stealFile();
    }
}

OFTEST(dcmseg_labelmap_8bit_palette)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    // Creation
    DcmSegmentation* seg = createLabelMap(8, DcmSegTypes::SLCM_PALETTE);
    setGenericValues(seg);
    addSharedFGs(seg);
    addFrames<Uint8>(seg, 8);
    addSegments(seg, 8);
    addDimensions(seg);
    addPaletteAndICCProfile(seg, 8);

    // Write to dataset and compare its dump with expected result
    DcmFileFormat dcmff;
    DcmDataset* created_ds = dcmff.getDataset();
    OFCHECK(seg->writeDataset(*created_ds).good());
    checkCreatedObject(created_ds, 8, DcmSegTypes::SLCM_PALETTE);

    // Save to disk, and re-load to test import
    OFTempFile tf;
    OFString temp_fn = tf.getFilename();
    OFCHECK(!temp_fn.empty());
    OFCHECK(seg->saveFile(temp_fn.c_str(), EXS_LittleEndianExplicit).good());

    // Read object from dataset into DcmSegmentation object, write again to dataset and
    // check whether object after writing is identical to object after writing.
    // the same expected result
    delete seg;
    seg = NULL;
    DcmDataset loaded_ds;
    DcmSegmentation::loadFile(temp_fn, seg).good();
    OFCHECK(seg != OFnullptr);
    if (seg)
    {
        seg->writeDataset(loaded_ds).good();
        checkCreatedObject(&loaded_ds, 8, DcmSegTypes::SLCM_PALETTE);
    }

    delete seg;
    if (keepTempFiles)
    {
        std::cout << "Keeping temporary file: " << temp_fn << " from test dcmseg_labelmap_8bit_palette" << std::endl;
        tf.stealFile();
    }
}


OFTEST_FLAGS(dcmseg_labelmap_16bit_palette, EF_Slow)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    // Creation
    DcmSegmentation* seg_object = createLabelMap(16, DcmSegTypes::SLCM_PALETTE);
    setGenericValues(seg_object);
    addSharedFGs(seg_object);
    addFrames<Uint16>(seg_object, 16);
    addSegments(seg_object, 16);
    addDimensions(seg_object);
    addPaletteAndICCProfile(seg_object, 16);

    // Write to dataset and compare its dump with expected result
    DcmFileFormat dcmff;
    DcmDataset* dset_created = dcmff.getDataset();
    OFCHECK(seg_object->writeDataset(*dset_created).good());
    checkCreatedObject(dset_created, 16, DcmSegTypes::SLCM_PALETTE);

    // Save to disk, and re-load to test import
    OFTempFile tf;
    OFString temp_fn = tf.getFilename();
    OFCHECK(!temp_fn.empty());
    OFCHECK(seg_object->saveFile(temp_fn.c_str(), EXS_LittleEndianExplicit).good());

    // Read object from dataset into DcmSegmentation object, write again to dataset and
    // check whether object after writing is identical to object after writing.
    // the same expected result
    delete seg_object;
    seg_object = NULL;
    DcmSegmentation::loadFile(temp_fn, seg_object).good();
    OFCHECK(seg_object != OFnullptr);
    DcmDataset dset_loaded;
    if (seg_object)
    {
        seg_object->writeDataset(dset_loaded).good();
        checkCreatedObject(&dset_loaded, 16, DcmSegTypes::SLCM_PALETTE);
    }

    delete seg_object;
    if (keepTempFiles)
    {
        std::cout << "Keeping temporary file: " << temp_fn << " from test dcmseg_labelmap_16bit_palette" << std::endl;
        tf.stealFile();
    }
}


static DcmSegmentation* createLabelMap(const Uint8 bits_allocated, const DcmSegTypes::E_SegmentationLabelmapColorModel cm)
{
    IODGeneralEquipmentModule::EquipmentInfo eq("Open Connections", "OC CT", "4711", "0.1");
    ContentIdentificationMacro ci("1", "LABEL", "DESCRIPTION", "Doe^John");
    DcmSegmentation* seg = NULL;
    OFCondition result;
    DcmSegmentation::createLabelmapSegmentation(seg, NUM_ROWS, NUM_COLS, eq, ci, bits_allocated == 16, cm);
    OFCHECK(result.good());
    OFCHECK(seg != OFnullptr);
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

template<typename T>
static void addFrames(DcmSegmentation* seg, const Uint8 bitsAllocated)
{
    if (!seg)
        return;

    FGFrameContent* fg     = new FGFrameContent();
    OFCHECK(fg);
    fg->setStackID("1");
    if (fg)
    {
        for (Uint16 frameNo = 1; frameNo <= NUM_FRAMES; frameNo++)
        {
            OFCHECK(fg->setFrameAcquisitionNumber(frameNo).good());
            OFCHECK(fg->setFrameReferenceDateTime("20190816092557").good());
            OFCHECK(fg->setFrameAcquisitionDateTime("20190816092557").good());
            OFCHECK(fg->setFrameAcquisitionDuration(0.001).good());
            OFCHECK(fg->setInStackPositionNumber(frameNo).good());
            OFCHECK(fg->setDimensionIndexValues(1, 0).good());
            OFCHECK(fg->setDimensionIndexValues(frameNo, 1).good());
            OFVector<FGBase*> groups;
            groups.push_back(fg);
            T* data = new T[NUM_PIXELS_PER_FRAME];
            Uint32 max_value = (bitsAllocated == 16 ? 65535 : 256);
            for (Uint32 i = 0; i < NUM_PIXELS_PER_FRAME; ++i)
            {
                data[i] = OFstatic_cast(T, (NUM_PIXELS_PER_FRAME * (frameNo - 1) + i) % max_value);
            }
            OFVector<FGBase*> perFrameFGs;
            perFrameFGs.push_back(fg);
            OFCHECK(seg->addFrame<T>(data, frameNo, perFrameFGs).good());
            delete[] data;
        }
    }
    delete fg;
}

static void addSegments(DcmSegmentation *seg, const Uint8 bitsAllocated)
{
    for (Uint32 i = 0; i < ((bitsAllocated == 16) ? 65535u : 255u); i++)
    {
        DcmSegment* segment = NULL;
        CodeSequenceMacro category("85756007", "SCT", "Tissue");
        CodeSequenceMacro propType("51114001", "SCT", "Artery");

        OFCHECK(DcmSegment::create(segment, "SEGLABEL", category, propType, DcmSegTypes::SAT_AUTOMATIC, "OC_DUMMY")
                    .good());
        OFCHECK(segment != OFnullptr);
        Uint16 segno = OFstatic_cast(Uint16, i);
        OFCHECK(seg->addSegment(segment, segno).good());
    }
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


static void checkCreatedObject(DcmDataset* dset, const Uint8 bits_allocated, const DcmSegTypes::E_SegmentationLabelmapColorModel cm)
{
    // Check imaging parameters
    Uint16 uint16;
    OFString str;
    OFCHECK(dset->findAndGetUint16(DCM_BitsAllocated, uint16).good());
    OFCHECK(uint16 == bits_allocated);
    OFCHECK(dset->findAndGetUint16(DCM_BitsStored, uint16).good());
    OFCHECK(uint16 == bits_allocated);
    OFCHECK(dset->findAndGetUint16(DCM_HighBit, uint16).good());
    OFCHECK(uint16 == bits_allocated - 1);
    OFCHECK(dset->findAndGetUint16(DCM_SamplesPerPixel, uint16).good());
    OFCHECK(uint16 == 1);
    OFCHECK(dset->findAndGetUint16(DCM_PixelRepresentation, uint16).good());
    OFCHECK(uint16 == 0);
    OFCHECK(dset->findAndGetOFString(DCM_PhotometricInterpretation, str).good());
    if (cm == DcmSegTypes::SLCM_MONOCHROME2)
    {
        OFCHECK(str == "MONOCHROME2");
    }
    else
    {
        OFCHECK(str == "PALETTE COLOR");
        checkPalette(dset, bits_allocated);
    }
    Sint32 numFrames = 0;
    OFCHECK(dset->findAndGetSint32(DCM_NumberOfFrames, numFrames).good());
    OFCHECK(numFrames > 0);
    OFCHECK(OFstatic_cast(Uint16, numFrames) == NUM_FRAMES);
    OFCHECK(dset->findAndGetUint16(DCM_Rows, uint16).good());
    OFCHECK(uint16 == NUM_ROWS);
    OFCHECK(dset->findAndGetUint16(DCM_Columns, uint16).good());
    OFCHECK(uint16 == NUM_COLS);
    OFCHECK(dset->findAndGetOFString(DCM_SegmentationType, str).good());
    OFCHECK(DcmSegTypes::OFString2Segtype(str) == DcmSegTypes::ST_LABELMAP);

    // Check pixel data
    if (bits_allocated == 8)
    {
        const Uint8* pixData = NULL;
        OFCHECK(dset->findAndGetUint8Array(DCM_PixelData, pixData).good());
        OFCHECK(pixData != NULL);
        if (pixData)
        {
            Uint16 max_value = 256;
            Uint32 numPixels = NUM_ROWS * NUM_COLS * NUM_FRAMES;
            for (Uint32 i = 0; i < numPixels; i++)
            {
                OFCHECK(pixData[i] == OFstatic_cast(Uint8, i % max_value));
            }
        }
        // pixData contains only reference, no need to delete
    }
    else if (bits_allocated == 16)
    {
        const Uint16* pixData = NULL;
        OFCHECK(dset->findAndGetUint16Array(DCM_PixelData, pixData).good());
        OFCHECK(pixData != NULL);
        if (pixData)
        {
            for (Uint32 i = 0; i < NUM_PIXELS_PER_FRAME * numFrames; i++)
            {
                OFCHECK(pixData[i] == OFstatic_cast(Uint16, i % 65535));
                // report expected and actual value in case of mismatch
                if (pixData[i] != OFstatic_cast(Uint16, i % 65535))
                {
                    DCMSEG_DEBUG("Expected at index " << i << ": " << OFstatic_cast(Uint16, i % 65535) << ", actual value: " << pixData[i]);
                }
            }
        }
        // pixData contains only reference, no need to delete
    }

    // check segments
    DcmSequenceOfItems* seq = NULL;
    OFCHECK(dset->findAndGetSequence(DCM_SegmentSequence, seq).good());
    OFCHECK(seq != NULL);
    if (seq)
    {
        OFCHECK(seq->card() == ((bits_allocated == 16) ? 65535u : 255u));
    }
    // check that there is no Segmentation FG
    DcmElement *fgSeq = NULL;
    OFCHECK(dset->findAndGetElement(DCM_SegmentIdentificationSequence, fgSeq, OFTrue /* search into sub */).bad());
}

static void addPaletteAndICCProfile(DcmSegmentation* seg, const Uint8 bitsAllocated)
{
    if (bitsAllocated == 8)
    {
        IODPaletteColorLUTModule& pal8 = seg->getPaletteColorLUT();
        OFCHECK(pal8.setRedPaletteColorLookupTableDescriptor(255, 0, 8).good());
        OFCHECK(pal8.setGreenPaletteColorLookupTableDescriptor(255, 0, 8).good());
        OFCHECK(pal8.setBluePaletteColorLookupTableDescriptor(255, 0, 8).good());
        // re-use the same data for all entries
        const Uint8 MAX_8BIT_ENTRIES = 255;
        Uint8* data = new Uint8[MAX_8BIT_ENTRIES];
        for (Uint16 i = 0; i < MAX_8BIT_ENTRIES; i++)
        {
            data[i] = OFstatic_cast(Uint8, i);
        }
        OFCHECK(pal8.setRedPaletteColorLookupTableData(data, MAX_8BIT_ENTRIES).good());
        OFCHECK(pal8.setGreenPaletteColorLookupTableData(data, MAX_8BIT_ENTRIES).good());
        OFCHECK(pal8.setBluePaletteColorLookupTableData(data, MAX_8BIT_ENTRIES).good());
        delete [] data;
    }
    else if (bitsAllocated == 16)
    {
        IODPaletteColorLUTModule& pal16= seg->getPaletteColorLUT();
        OFCHECK(pal16.setRedPaletteColorLookupTableDescriptor(65535, 0, 16).good());
        OFCHECK(pal16.setGreenPaletteColorLookupTableDescriptor(65535, 0, 16).good());
        OFCHECK(pal16.setBluePaletteColorLookupTableDescriptor(65535, 0, 16).good());
        // re-use the same data for all entries
        Uint16* data = new Uint16[65536];
        for (Uint16 i = 0; i < 65535u; i++) // TODO
        {
            data[i] = i;
        }
        OFCHECK(pal16.setRedPaletteColorLookupTableData(data, 65535).good());
        OFCHECK(pal16.setGreenPaletteColorLookupTableData(data, 65535).good());
        OFCHECK(pal16.setBluePaletteColorLookupTableData(data, 65535).good());
        delete [] data;
    }
    else
    {
        OFCHECK_FAIL("Unsupported value for bitsAllocated");
        return;
    }
    // Palette Color LUT Module requires ICC profile information.
    // Create ICC profile dummy data which is definitely not a valid ICC profile,
    // but should be sufficient for testing purposes.
    const size_t ICC_LENGTH = 256; // length of ICC profile in bytes
    Uint8* iccProfile = new Uint8[ICC_LENGTH];
    for (size_t i = 0; i < ICC_LENGTH; i++)
    {
        iccProfile[i] = OFstatic_cast(Uint8, i);
    }
    OFCHECK(seg->getICCProfile().setICCProfile(iccProfile, ICC_LENGTH, OFTrue /* check */).good());
    OFCHECK(seg->getICCProfile().setColorSpace("SRGB", OFTrue /* check */).good());
    delete[] iccProfile;
}

static void checkPalette(DcmDataset* dset, const Uint8 bitsAllocated)
{
    if (bitsAllocated == 8)
    {
        Uint8 MAX_ENTRIES_8_BIT = 255;
        IODPaletteColorLUTModule pal8;
        OFCHECK(pal8.read(*dset).good());
        Uint16 numEntries, firstEntry, bits;
        OFCHECK(pal8.getRedPaletteColorLookupTableDescriptor(numEntries, 0).good());
        OFCHECK(pal8.getRedPaletteColorLookupTableDescriptor(firstEntry, 1).good());
        OFCHECK(pal8.getRedPaletteColorLookupTableDescriptor(bits, 2).good());
        OFCHECK(numEntries == 255);
        OFCHECK(firstEntry == 0);
        OFCHECK(bits == bitsAllocated);
        OFCHECK(pal8.getGreenPaletteColorLookupTableDescriptor(numEntries, 0).good());
        OFCHECK(pal8.getGreenPaletteColorLookupTableDescriptor(firstEntry, 1).good());
        OFCHECK(pal8.getGreenPaletteColorLookupTableDescriptor(bits, 2).good());
        OFCHECK(numEntries == MAX_ENTRIES_8_BIT);
        OFCHECK(firstEntry == 0);
        OFCHECK(bits == bitsAllocated);
        OFCHECK(pal8.getBluePaletteColorLookupTableDescriptor(numEntries, 0).good());
        OFCHECK(pal8.getBluePaletteColorLookupTableDescriptor(firstEntry, 1).good());
        OFCHECK(pal8.getBluePaletteColorLookupTableDescriptor(bits, 2).good());
        OFCHECK(numEntries == MAX_ENTRIES_8_BIT);
        OFCHECK(firstEntry == 0);
        OFCHECK(bits == bitsAllocated);

        const Uint8* redData = NULL;
        const Uint8* greenData = NULL;
        const Uint8* blueData = NULL;
        unsigned long numEntriesRed, numEntriesGreen, numEntriesBlue;
        OFCHECK(pal8.getRedPaletteColorLookupTableData(redData, numEntriesRed).good());
        OFCHECK(pal8.getGreenPaletteColorLookupTableData(greenData, numEntriesGreen).good());
        OFCHECK(pal8.getBluePaletteColorLookupTableData(blueData, numEntriesBlue).good());
        OFCHECK(numEntriesRed == MAX_ENTRIES_8_BIT);
        OFCHECK(numEntriesBlue == MAX_ENTRIES_8_BIT);
        OFCHECK(numEntriesGreen == MAX_ENTRIES_8_BIT);
        OFCHECK(redData != NULL);
        OFCHECK(greenData != NULL);
        OFCHECK(blueData != NULL);
        if (redData && greenData && blueData)
        {
            for (Uint32 i = 0; i < MAX_ENTRIES_8_BIT; i++)
            {
                OFCHECK(redData[i] == OFstatic_cast(Uint8, i));
                OFCHECK(greenData[i] == OFstatic_cast(Uint8, i));
                OFCHECK(blueData[i] == OFstatic_cast(Uint8, i));
            }
        }
        delete[] redData;
        delete[] greenData;
        delete[] blueData;
    }
    else if (bitsAllocated == 16)
    {
        IODPaletteColorLUTModule pal16;
        OFCHECK(pal16.read(*dset).good());
        Uint16 numEntries, firstEntry, bits;
        OFCHECK(pal16.getRedPaletteColorLookupTableDescriptor(numEntries, 0).good());
        OFCHECK(pal16.getRedPaletteColorLookupTableDescriptor(firstEntry, 1).good());
        OFCHECK(pal16.getRedPaletteColorLookupTableDescriptor(bits, 2).good());
        OFCHECK(numEntries == 65535);
        OFCHECK(firstEntry == 0);
        OFCHECK(bits == bitsAllocated);
        OFCHECK(pal16.getGreenPaletteColorLookupTableDescriptor(numEntries, 0).good());
        OFCHECK(pal16.getGreenPaletteColorLookupTableDescriptor(firstEntry, 1).good());
        OFCHECK(pal16.getGreenPaletteColorLookupTableDescriptor(bits, 2).good());
        OFCHECK(numEntries == 65535);
        OFCHECK(firstEntry == 0);
        OFCHECK(bits == bitsAllocated);
        OFCHECK(pal16.getBluePaletteColorLookupTableDescriptor(numEntries, 0).good());
        OFCHECK(pal16.getBluePaletteColorLookupTableDescriptor(firstEntry, 1).good());
        OFCHECK(pal16.getBluePaletteColorLookupTableDescriptor(bits, 2).good());
        OFCHECK(numEntries == 65535);
        OFCHECK(firstEntry == 0 );
        OFCHECK(bits == bitsAllocated);

        const Uint16* redData = NULL;
        const Uint16* greenData = NULL;
        const Uint16* blueData = NULL;
        unsigned long numEntriesRed, numEntriesGreen, numEntriesBlue;
        OFCHECK(pal16.getRedPaletteColorLookupTableData(redData, numEntriesRed).good());
        OFCHECK(pal16.getGreenPaletteColorLookupTableData(greenData, numEntriesGreen).good());
        OFCHECK(pal16.getBluePaletteColorLookupTableData(blueData, numEntriesBlue).good());
        OFCHECK(numEntriesRed == 65535);
        OFCHECK(numEntriesBlue == 65535);
        OFCHECK(numEntriesGreen == 65535);
        OFCHECK(redData != NULL);
        OFCHECK(greenData != NULL);
        OFCHECK(blueData != NULL);
        if (redData && greenData && blueData)
        {
            for (Uint32 i = 0; i < 65535; i++)
            {
                OFCHECK(redData[i] == i);
                OFCHECK(greenData[i] == i);
                OFCHECK(blueData[i] == i);
            }
        }
    }
}

