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
 *  Module:  dcmect
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for creating and loading Enhanced CT objects
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/ofstd/oftest.h"

#include "dcmtk/dcmect/enhanced_ct.h"

#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmfg/concatenationcreator.h"
#include "dcmtk/dcmfg/concatenationloader.h"
#include "dcmtk/dcmfg/fgctacquisitiondetails.h"
#include "dcmtk/dcmfg/fgctacquisitiontype.h"
#include "dcmtk/dcmfg/fgctadditionalxraysource.h"
#include "dcmtk/dcmfg/fgctexposure.h"
#include "dcmtk/dcmfg/fgctgeometry.h"
#include "dcmtk/dcmfg/fgctimageframetype.h"
#include "dcmtk/dcmfg/fgctposition.h"
#include "dcmtk/dcmfg/fgctreconstruction.h"
#include "dcmtk/dcmfg/fgcttabledynamics.h"
#include "dcmtk/dcmfg/fgctxraydetails.h"
#include "dcmtk/dcmfg/fgfracon.h"
#include "dcmtk/dcmfg/fgframeanatomy.h"
#include "dcmtk/dcmfg/fgframevoilut.h"
#include "dcmtk/dcmfg/fgirradiationeventid.h"
#include "dcmtk/dcmfg/fgpixeltransform.h"
#include "dcmtk/dcmfg/fgpixmsr.h"
#include "dcmtk/dcmfg/fgplanor.h"
#include "dcmtk/dcmfg/fgplanpo.h"
#include "dcmtk/dcmfg/fgtemporalposition.h"

static OFLogger tRoundLogger = OFLog::getLogger("dcmtk.test.t_roundtrip");

// Do not change values below since
// a) The expected dataset (dump) is made for these values
// b) Total pixel data length in this test must not be bigger than
//    4 GB, otherwise the calls to writeDataset() will fail.
// The test dcmect/tests/t_huge_concat.cc allows for exercising
// "unlimited" pixel data size using writeConcatenation() on all
// occasions.

static const Uint16 NUM_ROWS             = 2;
static const Uint16 NUM_COLS             = 2;
static const Uint16 NUM_FRAMES           = 2;
static const size_t NUM_PIXELS_PER_FRAME = NUM_COLS * NUM_ROWS;
static const size_t NUM_FRAMES_CONCAT    = 1;

static OFString EXPECTED_DUMP;

static void prepareExpectedDump();
static EctEnhancedCT* create();
static void configureIOD(EctEnhancedCT* ct);
static void setGenericValues(EctEnhancedCT* ct);
static void addSharedFGs(EctEnhancedCT* ct);
static void addFrames(EctEnhancedCT* ct);
static void addDimensions(EctEnhancedCT* ct);
static OFString write(EctEnhancedCT* ct, DcmDataset& ds);
static void checkCreatedObject(const OFString& ds_dump);
static void writeAndCheckConcatenation(EctEnhancedCT* ct, OFList<OFFilename>& concats);
static void checkConcatenationInstance(size_t numInstance, EctEnhancedCT* srcInstance, DcmDataset* concatInstance);
static void loadAndCheckConcatenation(const OFList<OFFilename>& concats);

OFTEST(dcmect_roundtrip)
{
    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK(dcmDataDict.isDictionaryLoaded());
        return;
    }

    // Creation
    EctEnhancedCT* ct = create();
    configureIOD(ct);
    setGenericValues(ct);
    addSharedFGs(ct);
    addFrames(ct);
    addDimensions(ct);

    // Write to dataset and compare its dump with expected result
    DcmFileFormat dcmff;
    DcmDataset* ds = dcmff.getDataset();
    prepareExpectedDump();
    OFString dset_dump = write(ct, *ds);
    checkCreatedObject(dset_dump);

    // Save to disk, and re-load to test import
    OFTempFile tf;
    OFString temp_fn = tf.getFilename();
    OFCHECK(!temp_fn.empty());
    OFCHECK(ct->saveFile(temp_fn.c_str(), EXS_LittleEndianExplicit).good());

    // Read object from dataset into EctEnhancedCT object, write again to dataset and
    // check whether daset before saving to file is identical to object after loading from
    // disk and writing to dataset
    delete ct;
    ct = NULL;
    ds->clear();
    OFCHECK(EctEnhancedCT::loadFile(temp_fn, ct).good());
    OFCHECK(ct != NULL);
    if (ct)
    {
        dset_dump = write(ct, *ds);
        checkCreatedObject(dset_dump);
    }
    OFList<OFFilename> concats;
    if (ct)
    {
        writeAndCheckConcatenation(ct, concats);
        delete ct;
    }
    OFCHECK(!concats.empty());
    if (!concats.empty())
    {
        loadAndCheckConcatenation(concats);
    }
    OFListIterator(OFFilename) it = concats.begin();
    while (it != concats.end())
    {
        OFStandard::deleteFile(*it);
        it++;
    }
}

