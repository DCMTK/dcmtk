/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: Implementation of supplementary methods for a template list class
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-21 13:46:48 $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#if defined(HAVE_STL) || defined (HAVE_STL_LIST)
// We do not need to make this library
void OF__DUMMY()    // to make the linker happy!
{
}
#else

#include "dcmtk/ofstd/oflist.h"

OFListBase::OFListBase()
: afterLast(NULL)
, listSize(0)
{
    afterLast = new OFListLinkBase();
    afterLast->prev = afterLast->next = afterLast;
    afterLast->dummy = OFTrue;
}


OFListBase::~OFListBase()
{
    base_clear();
    if (afterLast)
        delete afterLast;
}

OFListLinkBase * OFListBase::base_insert(OFListLinkBase * pos,
                                         OFListLinkBase * newElem)
{
    assert(pos && newElem);
    if (pos && newElem)
    {
        newElem->next = pos;
        newElem->prev = pos->prev;
        pos->prev->next = newElem;
        pos->prev = newElem;
        listSize++;
        return newElem;
    }
    return NULL;
}


OFListLinkBase * OFListBase::base_erase(OFListLinkBase * pos)
{
    assert(pos && pos != afterLast);
    OFListLinkBase * tmp = pos->next;;
    pos->next->prev=pos->prev;
    pos->prev->next=pos->next;
    delete pos;
    listSize--;
    return tmp;
}

void OFListBase::base_splice(OFListLinkBase * pos,
                             OFListLinkBase * begin,
                             OFListLinkBase * end)
{
    assert(pos && begin && end);
    if (begin != end)
    {
        OFListLinkBase * beginPrev = begin->prev;
        OFListLinkBase * posPrev = pos->prev;
        pos->prev->next = begin;
        pos->prev = end->prev;
        begin->prev->next = end;
        begin->prev = posPrev;
        end->prev->next = pos;
        end->prev = beginPrev;
        base_recalcListSize();
    }
}

void OFListBase::base_clear()
{
    while(listSize)
        base_erase(afterLast->next);
}

void OFListBase::base_recalcListSize()
{
    OFListLinkBase * elem;
    for (listSize = 0, elem = afterLast->next;
         elem != afterLast;
         elem = elem->next, ++listSize)
        ;
}


#endif


/*
** CVS/RCS Log:
** $Log: oflist.cc,v $
** Revision 1.10  2010-10-21 13:46:48  joergr
** Replaced tab characters by spaces
**
** Revision 1.9  2010-10-14 13:14:53  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.8  2005/12/08 15:48:58  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.7  2001/06/01 15:51:39  meichel
** Updated copyright header
**
** Revision 1.6  2000/03/08 16:36:06  meichel
** Updated copyright header.
**
** Revision 1.5  1998/11/27 12:43:53  joergr
** Added copyright message to source files and changed CVS header.
**
** Revision 1.4  1998/07/02 07:47:05  meichel
** Some code purifications to avoid gcc 2.8.1 -Weffc++ warnings.
**
** Revision 1.3  1998/02/06 15:07:41  meichel
** Removed many minor problems (name clashes, unreached code)
**   reported by Sun CC4 with "+w" or Sun CC2.
**
** Revision 1.2  1997/07/07 07:34:23  andreas
** - Corrected destructor for OFListBase, now the dummy element is
**   deleted.
**
** Revision 1.1  1997/07/02 11:52:20  andreas
** - Preliminary release of the OFFIS Standard Library.
**   In the future this library shall contain a subset of the
**   ANSI C++ Library (Version 3) that works on a lot of different
**   compilers. Additionally this library shall include classes and
**   functions that are often used. All classes and functions begin
**   with OF... This library is independent of the DICOM development and
**   shall contain no DICOM specific stuff.
**
**
*/

