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
 *  Purpose: Tests for CT Image Frame Type FG class
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmfg/fgctimageframetype.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdict.h"

static void init_template(OFString& fg_dump)
{
    fg_dump += "(fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    fg_dump += "(0018,9329) SQ (Sequence with explicit length #=1)      #   0, 1 CTImageFrameTypeSequence\n";
    fg_dump += "  (fffe,e000) na (Item with explicit length #=4)          #   0, 1 Item\n";
    fg_dump += "    (0008,9007) CS [ORIGINAL\\PRIMARY\\VOLUME\\NONE]           #  28, 4 FrameType\n";
    fg_dump += "    (0008,9205) CS [MONOCHROME]                             #  10, 1 PixelPresentation\n";
    fg_dump += "    (0008,9206) CS [VOLUME]                                 #   6, 1 VolumetricProperties\n";
    fg_dump += "    (0008,9207) CS [VOLUME_RENDER]                          #  14, 1 VolumeBasedCalculationTechnique\n";
    fg_dump += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    fg_dump += "(fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
}

static void check_ct_image_frame_type_fg(FGCTImageFrameType& fg)
{
    OFCondition result;
    OFString val;
    OFCHECK(fg.getFrameType(val, -1 /* all components */).good());
    OFCHECK(val == "ORIGINAL\\PRIMARY\\VOLUME\\NONE");
    OFCHECK(fg.getPixelPresentation(val).good());
    OFCHECK(val == "MONOCHROME");
    OFCHECK(fg.getVolumetricProperties(val).good());
    OFCHECK(val == "VOLUME");
    OFCHECK(fg.getVolumeBasedCalculationTechnique(val).good());
    OFCHECK(val == "VOLUME_RENDER");

    // Check enum getters
    FGCTImageFrameType::E_PixelPresentation pix;
    OFCHECK(fg.getPixelPresentation(pix).good());
    OFCHECK(pix == FGCTImageFrameType::E_PixelPres_Monochrome);

    FGCTImageFrameType::E_VolumetricProperties vol;
    OFCHECK(fg.getVolumetricProperties(vol).good());
    OFCHECK(vol == FGCTImageFrameType::E_VolProp_Volume);
}

OFTEST(dcmfg_ct_image_frame_type)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    OFString fg_dump;
    init_template(fg_dump);

    FGCTImageFrameType fg;
    OFCHECK(fg.setFrameType("ORIGINAL\\PRIMARY\\VOLUME\\NONE").good());
    OFCHECK(fg.setPixelPresentation(FGCTImageFrameType::E_PixelPres_Monochrome).good());
    OFCHECK(fg.setVolumetricProperties(FGCTImageFrameType::E_VolProp_Volume).good());
    OFCHECK(fg.setVolumeBasedCalculationTechnique(FGCTImageFrameType::DT_VolBasedCalcTechnique_VolumeRender).good());

    // Check data structure in memory
    check_ct_image_frame_type_fg(fg);

    // Write to DcmItem and compare with pre-defined template
    DcmItem dest_item;
    OFCondition result = fg.write(dest_item);
    OFCHECK(result.good());
    OFStringStream out;
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str()); /**/

    // Test read method: Read from dataset, write again, and compare another time
    FGCTImageFrameType fg_for_read;
    out.str(""); // set to empty
    fg_for_read.read(dest_item);
    dest_item.clear();
    result = fg_for_read.write(dest_item);
    OFCHECK(result.good());
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str());

    // Test compare() method
    OFCHECK(fg.compare(fg_for_read) == 0);
    OFCHECK(fg_for_read.setVolumetricProperties(FGCTImageFrameType::E_VolProp_Distorted).good());
    OFCHECK(fg.compare(fg_for_read) != 0);

    // Test clone() method
    FGCTImageFrameType* clone = OFstatic_cast(FGCTImageFrameType*, fg.clone());
    OFCHECK(clone != NULL);
    OFCHECK(clone->compare(fg) == 0);
    delete clone;
}
