/*
 * 
 * Author: Gerd Ehlers      Created:  04-09-94
 *                          Modified: 02-07-95
 *
 * Module: dclist.h
 *
 * Purpose:
 * This file contains a List-Definition
 * 
 * 
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */

#ifndef DCLIST_H
#define DCLIST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"


class DcmObject;    // forward declaration


class DcmListNode {
    friend class DcmList;
    DcmListNode *nextNode;
    DcmListNode *prevNode;
    DcmObject *objNodeValue;

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
    T_VR_UL cardinality;

public:
    DcmList();
    DcmList( const DcmList &newList );
    ~DcmList();

    DcmObject *append(  DcmObject *obj );
    DcmObject *prepend( DcmObject *obj );
    DcmObject *insert(  DcmObject *obj,
                        E_ListPos pos = ELP_next );
    DcmObject *remove();
    DcmObject *get(     E_ListPos pos = ELP_atpos );
    DcmObject *seek(    E_ListPos pos = ELP_next );
    DcmObject *seek_to( T_VR_UL absolute_position );
    T_VR_UL card();
    BOOL empty();
    BOOL valid();
};


#endif  // DCLIST_H

