/*
 *
 *  Copyright (C) 2015-2019, J. Riesmeier, Oldenburg, Germany
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    test program for various CIDxxx and TIDxxx classes from the "Content Mapping Resource"
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdatset.h"

#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmsr/dsrnumvl.h"
#include "dcmtk/dcmsr/dsrnumtn.h"
#include "dcmtk/dcmsr/codes/dcm.h"
#include "dcmtk/dcmsr/codes/srt.h"
#include "dcmtk/dcmsr/codes/ucum.h"
#include "dcmtk/dcmsr/cmr/init.h"
#include "dcmtk/dcmsr/cmr/logger.h"
#include "dcmtk/dcmsr/cmr/cid29e.h"
#include "dcmtk/dcmsr/cmr/cid42.h"
#include "dcmtk/dcmsr/cmr/cid244e.h"
#include "dcmtk/dcmsr/cmr/cid4020.h"
#include "dcmtk/dcmsr/cmr/cid4031e.h"
#include "dcmtk/dcmsr/cmr/cid7181.h"
#include "dcmtk/dcmsr/cmr/cid7445.h"
#include "dcmtk/dcmsr/cmr/cid10013e.h"
#include "dcmtk/dcmsr/cmr/cid10033e.h"
#include "dcmtk/dcmsr/cmr/tid1001.h"
#include "dcmtk/dcmsr/cmr/tid1204.h"
#include "dcmtk/dcmsr/cmr/tid1500.h"
#include "dcmtk/dcmsr/cmr/tid1600.h"
#include "dcmtk/dcmsr/cmr/srnumvl.h"
#include "dcmtk/dcmsr/cmr/srnumvlu.h"


OFTEST(dcmsr_CID29e_AcquisitionModality)
{
    CID29e_AcquisitionModality ctxGroup;
    DSRCodedEntryValue codedEntry;
    OFCHECK(!ctxGroup.hasSelectedValue());
    OFCHECK(ctxGroup.mapModality("CT").isValid());
    OFCHECK_EQUAL(ctxGroup.mapModality("MR").getCodeMeaning(), "Magnetic Resonance");
    /* invalid/unknown defined terms */
    OFCHECK(ctxGroup.mapModality("XYZ").isEmpty());
    OFCHECK(ctxGroup.mapModality("ABC", codedEntry) == SR_EC_UnsupportedValue);
    OFCHECK(ctxGroup.selectValue("").bad());
}


OFTEST(dcmsr_CID42_NumericValueQualifier)
{
    DSRNumericMeasurementValue numValue;
    /* set coded entry from context group and check the value */
    OFCHECK(numValue.setNumericValueQualifier(CID42_NumericValueQualifier(CID42_NumericValueQualifier::NotANumber)).good());
    OFCHECK(numValue.getNumericValueQualifier() == CODE_DCM_NotANumber);
    /* perform some tests with invalid coded entries */
    CID42_NumericValueQualifier ctxGroup;
    OFCHECK(ctxGroup.addCodedEntry(DSRBasicCodedEntry("0815", "99TEST", "Meaning")).good());
    OFCHECK(ctxGroup.findCodedEntry(DSRBasicCodedEntry("", "99TEST", "Some invalid test code")).bad());
    OFCHECK(ctxGroup.findCodedEntry(DSRBasicCodedEntry("0815", "99TEST", "-")).good());
    OFCHECK(ctxGroup.findCodedEntry(DSRBasicCodedEntry("", "", "")).bad());
}


OFTEST(dcmsr_CID244e_Laterality)
{
    CID244e_Laterality ctxGroup;
    DSRCodedEntryValue codedEntry;
    OFCHECK(!ctxGroup.hasSelectedValue());
    OFCHECK_EQUAL(ctxGroup.mapImageLaterality("R").getCodeValue(), "G-A100");
    OFCHECK(ctxGroup.mapImageLaterality("B", codedEntry).good());
    OFCHECK(ctxGroup.selectValue("L").good());
    OFCHECK(DSRCodedEntryValue(ctxGroup).getCodeMeaning() == "Left");
    /* invalid/unknown enumerated values */
    OFCHECK(!ctxGroup.mapImageLaterality("XYZ").isValid());
    OFCHECK(ctxGroup.mapImageLaterality("ABC", codedEntry) == SR_EC_InvalidValue);
    OFCHECK(ctxGroup.selectValue("").bad());
}


OFTEST(dcmsr_CID4031e_CommonAnatomicRegions)
{
    CID4031e_CommonAnatomicRegions ctxGroup("HEART");
    DSRCodedEntryValue codedEntry = ctxGroup;
    OFCHECK(ctxGroup.hasSelectedValue());
    OFCHECK(ctxGroup.getSelectedValue() == codedEntry);
    OFCHECK_EQUAL(CMR_CID4031e::mapBodyPartExamined("ABDOMEN").getCodeMeaning(), "Abdomen");
    OFCHECK_EQUAL(CMR_CID4031e::mapBodyPartExamined("KNEE").getCodeMeaning(), "Knee");
    OFCHECK_EQUAL(CMR_CID4031e::mapBodyPartExamined("ZYGOMA").getCodeMeaning(), "Zygoma");
    /* invalid/unknown defined terms */
    OFCHECK(CMR_CID4031e::mapBodyPartExamined("XYZ").isEmpty());
    OFCHECK(CMR_CID4031e::mapBodyPartExamined("").isEmpty());
    OFCHECK(ctxGroup.selectValue("XYZ").bad());
}


