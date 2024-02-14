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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for Concatenation Creator class
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmfg/concatenationloader.h"
#include "dcmtk/dcmiod/iodtypes.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstrutl.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcdeftag.h"

// Path to NEMA example data with trailing slash
static OFString NEMA_ENHANCED_CT_DIR = "/home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/";
static OFString SCAN_DUMP, DSET_DUMP;

static void prepare_scan_dump();
static void prepare_dset_dump();
static void check_dump(const OFString& expected, const OFString& output);

OFTEST(dcmfg_concatenation_loader)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    if (!OFStandard::dirExists(NEMA_ENHANCED_CT_DIR))
        return;

    prepare_scan_dump();
    prepare_dset_dump();

    // Scan directory and check that result is expected
    ConcatenationLoader cl;
    cl.setIgnoreMissingSourceUID(OFTrue); // NEMA test data misses this attribute...
    OFCondition result = cl.scan(NEMA_ENHANCED_CT_DIR);
    OFCHECK(result.good());
    const ConcatenationLoader::TScanResult& concats = cl.getInfo();
    OFCHECK(concats.size() == 3);
    OFStringStream out;
    ConcatenationLoader::TScanResult::const_iterator it = concats.begin();
    while (it != concats.end())
    {
        (*it).second->print(out);
        it++;
        if (it != concats.end())
        {
            out << "--------------------------------------------------------------" << OFendl;
        }
    }
    check_dump(SCAN_DUMP, out.str().c_str());

    ConcatenationLoader::TScanFailures failed = cl.getFailedFiles();
    ConcatenationLoader::TScanFailureIt ff    = failed.begin();
    while (ff != failed.end())
    {
        // Filename is set
        OFCHECK(!(*ff).fname.isEmpty());
        // Error is set accordingly
        OFCHECK((*ff).errorText == "File is not part of Concatenation");
        // SOP Instance UID is set and valid
        DcmUniqueIdentifier::checkStringValue((*ff).sopInstance, "1");
        ff++;
    }
    // Scanning done, now load

    DcmFileFormat dcmff;
    OFVector<DcmIODTypes::Frame*> frames;
    result = cl.load("1.3.6.1.4.1.5962.1.7.70.2.1.1166562673.14401", dcmff.getDataset(), frames);
    OFCHECK(result.good());
    if (result.good())
    {
        // Patch in expected values stored in pre-defined dump. In input concatenation,
        // SOP Instance UID of Concatenation Source is missing so the conversion will create
        // a new SOP Instance UID for the merged dataset.
        OFCHECK(dcmff.getDataset()->putAndInsertOFStringArray(DCM_ContentDate, "20191009").good());
        OFCHECK(dcmff.getDataset()->putAndInsertOFStringArray(DCM_ContentTime, "154326").good());
        OFCHECK(dcmff.getDataset()
                    ->putAndInsertOFStringArray(DCM_SOPInstanceUID, "1.3.6.1.4.1.5962.1.7.70.2.1.1166562673.14401")
                    .good());
        OFStringStream s;
        dcmff.getDataset()->print(s);
        check_dump(DSET_DUMP, s.str().c_str());
    }

    DcmIODUtil::freeContainer(frames);
}

