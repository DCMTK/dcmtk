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

#include "dcmtk/dcmdata/dclist.h"


// *****************************************
// *** DcmList *****************************
// *****************************************


DcmList::DcmList()
  : current(objects.end())
{
}


// ********************************


DcmList::~DcmList()
{
    deleteAllElements();
}


// ********************************


DcmObject *DcmList::append( DcmObject *obj )
{
    current = objects.insert(objects.end(), obj);
    return obj;
}


// ********************************


DcmObject *DcmList::prepend( DcmObject *obj )
{
    current = objects.insert(objects.begin(), obj);
    return obj;
}


// ********************************


DcmObject *DcmList::insert( DcmObject *obj, E_ListPos pos )
{
    if ( obj != NULL )
    {
        if ( DcmList::empty() )                 // list is empty !
        {
            DcmList::append(obj);
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
                current = objects.insert(current, obj);
            }
            else //( pos==ELP_next || pos==ELP_atpos )
                                                // insert after current node
            {
                current = objects.insert(std::next(current), obj);
            }
        }
    } // obj == NULL
    return obj;
}


// ********************************


DcmObject *DcmList::remove()
{
    DcmObject *tempobj;

    if ( DcmList::empty() )                        // list is empty !
        return NULL;
    else if ( !DcmList::valid() )
        return NULL;                               // current node is 0
    else
    {
        tempobj = *current;
        current = objects.erase(current);
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
            current = objects.begin();
            break;
        case ELP_last :
            if (objects.empty())
            	current = objects.end();
            else
                current = std::prev(objects.end());
            break;
        case ELP_prev :
            if ( DcmList::valid() )
                if (current == objects.begin())
                    current = objects.end();
                else
                    --current;
            break;
        case ELP_next :
            if ( DcmList::valid() )
                ++current;
            break;
        default:
            break;
    }
    return DcmList::valid() ? *current : NULL;
}


// ********************************


DcmObject *DcmList::seek_to(unsigned long absolute_position)
{
    if (absolute_position < objects.size())
        current = objects.begin() + absolute_position;
    else
        current = objects.end();

    return get( ELP_atpos );
}


// ********************************


void DcmList::deleteAllElements()
{
    for (DcmObject* object : objects)
        delete object;
    objects.clear();
    current = objects.end();
}