OFTEST(dcmsr_CID7445_DeviceParticipatingRoles)
{
    CID7445_DeviceParticipatingRoles ctxGroup1;
    OFCHECK(!ctxGroup1.hasSelectedValue());
    OFCHECK(!ctxGroup1.hasCodedEntry(DSRBasicCodedEntry("0815", "99TEST", "Some test code")));
    OFCHECK(!ctxGroup1.hasExtendedCodedEntries());
    /* add an extended code to the context group */
    OFCHECK(ctxGroup1.addCodedEntry(DSRBasicCodedEntry("0815", "99TEST", "Some test code")).good());
    OFCHECK(ctxGroup1.hasCodedEntry(DSRBasicCodedEntry("0815", "99TEST", "Some test code")));
    OFCHECK(ctxGroup1.findCodedEntry(CODE_DCM_Recording) == SR_EC_CodedEntryInStandardContextGroup);
    OFCHECK(ctxGroup1.findCodedEntry(DSRBasicCodedEntry("0815", "99TEST", "Some test code")) == SR_EC_CodedEntryIsExtensionOfContextGroup);
    OFCHECK(ctxGroup1.hasExtendedCodedEntries());
    /* try again with a non-extensible context group */
    CID7445_DeviceParticipatingRoles ctxGroup2(DSRBasicCodedEntry("4711", "99TEST", "Some other test code"));
    ctxGroup2.setExtensible(OFFalse);
    OFCHECK(ctxGroup2.hasSelectedValue());
    OFCHECK(ctxGroup2.addCodedEntry(DSRBasicCodedEntry("4711", "99TEST", "Some other test code")).bad());
    OFCHECK(!ctxGroup2.hasExtendedCodedEntries());
    /* check whether the currently selected code is valid */
    OFCHECK(ctxGroup2.checkSelectedValue(OFFalse /*definedContextGroup*/).good());
    OFCHECK(ctxGroup2.checkSelectedValue(OFTrue /*definedContextGroup*/).bad());
    /* select another value (by two different ways) */
    OFCHECK(ctxGroup2.selectValue(CODE_DCM_XRayReadingDevice).good());
    OFCHECK(ctxGroup2.selectValue(CMR_CID7445::IrradiatingDevice).good());
    /* and finally, check the static get() function */
    DSRCodedEntryValue codeValue = CMR_CID7445::getCodedEntry(CMR_CID7445::Recording, OFTrue /*enhancedEncodingMode*/);
    OFCHECK(codeValue == CODE_DCM_Recording);
    OFCHECK_EQUAL(codeValue.getContextIdentifier(), "7445");
    OFCHECK_EQUAL(codeValue.getMappingResource(), "DCMR");
}


OFTEST(dcmsr_CID10013e_CTAcquisitionType)
{
    OFCHECK(CMR_CID10013e::mapAcquisitionType("SEQUENCED") == CODE_DCM_SequencedAcquisition);
    OFCHECK(CMR_CID10013e::mapAcquisitionType("CONSTANT_ANGLE") == CODE_DCM_ConstantAngleAcquisition);
    OFCHECK(CMR_CID10013e::mapAcquisitionType("FREE") == CODE_DCM_FreeAcquisition);
    /* invalid/unknown defined terms */
    OFCHECK(CMR_CID10013e::mapAcquisitionType("XYZ").isEmpty());
}


OFTEST(dcmsr_CID10033e_CTReconstructionAlgorithm)
{
    OFCHECK(CMR_CID10033e::mapReconstructionAlgorithm("FILTER_BACK_PROJ") == CODE_DCM_FilteredBackProjection);
    OFCHECK(CMR_CID10033e::mapReconstructionAlgorithm("ITERATIVE") == CODE_DCM_IterativeReconstruction);
    /* invalid/unknown defined terms */
    OFCHECK(CMR_CID10033e::mapReconstructionAlgorithm("XYZ").isEmpty());
}


OFTEST(dcmsr_TID1001_ObservationContext)
{
    TID1001_ObservationContext obsContext;
    OFList<CID7445_DeviceParticipatingRoles> procRoles;
    procRoles.push_back(CMR_CID7445::IrradiatingDevice);
    procRoles.push_back(CMR_CID7445::Recording);
    /* check template identification */
    OFCHECK(obsContext.compareTemplateIdentication("1001", "DCMR"));
    /* empty template is not valid */
    OFCHECK(!obsContext.isValid());
    /* add person and device observers */
    OFCHECK(obsContext.addDeviceObserver("1.2.4.5.6.7.8.9.0").good());
    OFCHECK(obsContext.addPersonObserver("Doe^John", "", CMR_CID7452::Physician, CMR_CID7453::Referring).good());
    OFCHECK(obsContext.addDeviceObserver("007", "Some Name", "Some Manufacturer", "Some Model Name", "", "", procRoles, "AETITLE", OFFalse /*check*/).good());
    OFCHECK(obsContext.addPersonObserver("Doe^Jane", "Some Organization").good());
    /* check some additions that should fail */
    OFCHECK(obsContext.addPersonObserver("").bad());
    OFCHECK(obsContext.addDeviceObserver("").bad());
    OFCHECK(obsContext.addDeviceObserver("01").bad());
    /* check whether all nodes are present */
    OFCHECK(obsContext.isValid());
    OFCHECK_EQUAL(obsContext.countNodes(), 17);
    /* check whether annotations are as expected */
    OFCHECK(obsContext.gotoAnnotatedNode("TID 1004 - Row 1") > 0);
    OFCHECK(obsContext.gotoNextAnnotatedNode("TID 1004 - Row 1") > 0);
    OFCHECK_EQUAL(obsContext.getCurrentContentItem().getStringValue(), "007");
    /* now, clone the document tree of the template and check annotations again */
    DSRDocumentSubTree *tree = obsContext.cloneTree();
    if (tree != NULL)
    {
        OFCHECK(tree->gotoAnnotatedNode("TID 1004 - Row 1") > 0);
        OFCHECK(tree->gotoNextAnnotatedNode("TID 1004 - Row 1") > 0);
    } else
        OFCHECK_FAIL("could not create clone of subtree");
    delete tree;
}


OFTEST(dcmsr_TID1204_LanguageOfContentItemAndDescendants)
{
    TID1204_LanguageOfContentItemAndDescendants lang;
    /* check template identification */
    OFCHECK(lang.compareTemplateIdentication("1204", "DCMR"));
    /* empty template is not valid */
    OFCHECK(!lang.isValid());
    /* add language */
    OFCHECK(lang.setLanguage(CID5000_Languages::German_DE).good());
    OFCHECK(lang.isValid());
    OFCHECK_EQUAL(lang.countNodes(), 1);
    /* add language and separate country */
    OFCHECK(lang.setLanguage(CID5000_Languages::German, CID5001_Countries::Germany).good());
    OFCHECK(lang.isValid());
    OFCHECK_EQUAL(lang.countNodes(), 2);
}


