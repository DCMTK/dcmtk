/*
 *
 *  Copyright (C) 2014, OFFIS e.V.
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for class DSRDocumentTree and DSRDocumentTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmsr/dsrnumtn.h"


OFTEST(dcmsr_addContentItem)
{
    /* first create an SR document to get an empty SR tree */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then try to add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    /* add content item with given pointer */
    DSRNumTreeNode *numNode = new DSRNumTreeNode(DSRTypes::RT_contains);
    OFCHECK_EQUAL(tree.addContentItem(numNode, DSRTypes::AM_belowCurrent), numNode);
    if (numNode != NULL)
    {
        OFCHECK(numNode->getRelationshipType() == DSRTypes::RT_contains);
        OFCHECK(numNode->getValueType() == DSRTypes::VT_Num);
        OFCHECK(numNode->setConceptName(DSRCodedEntryValue("121206", "DCM", "Distance")).good());
        OFCHECK(numNode->setValue("1.5", DSRCodedEntryValue("cm", "UCUM", "centimeter")).good());
        OFCHECK(numNode->setRationalRepresentation(3, 2).good());
        OFCHECK(numNode->setFloatingPointRepresentation(1.5).good());
        /* the following code usually implies "Measurement not available", but this is only a test */
        OFCHECK(numNode->setNumericValueQualifier(DSRCodedEntryValue("114006", "DCM", "Measurement failure")).good());
    }
    /* try to add content items that should fail */
    OFCHECK(tree.addContentItem(NULL) == NULL);
    OFCHECK(tree.addContentItem(DSRTypes::createDocumentTreeNode(DSRTypes::RT_hasProperties, DSRTypes::VT_Text), DSRTypes::AM_afterCurrent, OFTrue /*deleteIfFail*/) == NULL);
    /* NB: this test program does not always delete allocated memory (if adding a node fails) */
}
