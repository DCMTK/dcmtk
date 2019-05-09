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
 *  Purpose: Tests for Derivation Image FG class
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/dcmfg/fgderimg.h"
#include "dcmtk/dcmfg/fginterface.h"
#include "dcmtk/ofstd/oftest.h"

static void init_template(OFString& fg_dump)
{
    fg_dump = "(fffe,e000) na (Item with explicit length #=1)          #   0, 1 Item\n";
    fg_dump += "(0008,9124) SQ (Sequence with explicit length #=1)      #   0, 1 DerivationImageSequence\n";
    fg_dump += "  (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    fg_dump += "    (0008,2111) ST [Some Description]                       #  16, 1 DerivationDescription\n";
    fg_dump += "    (0008,2112) SQ (Sequence with explicit length #=1)      #   0, 1 SourceImageSequence\n";
    fg_dump += "      (fffe,e000) na (Item with explicit length #=5)          #   0, 1 Item\n";
    fg_dump += "        (0008,1150) UI =CTImageStorage                          #  26, 1 ReferencedSOPClassUID\n";
    fg_dump += "        (0008,1155) UI [1.2.3.4]                                #   8, 1 ReferencedSOPInstanceUID\n";
    fg_dump += "        (0008,1160) IS [1\\2]                                    #   4, 2 ReferencedFrameNumber\n";
    fg_dump
        += "        (0040,a170) SQ (Sequence with explicit length #=1)      #   0, 1 PurposeOfReferenceCodeSequence\n";
    fg_dump += "          (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    fg_dump += "            (0008,0100) SH [PURPOSE CODE]                           #  12, 1 CodeValue\n";
    fg_dump += "            (0008,0102) SH [99DCMFG]                                #   8, 1 CodingSchemeDesignator\n";
    fg_dump += "            (0008,0104) LO [Code Meaning Purpose]                   #  20, 1 CodeMeaning\n";
    fg_dump += "          (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "        (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    fg_dump += "        (0062,000b) US 3\\4                                      #   4, 2 ReferencedSegmentNumber\n";
    fg_dump += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    fg_dump += "    (0008,9215) SQ (Sequence with explicit length #=1)      #   0, 1 DerivationCodeSequence\n";
    fg_dump += "      (fffe,e000) na (Item with explicit length #=3)          #   0, 1 Item\n";
    fg_dump += "        (0008,0100) SH [CODE_VALUE]                             #  10, 1 CodeValue\n";
    fg_dump += "        (0008,0102) SH [99DCMFG]                                #   8, 1 CodingSchemeDesignator\n";
    fg_dump += "        (0008,0104) LO [Code Meaning Derivation Description]    #  36, 1 CodeMeaning\n";
    fg_dump += "      (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "    (fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    fg_dump += "  (fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
    fg_dump += "(fffe,e0dd) na (SequenceDelimitationItem for re-encod.) #   0, 0 SequenceDelimitationItem\n";
    fg_dump += "(fffe,e00d) na (ItemDelimitationItem for re-encoding)   #   0, 0 ItemDelimitationItem\n";
}

static void check_deriv_image_fg(FGDerivationImage& fg)
{
    OFVector<DerivationImageItem*> deriv_img_items = fg.getDerivationImageItems();
    OFCHECK(deriv_img_items.size() == 1);
    OFVector<CodeSequenceMacro*>& deriv_code_items = deriv_img_items[0]->getDerivationCodeItems();
    OFCHECK(deriv_code_items.size() == 1);
    CodeSequenceMacro* code_item = deriv_code_items[0];
    OFString str;
    code_item->getCodeValue(str);
    OFCHECK(str == "CODE_VALUE");
    code_item->getCodingSchemeDesignator(str);
    OFCHECK(str == "99DCMFG");
    code_item->getCodeMeaning(str);
    OFCHECK(str == "Code Meaning Derivation Description");

    DerivationImageItem* deriv_item = deriv_img_items[0];
    OFCHECK(deriv_item->getSourceImageItems().size() == 1);
    OFVector<Uint16> numbers;
    deriv_item->getSourceImageItems()[0]->getImageSOPInstanceReference().getReferencedFrameNumber(numbers);
    OFCHECK(numbers.size() == 2);
    OFCHECK(numbers[0] == 1);
    OFCHECK(numbers[1] == 2);
    numbers.clear();
    deriv_item->getSourceImageItems()[0]->getImageSOPInstanceReference().getReferencedSegmentNumber(numbers);
    OFCHECK(numbers.size() == 2);
    OFCHECK(numbers[0] == 3);
    OFCHECK(numbers[1] == 4);
    deriv_item->getSourceImageItems()[0]->getImageSOPInstanceReference().getReferencedSOPClassUID(str);
    OFCHECK(str == UID_CTImageStorage);
    deriv_item->getSourceImageItems()[0]->getImageSOPInstanceReference().getReferencedSOPInstanceUID(str);
    OFCHECK(str == "1.2.3.4");
    CodeSequenceMacro& code = deriv_item->getSourceImageItems()[0]->getPurposeOfReferenceCode();
    code.getCodeValue(str);
    OFCHECK(str == "PURPOSE CODE");
    code.getCodingSchemeDesignator(str);
    OFCHECK(str == "99DCMFG");
    code.getCodeMeaning(str);
    OFCHECK(str == "Code Meaning Purpose");
}

OFTEST(dcmfg_derivation_image)
{
    OFString fg_dump;
    init_template(fg_dump);

    FGDerivationImage fg;
    CodeSequenceMacro deriv_code("CODE_VALUE", "99DCMFG", "Code Meaning Derivation Description");
    DerivationImageItem* deriv_item = NULL;
    OFCondition result              = fg.addDerivationImageItem(deriv_code, "Some Description", deriv_item);
    OFCHECK(result.good());
    OFCHECK(deriv_item != NULL);

    SourceImageItem* src_image_item = new SourceImageItem();
    OFCHECK(src_image_item->getImageSOPInstanceReference().addReferencedFrameNumber(1).good());
    OFCHECK(src_image_item->getImageSOPInstanceReference().addReferencedFrameNumber(2).good());
    OFCHECK(src_image_item->getImageSOPInstanceReference().addReferencedSegmentNumber(3).good());
    OFCHECK(src_image_item->getImageSOPInstanceReference().addReferencedSegmentNumber(4).good());
    OFCHECK(src_image_item->getImageSOPInstanceReference().setReferencedSOPInstanceUID("1.2.3.4").good());
    OFCHECK(src_image_item->getImageSOPInstanceReference().setReferencedSOPClassUID(UID_CTImageStorage).good());
    OFCHECK(src_image_item->getPurposeOfReferenceCode().set("PURPOSE CODE", "99DCMFG", "Code Meaning Purpose").good());

    OFVector<SourceImageItem*>& src_image_items = deriv_item->getSourceImageItems();
    src_image_items.push_back(src_image_item);

    // Check data structure in memory
    check_deriv_image_fg(fg);

    // Write to DcmItem and compare with pre-defined template
    DcmItem dest_item;
    result = fg.write(dest_item);
    OFCHECK(result.good());
    OFStringStream out;
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str());

    // Test read method: Read from dataset, write again, and compare another time
    FGDerivationImage fg_for_read;
    out.str(""); // set to empty
    fg_for_read.read(dest_item);
    dest_item.clear();
    result = fg_for_read.write(dest_item);
    OFCHECK(result.good());
    dest_item.print(out);
    OFCHECK(out.str() == fg_dump.c_str());

    // Test compare() method
    OFCHECK(fg.compare(fg_for_read) == 0);
    fg_for_read.getDerivationImageItems()[0]->setDerivationDescription("Another Description");
    OFCHECK(fg.compare(fg_for_read) != 0);

    // Test clone() method
    FGDerivationImage* clone = OFstatic_cast(FGDerivationImage*, fg.clone());
    OFCHECK(clone != NULL);
    OFCHECK(clone->compare(fg) == 0);
    delete clone;
}
