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

#include "dcmtk/dcmfg/fgctacquisitiondetails.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcdict.h"

static void checkDump(const OFString& ds_dump, const OFString& expected_dump);

static void init_template(OFString& fg_dump)
{
    fg_dump += "(fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    fg_dump += "(0018,9304) SQ (Sequence with explicit length #=2)      #   0, 1 CTAcquisitionDetailsSequence\n";
    fg_dump += "  (fffe,e000) na (Item with explicit length #=8)          #   0, 1 Item\n";
    fg_dump += "    (0018,0090) DS [6]                                      #   2, 1 DataCollectionDiameter\n";
    fg_dump += "    (0018,1120) DS [5]                                      #   2, 1 GantryDetectorTilt\n";
    fg_dump += "    (0018,1130) DS [4]                                      #   2, 1 TableHeight\n";
    fg_dump += "    (0018,1140) CS [CC]                                     #   2, 1 RotationDirection\n";
    fg_dump += "    (0018,9305) FD 1                                        #   8, 1 RevolutionTime\n";
    fg_dump += "    (0018,9306) FD 3                                        #   8, 1 SingleCollimationWidth\n";
    fg_dump += "    (0018,9307) FD 2                                        #   8, 1 TotalCollimationWidth\n";
    fg_dump += "    (0018,9378) US 1\\2\\3                                    #   6, 3 ReferencedPathIndex\n";
    fg_dump += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "  (fffe,e000) na (Item with explicit length #=8)          #   0, 1 Item\n";
    fg_dump += "    (0018,0090) DS [12]                                     #   2, 1 DataCollectionDiameter\n";
    fg_dump += "    (0018,1120) DS [11]                                     #   2, 1 GantryDetectorTilt\n";
    fg_dump += "    (0018,1130) DS [10]                                     #   2, 1 TableHeight\n";
    fg_dump += "    (0018,1140) CS [CW]                                     #   2, 1 RotationDirection\n";
    fg_dump += "    (0018,9305) FD 7                                        #   8, 1 RevolutionTime\n";
    fg_dump += "    (0018,9306) FD 8                                        #   8, 1 SingleCollimationWidth\n";
    fg_dump += "    (0018,9307) FD 9                                        #   8, 1 TotalCollimationWidth\n";
    fg_dump += "    (0018,9378) US 4\\5\\6                                    #   6, 3 ReferencedPathIndex\n";
    fg_dump += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    fg_dump += "(fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
}

static void check_ct_acquisition_details_fg(FGCTAcquisitionDetails& fg)
{
    OFCondition result;
    OFString val;
    OFVector<FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem*>& items = fg.getCTAcquisitionDetailsItems();
    OFCHECK(items.size() == 2);
    FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem* item1 = items[0];
    OFVector<Uint16> vuint;
    item1->getReferencedPathIndex(vuint);
    OFCHECK(vuint.size() == 3);
    OFCHECK((vuint[0] == 1) && (vuint[1] == 2) && (vuint[2] == 3));
    Uint16 u16;
    OFCHECK(item1->getReferencedPathIndex(u16, 0).good());
    OFCHECK(u16 == 1);
    OFCHECK(item1->getReferencedPathIndex(u16, 1).good());
    OFCHECK(u16 == 2);
    OFCHECK(item1->getReferencedPathIndex(u16, 2).good());
    OFCHECK(u16 == 3);
    FGCTAcquisitionDetails::E_RotationDirection rot;
    OFCHECK(item1->getRotationDirection(rot).good());
    OFCHECK(rot == FGCTAcquisitionDetails::E_RotationDirection_CC);
    Float64 f64;
    OFCHECK(item1->getRevolutionTime(f64).good());
    OFCHECK(f64 == 1.0);
    OFCHECK(item1->getTotalCollimationWidth(f64).good());
    OFCHECK(f64 == 2.0);
    OFCHECK(item1->getSingleCollimationWidth(f64).good());
    OFCHECK(f64 == 3.0);
    OFCHECK(item1->getTableHeight(f64).good());
    OFCHECK(f64 == 4.0);
    OFCHECK(item1->getGantryDetectorTilt(f64).good());
    OFCHECK(f64 == 5.0);
    OFCHECK(item1->getDataCollectionDiameter(f64).good());
    OFCHECK(f64 == 6.0);

    FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem* item2 = items[1];
    vuint.clear();
    item2->getReferencedPathIndex(vuint);
    OFCHECK(vuint.size() == 3);
    OFCHECK((vuint[0] == 4) && (vuint[1] == 5) && (vuint[2] == 6));
    OFCHECK(item2->getReferencedPathIndex(u16, 0).good());
    OFCHECK(u16 == 4);
    OFCHECK(item2->getReferencedPathIndex(u16, 1).good());
    OFCHECK(u16 == 5);
    OFCHECK(item2->getReferencedPathIndex(u16, 2).good());
    OFCHECK(u16 == 6);
    OFCHECK(item2->getRotationDirection(rot).good());
    OFCHECK(rot == FGCTAcquisitionDetails::E_RotationDirection_CW);
    OFCHECK(item2->getRotationDirection(val).good());
    OFCHECK(val == "CW");
    OFCHECK(item2->getRevolutionTime(f64).good());
    OFCHECK(f64 == 7.0);
    OFCHECK(item2->getSingleCollimationWidth(f64).good());
    OFCHECK(f64 == 8.0);
    OFCHECK(item2->getTotalCollimationWidth(f64).good());
    OFCHECK(f64 == 9.0);
    OFCHECK(item2->getTableHeight(f64).good());
    OFCHECK(f64 == 10.0);
    OFCHECK(item2->getGantryDetectorTilt(f64).good());
    OFCHECK(f64 == 11.0);
    OFCHECK(item2->getDataCollectionDiameter(f64).good());
    OFCHECK(f64 == 12.0);
}

OFTEST(dcmfg_ct_acquisition_details)
{
    // Make sure data dictionary is loaded
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFCHECK_FAIL("no data dictionary loaded, check environment variable: " DCM_DICT_ENVIRONMENT_VARIABLE);
        return;
    }

    OFString fg_dump;
    init_template(fg_dump);

    FGCTAcquisitionDetails fg;
    FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem* fgi = new FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem;
    OFVector<Uint16> vuint;
    vuint.push_back(1);
    vuint.push_back(2);
    vuint.push_back(3);
    OFCHECK(fgi->setReferencedPathIndex(vuint).good());
    OFCHECK(fgi->setRotationDirection(FGCTAcquisitionDetails::E_RotationDirection_CC).good());
    OFCHECK(fgi->setRevolutionTime(1.0).good());
    OFCHECK(fgi->setTotalCollimationWidth(2.0).good());
    OFCHECK(fgi->setSingleCollimationWidth(3.0).good());
    OFCHECK(fgi->setTableHeight(4.0).good());
    OFCHECK(fgi->setGantryDetectorTilt(5.0).good());
    OFCHECK(fgi->setDataCollectionDiameter(6.0).good());
    OFVector<FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem*>& items = fg.getCTAcquisitionDetailsItems();
    items.push_back(fgi);

    FGCTAcquisitionDetails::FGCTAcquisitionDetailsItem* fgi2 = fgi->clone();
    vuint.clear();
    vuint.push_back(4);
    vuint.push_back(5);
    vuint.push_back(6);
    OFCHECK(fgi2->setReferencedPathIndex(vuint).good());
    OFCHECK(fgi2->setRotationDirection(FGCTAcquisitionDetails::E_RotationDirection_CW).good());
    OFCHECK(fgi2->setRevolutionTime(7.0).good());
    OFCHECK(fgi2->setSingleCollimationWidth(8.0).good());
    OFCHECK(fgi2->setTotalCollimationWidth(9.0).good());
    OFCHECK(fgi2->setTableHeight(10.0).good());
    OFCHECK(fgi2->setGantryDetectorTilt(11.0).good());
    OFCHECK(fgi2->setDataCollectionDiameter(12.0).good());
    items.push_back(fgi2);

    // Check data structure in memory
    check_ct_acquisition_details_fg(fg);

    // Write to DcmItem and compare with pre-defined template
    DcmItem dest_item;
    OFCondition result = fg.write(dest_item);
    OFCHECK(result.good());
    OFStringStream out;
    dest_item.print(out);
    checkDump(out.str().c_str(), fg_dump);

    // Test read method: Read from dataset, write again, and compare another time
    FGCTAcquisitionDetails fg_for_read;
    out.str(""); // set to empty
    fg_for_read.read(dest_item);
    dest_item.clear();
    result = fg_for_read.write(dest_item);
    OFCHECK(result.good());
    dest_item.print(out);
    checkDump(out.str().c_str(), fg_dump);

    // Test compare() method
    OFCHECK(fg.compare(fg_for_read) == 0);
    OFCHECK(fgi->setTotalCollimationWidth(100.0).good());
    OFCHECK(fg.compare(fg_for_read) != 0);

    // Test clone() method
    FGCTAcquisitionDetails* clone = OFstatic_cast(FGCTAcquisitionDetails*, fg.clone());
    OFCHECK(clone != NULL);
    OFCHECK(clone->compare(fg) == 0);
    delete clone;
}

static void checkDump(const OFString& ds_dump, const OFString& expected_dump)
{
    OFBool dump_ok = (ds_dump == expected_dump);
    OFCHECK(dump_ok);
    if (!dump_ok)
    {
        CERR << "Dump produced: " << OFendl << ds_dump << OFendl;
        CERR << "------------------------------------" << OFendl;
        CERR << "Dump expected: " << OFendl << expected_dump << OFendl;
        CERR << "------------------------------------" << OFendl;
    }
}
