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
 *  Update Date:      $Date: 2000-10-16 12:05:32 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrimgvl.h"


DSRImageReferenceValue::DSRImageReferenceValue()
  : DSRReferenceValue(),
    PresentationState(),
    FrameList()
{
}


DSRImageReferenceValue::DSRImageReferenceValue(const OFString &sopClassUID,
                                               const OFString &sopInstanceUID)
  : DSRReferenceValue(),
    PresentationState(),
    FrameList()
{
    /* check for appropriate SOP class UID */
    DSRReferenceValue::setValue(sopClassUID, sopInstanceUID);
}


DSRImageReferenceValue::DSRImageReferenceValue(const OFString &imageSOPClassUID,
                                               const OFString &imageSOPInstanceUID,
                                               const OFString &pstateSOPClassUID,
                                               const OFString &pstateSOPInstanceUID)
  : DSRReferenceValue(),
    PresentationState(),
    FrameList()
{
    /* check for appropriate SOP class UID */
    DSRReferenceValue::setValue(imageSOPClassUID, imageSOPInstanceUID);
    setPresentationState(DSRReferenceValue(pstateSOPClassUID, pstateSOPInstanceUID));
}


DSRImageReferenceValue::DSRImageReferenceValue(const DSRImageReferenceValue &referenceValue)
  : DSRReferenceValue(referenceValue),
    PresentationState(referenceValue.PresentationState),
    FrameList(referenceValue.FrameList)
{
    /* do not check since this would unexpected to the user */
}


DSRImageReferenceValue::DSRImageReferenceValue(const DSRReferenceValue &imageReferenceValue,
                                               const DSRReferenceValue &pstateReferenceValue)
  : DSRReferenceValue(),
    PresentationState(),
    FrameList()
{
    /* check for appropriate SOP class UID */
    DSRReferenceValue::setValue(imageReferenceValue);
    setPresentationState(pstateReferenceValue);
}


DSRImageReferenceValue::~DSRImageReferenceValue()
{
}


DSRImageReferenceValue &DSRImageReferenceValue::operator=(const DSRImageReferenceValue &referenceValue)
{
    DSRReferenceValue::operator=(referenceValue);
    /* do not check since this would unexpected to the user */
    PresentationState = referenceValue.PresentationState;
    FrameList = referenceValue.FrameList;
    return *this;
}


void DSRImageReferenceValue::clear()
{
    DSRReferenceValue::clear();
    PresentationState.clear();
    FrameList.clear();
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


E_Condition DSRImageReferenceValue::readItem(DcmItem &dataset,
                                             OFConsole *logStream)
{
    /* read ReferencedSOPClassUID and ReferencedSOPInstanceUID */
    E_Condition result = DSRReferenceValue::readItem(dataset, logStream);
    /* read ReferencedFrameNumber (conditional) */
    if (result == EC_Normal)
        FrameList.read(dataset, logStream);
    /* read ReferencedSOPSequence (Presentation State, optional) */
    if (result == EC_Normal)
        PresentationState.readSequence(dataset, logStream);
    return result;
}


E_Condition DSRImageReferenceValue::writeItem(DcmItem &dataset,
                                              OFConsole *logStream) const
{
    /* write ReferencedSOPClassUID and ReferencedSOPInstanceUID */
    E_Condition result = DSRReferenceValue::writeItem(dataset, logStream);
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
    docStream << "<a href=\"dicom://localhost/image/" << SOPInstanceUID << "\">";
    const char *string = dcmSOPClassUIDToModality(SOPClassUID.c_str());
    if (string != NULL)
        docStream << string;
    else
        docStream << "unknown";
    docStream << " image</a>" << endl;
    /* presentation state */
    if (PresentationState.isValid())
    {
        docStream << " with ";
        docStream << "<a href=\"dicom://localhost/pstate/" << SOPInstanceUID << "\">";
        docStream << " GSPS</a>" << endl;
    }
    if (!FrameList.isEmpty() && (flags & DSRTypes::HF_renderFullData))
    {
        if (flags & DSRTypes::HF_currentlyInsideAnnex)
        {
            docStream << "<p>" << endl;
            /* render frame list (= print)*/
            docStream << "<b>Referenced Frame Number:</b><br>";
            FrameList.print(docStream);
            docStream << "</p>" << endl;
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
    E_Condition result = DSRReferenceValue::setValue(referenceValue.SOPClassUID, referenceValue.SOPInstanceUID);
    if (result == EC_Normal)
    {
        FrameList = referenceValue.FrameList;
        setPresentationState(referenceValue.PresentationState);
    }
    return result;
}


E_Condition DSRImageReferenceValue::setPresentationState(const DSRReferenceValue &referenceValue)
{
    E_Condition result = EC_IllegalCall;
    if (referenceValue.isValid())
    {
        if (referenceValue.getSOPClassUID() == UID_GrayscaleSoftcopyPresentationStateStorage)
        {
            PresentationState = referenceValue;
            result = EC_Normal;
        }
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
    if (DSRReferenceValue::checkSOPClassUID(sopClassUID))
    {
        /* tbd: might check for IMAGE storage class later on */
        result = OFTrue;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrimgvl.cc,v $
 *  Revision 1.2  2000-10-16 12:05:32  joergr
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
