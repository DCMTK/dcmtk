/*
 *
 *  Copyright (C) 2015, J. Riesmeier, Oldenburg, Germany
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

#include "dcmtk/dcmsr/dsrnumvl.h"
#include "dcmtk/dcmsr/dsrnumtn.h"
#include "dcmtk/dcmsr/codes/dcm.h"
#include "dcmtk/dcmsr/codes/ucum.h"
#include "dcmtk/dcmsr/cmr/init.h"
#include "dcmtk/dcmsr/cmr/logger.h"
#include "dcmtk/dcmsr/cmr/cid29e.h"
#include "dcmtk/dcmsr/cmr/cid42.h"
#include "dcmtk/dcmsr/cmr/cid244e.h"
#include "dcmtk/dcmsr/cmr/cid4020.h"
#include "dcmtk/dcmsr/cmr/cid4031e.h"
#include "dcmtk/dcmsr/cmr/cid7445.h"
#include "dcmtk/dcmsr/cmr/cid10013e.h"
#include "dcmtk/dcmsr/cmr/cid10033e.h"
#include "dcmtk/dcmsr/cmr/tid1001.h"
#include "dcmtk/dcmsr/cmr/tid1204.h"
#include "dcmtk/dcmsr/cmr/tid1600.h"
#include "dcmtk/dcmsr/cmr/srnumvl.h"


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
    OFCHECK_EQUAL(CID4031e_CommonAnatomicRegions::mapBodyPartExamined("ABDOMEN").getCodeMeaning(), "Abdomen");
    OFCHECK_EQUAL(CID4031e_CommonAnatomicRegions::mapBodyPartExamined("KNEE").getCodeMeaning(), "Knee");
    OFCHECK_EQUAL(CID4031e_CommonAnatomicRegions::mapBodyPartExamined("ZYGOMA").getCodeMeaning(), "Zygoma");
    /* invalid/unknown defined terms */
    OFCHECK(CID4031e_CommonAnatomicRegions::mapBodyPartExamined("XYZ").isEmpty());
    OFCHECK(CID4031e_CommonAnatomicRegions::mapBodyPartExamined("").isEmpty());
    OFCHECK(ctxGroup.selectValue("XYZ").bad());
}


OFTEST(dcmsr_CID7445_DeviceParticipatingRoles)
{
    CID7445_DeviceParticipatingRoles ctxGroup1;
    OFCHECK(!ctxGroup1.hasSelectedValue());
    OFCHECK(!ctxGroup1.hasCodedEntry(DSRBasicCodedEntry("0815", "99TEST", "Some test code")));
    /* add an extended code to the context group */
    OFCHECK(ctxGroup1.addCodedEntry(DSRBasicCodedEntry("0815", "99TEST", "Some test code")).good());
    OFCHECK(ctxGroup1.hasCodedEntry(DSRBasicCodedEntry("0815", "99TEST", "Some test code")));
    OFCHECK(ctxGroup1.findCodedEntry(CODE_DCM_Recording) == SR_EC_CodedEntryInStandardContextGroup);
    OFCHECK(ctxGroup1.findCodedEntry(DSRBasicCodedEntry("0815", "99TEST", "Some test code")) == SR_EC_CodedEntryIsExtensionOfContextGroup);
    /* try again with a non-extensible context group */
    CID7445_DeviceParticipatingRoles ctxGroup2(DSRBasicCodedEntry("4711", "99TEST", "Some other test code"));
    ctxGroup2.setExtensible(OFFalse);
    OFCHECK(ctxGroup2.hasSelectedValue());
    OFCHECK(ctxGroup2.addCodedEntry(DSRBasicCodedEntry("4711", "99TEST", "Some other test code")).bad());
    /* check whether the currently selected code is valid */
    OFCHECK(ctxGroup2.checkSelectedValue(OFFalse /*definedContextGroup*/).good());
    OFCHECK(ctxGroup2.checkSelectedValue(OFTrue /*definedContextGroup*/).bad());
    /* select another value (by two different ways) */
    OFCHECK(ctxGroup2.selectValue(CODE_DCM_XRayReadingDevice).good());
    OFCHECK(ctxGroup2.selectValue(CID7445_DeviceParticipatingRoles::IrradiatingDevice).good());
    /* and finally, check the static get() function */
    DSRCodedEntryValue codeValue = CID7445_DeviceParticipatingRoles::getCodedEntry(CID7445_DeviceParticipatingRoles::Recording, OFTrue /*enhancedEncodingMode*/);
    OFCHECK(codeValue == CODE_DCM_Recording);
    OFCHECK_EQUAL(codeValue.getContextIdentifier(), "7445");
    OFCHECK_EQUAL(codeValue.getMappingResource(), "DCMR");
}


