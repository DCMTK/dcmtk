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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    test program for class DSRDocument
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmsr/dsrdoc.h"


OFTEST(dcmsr_changeDocumentType)
{
    /* first, create an SR document to get an empty SR tree */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* try to change the type an the empty document */
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_EnhancedSR).good());
    /* then add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addChildContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    /* and, try to change the document type again */
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_BasicTextSR).bad());
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_EnhancedSR).good());
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_ComprehensiveSR).good());
}
