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
 *    classes: DSRUIDRefTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-23 15:04:47 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsruidtn.h"


DSRUIDRefTreeNode::DSRUIDRefTreeNode(const E_RelationshipType relationshipType)
 : DSRDocumentTreeNode(relationshipType, VT_UIDRef),
   DSRStringValue()
{
}


DSRUIDRefTreeNode::DSRUIDRefTreeNode(const E_RelationshipType relationshipType,
                                     const OFString &stringValue)
 : DSRDocumentTreeNode(relationshipType, VT_UIDRef),
   DSRStringValue(stringValue)
{
}


DSRUIDRefTreeNode::~DSRUIDRefTreeNode()
{
}


void DSRUIDRefTreeNode::clear()
{
    DSRDocumentTreeNode::clear();
    DSRStringValue::clear();
}


OFBool DSRUIDRefTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required */
    return DSRDocumentTreeNode::isValid() && DSRStringValue::isValid() && getConceptName().isValid();
}


E_Condition DSRUIDRefTreeNode::print(ostream &stream,
                                     const size_t flags) const
{
    E_Condition result = DSRDocumentTreeNode::print(stream, flags);
    if (result ==EC_Normal)
    {
        stream << "=";
        DSRStringValue::print(stream);
    }
    return result;
}


E_Condition DSRUIDRefTreeNode::readContentItem(DcmItem &dataset,
                                               OFConsole *logStream)
{
    /* read UID */
    return DSRStringValue::read(dataset, DCM_UID, logStream);
}


E_Condition DSRUIDRefTreeNode::writeContentItem(DcmItem &dataset,
                                                OFConsole *logStream) const
{
    /* write UID */
    return DSRStringValue::write(dataset, DCM_UID, logStream);
}


E_Condition DSRUIDRefTreeNode::renderHTMLContentItem(ostream &docStream,
                                                     ostream & /* annexStream */,
                                                     const size_t /* nestingLevel */,
                                                     size_t & /* annexNumber */,
                                                     const size_t flags,
                                                     OFConsole *logStream) const
{
    /* render ConceptName */
    E_Condition result = renderHTMLConceptName(docStream, flags, logStream);
    /* render UID */
    docStream << getValue() << endl;
    return result;
}


OFBool DSRUIDRefTreeNode::canAddNode(const E_DocumentType /* documentType */,
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
 *  $Log: dsruidtn.cc,v $
 *  Revision 1.4  2000-10-23 15:04:47  joergr
 *  Added clear() method.
 *
 *  Revision 1.3  2000/10/18 17:24:22  joergr
 *  Moved read and write methods to base class.
 *
 *  Revision 1.2  2000/10/16 12:10:23  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:28  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
