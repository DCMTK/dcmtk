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
 *    classes: DSRImageFrameList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-04 14:26:54 $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dsrimgfr.h"

#define INCLUDE_CSTDIO
#include "ofstdinc.h"

/* declared in class DSRListOfItems<T> */
//const Sint32 DSRListOfItems<Sint32>::EmptyItem;


DSRImageFrameList::DSRImageFrameList()
  : DSRListOfItems<Sint32>()
{
}


DSRImageFrameList::DSRImageFrameList(const DSRImageFrameList &lst)
  : DSRListOfItems<Sint32>(lst)
{
}


DSRImageFrameList::~DSRImageFrameList()
{
}


DSRImageFrameList &DSRImageFrameList::operator=(const DSRImageFrameList &lst)
{
    DSRListOfItems<Sint32>::operator=(lst);
    return *this;
}


OFCondition DSRImageFrameList::print(ostream &stream,
                                     const size_t flags,
                                     const char separator) const
{
    const OFListIterator(Sint32) endPos = list_.end();
    OFListIterator(Sint32) iterator = list_.begin();
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


OFCondition DSRImageFrameList::read(DcmItem &dataset,
                                    OFConsole * /* logStream */)
{
    /* get integer string from dataset */
    DcmIntegerString delem(DCM_ReferencedFrameNumber);
    OFCondition result = DSRTypes::getElementFromDataset(dataset, delem);
    if (result.good())
    {
        /* clear internal list */
        clear();
        Sint32 value = 0;
        const unsigned long count = delem.getVM();
        /* fill list with values from integer string */
        for (unsigned long i = 0; i < count; i++)
        {
            if (delem.getSint32(value, i).good())
                addItem(value);
        }
    }
    return result;
}


OFCondition DSRImageFrameList::write(DcmItem &dataset,
                                     OFConsole * /* logStream */) const
{
    OFCondition result = EC_Normal;
    /* fill string with values from list */
    OFString string;
    char buffer[16];
    const OFListIterator(Sint32) endPos = list_.end();
    OFListIterator(Sint32) iterator = list_.begin();
    while (iterator != endPos)
    {
        if (string.length() > 0)
            string += '\\';
#if SIZEOF_LONG == 8
    	sprintf(buffer, "%d", *iterator);
#else
    	sprintf(buffer, "%ld", *iterator);
#endif
        string += buffer;
        iterator++;
    }
    /* set integer string */
    DcmIntegerString delem(DCM_ReferencedFrameNumber);
    result = delem.putOFStringArray(string);
    /* add to dataset */
    if (result.good())
        result = DSRTypes::addElementToDataset(result, dataset, new DcmIntegerString(delem));
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrimgfr.cc,v $
 *  Revision 1.11  2003-06-04 14:26:54  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.10  2003/06/04 12:40:02  meichel
 *  Replaced protected inheritance from OFList with protected aggregation
 *
 *  Revision 1.9  2003/06/03 10:16:46  meichel
 *  Renamed local variables to avoid name clashes with STL
 *
 *  Revision 1.8  2002/11/27 14:36:18  meichel
 *  Adapted module dcmsr to use of new header file ofstdinc.h
 *
 *  Revision 1.7  2002/08/20 12:53:56  meichel
 *  Added explicit includes for header files included implicitly
 *    via dcstream before.
 *
 *  Revision 1.6  2001/10/10 15:29:55  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.5  2001/09/26 13:04:21  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:51:08  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/11/06 11:32:04  joergr
 *  Added parameter to print() method specifying the item separator character.
 *
 *  Revision 1.2  2000/10/18 17:18:24  joergr
 *  Added comments.
 *
 *  Revision 1.1  2000/10/13 07:52:20  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
