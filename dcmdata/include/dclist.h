/*
 * 
 * Author: Gerd Ehlers      Created:  09.04.94
 *                          Modified: 02.12.95
 *
 * Module: dclist.h
 *
 * Purpose:
 * This file contains a List-Definition
 * 
 * 
 * Last Update:   $Author: meichel $
 * Revision:      $Revision: 1.7 $
 * Status:        $State: Exp $
 *
 */

#ifndef DCLIST_H
#define DCLIST_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stddef.h>
#include <stdlib.h>
#include "dcerror.h"
#include "dctypes.h"


const unsigned long DCM_EndOfListIndex = (unsigned long)(-1L);


class DcmObject;    // forward declaration


class DcmListNode {
    friend class DcmList;
    DcmListNode *nextNode;
    DcmListNode *prevNode;
    DcmObject *objNodeValue;

 // --- declarations to avoid compiler warnings
 
    DcmListNode(const DcmListNode &);
    DcmListNode &operator=(const DcmListNode &);

public:
    DcmListNode( DcmObject *obj );
    ~DcmListNode();
    DcmObject *value();
};


typedef enum
{
    ELP_atpos,
    ELP_first,
    ELP_last,
    ELP_prev,
    ELP_next
} E_ListPos;


/*  class DcmList:
 *  Es werden nur Zeiger auf Elemente eingefuegt (unter der Voraussetzung
 *  das Element ist kein NULL-Zeiger).
 *  Ein Aufruf von DcmList::remove() loescht die Objekte nicht,
 *  aber beim Destruktoraufruf werden alle Objekte, die sich in der Liste
 *  befinden, geloescht.
 */

class DcmList {
    DcmListNode *firstNode;
    DcmListNode *lastNode;
    DcmListNode *actualNode;
    unsigned long cardinality;

 // --- declarations to avoid compiler warnings
 
    DcmList &operator=(const DcmList &);
    DcmList(const DcmList &newList);

public:
    DcmList();
    ~DcmList();

    DcmObject *append(  DcmObject *obj );
    DcmObject *prepend( DcmObject *obj );
    DcmObject *insert(  DcmObject *obj,
                        E_ListPos pos = ELP_next );
    DcmObject *remove();
    DcmObject *get(     E_ListPos pos = ELP_atpos );
    DcmObject *seek(    E_ListPos pos = ELP_next );
    DcmObject *seek_to(unsigned long absolute_position);
    inline unsigned long card() const { return cardinality; }
    inline OFBool empty(void) const { return firstNode == NULL; }
    inline OFBool valid(void) const { return actualNode != NULL; }
};


#endif  // DCLIST_H

