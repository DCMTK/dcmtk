/*
 *
 *  Copyright (C) 2015-2017, J. Riesmeier, Oldenburg, Germany
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
 *    test program for classes DSRRootTemplate and DSRSubTemplate
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"

#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmsr/dsrdocst.h"
#include "dcmtk/dcmsr/dsrrtpl.h"
#include "dcmtk/dcmsr/dsrstpl.h"
#include "dcmtk/dcmsr/dsrreftn.h"


/* minimal test class for root template (Basic Diagnostic Imaging Report) */
class SRTestTemplate2000
  : public DSRRootTemplate
{

  public:

    SRTestTemplate2000()
      : DSRRootTemplate(DT_BasicTextSR, "2000", "DCMR", UID_DICOMContentMappingResource)
    {
        /* make sure that at least the root CONTAINER is there */
        OFCHECK(addContentItem(RT_isRoot, VT_Container) > 0);
    }
};

/* minimal test class for sub-template (Person Observer Identifying Attributes) */
class SRTestTemplate1003
  : public DSRSubTemplate
{

  public:

    SRTestTemplate1003()
      : DSRSubTemplate("1003", "DCMR")
    {
        setExtensible();
        /* make sure that at least the PNAME content item is there */
        OFCHECK(addContentItem(RT_unknown, VT_PName, DSRCodedEntryValue("121008", "DCM", "Person Observer Name")).good());
    }
};

/* minimal test class for sub-template (Planar ROI Measurements) */
class SRTestTemplate1410
  : public DSRSubTemplate
{

  public:

    SRTestTemplate1410()
      : DSRSubTemplate("1410", "DCMR")
    {
        setExtensible();
        setOrderSignificant();
        /* make sure that at least the "root" CONTAINER is there */
        OFCHECK(addContentItem(RT_unknown, VT_Container, DSRCodedEntryValue("125007", "DCM", "Measurement Group")).good());
        /* ... and two mandatory child nodes */
        OFCHECK(addChildContentItem(RT_hasObsContext, VT_Text, DSRCodedEntryValue("112039", "DCM", "Tracking Identifier")).good());
        OFCHECK(addContentItem(RT_hasObsContext, VT_UIDRef, DSRCodedEntryValue("112040", "DCM", "Tracking Unique Identifier")).good());
    }
};

/* minimal test class for root template with included templates (Measurement Report) */
class SRTestTemplate1500
  : public DSRRootTemplate
{

  public:

    SRTestTemplate1500()
      : DSRRootTemplate(DT_EnhancedSR, "1500", "DCMR", UID_DICOMContentMappingResource)
    {
        /* make sure that at least the root CONTAINER is there */
        OFCHECK(addContentItem(RT_isRoot, VT_Container, DSRCodedEntryValue("126000", "DCM", "Imaging Measurement Report")).good());
        /* ... and include two sub-templates */
        OFCHECK(includeTemplate(DSRSharedSubTemplate(new SRTestTemplate1003()), AM_belowCurrent, RT_hasObsContext).good());
        OFCHECK(includeTemplate(DSRSharedSubTemplate(new SRTestTemplate1410()), AM_afterCurrent, RT_contains).good());
    }
};

/* minimal test class for template with by-reference relationship (Measurement Group) */
class SRTestTemplate1501
  : public DSRSubTemplate
{

  public:

    SRTestTemplate1501()
      : DSRSubTemplate("1501", "DCMR", UID_DICOMContentMappingResource)
    {
        setExtensible();
        /* make sure that at least the top-level CONTAINER is there */
        OFCHECK(addContentItem(RT_contains, VT_Container, DSRCodedEntryValue("125007,", "DCM", "Measurement Group")).good());
        /* ... and add two measurements, one referring to the other */
        OFCHECK(addChildContentItem(RT_contains, VT_Num, DSRCodedEntryValue("12345", "99TEST", "Some Measurement")).good());
        const size_t nodeID = getNodeID();
        OFCHECK(addContentItem(RT_contains, VT_Num, DSRCodedEntryValue("09876", "99TEST", "Some other Measurement")).good());
        OFCHECK(addByReferenceRelationship(RT_inferredFrom, nodeID) > 0);
        /* update by-reference relationships (prepare for cloning) */
        OFCHECK(updateByReferenceRelationships().good());
    }
};


/* minimal test class for included sub-template with contained by-reference relationship */
class SRTestTemplate1410with1501
  : public DSRSubTemplate
{

  public:

    SRTestTemplate1410with1501()
      : DSRSubTemplate("1410", "DCMR")
    {
        setExtensible();
        /* insert sub-template with some content items */
        OFCHECK(insertExtraTemplate(SRTestTemplate1410(), AM_belowCurrent, RT_contains).good());
        /* include sub-template with by-reference relationship */
        OFCHECK(includeTemplate(DSRSharedSubTemplate(new SRTestTemplate1501()), AM_belowCurrent, RT_contains).good());
    }
};


