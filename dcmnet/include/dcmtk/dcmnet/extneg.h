/*
**
** Author: Andrew Hewett, medigration GmbH
**
** Module: extneg.h
**
** Purpose:
**   Extended Negotiation for A-ASSOCIATE
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 2010-10-14 13:17:22 $
** CVS/RCS Revision:    $Revision: 1.4 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef EXTNEG_H
#define EXTNEG_H

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/oflist.h"

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

void deleteListMembers(SOPClassExtendedNegotiationSubItemList& lst);

#endif

/*
** CVS/RCS Log:
** $Log: extneg.h,v $
** Revision 1.4  2010-10-14 13:17:22  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.3  2005/12/08 16:02:24  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.2  2003/06/02 16:44:11  meichel
** Renamed local variables to avoid name clashes with STL
**
** Revision 1.1  1999/04/19 08:39:55  meichel
** Added experimental support for extended SOP class negotiation.
**
**
*/
