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
 *    classes: DSRByReferenceTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-11-07 18:33:31 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrreftn.h"


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


E_Condition DSRByReferenceTreeNode::print(ostream &stream,
                                          const size_t /* flags */) const
{
    stream << relationshipTypeToReadableName(getRelationshipType()) << " " << ReferencedContentItem;
    return EC_Normal;
}


E_Condition DSRByReferenceTreeNode::writeXML(ostream &stream,
                                             const size_t flags,
                                             OFConsole *logStream) const
{
    E_Condition result = EC_Normal;
    stream << "<reference ref_id=\"" << ReferencedNodeID << "\">" << endl;
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    stream << "</reference>" << endl;
    return result;
}


E_Condition DSRByReferenceTreeNode::readContentItem(DcmItem &dataset,
                                                    OFConsole *logStream)
{
    DcmUnsignedLong delem(DCM_ReferencedContentItemIdentifier);
    /* clear before reaidng */
    ReferencedContentItem.clear();
    ReferencedNodeID = 0;
    /* read ReferencedContentItemIdentifier */
    E_Condition result = getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", logStream);
    if (result == EC_Normal)
    {
        /* create reference string from unsigned long values */
        Uint32 value = 0;
        char buffer[20];
        const unsigned long count = delem.getVM();
        for (unsigned long i = 0; i < count; i++)
        {
            if (i > 0)
                ReferencedContentItem += '.';
            if (delem.getUint32(value, i) == EC_Normal)
                ReferencedContentItem += DSRTypes::numberToString((size_t)value, buffer);
        }
    }
    return result;
}


E_Condition DSRByReferenceTreeNode::writeContentItem(DcmItem &dataset,
                                                     OFConsole * /* logStream */) const
{
    E_Condition result = EC_IllegalCall;
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


E_Condition DSRByReferenceTreeNode::renderHTMLContentItem(ostream &docStream,
                                                          ostream & /* annexStream */,
                                                          const size_t /* nestingLevel */,
                                                          size_t & /* annexNumber */,
                                                          const size_t /* flags */,
                                                          OFConsole * /* logStream */) const
{
    /* render reference string */
    docStream << "Content Item <a href=\"#content_item_" << ReferencedNodeID << "\">by-reference</a>" << endl;
    return EC_Normal;
}


E_Condition DSRByReferenceTreeNode::setConceptName(const DSRCodedEntryValue & /* conceptName */)
{
    /* invalid: no concept name allowed */
    return EC_IllegalCall;
}


E_Condition DSRByReferenceTreeNode::setObservationDateTime(const OFString & /* observationDateTime */)
{
    /* invalid: no observation date and time allowed */
    return EC_IllegalCall;
}


OFBool DSRByReferenceTreeNode::canAddNode(const E_DocumentType /* documentType */,
                                          const E_RelationshipType /* relationshipType */,
                                          const E_ValueType /* valueType */,
                                          const OFBool /* byReference */) const
{
    /* invalid: no child nodes allowed */
    return OFFalse;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrreftn.cc,v $
 *  Revision 1.3  2000-11-07 18:33:31  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.2  2000/11/01 16:37:02  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.1  2000/10/26 14:39:58  joergr
 *  Added support for "Comprehensive SR".
 *
 */
