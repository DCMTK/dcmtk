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
 *  Purpose: Tests for CT Table Dynamics FG class
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmfg/fgcttabledynamics.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdict.h"

static void init_template(OFString& fg_dump)
{
    fg_dump += "(fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    fg_dump += "(0018,9308) SQ (Sequence with explicit length #=2)      #   0, 1 CTTableDynamicsSequence\n";
    fg_dump += "  (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    fg_dump += "    (0018,9309) FD 1                                        #   8, 1 TableSpeed\n";
    fg_dump += "    (0018,9310) FD 2                                        #   8, 1 TableFeedPerRotation\n";
    fg_dump += "    (0018,9311) FD 3                                        #   8, 1 SpiralPitchFactor\n";
    fg_dump += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "  (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    fg_dump += "    (0018,9309) FD 4                                        #   8, 1 TableSpeed\n";
    fg_dump += "    (0018,9310) FD 5                                        #   8, 1 TableFeedPerRotation\n";
    fg_dump += "    (0018,9311) FD 6                                        #   8, 1 SpiralPitchFactor\n";
    fg_dump += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    fg_dump += "(fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
}

static void check_ct_table_dynamics_fg(FGCTTableDynamics& fg)
{
    OFCondition result;
    OFString val;
    Float64 fl64;
    OFVector<FGCTTableDynamics::FGCTTableDynamicsItem*>::iterator item = fg.getCTTableDynamicsItems().begin();
    OFCHECK((*item)->getTableSpeed(fl64).good());
    OFCHECK(fl64 == 1.0);
    OFCHECK((*item)->getTableSpeed(val).good());
    OFCHECK(val == "1");
    OFCHECK((*item)->getTableFeedPerRotation(fl64).good());
    OFCHECK(fl64 == 2.0);
    OFCHECK((*item)->getTableFeedPerRotation(val).good());
    OFCHECK(val == "2");
    OFCHECK((*item)->getSpiralPitchFactor(fl64).good());
    OFCHECK(fl64 == 3.0);
    OFCHECK((*item)->getSpiralPitchFactor(val).good());
    OFCHECK(val == "3");

    item++;
    OFCHECK((*item)->getTableSpeed(fl64).good());
    OFCHECK(fl64 == 4.0);
    OFCHECK((*item)->getTableSpeed(val).good());
    OFCHECK(val == "4");
    OFCHECK((*item)->getTableFeedPerRotation(fl64).good());
    OFCHECK(fl64 == 5.0);
    OFCHECK((*item)->getTableFeedPerRotation(val).good());
    OFCHECK(val == "5");
    OFCHECK((*item)->getSpiralPitchFactor(fl64).good());
    OFCHECK(fl64 == 6.0);
    OFCHECK((*item)->getSpiralPitchFactor(val).good());
    OFCHECK(val == "6");
}

OFTEST(dcmfg_ct_table_dynamics)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    OFString fg_dump;
    init_template(fg_dump);

    FGCTTableDynamics fg;
    FGCTTableDynamics::FGCTTableDynamicsItem* fgi1 = new FGCTTableDynamics::FGCTTableDynamicsItem;
    OFCHECK(fgi1->setTableSpeed(1.0).good());
    OFCHECK(fgi1->setTableFeedPerRotation(2.0).good());
    OFCHECK(fgi1->setSpiralPitchFactor(3.0).good());
    FGCTTableDynamics::FGCTTableDynamicsItem* fgi2 = new FGCTTableDynamics::FGCTTableDynamicsItem;
    OFCHECK(fgi2->setTableSpeed(4.0).good());
    OFCHECK(fgi2->setTableFeedPerRotation(5.0).good());
    OFCHECK(fgi2->setSpiralPitchFactor(6.0).good());
    fg.getCTTableDynamicsItems().push_back(fgi1);
    fg.getCTTableDynamicsItems().push_back(fgi2);

    // Check data structure in memory
    check_ct_table_dynamics_fg(fg);

    // Write to DcmItem and compare with pre-defined template
    DcmItem dest_item;
    OFCondition result = fg.write(dest_item);
    OFCHECK(result.good());
    OFStringStream out;
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str()); /**/

    // Test read method: Read from dataset, write again, and compare another time
    FGCTTableDynamics fg_for_read;
    out.str(""); // set to empty
    fg_for_read.read(dest_item);
    dest_item.clear();
    result = fg_for_read.write(dest_item);
    OFCHECK(result.good());
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str());

    // Test compare() method
    OFCHECK(fg.compare(fg_for_read) == 0);
    OFCHECK(fgi2->setTableSpeed(100.0).good());
    OFCHECK(fg.compare(fg_for_read) != 0);

    // Test clone() method
    FGCTTableDynamics* clone = OFstatic_cast(FGCTTableDynamics*, fg.clone());
    OFCHECK(clone != NULL);
    OFCHECK(clone->compare(fg) == 0);
    delete clone;
}
