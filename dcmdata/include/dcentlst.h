/*
**
** Author: Andrew Hewett	Created: 18.03.95
** Kuratorium OFFIS e.V.
**
** Module: dcentlst.h
**
** Purpose:
** Interface to doubly linked list of DcmDictEntry's
** 
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-03-20 16:23:43 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcentlst.h,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef DCENTLST_H
#define DCENTLST_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcdicent.h"
#include "pix.h"


class DcmDictEntryListNode {
public:
    DcmDictEntry *entry;
    DcmDictEntryListNode *prev;
    DcmDictEntryListNode *next;

    DcmDictEntryListNode(DcmDictEntry *e=NULL, 
			 DcmDictEntryListNode *p=NULL,
			 DcmDictEntryListNode *n=NULL)
	{ entry = e; prev = p; next = n; }
};


class DcmDictEntryList {
private:
    DcmDictEntryListNode *headNode;
    DcmDictEntryListNode *tailNode;

public:
    DcmDictEntryList();
    ~DcmDictEntryList();

    int length() const;

    void insertBefore(Pix p, DcmDictEntry *e);
    void insertAfter(Pix p, DcmDictEntry *e);

    DcmDictEntry* remove(Pix p);
    void del(DcmDictEntry* e);
    DcmDictEntry* replace(Pix p, DcmDictEntry* e);

    Pix head() const;
    Pix first() const;
    Pix tail() const;
    Pix last() const;
    void next(Pix& i) const;
    void prev(Pix& i) const;
    const DcmDictEntry* contents(Pix i) const;

    Pix find(const DcmDictEntry *e) const;
};


inline Pix 
DcmDictEntryList::head() const
{
    return (Pix)headNode;
}

inline Pix 
DcmDictEntryList::first() const
{
    return head();
}

inline Pix 
DcmDictEntryList::tail() const
{
    return (Pix)tailNode;
}

inline Pix 
DcmDictEntryList::last() const
{
    return last();
}

inline void 
DcmDictEntryList::next(Pix& i) const
{
    i = (Pix)((DcmDictEntryListNode*)i)->next;
}

inline void 
DcmDictEntryList::prev(Pix& i) const
{
    i = (Pix)((DcmDictEntryListNode*)i)->prev;
}

inline const DcmDictEntry* 
DcmDictEntryList::contents(Pix i) const
{
    return ((DcmDictEntryListNode*)i)->entry;
}

#endif /* ! DCENTLST_H */

/*
** CVS/RCS Log:
** $Log: dcentlst.h,v $
** Revision 1.1  1996-03-20 16:23:43  hewett
** A list of DcmDictEntry's. Needed for revised handling of repeating tags.
**
**
*/