static EctEnhancedCT* create()
{
    IODEnhGeneralEquipmentModule::EquipmentInfo eq("Open Connections", "OC CT", "4711", "0.1");
    EctEnhancedCT* ct = NULL;
    OFCondition result;
    result = EctEnhancedCT::create(ct,
                                   NUM_ROWS,
                                   NUM_COLS,
                                   OFFalse,
                                   EctTypes::E_ImageType1_Original,
                                   EctTypes::DT_ImageType3_Volume,
                                   EctTypes::DT_ImageType4_Maximum,
                                   "1" /* instance number */,
                                   EctTypes::E_ContQuali_Research,
                                   EctTypes::E_PixelPres_Monochrome,
                                   EctTypes::E_VolProps_Volume,
                                   EctTypes::DT_VolBasedCalcTechnique_VolumeRender,
                                   eq,
                                   "20190801120000" /* acquisition date */,
                                   2.0 /* acquisition duration */);

    OFCHECK(result.good());
    OFCHECK(ct != OFnullptr);
    return ct;
}

static void configureIOD(EctEnhancedCT* ct)
{
    if (!ct)
        return;
}

static void setGenericValues(EctEnhancedCT* ct)
{
    if (!ct)
        return;
    OFCHECK(ct->getPatient().setPatientName("Bond^James").good());
    OFCHECK(ct->getPatient().setPatientID("007").good());
    OFCHECK(ct->getPatient().setPatientBirthDate("19771007").good());
    OFCHECK(ct->getStudy().setStudyDate("20190801").good());
    OFCHECK(ct->getStudy().setStudyTime("120000").good());
    OFCHECK(ct->getStudy().setStudyID("1").good());
    OFCHECK(ct->getPatientStudy().setPatientAge("040Y").good());
    OFCHECK(ct->getSeries().setSeriesDescription("Test Description").good());
    OFCHECK(ct->getSeries().setSeriesNumber("1").good());
    OFCHECK(ct->getSeries().setPatientPosition("HFS").good());

    // Those values are usually computed automatically. UIDS are generated and date/times are set to current values.
    // But in order to compare the "old" dump with the freshly created image attributes, we set some values manually,
    // so that they are not overwritten with new, automatically created values later.
    OFCHECK(ct->getStudy().setStudyInstanceUID("1.2.276.0.7230010.3.1.2.8323329.14863.1565940357.864811").good());
    OFCHECK(ct->getFrameOfReference().setFrameOfReferenceUID("2.25.30853397773651184949181049330553108086").good());
    OFCHECK(ct->getSeries().setSeriesInstanceUID("1.2.276.0.7230010.3.1.3.8323329.14863.1565940357.864812").good());
    OFCHECK(ct->getSOPCommon().setSOPInstanceUID("1.2.276.0.7230010.3.1.4.8323329.14863.1565940357.864813").good());

    OFCHECK(ct->getIODMultiFrameFGModule().setContentTime("092557").good());
    OFCHECK(ct->getIODMultiFrameFGModule().setContentDate("20190816").good());
}

