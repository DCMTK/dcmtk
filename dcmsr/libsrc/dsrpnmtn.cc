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
 *  Update Date:      $Date: 2000-10-13 07:52:23 $
 *  CVS/RCS Revision: $Revision: 1.1 $
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
        stream << " = ";
        DSRStringValue::print(stream);
    }
    return result;
}


E_Condition DSRPNameTreeNode::readContentItem(DcmItem &dataset,
                                              OFConsole * /* logStream */)
{
    OFString string;
    /* read PName */
    E_Condition result = getStringValueFromDataset(dataset, DCM_PersonName, string);
    if (result == EC_Normal)
        result = setValue(string);
    return result;
}


E_Condition DSRPNameTreeNode::writeContentItem(DcmItem &dataset,
                                               OFConsole * /* logStream */) const
{
    /* write PName */
    return putStringValueToDataset(dataset, DCM_PersonName, getValue());
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
    docStream << getValue() << endl;
    return result;
}


OFBool DSRPNameTreeNode::canAddNode(const E_DocumentType /* documentType */,
                                    const E_RelationshipType relationshipType,
                                    const E_ValueType valueType) const
{
    OFBool result = OFFalse;
    if (relationshipType == RT_hasConceptMod)
    {
        if ((valueType == VT_Text) || (valueType == VT_Code))
            result = OFTrue;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrpnmtn.cc,v $
 *  Revision 1.1  2000-10-13 07:52:23  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
