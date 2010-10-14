/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRContainerTreeNode
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.28 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrcontn.h"
#include "dcmtk/dcmsr/dsrxmld.h"


DSRContainerTreeNode::DSRContainerTreeNode(const E_RelationshipType relationshipType,
                                           const E_ContinuityOfContent continuityOfContent)
 : DSRDocumentTreeNode(relationshipType, VT_Container),
   ContinuityOfContent(continuityOfContent)
{
}


DSRContainerTreeNode::~DSRContainerTreeNode()
{
}


void DSRContainerTreeNode::clear()
{
    ContinuityOfContent = COC_Separate;      // this is more useful that COC_invalid
}


OFBool DSRContainerTreeNode::isValid() const
{
    /* ConceptNameCodeSequence required for root node container */
    return DSRDocumentTreeNode::isValid() && (ContinuityOfContent != COC_invalid) &&
        ((getRelationshipType() != RT_isRoot) || getConceptName().isValid());
}


OFBool DSRContainerTreeNode::isShort(const size_t /*flags*/) const
{
    return OFFalse;
}


OFCondition DSRContainerTreeNode::print(STD_NAMESPACE ostream &stream,
                                        const size_t flags) const
{
    OFCondition result = DSRDocumentTreeNode::print(stream, flags);
    if (result.good())
        stream << "=" << continuityOfContentToEnumeratedValue(ContinuityOfContent);
    return result;
}


OFCondition DSRContainerTreeNode::readContentItem(DcmItem &dataset)
{
    OFString tmpString;
    /* read ContinuityOfContent */
    OFCondition result = getAndCheckStringValueFromDataset(dataset, DCM_ContinuityOfContent, tmpString, "1", "1", "CONTAINER content item");
    if (result.good())
    {
        ContinuityOfContent = enumeratedValueToContinuityOfContent(tmpString);
        /* check ContinuityOfContent value */
        if (ContinuityOfContent == COC_invalid)
        {
            printUnknownValueWarningMessage("ContinuityOfContent value", tmpString.c_str());
            result = SR_EC_InvalidValue;
        }
    }
    return result;
}


OFCondition DSRContainerTreeNode::writeContentItem(DcmItem &dataset) const
{
    /* write ContinuityOfContent */
    return putStringValueToDataset(dataset, DCM_ContinuityOfContent, continuityOfContentToEnumeratedValue(ContinuityOfContent));
}


OFCondition DSRContainerTreeNode::readXMLContentItem(const DSRXMLDocument &doc,
                                                     DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_CorruptedXMLStructure;
    if (cursor.valid())
    {
        OFString tmpString;
        /* read 'flag' and check validity */
        ContinuityOfContent = enumeratedValueToContinuityOfContent(doc.getStringFromAttribute(cursor, tmpString, "flag"));
        if (ContinuityOfContent == COC_invalid)
        {
            printUnknownValueWarningMessage("CONTAINER flag", tmpString.c_str());
            result = SR_EC_InvalidValue;
        } else
            result = EC_Normal;
    }
    return result;
}


OFCondition DSRContainerTreeNode::writeXML(STD_NAMESPACE ostream &stream,
                                           const size_t flags) const
{
    OFCondition result = EC_Normal;
    writeXMLItemStart(stream, flags, OFFalse /*closingBracket*/);
    stream << " flag=\"" << continuityOfContentToEnumeratedValue(ContinuityOfContent) << "\"";
    stream << ">" << OFendl;
    result = DSRDocumentTreeNode::writeXML(stream, flags);
    writeXMLItemEnd(stream, flags);
    return result;
}


OFCondition DSRContainerTreeNode::renderHTMLContentItem(STD_NAMESPACE ostream &docStream,
                                                        STD_NAMESPACE ostream & /*annexStream*/,
                                                        const size_t nestingLevel,
                                                        size_t & /*annexNumber*/,
                                                        const size_t flags) const
{
    /* section heading (optional) */
    if (nestingLevel > 0)
    {
        /* render ConceptName & Code (if valid) */
        if (!getConceptName().getCodeMeaning().empty())
        {
            const size_t section = (nestingLevel > 6) ? 6 : nestingLevel;
            docStream << "<h" << section << ">";
            getConceptName().renderHTML(docStream, flags, (flags & HF_renderConceptNameCodes) && getConceptName().isValid() /*fullCode*/);
            docStream << "</h" << section << ">" << OFendl;
        }
        /* render optional observation datetime */
        if (!getObservationDateTime().empty())
        {
            OFString tmpString;
            docStream << "<p>" << OFendl;
            if (flags & HF_XHTML11Compatibility)
                docStream << "<span class=\"observe\">";
            else
                docStream << "<small>";
            docStream << "(observed: " << dicomToReadableDateTime(getObservationDateTime(), tmpString) << ")";
            if (flags & HF_XHTML11Compatibility)
                docStream << "</span>" << OFendl;
            else
                docStream << "</small>" << OFendl;
            docStream << "</p>" << OFendl;
        }
    }
    return EC_Normal;
}