static void addSharedFGs(EctEnhancedCT* ct)
{
    if (!ct)
        return;

    FGPixelMeasures meas;
    OFCHECK(meas.setPixelSpacing("0.1\\0.1").good());
    OFCHECK(meas.setSliceThickness("1.0").good());
    OFCHECK(meas.setSpacingBetweenSlices("0.05").good());

    FGPlanePosPatient planpo;
    OFCHECK(planpo.setImagePositionPatient("0.0", "0.0", "0.0").good());

    FGPlaneOrientationPatient planor;
    OFCHECK(planor.setImageOrientationPatient("1.0", "0.0", "0.0", "0.0", "1.0", "0.0").good());

    FGFrameAnatomy ana;
    OFCHECK(ana.setLaterality(FGFrameAnatomy::LATERALITY_BOTH).good());
    OFCHECK(ana.getAnatomy().getAnatomicRegion().set("12738006", "SCT", "Brain").good());

    FGFrameVOILUT voi;
    OFCHECK(voi.setCenterWidthExplanation(1000, 2000, FGFrameVOILUT::DT_CT_WindowCenterWidthExplanation_Brain).good());

    FGIrradiationEventIdentification irr;
    OFCHECK(irr.setIrradiationEventUID("2.25.30853892236613436472911970638347155062").good());

    FGCTImageFrameType itype;
    OFCHECK(itype.setFrameType("ORIGINAL\\PRIMARY\\VOLUME\\MAXIMUM").good());
    OFCHECK(itype.setPixelPresentation(FGCTImageFrameType::E_PixelPres_Monochrome).good());
    OFCHECK(itype.setVolumetricProperties(FGCTImageFrameType::E_VolProp_Volume).good());
    OFCHECK(itype.setVolumeBasedCalculationTechnique(FGCTImageFrameType::DT_VolBasedCalcTechnique_VolumeRender).good());

    FGCTAcquisitionType atype;
    OFCHECK(atype.setAcquisitionType(FGCTAcquisitionType::DT_AcquisitionType_ConstantAngle).good());
    OFCHECK(atype.setTubeAngle(0.1).good());
    OFCHECK(atype.setConstantVolumeFlag(FGCTAcquisitionType::E_ConstVol_Yes).good());
    OFCHECK(atype.setFluoroscopyFlag(FGCTAcquisitionType::E_Fluoroscopy_No).good());

    FGCTAcquisitionDetails adetails;
    FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem* item = new FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem();
    OFCHECK(item->setRotationDirection(FGCTAcquisitionDetails::E_RotationDirection_CW).good());
    OFCHECK(item->setRevolutionTime(5).good());
    OFCHECK(item->setSingleCollimationWidth(1).good());
    OFCHECK(item->setTotalCollimationWidth(10).good());
    OFCHECK(item->setTableHeight(50).good());
    OFCHECK(item->setGantryDetectorTilt(5).good());
    OFCHECK(item->setDataCollectionDiameter(20).good());
    adetails.getCTAcquisitionDetailsItems().push_back(item);

    FGCTTableDynamics dyn;
    FGCTTableDynamics::FGCTTableDynamicsItem* dyn_item = new FGCTTableDynamics::FGCTTableDynamicsItem;
    OFCHECK(dyn_item);
    if (dyn_item)
    {
        OFCHECK(dyn_item->setTableSpeed(1.0).good());
        OFCHECK(dyn_item->setTableFeedPerRotation(0.1).good());
        OFCHECK(dyn_item->setSpiralPitchFactor(0.2).good());
        dyn.getCTTableDynamicsItems().push_back(dyn_item);
    }

    FGCTPosition pos;
    OFCHECK(pos.setTablePosition(100.0).good());
    OFCHECK(pos.setReconstructionTargetCenterPatient(OFVector<Float64>(3, 1.0)).good());
    OFCHECK(pos.setDataCollectionCenterPatient(OFVector<Float64>(3, 2.0)).good());

    FGCTGeometry geo;
    FGCTGeometry::FGCTGeometryItem* geo_item = new FGCTGeometry::FGCTGeometryItem;
    if (geo_item)
    {
        OFCHECK(geo_item->setDistanceSourceToDataCollectionCenter(5.0).good());
        OFCHECK(geo_item->setDistanceSourceToDetector(0.5).good());
        geo.getCTGeometryItems().push_back(geo_item);
    }

    FGCTReconstruction rec;
    OFCHECK(rec.setConvolutionKernel("DUMMY").good());
    OFCHECK(rec.setConvolutionKernelGroup("DUMMYGROUP").good());
    OFCHECK(rec.setImageFilter("FILTER").good());
    OFCHECK(rec.setReconstructionAlgorithm("ALGO").good());
    OFCHECK(rec.setReconstructionAngle(90.0).good());
    OFCHECK(rec.setReconstructionDiameter(100.0).good());
    // Not permitted if Reconstruction Diameter is provided instead
    // OFCHECK(rec.setReconstructionFieldOfView(100.0, 100.0).good());
    OFCHECK(rec.setReconstructionPixelSpacing(0.1, 0.1).good());

    FGCTExposure exp;
    FGCTExposure::FGCTExposureItem* exp_item = new FGCTExposure::FGCTExposureItem;
    if (exp_item)
    {
        OFCHECK(exp_item->setCTDIVol(0.1).good());
        CodeSequenceMacro* phantom_item = new CodeSequenceMacro("113682", "DCM", "ACR Accreditation Phantom - CT");
        exp_item->getCTDIPhantomTypeCodeSequence().push_back(phantom_item);
        OFCHECK(exp_item->setExposureInMas(0.3).good());
        OFCHECK(exp_item->setExposureModulationType("WEIRD").good());
        OFCHECK(exp_item->setExposureTimeInMs(0.4).good());
        OFCHECK(exp_item->setImageAndFluoroscopyAreaDoseProduct(0.5).good());
        OFCHECK(exp_item->setWaterEquivalentDiameter(0.6).good());
        CodeSequenceMacro* water_code = new CodeSequenceMacro("113987", "DCM", "AAPM 220");
        exp_item->getWaterEquivalentDiameterCalculationMethodCodeSequence().push_back(water_code);
        OFCHECK(exp_item->setXRayTubeCurrentInMa(0.7).good());
        exp.getCTExposureItems().push_back(exp_item);
    }

    FGCTXRayDetails det;
    FGCTXRayDetails::FGCTXRayDetailsItem* det_item = new FGCTXRayDetails::FGCTXRayDetailsItem;
    if (det_item)
    {
        OFCHECK(det_item->setCalciumScoringMassFactorDevice(OFVector<Float32>(3, 1)).good());
        OFCHECK(det_item->setCalciumScoringMassFactorPatient(2).good());
        OFCHECK(det_item->setEnergyWeightingFactor(3).good());
        OFCHECK(det_item->setFilterMaterial("FILTER_MATERIAL").good());
        OFCHECK(det_item->setFilterType("FILTER_TYPE").good());
        OFCHECK(det_item->setFocalSpots(OFVector<Float64>(4, 4.4)).good());
        OFCHECK(det_item->setKVP(5.0).good());
        det.getCTXRayDetailsItems().push_back(det_item);
    }

    FGPixelValueTransformation trans;
    trans.setFGType(FGPixelValueTransformation::E_PixelValTrans_CT);
    trans.setRescaleIntercept("0");
    trans.setRescaleSlope("1");
    trans.setRescaleType("HU");

    FGCTAdditionalXRaySource asrc;
    FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem* asrc_item
        = new FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem;
    if (asrc_item)
    {
        OFCHECK(asrc_item->setDataCollectionDiameter(1.0).good());
        OFCHECK(asrc_item->setEnergyWeightingFactor(2.0).good());
        OFCHECK(asrc_item->setExposureInmAs(3.0).good());
        OFCHECK(asrc_item->setFilterMaterial("FILTER_MATERIAL").good());
        OFCHECK(asrc_item->setFilterType("FILTER_TYPE").good());
        OFCHECK(asrc_item->setFocalSpots(OFVector<Float64>(4, 4.4)).good());
        OFCHECK(asrc_item->setKVP(5).good());
        OFCHECK(asrc_item->setXRayTubeCurrentInmA(6).good());
        asrc.getCTAdditionalXRaySourceItems().push_back(asrc_item);
    }

    FGTemporalPosition tempos;
    OFCHECK(tempos.setTemporalPositionTimeOffset(1.0).good());

    OFCHECK(ct->addForAllFrames(meas).good());
    OFCHECK(ct->addForAllFrames(planpo).good());
    OFCHECK(ct->addForAllFrames(planor).good());
    OFCHECK(ct->addForAllFrames(ana).good());
    OFCHECK(ct->addForAllFrames(voi).good());
    OFCHECK(ct->addForAllFrames(irr).good());
    OFCHECK(ct->addForAllFrames(itype).good());
    OFCHECK(ct->addForAllFrames(atype).good());
    OFCHECK(ct->addForAllFrames(adetails).good());
    OFCHECK(ct->addForAllFrames(dyn).good());
    OFCHECK(ct->addForAllFrames(pos).good());
    OFCHECK(ct->addForAllFrames(geo).good());
    OFCHECK(ct->addForAllFrames(rec).good());
    OFCHECK(ct->addForAllFrames(exp).good());
    OFCHECK(ct->addForAllFrames(det).good());
    OFCHECK(ct->addForAllFrames(trans).good());
    OFCHECK(ct->addForAllFrames(asrc).good());
    OFCHECK(ct->addForAllFrames(tempos).good());
}

