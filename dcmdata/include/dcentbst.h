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


#ifndef _DcmDictEntryPtrBSTSet_h
#ifdef __GNUG__
#pragma interface
#endif
#define _DcmDictEntryPtrBSTSet_h 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcentset.h"

#ifndef _DcmDictEntryPtrBSTNode
#define _DcmDictEntryPtrBSTNode 1

struct DcmDictEntryPtrBSTNode
{
  DcmDictEntryPtrBSTNode*         lt;
  DcmDictEntryPtrBSTNode*         rt;
  DcmDictEntryPtrBSTNode*         par;
  DcmDictEntryPtr                 item;
                      DcmDictEntryPtrBSTNode(DcmDictEntryPtr  h, DcmDictEntryPtrBSTNode* l=0, DcmDictEntryPtrBSTNode* r=0,
                                 DcmDictEntryPtrBSTNode* p = 0);
                      ~DcmDictEntryPtrBSTNode();
};

inline DcmDictEntryPtrBSTNode::DcmDictEntryPtrBSTNode(DcmDictEntryPtr  h, DcmDictEntryPtrBSTNode* l, DcmDictEntryPtrBSTNode* r,
                              DcmDictEntryPtrBSTNode* p)
:lt(l), rt(r), par(p), item(h) {}

inline DcmDictEntryPtrBSTNode::~DcmDictEntryPtrBSTNode() {}

typedef DcmDictEntryPtrBSTNode* DcmDictEntryPtrBSTNodePtr;

#endif

class DcmDictEntryPtrBSTSet : public DcmDictEntryPtrSet
{
protected:
  DcmDictEntryPtrBSTNode*      root;

  DcmDictEntryPtrBSTNode*     leftmost();
  DcmDictEntryPtrBSTNode*     rightmost();
  DcmDictEntryPtrBSTNode*     pred(DcmDictEntryPtrBSTNode* t);
  DcmDictEntryPtrBSTNode*     succ(DcmDictEntryPtrBSTNode* t);
  void            _kill(DcmDictEntryPtrBSTNode* t);
  DcmDictEntryPtrBSTNode*     _copy(DcmDictEntryPtrBSTNode* t);

public:
                   DcmDictEntryPtrBSTSet();
                   DcmDictEntryPtrBSTSet(DcmDictEntryPtrBSTSet& a);
                   ~DcmDictEntryPtrBSTSet();

  Pix             add(DcmDictEntryPtr  item, int deleteExisting=0);
  void            del(DcmDictEntryPtr  item);
  int             contains(DcmDictEntryPtr  item);

  void            clear();

  Pix             first();
  void            next(Pix& i);
  DcmDictEntryPtr&            operator () (Pix i);
  Pix             seek(DcmDictEntryPtr  item);

  Pix             last();
  void            prev(Pix& i);

  int             operator == (DcmDictEntryPtrBSTSet& b);
  int             operator != (DcmDictEntryPtrBSTSet& b);
  int             operator <= (DcmDictEntryPtrBSTSet& b); 

  void            balance();
  int             OK();
};

inline DcmDictEntryPtrBSTSet::~DcmDictEntryPtrBSTSet()
{
  _kill(root);
}

inline DcmDictEntryPtrBSTSet::DcmDictEntryPtrBSTSet()
{
  root = 0;
  count = 0;
}


inline DcmDictEntryPtrBSTSet::DcmDictEntryPtrBSTSet(DcmDictEntryPtrBSTSet& a)
{
  count = a.count;
  root = _copy(a.root);
}

inline int DcmDictEntryPtrBSTSet::operator != (DcmDictEntryPtrBSTSet& b)
{
  return ! (*this == b);
}

#if !defined(__MWERKS__)
/* for some strange reason Metrowerks 5.1 does not like these inlines.
 * Metrowerks 6.0 is happy
 */
inline Pix DcmDictEntryPtrBSTSet::first()
{
  return Pix(leftmost());
}

inline Pix DcmDictEntryPtrBSTSet::last()
{
  return Pix(rightmost());
}

inline void DcmDictEntryPtrBSTSet::next(Pix& i)
{
  if (i != 0) i = Pix(succ((DcmDictEntryPtrBSTNode*)i));
}

inline void DcmDictEntryPtrBSTSet::prev(Pix& i)
{
  if (i != 0) i = Pix(pred((DcmDictEntryPtrBSTNode*)i));
}

inline DcmDictEntryPtr& DcmDictEntryPtrBSTSet::operator () (Pix i)
{
  if (i == 0) error("null Pix");
  return ((DcmDictEntryPtrBSTNode*)i)->item;
}

inline void DcmDictEntryPtrBSTSet::clear()
{
  _kill(root);
  count = 0;
  root = 0;
}

inline int DcmDictEntryPtrBSTSet::contains(DcmDictEntryPtr  key)
{
  return seek(key) != 0;
}

#endif /*!defined(__MWERKS__)*/

#endif
