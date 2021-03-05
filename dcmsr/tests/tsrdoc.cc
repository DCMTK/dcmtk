/*
 *
 *  Copyright (C) 2014-2021, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
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


OFTEST(dcmsr_setAndGetPatientData)
{
    DSRDocument doc;
    OFString value;
    /* check whether the values are set correctly */
    OFCHECK(doc.setPatientName("Dow^John").good());
    OFCHECK(doc.getPatientName(value).good());
    OFCHECK_EQUAL(value, "Dow^John");
    OFCHECK(doc.setPatientBirthDate("19600707").good());
    OFCHECK(doc.getPatientBirthDate(value).good());
    OFCHECK_EQUAL(value, "19600707");
    OFCHECK(doc.setPatientID("0815").good());
    OFCHECK(doc.getPatientID(value).good());
    OFCHECK_EQUAL(value, "0815");
    OFCHECK(doc.setIssuerOfPatientID("The Assigning Authority").good());
    OFCHECK(doc.getIssuerOfPatientID(value).good());
    OFCHECK_EQUAL(value, "The Assigning Authority");
    OFCHECK(doc.setPatientSex("M").good());
    OFCHECK(doc.getPatientSex(value).good());
    OFCHECK_EQUAL(value, "M");
    /* also check some recently introduced attributes */
    OFCHECK(doc.setPatientSize("1.88").good());
    OFCHECK(doc.getPatientSize(value).good());
    OFCHECK_EQUAL(value, "1.88");
    OFCHECK(doc.setPatientWeight("80").good());
    OFCHECK(doc.getPatientWeight(value).good());
    OFCHECK_EQUAL(value, "80");
}


OFTEST(dcmsr_setSpecificCharacterSet)
{
    /* first, create an SR document and set the character set */
    DSRDocument doc;
    OFCHECK_EQUAL(doc.getSpecificCharacterSetType(), DSRTypes::CS_ASCII);
    OFCHECK(doc.setSpecificCharacterSet("ISO_IR 100").good());
    /* then pass a value with special or invalid characters to a setXXX() method */
    OFCHECK(doc.setPatientName("Riesmeier^J\366rg", OFTrue /*check*/).good());
    OFCHECK(doc.setPatientName("^^^^^^", OFTrue /*check*/).bad());
    OFCHECK(doc.setPatientName("^^^^^^", OFFalse /*check*/).good());
    OFCHECK(doc.setStudyDescription("not allowed: \n\010\r\014", OFTrue /*check*/).bad());
    OFCHECK(doc.setStudyDescription("not allowed: \n\010\r\014", OFFalse /*check*/).good());
    /* disable VR checker by setting and invalid character set and try again */
    OFCHECK(doc.setSpecificCharacterSet("UNKNOWN").good());
    OFCHECK_EQUAL(doc.getSpecificCharacterSetType(), DSRTypes::CS_invalid);
    OFCHECK(doc.setPatientName("^^^^^^", OFTrue /*check*/).good());
    OFCHECK(doc.setStudyDescription("not allowed: \n\010\r\014", OFTrue /*check*/).good());
    /* set default character repertoire (ASCII) and try again */
    OFCHECK(doc.setSpecificCharacterSet("").good());
    OFCHECK_EQUAL(doc.getSpecificCharacterSetType(), DSRTypes::CS_ASCII);
    OFCHECK(doc.setPatientName("Riesmeier^J\366rg", OFTrue /*check*/).bad());
    OFCHECK(doc.setSpecificCharacterSet("ISO_IR 6").good());
    OFCHECK_EQUAL(doc.getSpecificCharacterSetType(), DSRTypes::CS_ASCII);
}


OFTEST(dcmsr_changeDocumentType_1)
{
    /* first, create an SR document to get an empty SR tree */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* try to change the type an the empty document */
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_EnhancedSR).good());
    /* then, add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addChildContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    /* and, try to change the document type again */
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_BasicTextSR).bad());
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_EnhancedSR).good());
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_ComprehensiveSR).good());
}


OFTEST(dcmsr_changeDocumentType_2)
{
    /* first, create an SR document to get an empty SR tree */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then, add some content items (incl. by-reference relationship) */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.getCurrentContentItem().setConceptName(DSRCodedEntryValue("121111", "DCM", "Summary")).good());
    const size_t refTarget = tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Num, DSRTypes::AM_belowCurrent);
    OFCHECK(refTarget > 0);
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_afterCurrent));
    const size_t refSource = tree.addByReferenceRelationship(DSRTypes::RT_inferredFrom, refTarget);
    OFCHECK(refSource > 0);
    OFCHECK(tree.gotoRoot());
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    /* and, try to change the document type */
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_BasicTextSR).bad());
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_EnhancedSR).bad());
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_XRayRadiationDoseSR).bad());
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_Comprehensive3DSR).good());
    OFCHECK(doc.changeDocumentType(DSRTypes::DT_ComprehensiveSR).good());
}


OFTEST(dcmsr_setDocumentTree)
{
    /* first, create an SR document to get an empty SR tree */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    DSRDocumentTree &tree = doc.getTree();
    /* then add some content items */
    OFCHECK(tree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRTypes::AM_afterCurrent));
    OFCHECK(tree.getCurrentContentItem().setConceptName(DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent));
    OFCHECK(tree.addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_afterCurrent));
    OFCHECK_EQUAL(tree.countNodes(), 5);
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_ComprehensiveSR);
    /* create another SR tree (with less content items) */
    DSRDocumentTree newTree(DSRTypes::DT_EnhancedSR);
    OFCHECK(newTree.addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container, DSRCodedEntryValue("121111", "DCM", "Summary")).good());
    OFCHECK(newTree.addChildContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num, DSRCodedEntryValue("121206", "DCM", "Distance")).good());
    OFCHECK(newTree.addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRCodedEntryValue("1234", "99_PRV", "NOS")).good());
    OFCHECK_EQUAL(newTree.countNodes(), 3);
    OFCHECK_EQUAL(newTree.getDocumentType(), DSRTypes::DT_EnhancedSR);
    /* and, replace the SR tree of the SR document */
    OFCHECK(doc.setTree(newTree).good());
    OFCHECK_EQUAL(doc.getTree().countNodes(), 3);
    OFCHECK_EQUAL(newTree.countNodes(), 3);
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_EnhancedSR);
}
