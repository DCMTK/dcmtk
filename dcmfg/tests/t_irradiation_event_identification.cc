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
 *  Purpose: Tests for Irradiation Event Identification FG class
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmfg/fgirradiationeventid.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdict.h"

static void init_template(OFString& fg_dump)
{
    fg_dump += "(fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    fg_dump
        += "(0018,9477) SQ (Sequence with explicit length #=1)      #   0, 1 IrradiationEventIdentificationSequence\n";
    fg_dump += "  (fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    fg_dump += "    (0008,3010) UI [1.2.3.4]                                #   8, 1 IrradiationEventUID\n";
    fg_dump += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    fg_dump += "(fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
}

static void check_irradiation_event_identification_fg(FGIrradiationEventIdentification& fg)
{
    OFCondition result;
    OFString val;
    OFCHECK(fg.getIrradiationEventUID(val).good());
    OFCHECK(val == "1.2.3.4");
}

OFTEST(dcmfg_irradiation_event_identification)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    OFString fg_dump;
    init_template(fg_dump);

    FGIrradiationEventIdentification fg;
    OFCHECK(fg.setIrradiationEventUID("1.2.3.4").good());

    // Check data structure in memory
    check_irradiation_event_identification_fg(fg);

    // Write to DcmItem and compare with pre-defined template
    DcmItem dest_item;
    OFCondition result = fg.write(dest_item);
    OFCHECK(result.good());
    OFStringStream out;
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str()); /**/

    // Test read method: Read from dataset, write again, and compare another time
    FGIrradiationEventIdentification fg_for_read;
    out.str(""); // set to empty
    fg_for_read.read(dest_item);
    dest_item.clear();
    result = fg_for_read.write(dest_item);
    OFCHECK(result.good());
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str());

    // Test compare() method
    OFCHECK(fg.compare(fg_for_read) == 0);
    fg_for_read.setIrradiationEventUID("5.6.7.8");
    OFCHECK(fg.compare(fg_for_read) != 0);

    // Test clone() method
    FGIrradiationEventIdentification* clone = OFstatic_cast(FGIrradiationEventIdentification*, fg.clone());
    OFCHECK(clone != NULL);
    OFCHECK(clone->compare(fg) == 0);
    delete clone;
}
