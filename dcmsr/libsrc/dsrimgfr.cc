/*
 *
 *  Copyright (C) 2000-2017, OFFIS e.V.
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
 *    classes: DSRImageFrameList
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrimgfr.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvris.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


template<>
const Sint32& DSRgetEmptyItem<Sint32>()
{
    // no need to be thread-safe, since it is only an int
    static const Sint32 t = 0;
    return t;
}


DSRImageFrameList::DSRImageFrameList()
  : DSRListOfItems<Sint32>()
{
}


DSRImageFrameList::DSRImageFrameList(const DSRImageFrameList &lst)
  : DSRListOfItems<Sint32>(lst)
{
}


DSRImageFrameList::~DSRImageFrameList()
{
}


DSRImageFrameList &DSRImageFrameList::operator=(const DSRImageFrameList &lst)
{
    DSRListOfItems<Sint32>::operator=(lst);
    return *this;
}


OFCondition DSRImageFrameList::print(STD_NAMESPACE ostream &stream,
                                     const size_t flags,
                                     const char separator) const
{
    const OFListConstIterator(Sint32) endPos = ItemList.end();
    OFListConstIterator(Sint32) iterator = ItemList.begin();
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


OFCondition DSRImageFrameList::read(DcmItem &dataset,
                                    const size_t /*flags*/)
{
    /* get integer string from dataset */
    DcmIntegerString delem(DCM_ReferencedFrameNumber);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", "IMAGE content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Sint32 value = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from integer string */
        unsigned long i = 0;
        while ((i < count) && result.good())
        {
            result = delem.getSint32(value, i++);
            if (result.good())
                addItem(value);
        }
    }
    return result;
}


OFCondition DSRImageFrameList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* fill integer string with values from list */
    OFString tmpString;
    char buffer[16];
    const OFListConstIterator(Sint32) endPos = ItemList.end();
    OFListConstIterator(Sint32) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        if (!tmpString.empty())
            tmpString += '\\';
#if SIZEOF_LONG == 8
        sprintf(buffer, "%d", *iterator);
#else
        sprintf(buffer, "%ld", *iterator);
#endif
        tmpString += buffer;
        iterator++;
    }
    /* set integer string */
    DcmIntegerString delem(DCM_ReferencedFrameNumber);
    result = delem.putOFStringArray(tmpString);
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmIntegerString(delem), "1-n", "1", "IMAGE content item");
    return result;
}


OFCondition DSRImageFrameList::putString(const char *stringValue)
{
    OFCondition result = EC_Normal;
    /* clear internal list */
    clear();
    /* check input string */
    if ((stringValue != NULL) && (strlen(stringValue) > 0))
    {
        Sint32 value = 0;
        const char *ptr = stringValue;
        /* retrieve frame values from string */
        while (result.good() && (ptr != NULL))
        {
#if SIZEOF_LONG == 8
            if (sscanf(ptr, "%d", &value) == 1)
#else
            if (sscanf(ptr, "%ld", &value) == 1)
#endif
            {
                addItem(value);
                /* jump to next frame value */
                ptr = strchr(ptr, ',');
                if (ptr != NULL)
                    ptr++;
            } else
                result = EC_CorruptedData;
        }
    }
    return result;
}
