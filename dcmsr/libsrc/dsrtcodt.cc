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
 *    classes: DSRReferencedDatetimeList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-26 14:40:27 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtcodt.h"


/* declared in class DSRListOfItems<T> */
//const OFString DSRListOfItems<OFString>::EmptyItem;


DSRReferencedDatetimeList::DSRReferencedDatetimeList()
  : DSRListOfItems<OFString>()
{
}


DSRReferencedDatetimeList::DSRReferencedDatetimeList(const DSRReferencedDatetimeList &list)
  : DSRListOfItems<OFString>(list)
{
}


DSRReferencedDatetimeList::~DSRReferencedDatetimeList()
{
}


DSRReferencedDatetimeList &DSRReferencedDatetimeList::operator=(const DSRReferencedDatetimeList &list)
{
    DSRListOfItems<OFString>::operator=(list);
    return *this;
}


E_Condition DSRReferencedDatetimeList::print(ostream &stream,
                                             const size_t flags) const
{
    const OFListIterator(OFString) endPos = OFList<OFString>::end();
    OFListIterator(OFString) iterator = OFList<OFString>::begin();
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


E_Condition DSRReferencedDatetimeList::read(DcmItem &dataset,
                                            OFConsole *logStream)
{
    /* get string array from dataset */
    DcmDateTime delem(DCM_ReferencedDatetime);
    E_Condition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", logStream, "TCOORD content item");
    if (result == EC_Normal)
    {
        /* clear internal list */
        clear();
        OFString value;
        const unsigned long count = delem.getVM();
        /* fill list with values from string array */
        for (unsigned long i = 0; i < count; i++)
        {
            if (delem.getOFString(value, i) == EC_Normal)
                addItem(value);
        }
    }
    return result;
}


E_Condition DSRReferencedDatetimeList::write(DcmItem &dataset,
                                             OFConsole * /* logStream */) const
{
    E_Condition result = EC_Normal;
    /* fill string with values from list */
    OFString string;
    const OFListIterator(OFString) endPos = OFList<OFString>::end();
    OFListIterator(OFString) iterator = OFList<OFString>::begin();
    while (iterator != endPos)
    {
        if (string.length() > 0)
            string += '\\';
        string += *iterator;
        iterator++;
    }
    /* set decimal string */
    DcmDateTime delem(DCM_ReferencedDatetime);
    result = delem.putOFStringArray(string);
    /* add to dataset */
    if (result == EC_Normal)
        result = DSRTypes::addElementToDataset(result, dataset, new DcmDateTime(delem));
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcodt.cc,v $
 *  Revision 1.1  2000-10-26 14:40:27  joergr
 *  Added support for TCOORD content item.
 *
 *
 *
 */
