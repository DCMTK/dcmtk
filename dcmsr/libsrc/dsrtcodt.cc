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
 *    classes: DSRReferencedDatetimeList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-04 14:26:54 $
 *  CVS/RCS Revision: $Revision: 1.8 $
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


DSRReferencedDatetimeList::DSRReferencedDatetimeList(const DSRReferencedDatetimeList &lst)
  : DSRListOfItems<OFString>(lst)
{
}


DSRReferencedDatetimeList::~DSRReferencedDatetimeList()
{
}


DSRReferencedDatetimeList &DSRReferencedDatetimeList::operator=(const DSRReferencedDatetimeList &lst)
{
    DSRListOfItems<OFString>::operator=(lst);
    return *this;
}


OFCondition DSRReferencedDatetimeList::print(ostream &stream,
                                             const size_t flags,
                                             const char separator) const
{
    const OFListIterator(OFString) endPos = list_.end();
    OFListIterator(OFString) iterator = list_.begin();
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


OFCondition DSRReferencedDatetimeList::read(DcmItem &dataset,
                                            OFConsole *logStream)
{
    /* get string array from dataset */
    DcmDateTime delem(DCM_ReferencedDatetime);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", logStream, "TCOORD content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        OFString value;
        const unsigned long count = delem.getVM();
        /* fill list with values from string array */
        for (unsigned long i = 0; i < count; i++)
        {
            if (delem.getOFString(value, i).good())
                addItem(value);
        }
    }
    return result;
}


OFCondition DSRReferencedDatetimeList::write(DcmItem &dataset,
                                             OFConsole * /* logStream */) const
{
    OFCondition result = EC_Normal;
    /* fill string with values from list */
    OFString string;
    const OFListIterator(OFString) endPos = list_.end();
    OFListIterator(OFString) iterator = list_.begin();
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
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmDateTime(delem));
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcodt.cc,v $
 *  Revision 1.8  2003-06-04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.7  2003/06/04 12:40:02  meichel
 *  Replaced protected inheritance from OFList with protected aggregation
 *
 *  Revision 1.6  2003/06/03 10:16:46  meichel
 *  Renamed local variables to avoid name clashes with STL
 *
 *  Revision 1.5  2001/10/10 15:30:02  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.4  2001/09/26 13:04:25  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.3  2001/06/01 15:51:10  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2000/11/06 11:34:24  joergr
 *  Added parameter to print() method specifying the item separator character.
 *
 *  Revision 1.1  2000/10/26 14:40:27  joergr
 *  Added support for TCOORD content item.
 *
 *
 */
