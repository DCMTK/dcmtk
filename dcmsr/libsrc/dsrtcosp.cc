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
 *    classes: DSRReferencedSamplePositionList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-11-06 11:34:25 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtcosp.h"


/* declared in class DSRListOfItems<T> */
//const Uint32 DSRListOfItems<Uint32>::EmptyItem;


DSRReferencedSamplePositionList::DSRReferencedSamplePositionList()
  : DSRListOfItems<Uint32>()
{
}


DSRReferencedSamplePositionList::DSRReferencedSamplePositionList(const DSRReferencedSamplePositionList &list)
  : DSRListOfItems<Uint32>(list)
{
}


DSRReferencedSamplePositionList::~DSRReferencedSamplePositionList()
{
}


DSRReferencedSamplePositionList &DSRReferencedSamplePositionList::operator=(const DSRReferencedSamplePositionList &list)
{
    DSRListOfItems<Uint32>::operator=(list);
    return *this;
}


E_Condition DSRReferencedSamplePositionList::print(ostream &stream,
                                                   const size_t flags,
                                                   const char separator) const
{
    const OFListIterator(Uint32) endPos = OFList<Uint32>::end();
    OFListIterator(Uint32) iterator = OFList<Uint32>::begin();
    while (iterator != endPos)
    {
        stream << (*iterator);
        iterator++;
        if (flags & DSRTypes::PF_shortenLongItemValues)
        {
            stream << separator << "...";
            /* goto last item */
            iterator = endPos;
        } else if (iterator != endPos)
            stream << separator;
    }
    return EC_Normal;
}


E_Condition DSRReferencedSamplePositionList::read(DcmItem &dataset,
                                                  OFConsole *logStream)
{
    /* get element from dataset */
    DcmUnsignedLong delem(DCM_ReferencedSamplePositions);
    E_Condition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", logStream, "TCOORD content item");
    if (result == EC_Normal)
    {
        /* clear internal list */
        clear();
        Uint32 value = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from integer string */
        for (unsigned long i = 0; i < count; i++)
        {
            if (delem.getUint32(value, i) == EC_Normal)
                addItem(value);
        }
    }
    return result;
}


E_Condition DSRReferencedSamplePositionList::write(DcmItem &dataset,
                                                   OFConsole * /* logStream */) const
{
    E_Condition result = EC_Normal;
    unsigned long i = 0;
    /* create element */
    DcmUnsignedLong delem(DCM_ReferencedSamplePositions);
    const OFListIterator(Uint32) endPos = OFList<Uint32>::end();
    OFListIterator(Uint32) iterator = OFList<Uint32>::begin();
    /* set elemnent values */
    while (iterator != endPos)
    {
        delem.putUint32(*iterator, i++);
        iterator++;
    }
    /* add to dataset */
    if (result == EC_Normal)
        result = DSRTypes::addElementToDataset(result, dataset, new DcmUnsignedLong(delem));
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcosp.cc,v $
 *  Revision 1.2  2000-11-06 11:34:25  joergr
 *  Added parameter to print() method specifying the item separator character.
 *
 *  Revision 1.1  2000/10/26 14:40:28  joergr
 *  Added support for TCOORD content item.
 *
 *
 *
 */
