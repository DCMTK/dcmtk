/*
 *
 *  Copyright (C) 2000-2014, OFFIS e.V.
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
 *    classes: DSRDocumentTree
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrdoctr.h"
#include "dcmtk/dcmsr/dsrcontn.h"
#include "dcmtk/dcmsr/dsrreftn.h"
#include "dcmtk/dcmsr/dsrxmld.h"
#include "dcmtk/dcmsr/dsriodcc.h"


DSRDocumentTree::DSRDocumentTree(const E_DocumentType documentType)
  : DSRDocumentSubTree(),
    DocumentType(DT_invalid)
{
    /* check & set document type, create constraint checker object */
    changeDocumentType(documentType);
}


DSRDocumentTree::DSRDocumentTree(const DSRDocumentTree &tree)
  : DSRDocumentSubTree(tree),
    DocumentType(tree.DocumentType)
{
    /* create a new constraint checker */
    ConstraintChecker = createIODConstraintChecker(DocumentType);
}


DSRDocumentTree::~DSRDocumentTree()
{
}


DSRDocumentTree *DSRDocumentTree::clone() const
{
    return new DSRDocumentTree(*this);
}


void DSRDocumentTree::clear()
{
    DSRDocumentSubTree::clear();
}


OFBool DSRDocumentTree::isValid() const
{
    OFBool result = OFFalse;
    if (isDocumentTypeSupported(DocumentType))
    {
        /* check root node */
        const DSRDocumentTreeNode *node = getRoot();
        if (node != NULL)
        {
            if ((node->getRelationshipType() == RT_isRoot) && (node->getValueType() == VT_Container))
                result = OFTrue;
        }
    }
    return result;
}


OFCondition DSRDocumentTree::read(DcmItem &dataset,
                                  const E_DocumentType documentType,
                                  const size_t flags)
{
    /* clear current document tree, check & change document type */
    OFCondition result = changeDocumentType(documentType);
    if (result.good())
    {
        if (ConstraintChecker == NULL)
            DCMSR_WARN("Check for relationship content constraints not yet supported");
        else if (ConstraintChecker->isTemplateSupportRequired())
            DCMSR_WARN("Check for template constraints not yet supported");
        if (flags & RF_showCurrentlyProcessedItem)
            DCMSR_INFO("Processing content item 1");
        /* first try to read value type */
        OFString tmpString;
        if (getAndCheckStringValueFromDataset(dataset, DCM_ValueType, tmpString, "1", "1").good() ||
            (flags & RF_ignoreContentItemErrors))
        {
            /* root node should always be a container */
            if (definedTermToValueType(tmpString) != VT_Container)
            {
                if (flags & RF_ignoreContentItemErrors)
                    DCMSR_WARN("Root content item should always be a CONTAINER");
                else {
                    DCMSR_ERROR("Root content item should always be a CONTAINER");
                    result = SR_EC_InvalidDocumentTree;
                }
            }
            if (result.good())
            {
                /* ... then create corresponding document tree node */
                DSRDocumentTreeNode *node = new DSRContainerTreeNode(RT_isRoot);
                if (node != NULL)
                {
                    /* ... insert it into the (empty) tree - checking is not required here */
                    if (addNode(node))
                    {
                        /* ... and let the node read the rest of the document */
                        result = node->read(dataset, ConstraintChecker, flags);
                        /* check and update by-reference relationships (if applicable) */
                        checkByReferenceRelationships(CM_updateNodeID, flags);
                    } else
                        result = SR_EC_InvalidDocumentTree;
                } else
                    result = EC_MemoryExhausted;
            }
        } else {
            DCMSR_ERROR("ValueType attribute for root content item is missing");
            result = SR_EC_MandatoryAttributeMissing;
        }
    }
    return result;
}


