/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *  Update Date:      $Date: 2000-10-18 17:16:08 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrdoctr.h"
#include "dsrcontn.h"


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
                                   const size_t flags) const
{
    E_Condition result = EC_Normal;
    DSRTreeNodeCursor cursor(getRoot());
    if (cursor.isValid())
    {
        OFString string;
        const DSRDocumentTreeNode *node = NULL;
        do {
            node = (DSRDocumentTreeNode *)cursor.getNode();
            if (node != NULL)
            {
                if (flags & PF_printItemPosition)
                    stream << cursor.getPosition(string) << "  ";
                else
                    stream << OFString(cursor.getLevel() * 2, ' ');
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
                                  const E_DocumentType documentType)
{
    E_Condition result = EC_Normal;
    /* clear current document tree */
    clear();
    /* set document type */
    DocumentType = documentType;
    /* check document type */
    if (isDocumentTypeSupported(DocumentType))
    {
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
                        result = node->read(dataset, DocumentType, LogStream);
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


E_Condition DSRDocumentTree::write(DcmItem &dataset)
{
    E_Condition result = EC_CorruptedData;
    /* check whether root node has correct relationship and value type */
    if (isValid())
    {
        DSRDocumentTreeNode *node = (DSRDocumentTreeNode *)getRoot();
        /* start writing from root node */
        if (node != NULL)
            result = node->write(dataset, LogStream);
    }
    return result;
}


E_Condition DSRDocumentTree::renderHTML(ostream &docStream,
                                        ostream &annexStream,
                                        const size_t flags) const
{
    E_Condition result = EC_CorruptedData;
    /* check whether root node has correct relationship and value type */
    if (isValid())
    {
        DSRDocumentTreeNode *node = (DSRDocumentTreeNode *)getRoot();
        /* start rendering from root node */
        if (node != NULL)
        {
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
        if ((addMode == AM_beforeCurrent) || (addMode == AM_afterCurrent))
        {     /* check parent node */
            node = (const DSRDocumentTreeNode *)getParentNode();
            if (node != NULL)
                result = node->canAddNode(DocumentType, relationshipType, valueType);
        } else
            result = node->canAddNode(DocumentType, relationshipType, valueType);
    } else    /* root node */
        result = (relationshipType == RT_isRoot) && (valueType == VT_Container);
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


size_t DSRDocumentTree::removeCurrentContentItem()
{
    return removeNode();
}


DSRContentItem &DSRDocumentTree::getCurrentContentItem()
{
    CurrentContentItem.setTreeNode((DSRDocumentTreeNode *)getNode());
    return CurrentContentItem;
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


/*
 *  CVS/RCS Log:
 *  $Log: dsrdoctr.cc,v $
 *  Revision 1.4  2000-10-18 17:16:08  joergr
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