OFTEST(dcmsr_CID10013e_CTAcquisitionType)
{
    OFCHECK(CID10013e_CTAcquisitionType::mapAcquisitionType("SEQUENCED") == CODE_DCM_SequencedAcquisition);
    OFCHECK(CID10013e_CTAcquisitionType::mapAcquisitionType("CONSTANT_ANGLE") == CODE_DCM_ConstantAngleAcquisition);
    OFCHECK(CID10013e_CTAcquisitionType::mapAcquisitionType("FREE") == CODE_DCM_FreeAcquisition);
    /* invalid/unknown defined terms */
    OFCHECK(CID10013e_CTAcquisitionType::mapAcquisitionType("XYZ").isEmpty());
}


OFTEST(dcmsr_CID10033e_CTReconstructionAlgorithm)
{
    OFCHECK(CID10033e_CTReconstructionAlgorithm::mapReconstructionAlgorithm("FILTER_BACK_PROJ") == CODE_DCM_FilteredBackProjection);
    OFCHECK(CID10033e_CTReconstructionAlgorithm::mapReconstructionAlgorithm("ITERATIVE") == CODE_DCM_IterativeReconstruction);
    /* invalid/unknown defined terms */
    OFCHECK(CID10033e_CTReconstructionAlgorithm::mapReconstructionAlgorithm("XYZ").isEmpty());
}


