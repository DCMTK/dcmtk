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
 *    classes: DSRWaveformChannelList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:52:28 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrwavch.h"


DSRWaveformChannelList::DSRWaveformChannelList()
  : DSRListOfItems<DSRWaveformChannelItem>()
{
}


DSRWaveformChannelList::DSRWaveformChannelList(const DSRWaveformChannelList &list)
  : DSRListOfItems<DSRWaveformChannelItem>(list)
{
}


DSRWaveformChannelList::~DSRWaveformChannelList()
{
}


DSRWaveformChannelList &DSRWaveformChannelList::operator=(const DSRWaveformChannelList &list)
{
    DSRListOfItems<DSRWaveformChannelItem>::operator=(list);
    return *this;
}


E_Condition DSRWaveformChannelList::print(ostream &stream,
                                          const size_t flags) const
{
    const OFListIterator(DSRWaveformChannelItem) endPos = OFList<DSRWaveformChannelItem>::end();
    OFListIterator(DSRWaveformChannelItem) iterator = OFList<DSRWaveformChannelItem>::begin();
    while (iterator != endPos)
    {
        stream << (*iterator).MultiplexGroupNumber << "/" << (*iterator).ChannelNumber;
        iterator++;
        if (flags & DSRTypes::PF_shortenLongItemValues)
        {
            stream << ",...";
            iterator = endPos;
        } else if (iterator != endPos)
            stream << ",";
    }
    return EC_Normal;
}


E_Condition DSRWaveformChannelList::read(DcmItem &dataset,
                                         OFConsole *logStream)
{
    /* get integer string from dataset */
    DcmUnsignedShort delem(DCM_ReferencedWaveformChannels);
    E_Condition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "2-2n", "1C", logStream);
    if (result == EC_Normal)
    {
        /* clear internal list */
        clear();
        DSRWaveformChannelItem value;
        const unsigned long count = delem.getVM();
        /* fill list with values from integer string */
        unsigned long i = 0;
        while ((i < count) && (result == EC_Normal))
        {
            result = delem.getUint16(value.MultiplexGroupNumber, i++);
            if (result == EC_Normal)
                result = delem.getUint16(value.ChannelNumber, i++);
            if (result == EC_Normal)
                DSRListOfItems<DSRWaveformChannelItem>::addItem(value);
        }
    }
    return result;
}


E_Condition DSRWaveformChannelList::write(DcmItem &dataset,
                                          OFConsole * /* logStream */) const
{
    E_Condition result = EC_Normal;
    /* fill string with values from list */
    DcmUnsignedShort delem(DCM_ReferencedWaveformChannels);
    const OFListIterator(DSRWaveformChannelItem) endPos = OFList<DSRWaveformChannelItem>::end();
    OFListIterator(DSRWaveformChannelItem) iterator = OFList<DSRWaveformChannelItem>::begin();
    unsigned long i = 0;
    while ((iterator != endPos) && (result == EC_Normal))
    {
        result = delem.putUint16((*iterator).MultiplexGroupNumber, i++);
        if (result == EC_Normal)
            result = delem.putUint16((*iterator).ChannelNumber, i++);
        iterator++;
    }
    /* add to dataset */
    if (result == EC_Normal)
        result = DSRTypes::addElementToDataset(result, dataset, new DcmUnsignedShort(delem));
    return result;
}


E_Condition DSRWaveformChannelList::getItem(const size_t idx,
                                            Uint16 &multiplexGroupNumber,
                                            Uint16 &channelNumber) const
{
    DSRWaveformChannelItem item;    /* default: 0,0 */
    E_Condition result = DSRListOfItems<DSRWaveformChannelItem>::getItem(idx, item);
    multiplexGroupNumber = item.MultiplexGroupNumber;
    channelNumber = item.ChannelNumber;
    return result;
}


void DSRWaveformChannelList::addItem(const Uint16 multiplexGroupNumber,
                                     const Uint16 channelNumber)
{
    DSRListOfItems<DSRWaveformChannelItem>::addItem(DSRWaveformChannelItem(multiplexGroupNumber, channelNumber));
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrwavch.cc,v $
 *  Revision 1.1  2000-10-13 07:52:28  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
