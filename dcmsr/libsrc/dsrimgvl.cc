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
 *    classes: DSRImageReferenceValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-11-01 16:37:00 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrimgvl.h"


DSRImageReferenceValue::DSRImageReferenceValue()
  : DSRCompositeReferenceValue(),
    PresentationState(),
    FrameList()
{
}


DSRImageReferenceValue::DSRImageReferenceValue(const OFString &sopClassUID,
                                               const OFString &sopInstanceUID)
  : DSRCompositeReferenceValue(),
    PresentationState(),
    FrameList()
{
    /* check for appropriate SOP class UID */
    setReference(sopClassUID, sopInstanceUID);
}


DSRImageReferenceValue::DSRImageReferenceValue(const OFString &imageSOPClassUID,
                                               const OFString &imageSOPInstanceUID,
                                               const OFString &pstateSOPClassUID,
                                               const OFString &pstateSOPInstanceUID)
  : DSRCompositeReferenceValue(),
    PresentationState(),
    FrameList()
{
    /* check for appropriate SOP class UID */
    setReference(imageSOPClassUID, imageSOPInstanceUID);
    setPresentationState(DSRCompositeReferenceValue(pstateSOPClassUID, pstateSOPInstanceUID));
}


DSRImageReferenceValue::DSRImageReferenceValue(const DSRImageReferenceValue &referenceValue)
  : DSRCompositeReferenceValue(referenceValue),
    PresentationState(referenceValue.PresentationState),
    FrameList(referenceValue.FrameList)
{
    /* do not check since this would unexpected to the user */
}


DSRImageReferenceValue::DSRImageReferenceValue(const DSRCompositeReferenceValue &imageReferenceValue,
                                               const DSRCompositeReferenceValue &pstateReferenceValue)
  : DSRCompositeReferenceValue(),
    PresentationState(),
    FrameList()
{
    /* check for appropriate SOP class UID */
    DSRCompositeReferenceValue::setValue(imageReferenceValue);
    setPresentationState(pstateReferenceValue);
}


DSRImageReferenceValue::~DSRImageReferenceValue()
{
}


DSRImageReferenceValue &DSRImageReferenceValue::operator=(const DSRImageReferenceValue &referenceValue)
{
    DSRCompositeReferenceValue::operator=(referenceValue);
    /* do not check since this would unexpected to the user */
    PresentationState = referenceValue.PresentationState;
    FrameList = referenceValue.FrameList;
    return *this;
}


void DSRImageReferenceValue::clear()
{
    DSRCompositeReferenceValue::clear();
    PresentationState.clear();
    FrameList.clear();
}


OFBool DSRImageReferenceValue::isValid() const
{
    return DSRCompositeReferenceValue::isValid() && checkPresentationState(PresentationState);
}


OFBool DSRImageReferenceValue::isShort(const size_t flags) const
{
    return (FrameList.isEmpty()) || !(flags & DSRTypes::HF_renderFullData);
}


E_Condition DSRImageReferenceValue::print(ostream &stream,
                                          const size_t flags) const
{
    const char *string = dcmSOPClassUIDToModality(SOPClassUID.c_str());
    stream << "(";
    if (string != NULL)
        stream << string << " image";
    else
        stream << "\"" << SOPClassUID << "\"";
    stream << ",";
    if (flags & DSRTypes::PF_printSOPInstanceUID)
        stream << "\"" << SOPInstanceUID << "\"";
    if (!FrameList.isEmpty())
    {
        stream << ",";
        FrameList.print(stream, flags);
    }
    stream << ")";
    if (PresentationState.isValid())
    {
        stream << ",(GSPS,";
        if (flags & DSRTypes::PF_printSOPInstanceUID)
            stream << "\"" << PresentationState.getSOPInstanceUID() << "\"";
        stream << ")";
    }
    return EC_Normal;
}


E_Condition DSRImageReferenceValue::writeXML(ostream &stream,
                                             const size_t flags,
                                             OFConsole *logStream) const
{
    E_Condition result = DSRCompositeReferenceValue::writeXML(stream, flags, logStream);
    if ((flags & DSRTypes::XF_writeEmptyTags) || !FrameList.isEmpty())
    {
        stream << "<frames>" << endl;
        FrameList.print(stream);        
        stream << "</frames>" << endl;        
    }
    if ((flags & DSRTypes::XF_writeEmptyTags) || PresentationState.isValid())
    {
        stream << "<pstate>" << endl;
        PresentationState.writeXML(stream, flags, logStream);
        stream << "</pstate>" << endl;
    }
    return result;
}


E_Condition DSRImageReferenceValue::readItem(DcmItem &dataset,
                                             OFConsole *logStream)
{
    /* read ReferencedSOPClassUID and ReferencedSOPInstanceUID */
    E_Condition result = DSRCompositeReferenceValue::readItem(dataset, logStream);
    /* read ReferencedFrameNumber (conditional) */
    if (result == EC_Normal)
        FrameList.read(dataset, logStream);
    /* read ReferencedSOPSequence (Presentation State, optional) */
    if (result == EC_Normal)
        PresentationState.readSequence(dataset, "3" /* type */, logStream);
    return result;
}


