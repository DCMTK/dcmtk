/*
**
** Author: Andrew Hewett, medigration GmbH
**
** Module: extneg.cc
**
** Purpose:
**   Extended Negotiation for A-ASSOCIATE
**
** Last Update:         $Author: meichel $
** Update Date:         $Date: 2003-06-02 16:44:11 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/extneg.cc,v $
** CVS/RCS Revision:    $Revision: 1.2 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h" /* make sure OS specific configuration is included first */
#include "extneg.h"


void appendList(const SOPClassExtendedNegotiationSubItemList& from, SOPClassExtendedNegotiationSubItemList& to)
{
    OFListIterator(SOPClassExtendedNegotiationSubItem*) i = from.begin();
    while (i != from.end()) {
        to.push_back(*i);
        ++i;
    }
}

void deleteListMembers(SOPClassExtendedNegotiationSubItemList& lst)
{
    OFListIterator(SOPClassExtendedNegotiationSubItem*) i = lst.begin();
    while (i != lst.end()) {
        delete *i;
        ++i;
    }
    lst.clear();
}


/*
** CVS/RCS Log:
** $Log: extneg.cc,v $
** Revision 1.2  2003-06-02 16:44:11  meichel
** Renamed local variables to avoid name clashes with STL
**
** Revision 1.1  1999/04/19 08:40:03  meichel
** Added experimental support for extended SOP class negotiation.
**
**
*/
