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
 *    classes: DSRReferencedTimeOffsetList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-26 14:40:28 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtcoto.h"


/* declared in class DSRListOfItems<T> */
//const Float64 DSRListOfItems<Float64>::EmptyItem;


DSRReferencedTimeOffsetList::DSRReferencedTimeOffsetList()
  : DSRListOfItems<Float64>()
{
}


DSRReferencedTimeOffsetList::DSRReferencedTimeOffsetList(const DSRReferencedTimeOffsetList &list)
  : DSRListOfItems<Float64>(list)
{
}


DSRReferencedTimeOffsetList::~DSRReferencedTimeOffsetList()
{
}


DSRReferencedTimeOffsetList &DSRReferencedTimeOffsetList::operator=(const DSRReferencedTimeOffsetList &list)
{
    DSRListOfItems<Float64>::operator=(list);
    return *this;
}


E_Condition DSRReferencedTimeOffsetList::print(ostream &stream,
                                                   const size_t flags) const
{
    const OFListIterator(Float64) endPos = OFList<Float64>::end();
    OFListIterator(Float64) iterator = OFList<Float64>::begin();
    while (iterator != endPos)
    {
        stream << (*iterator);
        iterator++;
        if (flags & DSRTypes::PF_shortenLongItemValues)
        {
            stream << ",...";
            /* goto last item */
            iterator = endPos;
        } else if (iterator != endPos)
            stream << ",";
    }
    return EC_Normal;
}


E_Condition DSRReferencedTimeOffsetList::read(DcmItem &dataset,
                                              OFConsole *logStream)
{
    /* get decimal string from dataset */
    DcmDecimalString delem(DCM_ReferencedTimeOffsets);
    E_Condition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", logStream, "TCOORD content item");
    if (result == EC_Normal)
    {
        /* clear internal list */
        clear();
        Float64 value = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from decimal string */
        for (unsigned long i = 0; i < count; i++)
        {
            if (delem.getFloat64(value, i) == EC_Normal)
                addItem(value);
        }
    }
    return result;
}


E_Condition DSRReferencedTimeOffsetList::write(DcmItem &dataset,
                                               OFConsole * /* logStream */) const
{
    E_Condition result = EC_Normal;
    /* fill string with values from list */
    OFString string;
    char buffer[32];
    const OFListIterator(Float64) endPos = OFList<Float64>::end();
    OFListIterator(Float64) iterator = OFList<Float64>::begin();
    while (iterator != endPos)
    {
        if (string.length() > 0)
            string += '\\';
    	sprintf(buffer, "%f", *iterator);
        string += buffer;
        iterator++;
    }
    /* set decimal string */
    DcmDecimalString delem(DCM_ReferencedTimeOffsets);
    result = delem.putOFStringArray(string);
    /* add to dataset */
    if (result == EC_Normal)
        result = DSRTypes::addElementToDataset(result, dataset, new DcmDecimalString(delem));
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcoto.cc,v $
 *  Revision 1.1  2000-10-26 14:40:28  joergr
 *  Added support for TCOORD content item.
 *
 *
 *
 */
