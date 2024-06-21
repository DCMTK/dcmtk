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
 *  Purpose: Tests for CT Acquisition Type FG class
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmfg/fgctacquisitiontype.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdict.h"

static void init_template(OFString& fg_dump)
{
    fg_dump += "(fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    fg_dump += "(0018,9301) SQ (Sequence with explicit length #=1)      #   0, 1 CTAcquisitionTypeSequence\n";
    fg_dump += "  (fffe,e000) na (Item with explicit length #=4)          #   0, 1 Item\n";
    fg_dump += "    (0018,9302) CS [STATIONARY]                             #  10, 1 AcquisitionType\n";
    fg_dump += "    (0018,9303) FD 0.1234                                   #   8, 1 TubeAngle\n";
    fg_dump += "    (0018,9333) CS [YES]                                    #   4, 1 ConstantVolumeFlag\n";
    fg_dump += "    (0018,9334) CS [NO]                                     #   2, 1 FluoroscopyFlag\n";
    fg_dump += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    fg_dump += "(fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
}

static void check_ct_acquisition_type_fg(FGCTAcquisitionType& fg)
{
    OFCondition result;
    OFString val;
    OFCHECK(fg.getAcquisitionType(val).good());
    OFCHECK(val == "STATIONARY");
    Float64 f64;
    OFCHECK(fg.getTubeAngle(f64).good());
    OFCHECK(f64 == 0.1234);
    OFCHECK(fg.getConstantVolumeFlag(val).good());
    OFCHECK(val == "YES");
    OFCHECK(fg.getFluoroscopyFlag(val).good());
    OFCHECK(val == "NO");

    // check enum getters()
    FGCTAcquisitionType::E_ConstantVolumeFlag cv;
    OFCHECK(fg.getConstantVolumeFlag(cv).good());
    OFCHECK(cv == FGCTAcquisitionType::E_ConstVol_Yes);

    FGCTAcquisitionType::E_FluoroscopyFlag fl;
    OFCHECK(fg.getFluoroscopyFlag(fl).good());
    OFCHECK(fl == FGCTAcquisitionType::E_Fluoroscopy_No);
}

OFTEST(dcmfg_ct_acquisition_type)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    OFString fg_dump;
    init_template(fg_dump);

    FGCTAcquisitionType fg;
    OFCHECK(fg.setAcquisitionType(FGCTAcquisitionType::DT_AcquisitionType_Stationary).good());
    OFCHECK(fg.setTubeAngle(0.1234).good());
    OFCHECK(fg.setConstantVolumeFlag(FGCTAcquisitionType::E_ConstVol_Yes).good());
    OFCHECK(fg.setFluoroscopyFlag(FGCTAcquisitionType::E_Fluoroscopy_No).good());

    // Check data structure in memory
    check_ct_acquisition_type_fg(fg);

    // Write to DcmItem and compare with pre-defined template
    DcmItem dest_item;
    OFCondition result = fg.write(dest_item);
    OFCHECK(result.good());
    OFStringStream out;
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str()); /**/

    // Test read method: Read from dataset, write again, and compare another time
    FGCTAcquisitionType fg_for_read;
    out.str(""); // set to empty
    fg_for_read.read(dest_item);
    dest_item.clear();
    result = fg_for_read.write(dest_item);
    OFCHECK(result.good());
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str());

    // Test compare() method
    OFCHECK(fg.compare(fg_for_read) == 0);
    OFCHECK(fg_for_read.setAcquisitionType(FGCTAcquisitionType::DT_AcquisitionType_ConstantAngle).good());
    OFCHECK(fg.compare(fg_for_read) != 0);

    // Test clone() method
    FGCTAcquisitionType* clone = OFstatic_cast(FGCTAcquisitionType*, fg.clone());
    OFCHECK(clone != NULL);
    OFCHECK(clone->compare(fg) == 0);
    delete clone;
}
