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
 *    classes: DSRGraphicDataList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:41 $
 *  CVS/RCS Revision: $Revision: 1.23 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmsr/dsrscogr.h"
#include "dcmtk/ofstd/ofstd.h"

#ifdef HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION
#define EXPLICIT_SPECIALIZATION template<>
#else
#define EXPLICIT_SPECIALIZATION
#endif

EXPLICIT_SPECIALIZATION const DSRGraphicDataItem DSRListOfItems<DSRGraphicDataItem>::EmptyItem(0,0);


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


OFCondition DSRGraphicDataList::read(DcmItem &dataset)
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


/*
 *  CVS/RCS Log:
 *  $Log: dsrscogr.cc,v $
 *  Revision 1.23  2010-10-14 13:14:41  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.22  2010-09-29 15:16:51  joergr
 *  Enhanced checking and reporting of standard violations in write() methods.
 *
 *  Revision 1.21  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.20  2007-11-15 16:43:43  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.19  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.18  2005/12/08 15:48:04  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.17  2004/04/20 08:10:36  joergr
 *  Added explicit type cast to return value of OFStandard::atof() to avoid
 *  warnings reported by Visual Studio 7.
 *
 *  Revision 1.16  2003/12/02 19:36:47  joergr
 *  Replaced wrong newline character sequence.
 *
 *  Revision 1.15  2003/10/14 10:23:33  meichel
 *  Explicitly spelled out constructor parameters in const instance declaration.
 *    Avoids incompatibility between gcc 3.2 and Borland C++.
 *
 *  Revision 1.14  2003/10/13 13:28:56  meichel
 *  Added code for explicit template specialization, needed for Borland C++
 *
 *  Revision 1.13  2003/08/07 13:43:18  joergr
 *  Added new putString() method.
 *  Adapted for use of OFListConstIterator, needed for compiling with HAVE_STL.
 *
 *  Revision 1.12  2003/07/11 14:41:38  joergr
 *  Renamed member variable.
 *
 *  Revision 1.11  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.10  2003/06/04 12:40:02  meichel
 *  Replaced protected inheritance from OFList with protected aggregation
 *
 *  Revision 1.9  2003/06/03 10:16:46  meichel
 *  Renamed local variables to avoid name clashes with STL
 *
 *  Revision 1.8  2001/10/10 15:30:00  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.7  2001/09/26 13:04:24  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.6  2001/06/01 15:51:09  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2000/11/06 11:34:24  joergr
 *  Added parameter to print() method specifying the item separator character.
 *
 *  Revision 1.4  2000/10/26 14:33:31  joergr
 *  Reworked read() method.
 *
 *  Revision 1.3  2000/10/19 16:06:19  joergr
 *  Added optional module name to read method to provide more detailed warning
 *  messages.
 *
 *  Revision 1.2  2000/10/18 17:21:40  joergr
 *  Added comments.
 *
 *  Revision 1.1  2000/10/13 07:52:24  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
