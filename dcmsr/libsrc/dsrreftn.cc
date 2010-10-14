/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRByReferenceTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.21 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrreftn.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRByReferenceTreeNode::DSRByReferenceTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_byReference),
   ValidReference(OFFalse),
   ReferencedContentItem(),
   ReferencedNodeID(0)
{
}


DSRByReferenceTreeNode::DSRByReferenceTreeNode(const E_RelationshipType relationshipType,
                                               const size_t referencedNodeID)
 : DSRDocumentTreeNode(relationshipType, VT_byReference),
   ValidReference(OFFalse),
   ReferencedContentItem(),
   ReferencedNodeID(referencedNodeID)
{
}


DSRByReferenceTreeNode::~DSRByReferenceTreeNode()
{
}


void DSRByReferenceTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    ValidReference = OFFalse;
    ReferencedContentItem.clear();
    ReferencedNodeID = 0;
}


OFBool DSRByReferenceTreeNode::isValid() const
{
    /* ConceptNameCodeSequence not allowed */
    return DSRDocumentTreeNode::isValid() && getConceptName().isEmpty() && ValidReference;
}


OFCondition DSRByReferenceTreeNode::print(STD_NAMESPACE ostream &stream,
                                          const size_t /*flags*/) const
{
    stream << relationshipTypeToReadableName(getRelationshipType()) << " " << ReferencedContentItem;
    return EC_Normal;
}


OFCondition DSRByReferenceTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                             const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags, OFFalse /*closingBracket*/);
    stream << " ref=\"" << ReferencedNodeID << "\">" << OFendl;
    /* basically, there should be no child content items but ... */
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRByReferenceTreeNode::readContentItem(DcmItem &dataset)
{
    DcmUnsignedLong delem(DCM_ReferencedContentItemIdentifier);
    /* clear before reading */
    ReferencedContentItem.clear();
    ReferencedNodeID = 0;
    /* read ReferencedContentItemIdentifier */
    OFCondition result = getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", "by-reference relationship");
    if (result.good())
    {
        /* create reference string from unsigned long values */
        Uint32 value = 0;
        char buffer[20];
        const unsigned long count = delem.getVM();
        for (unsigned long i = 0; i < count; i++)
        {
            if (i > 0)
                ReferencedContentItem += '.';
            if (delem.getUint32(value, i).good())
                ReferencedContentItem += numberToString(OFstatic_cast(size_t, value), buffer);
        }
    }
    return result;
}


OFCondition DSRByReferenceTreeNode::writeContentItem(DcmItem &dataset) const
{
    OFCondition result = SR_EC_InvalidValue;
    /* only write references with valid format */
    if (checkForValidUIDFormat(ReferencedContentItem))
    {
        result = EC_Normal;
        DcmUnsignedLong delem(DCM_ReferencedContentItemIdentifier);
        /* create unsigned long values from reference string */
        size_t posStart = 0;
        size_t posEnd = 0;
        unsigned long i = 0;
        do {
            /* search for next separator */
            posEnd = ReferencedContentItem.find('.', posStart);
            /* is last segment? */
            if (posEnd == OFString_npos)
                delem.putUint32(DSRTypes::stringToNumber(ReferencedContentItem.substr(posStart).c_str()), i);
            else {
                delem.putUint32(DSRTypes::stringToNumber(ReferencedContentItem.substr(posStart, posEnd - posStart).c_str()), i);
                posStart = posEnd + 1;
            }
            i++;
        } while (posEnd != OFString_npos);
        /* write ReferencedContentItemIdentifier */
        addElementToDataset(result, dataset, new DcmUnsignedLong(delem), "1-n", "1", "by-reference relationship");
    }
    return result;
}


OFCondition DSRByReferenceTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                       DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        OFString refID;
        /* get "ref" attribute */
        if (!doc.getStringFromAttribute(cursor, refID, "ref").empty())
        {
            ReferencedNodeID = stringToNumber(refID.c_str());
            /* this does not mean that the reference is really correct, this will be checked later */
            result = EC_Normal;
        } else
            result = SR_EC_InvalidValue;
    }
    return result;
}


OFCondition DSRByReferenceTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                          STD_NAMESPACE ostream & /*annexStream*/,
                                                          const size_t /*nestingLevel*/,
                                                          size_t & /*annexNumber*/,
                                                          const size_t /*flags*/) const
{
    /* render reference string */
    docStream << "Content Item <a href=\"#content_item_" << ReferencedNodeID << "\">by-reference</a>" << OFendl;
    return EC_Normal;
}


OFCondition DSRByReferenceTreeNode::setConceptName(const DSRCodedEntryValue & /*conceptName*/)
{
    /* invalid: no concept name allowed */
    return EC_IllegalCall;
}


OFCondition DSRByReferenceTreeNode::setObservationDateTime(const OFString & /*observationDateTime*/)
{
    /* invalid: no observation date and time allowed */
    return EC_IllegalCall;
}


OFCondition DSRByReferenceTreeNode::setTemplateIdentification(const OFString & /*templateIdentifier*/,
                                                              const OFString & /*mappingResource*/)
{
    /* invalid: no template identification allowed */
    return EC_IllegalCall;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrreftn.cc,v $
 *  Revision 1.21  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.20  2010-09-29 15:16:50  joergr
 *  Enhanced checking and reporting of standard violations in write() methods.
 *
 *  Revision 1.19  2010-09-29 08:32:26  joergr
 *  Used more specific "moduleName" for getAndCheckElementFromDataset() and
 *  checkElementValue().
 *
 *  Revision 1.18  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.17  2007-11-15 16:43:43  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.16  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.15  2005/12/08 15:48:03  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.14  2004/01/05 14:37:00  joergr
 *  Renamed XML attribute "ref_id" to "ref".
 *
 *  Revision 1.13  2003/12/01 15:47:28  joergr
 *  Changed XML encoding of by-reference relationships if flag
 *  XF_valueTypeAsAttribute is set.
 *
 *  Revision 1.12  2003/10/30 17:59:37  joergr
 *  Added full support for the ContentTemplateSequence (read/write, get/set
 *  template identification). Template constraints are not checked yet.
 *
 *  Revision 1.11  2003/09/15 14:13:42  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 *  Revision 1.10  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.9  2003/08/07 13:42:22  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.8  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.7  2001/10/10 15:29:59  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.6  2001/10/02 12:07:10  joergr
 *  Adapted module "dcmsr" to the new class OFCondition. Introduced module
 *  specific error codes.
 *
 *  Revision 1.5  2001/09/26 13:04:23  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:51:09  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/11/07 18:33:31  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.2  2000/11/01 16:37:02  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.1  2000/10/26 14:39:58  joergr
 *  Added support for "Comprehensive SR".
 *
 */
