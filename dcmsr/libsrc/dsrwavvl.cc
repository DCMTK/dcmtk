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
 *    classes: DSRWaveformReferenceValue
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-26 14:38:02 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrwavvl.h"


DSRWaveformReferenceValue::DSRWaveformReferenceValue()
  : DSRCompositeReferenceValue(),
    ChannelList()
{
}


DSRWaveformReferenceValue::DSRWaveformReferenceValue(const OFString &sopClassUID,
                                                     const OFString &sopInstanceUID)
  : DSRCompositeReferenceValue(),
    ChannelList()
{
    /* check for appropriate SOP class UID */
    setReference(sopClassUID, sopInstanceUID);
}


DSRWaveformReferenceValue::DSRWaveformReferenceValue(const DSRWaveformReferenceValue &referenceValue)
  : DSRCompositeReferenceValue(referenceValue),
    ChannelList(referenceValue.ChannelList)
{
    /* do not check since this would unexpected to the user */
}


DSRWaveformReferenceValue::~DSRWaveformReferenceValue()
{
}


DSRWaveformReferenceValue &DSRWaveformReferenceValue::operator=(const DSRWaveformReferenceValue &referenceValue)
{
    DSRCompositeReferenceValue::operator=(referenceValue);
    /* do not check since this would unexpected to the user */
    ChannelList = referenceValue.ChannelList;
    return *this;
}


void DSRWaveformReferenceValue::clear()
{
    DSRCompositeReferenceValue::clear();
    ChannelList.clear();
}


OFBool DSRWaveformReferenceValue::isShort(const size_t flags) const
{
    return ChannelList.isEmpty() || !(flags & DSRTypes::HF_renderFullData);
}


E_Condition DSRWaveformReferenceValue::print(ostream &stream,
                                             const size_t flags) const
{
    const char *string = dcmFindNameOfUID(SOPClassUID.c_str());
    stream << "(";
    if (string != NULL)
        stream << string;
    else
        stream << "\"" << SOPClassUID << "\"";
    stream << ",";
    if (flags & DSRTypes::PF_printSOPInstanceUID)
        stream << "\"" << SOPInstanceUID << "\"";
    if (!ChannelList.isEmpty())
    {
        stream << ",";
        ChannelList.print(stream, flags);
    }
    stream << ")";
    return EC_Normal;
}


E_Condition DSRWaveformReferenceValue::readItem(DcmItem &dataset,
                                                OFConsole *logStream)
{
    /* read ReferencedSOPClassUID and ReferencedSOPInstanceUID */
    E_Condition result = DSRCompositeReferenceValue::readItem(dataset, logStream);
    /* read ReferencedWaveformChannels (conditional) */
    if (result == EC_Normal)
        ChannelList.read(dataset, logStream);
    return result;
}


E_Condition DSRWaveformReferenceValue::writeItem(DcmItem &dataset,
                                                 OFConsole *logStream) const
{
    /* write ReferencedSOPClassUID and ReferencedSOPInstanceUID */
    E_Condition result = DSRCompositeReferenceValue::writeItem(dataset, logStream);
    /* write ReferencedWaveformChannels (conditional) */
    if (result == EC_Normal)
    {
        if (!ChannelList.isEmpty())
            result = ChannelList.write(dataset, logStream);
    }
    return result;
}


E_Condition DSRWaveformReferenceValue::renderHTML(ostream &docStream,
                                                  ostream &annexStream,
                                                  size_t &annexNumber,
                                                  const size_t flags,
                                                  OFConsole * /* logStream */) const
{
    /* render reference */
    docStream << "<a href=\"file://dicom/waveform/" << SOPClassUID << "/" << SOPInstanceUID << "\">";
    const char *string = dcmFindNameOfUID(SOPClassUID.c_str());
    if (string != NULL)
        docStream << string;
    else
        docStream << "unknown waveform";
    docStream << "</a>" << endl;
    /* render (optional) channel list */
    if (!isShort(flags))
    {
        if (flags & DSRTypes::HF_currentlyInsideAnnex)
        {
            docStream << "<p>" << endl;
            /* render channel list (= print)*/
            docStream << "<b>Referenced Waveform Channels:</b><br>";
            ChannelList.print(docStream);
            docStream << "</p>" << endl;
        } else {
            DSRTypes::createHTMLAnnexEntry(docStream, annexStream, "for more details see", annexNumber);
            annexStream << "<p>" << endl;
            /* render channel list (= print)*/
            annexStream << "<b>Referenced Waveform Channels:</b><br>";
            ChannelList.print(annexStream);
            annexStream << "</p>" << endl;
        }
    }
    return EC_Normal;
}


E_Condition DSRWaveformReferenceValue::getValue(DSRWaveformReferenceValue &referenceValue) const
{
    referenceValue = *this;
    return EC_Normal;
}


E_Condition DSRWaveformReferenceValue::setValue(const DSRWaveformReferenceValue &referenceValue)
{
    E_Condition result = DSRCompositeReferenceValue::setValue(referenceValue);
    if (result == EC_Normal)
        ChannelList = referenceValue.ChannelList;
    return result;
}


OFBool DSRWaveformReferenceValue::appliesToChannel(const Uint16 multiplexGroupNumber,
                                                   const Uint16 channelNumber) const
{
    OFBool result = OFTrue;
    if (!ChannelList.isEmpty())
        result = ChannelList.isElement(multiplexGroupNumber, channelNumber);
    return result;
}


OFBool DSRWaveformReferenceValue::checkSOPClassUID(const OFString &sopClassUID) const
{
    OFBool result = OFFalse;
    if (DSRCompositeReferenceValue::checkSOPClassUID(sopClassUID))
    {
        /* check for all valid/known SOP classes (according to supplement 30) */
        if ((sopClassUID == UID_TwelveLeadECGWaveformStorage) ||
            (sopClassUID == UID_GeneralECGWaveformStorage) ||
            (sopClassUID == UID_AmbulatoryECGWaveformStorage) ||
            (sopClassUID == UID_HemodynamicWaveformStorage) ||
            (sopClassUID == UID_CardiacElectrophysiologyWaveformStorage) ||
            (sopClassUID == UID_BasicVoiceAudioWaveformStorage))
        {
            result = OFTrue;
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrwavvl.cc,v $
 *  Revision 1.8  2000-10-26 14:38:02  joergr
 *  Use method isShort() to decide whether a content item can be rendered
 *  "inline" or not.
 *
 *  Revision 1.7  2000/10/24 15:04:12  joergr
 *  Changed HTML hyperlinks to referenced objects from "dicom://" to "file://"
 *  to facilitate access from Java.
 *
 *  Revision 1.6  2000/10/23 15:01:05  joergr
 *  Added SOP class UID to hyperlink in method renderHTML().
 *
 *  Revision 1.5  2000/10/20 10:14:59  joergr
 *  Renamed class DSRReferenceValue to DSRCompositeReferenceValue.
 *
 *  Revision 1.4  2000/10/19 16:07:42  joergr
 *  Renamed some set methods.
 *
 *  Revision 1.3  2000/10/18 17:25:34  joergr
 *  Added check for read methods (VM and type).
 *
 *  Revision 1.2  2000/10/16 12:11:41  joergr
 *  Reformatted print output.
 *  Added new method checking whether a waveform content item applies to a
 *  certain channel.
 *  Added new options: number nested items instead of indenting them, print SOP
 *  instance UID of referenced composite objects.
 *
 *  Revision 1.1  2000/10/13 07:52:30  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
