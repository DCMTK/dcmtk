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
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: List class with procedural API compatible to MIR CTN
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2009-09-04 13:53:09 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/lst.cc,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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

OFCondition LST_Destroy(LST_HEAD **lst)
{
  delete *lst;
  *lst = NULL;
  return EC_Normal;
}

OFCondition LST_Enqueue(LST_HEAD **lst, void *node)
{
  (*lst)->push_back(node);
  return EC_Normal;
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


/*
 * CVS Log
 * $Log: lst.cc,v $
 * Revision 1.7  2009-09-04 13:53:09  meichel
 * Minor const iterator related changes needed to compile with VC6 with HAVE_STL
 *
 * Revision 1.6  2005-12-08 15:44:55  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.5  2003/06/02 16:44:11  meichel
 * Renamed local variables to avoid name clashes with STL
 *
 * Revision 1.4  2001/10/12 10:17:36  meichel
 * Re-implemented the LST module (double linked list functions)
 *   used in the dcmnet module from scratch based on OFList.
 *
 *
 */
