/*
 *
 *  Copyright (C) 2000-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRDocumentTree
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-06-20 15:05:48 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrdoctr.h"
#include "dsrcontn.h"
#include "dsrreftn.h"


DSRDocumentTree::DSRDocumentTree(const E_DocumentType documentType)
  : DSRTree(),
    DocumentType(documentType),
    LogStream(NULL),
    CurrentContentItem()
{
}


DSRDocumentTree::~DSRDocumentTree()
{
}


void DSRDocumentTree::clear()
{
    DSRTree::clear();
}


OFBool DSRDocumentTree::isValid() const
{
    OFBool result = OFFalse;
    if (isDocumentTypeSupported(DocumentType))
    {
        const DSRDocumentTreeNode *node = (DSRDocumentTreeNode *)getRoot();
        if ((node != NULL) && (node->getRelationshipType() == RT_isRoot) && (node->getValueType() == VT_Container))
            result = OFTrue;
    }
    return result;
}


void DSRDocumentTree::setLogStream(OFConsole *stream)
{
    LogStream = stream;
}


E_Condition DSRDocumentTree::print(ostream &stream,
                                   const size_t flags)
{
    E_Condition result = EC_Normal;
    DSRTreeNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        /* update by-reference relationships (if applicable) */
        checkByReferenceRelationships(OFTrue /* updateString */,  OFFalse /* updateNodeID */);
        OFString string;
        size_t level = 0;
        const DSRDocumentTreeNode *node = NULL;
        do {
            node = (DSRDocumentTreeNode *)cursor.getNode();
            if (node != NULL)
            {
                if (flags & PF_printItemPosition)
                    stream << cursor.getPosition(string) << "  ";
                else
                {
                    level = cursor.getLevel();
                    if (level > 0)  // valid ?
                        stream << OFString((level - 1) * 2, ' ');
                }
                stream << "<";
                result = node->print(stream, flags);
                stream << ">" << endl;
            } else
                result = EC_IllegalCall;
        } while ((result == EC_Normal) && (cursor.iterate()));
    }
    return result;
}


E_Condition DSRDocumentTree::read(DcmItem &dataset,
                                  const E_DocumentType documentType,
                                  const size_t flags)
{
    E_Condition result = EC_Normal;
    /* clear current document tree */
    clear();
    /* set document type */
    DocumentType = documentType;
    /* check document type */
    if (isDocumentTypeSupported(DocumentType))
    {
        if (!isConstraintCheckingSupported(DocumentType))
            printWarningMessage(LogStream, "Check for relationship content constraints not yet supported");
        /* first try to read value type */
        OFString string;
        if (getAndCheckStringValueFromDataset(dataset, DCM_ValueType, string, "1", "1", LogStream) == EC_Normal)
        {
            /* root node should always be a container */
            if (definedTermToValueType(string) == VT_Container)
            {
                /* ... then create corresponding document tree node */
                DSRDocumentTreeNode *node = new DSRContainerTreeNode(RT_isRoot);
                if (node != NULL)
                {
                    /* ... insert it into the (empty) tree - checking is not required here */
                    if (addNode(node))
                    {
                        /* ... and let the node read the rest of the document */
                        result = node->read(dataset, DocumentType, flags, LogStream);
                        /* check and update by-reference relationships (if applicable) */
                        checkByReferenceRelationships(OFFalse /* updateString */,  OFTrue /* updateNodeID */);
                    } else
                        result = EC_IllegalCall;
                } else
                    result = EC_MemoryExhausted;
            } else {
                printErrorMessage(LogStream, "Root content item should always be a CONTAINER");
                result = EC_CorruptedData;
            }
        } else {
            printErrorMessage(LogStream, "ValueType attribute for root content item is missing");
            result = EC_CorruptedData;
        }
    } else
        result = EC_IllegalCall;
    return result;
}


