/*
 *
 *  Copyright (C) 2000-2002, OFFIS
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-04 12:40:02 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dsrtcoto.h"
#include "ofstd.h"

#define INCLUDE_CSTDIO
#include "ofstdinc.h"

/* declared in class DSRListOfItems<T> */
//const Float64 DSRListOfItems<Float64>::EmptyItem;


DSRReferencedTimeOffsetList::DSRReferencedTimeOffsetList()
  : DSRListOfItems<Float64>()
{
}


DSRReferencedTimeOffsetList::DSRReferencedTimeOffsetList(const DSRReferencedTimeOffsetList &lst)
  : DSRListOfItems<Float64>(lst)
{
}


DSRReferencedTimeOffsetList::~DSRReferencedTimeOffsetList()
{
}


DSRReferencedTimeOffsetList &DSRReferencedTimeOffsetList::operator=(const DSRReferencedTimeOffsetList &lst)
{
    DSRListOfItems<Float64>::operator=(lst);
    return *this;
}


OFCondition DSRReferencedTimeOffsetList::print(ostream &stream,
                                               const size_t flags,
                                               const char separator) const
{
    const OFListIterator(Float64) endPos = list_.end();
    OFListIterator(Float64) iterator = list_.begin();
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


OFCondition DSRReferencedTimeOffsetList::read(DcmItem &dataset,
                                              OFConsole *logStream)
{
    /* get decimal string from dataset */
    DcmDecimalString delem(DCM_ReferencedTimeOffsets);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", logStream, "TCOORD content item");
    if (result.good())
    {
        /* clear internal list */
        clear();
        Float64 value = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from decimal string */
        for (unsigned long i = 0; i < count; i++)
        {
            if (delem.getFloat64(value, i).good())
                addItem(value);
        }
    }
    return result;
}


OFCondition DSRReferencedTimeOffsetList::write(DcmItem &dataset,
                                               OFConsole * /* logStream */) const
{
    OFCondition result = EC_Normal;
    /* fill string with values from list */
    OFString string;
    char buffer[32];
    const OFListIterator(Float64) endPos = list_.end();
    OFListIterator(Float64) iterator = list_.begin();
    while (iterator != endPos)
    {
        if (string.length() > 0)
            string += '\\';

        OFStandard::ftoa(buffer, sizeof(buffer), *iterator, OFStandard::ftoa_format_f);
        string += buffer;
        iterator++;
    }
    /* set decimal string */
    DcmDecimalString delem(DCM_ReferencedTimeOffsets);
    result = delem.putOFStringArray(string);
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmDecimalString(delem));
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcoto.cc,v $
 *  Revision 1.10  2003-06-04 12:40:02  meichel
 *  Replaced protected inheritance from OFList with protected aggregation
 *
 *  Revision 1.9  2003/06/03 10:16:46  meichel
 *  Renamed local variables to avoid name clashes with STL
 *
 *  Revision 1.8  2002/12/04 10:42:16  meichel
 *  Changed toolkit to use OFStandard::ftoa instead of sprintf for all
 *    double to string conversions that are supposed to be locale independent
 *
 *  Revision 1.7  2002/11/27 14:36:18  meichel
 *  Adapted module dcmsr to use of new header file ofstdinc.h
 *
 *  Revision 1.6  2002/08/20 12:53:56  meichel
 *  Added explicit includes for header files included implicitly
 *    via dcstream before.
 *
 *  Revision 1.5  2001/10/10 15:30:04  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.4  2001/09/26 13:04:26  meichel
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
 */