OFTEST(dcmsr_TID1411_VolumetricROIMeasurements)
{
    CMR_TID1411_in_TID1500 volumetric;
    /* create a new volumetric measurements object and set the mandatory values */
    OFCHECK(volumetric.setTrackingIdentifier("tracking").good());
    OFCHECK(volumetric.setTrackingUniqueIdentifier("1.2.3.4.5").good());
    OFCHECK(volumetric.setTimePoint("1").good());
    OFCHECK(volumetric.hasMeasurementGroup());
    OFCHECK(volumetric.hasTrackingIdentifier());
    OFCHECK(volumetric.hasTrackingUniqueIdentifier());
    /* the measurement is still missing */
    OFCHECK(!volumetric.hasMeasurements());
    OFCHECK(!volumetric.isValid());
    /* do more sophisticated tests */
    CMR_TID1419_in_TID1411_in_TID1500 &measurement = volumetric.getMeasurement();
    OFCHECK(volumetric.addFindingSite(DSRBasicCodedEntry("EFGH", "99TEST", "Finding Site"), CMR_CID244::Bilateral).good());
    OFCHECK(!volumetric.hasMeasurements());
    OFCHECK(!measurement.hasMeasurement());
    OFCHECK(measurement.createNewMeasurement(CMR_CID7469::Volume, CMR_TID1419_in_TID1411_in_TID1500::MeasurementValue("15", CMR_CID7181::CubicMillimeter)).good());
    OFCHECK(volumetric.hasMeasurements());
    OFCHECK(measurement.hasMeasurement());
    OFCHECK(measurement.setDerivation(CMR_CID7464::StandardDeviation).good());
    OFCHECK(measurement.addModifier(DSRBasicCodedEntry("ABCD", "99TEST", "Concept Name Modifier"), DSRBasicCodedEntry("ABCD.1", "99TEST", "Modifier 1")).good());
    OFCHECK(measurement.addFindingSite(DSRBasicCodedEntry("EFGH.1", "99TEST", "Finding Site 1"), CMR_CID244::Left).good());
    OFCHECK(measurement.addModifier(DSRBasicCodedEntry("ABCD", "99TEST", "Concept Name Modifier"), DSRBasicCodedEntry("ABCD.2", "99TEST", "Modifier 2")).good());
    OFCHECK(measurement.setMeasurementMethod(DSRCodedEntryValue("9876", "99TEST", "Some method")).good());
    OFCHECK(measurement.setAlgorithmIdentification("My Special Effects", "1.0").good());
    OFCHECK(measurement.setRealWorldValueMap(DSRCompositeReferenceValue(UID_RealWorldValueMappingStorage, "2.0.3.0.4.0")).good());
    OFCHECK(measurement.addFindingSite(DSRBasicCodedEntry("EFGH.2", "99TEST", "Finding Site 2"), CID244e_Laterality(), DSRBasicCodedEntry("EFGH.2-1", "99TEST", "Finding Site 2 Modifier")).good());
    OFCHECK(measurement.setEquivalentMeaningOfConceptName("blabla").good());
    OFCHECK(measurement.addDerivationParameter(CODE_DCM_Derivation, CMR_SRNumericMeasurementValue("1.5", CODE_UCUM_Centimeter)).good());
    OFCHECK(volumetric.addQualitativeEvaluation(CODE_DCM_Conclusion, "it's not ok").good());
    OFCHECK(measurement.setAlgorithmIdentification("My New Special Effects", "2.0").good());
    OFCHECK(measurement.getAlgorithmIdentification().addParameter("Parameter #1").good());
    OFCHECK(measurement.getAlgorithmIdentification().addParameter("Parameter #2").good());
    OFCHECK(measurement.isValid());

    /* check number of content items (expected) */
    OFCHECK_EQUAL(volumetric.getTree().countNodes(OFTrue /*searchIntoSubTemplates*/), 25);
    OFCHECK_EQUAL(volumetric.getTree().countNodes(OFTrue /*searchIntoSubTemplates*/, OFFalse), 23);

    /* output content of the tree (in debug mode only) */
    if (DCM_dcmsrCmrLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        volumetric.print(COUT, DSRTypes::PF_printTemplateIdentification | DSRTypes::PF_printAllCodes | DSRTypes::PF_printSOPInstanceUID |
                               DSRTypes::PF_printNodeID | DSRTypes::PF_printAnnotation | DSRTypes::PF_printLongSOPClassName);
    }
}


