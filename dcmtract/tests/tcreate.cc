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
 *  Module:  dcmtract
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests that check for pixel data overflow conditions
 *
 */


#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmtract/trctractographyresults.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/oftempf.h"
#include "dcmtk/dcmdata/dcdict.h"

static OFString EXPECTED_DUMP;

static TrcTractographyResults *create();
static void setGenericValues(TrcTractographyResults *ct);


OFTEST(dcmtract_create)
{
    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK(dcmDataDict.isDictionaryLoaded());
    }

    // Creation
    TrcTractographyResults* tract(create());
    OFCHECK(tract != OFnullptr);


    setGenericValues(tract);

    // Write to dataset and compare its dump with expected result
    DcmFileFormat dcmff;

    // We just use this to create a temporary file name
    OFTempFile tf(O_RDWR, "", "dcmtract_create", ".dcm");
    OFCondition result;
    result = tract->saveFile(tf.getFilename(), EXS_LittleEndianExplicit);
    OFCHECK_MSG(result == EC_Normal, result.text());
    delete tract; // delete tractography results object
}

static TrcTractographyResults *create()
{
    IODEnhGeneralEquipmentModule::EquipmentInfo eq("Open Connections", "OC CT", "4711", "0.1");
    TrcTractographyResults *tract = NULL;
    OFCondition result;
    result = TrcTractographyResults::create(ContentIdentificationMacro("1", "LABEL", "Description", "Open Connections"),
                                            "20250527",
                                            "103100",
                                            eq,
                                            IODReferences(),
                                            tract);

    OFCHECK(result.good());
    OFCHECK(tract != OFnullptr);
    return tract;
}


static void setGenericValues(TrcTractographyResults *tract)
{
    if (!tract)
        return;
    OFCHECK(tract->getPatient().setPatientName("Bond^James").good());
    OFCHECK(tract->getPatient().setPatientID("007").good());
    OFCHECK(tract->getPatient().setPatientBirthDate("19771007").good());
    OFCHECK(tract->getStudy().setStudyDate("20250101").good());
    OFCHECK(tract->getStudy().setStudyTime("120000").good());
    OFCHECK(tract->getStudy().setStudyID("1").good());
    OFCHECK(tract->getPatientStudy().setPatientAge("047Y").good());
    OFCHECK(tract->getSeries().setSeriesDescription("Test Description").good());
    OFCHECK(tract->getSeries().setSeriesNumber("1").good());

    // Those values are usually computed automatically. UIDS are generated and date/times are set to current values.
    // But in order to compare the "old" dump with the freshly created image attributes, we set some values manually,
    // so that they are not overwritten with new, automatically created values later.
    OFCHECK(tract->getStudy().setStudyInstanceUID("1.2.276.0.7230010.3.1.2.8323329.14863.1565940357.864811").good());
    OFCHECK(tract->getFrameOfReference().setFrameOfReferenceUID("2.25.30853397773651184949181049330553108086").good());
    OFCHECK(tract->getSeries().setSeriesInstanceUID("1.2.276.0.7230010.3.1.3.8323329.14863.1565940357.864812").good());
    OFCHECK(tract->getSOPCommon().setSOPInstanceUID("1.2.276.0.7230010.3.1.4.8323329.14863.1565940357.864813").good());

    // Create trackset
    CodeWithModifiers trackSetAnatomy("3", "1");
    trackSetAnatomy.set("12738006", "SCT", "Brain");
    // Create algo identification macro
    AlgorithmIdentificationMacro algo;
    algo.getAlgorithmFamilyCode().set("4711", "99OC", "Dummy Tracking");
    algo.setAlgorithmName("Deterministic Tracking");
    algo.setAlgorithmVersion("1.0");
    algo.setAlgorithmParameters("Parameter1=Value1\\Parameter2=Value2");
    algo.setAlgorithmSource("Open Connections");

    TrcTrackSet* tractSet = NULL;

    // Add trackset to tractography results
    OFCondition result = tract->addTrackSet("TRACK_SET_LABEL", "Track Set Description",
                                trackSetAnatomy,
                                CodeSequenceMacro("113231", "DCM", "Single Tensor"),
                                algo, tractSet);

    OFCHECK(result.good());
    OFCHECK(tractSet != OFnullptr);

    // Add tracks to trackset
    Float32 points[] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    Uint16 colors[] = { 0, 0, 0 };
    TrcTrack* track = NULL;
    result = tractSet->addTrack(points, 2, colors, 1, track);
    OFCHECK(result.good());
    OFCHECK(track != OFnullptr);
}
