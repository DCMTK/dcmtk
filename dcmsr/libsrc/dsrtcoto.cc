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
 *    classes: DSRReferencedTimeOffsetList
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtcoto.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrds.h"

#include "dcmtk/ofstd/ofstd.h"
#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


template<>
const Float64& DSRgetEmptyItem<Float64>()
{
    // no need to be thread-safe, since it is only a float
    static const Float64 t = 0;
    return t;
}


DSRReferencedTimeOffsetList::DSRReferencedTimeOffsetList()
  : DSRListOfItems<Float64>()
{
}


DSRReferencedTimeOffsetList::DSRReferencedTimeOffsetList(const DSRReferencedTimeOffsetList &lst)
  : DSRListOfItems<Float64>(lst)
{
}


DSRReferencedTimeOffsetList::~DSRReferencedTimeOffsetList()
{
}


DSRReferencedTimeOffsetList &DSRReferencedTimeOffsetList::operator=(const DSRReferencedTimeOffsetList &lst)
{
    DSRListOfItems<Float64>::operator=(lst);
    return *this;
}


OFCondition DSRReferencedTimeOffsetList::print(STD_NAMESPACE ostream &stream,
                                               const size_t flags,
                                               const char separator) const
{
    char buffer[64];
    const OFListConstIterator(Float64) endPos = ItemList.end();
    OFListConstIterator(Float64) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        /* need to convert float to avoid problems with decimal point ('.' or ',') */
        OFStandard::ftoa(buffer, sizeof(buffer), *iterator);
        stream << buffer;
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


OFCondition DSRReferencedTimeOffsetList::read(DcmItem &dataset,
                                              const size_t /*flags*/)
{
    /* get decimal string from dataset */
    DcmDecimalString delem(DCM_ReferencedTimeOffsets);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", "TCOORD content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Float64 value = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from decimal string */
        for (unsigned long i = 0; i < count; i++)
        {
            if (delem.getFloat64(value, i).good())
                addItem(value);
        }
    }
    return result;
}


OFCondition DSRReferencedTimeOffsetList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* fill string with values from list */
    OFString tmpString;
    char buffer[32];
    const OFListConstIterator(Float64) endPos = ItemList.end();
    OFListConstIterator(Float64) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        if (!tmpString.empty())
            tmpString += '\\';
        OFStandard::ftoa(buffer, sizeof(buffer), *iterator, OFStandard::ftoa_format_f);
        tmpString += buffer;
        iterator++;
    }
    /* set decimal string */
    DcmDecimalString delem(DCM_ReferencedTimeOffsets);
    result = delem.putOFStringArray(tmpString);
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmDecimalString(delem), "1-n", "1", "TCOORD content item");
    return result;
}


OFCondition DSRReferencedTimeOffsetList::putString(const char *stringValue)
{
    OFCondition result = EC_Normal;
    /* clear internal list */
    clear();
    /* check input string */
    if ((stringValue != NULL) && (strlen(stringValue) > 0))
    {
        Float64 value = 0;
        OFBool success = OFFalse;
        const char *ptr = stringValue;
        /* retrieve time offsets from string */
        while (result.good() && (ptr != NULL))
        {
            value = OFStandard::atof(ptr, &success);
            if (success)
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