OFTEST(dcmsr_TID1500_MeasurementReport)
{
    TID1500_MeasurementReport report(CMR_CID7021::ImagingMeasurementReport);
    DSRCodedEntryValue title;
    /* check initial settings */
    OFCHECK(!report.isValid());
    OFCHECK(report.getDocumentTitle(title).good());
    OFCHECK(title == CODE_DCM_ImagingMeasurementReport);
    OFCHECK(report.compareTemplateIdentication("1500", "DCMR"));
    /* create a new report */
    OFCHECK(report.createNewMeasurementReport(CMR_CID7021::PETMeasurementReport).good());
    OFCHECK(report.getDocumentTitle(title).good());
    OFCHECK(title == CODE_DCM_PETMeasurementReport);
    /* set the language */
    OFCHECK(report.setLanguage(CID5000_Languages::English).good());
    /* set details on the observation context */
    OFCHECK(report.getObservationContext().addPersonObserver("Doe^Jane", "Some Organization").good());
    /* create new image library (only needed after clear) */
    OFCHECK(report.getImageLibrary().createNewImageLibrary().good());
    /* set two values for "procedure reported" */
    OFCHECK(!report.isValid());
    OFCHECK(!report.hasProcedureReported());
    OFCHECK(report.addProcedureReported(CMR_CID100::PETWholeBody).good());
    OFCHECK(report.addProcedureReported(DSRCodedEntryValue("4711", "99TEST", "Some other test code")).good());
    OFCHECK(report.hasProcedureReported());
    OFCHECK(report.isValid());
    /* some further checks */
    OFCHECK(report.hasImagingMeasurements());
    OFCHECK(report.hasVolumetricROIMeasurements());
    OFCHECK(report.hasIndividualMeasurements());
    OFCHECK(!report.hasQualitativeEvaluations());
    OFCHECK(!report.hasImagingMeasurements(OFTrue /*checkChildren*/));
    OFCHECK(!report.hasVolumetricROIMeasurements(OFTrue /*checkChildren*/));
    OFCHECK(!report.hasIndividualMeasurements(OFTrue /*checkChildren*/));
    OFCHECK(!report.hasQualitativeEvaluations(OFTrue /*checkChildren*/));
    /* add two further volumetric ROI measurements */
    OFCHECK(report.addVolumetricROIMeasurements().good());
    OFCHECK(report.addVolumetricROIMeasurements(OFFalse /*checkEmpty*/).good());
    OFCHECK(!report.hasVolumetricROIMeasurements(OFTrue /*checkChildren*/));
    OFCHECK(report.getVolumetricROIMeasurements().setTrackingUniqueIdentifier("1.2.3.4.5").good());
    OFCHECK(report.hasVolumetricROIMeasurements(OFTrue /*checkChildren*/));
    OFCHECK(report.addVolumetricROIMeasurements().good());
    /* fill volumetric ROI measurements with data */
    TID1500_MeasurementReport::TID1411_Measurements &volMeasurements = report.getVolumetricROIMeasurements();
    OFCHECK(!volMeasurements.isValid());
    OFCHECK(volMeasurements.compareTemplateIdentication("1411", "DCMR"));
    OFCHECK(volMeasurements.setTrackingIdentifier("aorta reference region").good());
    OFCHECK(volMeasurements.setTrackingUniqueIdentifier("1.2.3.4.5").good());
    OFCHECK(volMeasurements.setTrackingIdentifier("some reference region").good());
    OFCHECK(volMeasurements.setActivitySession("1").good());
    OFCHECK(volMeasurements.setTimePoint("1.1").good());
    OFCHECK(volMeasurements.setSourceSeriesForSegmentation("6.7.8.9.0").good());
    OFCHECK(volMeasurements.setFinding(DSRBasicCodedEntry("0815", "99TEST", "Some test code")).good());
    OFCHECK(!volMeasurements.isValid());
    /* test two ways of adding a referenced segment */
    DSRImageReferenceValue segment(UID_SegmentationStorage, "1.0.2.0.3.0");
    segment.getSegmentList().addItem(1);
    DcmDataset dataset;
    DcmItem *ditem = NULL;
    OFCHECK(dataset.putAndInsertString(DCM_SOPClassUID, UID_SurfaceSegmentationStorage).good());
    OFCHECK(dataset.putAndInsertString(DCM_SOPInstanceUID, "99.0").good());
    OFCHECK(dataset.findOrCreateSequenceItem(DCM_SegmentSequence, ditem).good());
    if (ditem != NULL)
    {
        OFCHECK(ditem->putAndInsertUint16(DCM_SegmentNumber, 1).good());
        OFCHECK(ditem->putAndInsertString(DCM_TrackingID, "blabla").good());
        OFCHECK(ditem->putAndInsertString(DCM_TrackingUID, "1.2.3").good());
    }
    OFCHECK(volMeasurements.setReferencedSegment(segment).good());
    OFCHECK(volMeasurements.setReferencedSegment(DSRImageReferenceValue(UID_SegmentationStorage, "1.0")).bad());
    OFCHECK(volMeasurements.setReferencedSegment(dataset, 1).good());
    dataset.clear();
    OFCHECK(dataset.putAndInsertString(DCM_SOPClassUID, UID_RealWorldValueMappingStorage).good());
    OFCHECK(dataset.putAndInsertString(DCM_SOPInstanceUID, "99.9").good());
    OFCHECK(volMeasurements.setRealWorldValueMap(DSRCompositeReferenceValue(UID_RealWorldValueMappingStorage, "2.0.3.0.4.0")).good());
    OFCHECK(volMeasurements.setRealWorldValueMap(DSRCompositeReferenceValue(UID_CTImageStorage, "2.0")).bad());
    OFCHECK(volMeasurements.setRealWorldValueMap(dataset).good());
    OFCHECK(volMeasurements.addFindingSite(CODE_SRT_AorticArch).good());
    OFCHECK(volMeasurements.setMeasurementMethod(DSRCodedEntryValue(CODE_DCM_SUVBodyWeightCalculationMethod)).good());
    OFCHECK(!volMeasurements.isValid());
    /* add two measurement values */
    const CMR_TID1411_in_TID1500::MeasurementValue numVal1("99", CMR_CID7181::StandardizedUptakeValueBodyWeight);
    const CMR_TID1411_in_TID1500::MeasurementValue numVal2(CMR_CID42::MeasurementFailure);
    OFCHECK(!volMeasurements.hasMeasurements());
    OFCHECK(volMeasurements.addMeasurement(CMR_CID7469::SUVbw, numVal1).good());
    OFCHECK(volMeasurements.getMeasurement().setDerivation(CMR_CID7464::Mean).good());
    OFCHECK(volMeasurements.getMeasurement().setAlgorithmIdentification("My Algorithm", "0.9").good());
    OFCHECK(volMeasurements.addMeasurement(CMR_CID7469::SUVbw, numVal2).good());
    OFCHECK(volMeasurements.getMeasurement().setDerivation(CMR_CID7464::Mode).good());
    OFCHECK(volMeasurements.getMeasurement().setMeasurementMethod(DSRCodedEntryValue("0815", "99TEST", "Some test code")).good());
    OFCHECK(volMeasurements.hasMeasurements());
    OFCHECK(volMeasurements.isValid());
    /* also add an individual measurement */
    TID1500_MeasurementReport::TID1501_Measurements &linMeasurements = report.getIndividualMeasurements();
    OFCHECK(!linMeasurements.isValid());
    OFCHECK(linMeasurements.compareTemplateIdentication("1501", "DCMR"));
    OFCHECK(linMeasurements.setTrackingIdentifier("aorta reference region").good());
    OFCHECK(linMeasurements.setTrackingUniqueIdentifier("1.2.3.4.5").good());
    const CMR_TID1501_in_TID1500::MeasurementValue numVal3("11", CMR_CID7181::Millimeter);
    OFCHECK(linMeasurements.addMeasurement(CMR_CID7469::Distance, numVal3).good());
    OFCHECK(linMeasurements.addMeasurement(CMR_CID7469::Diameter, numVal3).good());

    /* now, add some qualitative evaluations */
    const DSRCodedEntryValue code("1234", "99TEST", "not bad");
    OFCHECK(report.addQualitativeEvaluation(DSRBasicCodedEntry("0815", "99TEST", "Some test code"), code).good());
    OFCHECK(report.addQualitativeEvaluation(DSRBasicCodedEntry("4711", "99TEST", "Some other test code"), "very good").good());
    /* and, add another finding site (introduced with CP-1591) */
    OFCHECK(volMeasurements.addFindingSite(DSRCodedEntryValue("0815", "99TEST", "Some test code")).good());
    /* some final checks */
    OFCHECK(report.isValid());
    OFCHECK(report.hasImagingMeasurements(OFTrue /*checkChildren*/));
    OFCHECK(report.hasVolumetricROIMeasurements(OFTrue /*checkChildren*/));
    OFCHECK(report.hasIndividualMeasurements(OFTrue /*checkChildren*/));
    OFCHECK(report.hasQualitativeEvaluations(OFTrue /*checkChildren*/));

    /* check number of content items (expected) */
    OFCHECK_EQUAL(report.getTree().countNodes(), 14);
    OFCHECK_EQUAL(report.getTree().countNodes(OFTrue /*searchIntoSubTemplates*/), 52);
    OFCHECK_EQUAL(report.getTree().countNodes(OFTrue /*searchIntoSubTemplates*/, OFFalse /*countIncludedTemplateNodes*/), 38);
    /* create an expanded version of the tree */
    DSRDocumentSubTree *tree = NULL;
    OFCHECK(report.getTree().createExpandedSubTree(tree).good());
    /* and check whether all content items are there */
    if (tree != NULL)
    {
        OFCHECK_EQUAL(tree->countNodes(), 38);
        OFCHECK_EQUAL(tree->countNodes(OFTrue /*searchIntoSubTemplates*/), 38);
        OFCHECK_EQUAL(tree->countNodes(OFTrue /*searchIntoSubTemplates*/, OFFalse /*countIncludedTemplateNodes*/), 38);
        delete tree;
    } else
        OFCHECK_FAIL("could create expanded tree");

    /* try to insert the root template into a document */
    DSRDocument doc;
    OFCHECK(!doc.isValid());
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_BasicTextSR);
    OFCHECK(doc.setTreeFromRootTemplate(report, OFFalse /*expandTree*/).good());
    OFCHECK(doc.isValid());
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_EnhancedSR);
    /* now, do the same with an expanded document tree */
    OFCHECK(doc.setTreeFromRootTemplate(report, OFTrue  /*expandTree*/).good());
    OFCHECK(doc.isValid());
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_EnhancedSR);

    /* output content of the tree (in debug mode only) */
    if (DCM_dcmsrCmrLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        report.print(COUT, DSRTypes::PF_printTemplateIdentification | DSRTypes::PF_printAllCodes | DSRTypes::PF_printSOPInstanceUID |
                           DSRTypes::PF_printNodeID | DSRTypes::PF_printAnnotation | DSRTypes::PF_printLongSOPClassName);
    }
}


