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
 *  Purpose: Tests for CT Position FG class
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmfg/fgctposition.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdict.h"

static void init_template(OFString& fg_dump)
{
    fg_dump += "(fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    fg_dump += "(0018,9326) SQ (Sequence with explicit length #=1)      #   0, 1 CTPositionSequence\n";
    fg_dump += "  (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    fg_dump += "    (0018,9313) FD 1\\2\\3                                    #  24, 3 DataCollectionCenterPatient\n";
    fg_dump
        += "    (0018,9318) FD 4\\5\\6                                    #  24, 3 ReconstructionTargetCenterPatient\n";
    fg_dump += "    (0018,9327) FD 1                                        #   8, 1 TablePosition\n";
    fg_dump += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    fg_dump += "(fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
}

static void check_ct_position_fg(FGCTPosition& fg)
{
    OFCondition result;
    OFString val;
    OFVector<Float64> vfl64;
    Float64 fl64;
    OFCHECK(fg.getTablePosition(fl64).good());
    OFCHECK(fl64 == 1.0);
    OFCHECK(fg.getDataCollectionCenterPatient(vfl64).good());
    OFCHECK(vfl64.size() == 3);
    OFCHECK(vfl64[0] == 1.0);
    OFCHECK(vfl64[1] == 2.0);
    OFCHECK(vfl64[2] == 3.0);
    OFCHECK(fg.getDataCollectionCenterPatient(val, -1).good());
    OFCHECK(val == "1\\2\\3");

    vfl64.clear();
    OFCHECK(fg.getReconstructionTargetCenterPatient(vfl64).good());
    OFCHECK(vfl64.size() == 3);
    OFCHECK(vfl64[0] == 4.0);
    OFCHECK(vfl64[1] == 5.0);
    OFCHECK(vfl64[2] == 6.0);
    OFCHECK(fg.getReconstructionTargetCenterPatient(val, -1).good());
    OFCHECK(val == "4\\5\\6");
}

OFTEST(dcmfg_ct_position)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    OFString fg_dump;
    init_template(fg_dump);

    FGCTPosition fg;
    OFCHECK(fg.setTablePosition(1.0).good());
    OFVector<Float64> vfl64;
    vfl64.push_back(1.0);
    vfl64.push_back(2.0);
    vfl64.push_back(3.0);
    OFCHECK(fg.setDataCollectionCenterPatient(vfl64).good());
    vfl64.clear();
    vfl64.push_back(4.0);
    vfl64.push_back(5.0);
    vfl64.push_back(6.0);
    OFCHECK(fg.setReconstructionTargetCenterPatient(vfl64).good());

    // Check data structure in memory
    check_ct_position_fg(fg);

    // Write to DcmItem and compare with pre-defined template
    DcmItem dest_item;
    OFCondition result = fg.write(dest_item);
    OFCHECK(result.good());
    OFStringStream out;
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str()); /**/

    // Test read method: Read from dataset, write again, and compare another time
    FGCTPosition fg_for_read;
    out.str(""); // set to empty
    fg_for_read.read(dest_item);
    dest_item.clear();
    result = fg_for_read.write(dest_item);
    OFCHECK(result.good());
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str());

    // Test compare() method
    OFCHECK(fg.compare(fg_for_read) == 0);
    OFCHECK(fg.setTablePosition(100.0).good());
    OFCHECK(fg.compare(fg_for_read) != 0);

    // Test clone() method
    FGCTPosition* clone = OFstatic_cast(FGCTPosition*, fg.clone());
    OFCHECK(clone != NULL);
    OFCHECK(clone->compare(fg) == 0);
    delete clone;
}
