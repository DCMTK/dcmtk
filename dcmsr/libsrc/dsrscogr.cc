/*
 *
 *  Copyright (C) 2000-2003, OFFIS
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
 *    classes: DSRGraphicDataList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2003-07-11 14:41:38 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrscogr.h"


/* declared in class DSRListOfItems<T> */
const DSRGraphicDataItem DSRListOfItems<DSRGraphicDataItem>::EmptyItem;


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


OFCondition DSRGraphicDataList::print(ostream &stream,
                                      const size_t flags,
                                      const char pairSeparator,
                                      const char itemSeparator) const
{
    const OFListIterator(DSRGraphicDataItem) endPos = ItemList.end();
    OFListIterator(DSRGraphicDataItem) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        stream << (*iterator).Column << pairSeparator << (*iterator).Row;
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


OFCondition DSRGraphicDataList::read(DcmItem &dataset,
                                     OFConsole *logStream)
{
    /* get floating point string from dataset */
    DcmFloatingPointSingle delem(DCM_GraphicData);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "2-2n", "1", logStream, "SCOORD content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Float32 column = 0;
        Float32 row = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from floating point string */
        unsigned long i = 0;
        while ((i < count) && (result.good()))
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


OFCondition DSRGraphicDataList::write(DcmItem &dataset,
                                      OFConsole * /* logStream */) const
{
    OFCondition result = EC_Normal;
    /* fill string with values from list */
    DcmFloatingPointSingle delem(DCM_GraphicData);
    const OFListIterator(DSRGraphicDataItem) endPos = ItemList.end();
    OFListIterator(DSRGraphicDataItem) iterator = ItemList.begin();
    unsigned long i = 0;
    while ((iterator != endPos) && (result.good()))
    {
        result = delem.putFloat32((*iterator).Column, i++);
        if (result.good())
            result = delem.putFloat32((*iterator).Row, i++);
        iterator++;
    }
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmFloatingPointSingle(delem));
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


/*
 *  CVS/RCS Log:
 *  $Log: dsrscogr.cc,v $
 *  Revision 1.12  2003-07-11 14:41:38  joergr
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
