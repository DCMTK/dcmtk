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
 * Last Update:	  $Author: andreas $
 * Revision:      $Revision: 1.4 $
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
    unsigned long cardinality;

public:
    DcmStack();
    DcmStack( const DcmStack &newStack );
    ~DcmStack();

    DcmObject* push( DcmObject *obj );
    DcmObject* pop();
    DcmObject* top();
    DcmObject* elem(const unsigned long number);
    OFBool empty();
    const unsigned long card();
    void clear();
};


#endif  // DCSTACK_H

