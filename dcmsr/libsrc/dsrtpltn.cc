/*
 *
 *  Copyright (C) 2015-2018, J. Riesmeier, Oldenburg, Germany
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


OFBool DSRIncludedTemplateTreeNode::operator==(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator==(node);
    if (result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = (ReferencedTemplate.get() == OFstatic_cast(const DSRIncludedTemplateTreeNode &, node).ReferencedTemplate.get());
    }
    return result;
}


OFBool DSRIncludedTemplateTreeNode::operator!=(const DSRDocumentTreeNode &node) const
{
    /* call comparison operator of base class (includes check of value type) */
    OFBool result = DSRDocumentTreeNode::operator!=(node);
    if (!result)
    {
        /* it's safe to cast the type since the value type has already been checked */
        result = (ReferencedTemplate.get() != OFstatic_cast(const DSRIncludedTemplateTreeNode &, node).ReferencedTemplate.get());
    }
    return result;
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


OFBool DSRIncludedTemplateTreeNode::isShort(const size_t /*flags*/) const
{
    return !hasValidValue();
}


OFCondition DSRIncludedTemplateTreeNode::print(STD_NAMESPACE ostream &stream,
                                               const size_t flags) const
{
    stream << "# INCLUDE ";
    /* check whether template identification is set */
    if (hasTemplateIdentification())
    {
        OFString templateIdentifier, mappingResource;
        getTemplateIdentification(templateIdentifier, mappingResource);
        stream << "TID " << templateIdentifier << " (" << mappingResource << ")";
    } else {
        /* no details on the template available */
        stream << "<unknown template>";
    }
    /* check whether default relationship type is specified */
    if (getRelationshipType() != RT_unknown)
        stream << " with default relationship type " << relationshipTypeToDefinedTerm(getRelationshipType());
    /* print annotation (optional) */
    if (hasAnnotation() && (flags & PF_printAnnotation))
        stream << "  \"" << getAnnotation().getText() << "\"";
    return EC_Normal;
}


OFCondition DSRIncludedTemplateTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                                  const size_t flags) const
{
    OFCondition result = EC_Normal;
    /* write content of included template in XML format (if non-empty) */
    if (hasValidValue() && !ReferencedTemplate->isEmpty())
    {
        OFString templateIdentifier, mappingResource;
        /* output details on beginning of included template (if enabled) */
        if (hasTemplateIdentification() && (flags & XF_addCommentsForIncludedTemplate))
        {
            getTemplateIdentification(templateIdentifier, mappingResource);
            stream << "<!-- BEGIN: included template TID " << templateIdentifier << " (" << mappingResource << ") -->" << OFendl;
        }
        /* write content of referenced document subtree */
        result = ReferencedTemplate->writeXML(stream, flags);
        /* output details on end of included template (if available, see above) */
        if (!templateIdentifier.empty() && !mappingResource.empty())
            stream << "<!-- END: included template TID " << templateIdentifier << " (" << mappingResource << ") -->" << OFendl;
    }
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
    /* invalid: cannot read document with included templates */
    return SR_EC_CannotProcessIncludedTemplates;
}


OFCondition DSRIncludedTemplateTreeNode::write(DcmItem & /*dataset*/,
                                               DcmStack * /*markedItems*/)
{
    /* invalid: cannot write document with included templates */
    return SR_EC_CannotProcessIncludedTemplates;
}


OFCondition DSRIncludedTemplateTreeNode::readXML(const DSRXMLDocument & /*doc*/,
                                                 DSRXMLCursor /*cursor*/,
                                                 const E_DocumentType /*documentType*/,
                                                 const size_t /*flags*/)
{
    /* invalid: cannot read document with included templates */
    return SR_EC_CannotProcessIncludedTemplates;
}


OFCondition DSRIncludedTemplateTreeNode::renderHTML(STD_NAMESPACE ostream & /*docStream*/,
                                                    STD_NAMESPACE ostream & /*annexStream*/,
                                                    const size_t /*nestingLevel*/,
                                                    size_t & /*annexNumber*/,
                                                    const size_t /*flags*/) const
{
    /* invalid: cannot render document with included templates */
    return SR_EC_CannotProcessIncludedTemplates;
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