E_Condition DSRDocumentTree::write(DcmItem &dataset,
                                   DcmStack *markedItems)
{
    E_Condition result = EC_CorruptedData;
    /* check whether root node has correct relationship and value type */
    if (isValid())
    {
        DSRDocumentTreeNode *node = (DSRDocumentTreeNode *)getRoot();
        if (node != NULL)
        {
            /* check and update by-reference relationships (if applicable) */
            checkByReferenceRelationships(OFTrue /* updateString */,  OFFalse /* updateNodeID */);
            /* start writing from root node */
            result = node->write(dataset, markedItems, LogStream);
        }
    }
    return result;
}


E_Condition DSRDocumentTree::writeXML(ostream &stream,
                                      const size_t flags)
{
    E_Condition result = EC_CorruptedData;
    /* check whether root node has correct relationship and value type */
    if (isValid())
    {
        DSRDocumentTreeNode *node = (DSRDocumentTreeNode *)getRoot();
        /* start writing from root node */
        if (node != NULL)
        {
            /* check and update by-reference relationships (if applicable) - not yet supported */
            // checkByReferenceRelationships(OFTrue /* updateString */,  OFFalse /* updateNodeID */);
            /* start writing from root node */
            result = node->writeXML(stream, flags, LogStream);
        }
    }
    return result;
}


E_Condition DSRDocumentTree::renderHTML(ostream &docStream,
                                        ostream &annexStream,
                                        const size_t flags)
{
    E_Condition result = EC_CorruptedData;
    /* check whether root node has correct relationship and value type */
    if (isValid())
    {
        DSRDocumentTreeNode *node = (DSRDocumentTreeNode *)getRoot();
        /* start rendering from root node */
        if (node != NULL)
        {
            /* update by-reference relationships (if applicable) */
            checkByReferenceRelationships(OFFalse /* updateString */,  OFFalse /* updateNodeID */);
            size_t annexNumber = 1;
            result = node->renderHTML(docStream, annexStream, 1 /* nestingLevel */, annexNumber, flags & ~HF_internalUseOnly, LogStream);
        }
    }
    return result;
}


E_Condition DSRDocumentTree::changeDocumentType(const E_DocumentType documentType)
{
    E_Condition result = EC_IllegalCall;
    if (isDocumentTypeSupported(documentType))
    {
        /* clear object */
        clear();
        /* store new document type */
        DocumentType = documentType;
        result = EC_Normal;
    }
    return result;
}


OFBool DSRDocumentTree::canAddContentItem(const E_RelationshipType relationshipType,
                                          const E_ValueType valueType,
                                          const E_AddMode addMode)
{
    OFBool result = OFFalse;
    const DSRDocumentTreeNode *node = (const DSRDocumentTreeNode *)getNode();
    if (node != NULL)
    {
        if (isConstraintCheckingSupported(DocumentType))
        {
            if ((addMode == AM_beforeCurrent) || (addMode == AM_afterCurrent))
            {     /* check parent node */
                node = (const DSRDocumentTreeNode *)getParentNode();
                if (node != NULL)
                    result = node->canAddNode(DocumentType, relationshipType, valueType);
            } else
                result = node->canAddNode(DocumentType, relationshipType, valueType);
        } else
            result = OFTrue;
    } else    /* root node */
        result = (relationshipType == RT_isRoot) && (valueType == VT_Container);
    return result;
}


OFBool DSRDocumentTree::canAddByReferenceRelationship(const E_RelationshipType relationshipType,
                                                      const E_ValueType valueType)
{
    OFBool result = OFFalse;
    if (isConstraintCheckingSupported(DocumentType))
    {
        const DSRDocumentTreeNode *node = (const DSRDocumentTreeNode *)getNode();
        if (node != NULL)
            result = node->canAddNode(DocumentType, relationshipType, valueType, OFTrue /* byReference */);
    }
    return result;
}


size_t DSRDocumentTree::addContentItem(const E_RelationshipType relationshipType,
                                       const E_ValueType valueType,
                                       const E_AddMode addMode)
{
    size_t nodeID = 0;
    if (canAddContentItem(relationshipType, valueType, addMode))
    {
        DSRDocumentTreeNode *node = createDocumentTreeNode(relationshipType, valueType);
        if (node != NULL)
            nodeID = addNode(node, addMode);
    }
    return nodeID;
}


