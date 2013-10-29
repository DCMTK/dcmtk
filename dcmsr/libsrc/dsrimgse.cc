/*
 *
 *  Copyright (C) 2013, OFFIS e.V.
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
 *    classes: DSRImageSegmentList
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrimgse.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION
#define EXPLICIT_SPECIALIZATION template<>
#else
#define EXPLICIT_SPECIALIZATION
#endif

/* declared in class DSRListOfItems<T> */
EXPLICIT_SPECIALIZATION const Uint16 DSRListOfItems<Uint16>::EmptyItem = 0;


DSRImageSegmentList::DSRImageSegmentList()
  : DSRListOfItems<Uint16>()
{
}


DSRImageSegmentList::DSRImageSegmentList(const DSRImageSegmentList &lst)
  : DSRListOfItems<Uint16>(lst)
{
}


DSRImageSegmentList::~DSRImageSegmentList()
{
}


DSRImageSegmentList &DSRImageSegmentList::operator=(const DSRImageSegmentList &lst)
{
    DSRListOfItems<Uint16>::operator=(lst);
    return *this;
}


OFCondition DSRImageSegmentList::print(STD_NAMESPACE ostream &stream,
                                     const size_t flags,
                                     const char separator) const
{
    const OFListConstIterator(Uint16) endPos = ItemList.end();
    OFListConstIterator(Uint16) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        stream << (*iterator);
        iterator++;
        if (iterator != endPos)
        {
            if (flags & DSRTypes::PF_shortenLongItemValues)
            {
                stream << separator << "...";
                /* goto last item */
                iterator = endPos;
            } else
                stream << separator;
        }
    }
    return EC_Normal;
}


OFCondition DSRImageSegmentList::read(DcmItem &dataset)
{
    /* get integer array from dataset */
    DcmUnsignedShort delem(DCM_ReferencedSegmentNumber);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", "IMAGE content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Uint16 value = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from integer array */
        unsigned long i = 0;
        while ((i < count) && result.good())
        {
            result = delem.getUint16(value, i++);
            if (result.good())
                addItem(value);
        }
    }
    return result;
}


OFCondition DSRImageSegmentList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* fill integer array with values from list */
    DcmUnsignedShort delem(DCM_ReferencedSegmentNumber);
    const OFListConstIterator(Uint16) endPos = ItemList.end();
    OFListConstIterator(Uint16) iterator = ItemList.begin();
    unsigned long i = 0;
    while ((iterator != endPos) && result.good())
    {
        result = delem.putUint16((*iterator), i++);
        iterator++;
    }
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmUnsignedShort(delem), "1-n", "1", "IMAGE content item");
    return result;
}


OFCondition DSRImageSegmentList::putString(const char *stringValue)
{
    OFCondition result = EC_Normal;
    /* clear internal list */
    clear();
    /* check input string */
    if ((stringValue != NULL) && (strlen(stringValue) > 0))
    {
        Uint16 value = 0;
        const char *ptr = stringValue;
        /* retrieve segment values from string */
        while (result.good() && (ptr != NULL))
        {
            if (sscanf(ptr, "%hu", &value) == 1)
            {
                addItem(value);
                /* jump to next segment value */
                ptr = strchr(ptr, ',');
                if (ptr != NULL)
                    ptr++;
            } else
                result = EC_CorruptedData;
        }
    }
    return result;
}
