/*
 *
 *  Copyright (C) 2000-2012, OFFIS e.V.
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
 *    classes: DSRImageReferenceValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2012-05-29 14:02:18 $
 *  CVS/RCS Revision: $Revision: 1.27 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrimgvl.h"
#include "dcmtk/dcmsr/dsrxmld.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmimgle/diutils.h"
#include "dcmtk/dcmimage/diregist.h"  /* add support for color images */


DSRImageReferenceValue::DSRImageReferenceValue()
  : DSRCompositeReferenceValue(),
    PresentationState(),
    FrameList(),
    IconImage(NULL)
{
}


DSRImageReferenceValue::DSRImageReferenceValue(const OFString &sopClassUID,
                                               const OFString &sopInstanceUID)
  : DSRCompositeReferenceValue(),
    PresentationState(),
    FrameList(),
    IconImage(NULL)
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
    FrameList(),
    IconImage(NULL)
{
    /* check for appropriate SOP class UID */
    setReference(imageSOPClassUID, imageSOPInstanceUID);
    setPresentationState(DSRCompositeReferenceValue(pstateSOPClassUID, pstateSOPInstanceUID));
}


DSRImageReferenceValue::DSRImageReferenceValue(const DSRImageReferenceValue &referenceValue)
  : DSRCompositeReferenceValue(referenceValue),
    PresentationState(referenceValue.PresentationState),
    FrameList(referenceValue.FrameList),
    IconImage(NULL)
{
    /* do not check values since this would be unexpected to the user */

    /* create copy of icon image (if any), first frame only */
    if (referenceValue.IconImage != NULL)
        IconImage = referenceValue.IconImage->createDicomImage(0 /*fstart*/, 1 /*fcount*/);
}


DSRImageReferenceValue::DSRImageReferenceValue(const DSRCompositeReferenceValue &imageReferenceValue,
                                               const DSRCompositeReferenceValue &pstateReferenceValue)
  : DSRCompositeReferenceValue(),
    PresentationState(),
    FrameList(),
    IconImage(NULL)
{
    /* check for appropriate SOP class UID */
    DSRCompositeReferenceValue::setValue(imageReferenceValue);
    setPresentationState(pstateReferenceValue);
}


DSRImageReferenceValue::~DSRImageReferenceValue()
{
    deleteIconImage();
}


DSRImageReferenceValue &DSRImageReferenceValue::operator=(const DSRImageReferenceValue &referenceValue)
{
    DSRCompositeReferenceValue::operator=(referenceValue);
    /* do not check since this would be unexpected to the user */
    PresentationState = referenceValue.PresentationState;
    FrameList = referenceValue.FrameList;
    /* create copy of icon image (if any), first frame only */
    IconImage = (referenceValue.IconImage != NULL) ? referenceValue.IconImage->createDicomImage(0 /*fstart*/, 1 /*fcount*/) : NULL;
    return *this;
}


void DSRImageReferenceValue::clear()
{
    DSRCompositeReferenceValue::clear();
    PresentationState.clear();
    FrameList.clear();
    deleteIconImage();
}


OFBool DSRImageReferenceValue::isValid() const
{
    return DSRCompositeReferenceValue::isValid() && checkPresentationState(PresentationState);
}


OFBool DSRImageReferenceValue::isShort(const size_t flags) const
{
    return FrameList.isEmpty() || !(flags & DSRTypes::HF_renderFullData);
}


OFCondition DSRImageReferenceValue::print(STD_NAMESPACE ostream &stream,
                                          const size_t flags) const
{
    const char *modality = dcmSOPClassUIDToModality(SOPClassUID.c_str());
    stream << "(";
    if (modality != NULL)
        stream << modality << " image";
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
        const DSRTypes::E_PresentationStateType pstateType = DSRTypes::sopClassUIDToPresentationStateType(PresentationState.getSOPClassUID());
        if (pstateType != DSRTypes::PT_invalid)
            stream << ",(" << DSRTypes::presentationStateTypeToShortName(pstateType) << ",";
        else
            stream << ",(" << PresentationState.getSOPClassUID() << ",";
        if (flags & DSRTypes::PF_printSOPInstanceUID)
            stream << "\"" << PresentationState.getSOPInstanceUID() << "\"";
        stream << ")";
    }
    return EC_Normal;
}