size_t DSRDocumentTree::addByReferenceRelationship(const E_RelationshipType relationshipType,
                                                   const size_t referencedNodeID)
{
    size_t nodeID = 0;
    if (referencedNodeID > 0)
    {
        DSRTreeNodeCursor cursor(getRoot());
        if (cursor.isValid())
        {
            /* goto specified target node (might be improved later on) */
            if (cursor.gotoNode(referencedNodeID))
            {
                OFString sourceString;
                OFString targetString;
                getPosition(sourceString);
                cursor.getPosition(targetString);
                /* check whether target node is an ancestor of source node (prevent loops) */
                if (sourceString.substr(0, targetString.length()) != targetString)
                {
                    const DSRDocumentTreeNode *targetNode = (DSRDocumentTreeNode *)cursor.getNode();
                    if (targetNode != NULL)
                    {
                        /* check whether relationship is valid/allowed */
                        if (canAddByReferenceRelationship(relationshipType, targetNode->getValueType()))
                        {
                            DSRDocumentTreeNode *node = new DSRByReferenceTreeNode(relationshipType, referencedNodeID);
                            if (node != NULL)
                            {
                                nodeID = addNode(node, AM_belowCurrent);
                                /* go back to current node */
                                if (nodeID > 0)
                                    goUp();
                            }
                        }
                    }
                }
            }
        }
    }
    return nodeID;
}


size_t DSRDocumentTree::removeCurrentContentItem()
{
    return removeNode();
}


DSRContentItem &DSRDocumentTree::getCurrentContentItem()
{
    CurrentContentItem.setTreeNode((DSRDocumentTreeNode *)getNode());
    return CurrentContentItem;
}


void DSRDocumentTree::unmarkAllContentItems()
{
    DSRTreeNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        DSRDocumentTreeNode *node = NULL;
        do {
            node = (DSRDocumentTreeNode *)cursor.getNode();
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
        do {
            node = (DSRDocumentTreeNode *)cursor.getNode();
            if (node != NULL)
                node->removeSignatures();
        } while (cursor.iterate());
    }
}


size_t DSRDocumentTree::addNode(DSRTreeNode * /* node */,
                                const E_AddMode /* addMode */)
{
    /* invalid for this class */
    return 0;
}


size_t DSRDocumentTree::addNode(DSRDocumentTreeNode *node,
                                const E_AddMode addMode)
{
    /* might add a check for templates */
    return DSRTree::addNode(node, addMode);
}


size_t DSRDocumentTree::removeNode()
{
    /* might add a check for templates */
    return DSRTree::removeNode();
}


