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
 *  Update Date:      $Date: 2003-06-03 10:16:46 $
 *  CVS/RCS Revision: $Revision: 1.10 $
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


DSRWaveformChannelList::DSRWaveformChannelList(const DSRWaveformChannelList &lst)
  : DSRListOfItems<DSRWaveformChannelItem>(lst)
{
}


DSRWaveformChannelList::~DSRWaveformChannelList()
{
}


DSRWaveformChannelList &DSRWaveformChannelList::operator=(const DSRWaveformChannelList &lst)
{
    DSRListOfItems<DSRWaveformChannelItem>::operator=(lst);
    return *this;
}


OFCondition DSRWaveformChannelList::print(ostream &stream,
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


OFCondition DSRWaveformChannelList::read(DcmItem &dataset,
                                         OFConsole *logStream)
{
    /* get integer string from dataset */
    DcmUnsignedShort delem(DCM_ReferencedWaveformChannels);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "2-2n", "1C", logStream, "WAVEFORM content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Uint16 group = 0;
        Uint16 channel = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from integer string */
        unsigned long i = 0;
        while ((i < count) && (result.good()))
        {
            result = delem.getUint16(group, i++);
            if (result.good())
            {
                result = delem.getUint16(channel, i++);
                if (result.good())
                    addItem(group, channel);
            }
        }
    }
    return result;
}


OFCondition DSRWaveformChannelList::write(DcmItem &dataset,
                                          OFConsole * /* logStream */) const
{
    OFCondition result = EC_Normal;
    /* fill string with values from list */
    DcmUnsignedShort delem(DCM_ReferencedWaveformChannels);
    const OFListIterator(DSRWaveformChannelItem) endPos = OFList<DSRWaveformChannelItem>::end();
    OFListIterator(DSRWaveformChannelItem) iterator = OFList<DSRWaveformChannelItem>::begin();
    unsigned long i = 0;
    while ((iterator != endPos) && (result.good()))
    {
        result = delem.putUint16((*iterator).MultiplexGroupNumber, i++);
        if (result.good())
            result = delem.putUint16((*iterator).ChannelNumber, i++);
        iterator++;
    }
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmUnsignedShort(delem));
    return result;
}


OFBool DSRWaveformChannelList::isElement(const Uint16 multiplexGroupNumber,
                                         const Uint16 channelNumber) const
{
    return DSRListOfItems<DSRWaveformChannelItem>::isElement(DSRWaveformChannelItem(multiplexGroupNumber, channelNumber));
}


OFCondition DSRWaveformChannelList::getItem(const size_t idx,
                                            Uint16 &multiplexGroupNumber,
                                            Uint16 &channelNumber) const
{
    DSRWaveformChannelItem item;    /* default: 0,0 */
    OFCondition result = DSRListOfItems<DSRWaveformChannelItem>::getItem(idx, item);
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
 *  Revision 1.10  2003-06-03 10:16:46  meichel
 *  Renamed local variables to avoid name clashes with STL
 *
 *  Revision 1.9  2001/10/10 15:30:07  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.8  2001/09/26 13:04:30  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.7  2001/06/01 15:51:11  meichel
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