static void addFrames(EctEnhancedCT* ct)
{
    if (!ct)
        return;

    FGFrameContent* fg = new FGFrameContent();
    fg->setStackID("1");
    OFCHECK(fg);
    if (fg)
    {
        EctEnhancedCT::FramesType frames = ct->getFrames();
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

            Uint16* data = new Uint16[NUM_PIXELS_PER_FRAME];
            for (size_t i = 0; i < NUM_PIXELS_PER_FRAME; ++i)
            {
                data[i] = frameNo;
            }
            OFCHECK(
                OFget<EctEnhancedCT::Frames<Uint16> >(&frames)->addFrame(data, NUM_PIXELS_PER_FRAME, groups).good());
            delete[] data;
        }
    }
    delete fg;
}

static void addDimensions(EctEnhancedCT* ct)
{
    if (!ct)
        return;
    IODMultiframeDimensionModule& dims = ct->getDimensions();
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

static OFString write(EctEnhancedCT* ct, DcmDataset& ds)
{
    OFCondition result = ct->writeDataset(ds);
    OFCHECK(result.good());
    if (result.bad())
    {
        OFLOG_ERROR(tRoundLogger, "Writing Enhanced CT dataset failed: " << result.text() << OFendl);
    }
    // Make dump and return it
    OFStringStream sstream;
    ds.print(sstream);
    OFSTRINGSTREAM_GETOFSTRING(sstream, dump);
    return dump;
}

static void writeAndCheckConcatenation(EctEnhancedCT* ct, OFList<OFFilename>& concats)
{
    ConcatenationCreator cc;
    cc.setCfgFramesPerInstance(NUM_FRAMES_CONCAT);
    OFCHECK(ct->writeConcatenation(cc).good());
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
            OFCHECK(concat.loadFile(tf.getFilename()).good());
            checkConcatenationInstance(n, ct, concat.getDataset());
            concats.push_back(tf.getFilename());
            tf.stealFile();
        }
    }
}

