/*
 *
 *  Copyright (C) 2019, OFFIS e.V.
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
 *  Purpose: Tests for Frame Content FG class
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmfg/fgfracon.h"
#include "dcmtk/dcmfg/fginterface.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oftest.h"

static void init_template(OFString& fg_dump)
{
    fg_dump += "(fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    fg_dump += "(0020,9111) SQ (Sequence with explicit length #=1)      #   0, 1 FrameContentSequence\n";
    fg_dump += "  (fffe,e000) na (Item with explicit length #=12)         #   0, 1 Item\n";
    fg_dump += "    (0018,9074) DT [20190508214701]                         #  14, 1 FrameAcquisitionDateTime\n";
    fg_dump += "    (0018,9151) DT [20190508214701]                         #  14, 1 FrameReferenceDateTime\n";
    fg_dump += "    (0018,9214) CS [START_RESPIR]                           #  12, 1 RespiratoryCyclePosition\n";
    fg_dump += "    (0018,9220) FD 500.5                                    #   8, 1 FrameAcquisitionDuration\n";
    fg_dump += "    (0018,9236) CS [END_SYSTOLE]                            #  12, 1 CardiacCyclePosition\n";
    fg_dump += "    (0020,9056) SH [1]                                      #   2, 1 StackID\n";
    fg_dump += "    (0020,9057) UL 3                                        #   4, 1 InStackPositionNumber\n";
    fg_dump += "    (0020,9128) UL 4                                        #   4, 1 TemporalPositionIndex\n";
    fg_dump += "    (0020,9156) US 2                                        #   2, 1 FrameAcquisitionNumber\n";
    fg_dump += "    (0020,9157) UL 1                                        #   4, 1 DimensionIndexValues\n";
    fg_dump += "    (0020,9158) LT [Comments]                               #   8, 1 FrameComments\n";
    fg_dump += "    (0020,9453) LO [LABEL]                                  #   6, 1 FrameLabel\n";
    fg_dump += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    fg_dump += "(fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
}

static void check_frame_content_fg(FGFrameContent& fg)
{
    OFCondition result;
    OFString val;
    OFCHECK(fg.getCardiacCyclePosition(val).good());
    OFCHECK(val == "END_SYSTOLE");
    Uint32 uint32 = 0;
    OFCHECK(fg.getDimensionIndexValues(uint32).good());
    OFCHECK(uint32 == 1);
    OFCHECK(fg.getFrameAcquisitionDateTime(val).good());
    OFCHECK(val == "20190508214701");
    Float64 float64 = 0.0;
    OFCHECK(fg.getFrameAcquisitionDuration(float64).good());
    OFCHECK(float64 == 500.50);
    Uint16 uint16 = 0;
    fg.getFrameAcquisitionNumber(uint16);
    OFCHECK(fg.getFrameAcquisitionNumber(uint16).good());
    OFCHECK(uint16 == 2);
    OFCHECK(fg.getFrameComments(val).good());
    OFCHECK(val == "Comments");
    OFCHECK(fg.getFrameLabel(val).good());
    OFCHECK(val == "LABEL");
    OFCHECK(fg.getFrameReferenceDateTime(val).good());
    OFCHECK(val == "20190508214701");
    OFCHECK(fg.getInStackPositionNumber(uint32).good());
    OFCHECK(uint32 == 3);
    OFCHECK(fg.getRespiratoryCyclePosition(val).good());
    OFCHECK(val == "START_RESPIR");
    OFCHECK(fg.getStackID(val).good());
    OFCHECK(val == "1");
    OFCHECK(fg.getTemporalPositionIndex(uint32).good());
    OFCHECK(uint32 == 4);
}

OFTEST(dcmfg_frame_content)
{
    OFString fg_dump;
    init_template(fg_dump);

    FGFrameContent fg;
    OFCHECK(fg.setCardiacCyclePosition("END_SYSTOLE").good());
    OFCHECK(fg.setDimensionIndexValues(1, 0).good());
    OFCHECK(fg.setFrameAcquisitionDateTime("20190508214701").good());
    OFCHECK(fg.setFrameAcquisitionDuration(500.50).good());
    OFCHECK(fg.setFrameAcquisitionNumber(2).good());
    OFCHECK(fg.setFrameComments("Comments").good());
    OFCHECK(fg.setFrameLabel("LABEL").good());
    OFCHECK(fg.setFrameReferenceDateTime("20190508214701").good());
    OFCHECK(fg.setInStackPositionNumber(3).good());
    OFCHECK(fg.setRespiratoryCyclePosition("START_RESPIR").good());
    OFCHECK(fg.setStackID("1").good());
    OFCHECK(fg.setTemporalPositionIndex(4).good());

    // Check data structure in memory
    check_frame_content_fg(fg);

    // Write to DcmItem and compare with pre-defined template
    DcmItem dest_item;
    OFCondition result = fg.write(dest_item);
    OFCHECK(result.good());
    OFStringStream out;
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str()); /**/

    // Test read method: Read from dataset, write again, and compare another time
    FGFrameContent fg_for_read;
    out.str(""); // set to empty
    fg_for_read.read(dest_item);
    dest_item.clear();
    result = fg_for_read.write(dest_item);
    OFCHECK(result.good());
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str());

    // Test compare() method
    OFCHECK(fg.compare(fg_for_read) == 0);
    fg_for_read.setFrameComments("Another Comment");
    OFCHECK(fg.compare(fg_for_read) != 0);

    // Test clone() method
    FGFrameContent* clone = OFstatic_cast(FGFrameContent*, fg.clone());
    OFCHECK(clone != NULL);
    OFCHECK(clone->compare(fg) == 0);
    delete clone;
}