/* minimal test class for root template with the mandatory CONTAINER */
class SRTestRootTemplate
  : public DSRRootTemplate
{

  public:

    SRTestRootTemplate()
      : DSRRootTemplate(DT_ComprehensiveSR, "0815", "99TEST")
    {
        setExtensible();
        /* make sure that at least the root CONTAINER is there */
        OFCHECK(addContentItem(RT_isRoot, VT_Container, DSRCodedEntryValue("1234", "99TEST", "Some test code")).good());
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
    OFCHECK(!templ.isOrderSignificant());
    /* replace the document tree with the content of the template */
    OFCHECK(doc.setTreeFromRootTemplate(templ, OFFalse /*expandTree*/).good());
    /* and perform some further checks */
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_BasicTextSR);
    OFCHECK_EQUAL(doc.getTree().countNodes(), 1);
    OFCHECK(doc.getTree().compareTemplateIdentification("2000", "DCMR"));
    OFCHECK(doc.getTree().compareTemplateIdentification("2000", "DCMR", UID_DICOMContentMappingResource));
    OFCHECK(!doc.getTree().compareTemplateIdentification("200", "DCMR"));
    OFCHECK(!doc.getTree().compareTemplateIdentification("2000", "DCM"));
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
    OFCHECK_EQUAL(templ.countNodes(), 3);
    OFCHECK_EQUAL(templ.getTemplateIdentifier(), "1410");
    OFCHECK_EQUAL(templ.getMappingResource(), "DCMR");
    OFCHECK_EQUAL(templ.getMappingResourceUID(), "");
    OFCHECK(!templ.isRootTemplate());
    OFCHECK(templ.isExtensible());
    OFCHECK(templ.isOrderSignificant());
    /* insert TID 1410 into the tree ... */
    OFCHECK(tree.insertSubTree(templ.cloneTree(), DSRTypes::AM_belowCurrent, DSRTypes::RT_isRoot).good());
    /* and replace the tree of the SR document with the content of the template */
    OFCHECK(doc.setTree(tree).good());
    /* finally, perform some further checks */
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_EnhancedSR);
    OFCHECK_EQUAL(doc.getTree().countNodes(), 3);
    OFCHECK(doc.getTree().compareTemplateIdentification("1410", "DCMR"));
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
    OFCHECK(templ1.insertExtraTemplate(templ2, DSRTypes::AM_belowCurrent, DSRTypes::RT_hasAcqContext).good());
    OFCHECK_EQUAL(templ1.countNodes(), 2);
    OFCHECK_EQUAL(templ2.countNodes(), 1);
    /* replace the document tree with the content of the template */
    OFCHECK(doc.setTreeFromRootTemplate(templ1, OFFalse /*expandTree*/).good());
    /* and perform some further checks */
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_BasicTextSR);
    OFCHECK_EQUAL(doc.getTree().countNodes(), 2);
    OFCHECK(doc.getTree().compareTemplateIdentification("2000", "DCMR"));
}


OFTEST(dcmsr_subTemplate_3)
{
    /* first, create an almost empty "Planar ROI Measurements" (TID 1410) */
    SRTestTemplate1410 templ;
    /* then, add additional content items (since the template is extensible) */
    OFCHECK(templ.isExtensible());
    OFCHECK(templ.addExtraContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text).good());
    OFCHECK(templ.getCurrentContentItem().setConceptName(DSRBasicCodedEntry("121106", "DCM", "Comment")).good());
    OFCHECK(templ.getCurrentContentItem().setStringValue("Some comment").good());
    OFCHECK(templ.addExtraContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent).good());
    OFCHECK(templ.getCurrentContentItem().setConceptName(DSRBasicCodedEntry("121051", "DCM", "Equivalent Meaning of Value")).good());
    OFCHECK(templ.getCurrentContentItem().setStringValue("blabla").good());
    OFCHECK(templ.gotoParent() > 0);
    /* also try to add if template is non-extensible */
    templ.setExtensible(OFFalse);
    OFCHECK(templ.addExtraContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container) == SR_EC_NonExtensibleTemplate);
    /* finally, perform some further checks */
    OFCHECK_EQUAL(templ.countNodes(), 5);
    OFCHECK_EQUAL(templ.countChildNodes(), 1);
}