static void checkConcatenationInstance(size_t numInstance, EctEnhancedCT* srcInstance, DcmDataset* concatInstance)
{
    EctEnhancedCT* concat = NULL;
    OFCHECK(EctEnhancedCT::loadDataset(*concatInstance, concat).good());
    if (concat)
    {
        size_t numFrames;
        numFrames = concat->getNumberOfFrames();
        OFCHECK(numFrames == 1);
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

        if (cPerFrame)
        {
            OFCHECK(cPerFrame->card() == 1);
        }

        OFBool perFrame = OFFalse;
        FGBase* fg      = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_CTACQUISITIONDETAILS, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_CTACQUISITIONTYPE, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_CTADDITIONALXRAYSOURCE, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_CTEXPOSURE, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_CTGEOMETRY, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_CTIMAGEFRAMETYPE, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_CTPOSITION, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_CTRECONSTRUCTION, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_CTTABLEDYNAMICS, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_CTXRAYDETAILS, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_CTXRAYDETAILS, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_FRAMECONTENT, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFTrue);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_FRAMEANATOMY, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_FRAMEANATOMY, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_FRAMEVOILUTMETA, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_FRAMEANATOMY, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_IRRADIATIONEVENTIDENT, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_PIXELVALUETRANSMETA, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_IRRADIATIONEVENTIDENT, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

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

        fg = concat->getFunctionalGroups().get(0, DcmFGTypes::EFG_TEMPORALPOSITION, perFrame);
        OFCHECK(fg != NULL);
        OFCHECK(perFrame == OFFalse);

        EctEnhancedCT::FramesType frames = concat->getFrames();
        Uint16* frame                    = OFget<EctEnhancedCT::Frames<Uint16> >(&frames)->getFrame(0);
        OFCHECK(frame != OFnullptr);
        // Check that all pixels are set to their original source instances frame number (starting from 1)
        for (size_t pix = 0; pix < NUM_PIXELS_PER_FRAME; pix++)
        {
            // We need to swap the 16 bit value if the test runs on big endian platforms
            swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, &frame[pix], 2, sizeof(Uint16));
            OFCHECK(frame[pix] == numInstance + 1);
        }
        delete concat;
    }
}

void loadAndCheckConcatenation(const OFList<OFFilename>& concats)
{
    ConcatenationLoader cl;
    OFCondition result = cl.scan(concats);
    OFCHECK(result.good());
    if (result.good())
    {
        DcmDataset merged;
        EctEnhancedCT* mergedCT = NULL;
        result                  = EctEnhancedCT::loadConcatenation(cl, cl.getInfo().begin()->first, mergedCT);
        if (result.good())
        {
            OFStringStream s;
            DcmDataset d;
            result = mergedCT->writeDataset(d);
            OFCHECK(result.good());
            if (result.good())
            {
                // patch in old timestamp to match dump
                OFCHECK(d.putAndInsertOFStringArray(DCM_ContentDate, "20190816").good());
                OFCHECK(d.putAndInsertOFStringArray(DCM_ContentTime, "092557").good());
                d.print(s);
                checkCreatedObject(s.str().c_str());
                delete mergedCT;
            }
        }
    }
}

