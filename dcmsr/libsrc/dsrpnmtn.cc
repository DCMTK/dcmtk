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
 *    classes: DSRPNameTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-08-07 17:29:13 $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrpnmtn.h"
#include "dsrxmld.h"


DSRPNameTreeNode::DSRPNameTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_PName),
   DSRStringValue()
{
}


DSRPNameTreeNode::DSRPNameTreeNode(const E_RelationshipType relationshipType,
                                   const OFString &stringValue)
 : DSRDocumentTreeNode(relationshipType, VT_PName),
   DSRStringValue(stringValue)
{
}


DSRPNameTreeNode::~DSRPNameTreeNode()
{
}


void DSRPNameTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRPNameTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRStringValue::isValid() && getConceptName().isValid();
}


OFCondition DSRPNameTreeNode::print(ostream &stream,
                                    const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        stream << "=";
        DSRStringValue::print(stream);
    }
    return result;
}


OFCondition DSRPNameTreeNode::writeXML(ostream &stream,
                                       const size_t flags,
                                       OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    if (!getValue().empty() || (flags & XF_writeEmptyTags))
    {
        OFString tmpString;
        stream << "<value>" << endl << dicomToXMLPersonName(getValue(), tmpString) << "</value>" << endl;
    }
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRPNameTreeNode::readContentItem(DcmItem &dataset,
                                              OFConsole *logStream)
{
    /* read PName */
    return DSRStringValue::read(dataset, DCM_PersonName, logStream);
}


OFCondition DSRPNameTreeNode::writeContentItem(DcmItem &dataset,
                                               OFConsole *logStream) const
{
    /* write PName */
    return DSRStringValue::write(dataset, DCM_PersonName, logStream);
}


OFCondition DSRPNameTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                 DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        /* goto sub-node "value" */
        cursor = doc.getNamedNode(cursor.getChild(), "value").getChild();
        if (cursor.valid())
        {
            /* retrieve person name from XML tag */
            OFString nameString;
            getValueFromXMLNodeContent(doc, cursor, nameString);
            /* set retrieved value */
            result = DSRStringValue::setValue(nameString);
        }
    }
    return result;
}


OFString &DSRPNameTreeNode::getValueFromXMLNodeContent(const DSRXMLDocument &doc,
                                                       DSRXMLCursor cursor,
                                                       OFString &nameValue)
{
    nameValue.clear();
    /* check whether node is valid */
    if (cursor.valid())
    {
        OFString first, middle, last, suffix, prefix;
        /* iterate over all nodes */
        while (cursor.valid())
        {
            /* check for known element tags */
            doc.getStringFromNodeContent(cursor, prefix, "prefix", OFTrue /*encoding*/, OFFalse /*clearString*/);
            doc.getStringFromNodeContent(cursor, first, "first", OFTrue /*encoding*/, OFFalse /*clearString*/);
            doc.getStringFromNodeContent(cursor, middle, "middle", OFTrue /*encoding*/, OFFalse /*clearString*/);
            doc.getStringFromNodeContent(cursor, last, "last", OFTrue /*encoding*/, OFFalse /*clearString*/);
            doc.getStringFromNodeContent(cursor, suffix, "suffix", OFTrue /*encoding*/, OFFalse /*clearString*/);
            /* proceed with next node */
            cursor.gotoNext();
        }
        /* create DICOM Person Name (PN) from name components */
        DcmPersonName::getStringFromNameComponents(last, first, middle, prefix, suffix, nameValue);
    }
    return nameValue;
}


OFCondition DSRPNameTreeNode::renderHTMLContentItem(ostream &docStream,
                                                    ostream & /*annexStream*/,
                                                    const size_t /*nestingLevel*/,
                                                    size_t & /*annexNumber*/,
                                                    const size_t flags,
                                                    OFConsole *logStream) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render PName */
    if (result.good())
    {
        OFString tmpString, htmlString;
        if (!(flags & DSRTypes::HF_renderItemsSeparately))
            docStream << "<u>";
        docStream << convertToMarkupString(dicomToReadablePersonName(getValue(), tmpString), htmlString, (flags & HF_convertNonASCIICharacters) > 0);
        if (!(flags & DSRTypes::HF_renderItemsSeparately))
            docStream << "</u>";
        docStream << endl;
    }
    return result;
}


OFBool DSRPNameTreeNode::canAddNode(const E_DocumentType documentType,
                                    const E_RelationshipType relationshipType,
                                    const E_ValueType valueType,
                                    const OFBool byReference) const
{
    OFBool result = OFFalse;
    if ((!byReference && (documentType != DT_KeyObjectDoc)) || (documentType == DT_ComprehensiveSR))
    {
        if (relationshipType == RT_hasConceptMod)
            result = (valueType == VT_Text) || (valueType == VT_Code);
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrpnmtn.cc,v $
 *  Revision 1.18  2003-08-07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.17  2003/08/07 15:21:53  joergr
 *  Added brackets around "bitwise and" operator/operands to avoid warnings
 *  reported by MSVC5.
 *
 *  Revision 1.16  2003/08/07 13:41:36  joergr
 *  Added readXML functionality.
 *  Renamed parameters/variables "string" to avoid name clash with STL class.
 *
 *  Revision 1.15  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.14  2001/11/09 16:17:53  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.13  2001/10/10 15:29:58  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.12  2001/09/26 13:04:23  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.11  2001/05/07 16:14:24  joergr
 *  Updated CVS header.
 *
 *  Revision 1.10  2001/02/02 14:41:52  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.9  2001/01/18 15:56:18  joergr
 *  Encode PN components in separate XML tags.
 *
 *  Revision 1.8  2000/11/09 20:34:01  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.7  2000/11/07 18:31:13  joergr
 *  Enhanced support for by-reference relationships.
 *  Enhanced rendered HTML output of date, time, datetime and pname.
 *
 *  Revision 1.6  2000/11/01 16:37:02  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.5  2000/10/26 14:32:09  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.4  2000/10/23 15:04:46  joergr
 *  Added clear() method.
 *
 *  Revision 1.3  2000/10/18 17:20:20  joergr
 *  Moved read and write methods to base class.
 *
 *  Revision 1.2  2000/10/16 12:06:21  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:23  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
