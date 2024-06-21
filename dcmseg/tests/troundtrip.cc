/*
 *
 *  Copyright (C) 2019-2024, OFFIS e.V.
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
 *  Purpose: Test for creating, writing and reading Segmentation objects
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmseg/segdoc.h"
#include "dcmtk/dcmseg/segment.h"
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

static const Uint8 NUM_ROWS             = 10;
static const Uint8 NUM_COLS             = 10;
static const Uint8 NUM_FRAMES           = 10;
static const Uint8 NUM_PIXELS_PER_FRAME = NUM_COLS * NUM_ROWS;

static OFString EXPECTED_DUMP;

static void prepareExpectedDump();
static DcmSegmentation* create();
static void setGenericValues(DcmSegmentation* seg);
static void addSharedFGs(DcmSegmentation* seg);
static void addFrames(DcmSegmentation* seg);
static void addDimensions(DcmSegmentation* seg);
static OFString write(DcmSegmentation* seg, DcmDataset& ds);
static void writeAndCheckConcatenation(DcmSegmentation* seg, OFList<OFFilename>& concats);
static void checkCreatedObject(const OFString& ds_dump);
static void loadAndCheckConcatenation(const OFList<OFFilename>& concats);
static void checkConcatenationInstance(size_t numInstance, DcmSegmentation* srcInstance, DcmDataset* concatInstance);

OFTEST(dcmseg_roundtrip)
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
    addFrames(seg);
    addDimensions(seg);

    // Write to dataset and compare its dump with expected result
    DcmFileFormat dcmff;
    DcmDataset* ds = dcmff.getDataset();
    prepareExpectedDump();
    OFString dset_dump = write(seg, *ds);
    checkCreatedObject(dset_dump);

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
    DcmSegmentation::loadFile(temp_fn, seg).good();
    OFCHECK(seg != OFnullptr);
    if (seg)
    {
        dset_dump = write(seg, *ds);
        checkCreatedObject(dset_dump);
    }

    // Check writing the object to a concatenation
    OFList<OFFilename> concats;
    writeAndCheckConcatenation(seg, concats);

    // Re-load object from concatenation
    loadAndCheckConcatenation(concats);

    OFListIterator(OFFilename) delFile = concats.begin();
    while (delFile != concats.end())
    {
        OFStandard::deleteFile(*delFile);
        delFile++;
    }
    delete seg;
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

static void addFrames(DcmSegmentation* seg)
{
    if (!seg)
        return;

    FGSegmentation* fg_seg = new FGSegmentation();
    FGFrameContent* fg     = new FGFrameContent();
    OFCHECK(fg && fg_seg);
    fg->setStackID("1");
    if (fg)
    {
        for (Uint16 frameNo = 1; frameNo <= NUM_FRAMES; frameNo++)
        {
            DcmSegment* segment = NULL;
            CodeSequenceMacro category("85756007", "SCT", "Tissue");
            CodeSequenceMacro propType("51114001", "SCT", "Artery");
            OFCHECK(DcmSegment::create(segment, "SEGLABEL", category, propType, DcmSegTypes::SAT_AUTOMATIC, "OC_DUMMY")
                        .good());
            OFCHECK(segment != OFnullptr);
            OFCHECK(seg->addSegment(segment, frameNo).good());

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
            for (Uint8 i = 0; i < NUM_PIXELS_PER_FRAME; ++i)
            {
                data[i] = i;
            }
            OFCHECK(fg_seg->setReferencedSegmentNumber(frameNo).good());
            OFVector<FGBase*> perFrameFGs;
            perFrameFGs.push_back(fg);
            perFrameFGs.push_back(fg_seg);
            OFCHECK(seg->addFrame(data, frameNo, perFrameFGs).good());
            delete[] data;
        }
    }
    delete fg;
    delete fg_seg;
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
    // Make dump and return it
    OFStringStream sstream;
    ds.print(sstream);
    OFSTRINGSTREAM_GETOFSTRING(sstream, dump);

    return dump;
}

static void writeAndCheckConcatenation(DcmSegmentation* seg, OFList<OFFilename>& concats)
{
    ConcatenationCreator cc;
    cc.setCfgFramesPerInstance(1);
    OFCHECK(seg->writeConcatenation(cc).good());
    size_t numInstances = cc.getNumInstances();
    OFCHECK(numInstances == NUM_FRAMES);
    OFCondition result;
    for (size_t n = 0; n < numInstances; n++)
    {
        OFStringStream s;
        s << "concat_" << n << "_";
        OFTempFile tf(O_RDWR, "", s.str().c_str(), ".dcm");
        result = cc.writeNextInstance(tf.getFilename());
        OFCHECK(result.good());
        if (result.good())
        {
            DcmFileFormat concat;
            result = concat.loadFile(tf.getFilename());
            OFCHECK(result.good());
            checkConcatenationInstance(n, seg, concat.getDataset());
            concats.push_back(tf.getFilename());
            tf.stealFile();
        }
    }
}

static void checkConcatenationInstance(size_t numInstance, DcmSegmentation* srcInstance, DcmDataset* concatInstance)
{
    DcmSegmentation* concat = NULL;
    OFCHECK(DcmSegmentation::loadDataset(*concatInstance, concat).good());
    if (concat == NULL) return; // loadDataset() failed, we cannot continue
    size_t numFrames;
    numFrames = concat->getNumberOfFrames();
    OFCHECK(numFrames == 1);
    size_t numSegs = concat->getNumberOfSegments();
    OFCHECK(numSegs == NUM_FRAMES);
    IODMultiFrameFGModule::ConcatenationInfo& ci = concat->getConcatenationInfo();
    OFString val;
    OFCHECK(ci.getConcatenationUID(val).good());
    OFCHECK(DcmUniqueIdentifier::checkStringValue(val, "1").good());
    Uint32 frameOffsetNo = 0;
    OFCHECK(ci.getConcatenationFrameOffsetNumber(frameOffsetNo).good());
    OFCHECK(frameOffsetNo == numInstance);
    Uint16 inConcatNo = 0;
    OFCHECK(ci.getInConcatenationNumber(inConcatNo).good());
    OFCHECK(inConcatNo == numInstance + 1);
    Uint16 concatTotalNo = 0;
    OFCHECK(ci.getInConcatenationTotalNumber(concatTotalNo).good());
    OFCHECK(concatTotalNo == NUM_FRAMES);

    OFString srcUID;
    OFCHECK(ci.getSOPInstanceUIDOfConcatenationSource(srcUID).good());
    OFCHECK(srcInstance->getSOPCommon().getSOPInstanceUID(val).good());
    OFCHECK(srcUID == val);

    OFCHECK(concat->getSOPCommon().getSOPInstanceUID(val).good());
    OFCHECK(srcUID != val);

    FunctionalGroups::const_iterator srcShared = srcInstance->getFunctionalGroups().getShared()->begin();
    FunctionalGroups::const_iterator cShared   = concat->getFunctionalGroups().getShared()->begin();
    do
    {
        OFCHECK(srcShared->second->compare(*cShared->second) == 0);
        srcShared++;
        cShared++;
    } while ((srcShared != srcInstance->getFunctionalGroups().getShared()->end())
             && (cShared != concat->getFunctionalGroups().getShared()->end()));
    OFCHECK((srcShared == srcInstance->getFunctionalGroups().getShared()->end())
            && (cShared == concat->getFunctionalGroups().getShared()->end()));
    DcmSequenceOfItems* cPerFrame = NULL;
    OFCHECK(concatInstance->findAndGetSequence(DCM_PerFrameFunctionalGroupsSequence, cPerFrame).good());
    OFCHECK(cPerFrame && (cPerFrame->card() == 1));

    OFBool perFrame = OFFalse;
    FGBase* fg      = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_FRAMECONTENT, perFrame);
    OFCHECK(fg != NULL);
    OFCHECK(perFrame == OFTrue);

    fg = NULL;
    fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_SEGMENTATION, perFrame);
    OFCHECK(fg != NULL);
    OFCHECK(perFrame == OFTrue);

    fg = NULL;
    fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_PIXELMEASURES, perFrame);
    OFCHECK(fg != NULL);
    OFCHECK(perFrame == OFFalse);

    fg = NULL;
    fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_PLANEPOSPATIENT, perFrame);
    OFCHECK(fg != NULL);
    OFCHECK(perFrame == OFFalse);

    fg = NULL;
    fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_PLANEORIENTPATIENT, perFrame);
    OFCHECK(fg != NULL);
    OFCHECK(perFrame == OFFalse);

    const DcmIODTypes::Frame* frame = concat->getFrame(0);
    OFCHECK(frame != OFnullptr);
    OFCHECK(frame->pixData != OFnullptr);
    OFCHECK(OFstatic_cast(Uint8, frame->length) == NUM_PIXELS_PER_FRAME);
    for (size_t pix = 0; pix < frame->length; pix++)
    {
        OFCHECK(frame->pixData[pix] == pix);
    }
    delete concat;
}

static void loadAndCheckConcatenation(const OFList<OFFilename>& concats)
{
    ConcatenationLoader cl;
    OFCondition result = cl.scan(concats);
    OFCHECK(result.good());
    if (result.good())
    {
        ConcatenationLoader::TScanResult results = cl.getInfo();
        OFCHECK(results.size() == 1);
        if (results.size() != 1)
            return;
        DcmSegmentation* seg = NULL;
        result               = DcmSegmentation::loadConcatenation(cl, results.begin()->first, seg);

        OFCHECK(result.good());
        if (result.good())
        {
            OFStringStream s;
            DcmDataset dset;
            OFString sop;
            result = seg->writeDataset(dset);

            // Patch date and time so that it fits the prepared dump
            OFCHECK(dset.putAndInsertOFStringArray(DCM_ContentDate, "20190927").good());
            OFCHECK(dset.putAndInsertOFStringArray(DCM_ContentTime, "153857").good());

            dset.print(s);
            OFCHECK(result.good());
            if (result.good())
            {
                checkCreatedObject(s.str().c_str());
            }
            delete seg;
        }
    }
}

static void prepareExpectedDump()
{
    EXPECTED_DUMP = "\n";
    EXPECTED_DUMP += "# Dicom-Data-Set\n";
    // DcmDataset.print() produces dumps in local endianness, so make sure the dump reflects the current machine
    if (gLocalByteOrder == EBO_LittleEndian)
        EXPECTED_DUMP += "# Used TransferSyntax: Little Endian Explicit\n";
    else
        EXPECTED_DUMP += "# Used TransferSyntax: Big Endian Explicit\n";
    EXPECTED_DUMP += "(0008,0008) CS [DERIVED\\PRIMARY]                        #  16, 2 ImageType\n";
    EXPECTED_DUMP += "(0008,0016) UI =SegmentationStorage                     #  28, 1 SOPClassUID\n";
    EXPECTED_DUMP
        += "(0008,0018) UI [1.2.276.0.7230010.3.1.4.8323329.14863.1565940357.864813] #  56, 1 SOPInstanceUID\n";
    EXPECTED_DUMP += "(0008,0020) DA [20190801]                               #   8, 1 StudyDate\n";
    EXPECTED_DUMP += "(0008,0023) DA [20190927]                               #   8, 1 ContentDate\n";
    EXPECTED_DUMP += "(0008,0030) TM [120000]                                 #   6, 1 StudyTime\n";
    EXPECTED_DUMP += "(0008,0033) TM [153857]                                 #   6, 1 ContentTime\n";
    EXPECTED_DUMP += "(0008,0050) SH (no value available)                     #   0, 0 AccessionNumber\n";
    EXPECTED_DUMP += "(0008,0060) CS [SEG]                                    #   4, 1 Modality\n";
    EXPECTED_DUMP += "(0008,0070) LO [Open Connections]                       #  16, 1 Manufacturer\n";
    EXPECTED_DUMP += "(0008,0090) PN (no value available)                     #   0, 0 ReferringPhysicianName\n";
    EXPECTED_DUMP += "(0008,103e) LO [Test Description]                       #  16, 1 SeriesDescription\n";
    EXPECTED_DUMP += "(0008,1090) LO [OC CT]                                  #   6, 1 ManufacturerModelName\n";
    EXPECTED_DUMP += "(0010,0010) PN [Bond^James]                             #  10, 1 PatientName\n";
    EXPECTED_DUMP += "(0010,0020) LO [007]                                    #   4, 1 PatientID\n";
    EXPECTED_DUMP += "(0010,0030) DA [19771007]                               #   8, 1 PatientBirthDate\n";
    EXPECTED_DUMP += "(0010,0040) CS (no value available)                     #   0, 0 PatientSex\n";
    EXPECTED_DUMP += "(0010,1010) AS [040Y]                                   #   4, 1 PatientAge\n";
    EXPECTED_DUMP += "(0018,1000) LO [4711]                                   #   4, 1 DeviceSerialNumber\n";
    EXPECTED_DUMP += "(0018,1020) LO [0.1]                                    #   4, 1 SoftwareVersions\n";
    EXPECTED_DUMP += "(0018,5100) CS [HFS]                                    #   4, 1 PatientPosition\n";
    EXPECTED_DUMP
        += "(0020,000d) UI [1.2.276.0.7230010.3.1.2.8323329.14863.1565940357.864811] #  56, 1 StudyInstanceUID\n";
    EXPECTED_DUMP
        += "(0020,000e) UI [1.2.276.0.7230010.3.1.3.8323329.14863.1565940357.864812] #  56, 1 SeriesInstanceUID\n";
    EXPECTED_DUMP += "(0020,0010) SH [1]                                      #   2, 1 StudyID\n";
    EXPECTED_DUMP += "(0020,0011) IS [1]                                      #   2, 1 SeriesNumber\n";
    EXPECTED_DUMP += "(0020,0013) IS [1]                                      #   2, 1 InstanceNumber\n";
    EXPECTED_DUMP += "(0020,0052) UI [2.25.30853397773651184949181049330553108086] #  44, 1 FrameOfReferenceUID\n";
    EXPECTED_DUMP += "(0020,1040) LO (no value available)                     #   0, 0 PositionReferenceIndicator\n";
    EXPECTED_DUMP += "(0020,9221) SQ (Sequence with explicit length #=1)      #   0, 1 DimensionOrganizationSequence\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "    (0020,9164) UI [2.25.30855560781715986879861690673941231222] #  44, 1 DimensionOrganizationUID\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "(0020,9222) SQ (Sequence with explicit length #=2)      #   0, 1 DimensionIndexSequence\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=4)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "    (0020,9164) UI [2.25.30855560781715986879861690673941231222] #  44, 1 DimensionOrganizationUID\n";
    EXPECTED_DUMP += "    (0020,9165) AT (0020,9056)                              #   4, 1 DimensionIndexPointer\n";
    EXPECTED_DUMP += "    (0020,9167) AT (0020,9111)                              #   4, 1 FunctionalGroupPointer\n";
    EXPECTED_DUMP += "    (0020,9421) LO [STACK_DIM]                              #  10, 1 DimensionDescriptionLabel\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=4)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "    (0020,9164) UI [2.25.30855560781715986879861690673941231222] #  44, 1 DimensionOrganizationUID\n";
    EXPECTED_DUMP += "    (0020,9165) AT (0020,9057)                              #   4, 1 DimensionIndexPointer\n";
    EXPECTED_DUMP += "    (0020,9167) AT (0020,9111)                              #   4, 1 FunctionalGroupPointer\n";
    EXPECTED_DUMP += "    (0020,9421) LO [STACK_DIM]                              #  10, 1 DimensionDescriptionLabel\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "(0028,0002) US 1                                        #   2, 1 SamplesPerPixel\n";
    EXPECTED_DUMP += "(0028,0004) CS [MONOCHROME2]                            #  12, 1 PhotometricInterpretation\n";
    EXPECTED_DUMP += "(0028,0008) IS [10]                                     #   2, 1 NumberOfFrames\n";
    EXPECTED_DUMP += "(0028,0010) US 10                                       #   2, 1 Rows\n";
    EXPECTED_DUMP += "(0028,0011) US 10                                       #   2, 1 Columns\n";
    EXPECTED_DUMP += "(0028,0100) US 8                                        #   2, 1 BitsAllocated\n";
    EXPECTED_DUMP += "(0028,0101) US 8                                        #   2, 1 BitsStored\n";
    EXPECTED_DUMP += "(0028,0102) US 7                                        #   2, 1 HighBit\n";
    EXPECTED_DUMP += "(0028,0103) US 0                                        #   2, 1 PixelRepresentation\n";
    EXPECTED_DUMP += "(0028,2110) CS [00]                                     #   2, 1 LossyImageCompression\n";
    EXPECTED_DUMP += "(0062,0001) CS [FRACTIONAL]                             #  10, 1 SegmentationType\n";
    EXPECTED_DUMP += "(0062,0002) SQ (Sequence with explicit length #=10)     #   0, 1 SegmentSequence\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=6)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0062,0003) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "SegmentedPropertyCategoryCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [85756007]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Tissue]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0062,0004) US 1                                        #   2, 1 SegmentNumber\n";
    EXPECTED_DUMP += "    (0062,0005) LO [SEGLABEL]                               #   8, 1 SegmentLabel\n";
    EXPECTED_DUMP += "    (0062,0008) CS [AUTOMATIC]                              #  10, 1 SegmentAlgorithmType\n";
    EXPECTED_DUMP += "    (0062,0009) LO [OC_DUMMY]                               #   8, 1 SegmentAlgorithmName\n";
    EXPECTED_DUMP
        += "    (0062,000f) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentedPropertyTypeCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [51114001]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Artery]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=6)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0062,0003) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "SegmentedPropertyCategoryCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [85756007]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Tissue]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0062,0004) US 2                                        #   2, 1 SegmentNumber\n";
    EXPECTED_DUMP += "    (0062,0005) LO [SEGLABEL]                               #   8, 1 SegmentLabel\n";
    EXPECTED_DUMP += "    (0062,0008) CS [AUTOMATIC]                              #  10, 1 SegmentAlgorithmType\n";
    EXPECTED_DUMP += "    (0062,0009) LO [OC_DUMMY]                               #   8, 1 SegmentAlgorithmName\n";
    EXPECTED_DUMP
        += "    (0062,000f) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentedPropertyTypeCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [51114001]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Artery]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=6)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0062,0003) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "SegmentedPropertyCategoryCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [85756007]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Tissue]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0062,0004) US 3                                        #   2, 1 SegmentNumber\n";
    EXPECTED_DUMP += "    (0062,0005) LO [SEGLABEL]                               #   8, 1 SegmentLabel\n";
    EXPECTED_DUMP += "    (0062,0008) CS [AUTOMATIC]                              #  10, 1 SegmentAlgorithmType\n";
    EXPECTED_DUMP += "    (0062,0009) LO [OC_DUMMY]                               #   8, 1 SegmentAlgorithmName\n";
    EXPECTED_DUMP
        += "    (0062,000f) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentedPropertyTypeCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [51114001]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Artery]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=6)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0062,0003) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "SegmentedPropertyCategoryCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [85756007]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Tissue]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0062,0004) US 4                                        #   2, 1 SegmentNumber\n";
    EXPECTED_DUMP += "    (0062,0005) LO [SEGLABEL]                               #   8, 1 SegmentLabel\n";
    EXPECTED_DUMP += "    (0062,0008) CS [AUTOMATIC]                              #  10, 1 SegmentAlgorithmType\n";
    EXPECTED_DUMP += "    (0062,0009) LO [OC_DUMMY]                               #   8, 1 SegmentAlgorithmName\n";
    EXPECTED_DUMP
        += "    (0062,000f) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentedPropertyTypeCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [51114001]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Artery]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=6)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0062,0003) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "SegmentedPropertyCategoryCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [85756007]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Tissue]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0062,0004) US 5                                        #   2, 1 SegmentNumber\n";
    EXPECTED_DUMP += "    (0062,0005) LO [SEGLABEL]                               #   8, 1 SegmentLabel\n";
    EXPECTED_DUMP += "    (0062,0008) CS [AUTOMATIC]                              #  10, 1 SegmentAlgorithmType\n";
    EXPECTED_DUMP += "    (0062,0009) LO [OC_DUMMY]                               #   8, 1 SegmentAlgorithmName\n";
    EXPECTED_DUMP
        += "    (0062,000f) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentedPropertyTypeCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [51114001]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Artery]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=6)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0062,0003) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "SegmentedPropertyCategoryCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [85756007]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Tissue]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0062,0004) US 6                                        #   2, 1 SegmentNumber\n";
    EXPECTED_DUMP += "    (0062,0005) LO [SEGLABEL]                               #   8, 1 SegmentLabel\n";
    EXPECTED_DUMP += "    (0062,0008) CS [AUTOMATIC]                              #  10, 1 SegmentAlgorithmType\n";
    EXPECTED_DUMP += "    (0062,0009) LO [OC_DUMMY]                               #   8, 1 SegmentAlgorithmName\n";
    EXPECTED_DUMP
        += "    (0062,000f) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentedPropertyTypeCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [51114001]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Artery]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=6)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0062,0003) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "SegmentedPropertyCategoryCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [85756007]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Tissue]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0062,0004) US 7                                        #   2, 1 SegmentNumber\n";
    EXPECTED_DUMP += "    (0062,0005) LO [SEGLABEL]                               #   8, 1 SegmentLabel\n";
    EXPECTED_DUMP += "    (0062,0008) CS [AUTOMATIC]                              #  10, 1 SegmentAlgorithmType\n";
    EXPECTED_DUMP += "    (0062,0009) LO [OC_DUMMY]                               #   8, 1 SegmentAlgorithmName\n";
    EXPECTED_DUMP
        += "    (0062,000f) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentedPropertyTypeCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [51114001]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Artery]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=6)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0062,0003) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "SegmentedPropertyCategoryCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [85756007]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Tissue]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0062,0004) US 8                                        #   2, 1 SegmentNumber\n";
    EXPECTED_DUMP += "    (0062,0005) LO [SEGLABEL]                               #   8, 1 SegmentLabel\n";
    EXPECTED_DUMP += "    (0062,0008) CS [AUTOMATIC]                              #  10, 1 SegmentAlgorithmType\n";
    EXPECTED_DUMP += "    (0062,0009) LO [OC_DUMMY]                               #   8, 1 SegmentAlgorithmName\n";
    EXPECTED_DUMP
        += "    (0062,000f) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentedPropertyTypeCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [51114001]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Artery]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=6)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0062,0003) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "SegmentedPropertyCategoryCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [85756007]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Tissue]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0062,0004) US 9                                        #   2, 1 SegmentNumber\n";
    EXPECTED_DUMP += "    (0062,0005) LO [SEGLABEL]                               #   8, 1 SegmentLabel\n";
    EXPECTED_DUMP += "    (0062,0008) CS [AUTOMATIC]                              #  10, 1 SegmentAlgorithmType\n";
    EXPECTED_DUMP += "    (0062,0009) LO [OC_DUMMY]                               #   8, 1 SegmentAlgorithmName\n";
    EXPECTED_DUMP
        += "    (0062,000f) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentedPropertyTypeCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [51114001]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Artery]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=6)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0062,0003) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "SegmentedPropertyCategoryCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [85756007]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Tissue]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0062,0004) US 10                                       #   2, 1 SegmentNumber\n";
    EXPECTED_DUMP += "    (0062,0005) LO [SEGLABEL]                               #   8, 1 SegmentLabel\n";
    EXPECTED_DUMP += "    (0062,0008) CS [AUTOMATIC]                              #  10, 1 SegmentAlgorithmType\n";
    EXPECTED_DUMP += "    (0062,0009) LO [OC_DUMMY]                               #   8, 1 SegmentAlgorithmName\n";
    EXPECTED_DUMP
        += "    (0062,000f) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentedPropertyTypeCodeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,0100) SH [51114001]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "        (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "        (0008,0104) LO [Artery]                                 #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "(0062,000e) US 255                                      #   2, 1 MaximumFractionalValue\n";
    EXPECTED_DUMP += "(0062,0010) CS [OCCUPANCY]                              #  10, 1 SegmentationFractionalType\n";
    EXPECTED_DUMP += "(0070,0080) CS [LABEL]                                  #   6, 1 ContentLabel\n";
    EXPECTED_DUMP += "(0070,0081) LO [DESCRIPTION]                            #  12, 1 ContentDescription\n";
    EXPECTED_DUMP += "(0070,0084) PN [Doe^John]                               #   8, 1 ContentCreatorName\n";
    EXPECTED_DUMP
        += "(5200,9229) SQ (Sequence with explicit length #=1)      #   0, 1 SharedFunctionalGroupsSequence\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0020,9113) SQ (Sequence with explicit length #=1)      #   0, 1 PlanePositionSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0020,0032) DS [0.0\\0.0\\0.0]                            #  12, 3 ImagePositionPatient\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0020,9116) SQ (Sequence with explicit length #=1)      #   0, 1 PlaneOrientationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0020,0037) DS [1.0\\0.0\\0.0\\0.0\\1.0\\0.0]                #  24, 6 ImageOrientationPatient\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0028,9110) SQ (Sequence with explicit length #=1)      #   0, 1 PixelMeasuresSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0018,0050) DS [1.0]                                    #   4, 1 SliceThickness\n";
    EXPECTED_DUMP += "        (0018,0088) DS [0.05]                                   #   4, 1 SpacingBetweenSlices\n";
    EXPECTED_DUMP += "        (0028,0030) DS [0.1\\0.1]                                #   8, 2 PixelSpacing\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "(5200,9230) SQ (Sequence with explicit length #=10)     #   0, 1 PerFrameFunctionalGroupsSequence\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0020,9111) SQ (Sequence with explicit length #=1)      #   0, 1 FrameContentSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,9074) DT [20190816092557]                         #  14, 1 FrameAcquisitionDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9151) DT [20190816092557]                         #  14, 1 FrameReferenceDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9220) FD 0.001                                    #   8, 1 FrameAcquisitionDuration\n";
    EXPECTED_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    EXPECTED_DUMP += "        (0020,9057) UL 1                                        #   4, 1 InStackPositionNumber\n";
    EXPECTED_DUMP
        += "        (0020,9156) US 1                                        #   2, 1 FrameAcquisitionNumber\n";
    EXPECTED_DUMP += "        (0020,9157) UL 1\\1                                      #   8, 2 DimensionIndexValues\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0062,000a) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentIdentificationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0062,000b) US 1                                        #   2, 1 ReferencedSegmentNumber\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0020,9111) SQ (Sequence with explicit length #=1)      #   0, 1 FrameContentSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,9074) DT [20190816092557]                         #  14, 1 FrameAcquisitionDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9151) DT [20190816092557]                         #  14, 1 FrameReferenceDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9220) FD 0.001                                    #   8, 1 FrameAcquisitionDuration\n";
    EXPECTED_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    EXPECTED_DUMP += "        (0020,9057) UL 2                                        #   4, 1 InStackPositionNumber\n";
    EXPECTED_DUMP
        += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    EXPECTED_DUMP += "        (0020,9157) UL 1\\2                                      #   8, 2 DimensionIndexValues\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0062,000a) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentIdentificationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0062,000b) US 2                                        #   2, 1 ReferencedSegmentNumber\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0020,9111) SQ (Sequence with explicit length #=1)      #   0, 1 FrameContentSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,9074) DT [20190816092557]                         #  14, 1 FrameAcquisitionDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9151) DT [20190816092557]                         #  14, 1 FrameReferenceDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9220) FD 0.001                                    #   8, 1 FrameAcquisitionDuration\n";
    EXPECTED_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    EXPECTED_DUMP += "        (0020,9057) UL 3                                        #   4, 1 InStackPositionNumber\n";
    EXPECTED_DUMP
        += "        (0020,9156) US 3                                        #   2, 1 FrameAcquisitionNumber\n";
    EXPECTED_DUMP += "        (0020,9157) UL 1\\3                                      #   8, 2 DimensionIndexValues\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0062,000a) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentIdentificationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0062,000b) US 3                                        #   2, 1 ReferencedSegmentNumber\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0020,9111) SQ (Sequence with explicit length #=1)      #   0, 1 FrameContentSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,9074) DT [20190816092557]                         #  14, 1 FrameAcquisitionDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9151) DT [20190816092557]                         #  14, 1 FrameReferenceDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9220) FD 0.001                                    #   8, 1 FrameAcquisitionDuration\n";
    EXPECTED_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    EXPECTED_DUMP += "        (0020,9057) UL 4                                        #   4, 1 InStackPositionNumber\n";
    EXPECTED_DUMP
        += "        (0020,9156) US 4                                        #   2, 1 FrameAcquisitionNumber\n";
    EXPECTED_DUMP += "        (0020,9157) UL 1\\4                                      #   8, 2 DimensionIndexValues\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0062,000a) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentIdentificationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0062,000b) US 4                                        #   2, 1 ReferencedSegmentNumber\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0020,9111) SQ (Sequence with explicit length #=1)      #   0, 1 FrameContentSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,9074) DT [20190816092557]                         #  14, 1 FrameAcquisitionDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9151) DT [20190816092557]                         #  14, 1 FrameReferenceDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9220) FD 0.001                                    #   8, 1 FrameAcquisitionDuration\n";
    EXPECTED_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    EXPECTED_DUMP += "        (0020,9057) UL 5                                        #   4, 1 InStackPositionNumber\n";
    EXPECTED_DUMP
        += "        (0020,9156) US 5                                        #   2, 1 FrameAcquisitionNumber\n";
    EXPECTED_DUMP += "        (0020,9157) UL 1\\5                                      #   8, 2 DimensionIndexValues\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0062,000a) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentIdentificationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0062,000b) US 5                                        #   2, 1 ReferencedSegmentNumber\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0020,9111) SQ (Sequence with explicit length #=1)      #   0, 1 FrameContentSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,9074) DT [20190816092557]                         #  14, 1 FrameAcquisitionDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9151) DT [20190816092557]                         #  14, 1 FrameReferenceDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9220) FD 0.001                                    #   8, 1 FrameAcquisitionDuration\n";
    EXPECTED_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    EXPECTED_DUMP += "        (0020,9057) UL 6                                        #   4, 1 InStackPositionNumber\n";
    EXPECTED_DUMP
        += "        (0020,9156) US 6                                        #   2, 1 FrameAcquisitionNumber\n";
    EXPECTED_DUMP += "        (0020,9157) UL 1\\6                                      #   8, 2 DimensionIndexValues\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0062,000a) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentIdentificationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0062,000b) US 6                                        #   2, 1 ReferencedSegmentNumber\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0020,9111) SQ (Sequence with explicit length #=1)      #   0, 1 FrameContentSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,9074) DT [20190816092557]                         #  14, 1 FrameAcquisitionDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9151) DT [20190816092557]                         #  14, 1 FrameReferenceDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9220) FD 0.001                                    #   8, 1 FrameAcquisitionDuration\n";
    EXPECTED_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    EXPECTED_DUMP += "        (0020,9057) UL 7                                        #   4, 1 InStackPositionNumber\n";
    EXPECTED_DUMP
        += "        (0020,9156) US 7                                        #   2, 1 FrameAcquisitionNumber\n";
    EXPECTED_DUMP += "        (0020,9157) UL 1\\7                                      #   8, 2 DimensionIndexValues\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0062,000a) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentIdentificationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0062,000b) US 7                                        #   2, 1 ReferencedSegmentNumber\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0020,9111) SQ (Sequence with explicit length #=1)      #   0, 1 FrameContentSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,9074) DT [20190816092557]                         #  14, 1 FrameAcquisitionDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9151) DT [20190816092557]                         #  14, 1 FrameReferenceDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9220) FD 0.001                                    #   8, 1 FrameAcquisitionDuration\n";
    EXPECTED_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    EXPECTED_DUMP += "        (0020,9057) UL 8                                        #   4, 1 InStackPositionNumber\n";
    EXPECTED_DUMP
        += "        (0020,9156) US 8                                        #   2, 1 FrameAcquisitionNumber\n";
    EXPECTED_DUMP += "        (0020,9157) UL 1\\8                                      #   8, 2 DimensionIndexValues\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0062,000a) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentIdentificationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0062,000b) US 8                                        #   2, 1 ReferencedSegmentNumber\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0020,9111) SQ (Sequence with explicit length #=1)      #   0, 1 FrameContentSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,9074) DT [20190816092557]                         #  14, 1 FrameAcquisitionDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9151) DT [20190816092557]                         #  14, 1 FrameReferenceDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9220) FD 0.001                                    #   8, 1 FrameAcquisitionDuration\n";
    EXPECTED_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    EXPECTED_DUMP += "        (0020,9057) UL 9                                        #   4, 1 InStackPositionNumber\n";
    EXPECTED_DUMP
        += "        (0020,9156) US 9                                        #   2, 1 FrameAcquisitionNumber\n";
    EXPECTED_DUMP += "        (0020,9157) UL 1\\9                                      #   8, 2 DimensionIndexValues\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0062,000a) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentIdentificationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0062,000b) US 9                                        #   2, 1 ReferencedSegmentNumber\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0020,9111) SQ (Sequence with explicit length #=1)      #   0, 1 FrameContentSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,9074) DT [20190816092557]                         #  14, 1 FrameAcquisitionDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9151) DT [20190816092557]                         #  14, 1 FrameReferenceDateTime\n";
    EXPECTED_DUMP
        += "        (0018,9220) FD 0.001                                    #   8, 1 FrameAcquisitionDuration\n";
    EXPECTED_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    EXPECTED_DUMP += "        (0020,9057) UL 10                                       #   4, 1 InStackPositionNumber\n";
    EXPECTED_DUMP
        += "        (0020,9156) US 10                                       #   2, 1 FrameAcquisitionNumber\n";
    EXPECTED_DUMP += "        (0020,9157) UL 1\\10                                     #   8, 2 DimensionIndexValues\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0062,000a) SQ (Sequence with explicit length #=1)      #   0, 1 SegmentIdentificationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0062,000b) US 10                                       #   2, 1 ReferencedSegmentNumber\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "(7fe0,0010) OB "
           "00\\01\\02\\03\\04\\05\\06\\07\\08\\09\\0a\\0b\\0c\\0d\\0e\\0f\\10\\11\\12\\13\\14\\15\\16\\17\\18\\19\\1a"
           "\\1b\\1c\\1d\\1e\\1f\\20\\21\\22\\23\\24\\25\\26\\27\\28\\29\\2a\\2b\\2c\\2d\\2e\\2f\\30\\31\\32\\33\\34\\3"
           "5\\36\\37\\38\\39\\3a\\3b\\3c\\3d\\3e\\3f\\40\\41\\42\\43\\44\\45\\46\\47\\48\\49\\4a\\4b\\4c\\4d\\4e\\4f\\"
           "50\\51\\52\\53\\54\\55\\56\\57\\58\\59\\5a\\5b\\5c\\5d\\5e\\5f\\60\\61\\62\\63\\00\\01\\02\\03\\04\\05\\06"
           "\\07\\08\\09\\0a\\0b\\0c\\0d\\0e\\0f\\10\\11\\12\\13\\14\\15\\16\\17\\18\\19\\1a\\1b\\1c\\1d\\1e\\1f\\20\\2"
           "1\\22\\23\\24\\25\\26\\27\\28\\29\\2a\\2b\\2c\\2d\\2e\\2f\\30\\31\\32\\33\\34\\35\\36\\37\\38\\39\\3a\\3b\\"
           "3c\\3d\\3e\\3f\\40\\41\\42\\43\\44\\45\\46\\47\\48\\49\\4a\\4b\\4c\\4d\\4e\\4f\\50\\51\\52\\53\\54\\55\\56"
           "\\57\\58\\59\\5a\\5b\\5c\\5d\\5e\\5f\\60\\61\\62\\63\\00\\01\\02\\03\\04\\05\\06\\07\\08\\09\\0a\\0b\\0c\\0"
           "d\\0e\\0f\\10\\11\\12\\13\\14\\15\\16\\17\\18\\19\\1a\\1b\\1c\\1d\\1e\\1f\\20\\21\\22\\23\\24\\25\\26\\27\\"
           "28\\29\\2a\\2b\\2c\\2d\\2e\\2f\\30\\31\\32\\33\\34\\35\\36\\37\\38\\39\\3a\\3b\\3c\\3d\\3e\\3f\\40\\41\\42"
           "\\43\\44\\45\\46\\47\\48\\49\\4a\\4b\\4c\\4d\\4e\\4f\\50\\51\\52\\53\\54\\55\\56\\57\\58\\59\\5a\\5b\\5c\\5"
           "d\\5e\\5f\\60\\61\\62\\63\\00\\01\\02\\03\\04\\05\\06\\07\\08\\09\\0a\\0b\\0c\\0d\\0e\\0f\\10\\11\\12\\13\\"
           "14\\15\\16\\17\\18\\19\\1a\\1b\\1c\\1d\\1e\\1f\\20\\21\\22\\23\\24\\25\\26\\27\\28\\29\\2a\\2b\\2c\\2d\\2e"
           "\\2f\\30\\31\\32\\33\\34\\35\\36\\37\\38\\39\\3a\\3b\\3c\\3d\\3e\\3f\\40\\41\\42\\43\\44\\45\\46\\47\\48\\4"
           "9\\4a\\4b\\4c\\4d\\4e\\4f\\50\\51\\52\\53\\54\\55\\56\\57\\58\\59\\5a\\5b\\5c\\5d\\5e\\5f\\60\\61\\62\\63\\"
           "00\\01\\02\\03\\04\\05\\06\\07\\08\\09\\0a\\0b\\0c\\0d\\0e\\0f\\10\\11\\12\\13\\14\\15\\16\\17\\18\\19\\1a"
           "\\1b\\1c\\1d\\1e\\1f\\20\\21\\22\\23\\24\\25\\26\\27\\28\\29\\2a\\2b\\2c\\2d\\2e\\2f\\30\\31\\32\\33\\34\\3"
           "5\\36\\37\\38\\39\\3a\\3b\\3c\\3d\\3e\\3f\\40\\41\\42\\43\\44\\45\\46\\47\\48\\49\\4a\\4b\\4c\\4d\\4e\\4f\\"
           "50\\51\\52\\53\\54\\55\\56\\57\\58\\59\\5a\\5b\\5c\\5d\\5e\\5f\\60\\61\\62\\63\\00\\01\\02\\03\\04\\05\\06"
           "\\07\\08\\09\\0a\\0b\\0c\\0d\\0e\\0f\\10\\11\\12\\13\\14\\15\\16\\17\\18\\19\\1a\\1b\\1c\\1d\\1e\\1f\\20\\2"
           "1\\22\\23\\24\\25\\26\\27\\28\\29\\2a\\2b\\2c\\2d\\2e\\2f\\30\\31\\32\\33\\34\\35\\36\\37\\38\\39\\3a\\3b\\"
           "3c\\3d\\3e\\3f\\40\\41\\42\\43\\44\\45\\46\\47\\48\\49\\4a\\4b\\4c\\4d\\4e\\4f\\50\\51\\52\\53\\54\\55\\56"
           "\\57\\58\\59\\5a\\5b\\5c\\5d\\5e\\5f\\60\\61\\62\\63\\00\\01\\02\\03\\04\\05\\06\\07\\08\\09\\0a\\0b\\0c\\0"
           "d\\0e\\0f\\10\\11\\12\\13\\14\\15\\16\\17\\18\\19\\1a\\1b\\1c\\1d\\1e\\1f\\20\\21\\22\\23\\24\\25\\26\\27\\"
           "28\\29\\2a\\2b\\2c\\2d\\2e\\2f\\30\\31\\32\\33\\34\\35\\36\\37\\38\\39\\3a\\3b\\3c\\3d\\3e\\3f\\40\\41\\42"
           "\\43\\44\\45\\46\\47\\48\\49\\4a\\4b\\4c\\4d\\4e\\4f\\50\\51\\52\\53\\54\\55\\56\\57\\58\\59\\5a\\5b\\5c\\5"
           "d\\5e\\5f\\60\\61\\62\\63\\00\\01\\02\\03\\04\\05\\06\\07\\08\\09\\0a\\0b\\0c\\0d\\0e\\0f\\10\\11\\12\\13\\"
           "14\\15\\16\\17\\18\\19\\1a\\1b\\1c\\1d\\1e\\1f\\20\\21\\22\\23\\24\\25\\26\\27\\28\\29\\2a\\2b\\2c\\2d\\2e"
           "\\2f\\30\\31\\32\\33\\34\\35\\36\\37\\38\\39\\3a\\3b\\3c\\3d\\3e\\3f\\40\\41\\42\\43\\44\\45\\46\\47\\48\\4"
           "9\\4a\\4b\\4c\\4d\\4e\\4f\\50\\51\\52\\53\\54\\55\\56\\57\\58\\59\\5a\\5b\\5c\\5d\\5e\\5f\\60\\61\\62\\63\\"
           "00\\01\\02\\03\\04\\05\\06\\07\\08\\09\\0a\\0b\\0c\\0d\\0e\\0f\\10\\11\\12\\13\\14\\15\\16\\17\\18\\19\\1a"
           "\\1b\\1c\\1d\\1e\\1f\\20\\21\\22\\23\\24\\25\\26\\27\\28\\29\\2a\\2b\\2c\\2d\\2e\\2f\\30\\31\\32\\33\\34\\3"
           "5\\36\\37\\38\\39\\3a\\3b\\3c\\3d\\3e\\3f\\40\\41\\42\\43\\44\\45\\46\\47\\48\\49\\4a\\4b\\4c\\4d\\4e\\4f\\"
           "50\\51\\52\\53\\54\\55\\56\\57\\58\\59\\5a\\5b\\5c\\5d\\5e\\5f\\60\\61\\62\\63\\00\\01\\02\\03\\04\\05\\06"
           "\\07\\08\\09\\0a\\0b\\0c\\0d\\0e\\0f\\10\\11\\12\\13\\14\\15\\16\\17\\18\\19\\1a\\1b\\1c\\1d\\1e\\1f\\20\\2"
           "1\\22\\23\\24\\25\\26\\27\\28\\29\\2a\\2b\\2c\\2d\\2e\\2f\\30\\31\\32\\33\\34\\35\\36\\37\\38\\39\\3a\\3b\\"
           "3c\\3d\\3e\\3f\\40\\41\\42\\43\\44\\45\\46\\47\\48\\49\\4a\\4b\\4c\\4d\\4e\\4f\\50\\51\\52\\53\\54\\55\\56"
           "\\57\\58\\59\\5a\\5b\\5c\\5d\\5e\\5f\\60\\61\\62\\63 # 1000, 1 PixelData\n";
}

static void checkCreatedObject(const OFString& ds_dump)
{
    OFBool dump_ok = (ds_dump == EXPECTED_DUMP);
    OFCHECK(dump_ok);
    if (!dump_ok)
    {
        CERR << "Dump produced: " << OFendl << ds_dump << OFendl;
        CERR << "------------------------------------" << OFendl;
        CERR << "Dump expected: " << OFendl << EXPECTED_DUMP << OFendl;
        CERR << "------------------------------------" << OFendl;
    }
}
