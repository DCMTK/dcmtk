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

#ifdef __GNUG__
#pragma implementation
#endif

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcentset.h"


Pix DcmDictEntryPtrSet::seek(DcmDictEntryPtr  item)
{
  Pix i;
  for (i = first(); i != 0 && !(DcmDictEntryPtrEQ((*this)(i), item)); next(i)) /* */;
  return i;
}

int DcmDictEntryPtrSet::owns(Pix idx)
{
  if (idx == 0) return 0;
  for (Pix i = first(); i; next(i)) if (i == idx) return 1;
  return 0;
}

void DcmDictEntryPtrSet::clear()
{
  Pix i = first(); 
  while (i != 0)
  {
    del((*this)(i));
    i = first();
  }
}

int DcmDictEntryPtrSet::contains (DcmDictEntryPtr  item)
{
  return seek(item) != 0;
}

int DcmDictEntryPtrSet::operator <= (DcmDictEntryPtrSet& b)
{
  if (count > b.count) return 0;
  if (count == 0) return 1;
  for (Pix i = first(); i; next(i)) if (b.seek((*this)(i)) == 0) return 0;
  return 1;
}

int DcmDictEntryPtrSet::operator == (DcmDictEntryPtrSet& b)
{
  int n = count;
  if (n != b.count) return 0;
  if (n == 0) return 1;
  Pix i = first();
  Pix j = b.first();
  while (n-- > 0)
  {
    if ((b.seek((*this)(i)) == 0) || (seek(b(j)) == 0)) return 0;
    next(i);
    b.next(j);
  }
  return 1;
}

int DcmDictEntryPtrSet::operator != (DcmDictEntryPtrSet& b)
{
  return !(*this == b);
}

void DcmDictEntryPtrSet::operator |= (DcmDictEntryPtrSet& b)
{
  if (&b != this)
    for (Pix i = b.first(); i; b.next(i)) add(b(i));
}

void DcmDictEntryPtrSet::operator -= (DcmDictEntryPtrSet& b)
{
  if (&b == this)
    clear();
  else
    for (Pix i = b.first(); i; b.next(i)) del(b(i));
}


void DcmDictEntryPtrSet::operator &= (DcmDictEntryPtrSet& b)
{
  if (&b != this)
  {
    Pix i = first();
    Pix n = i;
    while (i != 0)
    {
      next(n);
      if (b.seek((*this)(i)) == 0) del((*this)(i));
      i = n;
    }
  }
}

void DcmDictEntryPtrSet::error(const char* msg)
{
  cerr << "DcmDictEntryPtrSet: " << msg << endl;
}
