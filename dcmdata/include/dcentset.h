// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#ifndef _DcmDictEntryPtrSet_h
#ifdef __GNUG__
#pragma interface
#endif
#define _DcmDictEntryPtrSet_h 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "pix.h"
#include "dcentdef.h"

class DcmDictEntryPtrSet
{
protected:

  int                   count;

public:
  virtual              ~DcmDictEntryPtrSet();

  int                   length();                // current number of items
  int                   empty();

  virtual Pix           add(DcmDictEntryPtr  item, int deleteExisting=0) = 0;      // add item; return Pix
  virtual void          del(DcmDictEntryPtr  item) = 0;      // delete item
  virtual int           contains(DcmDictEntryPtr  item);     // is item in set?

  virtual void          clear();                 // delete all items

  virtual Pix           first() = 0;             // Pix of first item or 0
  virtual void          next(Pix& i) = 0;        // advance to next or 0
  virtual DcmDictEntryPtr&          operator () (Pix i) = 0; // access item at i

  virtual int           owns(Pix i);             // is i a valid Pix  ?
  virtual Pix           seek(DcmDictEntryPtr  item);         // Pix of item

  void                  operator |= (DcmDictEntryPtrSet& b); // add all items in b
  void                  operator -= (DcmDictEntryPtrSet& b); // delete items also in b
  void                  operator &= (DcmDictEntryPtrSet& b); // delete items not in b

  int                   operator == (DcmDictEntryPtrSet& b);
  int                   operator != (DcmDictEntryPtrSet& b);
  int                   operator <= (DcmDictEntryPtrSet& b); 

  void                  error(const char* msg);
  virtual int           OK() = 0;                // rep invariant
};

inline DcmDictEntryPtrSet::~DcmDictEntryPtrSet() {}

inline int DcmDictEntryPtrSet::length()
{
  return count;
}

inline int DcmDictEntryPtrSet::empty()
{
  return count == 0;
}

#endif
