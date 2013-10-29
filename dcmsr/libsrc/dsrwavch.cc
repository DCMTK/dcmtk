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
 *    classes: DSRWaveformChannelList
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrwavch.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION
#define EXPLICIT_SPECIALIZATION template<>
#else
#define EXPLICIT_SPECIALIZATION
#endif

/* declared in class DSRListOfItems<T> */
EXPLICIT_SPECIALIZATION const DSRWaveformChannelItem DSRListOfItems<DSRWaveformChannelItem>::EmptyItem(0, 0);


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


OFCondition DSRWaveformChannelList::print(STD_NAMESPACE ostream &stream,
                                          const size_t flags,
                                          const char pairSeparator,
                                          const char itemSeparator) const
{
    const OFListConstIterator(DSRWaveformChannelItem) endPos = ItemList.end();
    OFListConstIterator(DSRWaveformChannelItem) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        stream << (*iterator).MultiplexGroupNumber << pairSeparator << (*iterator).ChannelNumber;
        iterator++;
        if (iterator != endPos)
        {
            if (flags & DSRTypes::PF_shortenLongItemValues)
            {
                stream << itemSeparator << "...";
                iterator = endPos;
            } else
                stream << itemSeparator;
        }
    }
    return EC_Normal;
}


OFCondition DSRWaveformChannelList::read(DcmItem &dataset)
{
    /* get integer array from dataset */
    DcmUnsignedShort delem(DCM_ReferencedWaveformChannels);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "2-2n", "1C", "WAVEFORM content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Uint16 group = 0;
        Uint16 channel = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from integer array */
        unsigned long i = 0;
        while ((i < count) && result.good())
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


OFCondition DSRWaveformChannelList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* fill integer array with values from list */
    DcmUnsignedShort delem(DCM_ReferencedWaveformChannels);
    const OFListConstIterator(DSRWaveformChannelItem) endPos = ItemList.end();
    OFListConstIterator(DSRWaveformChannelItem) iterator = ItemList.begin();
    unsigned long i = 0;
    while ((iterator != endPos) && result.good())
    {
        result = delem.putUint16((*iterator).MultiplexGroupNumber, i++);
        if (result.good())
            result = delem.putUint16((*iterator).ChannelNumber, i++);
        iterator++;
    }
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmUnsignedShort(delem), "2-2n", "1", "WAVEFORM content item");
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


OFCondition DSRWaveformChannelList::putString(const char *stringValue)
{
    OFCondition result = EC_Normal;
    /* clear internal list */
    clear();
    /* check input string */
    if ((stringValue != NULL) && (strlen(stringValue) > 0))
    {
        Uint16 group = 0;
        Uint16 channel = 0;
        const char *ptr = stringValue;
        /* retrieve channel pairs from string */
        while (result.good() && (ptr != NULL))
        {
            if (sscanf(ptr, "%hu/%hu", &group, &channel) == 2)
            {
                addItem(group, channel);
                /* jump to next channel pair */
                ptr = strchr(ptr, ',');
                if (ptr != NULL)
                    ptr++;
            } else
                result = EC_CorruptedData;
        }
    }
    return result;
}
