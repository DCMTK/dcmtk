/*
 *
 *  Copyright (C) 2000-2001, OFFIS
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:51:11 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrwavch.h"


/* declared in class DSRListOfItems<T> */
const DSRWaveformChannelItem DSRListOfItems<DSRWaveformChannelItem>::EmptyItem;


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
                                          const size_t flags,
                                          const char pairSeparator,
                                          const char itemSeparator) const
{
    const OFListIterator(DSRWaveformChannelItem) endPos = OFList<DSRWaveformChannelItem>::end();
    OFListIterator(DSRWaveformChannelItem) iterator = OFList<DSRWaveformChannelItem>::begin();
    while (iterator != endPos)
    {
        stream << (*iterator).MultiplexGroupNumber << pairSeparator << (*iterator).ChannelNumber;
        iterator++;
        if (flags & DSRTypes::PF_shortenLongItemValues)
        {
            stream << itemSeparator << "...";
            iterator = endPos;
        } else if (iterator != endPos)
            stream << itemSeparator;
    }
    return EC_Normal;
}


E_Condition DSRWaveformChannelList::read(DcmItem &dataset,
                                         OFConsole *logStream)
{
    /* get integer string from dataset */
    DcmUnsignedShort delem(DCM_ReferencedWaveformChannels);
    E_Condition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "2-2n", "1C", logStream, "WAVEFORM content item");
    if (result == EC_Normal)
    {
        /* clear internal list */
        clear();
        Uint16 group = 0;
        Uint16 channel = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from integer string */
        unsigned long i = 0;
        while ((i < count) && (result == EC_Normal))
        {
            result = delem.getUint16(group, i++);
            if (result == EC_Normal)
            {
                result = delem.getUint16(channel, i++);
                if (result == EC_Normal)
                    addItem(group, channel);
            }
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


OFBool DSRWaveformChannelList::isElement(const Uint16 multiplexGroupNumber,
                                         const Uint16 channelNumber) const
{
    return DSRListOfItems<DSRWaveformChannelItem>::isElement(DSRWaveformChannelItem(multiplexGroupNumber, channelNumber));
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
 *  Revision 1.7  2001-06-01 15:51:11  meichel
 *  Updated copyright header
 *
 *  Revision 1.6  2000/11/06 11:34:25  joergr
 *  Added parameter to print() method specifying the item separator character.
 *
 *  Revision 1.5  2000/10/26 14:37:23  joergr
 *  Reworked read() method.
 *
 *  Revision 1.4  2000/10/19 16:07:14  joergr
 *  Added optional module name to read method to provide more detailed warning
 *  messages.
 *
 *  Revision 1.3  2000/10/18 17:24:50  joergr
 *  Added comments.
 *
 *  Revision 1.2  2000/10/16 12:10:53  joergr
 *  Added new method checking whether a waveform content item applies to a
 *  certain channel.
 *
 *  Revision 1.1  2000/10/13 07:52:28  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
