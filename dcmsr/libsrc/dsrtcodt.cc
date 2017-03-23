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
 *    classes: DSRReferencedDateTimeList
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtcodt.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrdt.h"


// global empty item object so it gets initialized and cleaned up by the linker
const OFString OFStringEmptyItem;

template<>
const OFString& DSRgetEmptyItem<OFString>()
{
    return OFStringEmptyItem;
}


DSRReferencedDateTimeList::DSRReferencedDateTimeList()
  : DSRListOfItems<OFString>()
{
}


DSRReferencedDateTimeList::DSRReferencedDateTimeList(const DSRReferencedDateTimeList &lst)
  : DSRListOfItems<OFString>(lst)
{
}


DSRReferencedDateTimeList::~DSRReferencedDateTimeList()
{
}


DSRReferencedDateTimeList &DSRReferencedDateTimeList::operator=(const DSRReferencedDateTimeList &lst)
{
    DSRListOfItems<OFString>::operator=(lst);
    return *this;
}


OFCondition DSRReferencedDateTimeList::print(STD_NAMESPACE ostream &stream,
                                             const size_t flags,
                                             const char separator) const
{
    const OFListConstIterator(OFString) endPos = ItemList.end();
    OFListConstIterator(OFString) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        stream << (*iterator);
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


OFCondition DSRReferencedDateTimeList::read(DcmItem &dataset,
                                            const size_t /*flags*/)
{
    /* get string array from dataset */
    DcmDateTime delem(DCM_ReferencedDateTime);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", "TCOORD content item");
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


OFCondition DSRReferencedDateTimeList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* fill string with values from list */
    OFString tmpString;
    const OFListConstIterator(OFString) endPos = ItemList.end();
    OFListConstIterator(OFString) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        if (!tmpString.empty())
            tmpString += '\\';
        tmpString += *iterator;
        iterator++;
    }
    /* set decimal string */
    DcmDateTime delem(DCM_ReferencedDateTime);
    result = delem.putOFStringArray(tmpString);
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmDateTime(delem), "1-n", "1", "TCOORD content item");
    return result;
}


OFCondition DSRReferencedDateTimeList::putString(const char *stringValue)
{
    OFCondition result = EC_Normal;
    /* clear internal list */
    clear();
    /* check input string */
    if ((stringValue != NULL) && (strlen(stringValue) > 0))
    {
        const char *ptr1 = stringValue;
        const char *ptr2;
        /* retrieve date/time values from string */
        do {
            ptr2 = strchr(ptr1, ',');
            if (ptr2 != NULL)
            {
                if (ptr2 > ptr1)
                {
                    addItem(OFString(ptr1, ptr2 - ptr1));
                    /* jump to next time offset */
                    ptr1 = ptr2 + 1;
                } else
                    result = EC_CorruptedData;
            } else
                addItem(ptr1);
        } while (result.good() && (ptr2 != NULL));
    }
    return result;
}