OFCondition DSRDocumentTree::readXML(const DSRXMLDocument &doc,
                                     DSRXMLCursor cursor,
                                     const size_t flags)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (ConstraintChecker == NULL)
        DCMSR_WARN("Check for relationship content constraints not yet supported");
    else if (ConstraintChecker->isTemplateSupportRequired())
        DCMSR_WARN("Check for template constraints not yet supported");
    /* we assume that 'cursor' points to the "content" element */
    if (cursor.valid())
    {
        OFString templateIdentifier, mappingResource;
        /* template identification information expected "outside" content item */
        if (flags & XF_templateElementEnclosesItems)
        {
            /* check for optional root template identification */
            const DSRXMLCursor childCursor = doc.getNamedNode(cursor, "template", OFFalse /*required*/);
            if (childCursor.valid())
            {
                doc.getStringFromAttribute(childCursor, mappingResource, "resource");
                doc.getStringFromAttribute(childCursor, templateIdentifier, "tid");
                /* get first child of the "template" element */
                cursor = childCursor.getChild();
            }
        }
        E_ValueType valueType = doc.getValueTypeFromNode(cursor);
        /* proceed to first valid container (if any) */
        while (cursor.getNext().valid() && (valueType != VT_Container))
            valueType = doc.getValueTypeFromNode(cursor.gotoNext());
        /* root node should always be a container */
        if (valueType == VT_Container)
        {
            /* ... then create corresponding document tree node */
            DSRDocumentTreeNode *node = new DSRContainerTreeNode(RT_isRoot);
            if (node != NULL)
            {
                /* ... insert it into the (empty) tree - checking is not required here */
                if (addNode(node))
                {
                    if (flags & XF_templateElementEnclosesItems)
                    {
                        /* set template identification (if any) */
                        if (node->setTemplateIdentification(templateIdentifier, mappingResource).bad())
                            DCMSR_WARN("Root content item has invalid/incomplete template identification");
                    }
                    /* ... and let the node read the rest of the document */
                    result = node->readXML(doc, cursor, DocumentType, flags);
                    /* check and update by-reference relationships (if applicable) */
                    checkByReferenceRelationships(CM_updatePositionString);
                } else
                    result = SR_EC_InvalidDocumentTree;
            } else
                result = EC_MemoryExhausted;
        } else {
            DCMSR_ERROR("Root content item should always be a CONTAINER");
            result = SR_EC_InvalidDocumentTree;
        }
    }
    return result;
}


OFCondition DSRDocumentTree::write(DcmItem &dataset,
                                   DcmStack *markedItems)
{
    OFCondition result = SR_EC_InvalidDocumentTree;
    /* check whether root node has correct relationship and value type */
    if (isValid())
    {
        DSRDocumentTreeNode *node = getRoot();
        if (node != NULL)
        {
            /* check and update by-reference relationships (if applicable) */
            checkByReferenceRelationships(CM_updatePositionString);
            /* start writing from root node */
            result = node->write(dataset, markedItems);
        }
    }
    return result;
}


OFCondition DSRDocumentTree::writeXML(STD_NAMESPACE ostream &stream,
                                      const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocumentTree;
    /* check whether root node has correct relationship and value type */
    if (isValid())
    {
        DSRDocumentTreeNode *node = getRoot();
        /* start writing from root node */
        if (node != NULL)
        {
            /* check by-reference relationships (if applicable) */
            checkByReferenceRelationships(CM_resetReferenceTargetFlag);
            /* start writing from root node */
            result = node->writeXML(stream, flags);
        }
    }
    return result;
}


OFCondition DSRDocumentTree::renderHTML(STD_NAMESPACE ostream &docStream,
                                        STD_NAMESPACE ostream &annexStream,
                                        const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocumentTree;
    /* check whether root node has correct relationship and value type */
    if (isValid())
    {
        DSRDocumentTreeNode *node = getRoot();
        /* start rendering from root node */
        if (node != NULL)
        {
            /* check by-reference relationships (if applicable) */
            checkByReferenceRelationships(CM_resetReferenceTargetFlag);
            size_t annexNumber = 1;
            /* start rendering from root node */
            result = node->renderHTML(docStream, annexStream, 1 /*nestingLevel*/, annexNumber, flags & ~HF_internalUseOnly);
        }
    }
    return result;
}


OFCondition DSRDocumentTree::changeDocumentType(const E_DocumentType documentType)
{
    OFCondition result = SR_EC_UnsupportedValue;
    /* first, check whether new document type is supported at all */
    if (isDocumentTypeSupported(documentType))
    {
        /* clear object */
        clear();
        /* store new document type */
        DocumentType = documentType;
        /* create appropriate IOD constraint checker */
        delete ConstraintChecker;
        ConstraintChecker = createIODConstraintChecker(documentType);
        result = EC_Normal;
    }
    return result;
}


OFBool DSRDocumentTree::canAddContentItem(const E_RelationshipType relationshipType,
                                          const E_ValueType valueType,
                                          const E_AddMode addMode)
{
    OFBool result = OFFalse;
    if (isEmpty())
    {
        /* root node has to be a Container */
        result = (relationshipType == RT_isRoot) && (valueType == VT_Container);
    }
    else if (relationshipType != RT_unknown)
    {
        /* use checking routine from base class */
        result = DSRDocumentSubTree::canAddContentItem(relationshipType, valueType, addMode);
    }
    return result;
}


void DSRDocumentTree::unmarkAllContentItems()
{
    DSRTreeNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        DSRDocumentTreeNode *node = NULL;
        /* iterate over all nodes */
        do {
            node = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
            if (node != NULL)
                node->setMark(OFFalse);
        } while (cursor.iterate());
    }
}


void DSRDocumentTree::removeSignatures()
{
    DSRTreeNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        DSRDocumentTreeNode *node = NULL;
        /* iterate over all nodes */
        do {
            node = OFstatic_cast(DSRDocumentTreeNode *, cursor.getNode());
            if (node != NULL)
                node->removeSignatures();
        } while (cursor.iterate());
    }
}