OFTEST(dcmsr_TID1001_ObservationContext)
{
    TID1001_ObservationContext obsContext;
    OFList<CID7445_DeviceParticipatingRoles> procRoles;
    procRoles.push_back(CID7445_DeviceParticipatingRoles::IrradiatingDevice);
    procRoles.push_back(CID7445_DeviceParticipatingRoles::Recording);
    /* empty template is not valid */
    OFCHECK(!obsContext.isValid());
    /* add person and device observers */
    OFCHECK(obsContext.addPersonObserver("Doe^John", "", CID7452_OrganizationalRoles::Physician, CID7453_PerformingRoles::Referring).good());
    OFCHECK(obsContext.addDeviceObserver("1.2.4.5.6.7.8.9.0").good());
    OFCHECK(obsContext.addDeviceObserver("007", "Some Name", "Some Manufacturer", "Some Model Name", "", "", procRoles, OFFalse /*check*/).good());
    OFCHECK(obsContext.addPersonObserver("Doe^Jane", "Some Organization").good());
    /* check some additions that should fail */
    OFCHECK(obsContext.addPersonObserver("").bad());
    OFCHECK(obsContext.addDeviceObserver("").bad());
    OFCHECK(obsContext.addDeviceObserver("01").bad());
    /* check whether all nodes are present */
    OFCHECK(obsContext.isValid());
    OFCHECK_EQUAL(obsContext.countNodes(), 16);
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


OFTEST(dcmsr_TID1600_ImageLibrary)
{
    TID1600_ImageLibrary library;
    DcmItem *item1, *item2;
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
    OFCHECK(library.isValid());
    OFCHECK(library.hasImageLibrary());
    OFCHECK(library.addImageGroup().good());
    OFCHECK(library.addImageEntry(dataset1).good());
    OFCHECK(library.addImageEntryDescriptors(dataset1).good());
    OFCHECK(library.addImageGroup().good());
    OFCHECK(library.addImageEntry(dataset2, TID1600_ImageLibrary::withAllDescriptors).good());
    OFCHECK(library.setPETImageRadionuclide(CID4020_PETRadionuclide::_18_Fluorine).good());
    OFCHECK(library.setPETImageRadiopharmaceuticalAgent(CID4021_PETRadiopharmaceutical::Fluorodeoxyglucose_F18).good());
    OFCHECK(library.setPETImageRadiopharmaceuticalStartDateTime("20151212").good());
    OFCHECK(library.setPETImageRadiopharmaceuticalVolume(DSRNumericMeasurementValue("99.9", CODE_UCUM_Cm3)).good());
    OFCHECK(library.setPETImageRadionuclideTotalDose(DSRNumericMeasurementValue("9999", CODE_UCUM_Bq)).good());
    OFCHECK(library.addImageEntry(dataset1, TID1600_ImageLibrary::withAllDescriptors).good());
    OFCHECK(library.setPETImageRadionuclide(CID4020_PETRadionuclide::_18_Fluorine).bad());
    OFCHECK(library.addImageEntry(dataset3, TID1600_ImageLibrary::withoutDescriptors).good());
    OFCHECK(library.addImageEntry(dataset4, TID1600_ImageLibrary::withAllDescriptors).good());
    OFCHECK(library.addImageEntryDescriptors(dataset3).good());
    /* check modality code of most recently added entry */
    DSRCodedEntryValue modality;
    OFCHECK(library.getImageEntryModality(modality).good());
    OFCHECK(modality == CODE_DCM_DigitalRadiography);
    /* try to add another invocation of TID 1602 */
    OFCHECK(library.addImageEntryDescriptors(dataset4).bad());
    /* check number of expected content items */
    OFCHECK_EQUAL(library.countNodes(), 61);

    /* output content of the tree (in debug mode only) */
    if (DCM_dcmsrCmrLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
    {
        library.print(COUT, DSRTypes::PF_printTemplateIdentification | DSRTypes::PF_printAllCodes | DSRTypes::PF_printSOPInstanceUID |
                            DSRTypes::PF_printNodeID | DSRTypes::PF_indicateEnhancedEncodingMode | DSRTypes::PF_printAnnotation);
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
    OFCHECK(numValue.setNumericValueQualifier(CID42_NumericValueQualifier(CID42_NumericValueQualifier::NotANumber)).good());
    OFCHECK(numValue.getNumericValueQualifier() == CODE_DCM_NotANumber);
    OFCHECK(numValue.isValid());
    /* set coded entry from context group (using its type) and check the value */
    OFCHECK(numValue.setNumericValueQualifier(CID42_NumericValueQualifier::ValueUnknown).good());
    OFCHECK(numValue.getNumericValueQualifier() == CODE_DCM_ValueUnknown);
    OFCHECK(numValue.isValid());
    /* set numeric measurement value to tree node */
    DSRNumTreeNode numNode(DSRTypes::RT_hasProperties);
    OFCHECK(numNode.setValue(numValue, OFTrue /*check*/).good());
    /* set coded entry that is not part of the context group */
    OFCHECK(numValue.setNumericValueQualifier(DSRBasicCodedEntry("0815", "99TEST", "Some test code"), OFTrue /*check*/) == SR_EC_CodedEntryNotInContextGroup);
    OFCHECK(numValue.isValid());
    OFCHECK(numValue.setNumericValueQualifier(DSRBasicCodedEntry("0815", "99TEST", "Some test code"), OFFalse /*check*/).good());
    OFCHECK(!numValue.isValid());
}


OFTEST(dcmsr_cleanupContentMappingResource)
{
    /* cleanup the internal code lists explicitly (should be the last test case) */
    CMR_ContentMappingResource::cleanupAllContextGroups();
}
