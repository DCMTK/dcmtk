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
 *    classes: DSRTextTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-11-01 16:37:05 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtextn.h"
#include "dsrstrvl.h"


DSRTextTreeNode::DSRTextTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_Text),
   DSRStringValue()
{
}


DSRTextTreeNode::DSRTextTreeNode(const E_RelationshipType relationshipType,
                                 const OFString &stringValue)
 : DSRDocumentTreeNode(relationshipType, VT_Text),
   DSRStringValue(stringValue)
{
}


DSRTextTreeNode::~DSRTextTreeNode()
{
}


void DSRTextTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRTextTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRStringValue::isValid() && getConceptName().isValid();
}


OFBool DSRTextTreeNode::isShort(const size_t /* flags */) const
{
    return (getValue().length() <= 40);
}


E_Condition DSRTextTreeNode::print(ostream &stream,
                                   const size_t flags) const
{
    E_Condition result = DSRDocumentTreeNode::print(stream, flags);
    if (result == EC_Normal)
    {
        stream << "=";
        if (flags & PF_shortenLongItemValues)
            DSRStringValue::print(stream, 30);     // text output is limited to 30 characters
        else
            DSRStringValue::print(stream);
    }
    return result;
}


E_Condition DSRTextTreeNode::writeXML(ostream &stream,
                                      const size_t flags,
                                      OFConsole *logStream) const
{
    E_Condition result = EC_Normal;
    stream << "<text>" << endl;
    result = DSRDocumentTreeNode::writeXML(stream, flags, logStream);
    writeStringValueToXML(stream, getValue(), "value", flags & XF_writeEmptyTags);
    stream << "</text>" << endl;
    return result;
}


E_Condition DSRTextTreeNode::readContentItem(DcmItem &dataset,
                                             OFConsole *logStream)
{
    /* read TextValue */
    return DSRStringValue::read(dataset, DCM_TextValue, logStream);
}


E_Condition DSRTextTreeNode::writeContentItem(DcmItem &dataset,
                                              OFConsole *logStream) const
{
    /* write TextValue */
    return DSRStringValue::write(dataset, DCM_TextValue, logStream);
}


E_Condition DSRTextTreeNode::renderHTMLContentItem(ostream &docStream,
                                                   ostream & /*annexStream */,
                                                   const size_t /* nestingLevel */,
                                                   size_t & /* annexNumber */,
                                                   const size_t flags,
                                                   OFConsole *logStream) const
{
    OFString htmlString;
    /* render ConceptName */
    E_Condition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render TextValue */
    if (flags & HF_renderItemInline)
        docStream << "\"" << convertToMarkupString(getValue(), htmlString) << "\"" << endl;
    else
        docStream << convertToMarkupString(getValue(), htmlString, OFTrue /* newlineAllowed */) << endl;
    return result;
}


OFBool DSRTextTreeNode::canAddNode(const E_DocumentType documentType,
                                   const E_RelationshipType relationshipType,
                                   const E_ValueType valueType) const
{
    OFBool result = OFFalse;
    switch (relationshipType)
    {
        case RT_hasConceptMod:
            result = (valueType == VT_Text) || (valueType == VT_Code);
            break;
        case RT_inferredFrom:
        case RT_hasProperties:
            switch (valueType)
            {
                case VT_Text:
                case VT_Code:
                case VT_DateTime:
                case VT_Date:
                case VT_Time:
                case VT_UIDRef:
                case VT_PName:
                case VT_Composite:
                case VT_Image:
                case VT_Waveform:
                    result = OFTrue;
                    break;
                case VT_Num:
                case VT_SCoord:
                case VT_TCoord:
                    result = (documentType == DT_EnhancedSR) || (documentType == DT_ComprehensiveSR);
                    break;
/*
                case VT_Container:
                    result = (documentType == DT_ComprehensiveSR);  // only by-reference - to be checked !
                    break;
*/
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtextn.cc,v $
 *  Revision 1.6  2000-11-01 16:37:05  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.5  2000/10/26 14:35:09  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.4  2000/10/23 15:04:46  joergr
 *  Added clear() method.
 *
 *  Revision 1.3  2000/10/18 17:23:12  joergr
 *  Moved read and write methods to base class.
 *
 *  Revision 1.2  2000/10/16 12:00:39  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:26  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
