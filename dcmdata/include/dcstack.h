/*
 * 
 * Author: Gerd Ehlers      Created:  03-20-94
 *                          Modified: 02-07-95
 *
 * Module: dcstack.h
 * 
 * Purpose:
 * This file contains a Stack-Definition
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */

#ifndef DCSTACK_H
#define DCSTACK_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"

class DcmObject;    // forward declaration


class DcmStackNode
{
    friend class DcmStack;
    DcmStackNode *link;
    DcmObject *objNodeValue;

public:
    DcmStackNode( DcmObject *obj );
    ~DcmStackNode();
    DcmObject *value();
};


/*  class DcmStack:
 *  Es werden nur Zeiger auf Elemente eingefuegt (unter der Voraussetzung
 *  das Element ist kein NULL-Zeiger).
 *  Ein Aufruf von DcmList::clear() loescht nur den Stack, die verwalteten
 *  Objekte jedoch nicht.
 *  Beim Destruktoraufruf werden die verwalteten Objekte nicht geloescht.
 */

class DcmStack {
    DcmStackNode *topNode;
    T_VR_UL cardinality;

public:
    DcmStack();
    DcmStack( const DcmStack &newStack );
    ~DcmStack();

    DcmObject* push( DcmObject *obj );
    DcmObject* pop();
    DcmObject* top();
    DcmObject* elem( T_VR_UL number );
    BOOL empty();
    T_VR_UL card();
    void clear();
};


#endif  // DCSTACK_H

