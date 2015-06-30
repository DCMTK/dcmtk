/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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
 *    test program for classes DSRRootTemplate and DSRSubTemplate
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmsr/dsrrtpl.h"
#include "dcmtk/dcmsr/dsrstpl.h"


/* minimal test class for root templates (Basic Diagnostic Imaging Report) */
class SRTestTemplate2000
  : public DSRRootTemplate
{

  public:

    SRTestTemplate2000()
      : DSRRootTemplate(DSRTypes::DT_BasicTextSR, "2000", "DCMR", UID_DICOMContentMappingResource)
    {
        /* make sure that at least the root CONTAINER is there */
        addContentItem(RT_isRoot, VT_Container);
    }
};

/* minimal test class for sub-templates (Person Observer Identifying Attributes) */
class SRTestTemplate1003
  : public DSRSubTemplate
{

  public:

    SRTestTemplate1003()
      : DSRSubTemplate("1003", "DCMR")
    {
        setExtensible();
        /* make sure that at least the PNAME content item is there */
        addContentItem(RT_unknown, VT_PName, DSRCodedEntryValue("121008", "DCM", "Person Observer Name"));
    }
};

/* minimal test class for sub-templates (Planar ROI Measurements) */
class SRTestTemplate1410
  : public DSRSubTemplate
{

  public:

    SRTestTemplate1410()
      : DSRSubTemplate("1410", "DCMR")
    {
        setExtensible();
        /* make sure that at least the "root" CONTAINER is there */
        addContentItem(RT_unknown, VT_Container, DSRCodedEntryValue("125007", "DCM", "Measurement Group"));
    }
};


OFTEST(dcmsr_rootTemplate)
{
    /* first, create an empty SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    /* then, create an almost empty "Basic Diagnostic Imaging Report" (TID 2000) */
    SRTestTemplate2000 templ;
    /* perform some basic checks */
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_ComprehensiveSR);
    OFCHECK_EQUAL(doc.getTree().countNodes(), 0);
    OFCHECK_EQUAL(templ.getDocumentType(), DSRTypes::DT_BasicTextSR);
    OFCHECK_EQUAL(templ.countNodes(), 1);
    OFCHECK_EQUAL(templ.getTemplateIdentifier(), "2000");
    OFCHECK_EQUAL(templ.getMappingResource(), "DCMR");
    OFCHECK_EQUAL(templ.getMappingResourceUID(), UID_DICOMContentMappingResource);
    OFCHECK(templ.isRootTemplate());
    OFCHECK(!templ.isExtensible());
    /* replace the document tree with the content of the template */
    OFCHECK(doc.setTreeFromRootTemplate(templ).good());
    /* and perform some further checks */
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_BasicTextSR);
    OFCHECK_EQUAL(doc.getTree().countNodes(), 1);
    OFString templID, mapResource;
    OFCHECK(doc.getTree().getTemplateIdentification(templID, mapResource).good());
    OFCHECK_EQUAL(templID, "2000");
    OFCHECK_EQUAL(mapResource, "DCMR");
}


OFTEST(dcmsr_subTemplate_1)
{
    /* first, create an empty SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    /* then, create an almost empty "Planar ROI Measurements" (TID 1410) */
    SRTestTemplate1410 templ;
    /* and, an empty SR document tree */
    DSRDocumentTree tree(DSRTypes::DT_EnhancedSR);
    /* perform some basic checks */
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_ComprehensiveSR);
    OFCHECK_EQUAL(doc.getTree().countNodes(), 0);
    OFCHECK_EQUAL(tree.getDocumentType(), DSRTypes::DT_EnhancedSR);
    OFCHECK_EQUAL(tree.countNodes(), 0);
    OFCHECK_EQUAL(templ.countNodes(), 1);
    OFCHECK_EQUAL(templ.getTemplateIdentifier(), "1410");
    OFCHECK_EQUAL(templ.getMappingResource(), "DCMR");
    OFCHECK_EQUAL(templ.getMappingResourceUID(), "");
    OFCHECK(!templ.isRootTemplate());
    OFCHECK(templ.isExtensible());
    /* insert TID 1410 into the tree ... */
    OFCHECK(tree.insertSubTree(templ.cloneTree(), DSRTypes::AM_belowCurrent, DSRTypes::RT_isRoot).good());
    /* and replace the tree of the SR document with the content of the template */
    OFCHECK(doc.setTree(tree).good());
    /* finally, perform some further checks */
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_EnhancedSR);
    OFCHECK_EQUAL(doc.getTree().countNodes(), 1);
    OFString templID, mapResource;
    OFCHECK(doc.getTree().getTemplateIdentification(templID, mapResource).good());
    OFCHECK_EQUAL(templID, "1410");
    OFCHECK_EQUAL(mapResource, "DCMR");
}


OFTEST(dcmsr_subTemplate_2)
{
    /* first, create an empty SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    /* then, create an almost empty "Basic Diagnostic Imaging Report" (TID 2000) */
    SRTestTemplate2000 templ1;
    /* and make it extensible (only needed for this test) */
    templ1.setExtensible();
    /* also create an almost empty "Person Observer Identifying Attributes" (TID 1003) */
    SRTestTemplate1003 templ2;
    /* perform some basic checks */
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_ComprehensiveSR);
    OFCHECK_EQUAL(doc.getTree().countNodes(), 0);
    OFCHECK_EQUAL(templ1.getDocumentType(), DSRTypes::DT_BasicTextSR);
    OFCHECK_EQUAL(templ1.countNodes(), 1);
    OFCHECK_EQUAL(templ2.countNodes(), 1);
    /* insert TID 1003 into TID 2000 */
    OFCHECK(templ1.insertTemplate(templ2, DSRTypes::AM_belowCurrent, DSRTypes::RT_hasAcqContext).good());
    OFCHECK_EQUAL(templ1.countNodes(), 2);
    OFCHECK_EQUAL(templ2.countNodes(), 1);
    /* replace the document tree with the content of the template */
    OFCHECK(doc.setTreeFromRootTemplate(templ1).good());
    /* and perform some further checks */
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_BasicTextSR);
    OFCHECK_EQUAL(doc.getTree().countNodes(), 2);
    OFString templID, mapResource;
    OFCHECK(doc.getTree().getTemplateIdentification(templID, mapResource).good());
    OFCHECK_EQUAL(templID, "2000");
    OFCHECK_EQUAL(mapResource, "DCMR");
}
