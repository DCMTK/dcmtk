/*
 *
 *  Copyright (C) 2000-2001, OFFIS
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
 *    classes: DSRTimeTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2001-05-07 16:14:26 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtimtn.h"


DSRTimeTreeNode::DSRTimeTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_Time),
   DSRStringValue()
{
}


DSRTimeTreeNode::DSRTimeTreeNode(const E_RelationshipType relationshipType,
                                 const OFString &stringValue)
 : DSRDocumentTreeNode(relationshipType, VT_Time),
   DSRStringValue(stringValue)
{
}


DSRTimeTreeNode::~DSRTimeTreeNode()
{
}


void DSRTimeTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRTimeTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRStringValue::isValid() && getConceptName().isValid();
}


E_Condition DSRTimeTreeNode::print(ostream &stream,
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


E_Condition DSRTimeTreeNode::writeXML(ostream &stream,
                                      const size_t flags,
                                      OFConsole *logStream) const
{
    E_Condition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    writeStringValueToXML(stream, getValue(), "value", flags & XF_writeEmptyTags);
    writeXMLItemEnd(stream, flags);
    return result;
}


E_Condition DSRTimeTreeNode::readContentItem(DcmItem &dataset,
                                             OFConsole *logStream)
{
    /* read Time */
    return DSRStringValue::read(dataset, DCM_Time, logStream);
}


E_Condition DSRTimeTreeNode::writeContentItem(DcmItem &dataset,
                                              OFConsole *logStream) const
{
    /* write Time */
    return DSRStringValue::write(dataset, DCM_Time, logStream);
}


E_Condition DSRTimeTreeNode::renderHTMLContentItem(ostream &docStream,
                                                   ostream & /* annexStream */,
                                                   const size_t /* nestingLevel */,
                                                   size_t & /* annexNumber */,
                                                   const size_t flags,
                                                   OFConsole *logStream) const
{
    /* render ConceptName */
    E_Condition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render Time */
    if (result == EC_Normal)
    {
        OFString htmlString;
        if (!(flags & DSRTypes::HF_renderItemsSeparately))
            docStream << "<u>";
        docStream << dicomToReadableTime(getValue(), htmlString);
        if (!(flags & DSRTypes::HF_renderItemsSeparately))
            docStream << "</u>";
        docStream << endl;
    }
    return result;
}


OFBool DSRTimeTreeNode::canAddNode(const E_DocumentType documentType,
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
 *  $Log: dsrtimtn.cc,v $
 *  Revision 1.9  2001-05-07 16:14:26  joergr
 *  Updated CVS header.
 *
 *  Revision 1.8  2001/02/02 14:41:50  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.7  2000/11/07 18:30:21  joergr
 *  Enhanced support for by-reference relationships.
 *  Enhanced rendered HTML output of date, time, datetime and pname.
 *
 *  Revision 1.6  2000/11/01 16:37:06  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.5  2000/10/26 14:35:09  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.4  2000/10/23 15:04:47  joergr
 *  Added clear() method.
 *
 *  Revision 1.3  2000/10/18 17:23:12  joergr
 *  Moved read and write methods to base class.
 *
 *  Revision 1.2  2000/10/16 12:08:51  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:26  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
