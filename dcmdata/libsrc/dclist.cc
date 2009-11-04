/*
 *
 *  Copyright (C) 1994-2009, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: generic list class
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2009-11-04 09:58:10 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dclist.h"


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


DcmList::DcmList()
  : firstNode(NULL),
    lastNode(NULL),
    currentNode(NULL),
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
            // delete temp->objNodeValue;;        // dangerous!
            delete temp;
        } while ( firstNode != NULL );
        currentNode = firstNode = lastNode = NULL;
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
            break;
        case ELP_last :
            currentNode = lastNode;
            break;
        case ELP_prev :
            if ( DcmList::valid() )
                currentNode = currentNode->prevNode;
            break;
        case ELP_next :
            if ( DcmList::valid() )
                currentNode = currentNode->nextNode;
            break;
        default:
            break;
    }
    return DcmList::valid() ? currentNode->value() : NULL;
}


// ********************************


DcmObject *DcmList::seek_to(unsigned long absolute_position)
{
    const unsigned long tmppos = absolute_position < cardinality ? absolute_position : cardinality;
    seek( ELP_first );
    for (unsigned long i = 0; i < tmppos; i++)
        seek( ELP_next );
    return get( ELP_atpos );
}


/*
 * CVS/RCS Log:
 * $Log: dclist.cc,v $
 * Revision 1.15  2009-11-04 09:58:10  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.14  2005-12-08 15:41:17  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.13  2004/01/16 13:50:22  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.12  2003/08/08 12:55:12  joergr
 * Made DcmListNode::value() inline. Translated German comments.
 * Renamed member variable "actualNode" to "currentNode".
 * Removed needless type casts (e.g. on the NULL constant).
 *
 * Revision 1.11  2002/04/16 13:43:18  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.10  2001/06/01 15:49:06  meichel
 * Updated copyright header
 *
 * Revision 1.9  2000/03/08 16:26:37  meichel
 * Updated copyright header.
 *
 * Revision 1.8  1999/03/31 09:25:32  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
