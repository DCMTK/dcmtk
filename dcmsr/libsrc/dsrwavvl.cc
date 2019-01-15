/*
 *
 *  Copyright (C) 2000-2019, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRWaveformReferenceValue
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrwavvl.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcuid.h"


DSRWaveformReferenceValue::DSRWaveformReferenceValue()
  : DSRCompositeReferenceValue(),
    ChannelList()
{
}


DSRWaveformReferenceValue::DSRWaveformReferenceValue(const OFString &sopClassUID,
                                                     const OFString &sopInstanceUID,
                                                     const OFBool check)
  : DSRCompositeReferenceValue(),
    ChannelList()
{
    /* use the set method for checking purposes */
    setReference(sopClassUID, sopInstanceUID, check);
}


DSRWaveformReferenceValue::DSRWaveformReferenceValue(const DSRWaveformReferenceValue &referenceValue)
  : DSRCompositeReferenceValue(referenceValue),
    ChannelList(referenceValue.ChannelList)
{
    /* do not check since this would be unexpected to the user */
}


DSRWaveformReferenceValue::~DSRWaveformReferenceValue()
{
}


DSRWaveformReferenceValue &DSRWaveformReferenceValue::operator=(const DSRWaveformReferenceValue &referenceValue)
{
    DSRCompositeReferenceValue::operator=(referenceValue);
    /* do not check since this would be unexpected to the user */
    ChannelList = referenceValue.ChannelList;
    return *this;
}


OFBool DSRWaveformReferenceValue::operator==(const DSRWaveformReferenceValue &referenceValue) const
{
    return DSRCompositeReferenceValue::operator==(referenceValue) &&
           (ChannelList == referenceValue.ChannelList);
}


