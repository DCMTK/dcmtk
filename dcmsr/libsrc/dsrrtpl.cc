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
 *    classes: DSRRootTemplate
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrrtpl.h"
#include "dcmtk/dcmsr/dsrstpl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRRootTemplate::DSRRootTemplate(const E_DocumentType documentType,
                                 const OFString &templateIdentifier,
                                 const OFString &mappingResource,
                                 const OFString &mappingResourceUID)
  : DSRDocumentTree(documentType),
    DSRTemplateCommon(templateIdentifier, mappingResource, mappingResourceUID)
{
}


DSRRootTemplate::DSRRootTemplate(const DSRRootTemplate &rootTemplate)
  : DSRDocumentTree(rootTemplate),
    DSRTemplateCommon(rootTemplate)
{
}


DSRRootTemplate::~DSRRootTemplate()
{
}


DSRRootTemplate *DSRRootTemplate::clone() const
{
    return new DSRRootTemplate(*this);
}


DSRDocumentTree *DSRRootTemplate::cloneTree() const
{
    DSRDocumentTree *tree = DSRDocumentTree::clone();
    if (tree != NULL)
    {
        /* mark root CONTAINER with template identification */
        tree->setTemplateIdentification(getTemplateIdentifier(), getMappingResource(), getMappingResourceUID());
    }
    return tree;
}


void DSRRootTemplate::clear()
{
    /* call clear() method of base classes */
    DSRDocumentTree::clear();
    DSRTemplateCommon::clear();
}


OFBool DSRRootTemplate::isValid() const
{
    /* check whether both base classes are valid */
    return DSRDocumentTree::isValid() && DSRTemplateCommon::isTemplateIdentificationValid();
}


const DSRDocumentTree &DSRRootTemplate::getTree()
{
    /* update the document tree for output (if needed) */
    updateTreeForOutput();
    return *this;
}


OFCondition DSRRootTemplate::addExtraContentItem(const E_RelationshipType relationshipType,
                                                 const E_ValueType valueType,
                                                 const E_AddMode addMode)
{
    OFCondition result = SR_EC_NonExtensibleTemplate;
    /* check whether this template is extensible */
    if (isExtensible())
    {
        /* call the function doing the real work */
        if (addContentItem(relationshipType, valueType, addMode) > 0)
            result = EC_Normal;
        else
            result = SR_EC_CannotAddContentItem;
    }
    return result;
}



OFCondition DSRRootTemplate::insertExtraTemplate(const DSRSubTemplate &subTemplate,
                                                 const E_AddMode addMode,
                                                 const E_RelationshipType defaultRelType)
{
    OFCondition result = SR_EC_NonExtensibleTemplate;
    /* check whether this template is extensible */
    if (isExtensible())
    {
        /* call the function doing the real work */
        result = insertSubTree(subTemplate.cloneTree(), addMode, defaultRelType, OFTrue /*deleteIfFail*/);
    }
    return result;
}


// protected methods

OFCondition DSRRootTemplate::read(DcmItem & /*dataset*/,
                                  const E_DocumentType /*documentType*/,
                                  const size_t /*flags*/)
{
    /* not implemented (yet) */
    return EC_IllegalCall;
}


OFCondition DSRRootTemplate::readXML(const DSRXMLDocument & /*doc*/,
                                     DSRXMLCursor /*cursor*/,
                                     const size_t /*flags*/)
{
    /* not implemented (yet) */
    return EC_IllegalCall;
}


void DSRRootTemplate::updateTreeForOutput()
{
    /* mark root CONTAINER with template identification (if applicable) */
    OFCondition result = setTemplateIdentification(getTemplateIdentifier(), getMappingResource(), getMappingResourceUID());
    /* in case of error, output some useful debug information */
    if (result.bad())
    {
        DCMSR_DEBUG("DSRRootTemplate::updateTreeForOutput() Problem with setting TID "
            << getTemplateIdentifier() << " (" << getMappingResource() << "): " << result.text());
    }
}