E_Condition DSRImageReferenceValue::writeItem(DcmItem &dataset,
                                              OFConsole *logStream) const
{
    /* write ReferencedSOPClassUID and ReferencedSOPInstanceUID */
    E_Condition result = DSRCompositeReferenceValue::writeItem(dataset, logStream);
    /* write ReferencedFrameNumber (conditional) */
    if (result == EC_Normal)
    {
        if (!FrameList.isEmpty())
            result = FrameList.write(dataset, logStream);
    }
    /* write ReferencedSOPSequence (Presentation State, optional) */
    if (result == EC_Normal)
    {
        if (PresentationState.isValid())
            result = PresentationState.writeSequence(dataset, logStream);
    }
    return result;
}


E_Condition DSRImageReferenceValue::renderHTML(ostream &docStream,
                                               ostream &annexStream,
                                               size_t &annexNumber,
                                               const size_t flags,
                                               OFConsole * /* logStream */) const
{
    /* image reference */
    docStream << "<a href=\"file://dicom/image/" << SOPClassUID << "/" << SOPInstanceUID << "\">";
    const char *string = dcmSOPClassUIDToModality(SOPClassUID.c_str());
    if (string != NULL)
        docStream << string;
    else
        docStream << "unknown";
    docStream << " image</a>";
    /* presentation state */
    if (PresentationState.isValid())
    {
        docStream << " with ";
        docStream << "<a href=\"file://dicom/pstate/";
        docStream << PresentationState.getSOPClassUID() << "/";
        docStream << PresentationState.getSOPInstanceUID() << "\">";
        docStream << " GSPS</a>";
    }
    if (!isShort(flags))
    {
        if (flags & DSRTypes::HF_currentlyInsideAnnex)
        {
            docStream << endl << "<p>" << endl;
            /* render frame list (= print)*/
            docStream << "<b>Referenced Frame Number:</b><br>";
            FrameList.print(docStream);
            docStream << "</p>";
        } else {
            DSRTypes::createHTMLAnnexEntry(docStream, annexStream, "for more details see", annexNumber);
            annexStream << "<p>" << endl;
            /* render frame list (= print)*/
            annexStream << "<b>Referenced Frame Number:</b><br>";
            FrameList.print(annexStream);
            annexStream << "</p>" << endl;
        }
    }
    return EC_Normal;
}


E_Condition DSRImageReferenceValue::getValue(DSRImageReferenceValue &referenceValue) const
{
    referenceValue = *this;
    return EC_Normal;
}


E_Condition DSRImageReferenceValue::setValue(const DSRImageReferenceValue &referenceValue)
{
    E_Condition result = DSRCompositeReferenceValue::setValue(referenceValue);
    if (result == EC_Normal)
    {
        FrameList = referenceValue.FrameList;
        setPresentationState(referenceValue.PresentationState);
    }
    return result;
}


E_Condition DSRImageReferenceValue::setPresentationState(const DSRCompositeReferenceValue &referenceValue)
{
    E_Condition result = EC_IllegalCall;
    if (checkPresentationState(referenceValue))
    {
        PresentationState = referenceValue;
        result = EC_Normal;
    }
    return result;
}


OFBool DSRImageReferenceValue::appliesToFrame(const Sint32 frameNumber) const
{
    OFBool result = OFTrue;
    if (!FrameList.isEmpty())
        result = FrameList.isElement(frameNumber);
    return result;
}


OFBool DSRImageReferenceValue::checkSOPClassUID(const OFString &sopClassUID) const
{
    OFBool result = OFFalse;
    if (DSRCompositeReferenceValue::checkSOPClassUID(sopClassUID))
    {
        /* tbd: might check for IMAGE storage class later on */
        result = OFTrue;
    }
    return result;
}


OFBool DSRImageReferenceValue::checkPresentationState(const DSRCompositeReferenceValue &referenceValue) const
{
    return referenceValue.isEmpty() || (referenceValue.isValid() &&
          (referenceValue.getSOPClassUID() == UID_GrayscaleSoftcopyPresentationStateStorage));
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrimgvl.cc,v $
 *  Revision 1.9  2000-11-01 16:37:00  joergr
 *  Added support for conversion to XML. Optimized HTML rendering.
 *
 *  Revision 1.8  2000/10/26 14:31:44  joergr
 *  Use method isShort() to decide whether a content item can be rendered
 *  "inline" or not.
 *
 *  Revision 1.7  2000/10/24 15:04:11  joergr
 *  Changed HTML hyperlinks to referenced objects from "dicom://" to "file://"
 *  to facilitate access from Java.
 *
 *  Revision 1.6  2000/10/23 15:01:05  joergr
 *  Added SOP class UID to hyperlink in method renderHTML().
 *
 *  Revision 1.5  2000/10/20 10:14:58  joergr
 *  Renamed class DSRReferenceValue to DSRCompositeReferenceValue.
 *
 *  Revision 1.4  2000/10/19 16:04:42  joergr
 *  Renamed some set methods.
 *
 *  Revision 1.3  2000/10/18 17:19:12  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.2  2000/10/16 12:05:32  joergr
 *  Reformatted print output.
 *  Added new method checking whether an image content item applies to a
 *  certain frame.
 *  Added new options: number nested items instead of indenting them, print SOP
 *  instance UID of referenced composite objects.
 *
 *  Revision 1.1  2000/10/13 07:52:21  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