E_Condition DSRDocumentTree::checkByReferenceRelationships(const OFBool updateString,
                                                           const OFBool updateNodeID)
{
    E_Condition result = EC_IllegalCall;
    /* the flags are mutually exclusive */
    if (!(updateString && updateNodeID))
    {
        result = EC_Normal;
        /* by-reference relationships are only allowed for Comprehensive SR */
        if (DocumentType == DT_ComprehensiveSR)
        {
            DSRTreeNodeCursor cursor(getRoot());
            if (cursor.isValid())
            {
                const DSRDocumentTreeNode *node = NULL;
                do {    /* for all content items */
                    node = (DSRDocumentTreeNode *)cursor.getNode();
                    if (node != NULL)
                    {
                        /* only check/update by-reference relationships */
                        if (node->getValueType() == VT_byReference)
                        {
                            size_t refNodeID = 0;
                            /* type cast to directly access member variables of by-reference class */
                            DSRByReferenceTreeNode *refNode = (DSRByReferenceTreeNode *)node;
                            /* start searching from root node (might be improved later on) */
                            DSRTreeNodeCursor refCursor(getRoot());
                            if (updateNodeID)
                            {
                                /* update node ID */
                                refNodeID = refCursor.gotoNode(refNode->ReferencedContentItem);
                                if (refNodeID > 0)
                                    refNode->ReferencedNodeID = refCursor.getNodeID();
                                else
                                    refNode->ReferencedNodeID = 0;
                                refNode->ValidReference = (refNode->ReferencedNodeID > 0);
                            } else {
                                /* ReferenceNodeID contains a valid value */
                                refNodeID = refCursor.gotoNode(refNode->ReferencedNodeID);
                                if (updateString)
                                {
                                    /* update position string */
                                    if (refNodeID > 0)
                                        refCursor.getPosition(refNode->ReferencedContentItem);
                                    else
                                        refNode->ReferencedContentItem.clear();
                                    refNode->ValidReference = checkForValidUIDFormat(refNode->ReferencedContentItem);
                                } else if (refNodeID == 0)
                                    refNode->ValidReference = OFFalse;
                            }
                            if (refNodeID > 0)
                            {
                                /* source and target content items should not be identical */
                                if (refNodeID != cursor.getNodeID())
                                {
                                    OFString posString;
                                    cursor.getPosition(posString);
                                    /* check whether target node is an ancestor of source node (prevent loops) */
                                    if (posString.substr(0, refNode->ReferencedContentItem.length()) != refNode->ReferencedContentItem)
                                    {
                                        /* refCursor should now point to the reference target (refNodeID > 0) */
                                        const DSRDocumentTreeNode *parentNode = (DSRDocumentTreeNode *)cursor.getParentNode();
                                        DSRDocumentTreeNode *targetNode = (DSRDocumentTreeNode *)refCursor.getNode();
                                        if ((parentNode != NULL) && (targetNode != NULL))
                                        {
                                            /* tbd: need to reset flag to OFFalse!? */
                                            targetNode->setReferenceTarget();
                                            /* check whether relationship is valid */
                                            if (!parentNode->canAddNode(DocumentType, refNode->getRelationshipType(),
                                                targetNode->getValueType(), OFTrue /* byReference */))
                                            {
                                                printWarningMessage(LogStream, "Invalid by-reference relationship between two content items");
                                            }
                                        } else
                                            printWarningMessage(LogStream, "Corrupted data structures while checking by-reference relationships");
                                    } else
                                        printWarningMessage(LogStream, "By-reference relationship to ancestor content item (loop check)");
                                } else
                                    printWarningMessage(LogStream, "Source and target content item of by-reference relationship are identical");
                            } else
                                printWarningMessage(LogStream, "Target content item of by-reference relationship does not exist");
                        }
                    } else
                        result = EC_IllegalCall;
                } while ((result == EC_Normal) && (cursor.iterate()));
            }
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrdoctr.cc,v $
 *  Revision 1.12  2001-06-20 15:05:48  joergr
 *  Added minimal support for new SOP class Key Object Selection Document
 *  (suppl. 59).
 *
 *  Revision 1.11  2001/06/07 14:35:01  joergr
 *  Removed unused variable (reported by gcc 2.5.8 on NeXTSTEP).
 *
 *  Revision 1.10  2001/04/03 08:25:19  joergr
 *  Added new command line option: ignore relationship content constraints
 *  specified for each SR document class.
 *
 *  Revision 1.9  2001/02/13 16:35:05  joergr
 *  Corrected wrong implementation of getLevel() - started from 0 instead of 1.
 *
 *  Revision 1.8  2001/01/18 15:55:48  joergr
 *  Added support for digital signatures.
 *
 *  Revision 1.7  2000/11/07 18:33:30  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.6  2000/11/01 16:34:12  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.5  2000/10/26 14:29:49  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.4  2000/10/18 17:16:08  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.3  2000/10/16 16:32:56  joergr
 *  Added const type specifier.
 *
 *  Revision 1.2  2000/10/16 12:04:14  joergr
 *  Added new options: number nested items instead of indenting them, print SOP
 *  instance UID of referenced composite objects.
 *
 *  Revision 1.1  2000/10/13 07:52:20  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