OFCondition DSRImageReferenceValue::readXML(const DSRXMLDocument &doc,
                                            DSRXMLCursor cursor)
{
    /* first read general composite reference information */
    OFCondition result = DSRCompositeReferenceValue::readXML(doc, cursor);
    /* then read image related XML tags */
    if (result.good())
    {
        /* frame list (optional) */
        const DSRXMLCursor childCursor = doc.getNamedNode(cursor.getChild(), "frames", OFFalse /*required*/);
        if (childCursor.valid())
        {
            OFString tmpString;
            /* put element content to the channel list */
            result = FrameList.putString(doc.getStringFromNodeContent(childCursor, tmpString).c_str());
        }
        if (result.good())
        {
            /* presentation state (optional) */
            cursor = doc.getNamedNode(cursor.getChild(), "pstate", OFFalse /*required*/);
            if (cursor.getChild().valid())
                result = PresentationState.readXML(doc, cursor);
        }
    }
    return result;
}


OFCondition DSRImageReferenceValue::writeXML(STD_NAMESPACE ostream &stream,
                                             const size_t flags) const
{
    OFCondition result = DSRCompositeReferenceValue::writeXML(stream, flags);
    if ((flags & DSRTypes::XF_writeEmptyTags) || !FrameList.isEmpty())
    {
        stream << "<frames>";
        FrameList.print(stream);
        stream << "</frames>" << OFendl;
    }
    if ((flags & DSRTypes::XF_writeEmptyTags) || PresentationState.isValid())
    {
        stream << "<pstate>" << OFendl;
        if (PresentationState.isValid())
            PresentationState.writeXML(stream, flags);
        stream << "</pstate>" << OFendl;
    }
    return result;
}


OFCondition DSRImageReferenceValue::readItem(DcmItem &dataset)
{
    /* be very careful, delete any previously created icon image (should never apply) */
    deleteIconImage();
    /* read ReferencedSOPClassUID and ReferencedSOPInstanceUID */
    OFCondition result = DSRCompositeReferenceValue::readItem(dataset);
    /* read ReferencedFrameNumber (conditional) */
    if (result.good())
        FrameList.read(dataset);
    /* read ReferencedSOPSequence (Presentation State, optional) */
    if (result.good())
        PresentationState.readSequence(dataset, "3" /*type*/);
    /* read IconImageSequence (optional) */
    if (result.good())
    {
        DcmSequenceOfItems dseq(DCM_IconImageSequence);
        /* use local status variable since the sequence is optional */
        const OFCondition seqStatus = DSRTypes::getElementFromDataset(dataset, dseq);
        DSRTypes::checkElementValue(dseq, "1", "3", seqStatus, "IMAGE content item");
        if (seqStatus.good())
        {
            /* check for empty sequence (allowed!) */
            if (!dseq.isEmpty())
            {
                /* read first item */
                DcmItem *ditem = dseq.getItem(0);
                if ((ditem != NULL) && !ditem->isEmpty())
                {
                    /* try to load/process the icon image */
                    IconImage = new DicomImage(ditem, EXS_LittleEndianExplicit);
                    if (IconImage != NULL)
                    {
                        if (IconImage->getStatus() != EIS_Normal)
                            result = SR_EC_CannotCreateIconImage;
                    } else
                        result = EC_MemoryExhausted;
                } else
                    result = SR_EC_InvalidDocumentTree;
            }
        }
    }
    return result;
}


OFCondition DSRImageReferenceValue::writeItem(DcmItem &dataset) const
{
    /* write ReferencedSOPClassUID and ReferencedSOPInstanceUID */
    OFCondition result = DSRCompositeReferenceValue::writeItem(dataset);
    /* write ReferencedFrameNumber (conditional) */
    if (result.good())
    {
        if (!FrameList.isEmpty())
            result = FrameList.write(dataset);
    }
    /* write ReferencedSOPSequence (Presentation State, optional) */
    if (result.good())
    {
        if (PresentationState.isValid())
            result = PresentationState.writeSequence(dataset);
    }
    /* write IconImageSequence (optional) */
    if (result.good() && (IconImage != NULL))
    {
        DcmItem *ditem = NULL;
        /* create sequence with a single item */
        result = dataset.findOrCreateSequenceItem(DCM_IconImageSequence, ditem, 0 /*position*/);
        if (result.good())
        {
            /* write icon image */
            if (IconImage->writeFrameToDataset(*ditem))
            {
                /* delete unwanted element NumberOfFrames (0028,0008) */
                ditem->findAndDeleteElement(DCM_NumberOfFrames);
            } else
                result = EC_CorruptedData;
        }
    }
    return result;
}