OFTEST(dcmsr_createExpandedTree)
{
    /* first, create an empty SR document */
    DSRDocument doc(DSRTypes::DT_ComprehensiveSR);
    /* then, create an almost empty "Measurement Report" (TID 1500) */
    SRTestTemplate1500 templ;
    OFCHECK_EQUAL(templ.countNodes(), 3);
    /* and set its content as the document tree (with expanded sub-templates) */
    OFCHECK(doc.getTree().isEmpty());
    OFCHECK(doc.setTreeFromRootTemplate(templ, OFTrue /*expandTree*/).good());
    OFCHECK(doc.getTree().isExpandedDocumentTree());
    OFCHECK_EQUAL(doc.getTree().countNodes(), 5);
    /* do the same without expanding the (included) sub-templates */
    OFCHECK(doc.setTreeFromRootTemplate(templ, OFFalse /*expandTree*/).good());
    OFCHECK(!doc.getTree().isExpandedDocumentTree());
    OFCHECK_EQUAL(doc.getTree().countNodes(), 3);
    /* and perform some further checks */
    OFCHECK_EQUAL(doc.getDocumentType(), DSRTypes::DT_EnhancedSR);
    OFCHECK(doc.getTree().compareTemplateIdentification("1500", "DCMR"));
}


OFTEST(dcmsr_templateWithByReferenceRelationship_1)
{
    /* first, create an almost empty "Planar ROI Measurements" (TID 1410) */
    SRTestTemplate1410 templ;
    /* insert sub-template with by-reference relationship */
    OFCHECK(templ.isExtensible());
    OFCHECK(templ.insertExtraTemplate(SRTestTemplate1501(), DSRTypes::AM_afterCurrent, DSRTypes::RT_contains).good());
    /* then, go to the source content item of the by-reference relationship */
    OFCHECK(templ.gotoNamedNode(DSRCodedEntryValue("09876", "99TEST", "Some other Measurement")) > 0);
    /* check whether the correct content item has been found */
    OFCHECK(templ.getCurrentContentItem().getValueType() == DSRTypes::VT_Num);
    /* and, finally, check whether the by-reference relationship is still valid */
    OFCHECK(templ.updateByReferenceRelationships().good());
    OFCHECK(templ.gotoChild() > 0);
    OFCHECK(templ.getCurrentContentItem().getValueType() == DSRTypes::VT_byReference);
    OFCHECK(templ.getCurrentContentItem().getReferencedNodeID() > 0);
    const DSRDocumentTreeNode *treeNode = templ.getTree().getCurrentNode();
    if (treeNode != NULL)
    {
        if (treeNode->getValueType() == DSRTypes::VT_byReference)
        {
            const DSRByReferenceTreeNode *node = OFstatic_cast(const DSRByReferenceTreeNode *, treeNode);
            OFCHECK_EQUAL(node->getReferencedContentItem(), "1.3.1");
        }
    } else
        OFCHECK_FAIL("could not get read-only access to current node");
}


OFTEST(dcmsr_templateWithByReferenceRelationship_2)
{
    DSRDocument doc;
    /* first, create a sub-template with included sub-template */
    SRTestTemplate1410with1501 subTempl;
    OFCHECK_EQUAL(subTempl.countNodes(), 4);
    OFCHECK_EQUAL(subTempl.countNodes(OFTrue /*searchIntoSubTemplates*/, OFFalse /*countIncludedTemplateNodes*/), 7);
    /* then, create a root template with a CONTAINER content item */
    SRTestRootTemplate rootTempl;
    OFCHECK_EQUAL(rootTempl.countNodes(), 1);
    /* insert the sub-template into it */
    OFCHECK(rootTempl.isExtensible());
    OFCHECK(rootTempl.insertExtraTemplate(subTempl).good());
    OFCHECK_EQUAL(rootTempl.countNodes(), 5);
    OFCHECK_EQUAL(rootTempl.countNodes(OFTrue /*searchIntoSubTemplates*/, OFFalse /*countIncludedTemplateNodes*/), 8);
    /* check whether the by-reference relationship is still valid */
    OFCHECK(rootTempl.updateByReferenceRelationships(OFTrue /*includedTemplates*/).good());
    DSRIncludedTemplateNodeCursor cursor;
    if (rootTempl.getTree().getCursorToRootNode(cursor))
    {
        do {
            const DSRDocumentTreeNode *treeNode = cursor.getNode();
            if (treeNode != NULL)
            {
                if (treeNode->getValueType() == DSRTypes::VT_byReference)
                {
                    const DSRByReferenceTreeNode *node = OFstatic_cast(const DSRByReferenceTreeNode *, treeNode);
                    OFCHECK_EQUAL(node->getReferencedContentItem(), "1.1.3.1.1");
                }
            }
        } while (cursor.iterate());
    }
    /* and, finally, set its content as the document tree */
    OFCHECK(doc.setTreeFromRootTemplate(rootTempl, OFTrue /*expandTree*/).good());
    OFCHECK_EQUAL(doc.getTree().countNodes(), 8);
}