static void prepareExpectedDump()
{
    EXPECTED_DUMP += "\n";
    EXPECTED_DUMP += "# Dicom-Data-Set\n";
    // DcmDataset.print() produces dumps in local endianness, so make sure the dump reflects the current machine
    if (gLocalByteOrder == EBO_LittleEndian)
        EXPECTED_DUMP += "# Used TransferSyntax: Little Endian Explicit\n";
    else
        EXPECTED_DUMP += "# Used TransferSyntax: Big Endian Explicit\n";
    EXPECTED_DUMP += "(0008,0008) CS [ORIGINAL\\PRIMARY\\VOLUME\\MAXIMUM]        #  32, 4 ImageType\n";
    EXPECTED_DUMP += "(0008,0016) UI =EnhancedCTImageStorage                  #  28, 1 SOPClassUID\n";
    EXPECTED_DUMP
        += "(0008,0018) UI [1.2.276.0.7230010.3.1.4.8323329.14863.1565940357.864813] #  56, 1 SOPInstanceUID\n";
    EXPECTED_DUMP += "(0008,0020) DA [20190801]                               #   8, 1 StudyDate\n";
    EXPECTED_DUMP += "(0008,0023) DA [20190816]                               #   8, 1 ContentDate\n";
    EXPECTED_DUMP += "(0008,002a) DT [20190801120000]                         #  14, 1 AcquisitionDateTime\n";
    EXPECTED_DUMP += "(0008,0030) TM [120000]                                 #   6, 1 StudyTime\n";
    EXPECTED_DUMP += "(0008,0033) TM [092557]                                 #   6, 1 ContentTime\n";
    EXPECTED_DUMP += "(0008,0050) SH (no value available)                     #   0, 0 AccessionNumber\n";
    EXPECTED_DUMP += "(0008,0060) CS [CT]                                     #   2, 1 Modality\n";
    EXPECTED_DUMP += "(0008,0070) LO [Open Connections]                       #  16, 1 Manufacturer\n";
    EXPECTED_DUMP += "(0008,0090) PN (no value available)                     #   0, 0 ReferringPhysicianName\n";
    EXPECTED_DUMP += "(0008,103e) LO [Test Description]                       #  16, 1 SeriesDescription\n";
    EXPECTED_DUMP += "(0008,1090) LO [OC CT]                                  #   6, 1 ManufacturerModelName\n";
    EXPECTED_DUMP += "(0008,9205) CS [MONOCHROME]                             #  10, 1 PixelPresentation\n";
    EXPECTED_DUMP += "(0008,9206) CS [VOLUME]                                 #   6, 1 VolumetricProperties\n";
    EXPECTED_DUMP
        += "(0008,9207) CS [VOLUME_RENDER]                          #  14, 1 VolumeBasedCalculationTechnique\n";
    EXPECTED_DUMP += "(0010,0010) PN [Bond^James]                             #  10, 1 PatientName\n";
    EXPECTED_DUMP += "(0010,0020) LO [007]                                    #   4, 1 PatientID\n";
    EXPECTED_DUMP += "(0010,0030) DA [19771007]                               #   8, 1 PatientBirthDate\n";
    EXPECTED_DUMP += "(0010,0040) CS (no value available)                     #   0, 0 PatientSex\n";
    EXPECTED_DUMP += "(0010,1010) AS [040Y]                                   #   4, 1 PatientAge\n";
    EXPECTED_DUMP += "(0018,1000) LO [4711]                                   #   4, 1 DeviceSerialNumber\n";
    EXPECTED_DUMP += "(0018,1020) LO [0.1]                                    #   4, 1 SoftwareVersions\n";
    EXPECTED_DUMP += "(0018,5100) CS [HFS]                                    #   4, 1 PatientPosition\n";
    EXPECTED_DUMP += "(0018,9004) CS [RESEARCH]                               #   8, 1 ContentQualification\n";
    EXPECTED_DUMP += "(0018,9073) FD 2                                        #   8, 1 AcquisitionDuration\n";
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
    EXPECTED_DUMP += "(0028,0008) IS [2]                                      #   2, 1 NumberOfFrames\n";
    EXPECTED_DUMP += "(0028,0010) US 2                                        #   2, 1 Rows\n";
    EXPECTED_DUMP += "(0028,0011) US 2                                        #   2, 1 Columns\n";
    EXPECTED_DUMP += "(0028,0100) US 16                                       #   2, 1 BitsAllocated\n";
    EXPECTED_DUMP += "(0028,0101) US 16                                       #   2, 1 BitsStored\n";
    EXPECTED_DUMP += "(0028,0102) US 15                                       #   2, 1 HighBit\n";
    EXPECTED_DUMP += "(0028,0103) US 0                                        #   2, 1 PixelRepresentation\n";
    EXPECTED_DUMP += "(0028,0301) CS [NO]                                     #   2, 1 BurnedInAnnotation\n";
    EXPECTED_DUMP += "(0028,2110) CS [00]                                     #   2, 1 LossyImageCompression\n";
    EXPECTED_DUMP += "(0040,0555) SQ (Sequence with explicit length #=0)      #   0, 1 AcquisitionContextSequence\n";
    EXPECTED_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "(2050,0020) CS [IDENTITY]                               #   8, 1 PresentationLUTShape\n";
    EXPECTED_DUMP
        += "(5200,9229) SQ (Sequence with explicit length #=1)      #   0, 1 SharedFunctionalGroupsSequence\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=18)         #   0, 1 Item\n";
    EXPECTED_DUMP += "    (0018,9301) SQ (Sequence with explicit length #=1)      #   0, 1 CTAcquisitionTypeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=4)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0018,9302) CS [CONSTANT_ANGLE]                         #  14, 1 AcquisitionType\n";
    EXPECTED_DUMP += "        (0018,9303) FD 0.1                                      #   8, 1 TubeAngle\n";
    EXPECTED_DUMP += "        (0018,9333) CS [YES]                                    #   4, 1 ConstantVolumeFlag\n";
    EXPECTED_DUMP += "        (0018,9334) CS [NO]                                     #   2, 1 FluoroscopyFlag\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0018,9304) SQ (Sequence with explicit length #=1)      #   0, 1 CTAcquisitionDetailsSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,0090) DS [20]                                     #   2, 1 DataCollectionDiameter\n";
    EXPECTED_DUMP += "        (0018,1120) DS [5]                                      #   2, 1 GantryDetectorTilt\n";
    EXPECTED_DUMP += "        (0018,1130) DS [50]                                     #   2, 1 TableHeight\n";
    EXPECTED_DUMP += "        (0018,1140) CS [CW]                                     #   2, 1 RotationDirection\n";
    EXPECTED_DUMP += "        (0018,9305) FD 5                                        #   8, 1 RevolutionTime\n";
    EXPECTED_DUMP
        += "        (0018,9306) FD 1                                        #   8, 1 SingleCollimationWidth\n";
    EXPECTED_DUMP += "        (0018,9307) FD 10                                       #   8, 1 TotalCollimationWidth\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0018,9308) SQ (Sequence with explicit length #=1)      #   0, 1 CTTableDynamicsSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0018,9309) FD 1                                        #   8, 1 TableSpeed\n";
    EXPECTED_DUMP += "        (0018,9310) FD 0.1                                      #   8, 1 TableFeedPerRotation\n";
    EXPECTED_DUMP += "        (0018,9311) FD 0.2                                      #   8, 1 SpiralPitchFactor\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0018,9312) SQ (Sequence with explicit length #=1)      #   0, 1 CTGeometrySequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,1110) DS [0.5]                                    #   4, 1 DistanceSourceToDetector\n";
    EXPECTED_DUMP += "        (0018,9335) FD 5                                        #   8, 1 "
                     "DistanceSourceToDataCollectionCenter\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0018,9314) SQ (Sequence with explicit length #=1)      #   0, 1 CTReconstructionSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,1100) DS [100]                                    #   4, 1 ReconstructionDiameter\n";
    EXPECTED_DUMP += "        (0018,1210) SH [DUMMY]                                  #   6, 1 ConvolutionKernel\n";
    EXPECTED_DUMP
        += "        (0018,9315) CS [ALGO]                                   #   4, 1 ReconstructionAlgorithm\n";
    EXPECTED_DUMP
        += "        (0018,9316) CS [DUMMYGROUP]                             #  10, 1 ConvolutionKernelGroup\n";
    EXPECTED_DUMP += "        (0018,9319) FD 90                                       #   8, 1 ReconstructionAngle\n";
    EXPECTED_DUMP += "        (0018,9320) SH [FILTER]                                 #   6, 1 ImageFilter\n";
    EXPECTED_DUMP
        += "        (0018,9322) FD 0.1\\0.1                                  #  16, 2 ReconstructionPixelSpacing\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0018,9321) SQ (Sequence with explicit length #=1)      #   0, 1 CTExposureSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=9)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0018,115e) DS [0.5]                                    #   4, 1 "
                     "ImageAndFluoroscopyAreaDoseProduct\n";
    EXPECTED_DUMP
        += "        (0018,1271) FD 0.6                                      #   8, 1 WaterEquivalentDiameter\n";
    EXPECTED_DUMP += "        (0018,1272) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "WaterEquivalentDiameterCalculationMethodCodeSequence\n";
    EXPECTED_DUMP += "          (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "            (0008,0100) SH [113987]                                 #   6, 1 CodeValue\n";
    EXPECTED_DUMP
        += "            (0008,0102) SH [DCM]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "            (0008,0104) LO [AAPM 220]                               #   8, 1 CodeMeaning\n";
    EXPECTED_DUMP
        += "          (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP
        += "        (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "        (0018,9323) CS [WEIRD]                                  #   6, 1 ExposureModulationType\n";
    EXPECTED_DUMP += "        (0018,9328) FD 0.4                                      #   8, 1 ExposureTimeInms\n";
    EXPECTED_DUMP += "        (0018,9330) FD 0.7                                      #   8, 1 XRayTubeCurrentInmA\n";
    EXPECTED_DUMP += "        (0018,9332) FD 0.3                                      #   8, 1 ExposureInmAs\n";
    EXPECTED_DUMP += "        (0018,9345) FD 0.1                                      #   8, 1 CTDIvol\n";
    EXPECTED_DUMP
        += "        (0018,9346) SQ (Sequence with explicit length #=1)      #   0, 1 CTDIPhantomTypeCodeSequence\n";
    EXPECTED_DUMP += "          (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "            (0008,0100) SH [113682]                                 #   6, 1 CodeValue\n";
    EXPECTED_DUMP
        += "            (0008,0102) SH [DCM]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "            (0008,0104) LO [ACR Accreditation Phantom - CT]         #  30, 1 CodeMeaning\n";
    EXPECTED_DUMP
        += "          (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP
        += "        (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0018,9325) SQ (Sequence with explicit length #=1)      #   0, 1 CTXRayDetailsSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=7)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0018,0060) DS [5]                                      #   2, 1 KVP\n";
    EXPECTED_DUMP += "        (0018,1160) SH [FILTER_TYPE]                            #  12, 1 FilterType\n";
    EXPECTED_DUMP += "        (0018,1190) DS [4.4\\4.4\\4.4\\4.4]                        #  16, 4 FocalSpots\n";
    EXPECTED_DUMP += "        (0018,7050) CS [FILTER_MATERIAL]                        #  16, 1 FilterMaterial\n";
    EXPECTED_DUMP
        += "        (0018,9351) FL 2                                        #   4, 1 CalciumScoringMassFactorPatient\n";
    EXPECTED_DUMP += "        (0018,9352) FL 1\\1\\1                                    #  12, 3 "
                     "CalciumScoringMassFactorDevice\n";
    EXPECTED_DUMP += "        (0018,9353) FL 3                                        #   4, 1 EnergyWeightingFactor\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0018,9326) SQ (Sequence with explicit length #=1)      #   0, 1 CTPositionSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0018,9313) FD 2\\2\\2                                    #  24, 3 DataCollectionCenterPatient\n";
    EXPECTED_DUMP += "        (0018,9318) FD 1\\1\\1                                    #  24, 3 "
                     "ReconstructionTargetCenterPatient\n";
    EXPECTED_DUMP += "        (0018,9327) FD 100                                      #   8, 1 TablePosition\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0018,9329) SQ (Sequence with explicit length #=1)      #   0, 1 CTImageFrameTypeSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=4)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0008,9007) CS [ORIGINAL\\PRIMARY\\VOLUME\\MAXIMUM]        #  32, 4 FrameType\n";
    EXPECTED_DUMP += "        (0008,9205) CS [MONOCHROME]                             #  10, 1 PixelPresentation\n";
    EXPECTED_DUMP += "        (0008,9206) CS [VOLUME]                                 #   6, 1 VolumetricProperties\n";
    EXPECTED_DUMP
        += "        (0008,9207) CS [VOLUME_RENDER]                          #  14, 1 VolumeBasedCalculationTechnique\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0018,9360) SQ (Sequence with explicit length #=1)      #   0, 1 CTAdditionalXRaySourceSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=8)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0018,0060) DS [5]                                      #   2, 1 KVP\n";
    EXPECTED_DUMP
        += "        (0018,0090) DS [1]                                      #   2, 1 DataCollectionDiameter\n";
    EXPECTED_DUMP += "        (0018,1160) SH [FILTER_TYPE]                            #  12, 1 FilterType\n";
    EXPECTED_DUMP += "        (0018,1190) DS [4.4\\4.4\\4.4\\4.4]                        #  16, 4 FocalSpots\n";
    EXPECTED_DUMP += "        (0018,7050) CS [FILTER_MATERIAL]                        #  16, 1 FilterMaterial\n";
    EXPECTED_DUMP += "        (0018,9330) FD 6                                        #   8, 1 XRayTubeCurrentInmA\n";
    EXPECTED_DUMP += "        (0018,9332) FD 3                                        #   8, 1 ExposureInmAs\n";
    EXPECTED_DUMP += "        (0018,9353) FL 2                                        #   4, 1 EnergyWeightingFactor\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0018,9477) SQ (Sequence with explicit length #=1)      #   0, 1 "
                     "IrradiationEventIdentificationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0008,3010) UI [2.25.30853892236613436472911970638347155062] #  44, 1 IrradiationEventUID\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0020,9071) SQ (Sequence with explicit length #=1)      #   0, 1 FrameAnatomySequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=2)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0008,2218) SQ (Sequence with explicit length #=1)      #   0, 1 AnatomicRegionSequence\n";
    EXPECTED_DUMP += "          (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "            (0008,0100) SH [12738006]                               #   8, 1 CodeValue\n";
    EXPECTED_DUMP
        += "            (0008,0102) SH [SCT]                                    #   4, 1 CodingSchemeDesignator\n";
    EXPECTED_DUMP += "            (0008,0104) LO [Brain]                                  #   6, 1 CodeMeaning\n";
    EXPECTED_DUMP
        += "          (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP
        += "        (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "        (0020,9072) CS [B]                                      #   2, 1 FrameLaterality\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
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
    EXPECTED_DUMP += "    (0020,9310) SQ (Sequence with explicit length #=1)      #   0, 1 TemporalPositionSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    EXPECTED_DUMP
        += "        (0020,930d) FD 1                                        #   8, 1 TemporalPositionTimeOffset\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0028,9110) SQ (Sequence with explicit length #=1)      #   0, 1 PixelMeasuresSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0018,0050) DS [1.0]                                    #   4, 1 SliceThickness\n";
    EXPECTED_DUMP += "        (0018,0088) DS [0.05]                                   #   4, 1 SpacingBetweenSlices\n";
    EXPECTED_DUMP += "        (0028,0030) DS [0.1\\0.1]                                #   8, 2 PixelSpacing\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "    (0028,9132) SQ (Sequence with explicit length #=1)      #   0, 1 FrameVOILUTSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0028,1050) DS [1000]                                   #   4, 1 WindowCenter\n";
    EXPECTED_DUMP += "        (0028,1051) DS [2000]                                   #   4, 1 WindowWidth\n";
    EXPECTED_DUMP
        += "        (0028,1055) LO [BRAIN]                                  #   6, 1 WindowCenterWidthExplanation\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "    (0028,9145) SQ (Sequence with explicit length #=1)      #   0, 1 PixelValueTransformationSequence\n";
    EXPECTED_DUMP += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    EXPECTED_DUMP += "        (0028,1052) DS [0]                                      #   2, 1 RescaleIntercept\n";
    EXPECTED_DUMP += "        (0028,1053) DS [1]                                      #   2, 1 RescaleSlope\n";
    EXPECTED_DUMP += "        (0028,1054) LO [HU]                                     #   2, 1 RescaleType\n";
    EXPECTED_DUMP += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP
        += "(5200,9230) SQ (Sequence with explicit length #=2)      #   0, 1 PerFrameFunctionalGroupsSequence\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
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
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
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
    EXPECTED_DUMP += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    EXPECTED_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    EXPECTED_DUMP += "(7fe0,0010) OW 0001\\0001\\0001\\0001\\0002\\0002\\0002\\0002  #  16, 1 PixelData\n";
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