OFCondition DSRImageReferenceValue::renderHTML(STD_NAMESPACE ostream &docStream,
                                               STD_NAMESPACE ostream &annexStream,
                                               size_t &annexNumber,
                                               const size_t flags) const
{
    /* reference: image */
    docStream << "<a href=\"" << HTML_HYPERLINK_PREFIX_FOR_CGI;
    docStream << "?image=" << SOPClassUID << "+" << SOPInstanceUID;
    /* reference: pstate */
    if (PresentationState.isValid())
    {
        docStream << "&amp;pstate=" << PresentationState.getSOPClassUID();
        docStream << "+" << PresentationState.getSOPInstanceUID();
    }
    /* reference: frames */
    if (!FrameList.isEmpty())
    {
        docStream << "&amp;frames=";
        FrameList.print(docStream, 0 /*flags*/, '+');
    }
    docStream << "\">";
    /* text: image */
    const char *modality = dcmSOPClassUIDToModality(SOPClassUID.c_str());
    if (modality != NULL)
        docStream << modality;
    else
        docStream << "unknown";
    docStream << " image";
    /* text: pstate */
    if (PresentationState.isValid())
        docStream << " with presentation state";
    docStream << "</a>";
    if (!isShort(flags))
    {
        const char *lineBreak = (flags & DSRTypes::HF_renderSectionTitlesInline) ? " " :
                                (flags & DSRTypes::HF_XHTML11Compatibility) ? "<br />" : "<br>";
        if (flags & DSRTypes::HF_currentlyInsideAnnex)
        {
            docStream << OFendl << "<p>" << OFendl;
            /* render frame list (= print)*/
            docStream << "<b>Referenced Frame Number:</b>" << lineBreak;
            FrameList.print(docStream);
            docStream << "</p>";
        } else {
            docStream << " ";
            DSRTypes::createHTMLAnnexEntry(docStream, annexStream, "for more details see", annexNumber, flags);
            annexStream << "<p>" << OFendl;
            /* render frame list (= print)*/
            annexStream << "<b>Referenced Frame Number:</b>" << lineBreak;
            FrameList.print(annexStream);
            annexStream << "</p>" << OFendl;
        }
    }
    return EC_Normal;
}


