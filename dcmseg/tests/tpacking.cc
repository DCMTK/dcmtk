/*
 *
 *  Copyright (C) 2025, OFFIS e.V.
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
 *  Purpose: Test for packing and unpacking binary segmentation pixel data
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmseg/segdoc.h"
#include "dcmtk/dcmseg/segment.h"
#include "dcmtk/dcmseg/segutils.h"
#include "dcmtk/ofstd/oftest.h"

#include "dcmtk/dcmfg/fgfracon.h"
#include "dcmtk/dcmfg/fgpixmsr.h"
#include "dcmtk/dcmfg/fgplanor.h"
#include "dcmtk/dcmfg/fgplanpo.h"
#include "dcmtk/dcmfg/fgseg.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/ofstd/oftest.h"
//#include <iostream>

static const Uint16 NUM_ROWS             = 512;
static const Uint16 NUM_COLS             = 512;
// 17388
static const Uint16 NUM_FRAMES           = 17000;
static const size_t NUM_PIXELS_PER_FRAME = NUM_COLS * NUM_ROWS;

static OFString EXPECTED_DUMP;

static void prepareExpectedDump();
static DcmSegmentation* create();
static void setGenericValues(DcmSegmentation* seg);
static void addSharedFGs(DcmSegmentation* seg);
static void addFrames(DcmSegmentation* seg);
static void addDimensions(DcmSegmentation* seg);
static OFString write(DcmSegmentation* seg, DcmDataset& ds);

OFTEST(dcmseg_packing)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    // Creation
    std::cout << "Creating segmentation" << std::endl;
    DcmSegmentation* seg = create();
    std::cout << "Setting generic values" << std::endl;
    setGenericValues(seg);
    std::cout << "Adding shared functional groups" << std::endl;
    addSharedFGs(seg);
    std::cout << "Adding frames" << std::endl;
    addFrames(seg);
    std::cout << "Adding dimensions" << std::endl;
    addDimensions(seg);

    // Write to dataset and compare its dump with expected result
    DcmFileFormat dcmff;
    DcmDataset* ds = dcmff.getDataset();

    // Save to disk, and re-load to test import
    seg->getFunctionalGroups().setCheckOnWrite(OFFalse);
    seg->getFunctionalGroups().setUseThreads(16);
    seg->setCheckDimensionsOnWrite(OFFalse);
    OFString temp_fn = "/tmp/test.dcm";
    std::cout << "Writing segmentation to file: " << temp_fn << std::endl;
    OFCHECK(seg->saveFile(temp_fn.c_str(), EXS_LittleEndianExplicit).good());
    std::cout << "Temporary file created: " << temp_fn << std::endl;
    delete seg;
}

static DcmSegmentation* create()
{
    IODGeneralEquipmentModule::EquipmentInfo eq("Open Connections", "OC CT", "4711", "0.1");
    ContentIdentificationMacro ci("1", "LABEL", "DESCRIPTION", "Doe^John");
    DcmSegmentation* seg = NULL;
    OFCondition result;
    DcmSegmentation::createBinarySegmentation(seg, NUM_ROWS, NUM_COLS , eq, ci);
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

static void addFrames(DcmSegmentation* seg)
{
    if (!seg)
        return;

    DcmSegment* segment = NULL;
    CodeSequenceMacro category("85756007", "SCT", "Tissue");
    CodeSequenceMacro propType("51114001", "SCT", "Artery");

    OFCHECK(DcmSegment::create(segment, "SEGLABEL", category, propType, DcmSegTypes::SAT_AUTOMATIC, "OC_DUMMY")
                .good());
    OFCHECK(segment != OFnullptr);
    Uint16 forget;
    OFCHECK(seg->addSegment(segment, forget).good());
    // Segmentation FG is created automatically
    FGFrameContent* fg     = new FGFrameContent();
    OFCHECK(fg);
    for (Uint16 frameNo = 1; frameNo <= NUM_FRAMES; frameNo++)
    {
        fg->setStackID("1");

        OFCHECK(fg->setFrameAcquisitionNumber(frameNo).good());
        OFCHECK(fg->setFrameReferenceDateTime("20190816092557").good());
        OFCHECK(fg->setFrameAcquisitionDateTime("20190816092557").good());
        OFCHECK(fg->setFrameAcquisitionDuration(0.001).good());
        OFCHECK(fg->setInStackPositionNumber(frameNo).good());
        OFCHECK(fg->setDimensionIndexValues(1, 0).good());
        OFCHECK(fg->setDimensionIndexValues(frameNo, 1).good());
        OFVector<FGBase*> groups;
        groups.push_back(fg);

        Uint8* data = new Uint8[NUM_PIXELS_PER_FRAME];
        for (size_t i = 0; i < NUM_PIXELS_PER_FRAME; ++i)
        {
            data[i] = 1;
        }
        OFVector<FGBase*> perFrameFGs;
        perFrameFGs.push_back(fg);
        OFCHECK(seg->addFrame(data, forget, perFrameFGs).good());
        delete[] data;
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

static OFString write(DcmSegmentation* seg, DcmDataset& ds)
{
    OFCondition result = seg->writeDataset(ds);
    OFCHECK(result.good());

    return "";
}
