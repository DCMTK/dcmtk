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
 *    classes: DSRTemplateCommon
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrctpl.h"
#include "dcmtk/dcmsr/dsrdocst.h"

#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrui.h"


DSRTemplateCommon::DSRTemplateCommon(const OFString &templateIdentifier,
                                     const OFString &mappingResource,
                                     const OFString &mappingResourceUID)
  : TemplateIdentifier(templateIdentifier),
    MappingResource(mappingResource),
    MappingResourceUID(mappingResourceUID),
    ExtensibleMode(OFFalse),
    OrderSignificantMode(OFFalse),
    NodeList()
{
    /* by default, a template is non-extensible */
}


DSRTemplateCommon::~DSRTemplateCommon()
{
}


void DSRTemplateCommon::clear()
{
    clearEntriesInNodeList();
}


OFBool DSRTemplateCommon::hasTemplateIdentification() const
{
    /* mapping resource UID is optional, so do not check it */
    return !TemplateIdentifier.empty() && !MappingResource.empty();
}


OFBool DSRTemplateCommon::isTemplateIdentificationValid(const OFBool check) const
{
    OFBool result = OFFalse;
    /* either all three values are empty ... */
    if (TemplateIdentifier.empty() && MappingResource.empty() && MappingResourceUID.empty())
        result = OFTrue;
    /* ... or the first two are both non-empty */
    else if (!TemplateIdentifier.empty() && !MappingResource.empty())
    {
        /* check more thoroughly whether the stored values are valid */
        if (check)
        {
            if (DcmCodeString::checkStringValue(TemplateIdentifier, "1").good() &&
                DcmCodeString::checkStringValue(MappingResource, "1").good() &&
                DcmUniqueIdentifier::checkStringValue(MappingResourceUID, "1").good())
            {
                result = OFTrue;
            }
        } else
            result = OFTrue;
    }
    return result;
}


OFBool DSRTemplateCommon::compareTemplateIdentication(const OFString &templateIdentifier,
                                                      const OFString &mappingResource,
                                                      const OFString &mappingResourceUID) const
{
    OFBool result = (TemplateIdentifier == templateIdentifier) && (MappingResource == mappingResource);
    /* mapping resource UID is optional, so only check it if present */
    if (result && !MappingResourceUID.empty() && !mappingResourceUID.empty())
        result = (MappingResourceUID == mappingResourceUID);
    return result;
}


// protected methods

void DSRTemplateCommon::clearEntriesInNodeList()
{
    /* set all entries to 0 */
    for (size_t i = 0; i < NodeList.size(); ++i)
        NodeList[i] = 0;
}


void DSRTemplateCommon::reserveEntriesInNodeList(const size_t count,
                                                 const OFBool initialize)
{
    /* check whether the entries should be initialized... */
    if (initialize)
    {
        NodeList.resize(count);
        clearEntriesInNodeList();
    } else {
        /* ... or only reserved (not created and initialized) */
        NodeList.reserve(count);
    }
}


void DSRTemplateCommon::storeEntryInNodeList(const size_t pos,
                                             const size_t nodeID)
{
    /* make sure that entry can be stored */
    if (pos >= NodeList.size())
        NodeList.resize(pos + 1, 0);
    NodeList[pos] = nodeID;
}


size_t DSRTemplateCommon::getEntryFromNodeList(const size_t pos) const
{
    size_t nodeID = 0;
    /* make sure that entry exists */
    if (pos < NodeList.size())
        nodeID = NodeList[pos];
    return nodeID;
}


size_t DSRTemplateCommon::gotoEntryFromNodeList(DSRDocumentTreeNodeCursor &cursor,
                                                const size_t pos) const
{
    size_t nodeID = 0;
    /* make sure that cursor is valid and list entry exists */
    if ((cursor.isValid()) && (pos < NodeList.size()))
    {
        nodeID = NodeList[pos];
        /* check whether current node is already the right one */
        if (cursor.getNodeID() != nodeID)
            nodeID = cursor.gotoNode(nodeID);
    }
    return nodeID;
}


size_t DSRTemplateCommon::gotoEntryFromNodeList(DSRDocumentSubTree *tree,
                                                const size_t pos)
{
    size_t nodeID = 0;
    /* make sure that tree is valid and list entry exists */
    if ((tree != NULL) && (pos < NodeList.size()))
    {
        nodeID = NodeList[pos];
        /* check whether current node is already the right one */
        if (tree->getNodeID() != nodeID)
            nodeID = tree->gotoNode(nodeID, OFTrue /*startFromRoot*/);
    }
    return nodeID;
}


size_t DSRTemplateCommon::gotoLastEntryFromNodeList(DSRDocumentSubTree *tree,
                                                    const size_t lastPos,
                                                    const size_t firstPos)
{
    size_t nodeID = 0;
    /* make sure that tree is valid and list entry exists */
    if ((tree != NULL) && (firstPos <= lastPos) && (lastPos < NodeList.size()))
    {
        size_t pos = lastPos + 1;
        while ((pos > firstPos) && (nodeID == 0))
            nodeID = NodeList[--pos];
        /* check whether current node is already the right one */
        if (tree->getNodeID() != nodeID)
            nodeID = tree->gotoNode(nodeID, OFTrue /*startFromRoot*/);
    }
    return nodeID;
}