static void prepare_scan_dump()
{
    SCAN_DUMP += "Concatenation UID*           : 1.3.6.1.4.1.5962.1.7.70.2.1.1166562673.14401\n";
    SCAN_DUMP += "  SOP Class UID*             : 1.2.840.10008.5.1.4.1.1.2.1\n";
    SCAN_DUMP += "  Concatenation Source UID*  : \n";
    SCAN_DUMP += "  Concatenation Source File  : \n";
    SCAN_DUMP += "  Number of Frames (computed): 60\n";
    SCAN_DUMP += "  In-conc. Total Number      : 6\n";
    SCAN_DUMP += "  Patient ID                 : 0070\n";
    SCAN_DUMP += "  Study Instance UID*        : 1.3.6.1.4.1.5962.1.2.70.1166562673.14401\n";
    SCAN_DUMP += "  Series Instance UID*       : 1.3.6.1.4.1.5962.1.3.70.2.1166562673.14401\n";
    SCAN_DUMP += "  Bits Allocated*            : 16\n";
    SCAN_DUMP += "  Rows*                      : 512\n";
    SCAN_DUMP += "  Columns*                   : 512\n";
    SCAN_DUMP += "  Files: \n";
    SCAN_DUMP += "    1. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0074_1\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.1.1.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 1\n";
    SCAN_DUMP += "    2. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0074_2\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.1.2.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 2\n";
    SCAN_DUMP += "    3. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0074_3\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.1.3.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 3\n";
    SCAN_DUMP += "    4. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0074_4\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.1.4.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 4\n";
    SCAN_DUMP += "    5. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0074_5\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.1.5.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 5\n";
    SCAN_DUMP += "    6. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0074_6\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.1.6.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 6\n";
    SCAN_DUMP += "--------------------------------------------------------------\n";
    SCAN_DUMP += "Concatenation UID*           : 1.3.6.1.4.1.5962.1.7.70.2.2.1166562673.14401\n";
    SCAN_DUMP += "  SOP Class UID*             : 1.2.840.10008.5.1.4.1.1.2.1\n";
    SCAN_DUMP += "  Concatenation Source UID*  : \n";
    SCAN_DUMP += "  Concatenation Source File  : \n";
    SCAN_DUMP += "  Number of Frames (computed): 30\n";
    SCAN_DUMP += "  In-conc. Total Number      : 3\n";
    SCAN_DUMP += "  Patient ID                 : 0070\n";
    SCAN_DUMP += "  Study Instance UID*        : 1.3.6.1.4.1.5962.1.2.70.1166562673.14401\n";
    SCAN_DUMP += "  Series Instance UID*       : 1.3.6.1.4.1.5962.1.3.70.2.1166562673.14401\n";
    SCAN_DUMP += "  Bits Allocated*            : 16\n";
    SCAN_DUMP += "  Rows*                      : 512\n";
    SCAN_DUMP += "  Columns*                   : 512\n";
    SCAN_DUMP += "  Files: \n";
    SCAN_DUMP += "    1. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0075_1\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.2.1.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 1\n";
    SCAN_DUMP += "    2. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0075_2\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.2.2.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 2\n";
    SCAN_DUMP += "    3. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0075_3\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.2.3.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 3\n";
    SCAN_DUMP += "--------------------------------------------------------------\n";
    SCAN_DUMP += "Concatenation UID*           : 1.3.6.1.4.1.5962.1.7.70.2.3.1166562673.14401\n";
    SCAN_DUMP += "  SOP Class UID*             : 1.2.840.10008.5.1.4.1.1.2.1\n";
    SCAN_DUMP += "  Concatenation Source UID*  : \n";
    SCAN_DUMP += "  Concatenation Source File  : \n";
    SCAN_DUMP += "  Number of Frames (computed): 30\n";
    SCAN_DUMP += "  In-conc. Total Number      : 3\n";
    SCAN_DUMP += "  Patient ID                 : 0070\n";
    SCAN_DUMP += "  Study Instance UID*        : 1.3.6.1.4.1.5962.1.2.70.1166562673.14401\n";
    SCAN_DUMP += "  Series Instance UID*       : 1.3.6.1.4.1.5962.1.3.70.2.1166562673.14401\n";
    SCAN_DUMP += "  Bits Allocated*            : 16\n";
    SCAN_DUMP += "  Rows*                      : 512\n";
    SCAN_DUMP += "  Columns*                   : 512\n";
    SCAN_DUMP += "  Files: \n";
    SCAN_DUMP += "    1. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0076_1\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.3.1.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 1\n";
    SCAN_DUMP += "    2. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0076_2\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.3.2.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 2\n";
    SCAN_DUMP += "    3. /home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/CT0076_3\n";
    SCAN_DUMP += "    SOP Instance UID: 1.3.6.1.4.1.5962.1.1.70.2.3.3.1166562673.14401\n";
    SCAN_DUMP += "    Number of Frames: 10\n";
    SCAN_DUMP += "    In-Concatenation Frame Offset Number: 3\n";

    OFStringUtil::replace_all(SCAN_DUMP, "/home/michael/data/dcm/nema_mf/DISCIMG/IMAGES/", NEMA_ENHANCED_CT_DIR);
}

