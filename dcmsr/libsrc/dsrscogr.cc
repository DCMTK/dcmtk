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
 *    classes: DSRGraphicDataList
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrscogr.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrfl.h"

#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofdiag.h"

// This is not about specialization but static member initialization, the
// Visual Studio warning is therefore inappropriate and suppressed.
#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_NO_SUITABLE_DEFINITION_FOR_TEMPLATE_INSTANTIATION
/* declared in class DSRListOfItems<T> */
DCMTK_EXPLICIT_SPECIALIZATION
const DSRGraphicDataItem DSRListOfItems<DSRGraphicDataItem>::EmptyItem(0, 0);
#include DCMTK_DIAGNOSTIC_POP


DSRGraphicDataList::DSRGraphicDataList()
  : DSRListOfItems<DSRGraphicDataItem>()
{
}


DSRGraphicDataList::DSRGraphicDataList(const DSRGraphicDataList &lst)
  : DSRListOfItems<DSRGraphicDataItem>(lst)
{
}


DSRGraphicDataList::~DSRGraphicDataList()
{
}


DSRGraphicDataList &DSRGraphicDataList::operator=(const DSRGraphicDataList &lst)
{
    DSRListOfItems<DSRGraphicDataItem>::operator=(lst);
    return *this;
}


OFCondition DSRGraphicDataList::print(STD_NAMESPACE ostream &stream,
                                      const size_t flags,
                                      const char pairSeparator,
                                      const char itemSeparator) const
{
    char buffer[64];
    const OFListConstIterator(DSRGraphicDataItem) endPos = ItemList.end();
    OFListConstIterator(DSRGraphicDataItem) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        /* need to convert float to avoid problems with decimal point ('.' or ',') */
        OFStandard::ftoa(buffer, sizeof(buffer), (*iterator).Column);
        stream << buffer << pairSeparator;
        OFStandard::ftoa(buffer, sizeof(buffer), (*iterator).Row);
        stream << buffer;
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


OFCondition DSRGraphicDataList::read(DcmItem &dataset,
                                     const size_t /*flags*/)
{
    /* get floating point string from dataset */
    DcmFloatingPointSingle delem(DCM_GraphicData);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "2-2n", "1", "SCOORD content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Float32 column = 0;
        Float32 row = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from floating point string */
        unsigned long i = 0;
        while ((i < count) && result.good())
        {
            result = delem.getFloat32(column, i++);
            if (result.good())
            {
                result = delem.getFloat32(row, i++);
                if (result.good())
                    addItem(column, row);
            }
        }
    }
    return result;
}


OFCondition DSRGraphicDataList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* fill string with values from list */
    DcmFloatingPointSingle delem(DCM_GraphicData);
    const OFListConstIterator(DSRGraphicDataItem) endPos = ItemList.end();
    OFListConstIterator(DSRGraphicDataItem) iterator = ItemList.begin();
    unsigned long i = 0;
    while ((iterator != endPos) && result.good())
    {
        result = delem.putFloat32((*iterator).Column, i++);
        if (result.good())
            result = delem.putFloat32((*iterator).Row, i++);
        iterator++;
    }
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmFloatingPointSingle(delem), "2-2n", "1", "SCOORD content item");
    return result;
}


const DSRGraphicDataItem &DSRGraphicDataList::getItem(const size_t idx) const
{
    /* hidden by the following getItem() method */
    return DSRListOfItems<DSRGraphicDataItem>::getItem(idx);
}


OFCondition DSRGraphicDataList::getItem(const size_t idx,
                                        Float32 &column,
                                        Float32 &row) const
{
    DSRGraphicDataItem item;    /* default: 0,0 */
    OFCondition result = DSRListOfItems<DSRGraphicDataItem>::getItem(idx, item);
    column = item.Column;
    row = item.Row;
    return result;
}


void DSRGraphicDataList::addItem(const Float32 column,
                                 const Float32 row)
{
    DSRListOfItems<DSRGraphicDataItem>::addItem(DSRGraphicDataItem(column, row));
}


OFCondition DSRGraphicDataList::putString(const char *stringValue)
{
    OFCondition result = EC_Normal;
    /* clear internal list */
    clear();
    /* check input string */
    if ((stringValue != NULL) && (strlen(stringValue) > 0))
    {
        Float32 column = 0;
        Float32 row = 0;
        OFBool success = OFFalse;
        const char *ptr = stringValue;
        /* retrieve data pairs from string */
        while (result.good() && (ptr != NULL))
        {
            /* first get the 'column' value */
            column = OFstatic_cast(Float32, OFStandard::atof(ptr, &success));
            if (success)
            {
                ptr = strchr(ptr, '/');
                if (ptr != NULL)
                {
                    /* then get the 'row' value */
                    row = OFstatic_cast(Float32, OFStandard::atof(++ptr, &success));
                    if (success)
                    {
                        addItem(column, row);
                        /* jump to next data pair */
                        ptr = strchr(ptr, ',');
                        if (ptr != NULL)
                            ptr++;
                    } else
                        result = EC_CorruptedData;
                } else
                    result = EC_CorruptedData;
            } else
                result = EC_CorruptedData;
        }
    }
    return result;
}
