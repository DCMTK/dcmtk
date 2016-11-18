/*
 *
 *  Copyright (C) 2000-2016, OFFIS e.V.
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
 *    classes: DSRReferencedSamplePositionList
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtcosp.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrul.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofdiag.h"

// This is not about specialization but static member initialization, the
// Visual Studio warning is therefore inappropriate and suppressed.
#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_NO_SUITABLE_DEFINITION_FOR_TEMPLATE_INSTANTIATION
/* declared in class DSRListOfItems<T> */
DCMTK_EXPLICIT_SPECIALIZATION
const Uint32 DSRListOfItems<Uint32>::EmptyItem = 0;
#include DCMTK_DIAGNOSTIC_POP


DSRReferencedSamplePositionList::DSRReferencedSamplePositionList()
  : DSRListOfItems<Uint32>()
{
}


DSRReferencedSamplePositionList::DSRReferencedSamplePositionList(const DSRReferencedSamplePositionList &lst)
  : DSRListOfItems<Uint32>(lst)
{
}


DSRReferencedSamplePositionList::~DSRReferencedSamplePositionList()
{
}


DSRReferencedSamplePositionList &DSRReferencedSamplePositionList::operator=(const DSRReferencedSamplePositionList &lst)
{
    DSRListOfItems<Uint32>::operator=(lst);
    return *this;
}


OFCondition DSRReferencedSamplePositionList::print(STD_NAMESPACE ostream &stream,
                                                   const size_t flags,
                                                   const char separator) const
{
    const OFListConstIterator(Uint32) endPos = ItemList.end();
    OFListConstIterator(Uint32) iterator = ItemList.begin();
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


OFCondition DSRReferencedSamplePositionList::read(DcmItem &dataset,
                                                  const size_t /*flags*/)
{
    /* get element from dataset */
    DcmUnsignedLong delem(DCM_ReferencedSamplePositions);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", "TCOORD content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Uint32 value = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from integer string */
        for (unsigned long i = 0; i < count; i++)
        {
            if (delem.getUint32(value, i).good())
                addItem(value);
        }
    }
    return result;
}


OFCondition DSRReferencedSamplePositionList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    unsigned long i = 0;
    /* create element */
    DcmUnsignedLong delem(DCM_ReferencedSamplePositions);
    const OFListConstIterator(Uint32) endPos = ItemList.end();
    OFListConstIterator(Uint32) iterator = ItemList.begin();
    /* set element values */
    while (iterator != endPos)
    {
        delem.putUint32(*iterator, i++);
        iterator++;
    }
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmUnsignedLong(delem), "1-n", "1", "TCOORD content item");
    return result;
}


OFCondition DSRReferencedSamplePositionList::putString(const char *stringValue)
{
    OFCondition result = EC_Normal;
    /* clear internal list */
    clear();
    /* check input string */
    if ((stringValue != NULL) && (strlen(stringValue) > 0))
    {
        Uint32 value = 0;
        const char *ptr = stringValue;
        /* retrieve sample positions from string */
        while (result.good() && (ptr != NULL))
        {
#if SIZEOF_LONG == 8
            if (sscanf(ptr, "%u", &value) == 1)
#else
            if (sscanf(ptr, "%lu", &value) == 1)
#endif
            {
                addItem(value);
                /* jump to next time offset */
                ptr = strchr(ptr, ',');
                if (ptr != NULL)
                    ptr++;
            } else
                result = EC_CorruptedData;
        }
    }
    return result;
}