OFCondition DSRContainerTreeNode::renderHTML(STD_NAMESPACE ostream &docStream,
                                             STD_NAMESPACE ostream &annexStream,
                                             const size_t nestingLevel,
                                             size_t &annexNumber,
                                             const size_t flags) const
{
    /* check for validity */
    if (!isValid())
        printInvalidContentItemMessage("Rendering", this);
    /* render content item */
    OFCondition result = renderHTMLContentItem(docStream, annexStream, nestingLevel, annexNumber, flags);
    if (result.good())
    {
        /* section body: render child nodes */
        if (ContinuityOfContent == COC_Continuous)
            result = renderHTMLChildNodes(docStream, annexStream, nestingLevel, annexNumber, flags & ~HF_renderItemsSeparately);
        else  // might be invalid
            result = renderHTMLChildNodes(docStream, annexStream, nestingLevel, annexNumber, flags | HF_renderItemsSeparately);
    } else
        printContentItemErrorMessage("Rendering", result, this);
    return result;
}


OFCondition DSRContainerTreeNode::setContinuityOfContent(const E_ContinuityOfContent continuityOfContent)
{
    OFCondition result = EC_IllegalParameter;
    if (continuityOfContent != COC_invalid)
    {
        ContinuityOfContent = continuityOfContent;
        result = EC_Normal;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrcontn.cc,v $
 *  Revision 1.28  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.27  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.26  2007-11-15 16:45:26  joergr
 *  Added support for output in XHTML 1.1 format.
 *  Enhanced support for output in valid HTML 3.2 format. Migrated support for
 *  standard HTML from version 4.0 to 4.01 (strict).
 *
 *  Revision 1.25  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.24  2005/12/08 15:47:45  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.23  2003/09/15 14:13:42  joergr
 *  Introduced new class to facilitate checking of SR IOD relationship content
 *  constraints. Replaced old implementation distributed over numerous classes.
 *
 *  Revision 1.22  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.21  2003/08/07 13:14:59  joergr
 *  Added readXML functionality.
 *
 *  Revision 1.20  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.19  2002/12/05 13:52:52  joergr
 *  Added further checks when reading SR documents (e.g. value of VerificationFlag,
 *  CompletionsFlag, ContinuityOfContent and SpecificCharacterSet).
 *
 *  Revision 1.18  2002/11/25 12:00:34  joergr
 *  Adapted code according to CP 286, i.e. allow a COMPOSITE content item to be
 *  the target of a HAS OBS CONTEXT relationship with a CONTAINER item.
 *
 *  Revision 1.17  2001/11/09 16:14:10  joergr
 *  Added preliminary support for Mammography CAD SR.
 *
 *  Revision 1.16  2001/10/10 15:29:50  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.15  2001/09/26 13:04:18  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.14  2001/05/07 16:14:22  joergr
 *  Updated CVS header.
 *
 *  Revision 1.13  2001/02/02 14:41:54  joergr
 *  Added new option to dsr2xml allowing to specify whether value and/or
 *  relationship type are to be encoded as XML attributes or elements.
 *
 *  Revision 1.12  2001/01/25 15:32:17  joergr
 *  Allow invalid continuity of content flag when reading SR datasets.
 *
 *  Revision 1.11  2000/11/14 11:18:59  joergr
 *  Added output of optional observation datetime to rendered HTML page.
 *
 *  Revision 1.10  2000/11/09 20:34:00  joergr
 *  Added support for non-ASCII characters in HTML 3.2 (use numeric value).
 *
 *  Revision 1.9  2000/11/09 11:32:12  joergr
 *  Minor HTML code purifications.
 *
 *  Revision 1.8  2000/11/07 18:33:29  joergr
 *  Enhanced support for by-reference relationships.
 *
 *  Revision 1.7  2000/11/01 16:31:17  joergr
 *  Added support for conversion to XML.
 *
 *  Revision 1.6  2000/10/26 14:27:47  joergr
 *  Added support for "Comprehensive SR".
 *
 *  Revision 1.5  2000/10/23 15:02:49  joergr
 *  Enhanced implementation of method isValid().
 *
 *  Revision 1.4  2000/10/19 16:03:48  joergr
 *  Added optional module name to read method to provide more detailed warning
 *  messages.
 *
 *  Revision 1.3  2000/10/18 17:14:30  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.2  2000/10/16 12:01:56  joergr
 *  Reformatted print output.
 *
 *  Revision 1.1  2000/10/13 07:52:18  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
