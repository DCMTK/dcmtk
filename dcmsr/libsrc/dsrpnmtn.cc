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
 *    classes: DSRPNameTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-11-07 18:31:13 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrpnmtn.h"


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


E_Condition DSRPNameTreeNode::print(ostream &stream,
                                    const size_t flags) const
{
    E_Condition result = DSRDocumentTreeNode::print(stream, flags);
    if (result == EC_Normal)
    {
        stream << "=";
        DSRStringValue::print(stream);
    }
    return result;
}


E_Condition DSRPNameTreeNode::writeXML(ostream &stream,
                                       const size_t flags,
                                       OFConsole *logStream) const
{
    E_Condition result = EC_Normal;
    stream << "<pname";
    if (isReferenceTarget())
        stream << " id=\"" << getNodeID() << "\"";
    stream << ">" << endl;
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    writeStringValueToXML(stream, getValue(), "value", flags & XF_writeEmptyTags);
    stream << "</pname>" << endl;
    return result;
}


E_Condition DSRPNameTreeNode::readContentItem(DcmItem &dataset,
                                              OFConsole *logStream)
{
    /* read PName */
    return DSRStringValue::read(dataset, DCM_PersonName, logStream);
}


E_Condition DSRPNameTreeNode::writeContentItem(DcmItem &dataset,
                                               OFConsole *logStream) const
{
    /* write PName */
    return DSRStringValue::write(dataset, DCM_PersonName, logStream);
}


E_Condition DSRPNameTreeNode::renderHTMLContentItem(ostream &docStream,
                                                    ostream & /* annexStream */,
                                                    const size_t /* nestingLevel */,
                                                    size_t & /* annexNumber */,
                                                    const size_t flags,
                                                    OFConsole *logStream) const
{
    /* render ConceptName */
    E_Condition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render PName */
    if (result == EC_Normal)
    {
        OFString string, htmlString;
        if (!(flags & DSRTypes::HF_renderItemsSeparately))
            docStream << "<u>";
        docStream << convertToMarkupString(dicomToReadablePersonName(getValue(), string), htmlString);
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
    if (!byReference || (documentType == DT_ComprehensiveSR))
    {
        if (relationshipType == RT_hasConceptMod)
            result = (valueType == VT_Text) || (valueType == VT_Code);
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrpnmtn.cc,v $
 *  Revision 1.7  2000-11-07 18:31:13  joergr
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