OFTEST(dcmsr_TID1501_MeasurementGroup)
{
    CMR_TID1501_in_TID1500 group;
    /* create a new measurement group and set the mandatory values */
    OFCHECK(group.setTrackingIdentifier("tracking").good());
    OFCHECK(group.setTrackingUniqueIdentifier("1.2.3.4.5").good());
    OFCHECK(group.setTimePoint("1").good());
    OFCHECK(group.hasMeasurementGroup());
    OFCHECK(group.hasTrackingIdentifier());
    OFCHECK(group.hasTrackingUniqueIdentifier());
    /* the measurement is still missing */
    OFCHECK(!group.hasMeasurements());
    OFCHECK(!group.isValid());
    /* do more sophisticated tests, e.g. on TID 300 */
    CMR_TID300_in_TID1501_in_TID1500 &measurement = group.getMeasurement();
    OFCHECK(!group.hasMeasurements());
    OFCHECK(!measurement.hasMeasurement());
    OFCHECK(measurement.createNewMeasurement(CMR_CID7469::Distance, CMR_TID1501_in_TID1500::MeasurementValue("5", CMR_CID7181::Centimeter)).good());
    OFCHECK(group.hasMeasurements());
    OFCHECK(measurement.hasMeasurement());
    OFCHECK(group.isValid());
    OFCHECK(measurement.setDerivation(CMR_CID7464::StandardDeviation).good());
    OFCHECK(measurement.addModifier(DSRBasicCodedEntry("ABCD", "99TEST", "Concept Name Modifier"), DSRBasicCodedEntry("ABCD.1", "99TEST", "Modifier 1")).good());
    OFCHECK(measurement.addFindingSite(DSRBasicCodedEntry("EFGH.1", "99TEST", "Finding Site 1"), CMR_CID244::Left).good());
    OFCHECK(measurement.addModifier(DSRBasicCodedEntry("ABCD", "99TEST", "Concept Name Modifier"), DSRBasicCodedEntry("ABCD.2", "99TEST", "Modifier 2")).good());
    OFCHECK(measurement.setMeasurementMethod(DSRCodedEntryValue("9876", "99TEST", "Some method")).good());
    OFCHECK(measurement.setRealWorldValueMap(DSRCompositeReferenceValue(UID_RealWorldValueMappingStorage, "2.0.3.0.4.0")).good());
    OFCHECK(measurement.addFindingSite(DSRBasicCodedEntry("EFGH.2", "99TEST", "Finding Site 2"), CID244e_Laterality(), DSRBasicCodedEntry("EFGH.2-1", "99TEST", "Finding Site 2 Modifier")).good());
    OFCHECK(measurement.setEquivalentMeaningOfConceptName("blabla").good());
    OFCHECK(measurement.addDerivationParameter(CODE_DCM_Derivation, CMR_SRNumericMeasurementValue("1.5", CODE_UCUM_Centimeter)).good());
    OFCHECK(measurement.addImage(CODE_DCM_SourceImageForImageProcessingOperation, DSRImageReferenceValue(UID_CTImageStorage, "1.2.3.4")).good());
    DSRSpatialCoordinatesValue spatialCoord(DSRTypes::GT_Point);
    spatialCoord.getGraphicDataList().addItem(100, 200);
    OFCHECK(measurement.addSpatialCoordinates(CODE_DCM_SourceImageForImageProcessingOperation, spatialCoord, DSRImageReferenceValue(UID_CTImageStorage, "1.2.3.4")).good());
    OFCHECK(group.addQualitativeEvaluation(CODE_DCM_Conclusion, "it's ok").good());
    OFCHECK_EQUAL(group.countNodes(OFTrue /*searchIntoSubTemplates*/), 21);
    /* add extra content items (TID 1501 is extensible) */
    OFCHECK(group.addExtraContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container).good());
    OFCHECK(group.addExtraContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent).good());
    OFCHECK(group.getCurrentContentItem().setConceptName(CODE_DCM_Comment).good());
    OFCHECK(group.getCurrentContentItem().setStringValue("some extra stuff").good());
    OFCHECK(group.gotoAnnotatedNode("TID 1501 - Row 3") > 0);
    OFCHECK(group.addExtraContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Code).good());
    OFCHECK(group.getCurrentContentItem().setConceptName(CODE_DCM_FindingObservationType).good());
    OFCHECK(group.getCurrentContentItem().setCodeValue(DSRBasicCodedEntry("0815", "99TEST", "Some test code")).good());
    OFCHECK_EQUAL(group.countNodes(OFTrue /*searchIntoSubTemplates*/), 24);
    /* and, finally, change an existing template value and add a measurement */
    OFCHECK(group.setTrackingIdentifier("tracking #2").good());
    OFCHECK(group.addMeasurement(CMR_CID7469::Diameter, CMR_TID1501_in_TID1500::MeasurementValue("10", CMR_CID7181::Millimeter)).good());
    OFCHECK_EQUAL(group.countNodes(OFTrue /*searchIntoSubTemplates*/), 26);

    /* output content of the tree (in debug mode only) */
    if (DCM_dcmsrCmrLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        group.print(COUT, DSRTypes::PF_printTemplateIdentification | DSRTypes::PF_printAllCodes | DSRTypes::PF_printSOPInstanceUID |
                          DSRTypes::PF_printNodeID | DSRTypes::PF_printAnnotation | DSRTypes::PF_printLongSOPClassName);
    }
}


