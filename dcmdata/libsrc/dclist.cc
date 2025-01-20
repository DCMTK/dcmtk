/*
 *
 *  Copyright (C) 1994-2025, OFFIS e.V.
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
#include "dcmtk/ofstd/oflimits.h"
#include "dcmtk/dcmdata/dclist.h"


// *****************************************
// *** DcmListNode *************************
// *****************************************


DcmListNode::DcmListNode(DcmObject *obj)
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


// value should be identical to DCM_EndOfListIndex, e.g. 0xffffffff for 32 bit
static const unsigned long invalidListPosition = OFnumeric_limits<unsigned long>::max();


// ********************************


DcmList::DcmList()
  : firstNode(NULL),
    lastNode(NULL),
    currentNode(NULL),
    currentPosition(invalidListPosition),
    cardinality(0)
{
}


// ********************************


DcmList::~DcmList()
{
    if (!DcmList::empty())
    {
        lastNode->nextNode = NULL;                 // set to 0 for safety reasons
        do {
            DcmListNode *temp = firstNode;
            firstNode = firstNode->nextNode;
            // delete temp->objNodeValue;          // dangerous!
            delete temp;
        } while (firstNode != NULL);
        currentNode = firstNode = lastNode = NULL;
        currentPosition = invalidListPosition;
    }
}


// ********************************


DcmObject *DcmList::append(DcmObject *obj)
{
    if (obj != NULL)
    {
        if (DcmList::empty())
        {
            currentNode = firstNode = lastNode = new DcmListNode(obj);
            currentPosition = cardinality;
            cardinality++;
        }
        // check whether object can be inserted
        else if (cardinality < DCM_EndOfListIndex)
        {
            DcmListNode *node = new DcmListNode(obj);
            lastNode->nextNode = node;
            node->prevNode = lastNode;
            currentNode = lastNode = node;
            currentPosition = cardinality;
            cardinality++;
        } else {
            DCMDATA_DEBUG("DcmList::append() cannot insert object, maximum number of entries reached");
            obj = NULL;
        }
    } // obj == NULL
    return obj;
}


// ********************************


DcmObject *DcmList::prepend(DcmObject *obj)
{
    if (obj != NULL)
    {
        if (DcmList::empty())
        {
            currentNode = firstNode = lastNode = new DcmListNode(obj);
            currentPosition = 0;
            cardinality++;
        }
        // check whether object can be inserted
        else if (cardinality < DCM_EndOfListIndex)
        {
            DcmListNode *node = new DcmListNode(obj);
            node->nextNode = firstNode;
            firstNode->prevNode = node;
            currentNode = firstNode = node;
            currentPosition = 0;
            cardinality++;
        } else {
            DCMDATA_DEBUG("DcmList::prepend() cannot insert object, maximum number of entries reached");
            obj = NULL;
        }
    } // obj == NULL
    return obj;
}


// ********************************


DcmObject *DcmList::insert(DcmObject *obj, const E_ListPos pos)
{
    if (obj != NULL)
    {
        if (DcmList::empty())
        {
            currentNode = firstNode = lastNode = new DcmListNode(obj);
            currentPosition = 0;
            cardinality++;
        }
        // check whether object can be inserted
        else if (cardinality < DCM_EndOfListIndex)
        {
            if (pos == ELP_last)                   // insert at the end
                DcmList::append(obj);
            else if (pos == ELP_first)             // insert at the beginning
                DcmList::prepend(obj);
            else if (!DcmList::valid())
                // set current node to the end if there is no predecessor or
                // there are successors to be determined
                DcmList::append(obj);
            else if (pos == ELP_prev)              // insert before current node
            {
                DcmListNode *node = new DcmListNode(obj);
                if (currentNode->prevNode == NULL)
                    firstNode = node;              // insert at the beginning
                else
                    currentNode->prevNode->nextNode = node;
                node->prevNode = currentNode->prevNode;
                node->nextNode = currentNode;
                currentNode->prevNode = node;
                currentNode = node;
                // NB: no need to update currentPosition
                cardinality++;
            }
            else // (pos == ELP_next || pos == ELP_atpos)
                                                   // insert after current node
            {
                DcmListNode *node = new DcmListNode(obj);
                if (currentNode->nextNode == NULL)
                    lastNode = node;               // append to the end
                else
                    currentNode->nextNode->prevNode = node;
                node->nextNode = currentNode->nextNode;
                node->prevNode = currentNode;
                currentNode->nextNode = node;
                currentNode = node;
                currentPosition++;
                cardinality++;
            }
        } else {
            DCMDATA_DEBUG("DcmList::insert() cannot insert object, maximum number of entries reached");
            obj = NULL;
        }
    } // obj == NULL
    return obj;
}


// ********************************


DcmObject *DcmList::remove()
{
    DcmObject *tempobj;
    DcmListNode *tempnode;

    if (DcmList::empty())
        return NULL;
    else if (!DcmList::valid())
        return NULL;                               // current node is 0
    else
    {
        tempnode = currentNode;

        if (currentNode->prevNode == NULL)
            firstNode = currentNode->nextNode;     // delete first element
        else
            currentNode->prevNode->nextNode = currentNode->nextNode;

        if (currentNode->nextNode == NULL)
            lastNode = currentNode->prevNode;      // delete last element
        else
            currentNode->nextNode->prevNode = currentNode->prevNode;

        currentNode = currentNode->nextNode;
        tempobj = tempnode->value();
        delete tempnode;
        // NB: no need to update currentPosition
        cardinality--;
        return tempobj;
    }
}


// ********************************


DcmObject *DcmList::get(const E_ListPos pos)
{
    return seek(pos);
}


// ********************************


DcmObject *DcmList::seek(const E_ListPos pos)
{
    switch (pos)
    {
        case ELP_first:
            currentNode = firstNode;
            if (DcmList::valid())
                currentPosition = 0;
            else
                currentPosition = invalidListPosition;
            break;
        case ELP_last:
            currentNode = lastNode;
            if (DcmList::valid())
                currentPosition = cardinality - 1;
            else
                currentPosition = invalidListPosition;
            break;
        case ELP_prev:
            if (DcmList::valid())
            {
                currentNode = currentNode->prevNode;
                if (DcmList::valid())
                    currentPosition--;
                else
                    currentPosition = invalidListPosition;
            }
            break;
        case ELP_next:
            if (DcmList::valid())
            {
                currentNode = currentNode->nextNode;
                if (DcmList::valid())
                    currentPosition++;
                else
                    currentPosition = invalidListPosition;
            }
            break;
        default:
            break;
    }
    return DcmList::valid() ? currentNode->value() : NULL;
}


// ********************************


DcmObject *DcmList::seek_to(const unsigned long absolute_position)
{
    if (absolute_position >= cardinality)
    {
        // invalid position
        currentNode = NULL;
        currentPosition = invalidListPosition;
        return NULL;
    }
    else if (absolute_position == 0)
    {
        // first item in the list
        return seek(ELP_first);
    }
    else if (absolute_position == cardinality - 1)
    {
        // last item in the list
        return seek(ELP_last);
    }
    else if (currentPosition != invalidListPosition)
    {
        // determine distance between current and requested position
        const unsigned long distance = (absolute_position >= currentPosition)
            ? (absolute_position - currentPosition)
            : (currentPosition - absolute_position);

         // Are we seeking to a position that is closer to the current position than to
         // the start or end of the sequence? This is often the case, if we are using
         // seek_to() to essentially iterate over the sequence, for example. If so, then
         // let's start iterating from the current position. Often, the position we want
         // is simply the next position (or maybe the previous one). Let's make those
         // use cases be O(1), and not O(n).
        if ((distance <= absolute_position) && (distance < cardinality - absolute_position))
        {
            if (currentPosition <= absolute_position)
            {
                while (currentPosition < absolute_position)
                    seek(ELP_next);
            }
            else
            {
                while (currentPosition > absolute_position)
                    seek(ELP_prev);
            }
            return get(ELP_atpos);
        }
    }

    // iterate from the start...
    if (absolute_position < cardinality / 2)
    {
        /* iterate over first half of the list */
        seek(ELP_first);
        for (unsigned long i = 0; i < absolute_position; i++)
            seek(ELP_next);
    }
    else // ... or the end of the list
    {
        assert(absolute_position < cardinality);
        // iterate over second half of the list (starting from the end)
        seek(ELP_last);
        for (unsigned long i = absolute_position + 1; i < cardinality; i++)
            seek(ELP_prev);
    }
    return get(ELP_atpos);
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
    currentPosition = invalidListPosition;
    cardinality = 0;
}
