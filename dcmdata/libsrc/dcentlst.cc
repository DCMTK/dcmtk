/*
**
** Author: Andrew Hewett	Created: 18.3.96
** Kuratorium OFFIS e.V.
**
** Module: dcentlst.cc
**
** Purpose:
** A doubly linked list of DcmDictEntry's
** 
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-03-20 16:23:05 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/Attic/dcentlst.cc,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>

#include "dcentlst.h"


DcmDictEntryList::DcmDictEntryList()
{
    headNode = tailNode = NULL;
}

DcmDictEntryList::~DcmDictEntryList()
{
    Pix p;
    while ((p = head()) != NULL) {
	DcmDictEntry *e = remove(p);
	delete e;
    }
}

int
DcmDictEntryList::length() const
{
    int n = 0;
    for (Pix p = head(); p != NULL; next(p)) {
	n++;
    }
    return n;
}

void 
DcmDictEntryList::insertBefore(Pix p, DcmDictEntry *e)
{
    DcmDictEntryListNode *n = new DcmDictEntryListNode(e);
    DcmDictEntryListNode *pn = (DcmDictEntryListNode*)p;

    if (headNode == NULL && tailNode == NULL) {
	headNode = tailNode = n;
    } else if (pn == NULL || pn == headNode) {
	n->next = headNode;
        headNode->prev = n;
	headNode = n;
    } else {
	n->next = pn;
	n->prev = pn->prev;
	if (pn->prev != NULL) {
	    pn->prev->next = n;
	}
	pn->prev = n;
    }
}

void 
DcmDictEntryList::insertAfter(Pix p, DcmDictEntry *e)
{
    DcmDictEntryListNode *n = new DcmDictEntryListNode(e);
    DcmDictEntryListNode *pn = (DcmDictEntryListNode*)p;

    if (headNode == NULL && tailNode == NULL) {
	headNode = tailNode = n;
    } else if (pn == NULL || pn == tailNode) {
	n->prev = tailNode;
	tailNode->next = n;
	tailNode = n;
    } else {
	n->prev = pn;
	n->next = pn->next;
	if (pn->next != NULL) {
	    pn->next->prev = n;
	}
	pn->next = n;
    }
}

DcmDictEntry* 
DcmDictEntryList::remove(Pix p)
{
    DcmDictEntryListNode *pn = (DcmDictEntryListNode*)p;

    if (pn == NULL) return NULL;

    DcmDictEntry *e = (DcmDictEntry*)contents(p);

    if (pn->prev == NULL && pn->next == NULL) {
	/* last node */
	headNode = tailNode = NULL;
    } else if (pn->prev == NULL) {
	headNode = pn->next;
	if (headNode != NULL) headNode->prev = NULL;
    } else if (pn->next == NULL) {
	tailNode = pn->prev;
	if (tailNode != NULL) tailNode->next = NULL;
    } else {
	pn->prev->next = pn->next;
	pn->next->prev = pn->prev;
    }
    delete pn;

    return e;
}

void 
DcmDictEntryList::del(DcmDictEntry* e)
{
    Pix p = find(e);
    remove(p);
}

DcmDictEntry* 
DcmDictEntryList::replace(Pix p, DcmDictEntry* e)
{
    Pix pp = p;
    prev(pp);
    DcmDictEntry *old = remove(p);
    insertAfter(pp, e);
    return old;
}

Pix
DcmDictEntryList::find(const DcmDictEntry *e) const
{
    int found = FALSE;
    Pix p = NULL;
    for (p = head(); !found && p != NULL; next(p)) {
	found = (e == contents(p));
    }
    return p;
}

/*
** CVS/RCS Log:
** $Log: dcentlst.cc,v $
** Revision 1.1  1996-03-20 16:23:05  hewett
** A list of DcmDictEntry's.  Needed for revised handling of repeating tags.
**
**
*/
