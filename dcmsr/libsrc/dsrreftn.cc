/*
 *
 *  Copyright (C) 2000-2003, OFFIS
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
 *    classes: DSRByReferenceTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-08-07 17:29:13 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrreftn.h"
#include "dsrxmld.h"


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


OFCondition DSRByReferenceTreeNode::print(ostream &stream,
                                          const size_t /*flags*/) const
{
    stream << relationshipTypeToReadableName(getRelationshipType()) << " " << ReferencedContentItem;
    return EC_Normal;
}


OFCondition DSRByReferenceTreeNode::writeXML(ostream &stream,
                                             const size_t flags,
                                             OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    stream << "<reference";
    if (flags & XF_relationshipTypeAsAttribute)
        stream << " relType=\"" << relationshipTypeToDefinedTerm(getRelationshipType()) << "\"";
    stream << " ref_id=\"" << ReferencedNodeID << "\">" << endl;
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    stream << "</reference>" << endl;
    return result;
}


OFCondition DSRByReferenceTreeNode::readContentItem(DcmItem &dataset,
                                                    OFConsole *logStream)
{
    DcmUnsignedLong delem(DCM_ReferencedContentItemIdentifier);
    /* clear before reading */
    ReferencedContentItem.clear();
    ReferencedNodeID = 0;
    /* read ReferencedContentItemIdentifier */
    OFCondition result = getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", logStream);
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


OFCondition DSRByReferenceTreeNode::writeContentItem(DcmItem &dataset,
                                                     OFConsole * /*logStream*/) const
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
        addElementToDataset(result, dataset, new DcmUnsignedLong(delem));
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
        /* get "ref_id" attribute */
        if (!doc.getStringFromAttribute(cursor, refID, "ref_id").empty())
        {
            ReferencedNodeID = stringToNumber(refID.c_str());
            /* this does not mean that the reference is really correct, this will be checked later */
            result = EC_Normal;
        } else
            result = SR_EC_InvalidValue;
    }
    return result;
}


OFCondition DSRByReferenceTreeNode::renderHTMLContentItem(ostream &docStream,
                                                          ostream & /*annexStream*/,
                                                          const size_t /*nestingLevel*/,
                                                          size_t & /*annexNumber*/,
                                                          const size_t /*flags*/,
                                                          OFConsole * /*logStream*/) const
{
    /* render reference string */
    docStream << "Content Item <a href=\"#content_item_" << ReferencedNodeID << "\">by-reference</a>" << endl;
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


OFBool DSRByReferenceTreeNode::canAddNode(const E_DocumentType /*documentType*/,
                                          const E_RelationshipType /*relationshipType*/,
                                          const E_ValueType /*valueType */,
                                          const OFBool /*byReference*/) const
{
    /* invalid: no child nodes allowed */
    return OFFalse;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrreftn.cc,v $
 *  Revision 1.10  2003-08-07 17:29:13  joergr
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
