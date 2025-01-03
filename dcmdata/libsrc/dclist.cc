/*
 *
 *  Copyright (C) 1994-2019, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: generic list class
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dclist.h"

#include <limits>


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


// *****************************************
// *** DcmList *****************************
// *****************************************


namespace
{
    static const unsigned long invalidAbsolutePosition = std::numeric_limits<unsigned long>::max();
}


// ********************************


DcmList::DcmList()
  : firstNode(NULL),
    lastNode(NULL),
    currentNode(NULL),
    currentAbsolutePosition(invalidAbsolutePosition),
    cardinality(0)
{
}


// ********************************


DcmList::~DcmList()
{
    if ( !DcmList::empty() )                      // list is not empty !
    {
        lastNode->nextNode = NULL;                // set to 0 for safety reasons
        do {
            DcmListNode *temp = firstNode;
            firstNode = firstNode->nextNode;
            // delete temp->objNodeValue;         // dangerous!
            delete temp;
        } while ( firstNode != NULL );
        currentNode = firstNode = lastNode = NULL;
        currentAbsolutePosition = invalidAbsolutePosition;
    }
}


// ********************************


DcmObject *DcmList::append( DcmObject *obj )
{
    if ( obj != NULL )
    {
        if ( DcmList::empty() )                        // list is empty !
            currentNode = firstNode = lastNode = new DcmListNode(obj);
        else
        {
            DcmListNode *node = new DcmListNode(obj);
            lastNode->nextNode = node;
            node->prevNode = lastNode;
            currentNode = lastNode = node;
        }
        currentAbsolutePosition = cardinality;
        cardinality++;
    } // obj == NULL
    return obj;
}


// ********************************


DcmObject *DcmList::prepend( DcmObject *obj )
{
    if ( obj != NULL )
    {
        if ( DcmList::empty() )                        // list is empty !
            currentNode = firstNode = lastNode = new DcmListNode(obj);
        else
        {
            DcmListNode *node = new DcmListNode(obj);
            node->nextNode = firstNode;
            firstNode->prevNode = node;
            currentNode = firstNode = node;
        }
        currentAbsolutePosition = 0;
        cardinality++;
    } // obj == NULL
    return obj;
}


// ********************************


DcmObject *DcmList::insert( DcmObject *obj, E_ListPos pos )
{
    if ( obj != NULL )
    {
        if ( DcmList::empty() )                 // list is empty !
        {
            currentNode = firstNode = lastNode = new DcmListNode(obj);
            currentAbsolutePosition = 0;
            cardinality++;
        }
        else {
            if ( pos==ELP_last )
                DcmList::append( obj );         // cardinality++;
            else if ( pos==ELP_first )
                DcmList::prepend( obj );        // cardinality++;
            else if ( !DcmList::valid() )
                // set current node to the end if there is no predecessor or
                // there are successors to be determined
                DcmList::append( obj );         // cardinality++;
            else if ( pos == ELP_prev )         // insert before current node
            {
                DcmListNode *node = new DcmListNode(obj);
                if ( currentNode->prevNode == NULL )
                    firstNode = node;           // insert at the beginning
                else
                    currentNode->prevNode->nextNode = node;
                node->prevNode = currentNode->prevNode;
                node->nextNode = currentNode;
                currentNode->prevNode = node;
                currentNode = node;
                currentAbsolutePosition++;
                cardinality++;
            }
            else //( pos==ELP_next || pos==ELP_atpos )
                                                // insert after current node
            {
                DcmListNode *node = new DcmListNode(obj);
                if ( currentNode->nextNode == NULL )
                    lastNode = node;            // append to the end
                else
                    currentNode->nextNode->prevNode = node;
                node->nextNode = currentNode->nextNode;
                node->prevNode = currentNode;
                currentNode->nextNode = node;
                currentNode = node;
                currentAbsolutePosition++;
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

    if ( DcmList::empty() )                        // list is empty !
        return NULL;
    else if ( !DcmList::valid() )
        return NULL;                               // current node is 0
    else
    {
        tempnode = currentNode;

        if ( currentNode->prevNode == NULL )
            firstNode = currentNode->nextNode;     // delete first element
        else
            currentNode->prevNode->nextNode = currentNode->nextNode;

        if ( currentNode->nextNode == NULL )
            lastNode = currentNode->prevNode;      // delete last element
        else
            currentNode->nextNode->prevNode = currentNode->prevNode;

        currentNode = currentNode->nextNode;
        tempobj = tempnode->value();
        delete tempnode;
        // NB: no need to update currentAbsolutePosition
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
            currentNode = firstNode;
            if ( DcmList::valid() )
                currentAbsolutePosition = 0;
            else
                currentAbsolutePosition = invalidAbsolutePosition;
            break;
        case ELP_last :
            currentNode = lastNode;
            if ( DcmList::valid() )
                currentAbsolutePosition = cardinality - 1;
            else
                currentAbsolutePosition = invalidAbsolutePosition;
            break;
        case ELP_prev :
            if ( DcmList::valid() )
            {
                currentNode = currentNode->prevNode;
                if ( DcmList::valid() )
                    currentAbsolutePosition--;
                else
                    currentAbsolutePosition = invalidAbsolutePosition;
            }
            break;
        case ELP_next :
            if ( DcmList::valid() )
            {
                currentNode = currentNode->nextNode;
                if ( DcmList::valid() )
                    currentAbsolutePosition++;
                else
                    currentAbsolutePosition = invalidAbsolutePosition;
            }
            break;
        default:
            break;
    }
    return DcmList::valid() ? currentNode->value() : NULL;
}


// ********************************


DcmObject *DcmList::seek_to(unsigned long absolute_position)
{
    if (absolute_position >= cardinality)
    {
        /* invalid position */
        currentNode = NULL;
        currentAbsolutePosition = invalidAbsolutePosition;
        return get( ELP_atpos );
    }

    if (currentAbsolutePosition != invalidAbsolutePosition)
    {
        const unsigned long distance = absolute_position >= currentAbsolutePosition
            ? absolute_position - currentAbsolutePosition
            : currentAbsolutePosition - absolute_position;

        // Are we seeking to a position that is closer to the current position, than to
        // the start or end of the sequence? This is often the case, if we are using
        // `seek_to` to essentially iterate over the sequence, for example. If so, then
        // let's start iterating from the current position. Often the position we want 
        // is simply the next position (or maybe the previous one). Let's make those
        // use cases be O(1), and not O(n).
        if (distance <= absolute_position && distance < cardinality - absolute_position)
        {
            if (currentAbsolutePosition <= absolute_position)
                while (currentAbsolutePosition < absolute_position)
                    seek( ELP_next );
            else
                while (currentAbsolutePosition > absolute_position)
                    seek( ELP_prev );

            return get( ELP_atpos );
        }
    }

    // Iterate from the start or the end of the list.

    if (absolute_position < cardinality / 2)
    {
        /* iterate over first half of the list */
        seek( ELP_first );
        for (unsigned long i = 0; i < absolute_position; i++)
            seek( ELP_next );
    }
    else
    {
        assert(absolute_position < cardinality);
        /* iterate over second half of the list (starting from the end) */
        seek( ELP_last );
        for (unsigned long i = absolute_position + 1; i < cardinality; i++)
            seek( ELP_prev );
    }
    return get( ELP_atpos );
}


// ********************************


void DcmList::deleteAllElements()
{
    unsigned long numElements = cardinality;
    DcmListNode* tmpNode = NULL;
    DcmObject* tmpObject = NULL;
    // delete all elements
    for (unsigned long i = 0; i < numElements; i++)
    {
        // always select first node so no search is necessary
        tmpNode = firstNode;
        // clear value of node
        tmpObject = tmpNode->value();
        if (tmpObject != NULL)
        {
          // delete load of selected list node
          delete tmpObject;
          tmpObject = NULL;
        }
        firstNode = tmpNode->nextNode;
        // delete the list node itself
        delete tmpNode;
    }
    // reset all attributes for later use
    firstNode = NULL;
    lastNode = NULL;
    currentNode = NULL;
    currentAbsolutePosition = invalidAbsolutePosition;
    cardinality = 0;
}