OFTEST(dcmsr_TID1600_ImageLibrary)
{
    TID1600_ImageLibrary library;
    DcmItem *item1, *item2;
    /* check template identification */
    OFCHECK(library.compareTemplateIdentication("1600", "DCMR"));
    /* create four image datasets */
    DcmDataset dataset1;
    OFCHECK(dataset1.putAndInsertString(DCM_SOPClassUID, UID_CTImageStorage).good());
    OFCHECK(dataset1.putAndInsertString(DCM_SOPInstanceUID, "1.2.3.4.5.6.7.8.9.0").good());
    OFCHECK(dataset1.putAndInsertString(DCM_Modality, "CT").good());
    OFCHECK(dataset1.putAndInsertString(DCM_StudyDate, "20150818").good());
    OFCHECK(dataset1.putAndInsertUint16(DCM_Columns, 512).good());
    OFCHECK(dataset1.putAndInsertUint16(DCM_Rows, 512).good());
    OFCHECK(dataset1.putAndInsertString(DCM_PixelSpacing, "0.5\\1.5").good());
    OFCHECK(dataset1.putAndInsertString(DCM_BodyPartExamined, "HEAD").good());
    OFCHECK(dataset1.putAndInsertString(DCM_ImageOrientationPatient, "1.000000\\0.000000\\0.000000\\0.000000\\1.000000\\0.000000").good());
    OFCHECK(dataset1.putAndInsertString(DCM_ImagePositionPatient, "-120.000000\\-120.000000\\-545.000000").good());
    OFCHECK(dataset1.findOrCreateSequenceItem(DCM_SharedFunctionalGroupsSequence, item1, -2).good());
    if (item1 != NULL)
    {
        OFCHECK(item1->findOrCreateSequenceItem(DCM_CTAcquisitionTypeSequence, item2, -2).good());
        if (item2 != NULL)
            OFCHECK(item2->putAndInsertString(DCM_AcquisitionType, "SPIRAL").good());
        OFCHECK(item1->findOrCreateSequenceItem(DCM_CTReconstructionSequence, item2, -2).good());
        if (item2 != NULL)
            OFCHECK(item2->putAndInsertString(DCM_ReconstructionAlgorithm, "ITERATIVE").good());
    }
    DcmDataset dataset2;
    OFCHECK(dataset2.putAndInsertString(DCM_SOPClassUID, UID_PositronEmissionTomographyImageStorage).good());
    OFCHECK(dataset2.putAndInsertString(DCM_SOPInstanceUID, "1.2.3.4.5.6.7.8.9.1").good());
    OFCHECK(dataset2.putAndInsertString(DCM_Modality, "PT").good());
    OFCHECK(dataset2.putAndInsertString(DCM_ImageLaterality, "B").good());
    OFCHECK(DSRCodedEntryValue("T-32000", "SRT", "Heart").writeSequence(dataset2, DCM_AnatomicRegionSequence).good());
    OFCHECK(dataset2.findOrCreateSequenceItem(DCM_RadiopharmaceuticalInformationSequence, item1, -2).good());
    if (item1 != NULL)
    {
        OFCHECK(DSRCodedEntryValue("C-128A2", "SRT", "^68^Germanium").writeSequence(*item1, DCM_RadionuclideCodeSequence).good());
        OFCHECK(item1->putAndInsertString(DCM_RadiopharmaceuticalVolume, "50.5").good());
    }
    DcmDataset dataset3;
    OFCHECK(dataset3.putAndInsertString(DCM_SOPClassUID, UID_SecondaryCaptureImageStorage).good());
    OFCHECK(dataset3.putAndInsertString(DCM_SOPInstanceUID, "1.2.3.4.5.6.7.8.9.2").good());
    OFCHECK(dataset3.putAndInsertString(DCM_Modality, "OT").good());
    OFCHECK(dataset3.putAndInsertString(DCM_BodyPartExamined, "EAR").good());
    OFCHECK(dataset3.putAndInsertString(DCM_FrameOfReferenceUID, "1.2.3.4.5.6.7.8.9.3").good());
    DcmDataset dataset4;
    OFCHECK(dataset4.putAndInsertString(DCM_SOPClassUID, UID_DigitalXRayImageStorageForPresentation).good());
    OFCHECK(dataset4.putAndInsertString(DCM_SOPInstanceUID, "1.2.3.4.5.6.7.8.9.4").good());
    OFCHECK(dataset4.putAndInsertString(DCM_Modality, "DX").good());
    OFCHECK(dataset4.putAndInsertString(DCM_PatientOrientation, "A\\F").good());
    OFCHECK(dataset4.putAndInsertString(DCM_PositionerPrimaryAngle, "45").good());
    OFCHECK(dataset4.putAndInsertString(DCM_PositionerSecondaryAngle, "-45").good());
    OFCHECK(DSRCodedEntryValue("R-10202", "SRT", "frontal").writeSequence(dataset4, DCM_ViewCodeSequence).good());
    OFCHECK(dataset4.findAndGetSequenceItem(DCM_ViewCodeSequence, item1).good());
    if (item1 != NULL)
    {
        OFCHECK(item1->findOrCreateSequenceItem(DCM_ViewModifierCodeSequence, item2, -2).good());
        if (item2 != NULL)
            OFCHECK(DSRCodedEntryValue("4711", "99TEST", "some strange modifier").writeSequenceItem(*item2, DCM_ViewModifierCodeSequence).good());
        OFCHECK(item1->findOrCreateSequenceItem(DCM_ViewModifierCodeSequence, item2, -2).good());
        if (item2 != NULL)
            OFCHECK(DSRCodedEntryValue("4711b", "99TEST", "some even more strange modifier").writeSequenceItem(*item2, DCM_ViewModifierCodeSequence).good());
    }
    /* add two image groups */
    TID1600_ImageLibrary::ConceptNameList descriptors;
    descriptors.push_back(CODE_DCM_Modality);
    descriptors.push_back(CODE_DCM_TargetRegion);
    OFCHECK(library.isValid());
    OFCHECK(library.hasImageLibrary());
    OFCHECK(library.addImageGroup().good());
    OFCHECK(library.addImageEntry(dataset1, TID1600_ImageLibrary::withoutSelectedDescriptors, descriptors).good());
    OFCHECK(library.addImageGroupDescriptors(dataset1, TID1600_ImageLibrary::withSelectedDescriptors, descriptors).good());
    OFCHECK(library.addImageGroup().good());
    OFCHECK(library.addImageEntry(dataset2, TID1600_ImageLibrary::withAllDescriptors).good());
    OFCHECK(library.setPETImageRadionuclide(CID4020_PETRadionuclide::_18_Fluorine).good());
    OFCHECK(library.setPETImageRadiopharmaceuticalAgent(CID4021_PETRadiopharmaceutical::Fluorodeoxyglucose_F18).good());
    OFCHECK(library.setPETImageRadiopharmaceuticalStartDateTime("20151212").good());
    OFCHECK(library.setPETImageRadiopharmaceuticalVolume(DSRNumericMeasurementValue("99.9", CODE_UCUM_cm3)).good());
    OFCHECK(library.setPETImageRadionuclideTotalDose(DSRNumericMeasurementValue("9999", CODE_UCUM_Bq)).good());
    OFCHECK(library.addImageEntry(dataset1, TID1600_ImageLibrary::withAllDescriptors).good());
    OFCHECK(library.setPETImageRadionuclide(CID4020_PETRadionuclide::_18_Fluorine).bad());
    OFCHECK(library.addImageEntry(dataset3, TID1600_ImageLibrary::withoutDescriptors).good());
    OFCHECK(library.addImageEntry(dataset4, TID1600_ImageLibrary::withAllDescriptors).good());
    /* the following two calls should fail */
    OFCHECK(library.addImageGroupDescriptors(dataset3, TID1600_ImageLibrary::withoutDescriptors).bad());
    OFCHECK(library.addImageGroupDescriptors(dataset3, TID1600_ImageLibrary::withSelectedDescriptors, TID1600_ImageLibrary::ConceptNameList()).bad());
    OFCHECK(library.addImageGroupDescriptors(dataset3, TID1600_ImageLibrary::withAllDescriptors).good());
    /* check modality code of most recently added entry */
    DSRCodedEntryValue modality;
    OFCHECK(library.getImageEntryModality(modality).good());
    OFCHECK(modality == CODE_DCM_DigitalRadiography);
    /* try to add another invocation of TID 1602 */
    OFCHECK(library.addImageGroupDescriptors(dataset4, TID1600_ImageLibrary::withAllDescriptors).bad());
    /* create another group for testing the "move common descriptors" method */
    OFCHECK(library.addImageGroup().good());
    OFCHECK(library.addImageEntry(dataset1, TID1600_ImageLibrary::withAllDescriptors).good());
    OFCHECK(dataset1.putAndInsertString(DCM_SOPInstanceUID, "1.2.3.4.5.6.7.8.9.10").good());
    OFCHECK(dataset1.putAndInsertString(DCM_PixelSpacing, "1.0\\1.0").good());
    OFCHECK(library.addImageEntry(dataset1, TID1600_ImageLibrary::withAllDescriptors).good());

    /* check number of expected content items */
    OFCHECK_EQUAL(library.countNodes(), 100);

    /* move common descriptors to group level */
    OFCHECK(library.moveCommonImageDescriptorsToImageGroups() == EC_Normal);
    OFCHECK(library.moveCommonImageDescriptorsToImageGroups() == CMR_EC_NoImageLibraryEntryDescriptorsToBeMoved);

    /* check number of expected content items (again) */
    OFCHECK_EQUAL(library.countNodes(), 84);

    /* output content of the tree (in debug mode only) */
    if (DCM_dcmsrCmrLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        library.print(COUT, DSRTypes::PF_printTemplateIdentification | DSRTypes::PF_printAllCodes | DSRTypes::PF_printSOPInstanceUID |
                            DSRTypes::PF_printNodeID | DSRTypes::PF_indicateEnhancedEncodingMode | DSRTypes::PF_printAnnotation |
                            DSRTypes::PF_printLongSOPClassName);
    }

    /* empty template is not valid */
    library.clear();
    OFCHECK(!library.isValid());
    OFCHECK(!library.hasImageLibrary());
    /* cannot add image group to empty template */
    OFCHECK(library.addImageGroup().bad());
    /* try again... */
    OFCHECK(library.createNewImageLibrary().good());
    OFCHECK(library.isValid());
    OFCHECK(library.hasImageLibrary());
    OFCHECK(library.addImageGroup().good());
}