OFCondition DSRImageReferenceValue::createIconImage(const OFString &filename,
                                                    const unsigned long frame,
                                                    const unsigned long width,
                                                    const unsigned long height)
{
    /* delete old icon image (if any) */
    deleteIconImage();
    OFCondition result = EC_IllegalParameter;
    if (!filename.empty())
    {
        /* try to load specified DICOM image */
        const unsigned long flags = CIF_UsePartialAccessToPixelData | CIF_NeverAccessEmbeddedOverlays;
        DicomImage *image = new DicomImage(filename.c_str(), flags, frame, 1 /*fcount*/);
        if (image != NULL)
        {
            /* set VOI window (for monochrome images) */
            if (image->isMonochrome() && !image->setWindow(0))
                image->setMinMaxWindow();
            /* do the real work: create a down-scaled version of the DICOM image */
            result = createIconImage(image, width, height);
            delete image;
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition DSRImageReferenceValue::createIconImage(DcmObject *object,
                                                    const E_TransferSyntax xfer,
                                                    const unsigned long frame,
                                                    const unsigned long width,
                                                    const unsigned long height)
{
    /* delete old icon image (if any) */
    deleteIconImage();
    OFCondition result = EC_IllegalParameter;
    if (object != NULL)
    {
        /* try to load specified DICOM image */
        const unsigned long flags = CIF_UsePartialAccessToPixelData | CIF_NeverAccessEmbeddedOverlays;
        DicomImage *image = new DicomImage(object, xfer, flags, frame, 1 /*fcount*/);
        if (image != NULL)
        {
            /* set VOI window (for monochrome images) */
            if (image->isMonochrome() && !image->setWindow(0))
                image->setMinMaxWindow();
            /* do the real work: create a down-scaled version of the DICOM image */
            result = createIconImage(image, width, height);
            delete image;
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition DSRImageReferenceValue::createIconImage(const DicomImage *image,
                                                    const unsigned long width,
                                                    const unsigned long height)
{
    /* delete old icon image (if any) */
    deleteIconImage();
    OFCondition result = EC_IllegalParameter;
    if (image != NULL)
    {
        const EI_Status imageStatus = image->getStatus();
        /* check whether image loading/processing was successful */
        switch (imageStatus)
        {
            case EIS_Normal:
            {
                if (image->getFrameCount() > 1)
                    DCMSR_DEBUG("DICOM image passed for creating an icon image contains multiple frames");
                /* create a down-scaled version of the DICOM image */
                const int aspect = (width == 0) || (height == 0);
                IconImage = image->createScaledImage(width, height, 1 /*interpolate*/, aspect);
                result = (IconImage != NULL) ? EC_Normal : SR_EC_CannotCreateIconImage;
                break;
            }
            case EIS_InvalidDocument:
            case EIS_InvalidImage:
                result = SR_EC_InvalidDocument;
                break;
            case EIS_MissingAttribute:
                result = SR_EC_MandatoryAttributeMissing;
                break;
            case EIS_InvalidValue:
                result = SR_EC_InvalidValue;
                break;
            case EIS_NotSupportedValue:
                result = SR_EC_UnsupportedValue;
                break;
            case EIS_MemoryFailure:
                result = EC_MemoryExhausted;
                break;
            default:
                /* this is the fallback for all other kind of errors */
                result = SR_EC_CannotCreateIconImage;
                break;
        }
    }
    return result;
}


void DSRImageReferenceValue::deleteIconImage()
{
    delete IconImage;
    IconImage = NULL;
}


OFCondition DSRImageReferenceValue::getValue(DSRImageReferenceValue &referenceValue) const
{
    referenceValue = *this;
    return EC_Normal;
}


OFCondition DSRImageReferenceValue::setValue(const DSRImageReferenceValue &referenceValue)
{
    OFCondition result = DSRCompositeReferenceValue::setValue(referenceValue);
    if (result.good())
    {
        FrameList = referenceValue.FrameList;
        setPresentationState(referenceValue.PresentationState);
    }
    return result;
}


OFCondition DSRImageReferenceValue::setPresentationState(const DSRCompositeReferenceValue &referenceValue)
{
    OFCondition result = EC_IllegalParameter;
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
          (DSRTypes::sopClassUIDToPresentationStateType(referenceValue.getSOPClassUID()) != DSRTypes::PT_invalid));
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrimgvl.cc,v $
 *  Revision 1.27  2012-05-29 14:02:18  joergr
 *  Slightly modified code for using methods from class DcmSequenceOfItems.
 *
 *  Revision 1.26  2012-01-26 09:10:55  joergr
 *  Fixed wrong return status when reading an IMAGE content item without icon.
 *
 *  Revision 1.25  2012-01-03 10:58:08  joergr
 *  Added support for icon image to IMAGE content item (introduced with CP-217).
 *
 *  Revision 1.24  2011-12-15 16:30:18  joergr
 *  Fixed typo in comments.
 *
 *  Revision 1.23  2011-08-02 08:32:36  joergr
 *  Added more general support for softcopy presentation states (not only GSPS).
 *
 *  Revision 1.22  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.21  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.20  2007-11-15 16:45:26  joergr
 *  Added support for output in XHTML 1.1 format.
 *  Enhanced support for output in valid HTML 3.2 format. Migrated support for
 *  standard HTML from version 4.0 to 4.01 (strict).
 *
 *  Revision 1.19  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.18  2006/07/25 13:37:48  joergr
 *  Added new optional flags for the HTML rendering of SR documents:
 *  HF_alwaysExpandChildrenInline, HF_useCodeDetailsTooltip and
 *  HF_renderSectionTitlesInline.
 *
 *  Revision 1.17  2005/12/08 15:47:55  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.16  2004/01/16 10:14:14  joergr
 *  Made readXML() more robust with regard to expected XML structure.
 *  Only write <pstate/> XML element value when presentation state is valid.
 *
 *  Revision 1.15  2003/08/07 13:38:32  joergr
 *  Added readXML functionality.
 *  Renamed parameters/variables "string" to avoid name clash with STL class.
 *
 *  Revision 1.14  2001/10/10 15:29:56  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.13  2001/09/26 13:04:22  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.12  2001/05/07 16:14:24  joergr
 *  Updated CVS header.
 *
 *  Revision 1.11  2001/02/13 16:35:28  joergr
 *  Minor corrections in XML output (newlines, etc.).
 *
 *  Revision 1.10  2000/11/06 11:32:51  joergr
 *  Changes structure of HTML hyperlinks to composite objects (now using pseudo
 *  CGI script).
 *
 *  Revision 1.9  2000/11/01 16:37:00  joergr
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
