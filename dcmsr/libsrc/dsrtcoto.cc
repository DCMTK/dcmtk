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
 *    classes: DSRReferencedTimeOffsetList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:42 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmsr/dsrtcoto.h"
#include "dcmtk/ofstd/ofstd.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"


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


OFCondition DSRReferencedTimeOffsetList::print(STD_NAMESPACE ostream &stream,
                                               const size_t flags,
                                               const char separator) const
{
    char buffer[64];
    const OFListConstIterator(Float64) endPos = ItemList.end();
    OFListConstIterator(Float64) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        /* need to convert float to avoid problems with decimal point ('.' or ',') */
        OFStandard::ftoa(buffer, sizeof(buffer), *iterator);
        stream << buffer;
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


OFCondition DSRReferencedTimeOffsetList::read(DcmItem &dataset)
{
    /* get decimal string from dataset */
    DcmDecimalString delem(DCM_ReferencedTimeOffsets);
    OFCondition result = DSRTypes::getAndCheckElementFromDataset(dataset, delem, "1-n", "1C", "TCOORD content item");
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


OFCondition DSRReferencedTimeOffsetList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* fill string with values from list */
    OFString tmpString;
    char buffer[32];
    const OFListConstIterator(Float64) endPos = ItemList.end();
    OFListConstIterator(Float64) iterator = ItemList.begin();
    while (iterator != endPos)
    {
        if (!tmpString.empty())
            tmpString += '\\';
        OFStandard::ftoa(buffer, sizeof(buffer), *iterator, OFStandard::ftoa_format_f);
        tmpString += buffer;
        iterator++;
    }
    /* set decimal string */
    DcmDecimalString delem(DCM_ReferencedTimeOffsets);
    result = delem.putOFStringArray(tmpString);
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmDecimalString(delem), "1-n", "1", "TCOORD content item");
    return result;
}


OFCondition DSRReferencedTimeOffsetList::putString(const char *stringValue)
{
    OFCondition result = EC_Normal;
    /* clear internal list */
    clear();
    /* check input string */
    if ((stringValue != NULL) && (strlen(stringValue) > 0))
    {
        Float64 value = 0;
        OFBool success = OFFalse;
        const char *ptr = stringValue;
        /* retrieve time offsets from string */
        while (result.good() && (ptr != NULL))
        {
            value = OFStandard::atof(ptr, &success);
            if (success)
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
 *  $Log: dsrtcoto.cc,v $
 *  Revision 1.19  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.18  2010-09-29 15:16:51  joergr
 *  Enhanced checking and reporting of standard violations in write() methods.
 *
 *  Revision 1.17  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.16  2007-11-15 16:43:43  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.15  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.14  2005/12/08 15:48:13  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.13  2003/08/07 14:07:04  joergr
 *  Added new putString() method.
 *  Renamed parameters/variables "string" to avoid name clash with STL class.
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
