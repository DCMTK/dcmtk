/*
**
** Author: Andrew Hewett, medigration GmbH
**
** Module: extneg.h
**
** Purpose:
**   Extended Negotiation for A-ASSOCIATE
**
** Last Update:         $Author: meichel $
** Update Date:         $Date: 1999-04-19 08:39:55 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/Attic/extneg.h,v $
** CVS/RCS Revision:    $Revision: 1.1 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef EXTNEG_H
#define EXTNEG_H

#include "osconfig.h" /* make sure OS specific configuration is included first */

#include "ofstring.h"
#include "oflist.h"

#define DUL_TYPESOPCLASSEXTENDEDNEGOTIATION (unsigned char)0x56


struct SOPClassExtendedNegotiationSubItem {
    unsigned char itemType;
    unsigned char reserved1;
    unsigned short itemLength;
    unsigned short sopClassUIDLength;
    OFString sopClassUID;
    unsigned char* serviceClassAppInfo;
    unsigned short serviceClassAppInfoLength;

    SOPClassExtendedNegotiationSubItem()
    : itemType(0x56)
    , reserved1(0)
    , itemLength(0)
    , sopClassUIDLength(0)
    , sopClassUID()
    , serviceClassAppInfo(NULL)
    , serviceClassAppInfoLength(0)
    {
    }

private:
    // private undefined copy constructor and operator=
    SOPClassExtendedNegotiationSubItem(const SOPClassExtendedNegotiationSubItem& arg);
    SOPClassExtendedNegotiationSubItem& operator= (const SOPClassExtendedNegotiationSubItem& arg);
};

typedef OFList<SOPClassExtendedNegotiationSubItem*> SOPClassExtendedNegotiationSubItemList;

void appendList(const SOPClassExtendedNegotiationSubItemList& from, SOPClassExtendedNegotiationSubItemList& to);

void deleteListMembers(SOPClassExtendedNegotiationSubItemList& list);

#endif

/*
** CVS/RCS Log:
** $Log: extneg.h,v $
** Revision 1.1  1999-04-19 08:39:55  meichel
** Added experimental support for extended SOP class negotiation.
**
**
*/
