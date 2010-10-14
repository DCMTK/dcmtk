/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: test programm for classes OFList and OFListIterator
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:15 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofalgo.h"
#include "dcmtk/ofstd/ofconsol.h"

#define INCLUDE_CASSERT
#include "dcmtk/ofstd/ofstdinc.h"

#define tassert(ex) {if ((ex)) CERR << #ex << "\n"; \
                       else _assert(#ex, __FILE__,__LINE__); }

OFBool int_compare(int a, int b)
{
  return a < b;
}

int inc(int x)
{
  return x + 1;
}

void print(OFList<int>& l)
{
  for (OFListIterator(int) it = l.begin(); it != l.end(); it++)
    COUT << *it << " ";
  COUT << "\n";
}

int is_odd(int x)
{
  return x & 1;
}

int is_even(int x)
{
  return (x & 1) == 0;
}

void sequence(OFList<int>& a, int lo, int hi)
{
  while (lo <= hi)
    a.push_back(lo++);
}

int old_rand = 9999;

int get_rand()
{
    old_rand = OFstatic_cast(int,
      (OFstatic_cast(long, old_rand) * OFstatic_cast(long, 1243))
      % OFstatic_cast(long, 971));
    return old_rand;
}

void randseq(OFList<int>& a, int n)
{
  while (--n >= 0)
    a.push_back(get_rand() % 50);
}

int array1 [] = { 9, 16, 36 };
int array2 [] = { 1, 4 };

int test_splice ()
{
  OFList<int> l1;
  OFListInsert(int *, int, l1, l1.end(), array1, array1 + 3);
  OFList<int> l2;
  OFListInsert(int *, int, l2, l2.end(), array2, array2 + 2);
  OFListIterator(int) i1 = l1.begin ();
  l1.splice (i1, l2);
  OFListIterator(int) i2 = l1.begin ();
  while (i2 != l1.end ())
    COUT << *i2++ << OFendl;
  return 0;
}

int main()
{
  OFList<int> a;  int i;
  OFListIterator(int) it, bit;
  sequence(a, 1, 20);
  COUT << "\nOFList<int> a = sequence(1, 20);\n"; print(a);
  for (it = a.begin (), i = 0; it != a.end (); it++, i++)
    assert (*it == i + 1);
  OFList<int> b;
  randseq(b, 20);
  COUT << "\nOFList<int> b = randseq(20);\n"; print(b);
  OFList<int> c;
  OFListInsert(OFListIterator(int), int, c, c.end(), a.begin(), a.end());
  OFListInsert(OFListIterator(int), int, c, c.end(), b.begin(), b.end());
  COUT << "\nOFList<int> c = a and b;\n"; print(c);

  OFList<int> d;
  for (it = a.begin(); it != a.end(); it++)
    d.insert(d.end (), inc(*it));
  COUT << "\nOFList<int> d = map(inc, a);\n"; print(d);

  OFList<int> f;
  for (it = a.begin(); it != a.end(); it++)
    if (is_odd (*it))
      f.insert(f.end (), *it);
  COUT << "\nOFList<int> f = select(is_odd, a);\n"; print(f);
  OFList<int> ff;
  for (it = f.begin(); it != f.end(); it++)
    if (is_even (*it))
      ff.insert(ff.end (), *it);
  assert(ff.empty());

  int red = 0;
  for (it = a.begin(); it != a.end(); it++)
    red += *it;
  COUT << "\nint  red = a.reduce(plus, 0);\n"; COUT << red;
  it = a.begin(); ++it; ++it;
  int second = *it;
  COUT << "\nint second = a[2];\n"; COUT << second;
  OFList<int> g;
  for (it = a.begin(), bit = b.begin(); it != a.end () && bit != b.end (); )
    g.insert (g.end (), *it++ + *bit++);
  COUT << "\nOFList<int> g = combine(plus, a, b);\n"; print(g);
#if 0
  for (it = g.begin(); it != g.end(); )
    {
      bit = it++;
      if (is_odd (*bit))
	g.erase (bit);
    }
#else
  typedef int (* Function)(int);
  OFListRemoveIf(Function, int, g, Function(is_odd));
#endif
  COUT << "\ng.del(is_odd);\n"; print(g);

  ff.erase (ff.begin (), ff.end());
  for (it = g.begin(); it != g.end(); it++)
    if (is_odd (*it))
      ff.insert (ff.end (), *it);
  assert(ff.empty());

  test_splice ();

  COUT << "\ndone\n";
}


/*
**
** CVS/RCS Log:
** $Log: tlist.cc,v $
** Revision 1.12  2010-10-14 13:15:15  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.11  2006/08/14 16:42:48  meichel
** Updated all code in module ofstd to correctly compile if the standard
**   namespace has not included into the global one with a "using" directive.
**
** Revision 1.10  2005/12/08 15:49:04  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.9  2004/01/16 10:37:23  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.8  2003/08/14 09:01:20  meichel
** Adapted type casts to new-style typecast operators defined in ofcast.h
**
** Revision 1.7  2002/11/27 11:23:13  meichel
** Adapted module ofstd to use of new header file ofstdinc.h
**
** Revision 1.6  2002/04/16 13:37:00  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.5  2001/06/01 15:51:40  meichel
** Updated copyright header
**
** Revision 1.4  2000/03/08 16:36:07  meichel
** Updated copyright header.
**
** Revision 1.3  2000/03/03 14:02:52  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.2  1998/11/27 12:42:09  joergr
** Added copyright message to source files and changed CVS header.
**
**
*/
