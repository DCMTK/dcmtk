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
 *  Update Date:      $Date: 2000-10-16 12:00:39 $
 *  CVS/RCS Revision: $Revision: 1.2 $
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


OFBool DSRTextTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRStringValue::isValid() && getConceptName().isValid();
}


OFBool DSRTextTreeNode::isShort(const size_t /* flags */) const
{
    return (getValue().length() <= 30);
}


E_Condition DSRTextTreeNode::print(ostream &stream,
                                   const size_t flags) const
{
    E_Condition result = DSRDocumentTreeNode::print(stream, flags);
    if (result == EC_Normal)
    {
        stream << "=";
        if (flags & PF_shortenLongItemValues)
            DSRStringValue::print(stream, 40);     // text output is limited to 40 characters
        else
            DSRStringValue::print(stream);
    }
    return result;
}


E_Condition DSRTextTreeNode::readContentItem(DcmItem &dataset,
                                             OFConsole * /* logStream */)
{
    OFString string;
    /* read TextValue */
    E_Condition result = getStringValueFromDataset(dataset, DCM_TextValue, string);
    if (result == EC_Normal)
        result = setValue(string);
    return result;
}


E_Condition DSRTextTreeNode::writeContentItem(DcmItem &dataset,
                                              OFConsole * /* logStream */) const
{
    /* write TextValue */
    return putStringValueToDataset(dataset, DCM_TextValue, getValue());
}


E_Condition DSRTextTreeNode::renderHTMLContentItem(ostream &docStream,
                                                   ostream & /*annexStream */,
                                                   const size_t /* nestingLevel */,
                                                   size_t & /* annexNumber */,
                                                   const size_t flags,
                                                   OFConsole *logStream) const
{
    /* render ConceptName */
    E_Condition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render TextValue */
    if (flags & HF_renderItemInline)
        docStream << "\"" << getValue() << "\"" << endl;
    else
        docStream << getValue() << endl;
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
            switch (valueType)
            {
                case VT_Text:
                case VT_Code:
                    result = OFTrue;
                    break;
                default:
                    break;
            }
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
                case VT_Container:
                    result = (documentType == DT_ComprehensiveSR);  /* only by-reference - to be checked ! */
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return result;
}


OFBool DSRTextTreeNode::checkValue(const OFString &stringValue) const
{
    return (stringValue.length() > 0);
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtextn.cc,v $
 *  Revision 1.2  2000-10-16 12:00:39  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:26  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
