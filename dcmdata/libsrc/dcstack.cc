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
 * Last Update:	  $Author: meichel $
 * Revision:      $Revision: 1.7 $
 * Status:        $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>
#include <stdlib.h>

#include "dcstack.h"
#include "dcdebug.h"


// ************************************************
// *** DcmStackNode() *****************************
// ************************************************


DcmStackNode::DcmStackNode( DcmObject *obj )
{
    objNodeValue = obj;
    link = (DcmStackNode*)NULL;
}


// ********************************


DcmStackNode::~DcmStackNode()
{
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
    topNode = (DcmStackNode*)NULL;
    cardinality = 0;
}


// ************************************************


DcmStack::DcmStack( const DcmStack & oldStack)
{
    topNode = (DcmStackNode*)NULL;
    cardinality = oldStack.cardinality;
    if (cardinality)
    {
	topNode = new DcmStackNode(oldStack.topNode->objNodeValue);
	DcmStackNode * oldPtr = oldStack.topNode->link;
	DcmStackNode * newPtr = topNode;
	while (oldPtr)
	{
	    newPtr->link = new DcmStackNode(oldPtr->objNodeValue);
	    oldPtr = oldPtr->link;
	    newPtr = newPtr->link;
	}
    }
}


// ********************************


DcmStack::~DcmStack()
{
    clear();
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


DcmObject* DcmStack::elem(const unsigned long number)
{
    unsigned long num = number;
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


unsigned long DcmStack::card()
{
    return cardinality;
}


// ********************************


OFBool DcmStack::empty()
{
    return (OFBool)( topNode == (DcmStackNode*)NULL );
}


// ********************************


