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
 *    classes: DSRNumTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-09-15 14:13:42 $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrnumtn.h"
#include "dsrxmld.h"


DSRNumTreeNode::DSRNumTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_Num),
   DSRNumericMeasurementValue()
{
}


DSRNumTreeNode::~DSRNumTreeNode()
{
}


void DSRNumTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRNumericMeasurementValue::clear();
}


OFBool DSRNumTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRNumericMeasurementValue::isValid() && getConceptName().isValid();
}


OFCondition DSRNumTreeNode::print(ostream &stream,
                                  const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
    {
        stream << "=";
        result = DSRNumericMeasurementValue::print(stream, flags);
    }
    return result;
}


OFCondition DSRNumTreeNode::writeXML(ostream &stream,
                                     const size_t flags,
                                     OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags);
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    DSRNumericMeasurementValue::writeXML(stream, flags, logStream);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRNumTreeNode::readContentItem(DcmItem &dataset,
                                            OFConsole *logStream)
{
    return DSRNumericMeasurementValue::readSequence(dataset, logStream);
}


OFCondition DSRNumTreeNode::writeContentItem(DcmItem &dataset,
                                             OFConsole *logStream) const
{
    return DSRNumericMeasurementValue::writeSequence(dataset, logStream);
}


OFCondition DSRNumTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                               DSRXMLCursor cursor)
{
    return DSRNumericMeasurementValue::readXML(doc, cursor);
}


OFCondition DSRNumTreeNode::renderHTMLContentItem(ostream &docStream,
                                                  ostream &annexStream,
                                                  const size_t /*nestingLevel*/,
                                                  size_t &annexNumber,
                                                  const size_t flags,
                                                  OFConsole *logStream) const
{
    /* render ConceptName */
    OFCondition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render Num */
    if (result.good())
    {
        result = DSRNumericMeasurementValue::renderHTML(docStream, annexStream, annexNumber, flags, logStream);
        docStream << endl;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrnumtn.cc,v $
 *  Revision 1.16  2003-09-15 14:13:42  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 *  Revision 1.15  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.14  2003/08/07 13:39:00  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.13  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.12  2001/11/19 13:16:59  joergr
 *  Adapted implementation according to correction proposal 266.
 *
 *  Revision 1.11  2001/11/09 16:16:48  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.10  2001/10/10 15:29:57  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.9  2001/09/26 13:04:22  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.8  2001/05/07 16:14:24  joergr
 *  Updated CVS header.
 *
 *  Revision 1.7  2001/02/02 14:41:52  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.6  2000/11/07 18:33:30  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.5  2000/11/01 16:37:00  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.4  2000/10/26 14:32:09  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.3  2000/10/23 15:04:46  joergr
 *  Added clear() method.
 *
 *  Revision 1.2  2000/10/16 12:06:20  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:22  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