static void prepare_dset_dump()
{
    DSET_DUMP += "\n";
    DSET_DUMP += "# Dicom-Data-Set\n";
    DSET_DUMP += "# Used TransferSyntax: Little Endian Explicit\n";
    DSET_DUMP += "(0008,0005) CS [ISO_IR 100]                             #  10, 1 SpecificCharacterSet\n";
    DSET_DUMP += "(0008,0008) CS [ORIGINAL\\PRIMARY\\POST_CONTRAST\\NONE]    #  36, 4 ImageType\n";
    DSET_DUMP += "(0008,0012) DA [20061219]                               #   8, 1 InstanceCreationDate\n";
    DSET_DUMP += "(0008,0013) TM [205025]                                 #   6, 1 InstanceCreationTime\n";
    DSET_DUMP += "(0008,0014) UI [1.3.6.1.4.1.5962.3]                     #  18, 1 InstanceCreatorUID\n";
    DSET_DUMP += "(0008,0016) UI =EnhancedCTImageStorage                  #  28, 1 SOPClassUID\n";
    DSET_DUMP += "(0008,0018) UI [1.3.6.1.4.1.5962.1.7.70.2.1.1166562673.14401] #  44, 1 SOPInstanceUID\n";
    DSET_DUMP += "(0008,0020) DA [20061219]                               #   8, 1 StudyDate\n";
    DSET_DUMP += "(0008,0021) DA [20061219]                               #   8, 1 SeriesDate\n";
    DSET_DUMP += "(0008,0023) DA [20191009]                               #   8, 1 ContentDate\n";
    DSET_DUMP += "(0008,002a) DT [20061219204055]                         #  14, 1 AcquisitionDateTime\n";
    DSET_DUMP += "(0008,0030) TM [203523]                                 #   6, 1 StudyTime\n";
    DSET_DUMP += "(0008,0031) TM [204010]                                 #   6, 1 SeriesTime\n";
    DSET_DUMP += "(0008,0033) TM [154326]                                 #   6, 1 ContentTime\n";
    DSET_DUMP += "(0008,0050) SH [0070]                                   #   4, 1 AccessionNumber\n";
    DSET_DUMP += "(0008,0060) CS [CT]                                     #   2, 1 Modality\n";
    DSET_DUMP += "(0008,0070) LO [Acme Medical Devices]                   #  20, 1 Manufacturer\n";
    DSET_DUMP += "(0008,0080) LO [St. Nowhere Hospital]                   #  20, 1 InstitutionName\n";
    DSET_DUMP += "(0008,0090) PN [Thomas^Albert]                          #  14, 1 ReferringPhysicianName\n";
    DSET_DUMP += "(0008,0201) SH [-0400]                                  #   6, 1 TimezoneOffsetFromUTC\n";
    DSET_DUMP += "(0008,1010) SH [CONSOLE01]                              #  10, 1 StationName\n";
    DSET_DUMP += "(0008,1030) LO (no value available)                     #   0, 0 StudyDescription\n";
    DSET_DUMP += "(0008,103e) LO [Shared dimensions and stacks across series and concatenations] #  62, 1 "
                 "SeriesDescription\n";
    DSET_DUMP += "(0008,1050) PN [Smith^John]                             #  10, 1 PerformingPhysicianName\n";
    DSET_DUMP += "(0008,1060) PN [Smith^John]                             #  10, 1 NameOfPhysiciansReadingStudy\n";
    DSET_DUMP += "(0008,1070) PN [Jones^Molly]                            #  12, 1 OperatorsName\n";
    DSET_DUMP += "(0008,1090) LO [Super Dooper Scanner]                   #  20, 1 ManufacturerModelName\n";
    DSET_DUMP += "(0008,9121) SQ (Sequence with undefined length #=1)     # u/l, 1 ReferencedRawDataSequence\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=2)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0008,1115) SQ (Sequence with undefined length #=1)     # u/l, 1 ReferencedSeriesSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=2)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0008,1199) SQ (Sequence with undefined length #=1)     # u/l, 1 ReferencedSOPSequence\n";
    DSET_DUMP += "          (fffe,e000) na (Item with undefined length #=2)         # u/l, 1 Item\n";
    DSET_DUMP += "            (0008,1150) UI =RawDataStorage                          #  26, 1 ReferencedSOPClassUID\n";
    DSET_DUMP += "            (0008,1155) UI [1.3.6.1.4.1.5962.1.9.70.2.1166562673.14401] #  42, 1 "
                 "ReferencedSOPInstanceUID\n";
    DSET_DUMP += "          (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "        (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "        (0020,000e) UI [1.3.6.1.4.1.5962.1.3.70.2.1166562673.14401] #  42, 1 SeriesInstanceUID\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,000d) UI [1.3.6.1.4.1.5962.1.2.70.1166562673.14401] #  40, 1 StudyInstanceUID\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "(0008,9205) CS [MONOCHROME]                             #  10, 1 PixelPresentation\n";
    DSET_DUMP += "(0008,9206) CS [VOLUME]                                 #   6, 1 VolumetricProperties\n";
    DSET_DUMP += "(0008,9207) CS [NONE]                                   #   4, 1 VolumeBasedCalculationTechnique\n";
    DSET_DUMP += "(0010,0010) PN [ChestAbdoPelvis^MultiphaseLiver]        #  32, 1 PatientName\n";
    DSET_DUMP += "(0010,0020) LO [0070]                                   #   4, 1 PatientID\n";
    DSET_DUMP += "(0010,0030) DA [19500704]                               #   8, 1 PatientBirthDate\n";
    DSET_DUMP += "(0010,0040) CS [M]                                      #   2, 1 PatientSex\n";
    DSET_DUMP += "(0010,1010) AS [052Y]                                   #   4, 1 PatientAge\n";
    DSET_DUMP += "(0010,1020) DS [1.6]                                    #   4, 1 PatientSize\n";
    DSET_DUMP += "(0010,1030) DS [75]                                     #   2, 1 PatientWeight\n";
    DSET_DUMP += "(0018,0012) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusAgentSequence\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=8)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0008,0100) SH [C-B0322]                                #   8, 1 CodeValue\n";
    DSET_DUMP += "    (0008,0102) SH [SRT]                                    #   4, 1 CodingSchemeDesignator\n";
    DSET_DUMP += "    (0008,0104) LO [Iohexol]                                #   8, 1 CodeMeaning\n";
    DSET_DUMP += "    (0018,0014) SQ (Sequence with undefined length #=1)     # u/l, 1 "
                 "ContrastBolusAdministrationRouteSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0008,0100) SH [G-D101]                                 #   6, 1 CodeValue\n";
    DSET_DUMP += "        (0008,0102) SH [SNM3]                                   #   4, 1 CodingSchemeDesignator\n";
    DSET_DUMP += "        (0008,0104) LO [Intravenous route]                      #  18, 1 CodeMeaning\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,1041) DS [150]                                    #   4, 1 ContrastBolusVolume\n";
    DSET_DUMP += "    (0018,1049) DS [300]                                    #   4, 1 "
                 "ContrastBolusIngredientConcentration\n";
    DSET_DUMP += "    (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "    (0018,9338) SQ (Sequence with undefined length #=1)     # u/l, 1 ContrastBolusIngredientCodeSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0008,0100) SH [C-11400]                                #   8, 1 CodeValue\n";
    DSET_DUMP += "        (0008,0102) SH [SRT]                                    #   4, 1 CodingSchemeDesignator\n";
    DSET_DUMP += "        (0008,0104) LO [Iodine]                                 #   6, 1 CodeMeaning\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=8)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0008,0100) SH [C-B0317]                                #   8, 1 CodeValue\n";
    DSET_DUMP += "    (0008,0102) SH [SNM3]                                   #   4, 1 CodingSchemeDesignator\n";
    DSET_DUMP += "    (0008,0104) LO [Diatrizoate]                            #  12, 1 CodeMeaning\n";
    DSET_DUMP += "    (0018,0014) SQ (Sequence with undefined length #=1)     # u/l, 1 "
                 "ContrastBolusAdministrationRouteSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0008,0100) SH [G-D140]                                 #   6, 1 CodeValue\n";
    DSET_DUMP += "        (0008,0102) SH [SNM3]                                   #   4, 1 CodingSchemeDesignator\n";
    DSET_DUMP += "        (0008,0104) LO [Oral route]                             #  10, 1 CodeMeaning\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,1041) DS [1000]                                   #   4, 1 ContrastBolusVolume\n";
    DSET_DUMP += "    (0018,1049) DS [11.1]                                   #   4, 1 "
                 "ContrastBolusIngredientConcentration\n";
    DSET_DUMP += "    (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "    (0018,9338) SQ (Sequence with undefined length #=1)     # u/l, 1 ContrastBolusIngredientCodeSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0008,0100) SH [C-11400]                                #   8, 1 CodeValue\n";
    DSET_DUMP += "        (0008,0102) SH [SRT]                                    #   4, 1 CodingSchemeDesignator\n";
    DSET_DUMP += "        (0008,0104) LO [Iodine]                                 #   6, 1 CodeMeaning\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "(0018,1000) LO [123456]                                 #   6, 1 DeviceSerialNumber\n";
    DSET_DUMP += "(0018,1020) LO [1.00]                                   #   4, 1 SoftwareVersions\n";
    DSET_DUMP += "(0018,5100) CS [FFS]                                    #   4, 1 PatientPosition\n";
    DSET_DUMP += "(0018,9004) CS [PRODUCT]                                #   8, 1 ContentQualification\n";
    DSET_DUMP += "(0018,9073) FD 522.00048828124988631                    #   8, 1 AcquisitionDuration\n";
    DSET_DUMP += "(0020,000d) UI [1.3.6.1.4.1.5962.1.2.70.1166562673.14401] #  40, 1 StudyInstanceUID\n";
    DSET_DUMP += "(0020,000e) UI [1.3.6.1.4.1.5962.1.3.70.2.1166562673.14401] #  42, 1 SeriesInstanceUID\n";
    DSET_DUMP += "(0020,0010) SH [0070]                                   #   4, 1 StudyID\n";
    DSET_DUMP += "(0020,0011) IS [2]                                      #   2, 1 SeriesNumber\n";
    DSET_DUMP += "(0020,0012) IS [2]                                      #   2, 1 AcquisitionNumber\n";
    DSET_DUMP += "(0020,0013) IS [1]                                      #   2, 1 InstanceNumber\n";
    DSET_DUMP += "(0020,0052) UI [1.3.6.1.4.1.5962.1.4.70.1.1166562673.14401] #  42, 1 FrameOfReferenceUID\n";
    DSET_DUMP += "(0020,1040) LO [XY]                                     #   2, 1 PositionReferenceIndicator\n";
    DSET_DUMP += "(0020,4000) LT [Both arterial and portal venous]        #  32, 1 ImageComments\n";
    DSET_DUMP += "(0020,9221) SQ (Sequence with undefined length #=1)     # u/l, 1 DimensionOrganizationSequence\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP
        += "    (0020,9164) UI [1.3.6.1.4.1.5962.1.6.70.2.0.1166562673.14401] #  44, 1 DimensionOrganizationUID\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "(0020,9222) SQ (Sequence with undefined length #=3)     # u/l, 1 DimensionIndexSequence\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP
        += "    (0020,9164) UI [1.3.6.1.4.1.5962.1.6.70.2.0.1166562673.14401] #  44, 1 DimensionOrganizationUID\n";
    DSET_DUMP += "    (0020,9165) AT (0020,9056)                              #   4, 1 DimensionIndexPointer\n";
    DSET_DUMP += "    (0020,9167) AT (0020,9111)                              #   4, 1 FunctionalGroupPointer\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP
        += "    (0020,9164) UI [1.3.6.1.4.1.5962.1.6.70.2.0.1166562673.14401] #  44, 1 DimensionOrganizationUID\n";
    DSET_DUMP += "    (0020,9165) AT (0020,9057)                              #   4, 1 DimensionIndexPointer\n";
    DSET_DUMP += "    (0020,9167) AT (0020,9111)                              #   4, 1 FunctionalGroupPointer\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP
        += "    (0020,9164) UI [1.3.6.1.4.1.5962.1.6.70.2.0.1166562673.14401] #  44, 1 DimensionOrganizationUID\n";
    DSET_DUMP += "    (0020,9165) AT (0018,9344)                              #   4, 1 DimensionIndexPointer\n";
    DSET_DUMP += "    (0020,9167) AT (0018,9341)                              #   4, 1 FunctionalGroupPointer\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "(0028,0002) US 1                                        #   2, 1 SamplesPerPixel\n";
    DSET_DUMP += "(0028,0004) CS [MONOCHROME2]                            #  12, 1 PhotometricInterpretation\n";
    DSET_DUMP += "(0028,0008) IS [60]                                     #   2, 1 NumberOfFrames\n";
    DSET_DUMP += "(0028,0010) US 512                                      #   2, 1 Rows\n";
    DSET_DUMP += "(0028,0011) US 512                                      #   2, 1 Columns\n";
    DSET_DUMP += "(0028,0100) US 16                                       #   2, 1 BitsAllocated\n";
    DSET_DUMP += "(0028,0101) US 16                                       #   2, 1 BitsStored\n";
    DSET_DUMP += "(0028,0102) US 15                                       #   2, 1 HighBit\n";
    DSET_DUMP += "(0028,0103) US 1                                        #   2, 1 PixelRepresentation\n";
    DSET_DUMP += "(0028,0301) CS [NO]                                     #   2, 1 BurnedInAnnotation\n";
    DSET_DUMP += "(0028,2110) CS [00]                                     #   2, 1 LossyImageCompression\n";
    DSET_DUMP += "(0040,0555) SQ (Sequence with undefined length #=0)     # u/l, 1 AcquisitionContextSequence\n";
    DSET_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "(2050,0020) CS [IDENTITY]                               #   8, 1 PresentationLUTShape\n";
    DSET_DUMP += "(5200,9229) SQ (Sequence with undefined length #=1)     # u/l, 1 SharedFunctionalGroupsSequence\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=14)        # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9301) SQ (Sequence with undefined length #=1)     # u/l, 1 CTAcquisitionTypeSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9302) CS [SPIRAL]                                 #   6, 1 AcquisitionType\n";
    DSET_DUMP += "        (0018,9333) CS [NO]                                     #   2, 1 ConstantVolumeFlag\n";
    DSET_DUMP += "        (0018,9334) CS [NO]                                     #   2, 1 FluoroscopyFlag\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9304) SQ (Sequence with undefined length #=1)     # u/l, 1 CTAcquisitionDetailsSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,0090) DS [480.000]                                #   8, 1 DataCollectionDiameter\n";
    DSET_DUMP += "        (0018,1120) DS [0.00000]                                #   8, 1 GantryDetectorTilt\n";
    DSET_DUMP += "        (0018,1130) DS [103.200]                                #   8, 1 TableHeight\n";
    DSET_DUMP += "        (0018,1140) CS [CW]                                     #   2, 1 RotationDirection\n";
    DSET_DUMP += "        (0018,9305) FD 1                                        #   8, 1 RevolutionTime\n";
    DSET_DUMP += "        (0018,9306) FD 7.000007629394530361822                  #   8, 1 SingleCollimationWidth\n";
    DSET_DUMP += "        (0018,9307) FD 7.000007629394530361822                  #   8, 1 TotalCollimationWidth\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9308) SQ (Sequence with undefined length #=1)     # u/l, 1 CTTableDynamicsSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9309) FD 9.8000183105468732                       #   8, 1 TableSpeed\n";
    DSET_DUMP += "        (0018,9310) FD 9.800010681152342                        #   8, 1 TableFeedPerRotation\n";
    DSET_DUMP += "        (0018,9311) FD 1.4000015258789061                       #   8, 1 SpiralPitchFactor\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9312) SQ (Sequence with undefined length #=1)     # u/l, 1 CTGeometrySequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=2)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,1110) DS [1099.31]                                #   8, 1 DistanceSourceToDetector\n";
    DSET_DUMP += "        (0018,9335) FD 630.00048828124988631                    #   8, 1 "
                 "DistanceSourceToDataCollectionCenter\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9314) SQ (Sequence with undefined length #=1)     # u/l, 1 CTReconstructionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,1100) DS [325.000]                                #   8, 1 ReconstructionDiameter\n";
    DSET_DUMP += "        (0018,1210) SH [STANDARD]                               #   8, 1 ConvolutionKernel\n";
    DSET_DUMP += "        (0018,9315) CS [ITERATIVE]                              #  10, 1 ReconstructionAlgorithm\n";
    DSET_DUMP += "        (0018,9316) CS [SOFT_TISSUE]                            #  12, 1 ConvolutionKernelGroup\n";
    DSET_DUMP += "        (0018,9319) FD 360.00024414062494316                    #   8, 1 ReconstructionAngle\n";
    DSET_DUMP += "        (0018,9320) SH [None]                                   #   4, 1 ImageFilter\n";
    DSET_DUMP
        += "        (0018,9322) FD 0.63476610183715811\\0.63476610183715811  #  16, 2 ReconstructionPixelSpacing\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9321) SQ (Sequence with undefined length #=1)     # u/l, 1 CTExposureSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=5)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9323) CS [NONE]                                   #   4, 1 ExposureModulationType\n";
    DSET_DUMP += "        (0018,9328) FD 1000.0004882812498863                    #   8, 1 ExposureTimeInms\n";
    DSET_DUMP += "        (0018,9330) FD 230.00012207031247158                    #   8, 1 XRayTubeCurrentInmA\n";
    DSET_DUMP += "        (0018,9332) FD 230.00012207031247158                    #   8, 1 ExposureInmAs\n";
    DSET_DUMP += "        (0018,9345) FD (no value available)                     #   0, 0 CTDIvol\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9325) SQ (Sequence with undefined length #=1)     # u/l, 1 CTXRayDetailsSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,0060) DS [120.000]                                #   8, 1 KVP\n";
    DSET_DUMP += "        (0018,1160) SH [WEDGE]                                  #   6, 1 FilterType\n";
    DSET_DUMP += "        (0018,1190) DS [1.20000]                                #   8, 1 FocalSpots\n";
    DSET_DUMP += "        (0018,7050) CS [MIXED]                                  #   6, 1 FilterMaterial\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9329) SQ (Sequence with undefined length #=1)     # u/l, 1 CTImageFrameTypeSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0008,9007) CS [ORIGINAL\\PRIMARY\\POST_CONTRAST\\NONE]    #  36, 4 FrameType\n";
    DSET_DUMP += "        (0008,9205) CS [MONOCHROME]                             #  10, 1 PixelPresentation\n";
    DSET_DUMP += "        (0008,9206) CS [VOLUME]                                 #   6, 1 VolumetricProperties\n";
    DSET_DUMP
        += "        (0008,9207) CS [NONE]                                   #   4, 1 VolumeBasedCalculationTechnique\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9477) SQ (Sequence with undefined length #=1)     # u/l, 1 "
                 "IrradiationEventIdentificationSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP
        += "        (0008,3010) UI [1.3.6.1.4.1.5962.1.10.70.2.1.1166562673.14401] #  46, 1 IrradiationEventUID\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9071) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameAnatomySequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=2)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0008,2218) SQ (Sequence with undefined length #=1)     # u/l, 1 AnatomicRegionSequence\n";
    DSET_DUMP += "          (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "            (0008,0100) SH [R-FAB57]                                #   8, 1 CodeValue\n";
    DSET_DUMP
        += "            (0008,0102) SH [SRT]                                    #   4, 1 CodingSchemeDesignator\n";
    DSET_DUMP += "            (0008,0104) LO [Abdomen and Pelvis]                     #  18, 1 CodeMeaning\n";
    DSET_DUMP += "          (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "        (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "        (0020,9072) CS [U]                                      #   2, 1 FrameLaterality\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9116) SQ (Sequence with undefined length #=1)     # u/l, 1 PlaneOrientationSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0037) DS [1.00000\\0.00000\\0.00000\\0.00000\\1.00000\\0.00000] #  48, 6 "
                 "ImageOrientationPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0028,9110) SQ (Sequence with undefined length #=1)     # u/l, 1 PixelMeasuresSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=2)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,0050) DS [7.00000]                                #   8, 1 SliceThickness\n";
    DSET_DUMP += "        (0028,0030) DS [0.634766\\0.634766]                      #  18, 2 PixelSpacing\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0028,9132) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameVOILUTSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=2)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0028,1050) DS [40.0000]                                #   8, 1 WindowCenter\n";
    DSET_DUMP += "        (0028,1051) DS [400.000]                                #   8, 1 WindowWidth\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP
        += "    (0028,9145) SQ (Sequence with undefined length #=1)     # u/l, 1 PixelValueTransformationSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0028,1052) DS [-1024.00]                               #   8, 1 RescaleIntercept\n";
    DSET_DUMP += "        (0028,1053) DS [1.00000]                                #   8, 1 RescaleSlope\n";
    DSET_DUMP += "        (0028,1054) LO [HU]                                     #   2, 1 RescaleType\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "(5200,9230) SQ (Sequence with explicit length #=60)     #   0, 1 PerFrameFunctionalGroupsSequence\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-10.00000762939452947364 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-10.00000762939452947364 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -10.00000762939452947364                 #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204055]                         #  14, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204055.350]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 30                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\30\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-10.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-17.0000152587890589473 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-17.0000152587890589473 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -17.00143432617187145                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204055.714]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204056.064]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 29                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\29\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-17.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-24.0000152587890589473 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-24.0000152587890589473 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -24.00285339355468395                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204056.428]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204056.778]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 28                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\28\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-24.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-31.0000152587890589473 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-31.0000152587890589473 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -31.00427246093749645                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204057.142]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204057.492]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 27                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\27\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-31.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-38.0000305175781178946 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-38.0000305175781178946 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -37.995758056640618                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204057.857]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204058.207]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 26                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\26\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-38.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-45.0000305175781178946 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-45.0000305175781178946 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -44.997161865234368                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204058.571]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204058.921]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 25                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\25\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-45.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-52.0000305175781178946 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-52.0000305175781178946 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -51.998596191406243                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204059.285]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204059.635]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 24                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\24\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-52.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-59.0000305175781178946 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-59.0000305175781178946 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -59.0000305175781178946                  #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204100]                         #  14, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204100.350]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 23                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\23\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-59.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-66.0000610351562357891 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-66.0000610351562357891 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -66.00146484374998579                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204100.714]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204101.064]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 22                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\22\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-66.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-73.0000610351562357891 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-73.0000610351562357891 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -73.00286865234373579                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204101.428]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204101.778]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 21                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\21\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-73.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-80.0000610351562357891 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-80.0000610351562357891 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -80.00433349609373579                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204102.142]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204102.492]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 20                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\20\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-80.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-87.0000610351562357891 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-87.0000610351562357891 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -86.995788574218736                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204102.857]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204103.207]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 19                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\19\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-87.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-94.0000610351562357891 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-94.0000610351562357891 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -93.997192382812486                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204103.571]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204103.921]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 18                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\18\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-94.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-101.000061035156235789 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-101.000061035156235789 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -100.99859619140624                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204104.285]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204104.635]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 17                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\17\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-101.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-108.000061035156235789 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-108.000061035156235789 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -108.000061035156235789                  #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204105]                         #  14, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204105.350]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 16                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\16\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-108.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-115.000061035156235789 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-115.000061035156235789 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -115.0014648437499858                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204105.714]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204106.064]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 15                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\15\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-115.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-122.000061035156235789 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-122.000061035156235789 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -122.0028686523437358                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204106.428]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204106.778]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 14                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\14\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-122.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-129.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-129.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -129.0043945312499716                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204107.142]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204107.492]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 13                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\13\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-129.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-136.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-136.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -135.99584960937497                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204107.857]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204108.207]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 12                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\12\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-136.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-143.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-143.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -142.99719238281247                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204108.571]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204108.921]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 11                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\11\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-143.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-150.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-150.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -149.99865722656247                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204109.285]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204109.635]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 10                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\10\\1                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-150.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-157.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-157.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -157.00012207031247158                   #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204109.999]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204110.349]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 9                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\9\\1                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-157.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-164.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-164.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -164.0014648437499716                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204110.714]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204111.064]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 8                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\8\\1                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-164.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-171.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-171.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -171.0029296874999716                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204111.428]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204111.778]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 7                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\7\\1                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-171.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-178.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-178.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -178.0043945312499716                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204112.142]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204112.492]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 6                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\6\\1                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-178.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-185.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-185.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -184.99584960937497                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204112.857]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204113.207]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 5                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\5\\1                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-185.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-192.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-192.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -191.99719238281247                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204113.571]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204113.921]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 4                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\4\\1                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-192.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-199.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-199.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -198.99865722656247                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204114.285]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204114.635]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 3                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\3\\1                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-199.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-206.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-206.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -206.00012207031247158                   #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204115]                         #  14, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204115.350]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 2                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\2\\1                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-206.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-213.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-213.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -213.0014648437499716                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [ARTERIAL]                               #   8, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204115.714]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204116.064]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 1                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\1\\1                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-213.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-10.00000762939452947364 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-10.00000762939452947364 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -10.00000762939452947364                 #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204137]                         #  14, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204137.350]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 30                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\30\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-10.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-17.0000152587890589473 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-17.0000152587890589473 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -17.00143432617187145                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204137.714]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204138.064]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 29                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\29\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-17.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-24.0000152587890589473 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-24.0000152587890589473 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -24.00285339355468395                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204138.428]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204138.778]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 28                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\28\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-24.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-31.0000152587890589473 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-31.0000152587890589473 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -31.00427246093749645                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204139.142]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204139.492]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 27                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\27\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-31.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-38.0000305175781178946 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-38.0000305175781178946 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -37.995758056640618                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204139.857]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204140.207]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 26                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\26\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-38.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-45.0000305175781178946 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-45.0000305175781178946 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -44.997161865234368                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204140.571]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204140.921]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 25                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\25\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-45.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-52.0000305175781178946 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-52.0000305175781178946 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -51.998596191406243                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204141.285]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204141.635]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 24                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\24\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-52.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-59.0000305175781178946 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-59.0000305175781178946 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -59.0000305175781178946                  #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204142]                         #  14, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204142.350]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 23                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\23\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-59.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-66.0000610351562357891 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-66.0000610351562357891 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -66.00146484374998579                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204142.714]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204143.064]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 22                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\22\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-66.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-73.0000610351562357891 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-73.0000610351562357891 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -73.00286865234373579                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204143.428]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204143.778]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 21                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\21\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-73.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-80.0000610351562357891 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-80.0000610351562357891 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -80.00433349609373579                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204144.142]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204144.492]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 20                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\20\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-80.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-87.0000610351562357891 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-87.0000610351562357891 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -86.995788574218736                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204144.857]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204145.207]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 19                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\19\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-87.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-94.0000610351562357891 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-94.0000610351562357891 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -93.997192382812486                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204145.571]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204145.921]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 18                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\18\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-94.0000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-101.000061035156235789 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-101.000061035156235789 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -100.99859619140624                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204146.285]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204146.635]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 17                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\17\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-101.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-108.000061035156235789 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-108.000061035156235789 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -108.000061035156235789                  #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204147]                         #  14, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204147.350]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 16                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\16\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-108.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-115.000061035156235789 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-115.000061035156235789 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -115.0014648437499858                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204147.714]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204148.064]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 15                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\15\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-115.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-122.000061035156235789 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-122.000061035156235789 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -122.0028686523437358                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204148.428]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204148.778]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 14                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\14\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-122.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-129.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-129.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -129.0043945312499716                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204149.142]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204149.492]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 13                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\13\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-129.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-136.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-136.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -135.99584960937497                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204149.857]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204150.207]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 12                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\12\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-136.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-143.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-143.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -142.99719238281247                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204150.571]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204150.921]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 11                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\11\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-143.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-150.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-150.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -149.99865722656247                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204151.285]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204151.635]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 10                                       #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\10\\2                                   #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-150.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-157.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-157.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -157.00012207031247158                   #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204151.999]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204152.349]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 9                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\9\\2                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-157.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-164.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-164.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -164.0014648437499716                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204152.714]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204153.064]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 8                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\8\\2                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-164.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-171.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-171.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -171.0029296874999716                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204153.428]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204153.778]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 7                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\7\\2                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-171.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-178.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-178.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -178.0043945312499716                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204154.142]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204154.492]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 6                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\6\\2                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-178.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-185.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-185.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -184.99584960937497                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204154.857]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204155.207]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 5                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\5\\2                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-185.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-192.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-192.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -191.99719238281247                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204155.571]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204155.921]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 4                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\4\\2                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-192.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-199.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-199.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -198.99865722656247                      #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204156.285]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204156.635]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 3                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\3\\2                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-199.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-206.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-206.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -206.00012207031247158                   #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204157]                         #  14, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204157.350]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 2                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\2\\2                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-206.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "  (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "    (0018,9326) SQ (Sequence with undefined length #=1)     # u/l, 1 CTPositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9313) FD 3.08271408081054643\\-0.31728720664978023\\-213.00012207031247158 #  24, 3 "
                 "DataCollectionCenterPatient\n";
    DSET_DUMP += "        (0018,9318) FD 3.08271408081054643\\-0.31728720664978023\\-213.00012207031247158 #  24, 3 "
                 "ReconstructionTargetCenterPatient\n";
    DSET_DUMP += "        (0018,9327) FD -213.0014648437499716                    #   8, 1 TablePosition\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0018,9341) SQ (Sequence with undefined length #=2)     # u/l, 1 ContrastBolusUsageSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=4)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 1                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS [YES]                                    #   4, 1 ContrastBolusAgentDetected\n";
    DSET_DUMP += "        (0018,9344) CS [PORTAL_VENOUS]                          #  14, 1 ContrastBolusAgentPhase\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=3)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9337) US 2                                        #   2, 1 ContrastBolusAgentNumber\n";
    DSET_DUMP
        += "        (0018,9342) CS [YES]                                    #   4, 1 ContrastBolusAgentAdministered\n";
    DSET_DUMP
        += "        (0018,9343) CS (no value available)                     #   0, 0 ContrastBolusAgentDetected\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9111) SQ (Sequence with undefined length #=1)     # u/l, 1 FrameContentSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=7)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0018,9074) DT [20061219204157.714]                     #  18, 1 FrameAcquisitionDateTime\n";
    DSET_DUMP += "        (0018,9151) DT [20061219204158.064]                     #  18, 1 FrameReferenceDateTime\n";
    DSET_DUMP += "        (0018,9220) FD 700.00048828124988631                    #   8, 1 FrameAcquisitionDuration\n";
    DSET_DUMP += "        (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    DSET_DUMP += "        (0020,9057) UL 1                                        #   4, 1 InStackPositionNumber\n";
    DSET_DUMP += "        (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    DSET_DUMP += "        (0020,9157) UL 1\\1\\2                                    #  12, 3 DimensionIndexValues\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "    (0020,9113) SQ (Sequence with undefined length #=1)     # u/l, 1 PlanePositionSequence\n";
    DSET_DUMP += "      (fffe,e000) na (Item with undefined length #=1)         # u/l, 1 Item\n";
    DSET_DUMP += "        (0020,0032) DS [-159.100\\-162.500\\-213.000]             #  26, 3 ImagePositionPatient\n";
    DSET_DUMP += "      (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "    (fffe,e0dd) na (SequenceDelimitationItem)               #   0, 0 SequenceDelimitationItem\n";
    DSET_DUMP += "  (fffe,e00d) na (ItemDelimitationItem)                   #   0, 0 ItemDelimitationItem\n";
    DSET_DUMP += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
}

void check_dump(const OFString& expected, const OFString& output)
{
    OFBool matches = (expected == output);
    OFCHECK(matches);
    if (!matches)
    {
        CERR << "Expected dump:" << OFendl;
        CERR << expected << OFendl;
        CERR << "Produced dump:" << OFendl;
        CERR << output << OFendl;
    }
}
