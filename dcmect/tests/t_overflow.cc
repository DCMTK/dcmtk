/*
 *
 *  Copyright (C) 2024, OFFIS e.V.
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
 *  Author:  Daniel Berredo / Michael Onken
 *
 *  Purpose: Tests that check for pixel data overflow conditions
 *
 */


#include <dcmtk/config/osconfig.h> /* make sure OS specific configuration is included first */

#include <dcmtk/dcmect/enhanced_ct.h>

#include <dcmtk/ofstd/oftempf.h>
#include <dcmtk/ofstd/oftest.h>

#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/dcfilefo.h>

#include <dcmtk/dcmfg/fgctacquisitiondetails.h>
#include <dcmtk/dcmfg/fgctacquisitiontype.h>
#include <dcmtk/dcmfg/fgctadditionalxraysource.h>
#include <dcmtk/dcmfg/fgctexposure.h>
#include <dcmtk/dcmfg/fgctgeometry.h>
#include <dcmtk/dcmfg/fgctimageframetype.h>
#include <dcmtk/dcmfg/fgctposition.h>
#include <dcmtk/dcmfg/fgctreconstruction.h>
#include <dcmtk/dcmfg/fgcttabledynamics.h>
#include <dcmtk/dcmfg/fgctxraydetails.h>
#include <dcmtk/dcmfg/fgfracon.h>
#include <dcmtk/dcmfg/fgframeanatomy.h>
#include <dcmtk/dcmfg/fgirradiationeventid.h>
#include <dcmtk/dcmfg/fgpixeltransform.h>
#include <dcmtk/dcmfg/fgpixmsr.h>
#include <dcmtk/dcmfg/fgplanor.h>
#include <dcmtk/dcmfg/fgplanpo.h>
#include <dcmtk/dcmfg/fgrealworldvaluemapping.h>

static const Uint16 NUM_ROWS = 1024;
static const Uint16 NUM_COLS = 1;
static const Uint16 NUM_FRAMES = 2;
static const size_t NUM_PIXELS_PER_FRAME = 1;

static OFString EXPECTED_DUMP;

static EctEnhancedCT *create();
static void configureIOD(EctEnhancedCT *ct);
static void setGenericValues(EctEnhancedCT *ct);
static void addSharedFGs(EctEnhancedCT *ct);
static void addFrames(EctEnhancedCT *ct);
static void addDimensions(EctEnhancedCT *ct);


OFTEST(dcmect_overflow)
{
    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK(dcmDataDict.isDictionaryLoaded());
    }

    // Creation
    EctEnhancedCT *ct = create();
    configureIOD(ct);
    setGenericValues(ct);
    addSharedFGs(ct);
    addFrames(ct);
    addDimensions(ct);

    // Write to dataset and compare its dump with expected result
    DcmFileFormat dcmff;

    OFTempFile tf(O_RDWR, "", "t_overflow", ".dcm");
    OFCondition result;
    result = ct->saveFile("output.dcm", EXS_LittleEndianExplicit);
    OFCHECK_MSG(result == ECT_InvalidPixelInfo, result.text());
}

static EctEnhancedCT *create()
{
    IODEnhGeneralEquipmentModule::EquipmentInfo eq("Open Connections", "OC CT", "4711", "0.1");
    EctEnhancedCT *ct = NULL;
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

static void configureIOD(EctEnhancedCT *ct)
{
    if (!ct)
        return;
}

static void setGenericValues(EctEnhancedCT *ct)
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

static void addSharedFGs(EctEnhancedCT *ct)
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
    FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem *item = new FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem();
    OFCHECK(item->setRotationDirection(FGCTAcquisitionDetails::E_RotationDirection_CW).good());
    OFCHECK(item->setRevolutionTime(5).good());
    OFCHECK(item->setSingleCollimationWidth(1).good());
    OFCHECK(item->setTotalCollimationWidth(10).good());
    OFCHECK(item->setTableHeight(50).good());
    OFCHECK(item->setGantryDetectorTilt(5).good());
    OFCHECK(item->setDataCollectionDiameter(20).good());
    adetails.getCTAcquisitionDetailsItems().push_back(item);

    FGCTTableDynamics dyn;
    FGCTTableDynamics::FGCTTableDynamicsItem *dyn_item = new FGCTTableDynamics::FGCTTableDynamicsItem;
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
    FGCTGeometry::FGCTGeometryItem *geo_item = new FGCTGeometry::FGCTGeometryItem;
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
    FGCTExposure::FGCTExposureItem *exp_item = new FGCTExposure::FGCTExposureItem;
    if (exp_item)
    {
        OFCHECK(exp_item->setCTDIVol(0.1).good());
        CodeSequenceMacro *phantom_item = new CodeSequenceMacro("113682", "DCM", "ACR Accreditation Phantom - CT");
        exp_item->getCTDIPhantomTypeCodeSequence().push_back(phantom_item);
        OFCHECK(exp_item->setExposureInMas(0.3).good());
        OFCHECK(exp_item->setExposureModulationType("WEIRD").good());
        OFCHECK(exp_item->setExposureTimeInMs(0.4).good());
        OFCHECK(exp_item->setImageAndFluoroscopyAreaDoseProduct(0.5).good());
        OFCHECK(exp_item->setWaterEquivalentDiameter(0.6).good());
        CodeSequenceMacro *water_code = new CodeSequenceMacro("113987", "DCM", "AAPM 220");
        exp_item->getWaterEquivalentDiameterCalculationMethodCodeSequence().push_back(water_code);
        OFCHECK(exp_item->setXRayTubeCurrentInMa(0.7).good());
        exp.getCTExposureItems().push_back(exp_item);
    }

    FGCTXRayDetails det;
    FGCTXRayDetails::FGCTXRayDetailsItem *det_item = new FGCTXRayDetails::FGCTXRayDetailsItem;
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
    FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem *asrc_item = new FGCTAdditionalXRaySource::FGCTAdditionalXRaySourceItem;
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

    OFCHECK(ct->addForAllFrames(meas).good());
    OFCHECK(ct->addForAllFrames(planpo).good());
    OFCHECK(ct->addForAllFrames(planor).good());
    OFCHECK(ct->addForAllFrames(ana).good());
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
}

static void addFrames(EctEnhancedCT *ct)
{
    if (!ct)
        return;

    FGFrameContent *fg = new FGFrameContent();
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
            OFVector<FGBase *> groups;
            groups.push_back(fg);

            Uint16 *data = new Uint16[NUM_PIXELS_PER_FRAME];
            for (size_t i = 0; i < NUM_PIXELS_PER_FRAME; ++i)
            {
                data[i] = 0x4141;
            }
            OFCHECK(
                OFget<EctEnhancedCT::Frames<Uint16>>(&frames)->addFrame(data, NUM_PIXELS_PER_FRAME, groups).good());
            delete[] data;
        }
    }
    delete fg;
}

static void addDimensions(EctEnhancedCT *ct)
{
    if (!ct)
        return;
    IODMultiframeDimensionModule &dims = ct->getDimensions();
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


