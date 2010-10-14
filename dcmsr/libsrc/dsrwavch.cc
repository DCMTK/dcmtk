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
 *    classes: DSRWaveformChannelList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:42 $
 *  CVS/RCS Revision: $Revision: 1.26 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
EXPLICIT_SPECIALIZATION const DSRWaveformChannelItem DSRListOfItems<DSRWaveformChannelItem>::EmptyItem(0,0);


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
    /* get integer string from dataset */
    DcmUnsignedShort delem(DCM_ReferencedWaveformChannels);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "2-2n", "1C", "WAVEFORM content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Uint16 group = 0;
        Uint16 channel = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from integer string */
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
    /* fill string with values from list */
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


/*
 *  CVS/RCS Log:
 *  $Log: dsrwavch.cc,v $
 *  Revision 1.26  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.25  2010-09-29 15:16:51  joergr
 *  Enhanced checking and reporting of standard violations in write() methods.
 *
 *  Revision 1.24  2009-10-13 14:57:52  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.23  2007-11-15 16:43:43  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.22  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.21  2005/12/08 15:48:21  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.20  2003/12/02 19:36:47  joergr
 *  Replaced wrong newline character sequence.
 *
 *  Revision 1.19  2003/10/14 10:23:33  meichel
 *  Explicitly spelled out constructor parameters in const instance declaration.
 *    Avoids incompatibility between gcc 3.2 and Borland C++.
 *
 *  Revision 1.18  2003/10/13 13:28:56  meichel
 *  Added code for explicit template specialization, needed for Borland C++
 *
 *  Revision 1.17  2003/10/09 14:43:02  joergr
 *  Replaced wrong newline character sequence.
 *
 *  Revision 1.16  2003/09/03 16:00:12  meichel
 *  Added standard includes needed on Win32
 *
 *  Revision 1.15  2003/08/07 17:29:13  joergr
 *  Removed libxml dependency from header files. Simplifies linking (MSVC).
 *
 *  Revision 1.14  2003/08/07 14:16:22  joergr
 *  Added new putString() method.
 *  Adapted for use of OFListConstIterator, needed for compiling with HAVE_STL.
 *
 *  Revision 1.13  2003/07/11 14:41:38  joergr
 *  Renamed member variable.
 *
 *  Revision 1.12  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.11  2003/06/04 12:40:02  meichel
 *  Replaced protected inheritance from OFList with protected aggregation
 *
 *  Revision 1.10  2003/06/03 10:16:46  meichel
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
