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
 *    classes: DSRReferencedSamplePositionList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:42 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtcosp.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


/* declared in class DSRListOfItems<T> */
//const Uint32 DSRListOfItems<Uint32>::EmptyItem;


DSRReferencedSamplePositionList::DSRReferencedSamplePositionList()
  : DSRListOfItems<Uint32>()
{
}


DSRReferencedSamplePositionList::DSRReferencedSamplePositionList(const DSRReferencedSamplePositionList &lst)
  : DSRListOfItems<Uint32>(lst)
{
}


DSRReferencedSamplePositionList::~DSRReferencedSamplePositionList()
{
}


DSRReferencedSamplePositionList &DSRReferencedSamplePositionList::operator=(const DSRReferencedSamplePositionList &lst)
{
    DSRListOfItems<Uint32>::operator=(lst);
    return *this;
}


OFCondition DSRReferencedSamplePositionList::print(STD_NAMESPACE ostream &stream,
                                                   const size_t flags,
                                                   const char separator) const
{
    const OFListConstIterator(Uint32) endPos = ItemList.end();
    OFListConstIterator(Uint32) iterator = ItemList.begin();
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


OFCondition DSRReferencedSamplePositionList::read(DcmItem &dataset)
{
    /* get element from dataset */
    DcmUnsignedLong delem(DCM_ReferencedSamplePositions);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", "TCOORD content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Uint32 value = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from integer string */
        for (unsigned long i = 0; i < count; i++)
        {
            if (delem.getUint32(value, i).good())
                addItem(value);
        }
    }
    return result;
}


OFCondition DSRReferencedSamplePositionList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    unsigned long i = 0;
    /* create element */
    DcmUnsignedLong delem(DCM_ReferencedSamplePositions);
    const OFListConstIterator(Uint32) endPos = ItemList.end();
    OFListConstIterator(Uint32) iterator = ItemList.begin();
    /* set element values */
    while (iterator != endPos)
    {
        delem.putUint32(*iterator, i++);
        iterator++;
    }
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmUnsignedLong(delem), "1-n", "1", "TCOORD content item");
    return result;
}


OFCondition DSRReferencedSamplePositionList::putString(const char *stringValue)
{
    OFCondition result = EC_Normal;
    /* clear internal list */
    clear();
    /* check input string */
    if ((stringValue != NULL) && (strlen(stringValue) > 0))
    {
        Uint32 value = 0;
        const char *ptr = stringValue;
        /* retrieve sample positions from string */
        while (result.good() && (ptr != NULL))
        {
#if SIZEOF_LONG == 8
            if (sscanf(ptr, "%u", &value) == 1)
#else
            if (sscanf(ptr, "%lu", &value) == 1)
#endif
            {
                addItem(value);
                /* jump to next time offset */
                ptr = strchr(ptr, ',');
                if (ptr != NULL)
                    ptr++;
            } else
                result = EC_CorruptedData;
        }
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcosp.cc,v $
 *  Revision 1.17  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.16  2010-09-29 15:16:51  joergr
 *  Enhanced checking and reporting of standard violations in write() methods.
 *
 *  Revision 1.15  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.14  2007-11-15 16:43:43  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.13  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.12  2005/12/08 15:48:11  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.11  2003/08/11 18:32:26  joergr
 *  Included "stdio" header file required for gcc 3.2.3 on Debian Linux.
 *
 *  Revision 1.10  2003/08/07 13:54:49  joergr
 *  Added new putString() method.
 *  Adapted for use of OFListConstIterator, needed for compiling with HAVE_STL.
 *
 *  Revision 1.9  2003/07/11 14:41:38  joergr
 *  Renamed member variable.
 *
 *  Revision 1.8  2003/06/04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.7  2003/06/04 12:40:02  meichel
 *  Replaced protected inheritance from OFList with protected aggregation
 *
 *  Revision 1.6  2003/06/03 10:16:46  meichel
 *  Renamed local variables to avoid name clashes with STL
 *
 *  Revision 1.5  2001/10/10 15:30:03  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.4  2001/09/26 13:04:25  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.3  2001/06/01 15:51:10  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2000/11/06 11:34:25  joergr
 *  Added parameter to print() method specifying the item separator character.
 *
 *  Revision 1.1  2000/10/26 14:40:28  joergr
 *  Added support for TCOORD content item.
 *
 *
 *
 */
