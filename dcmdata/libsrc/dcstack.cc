/*
 * 
 * Author: Gerd Ehlers      Created:  03-20-94
 *                          Modified: 02-07-95
 *
 * Module: dcstack.cc
 * 
 * Purpose:
 * Implementation of the stack-class
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>

#include "dcstack.h"
#include "dcdebug.h"


// ************************************************
// *** DcmStackNode() *****************************
// ************************************************


DcmStackNode::DcmStackNode( DcmObject *obj )
{
Bdebug((6, "dcstack:DcmStackNode::DcmStackNode(DcmObject*)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    objNodeValue = obj;
    link = (DcmStackNode*)NULL;
Edebug(());

}


// ********************************


DcmStackNode::~DcmStackNode()
{
Bdebug((6, "dcstack:DcmStackNode::~DcmStackNode()" ));
debug(( 8, "Object pointer this=0x%p", this ));
Edebug(());

}


// ********************************


DcmObject* DcmStackNode::value()
{
    return objNodeValue;
}



// ************************************************
// *** DcmStack() *********************************
// ************************************************


DcmStack::DcmStack()
{
Bdebug((6, "dcstack:DcmStack::DcmStack()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    topNode = (DcmStackNode*)NULL;
    cardinality = 0;
Edebug(());

}


// ************************************************


DcmStack::DcmStack( const DcmStack & /*newStack*/ )
{
Bdebug((6, "dcstack:DcmStack::DcmStack(DcmStack&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    topNode = (DcmStackNode*)NULL;
    cardinality = 0;
    cerr << "Warning: DcmStack: use of Copy-Constructor not allowed"
         << endl;
Edebug(());

}


// ********************************


DcmStack::~DcmStack()
{
Bdebug((6, "dcstack:DcmStack::~DcmStack()" ));
debug(( 8, "Object pointer this=0x%p", this ));

    clear();
Edebug(());

}


// ********************************


void DcmStack::clear()
{
    DcmStackNode *node;
    while (topNode != (DcmStackNode*)NULL)
    {
        node = topNode;
        topNode = topNode->link;
        delete node;
    }
    cardinality = 0;
}


// ********************************


DcmObject* DcmStack::push( DcmObject *obj )
{
    if ( obj != (DcmObject*)NULL )
    {
        DcmStackNode *node = new DcmStackNode( obj );
        node->link = topNode;
        topNode = node;
        cardinality++;
    }
    return obj;
}


// ********************************


DcmObject* DcmStack::pop()
{
    DcmObject *obj;
    DcmStackNode *node;
    if ( topNode != (DcmStackNode*)NULL )
    {
        obj = topNode->value();
        node = topNode;
        topNode = topNode->link;
        delete node;
        cardinality--;
    }
    else
        obj = (DcmObject*)NULL;
    return obj;
}


// ********************************


DcmObject* DcmStack::elem( T_VR_UL number )
{
    T_VR_UL num = number;
    DcmObject *obj;
    DcmStackNode *node = topNode;
    while ( num-- > 0 && node != (DcmStackNode*)NULL )
         node = node->link;

    if ( node != (DcmStackNode*)NULL )
        obj = node->value();
    else
        obj = (DcmObject*)NULL;
    return obj;
}


// ********************************


DcmObject* DcmStack::top()
{
    DcmObject *obj;
    if ( topNode != (DcmStackNode*)NULL )
        obj = topNode->value();
    else
        obj = (DcmObject*)NULL;
    return obj;
}


// ********************************


T_VR_UL DcmStack::card()
{
    return cardinality;
}


// ********************************


BOOL DcmStack::empty()
{
    return (BOOL)( topNode == (DcmStackNode*)NULL );
}


// ********************************


