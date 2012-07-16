/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: List class with procedural API compatible to MIR CTN
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/lst.h"

LST_HEAD::LST_HEAD()
: theList()
, theIterator()
{
  theIterator = theList.end();
}

LST_HEAD::~LST_HEAD()
{
}

void LST_HEAD::push_back(void *node)
{
  theList.push_back(node);
}

void *LST_HEAD::dequeue()
{
  if (theList.size() == 0) return NULL;
  void *result = theList.front();
  theList.pop_front();
  return result;
}

size_t LST_HEAD::size() const
{
  return theList.size();
}

void *LST_HEAD::front()
{
  if (theList.size() > 0) return theList.front();
  else return NULL;
}

void *LST_HEAD::next()
{
  if (theList.size() == 0) return NULL;
  if (theIterator == theList.end()) return NULL;
  ++theIterator;
  if (theIterator == theList.end()) return NULL;
  return *theIterator;  
}

void *LST_HEAD::current() const
{
  if (theList.size() == 0) return NULL;
  OFListConstIterator(void *) it = theIterator;
  if (it == theList.end()) return NULL;
  return *theIterator;  
}

void *LST_HEAD::position(void *node)
{
  OFListIterator(void *) first = theList.begin();
  OFListIterator(void *) last = theList.end();
  while (first != last)
  {
    if (*first == node)
    {
      theIterator = first;
      return node;
    }
    ++first;
  }
  theIterator = last;
  return NULL;  
}


/*******************************************************/


LST_HEAD *LST_Create()
{
  return new LST_HEAD();
}

void LST_Destroy(LST_HEAD **lst)
{
  delete *lst;
  *lst = NULL;
}

void LST_Enqueue(LST_HEAD **lst, void *node)
{
  (*lst)->push_back(node);
}

void *LST_Dequeue(LST_HEAD **lst)
{
  return (*lst)->dequeue();
}

void *LST_Pop(LST_HEAD **lst)
{
  return (*lst)->dequeue();
}

unsigned long LST_Count(LST_HEAD **lst)
{
  return (unsigned long)((*lst)->size());
}

void *LST_Head(LST_HEAD ** lst)
{
    return (*lst)->front();
}


void *LST_Next(LST_HEAD **lst)
{
  return (*lst)->next();
}
  
void *LST_Current(LST_HEAD **lst)
{
  return (*lst)->current();
}

void *LST_Position(LST_HEAD ** lst, void *node)
{
  return (*lst)->position(node);
}
