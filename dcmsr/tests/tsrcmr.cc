/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    test program for classes CID7445_DeviceParticipatingRoles,
 *    TID1001_ObservationContext and TID1204_LanguageOfContentItemAndDescendants
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"

#include "dcmtk/dcmsr/dsrnumvl.h"
#include "dcmtk/dcmsr/dsrnumtn.h"
#include "dcmtk/dcmsr/codes/dcm.h"
#include "dcmtk/dcmsr/cmr/cid29e.h"
#include "dcmtk/dcmsr/cmr/cid42.h"
#include "dcmtk/dcmsr/cmr/cid244e.h"
#include "dcmtk/dcmsr/cmr/cid4031e.h"
#include "dcmtk/dcmsr/cmr/cid7445.h"
#include "dcmtk/dcmsr/cmr/tid1001.h"
#include "dcmtk/dcmsr/cmr/tid1204.h"
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
    /* invalid/unknown enumerated values */
    OFCHECK(!ctxGroup.mapImageLaterality("XYZ").isValid());
    OFCHECK(ctxGroup.mapImageLaterality("ABC", codedEntry) == SR_EC_InvalidValue);
    OFCHECK(ctxGroup.selectValue("").bad());
}


OFTEST(dcmsr_CID4031e_CommonAnatomicRegions)
{
    CID4031e_CommonAnatomicRegions ctxGroup("HEART");
    OFCHECK(ctxGroup.hasSelectedValue());
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


OFTEST(dcmsr_TID1001_ObservationContext)
{
    TID1001_ObservationContext obsContext;
    OFList<CID7445_DeviceParticipatingRoles> procRoles;
    procRoles.push_back(CID7445_DeviceParticipatingRoles::IrradiatingDevice);
    procRoles.push_back(CID7445_DeviceParticipatingRoles::Recording);
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
}


OFTEST(dcmsr_TID1204_LanguageOfContentItemAndDescendants)
{
    TID1204_LanguageOfContentItemAndDescendants lang;
    /* add language */
    OFCHECK(lang.setLanguage(CID5000_Languages::German_DE).good());
    OFCHECK(lang.isValid());
    OFCHECK_EQUAL(lang.countNodes(), 1);
    /* add language and separate country */
    OFCHECK(lang.setLanguage(CID5000_Languages::German, CID5001_Countries::Germany).good());
    OFCHECK(lang.isValid());
    OFCHECK_EQUAL(lang.countNodes(), 2);
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