OFTEST(dcmsr_CMR_SRNumericMeasurementValue)
{
    CMR_SRNumericMeasurementValue numValue;
    /* set coded entry from context group and check the value */
    OFCHECK(numValue.setNumericValueQualifier(CID42_NumericValueQualifier(CMR_CID42::NotANumber, OFTrue /*enhancedEncodingMode*/)).good());
    OFCHECK(numValue.getNumericValueQualifier() == CODE_DCM_NotANumber);
    OFCHECK(numValue.getNumericValueQualifier().usesEnhancedEncodingMode());
    OFCHECK(numValue.isValid());
    /* set coded entry from context group (using its type) and check the value */
    OFCHECK(numValue.setNumericValueQualifier(CMR_CID42::ValueUnknown).good());
    OFCHECK(numValue.getNumericValueQualifier() == CODE_DCM_ValueUnknown);
    OFCHECK(!numValue.getNumericValueQualifier().usesEnhancedEncodingMode());
    OFCHECK(numValue.isValid());
    /* set numeric measurement value to tree node */
    DSRNumTreeNode numNode(DSRTypes::RT_hasProperties);
    OFCHECK(numNode.setValue(numValue, OFTrue /*check*/).good());
    /* set coded entry that is not part of the context group */
    OFCHECK(numValue.setNumericValueQualifier(DSRBasicCodedEntry("0815", "99TEST", "Some test code"), OFTrue /*check*/) == SR_EC_CodedEntryNotInContextGroup);
    OFCHECK(numValue.isValid());
    OFCHECK(numValue.setNumericValueQualifier(DSRBasicCodedEntry("0815", "99TEST", "Some test code"), OFFalse /*check*/).good());
    OFCHECK(!numValue.isValid());
    /* check assignment operator */
    numValue = DSRNumericMeasurementValue(CODE_DCM_NotANumber);
    OFCHECK(numValue.isValid());
}


