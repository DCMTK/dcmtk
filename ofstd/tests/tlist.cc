/*
 *
 *  Copyright (C) 1997-2022, OFFIS e.V.
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
 *  Purpose: test program for classes OFList and OFListIterator
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofalgo.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"

/* This is a macro so that we get better line numbers in error msgs */
#define CHECK(list, expected) do { \
    OFListIterator(int) iter_((list).begin()); \
    const size_t max_idx_ = sizeof(expected)/sizeof((expected)[0]); \
    for (size_t idx_ = 0; idx_ < max_idx_; idx_++) { \
        if (iter_ == (list).end()) { \
            OFCHECK_FAIL("List already ended at index " << idx_); \
            break; \
        } \
        if (*iter_ != (expected)[idx_]) { \
            OFCHECK_FAIL("At index " << idx_ << ": (" \
                    << *iter_ << ") should equal (" << (expected)[idx_] << ")"); \
        } \
        iter_++; \
    } \
    OFCHECK(iter_ == (list).end()); \
} while (0)

static int inc(int x)
{
  return x + 1;
}

static int is_odd(int x)
{
  return x & 1;
}

static int is_even(int x)
{
  return (x & 1) == 0;
}

static void sequence(OFList<int>& a, int lo, int hi)
{
  while (lo <= hi)
    a.push_back(lo++);
}

static int old_rand = 9999;

static int get_rand()
{
    old_rand = OFstatic_cast(int,
      (OFstatic_cast(long, old_rand) * OFstatic_cast(long, 1243))
      % OFstatic_cast(long, 971));
    return old_rand;
}

static void randseq(OFList<int>& a, int n)
{
  while (--n >= 0)
    a.push_back(get_rand() % 50);
}

OFTEST(ofstd_OFList_splice)
{
  int array1 [] = { 9, 16, 36 };
  int array2 [] = { 1, 4 };
  int array3 [] = { 1, 4, 9, 16, 36 };

  OFList<int> l1;
  OFListInsert(int *, int, l1, l1.end(), array1, array1 + 3);
  OFList<int> l2;
  OFListInsert(int *, int, l2, l2.end(), array2, array2 + 2);
  OFListIterator(int) i1 = l1.begin ();
  l1.splice (i1, l2);
  CHECK(l1, array3);
}

OFTEST(ofstd_OFList_1)
{
    OFList<int> l;
    int exp1[] = { 5, 2, 1, 3, 4 };
    int exp2[] = { 5, 2, 50, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
                   1, 3, 4, };
    int exp3[] = { 5, 2, 50, 1, 3, 4 };
    int exp4[] = { 5, 2, 50, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
                   1, 3 };
    int exp5[] = { 5, 2, 50, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };
    int exp6[] = { 5, 2, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };

    l.push_front(1);
    l.push_front(2);
    l.push_back(3);
    l.push_back(4);
    l.push_front(5);

    CHECK(l, exp1);

    OFListIterator(int) a(l.begin());
    ++a; ++a;
    OFCHECK_EQUAL(*a, 1);
    l.insert(a, 50);
    l.insert(a, 10, 100);

    OFListIterator(int) e(l.end());

    // Find 50
    OFCHECK(OFFind(OFListIterator(int), int, l.begin(), e, 50) != e);

    CHECK(l, exp2);

    // Copy list l into l1
    OFList<int> l1(l);
    CHECK(l1, exp2);

    // Copy list l into l1 with operator=
    OFList<int> l2;
    // check if existing elements are properly removed
    l2.push_back(5);
    l2 = l;
    CHECK(l2, exp2);

    // Delete all elements with value 100
    OFListIterator(int) del;
    a = l.begin();
    while(a != e)
    {
        del = a++;
        if (*del == 100)
        {
            l.erase(del);
        }
    }
    CHECK(l, exp3);
    OFCHECK_EQUAL(l.size(), 6);

    unsigned int idx = 0;
    while(!l.empty())
    {
        OFCHECK_EQUAL(l.front(), exp3[idx]);
        l.pop_front();
        idx++;
    }

    OFCHECK_EQUAL(l1.back(), 4);
    l1.pop_back();
    CHECK(l1, exp4);

    // Search for 1
    a = OFFind(OFListIterator(int), int, l1.begin(), l1.end(), 1);
    OFCHECK(a != l1.end());

    // Delete everything after the 1
    l1.erase(a, l1.end());
    CHECK(l1, exp5);
    l1.remove(50);
    CHECK(l1, exp6);
}

OFTEST(ofstd_OFList_2)
{
  int exp1[] = { 28, 27, 5, 17, 44, 6, 9, 40, 15, 26, 49, 35, 15, 48,
                 13, 27, 25, 25, 9, 6 };
  int exp2[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                 17, 18, 19, 20, 28, 27, 5, 17, 44, 6, 9, 40, 15, 26, 49,
                 35, 15, 48, 13, 27, 25, 25, 9, 6 };
  int exp3[] = { 29, 29, 8, 21, 49, 12, 16, 48, 24, 36, 60, 47, 28, 62,
                 28, 43, 42, 43, 28, 26 };
  int exp4[] = { 8, 12, 16, 48, 24, 36, 60, 28, 62, 28, 42, 28, 26 };

  OFList<int> a;  int i;
  OFListIterator(int) it, bit;
  sequence(a, 1, 20);
  OFCHECK_EQUAL(a.size(), 20);
  for (it = a.begin (), i = 0; it != a.end (); it++, i++)
    OFCHECK_EQUAL(*it, i+1);

  OFList<int> b;
  randseq(b, 20);
  CHECK(b, exp1);

  OFList<int> c;
  OFListInsert(OFListIterator(int), int, c, c.end(), a.begin(), a.end());
  OFListInsert(OFListIterator(int), int, c, c.end(), b.begin(), b.end());
  CHECK(c, exp2);

  OFList<int> d;
  for (it = a.begin(); it != a.end(); it++)
    d.insert(d.end (), inc(*it));
  OFCHECK_EQUAL(a.size(), 20);
  for (it = d.begin (), i = 0; it != d.end (); it++, i++)
    OFCHECK_EQUAL(*it, i+2);

  OFList<int> f;
  for (it = a.begin(); it != a.end(); it++)
    if (is_odd (*it))
      f.insert(f.end (), *it);
  for (it = f.begin (), i = 0; it != f.end (); it++, i++)
    OFCHECK_EQUAL(*it, i*2+1);

  OFList<int> ff;
  for (it = f.begin(); it != f.end(); it++)
    if (is_even (*it))
      ff.insert(ff.end (), *it);
  OFCHECK(ff.empty());

  int red = 0;
  for (it = a.begin(); it != a.end(); it++)
    red += *it;
  OFCHECK_EQUAL(red, 210);
  it = a.begin(); ++it; ++it;
  int second = *it;
  OFCHECK_EQUAL(second, 3);
  OFList<int> g;
  for (it = a.begin(), bit = b.begin(); it != a.end () && bit != b.end (); )
    g.insert (g.end (), *it++ + *bit++);
  CHECK(g, exp3);
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
  CHECK(g, exp4);

  ff.erase (ff.begin (), ff.end());
  for (it = g.begin(); it != g.end(); it++)
    if (is_odd (*it))
      ff.insert (ff.end (), *it);
  OFCHECK(ff.empty());

}
