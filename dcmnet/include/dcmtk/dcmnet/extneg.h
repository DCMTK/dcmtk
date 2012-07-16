/*
**
** Author: Andrew Hewett, medigration GmbH
**
** Module: extneg.h
**
** Purpose:
**   Extended Negotiation for A-ASSOCIATE
**
*/

#ifndef EXTNEG_H
#define EXTNEG_H

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/dcmnet/dndefine.h"

#define DUL_TYPESOPCLASSEXTENDEDNEGOTIATION (unsigned char)0x56


struct DCMTK_DCMNET_EXPORT SOPClassExtendedNegotiationSubItem {
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

DCMTK_DCMNET_EXPORT void appendList(const SOPClassExtendedNegotiationSubItemList& from, SOPClassExtendedNegotiationSubItemList& to);

DCMTK_DCMNET_EXPORT void deleteListMembers(SOPClassExtendedNegotiationSubItemList& lst);

#endif
