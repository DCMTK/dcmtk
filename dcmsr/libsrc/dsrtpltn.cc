/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
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
 *    classes: DSRIncludedTemplateTreeNode
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtpltn.h"
#include "dcmtk/dcmsr/dsrstpl.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRIncludedTemplateTreeNode::DSRIncludedTemplateTreeNode(const DSRSharedSubTemplate &referencedTemplate,
                                                         const E_RelationshipType defaultRelType)
  : DSRDocumentTreeNode(defaultRelType, VT_includedTemplate),
    ReferencedTemplate(referencedTemplate)
{
    if (ReferencedTemplate)
    {
        /* store template identification of the referenced template */
        DSRDocumentTreeNode::setTemplateIdentification(ReferencedTemplate->getTemplateIdentifier(),
                                                       ReferencedTemplate->getMappingResource(),
                                                       ReferencedTemplate->getMappingResourceUID());
    }
}


DSRIncludedTemplateTreeNode::DSRIncludedTemplateTreeNode(const DSRIncludedTemplateTreeNode &node)
  : DSRDocumentTreeNode(node),
    ReferencedTemplate(node.ReferencedTemplate)
{
}


DSRIncludedTemplateTreeNode::~DSRIncludedTemplateTreeNode()
{
}


DSRIncludedTemplateTreeNode *DSRIncludedTemplateTreeNode::clone() const
{
    return new DSRIncludedTemplateTreeNode(*this);
}


void DSRIncludedTemplateTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    ReferencedTemplate.reset();
}


OFBool DSRIncludedTemplateTreeNode::isValid() const
{
    return DSRDocumentTreeNode::isValid() && hasValidValue();
}


OFBool DSRIncludedTemplateTreeNode::hasValidValue() const
{
    /* check whether the reference to the included template is valid */
    return ReferencedTemplate ? OFTrue : OFFalse;
}


OFCondition DSRIncludedTemplateTreeNode::print(STD_NAMESPACE ostream &stream,
                                               const size_t flags) const
{
    /* print separate line for internal template node (if requested) */
    if (flags & PF_printIncludedTemplateNode)
    {
        /* print node ID (might be useful for debugging purposes) */
        if (flags & PF_printNodeID)
            stream << "id:" << getNodeID() << " ";
        stream << "# INCLUDE ";
        /* check whether template identification is set */
        if (hasTemplateIdentification())
        {
            OFString templateIdentifier;
            OFString mappingResource;
            getTemplateIdentification(templateIdentifier, mappingResource);
            stream << "TID " << templateIdentifier << " (" << mappingResource << ")";
        } else {
            /* no details on the template available */
            stream << "<unknown template>";
        }
        /* check whether default relationship type is specified */
        if (getRelationshipType() != RT_unknown)
        {
            stream << " with default relationship type "
                   << relationshipTypeToDefinedTerm(getRelationshipType());
        }
        /* print annotation (optional) */
        if (hasAnnotation() && (flags & PF_printAnnotation))
            stream << "  \"" << getAnnotation().getText() << "\"";
        stream << OFendl;
    }
    return EC_Normal;
}


OFCondition DSRIncludedTemplateTreeNode::printTemplate(STD_NAMESPACE ostream &stream,
                                                       const size_t flags,
                                                       const OFString &linePrefix) const
{
    OFCondition result = EC_Normal;
    /* print content of included template (if non-empty) */
    if (!ReferencedTemplate->isEmpty())
        result = ReferencedTemplate->print(stream, flags, linePrefix);
    return result;
}


OFCondition DSRIncludedTemplateTreeNode::write(DcmItem &dataset,
                                               DcmStack *markedItems)
{
    OFCondition result = EC_Normal;
    /* write content of included template (if non-empty) */
    if (!ReferencedTemplate->isEmpty())
        result = ReferencedTemplate->write(dataset, markedItems);
    return result;
}


OFCondition DSRIncludedTemplateTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                                  const size_t flags) const
{
    OFCondition result = EC_Normal;
    /* write content of included template in XML format (if non-empty) */
    if (!ReferencedTemplate->isEmpty())
        result = ReferencedTemplate->writeXML(stream, flags);
    return result;
}


OFCondition DSRIncludedTemplateTreeNode::renderHTML(STD_NAMESPACE ostream &docStream,
                                                    STD_NAMESPACE ostream &annexStream,
                                                    const size_t nestingLevel,
                                                    size_t &annexNumber,
                                                    const size_t flags) const
{
    OFCondition result = EC_Normal;
    /* render content of included template in HTML/XHTML format (if non-empty) */
    if (!ReferencedTemplate->isEmpty())
        result = ReferencedTemplate->renderHTML(docStream, annexStream, nestingLevel, annexNumber, flags);
    return result;
}


OFCondition DSRIncludedTemplateTreeNode::setValue(const DSRSharedSubTemplate &referencedTemplate)
{
    ReferencedTemplate = referencedTemplate;
    /* currently, no checks are performed */
    return EC_Normal;
}


// protected methods

OFCondition DSRIncludedTemplateTreeNode::read(DcmItem & /*dataset*/,
                                              const DSRIODConstraintChecker * /*constraintChecker*/,
                                              const size_t /*flags*/)
{
    /* invalid: no content to read */
    return EC_IllegalCall;
}


OFCondition DSRIncludedTemplateTreeNode::readXML(const DSRXMLDocument & /*doc*/,
                                                 DSRXMLCursor /*cursor*/,
                                                 const E_DocumentType /*documentType*/,
                                                 const size_t /*flags*/)
{
    /* invalid: no content to read */
    return EC_IllegalCall;
}


OFCondition DSRIncludedTemplateTreeNode::setConceptName(const DSRCodedEntryValue & /*conceptName*/,
                                                        const OFBool /*check*/)
{
    /* invalid: no concept name allowed */
    return EC_IllegalCall;
}


OFCondition DSRIncludedTemplateTreeNode::setObservationDateTime(const OFString & /*observationDateTime*/,
                                                                const OFBool /*check*/)
{
    /* invalid: no observation date/time allowed */
    return EC_IllegalCall;
}


OFCondition DSRIncludedTemplateTreeNode::setObservationDateTime(const DcmElement & /*delem*/,
                                                                const unsigned long /*pos*/,
                                                                const OFBool /*check*/)
{
    /* invalid: no observation date/time allowed */
    return EC_IllegalCall;
}


OFCondition DSRIncludedTemplateTreeNode::setObservationDateTime(DcmItem & /*dataset*/,
                                                                const DcmTagKey & /*tagKey*/,
                                                                const unsigned long /*pos*/,
                                                                const OFBool /*check*/)
{
    /* invalid: no observation date/time allowed */
    return EC_IllegalCall;
}


OFCondition DSRIncludedTemplateTreeNode::setObservationUID(const OFString & /*observationUID*/,
                                                           const OFBool /*check*/)
{
    /* invalid: no observation unique identifier allowed */
    return EC_IllegalCall;
}


OFCondition DSRIncludedTemplateTreeNode::setTemplateIdentification(const OFString & /*templateIdentifier*/,
                                                                   const OFString & /*mappingResource*/,
                                                                   const OFString & /*mappingResourceUID*/,
                                                                   const OFBool /*check*/)
{
    /* invalid: no manual setting of template identification allowed */
    return EC_IllegalCall;
}
