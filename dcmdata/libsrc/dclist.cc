/*
 * 
 * Author: Gerd Ehlers      Created:  04-09-94
 *                          Modified: 02-07-95
 *
 * Module: dclist.cc
 * 
 * Purpose:
 * Implementation of the list-class
 * 
 * 
 * Last Update:   $Author: joergr $
 * Revision:      $Revision: 1.6 $
 * Status:        $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>

#include "dclist.h"
#include "dcdebug.h"


// *****************************************
// *** DcmListNode *************************
// *****************************************


DcmListNode::DcmListNode( DcmObject *obj )
  : nextNode(NULL),
    prevNode(NULL),
    objNodeValue(obj)
{
}


// ********************************


DcmListNode::~DcmListNode()
{
}


// ********************************


DcmObject *DcmListNode::value()
{
    return objNodeValue;
}


// *****************************************
// *** DcmList *****************************
// *****************************************


DcmList::DcmList()
  : firstNode(NULL),
    lastNode(NULL),
    actualNode(NULL),
    cardinality(0)
{
}


// *****************************************


DcmList::DcmList( const DcmList & /*newList*/ )
  : firstNode(NULL),
    lastNode(NULL),
    actualNode(NULL),
    cardinality(0)
{
    cerr << "Warning: DcmList: use of Copy-Constructor not allowed"
         << endl;
    abort();
}


// ********************************


DcmList::~DcmList()
{
    if ( !DcmList::empty() )                      // Liste ist nicht leer !
    {
        lastNode->nextNode = (DcmListNode*)NULL;  // setze zur Sicherheit auf 0
        do {
            DcmListNode *temp = firstNode;
            firstNode = firstNode->nextNode;
            // delete temp->objNodeValue;;        // gefaehrlich!
            delete temp;
        } while ( firstNode != (DcmListNode*)NULL );
        actualNode = firstNode = lastNode = (DcmListNode*)NULL;
    }
}


// ********************************


DcmObject *DcmList::append( DcmObject *obj )
{
    if ( obj != (DcmObject*)NULL )
    {
        if ( DcmList::empty() )                        // Liste ist leer !
            actualNode = firstNode = lastNode = new DcmListNode(obj);
        else
        {
            DcmListNode *node = new DcmListNode(obj);
            lastNode->nextNode = node;
            node->prevNode = lastNode;
            actualNode = lastNode = node;
        }
        cardinality++;
    } // obj == NULL
    return obj;
}


// ********************************


DcmObject *DcmList::prepend( DcmObject *obj )
{
    if ( obj != (DcmObject*)NULL )
    {
        if ( DcmList::empty() )                        // Liste ist leer !
            actualNode = firstNode = lastNode = new DcmListNode(obj);
        else
        {
            DcmListNode *node = new DcmListNode(obj);
            node->nextNode = firstNode;
            firstNode->prevNode = node;
            actualNode = firstNode = node;
        }
        cardinality++;
    } // obj == NULL
    return obj;
}


// ********************************


DcmObject *DcmList::insert( DcmObject *obj, E_ListPos pos )
{
    if ( obj != (DcmObject*)NULL )
    {
        if ( DcmList::empty() )                 // Liste ist leer !
        {
            actualNode = firstNode = lastNode = new DcmListNode(obj);
            cardinality++;
        }
        else {
            if ( pos==ELP_last )
                DcmList::append( obj );         // cardinality++;
            else if ( pos==ELP_first )
                DcmList::prepend( obj );        // cardinality++;
            else if ( !DcmList::valid() )
                // setze akt. Zeiger ans Ende wenn keine Vorganger bzw.
                // Nachfolger zu bestimmen sind
                DcmList::append( obj );         // cardinality++;
            else if ( pos==ELP_prev )           // vor akt. Zeiger einfuegen
            {
                DcmListNode *node = new DcmListNode(obj);
                if ( actualNode->prevNode == (DcmListNode*)NULL )
                    firstNode = node;           // am Anfang anfuegen
                else
                    actualNode->prevNode->nextNode = node;
                node->prevNode = actualNode->prevNode;
                node->nextNode = actualNode;
                actualNode->prevNode = node;
                actualNode = node;
                cardinality++;
            }
            else //( pos==ELP_next || pos==ELP_atpos )
                                                // nach akt. Zeiger einfuegen
            {
                DcmListNode *node = new DcmListNode(obj);
                if ( actualNode->nextNode == (DcmListNode*)NULL )
                    lastNode = node;            // am Ende anfuegen
                else
                    actualNode->nextNode->prevNode = node;
                node->nextNode = actualNode->nextNode;
                node->prevNode = actualNode;
                actualNode->nextNode = node;
                actualNode = node;
                cardinality++;
            }
        }
    } // obj == NULL
    return obj;
}


// ********************************


DcmObject *DcmList::remove()
{
    DcmObject *tempobj;
    DcmListNode *tempnode;

    if ( DcmList::empty() )                        // Liste ist leer !
        return (DcmObject*)NULL;
    else if ( !DcmList::valid() )
        return (DcmObject*)NULL;                   // akt. Zeiger zeigt auf 0
    else
    {
        tempnode = actualNode;

        if ( actualNode->prevNode == (DcmListNode*)NULL )
            firstNode = actualNode->nextNode;       // erstes Element loeschen
        else
            actualNode->prevNode->nextNode = actualNode->nextNode;

        if ( actualNode->nextNode == (DcmListNode*)NULL )
            lastNode = actualNode->prevNode;        // letztes Element loeschen
        else
            actualNode->nextNode->prevNode = actualNode->prevNode;

        actualNode = actualNode->nextNode;
        tempobj = tempnode->value();
        delete tempnode;
        cardinality--;
        return tempobj;
    }
}


// ********************************


DcmObject *DcmList::get( E_ListPos pos )
{
    return seek( pos );
}


// ********************************


DcmObject *DcmList::seek( E_ListPos pos )
{
    switch (pos)
    {
        case ELP_first :
            actualNode = firstNode;
            break;
        case ELP_last :
            actualNode = lastNode;
            break;
        case ELP_prev :
            if ( DcmList::valid() )
                actualNode = actualNode->prevNode;
            break;
        case ELP_next :
            if ( DcmList::valid() )
                actualNode = actualNode->nextNode;
            break;
        default:
            break;
    }
    return DcmList::valid() ? actualNode->value() : (DcmObject*)NULL;
}


// ********************************


DcmObject *DcmList::seek_to(unsigned long absolute_position)
{
    const unsigned long tmppos = absolute_position < cardinality
                        ? absolute_position
                        : cardinality;
    seek( ELP_first );
    for (unsigned long i = 0; i < tmppos; i++)
        seek( ELP_next );
    return get( ELP_atpos );
}


// ********************************


