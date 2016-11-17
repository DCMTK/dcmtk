/*
 *
 *  Copyright (C) 2000-2016, OFFIS e.V.
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

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"


DSRDocumentTree::DSRDocumentTree(const E_DocumentType documentType)
  : DSRDocumentSubTree(),
    DocumentType(DT_invalid)
{
    /* check & set document type, create constraint checker object */
    changeDocumentType(documentType, OFTrue /*deleteTree*/);
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


DSRDocumentTree &DSRDocumentTree::operator=(DSRDocumentTree tree)
{
    /* by-value parameter serves as a temporary */
    swap(tree);
    return *this;
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
    /* check whether both document type and tree are valid */
    return isDocumentTypeSupported(DocumentType) && isValidDocumentTree();
}


OFCondition DSRDocumentTree::print(STD_NAMESPACE ostream &stream,
                                   const size_t flags)
{
    /* call the inherited method (hide additional parameters) */
    return DSRDocumentSubTree::print(stream, flags);
}


OFCondition DSRDocumentTree::read(DcmItem &dataset,
                                  const E_DocumentType documentType,
                                  const size_t flags)
{
    /* clear current document tree, check & change document type */
    OFCondition result = changeDocumentType(documentType, OFTrue /*deleteTree*/);
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
                        checkByReferenceRelationships<DSRDocumentTreeNodeCursor>(CM_updateNodeID, flags & CB_maskReadFlags);
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


OFCondition DSRDocumentTree::write(DcmItem &dataset,
                                   DcmStack *markedItems)
{
    OFCondition result = SR_EC_InvalidDocumentTree;
    /* check whether document tree is valid */
    if (isValid())
    {
        /* check whether document tree contains any included templates */
        if (isExpandedDocumentTree())
        {
            DSRDocumentTreeNode *node = getRoot();
            if (node != NULL)
            {
                /* check and update by-reference relationships (if applicable) */
                checkByReferenceRelationships<DSRDocumentTreeNodeCursor>(CM_updatePositionString);
                /* update the document tree for output (if needed) */
                updateTreeForOutput();
                /* start writing from root node */
                result = node->write(dataset, markedItems);
            }
        } else {
            /* tbd: cannot write document with included templates */
            result = SR_EC_CannotProcessIncludedTemplates;
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
        OFString mappingResource;
        OFString mappingResourceUID;
        OFString templateIdentifier;
        /* template identification information expected "outside" content item */
        if (flags & XF_templateElementEnclosesItems)
        {
            /* check for optional root template identification */
            const DSRXMLCursor childCursor = doc.getNamedNode(cursor, "template", OFFalse /*required*/);
            if (childCursor.valid())
            {
                doc.getStringFromAttribute(childCursor, mappingResource, "resource");
                doc.getStringFromAttribute(childCursor, mappingResourceUID, "uid", OFFalse /*encoding*/, OFFalse /*required*/);
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
                        if (node->setTemplateIdentification(templateIdentifier, mappingResource, mappingResourceUID).bad())
                            DCMSR_WARN("Root content item has invalid/incomplete template identification");
                    }
                    /* ... and let the node read the rest of the document */
                    result = node->readXML(doc, cursor, DocumentType, flags);
                    /* check and update by-reference relationships (if applicable) */
                    checkByReferenceRelationships<DSRDocumentTreeNodeCursor>(CM_updatePositionString);
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


OFCondition DSRDocumentTree::renderHTML(STD_NAMESPACE ostream &docStream,
                                        STD_NAMESPACE ostream &annexStream,
                                        const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocumentTree;
    /* check whether document tree is valid */
    if (isValid())
    {
        /* check whether document tree contains any included templates */
        if (isExpandedDocumentTree())
        {
            DSRDocumentTreeNode *node = getRoot();
            if (node != NULL)
            {
                size_t annexNumber = 1;
                /* check by-reference relationships (if applicable) */
                checkByReferenceRelationships<DSRDocumentTreeNodeCursor>(CM_resetReferenceTargetFlag);
                /* update the document tree for output (if needed) */
                updateTreeForOutput();
                /* start rendering from root node */
                result = node->renderHTML(docStream, annexStream, 1 /*nestingLevel*/, annexNumber, flags & ~HF_internalUseOnly);
            }
        } else {
            /* tbd: cannot render document with included templates */
            result = SR_EC_CannotProcessIncludedTemplates;
        }
    }
    return result;
}


OFCondition DSRDocumentTree::changeDocumentType(const E_DocumentType documentType,
                                                const OFBool deleteTree)
{
    OFCondition result = SR_EC_UnsupportedValue;
    /* first, check whether new document type is supported at all */
    if (isDocumentTypeSupported(documentType))
    {
        /* create constraint checker for new document type */
        DSRIODConstraintChecker *constraintChecker = createIODConstraintChecker(documentType);
        if (deleteTree)
        {
            /* clear object, i.e. delete the currently stored tree */
            clear();
            result = EC_Normal;
        } else {
            /* check whether new document type is "compatible" */
            result = checkDocumentTreeConstraints(constraintChecker);
        }
        /* check whether we can proceed */
        if (result.good())
        {
            /* store new document type ... */
            DocumentType = documentType;
            /* and new IOD constraint checker */
            delete ConstraintChecker;
            ConstraintChecker = constraintChecker;
        } else {
            /* if not, free allocated memory */
            delete constraintChecker;
        }
    }
    return result;
}


OFBool DSRDocumentTree::canAddContentItem(const E_RelationshipType relationshipType,
                                          const E_ValueType valueType,
                                          const E_AddMode addMode) const
{
    OFBool result = OFFalse;
    if (isEmpty())
    {
        /* root node has to be a container */
        result = (relationshipType == RT_isRoot) && (valueType == VT_Container);
    }
    else if (relationshipType != RT_unknown)
    {
        /* use checking routine from base class */
        result = DSRDocumentSubTree::canAddContentItem(relationshipType, valueType, addMode);
    }
    return result;
}


OFBool DSRDocumentTree::canInsertSubTree(const DSRDocumentSubTree *tree,
                                         const E_AddMode addMode,
                                         const E_RelationshipType defaultRelType) const
{
    OFBool result = OFFalse;
    if (isEmpty())
    {
        /* check whether the subtree to be inserted is a valid document tree */
        if (tree != NULL)
            result = tree->isValidDocumentTree(defaultRelType);
    } else {
        /* use checking routine from base class */
        result = DSRDocumentSubTree::canInsertSubTree(tree, addMode, defaultRelType);
    }
    return result;
}


OFCondition DSRDocumentTree::checkDocumentTreeConstraints(DSRIODConstraintChecker *checker)
{
    OFCondition result = EC_Normal;
    /* make sure that the passed parameter is valid */
    if (checker != NULL)
    {
        /* an empty document tree always complies with the constraints */
        if (!isEmpty())
        {
            /* check whether the current document tree is valid, i.e. the root node is a container */
            if (isValid())
            {
                /* determine template identifier (TID) and mapping resource expected for the new document type */
                OFString expectedTemplateIdentifier;
                OFString expectedMappingResource;
                checker->getRootTemplateIdentification(expectedTemplateIdentifier, expectedMappingResource);
                /* check whether the expected template (if known) has been used */
                if (!expectedTemplateIdentifier.empty())
                {
                    OFString templateIdentifier;
                    OFString mappingResource;
                    OFString mappingResourceUID;
                    if (getRoot()->getTemplateIdentification(templateIdentifier, mappingResource, mappingResourceUID).good())
                    {
                        /* check for DICOM Content Mapping Resource */
                        if (mappingResource == "DCMR")
                        {
                            /* check whether the correct Mapping Resource UID is used (if present) */
                            if (!mappingResourceUID.empty() && (mappingResourceUID != UID_DICOMContentMappingResource))
                            {
                                DCMSR_WARN("Incorrect value for Mapping Resource UID (" << mappingResourceUID << "), "
                                    << UID_DICOMContentMappingResource << " expected");
                            }
                        }
                        /* compare with expected mapping resource */
                        if (mappingResource != expectedMappingResource)
                        {
                            DCMSR_WARN("Incorrect value for Mapping Resource ("
                                << ((mappingResource.empty()) ? "<empty>" : mappingResource) << "), "
                                << expectedMappingResource << " expected");
                        }
                        /* compare with expected template identifier */
                        if (templateIdentifier != expectedTemplateIdentifier)
                        {
                            DCMSR_WARN("Incorrect value for Template Identifier ("
                                << ((templateIdentifier.empty()) ? "<empty>" : templateIdentifier) << "), "
                                << expectedTemplateIdentifier << " expected");
                        }
                    }
                }
                /* check by-reference relationships (update 'target value type' if applicable) */
                result = checkByReferenceRelationships<DSRDocumentTreeNodeCursor>(CM_resetReferenceTargetFlag, RF_ignoreRelationshipConstraints);
                /* check whether the nodes of this tree also comply with the given constraints */
                if (result.good())
                    result = checkSubTreeConstraints(this, checker);
            } else
                result = SR_EC_InvalidDocumentTree;
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


void DSRDocumentTree::unmarkAllContentItems()
{
    DSRIncludedTemplateNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        /* iterate over all nodes */
        do {
            cursor.getNode()->setMark(OFFalse);
        } while (cursor.iterate());
    }
}


void DSRDocumentTree::removeSignatures()
{
    DSRIncludedTemplateNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        /* iterate over all nodes */
        do {
            cursor.getNode()->removeSignatures();
        } while (cursor.iterate());
    }
}


// protected methods

void DSRDocumentTree::swap(DSRDocumentTree &tree)
{
    /* call inherited method */
    DSRDocumentSubTree::swap(tree);
    /* swap other members */
    OFswap(DocumentType, tree.DocumentType);
}


OFCondition DSRDocumentTree::print(STD_NAMESPACE ostream &stream,
                                   const size_t flags,
                                   const DSRPositionCounter *posCounter)
{
    /* call inherited method */
    return DSRDocumentSubTree::print(stream, flags, posCounter);
}
