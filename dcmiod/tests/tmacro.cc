/*
 *
 *  Copyright (C) 2025-2026, OFFIS e.V.
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
 *  Module:  dcmiod
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for dcmiod macros
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmiod/iodmacro.h"


OFTEST(dcmiod_content_identification_macro)
{
    ContentIdentificationMacro macro;
    OFCHECK((macro.check(OFTrue /* quiet */).bad()));

    OFCHECK(macro.setContentCreatorName("OC").good());
    OFCHECK(macro.setContentDescription("Test Description").good());
    OFCHECK(macro.setContentLabel("TEST_LABEL").good());
    OFCHECK(macro.setInstanceNumber("42").good());

    OFCHECK(macro.check(OFTrue /* quiet */).good());
    OFString str;

    // Check all strings
    OFCHECK(macro.getContentCreatorName(str).good());
    OFCHECK(str == "OC");
    OFCHECK(macro.getContentDescription(str).good());
    OFCHECK(str == "Test Description");
    OFCHECK(macro.getContentLabel(str).good());
    OFCHECK(str == "TEST_LABEL");
    OFCHECK(macro.getInstanceNumber(str).good());
    OFCHECK(str == "42");

    // Write and re-read
    DcmItem item;
    OFCHECK(macro.write(item).good());
    ContentIdentificationMacro macro2;
    OFCHECK(macro2.read(item).good());
    OFCHECK(macro2.check(OFTrue /* quiet */).good());

    // Check strings in macro2
    OFCHECK(macro2.getContentCreatorName(str).good());
    OFCHECK(str == "OC");
    OFCHECK(macro2.getContentDescription(str).good());
    OFCHECK(str == "Test Description");
    OFCHECK(macro2.getContentLabel(str).good());
    OFCHECK(str == "TEST_LABEL");
    OFCHECK(macro2.getInstanceNumber(str).good());
    OFCHECK(str == "42");
}


OFTEST(dcmiod_image_sop_instance_reference_macro)
{
    ImageSOPInstanceReferenceMacro macro;
    OFCHECK(macro.setReferencedSOPClassUID(UID_CTImageStorage).good());
    OFCHECK(macro.setReferencedSOPInstanceUID("1.2.3.4").good());

    // Set several frame numbers at once. Referenced Frame Number has VR IS;
    // regression test for setReferencedFrameNumber() always failing with
    // EC_IllegalCall (putUint16() on the string-based attribute)
    OFVector<Uint16> frames;
    frames.push_back(1);
    frames.push_back(3);
    frames.push_back(65535);
    OFCHECK(macro.setReferencedFrameNumber(frames).good());

    OFVector<Uint16> readBack;
    OFCHECK(macro.getReferencedFrameNumber(readBack).good());
    OFCHECK(readBack.size() == 3);
    OFCHECK((readBack.size() == 3) && (readBack[0] == 1) && (readBack[1] == 3) && (readBack[2] == 65535));

    // Setting again must replace the old values, adding must append
    frames.clear();
    frames.push_back(7);
    OFCHECK(macro.setReferencedFrameNumber(frames).good());
    OFCHECK(macro.addReferencedFrameNumber(9).good());

    // Write and check the resulting attribute, then re-read
    DcmItem item;
    OFCHECK(macro.write(item).good());
    OFString str;
    OFCHECK(item.findAndGetOFStringArray(DCM_ReferencedFrameNumber, str).good());
    OFCHECK(str == "7\\9");

    ImageSOPInstanceReferenceMacro macro2;
    OFCHECK(macro2.read(item).good());
    readBack.clear();
    OFCHECK(macro2.getReferencedFrameNumber(readBack).good());
    OFCHECK((readBack.size() == 2) && (readBack[0] == 7) && (readBack[1] == 9));

    // The create() factory stores the given numbers as frame numbers for
    // non-segmentation SOP classes; it failed for those before, too
    ImageSOPInstanceReferenceMacro* created = NULL;
    OFCHECK(ImageSOPInstanceReferenceMacro::create(UID_CTImageStorage, "1.2.3.4", readBack, created).good());
    OFCHECK(created != NULL);
    if (created != NULL)
    {
        readBack.clear();
        OFCHECK(created->getReferencedFrameNumber(readBack).good());
        OFCHECK((readBack.size() == 2) && (readBack[0] == 7) && (readBack[1] == 9));
        delete created;
    }
}


OFTEST(dcmiod_mandatory_view_and_slice_progression_direction_macro)
{
    // The test creates and checks the following structure (PS3.3 Table 10-24):
    //
    // (item)
    // |-- View Code Sequence (0054,0220), 1 item
    // |   `-- Item 1: (399033003, SCT, "Lateral projection")
    // |       `-- View Modifier Code Sequence (0054,0222), 1 item
    // |           `-- Item 1: (399162004, SCT, "Right lateral projection")
    // `-- Slice Progression Direction (0054,0500) = "APEX_TO_BASE"
    MandatoryViewAndSliceProgressionDirectionMacro macro;
    OFCHECK(macro.getViewCode().set("399033003", "SCT", "Lateral projection").good());
    macro.getViewModifierCode().push_back(new CodeSequenceMacro("399162004", "SCT", "Right lateral projection"));
    OFCHECK(macro.getData().putAndInsertOFStringArray(DCM_SliceProgressionDirection, "APEX_TO_BASE").good());

    // Write and check the resulting structure: the View Modifier Code Sequence
    // must be nested within the single item of the View Code Sequence
    DcmItem item;
    OFCHECK(macro.write(item).good());
    DcmItem* viewCodeItem = NULL;
    OFCHECK(item.findAndGetSequenceItem(DCM_ViewCodeSequence, viewCodeItem).good());
    OFString str;
    if (viewCodeItem != NULL)
    {
        OFCHECK(viewCodeItem->findAndGetOFString(DCM_CodeValue, str).good());
        OFCHECK(str == "399033003");
        DcmItem* modifierItem = NULL;
        OFCHECK(viewCodeItem->findAndGetSequenceItem(DCM_ViewModifierCodeSequence, modifierItem).good());
        if (modifierItem != NULL)
        {
            OFCHECK(modifierItem->findAndGetOFString(DCM_CodeValue, str).good());
            OFCHECK(str == "399162004");
        }
    }
    OFCHECK(!item.tagExists(DCM_ViewModifierCodeSequence)); // not on top level
    OFCHECK(item.findAndGetOFString(DCM_SliceProgressionDirection, str).good());
    OFCHECK(str == "APEX_TO_BASE");

    // Re-read with default clearOldData=OFTrue; regression test for read()
    // clearing the already read View Code Sequence and View Modifier Code
    // Sequence again by calling IODComponent::read() last (which calls this
    // class' virtual clearData() method)
    MandatoryViewAndSliceProgressionDirectionMacro macro2;
    OFCHECK(macro2.read(item).good());
    OFCHECK(macro2.getViewCode().getCodeValue(str).good());
    OFCHECK(str == "399033003");
    OFCHECK(macro2.getViewModifierCode().size() == 1);
    if (macro2.getViewModifierCode().size() == 1)
    {
        OFCHECK(macro2.getViewModifierCode()[0]->getCodeValue(str).good());
        OFCHECK(str == "399162004");
    }
    OFCHECK(macro2.getData().findAndGetOFString(DCM_SliceProgressionDirection, str).good());
    OFCHECK(str == "APEX_TO_BASE");

    // Reading again must replace, not append, the modifier codes
    OFCHECK(macro2.read(item).good());
    OFCHECK(macro2.getViewModifierCode().size() == 1);
}
