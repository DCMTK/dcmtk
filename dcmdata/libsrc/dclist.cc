/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-04-16 13:43:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dclist.cc,v $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofstream.h"
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


/*
 * CVS/RCS Log:
 * $Log: dclist.cc,v $
 * Revision 1.11  2002-04-16 13:43:18  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
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
