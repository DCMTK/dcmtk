/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *  Update Date:      $Date: 2000-10-18 17:21:40 $
 *  CVS/RCS Revision: $Revision: 1.2 $
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


DSRGraphicDataList::DSRGraphicDataList(const DSRGraphicDataList &list)
  : DSRListOfItems<DSRGraphicDataItem>(list)
{
}


DSRGraphicDataList::~DSRGraphicDataList()
{
}


DSRGraphicDataList &DSRGraphicDataList::operator=(const DSRGraphicDataList &list)
{
    DSRListOfItems<DSRGraphicDataItem>::operator=(list);
    return *this;
}


E_Condition DSRGraphicDataList::print(ostream &stream,
                                      const size_t flags) const
{
    const OFListIterator(DSRGraphicDataItem) endPos = OFList<DSRGraphicDataItem>::end();
    OFListIterator(DSRGraphicDataItem) iterator = OFList<DSRGraphicDataItem>::begin();
    while (iterator != endPos)
    {
        stream << (*iterator).Column << "/" << (*iterator).Row;
        iterator++;
        if (flags & DSRTypes::PF_shortenLongItemValues)
        {
            stream << ",...";
            iterator = endPos;
        } else if (iterator != endPos)
            stream << ",";
    }
    return EC_Normal;
}


E_Condition DSRGraphicDataList::read(DcmItem &dataset,
                                     OFConsole *logStream)
{
    /* get floating point string from dataset */
    DcmFloatingPointSingle delem(DCM_GraphicData);
    E_Condition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "2-2n", "1", logStream);
    if (result == EC_Normal)
    {
        /* clear internal list */
        clear();
        DSRGraphicDataItem value;
        const unsigned long count = delem.getVM();
        /* fill list with values from floating point string */
        unsigned long i = 0;
        while ((i < count) && (result == EC_Normal))
        {
            result = delem.getFloat32(value.Column, i++);
            if (result == EC_Normal)
                result = delem.getFloat32(value.Row, i++);
            if (result == EC_Normal)
                DSRListOfItems<DSRGraphicDataItem>::addItem(value);
        }
    }
    return result;
}


E_Condition DSRGraphicDataList::write(DcmItem &dataset,
                                      OFConsole * /* logStream */) const
{
    E_Condition result = EC_Normal;
    /* fill string with values from list */
    DcmFloatingPointSingle delem(DCM_GraphicData);
    const OFListIterator(DSRGraphicDataItem) endPos = OFList<DSRGraphicDataItem>::end();
    OFListIterator(DSRGraphicDataItem) iterator = OFList<DSRGraphicDataItem>::begin();
    unsigned long i = 0;
    while ((iterator != endPos) && (result == EC_Normal))
    {
        result = delem.putFloat32((*iterator).Column, i++);
        if (result == EC_Normal)
            result = delem.putFloat32((*iterator).Row, i++);
        iterator++;
    }
    /* add to dataset */
    if (result == EC_Normal)
        result = DSRTypes::addElementToDataset(result, dataset, new DcmFloatingPointSingle(delem));
    return result;
}


const DSRGraphicDataItem &DSRGraphicDataList::getItem(const size_t idx) const
{
    /* hidden by the following getItem() method */
    return DSRListOfItems<DSRGraphicDataItem>::getItem(idx);
}


E_Condition DSRGraphicDataList::getItem(const size_t idx,
                                        Float32 &column,
                                        Float32 &row) const
{
    DSRGraphicDataItem item;    /* default: 0,0 */
    E_Condition result = DSRListOfItems<DSRGraphicDataItem>::getItem(idx, item);    
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
 *  Revision 1.2  2000-10-18 17:21:40  joergr
 *  Added comments.
 *
 *  Revision 1.1  2000/10/13 07:52:24  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
