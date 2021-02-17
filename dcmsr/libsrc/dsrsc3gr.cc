/*
 *
 *  Copyright (C) 2010-2021, OFFIS e.V.
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
 *    classes: DSRGraphicData3DList
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrsc3gr.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrfl.h"

#include "dcmtk/ofstd/ofstd.h"


// global empty item object so it gets initialized and cleaned up by the linker
const DSRGraphicData3DItem DSRGraphicData3DEmptyItem(0, 0, 0);

template<>
const DSRGraphicData3DItem& DSRgetEmptyItem<DSRGraphicData3DItem>()
{
    return DSRGraphicData3DEmptyItem;
}


DSRGraphicData3DList::DSRGraphicData3DList()
  : DSRListOfItems<DSRGraphicData3DItem>()
{
}


DSRGraphicData3DList::DSRGraphicData3DList(const DSRGraphicData3DList &lst)
  : DSRListOfItems<DSRGraphicData3DItem>(lst)
{
}


DSRGraphicData3DList::~DSRGraphicData3DList()
{
}


DSRGraphicData3DList &DSRGraphicData3DList::operator=(const DSRGraphicData3DList &lst)
{
    DSRListOfItems<DSRGraphicData3DItem>::operator=(lst);
    return *this;
}


OFCondition DSRGraphicData3DList::print(STD_NAMESPACE ostream &stream,
                                        const size_t flags,
                                        const char tripletSeparator,
                                        const char itemSeparator) const
{
    char buffer[64];
    const OFListConstIterator(DSRGraphicData3DItem) endPos = ItemList.end();
    OFListConstIterator(DSRGraphicData3DItem) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        /* need to convert float to avoid problems with decimal point ('.' or ',') */
        OFStandard::ftoa(buffer, sizeof(buffer), (*iterator).XCoord, 0, 0, 9 /* FLT_DECIMAL_DIG for DICOM FL */);
        stream << buffer << tripletSeparator;
        OFStandard::ftoa(buffer, sizeof(buffer), (*iterator).YCoord, 0, 0, 9 /* FLT_DECIMAL_DIG for DICOM FL */);
        stream << buffer << tripletSeparator;
        OFStandard::ftoa(buffer, sizeof(buffer), (*iterator).ZCoord, 0, 0, 9 /* FLT_DECIMAL_DIG for DICOM FL */);
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


OFCondition DSRGraphicData3DList::read(DcmItem &dataset,
                                       const size_t /*flags*/)
{
    /* get floating point string from dataset */
    DcmFloatingPointSingle delem(DCM_GraphicData);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "3-3n", "1", "SCOORD3D content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Float32 x = 0;
        Float32 y = 0;
        Float32 z = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from floating point string */
        unsigned long i = 0;
        while ((i < count) && result.good())
        {
            result = delem.getFloat32(x, i++);
            if (result.good())
            {
                result = delem.getFloat32(y, i++);
                if (result.good())
                {
                    result = delem.getFloat32(z, i++);
                    if (result.good())
                        addItem(x, y, z);
                }
            }
        }
    }
    return result;
}


OFCondition DSRGraphicData3DList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* fill string with values from list */
    DcmFloatingPointSingle delem(DCM_GraphicData);
    const OFListConstIterator(DSRGraphicData3DItem) endPos = ItemList.end();
    OFListConstIterator(DSRGraphicData3DItem) iterator = ItemList.begin();
    unsigned long i = 0;
    while ((iterator != endPos) && result.good())
    {
        result = delem.putFloat32((*iterator).XCoord, i++);
        if (result.good())
        {
            result = delem.putFloat32((*iterator).YCoord, i++);
            if (result.good())
                result = delem.putFloat32((*iterator).ZCoord, i++);
        }
        iterator++;
    }
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmFloatingPointSingle(delem), "3-3n", "1", "SCOORD3D content item");
    return result;
}


const DSRGraphicData3DItem &DSRGraphicData3DList::getItem(const size_t idx) const
{
    /* hidden by the following getItem() method */
    return DSRListOfItems<DSRGraphicData3DItem>::getItem(idx);
}


OFCondition DSRGraphicData3DList::getItem(const size_t idx,
                                          Float32 &x,
                                          Float32 &y,
                                          Float32 &z) const
{
    DSRGraphicData3DItem item;    /* default: 0,0,0 */
    OFCondition result = DSRListOfItems<DSRGraphicData3DItem>::getItem(idx, item);
    x = item.XCoord;
    y = item.YCoord;
    z = item.ZCoord;
    return result;
}


void DSRGraphicData3DList::addItem(const Float32 x,
                                   const Float32 y,
                                   const Float32 z)
{
    DSRListOfItems<DSRGraphicData3DItem>::addItem(DSRGraphicData3DItem(x, y, z));
}


OFCondition DSRGraphicData3DList::putString(const char *stringValue)
{
    OFCondition result = EC_Normal;
    /* clear internal list */
    clear();
    /* check input string */
    if ((stringValue != NULL) && (strlen(stringValue) > 0))
    {
        Float32 x = 0;
        Float32 y = 0;
        Float32 z = 0;
        OFBool success = OFFalse;
        const char *ptr = stringValue;
        /* retrieve data triplets from string */
        while (result.good() && (ptr != NULL))
        {
            /* first get the 'x' value */
            x = OFstatic_cast(Float32, OFStandard::atof(ptr, &success));
            if (success)
            {
                ptr = strchr(ptr, '/');
                if (ptr != NULL)
                {
                    /* then get the 'y' value */
                    y = OFstatic_cast(Float32, OFStandard::atof(++ptr, &success));
                    if (success)
                    {
                        ptr = strchr(ptr, '/');
                        if (ptr != NULL)
                        {
                            /* then get the 'z' value */
                            z = OFstatic_cast(Float32, OFStandard::atof(++ptr, &success));
                            if (success)
                            {
                                addItem(x, y, z);
                                /* jump to next data triplet */
                                ptr = strchr(ptr, ',');
                                if (ptr != NULL)
                                    ptr++;
                            }
                        } else
                            result = EC_CorruptedData;
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