OFTEST(dcmsr_CMR_SRNumericMeasurementValueWithUnits_baselineGroup)
{
    /* start without a numeric value (but with a value qualifier) */
    CMR_SRNumericMeasurementValueWithUnits<CMR_CID7181> numValue(CMR_CID42::NotANumber);
    OFCHECK(numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK(numValue.getNumericValue().empty());
    OFCHECK(numValue.getMeasurementUnit().isEmpty());
    OFCHECK(!numValue.getNumericValueQualifier().isEmpty());
    /* set numeric value and measurement unit (from given context group) */
    OFCHECK(numValue.setValue("999", CMR_CID7181::Counts).good());
    OFCHECK(!numValue.isEmpty());
    OFCHECK(numValue.isValid());
    OFCHECK_EQUAL(numValue.getNumericValue(), "999");
    OFCHECK_EQUAL(numValue.getMeasurementUnit(), CMR_CID7181::getCodedEntry(CMR_CID7181::Counts));
    /* set coded entry that is not part of the context group */
    OFCHECK(numValue.setNumericValueQualifier(DSRBasicCodedEntry("0815", "99TEST", "Some test code"), OFTrue /*check*/).bad());
}


OFTEST(dcmsr_CMR_SRNumericMeasurementValueWithUnits_definedGroup)
{
    /* start with a numeric value that has an unknown measurement unit */
    const DSRCodedEntryValue validUnit(CMR_CID7181::getCodedEntry(CMR_CID7181::Counts));
    const DSRCodedEntryValue validQualifier(CMR_CID42::getCodedEntry(CMR_CID42::MeasurementFailure));
    const DSRCodedEntryValue invalidUnit("0815", "99TEST", "Some test code");
    const DSRCodedEntryValue invalidQualifier("4711", "99TEST", "Some other test code");
    CMR_SRNumericMeasurementValueWithUnits<CMR_CID7181, OFTrue /*T_DefinedGroup*/> numValue("1.5", invalidUnit);
    /* coded entry is not part of the defined context group (i.e. not set)*/
    OFCHECK(numValue.isEmpty());
    OFCHECK(numValue.isValid());
    /* try with a valid (defined) measurement unit */
    OFCHECK(numValue.setValue("1.5", CMR_CID7181::Counts).good());
    OFCHECK(numValue.isValid());
    /* try to disable the (extended) validity check */
    OFCHECK(numValue.setValue("1.5", invalidUnit, invalidQualifier, OFFalse /*check*/).good());
    OFCHECK(!numValue.isValid());
    OFCHECK(numValue.isComplete());
    OFCHECK_EQUAL(numValue.getMeasurementUnit(), invalidUnit);
    /* finally, set the measurement unit and value qualifier manually */
    OFCHECK(numValue.setMeasurementUnit(validUnit).good());
    OFCHECK(numValue.setNumericValueQualifier(validQualifier).good());
    OFCHECK(numValue.isValid());
}


OFTEST(dcmsr_cleanupContentMappingResource)
{
    /* cleanup the internal code lists explicitly (should be the last test case) */
    CMR_ContentMappingResource::cleanupAllContextGroups();
}