OFBool DSRWaveformReferenceValue::operator!=(const DSRWaveformReferenceValue &referenceValue) const
{
    return DSRCompositeReferenceValue::operator!=(referenceValue) ||
           (ChannelList != referenceValue.ChannelList);
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


OFCondition DSRWaveformReferenceValue::print(STD_NAMESPACE ostream &stream,
                                             const size_t flags) const
{
    /* first, determine SOP class component */
    OFString sopClassString = "\"" + SOPClassUID + "\"";
    if (!(flags & DSRTypes::PF_printSOPClassUID))
    {
        /* look up name of known SOP classes */
        const char *className = dcmFindNameOfUID(SOPClassUID.c_str());
        if (className != NULL)
            sopClassString = className;
    }
    /* and then, print it */
    stream << "(" << sopClassString << ",";
    /* print SOP instance component (if desired) */
    if (flags & DSRTypes::PF_printSOPInstanceUID)
        stream << "\"" << SOPInstanceUID << "\"";
    /* print channel list (if present) */
    if (!ChannelList.isEmpty())
    {
        stream << ",";
        ChannelList.print(stream, flags);
    }
    stream << ")";
    return EC_Normal;
}


OFCondition DSRWaveformReferenceValue::readXML(const DSRXMLDocument &doc,
                                               DSRXMLCursor cursor,
                                               const size_t flags)
{
    /* first read general composite reference information */
    OFCondition result = DSRCompositeReferenceValue::readXML(doc, cursor, flags);
    /* then read waveform related XML tags */
    if (result.good())
    {
        /* channel list (optional) */
        cursor = doc.getNamedChildNode(cursor, "channels");
        if (cursor.valid())
        {
            OFString tmpString;
            /* put element content to the channel list */
            result = ChannelList.putString(doc.getStringFromNodeContent(cursor, tmpString).c_str());
        }
    }
    return result;
}


OFCondition DSRWaveformReferenceValue::writeXML(STD_NAMESPACE ostream &stream,
                                                const size_t flags) const
{
    OFCondition result = DSRCompositeReferenceValue::writeXML(stream, flags);
    if ((flags & DSRTypes::XF_writeEmptyTags) || !ChannelList.isEmpty())
    {
        stream << "<channels>";
        ChannelList.print(stream);
        stream << "</channels>" << OFendl;
    }
    return result;
}


OFCondition DSRWaveformReferenceValue::readItem(DcmItem &dataset,
                                                const size_t flags)
{
    /* read ReferencedSOPClassUID and ReferencedSOPInstanceUID */
    OFCondition result = DSRCompositeReferenceValue::readItem(dataset, flags);
    /* read ReferencedWaveformChannels (conditional) */
    if (result.good())
        ChannelList.read(dataset, flags);
    return result;
}


OFCondition DSRWaveformReferenceValue::writeItem(DcmItem &dataset) const
{
    /* write ReferencedSOPClassUID and ReferencedSOPInstanceUID */
    OFCondition result = DSRCompositeReferenceValue::writeItem(dataset);
    /* write ReferencedWaveformChannels (conditional) */
    if (result.good())
    {
        if (!ChannelList.isEmpty())
            result = ChannelList.write(dataset);
    }
    return result;
}


OFCondition DSRWaveformReferenceValue::renderHTML(STD_NAMESPACE ostream &docStream,
                                                  STD_NAMESPACE ostream &annexStream,
                                                  size_t &annexNumber,
                                                  const size_t flags) const
{
    /* render reference */
    docStream << "<a href=\"" << HTML_HYPERLINK_PREFIX_FOR_CGI;
    docStream << "?waveform=" << SOPClassUID << "+" << SOPInstanceUID;
    if (!ChannelList.isEmpty())
    {
        docStream << "&amp;channels=";
        ChannelList.print(docStream, 0 /*flags*/, '+', '+');
    }
    docStream << "\">";
    /* retrieve name of SOP class (if known) */
    docStream << dcmFindNameOfUID(SOPClassUID.c_str(), "unknown waveform");
    docStream << "</a>";
    /* render (optional) channel list */
    if (!isShort(flags))
    {
        const char *lineBreak = (flags & DSRTypes::HF_renderSectionTitlesInline) ? " " :
                                (flags & DSRTypes::HF_XHTML11Compatibility) ? "<br />" : "<br>";
        if (flags & DSRTypes::HF_currentlyInsideAnnex)
        {
            docStream  << OFendl << "<p>" << OFendl;
            /* render channel list (= print)*/
            docStream << "<b>Referenced Waveform Channels:</b>" << lineBreak;
            ChannelList.print(docStream);
            docStream << "</p>";
        } else {
            docStream << " ";
            DSRTypes::createHTMLAnnexEntry(docStream, annexStream, "for more details see", annexNumber, flags);
            annexStream << "<p>" << OFendl;
            /* render channel list (= print)*/
            annexStream << "<b>Referenced Waveform Channels:</b>" << lineBreak;
            ChannelList.print(annexStream);
            annexStream << "</p>" << OFendl;
        }
    }
    return EC_Normal;
}


OFCondition DSRWaveformReferenceValue::getValue(DSRWaveformReferenceValue &referenceValue) const
{
    referenceValue = *this;
    return EC_Normal;
}


OFCondition DSRWaveformReferenceValue::setValue(const DSRWaveformReferenceValue &referenceValue,
                                                const OFBool check)
{
    OFCondition result = DSRCompositeReferenceValue::setValue(referenceValue, check);
    if (result.good())
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


OFCondition DSRWaveformReferenceValue::checkSOPClassUID(const OFString &sopClassUID) const
{
    OFCondition result = DSRCompositeReferenceValue::checkSOPClassUID(sopClassUID);
    if (result.good())
    {
        /* check for all valid/known SOP classes (according to DICOM PS 3.6-2017e) */
        if ((sopClassUID != UID_TwelveLeadECGWaveformStorage) &&
            (sopClassUID != UID_GeneralECGWaveformStorage) &&
            (sopClassUID != UID_AmbulatoryECGWaveformStorage) &&
            (sopClassUID != UID_HemodynamicWaveformStorage) &&
            (sopClassUID != UID_CardiacElectrophysiologyWaveformStorage) &&
            (sopClassUID != UID_BasicVoiceAudioWaveformStorage) &&
            (sopClassUID != UID_GeneralAudioWaveformStorage) &&
            (sopClassUID != UID_ArterialPulseWaveformStorage) &&
            (sopClassUID != UID_RespiratoryWaveformStorage))
        {
            result = SR_EC_InvalidValue;
        }
    }
    return result;
}
