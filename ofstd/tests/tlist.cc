/*
 test/demo of generic lists
*/

#include <assert.h>

#define tassert(ex) {if ((ex)) cerr << #ex << "\n"; \
                       else _assert(#ex, __FILE__,__LINE__); }

#include <iostream.h>
#include "oflist.h"
#include "ofalgo.h"

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
    cout << *it << " ";
  cout << "\n";
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
    old_rand = (int)(((long)old_rand * (long)1243) % (long)971);
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
    cout << *i2++ << endl;
  return 0;
}

main()
{
  OFList<int> a;  int i;
  OFListIterator(int) it, bit;
  sequence(a, 1, 20);
  cout << "\nOFList<int> a = sequence(1, 20);\n"; print(a);
  for (it = a.begin (), i = 0; it != a.end (); it++, i++)
    assert (*it == i + 1);
  OFList<int> b;
  randseq(b, 20);
  cout << "\nOFList<int> b = randseq(20);\n"; print(b);
  OFList<int> c;
  OFListInsert(OFListIterator(int), int, c, c.end(), a.begin(), a.end());
  OFListInsert(OFListIterator(int), int, c, c.end(), b.begin(), b.end());
  cout << "\nOFList<int> c = a and b;\n"; print(c);

  OFList<int> d;
  for (it = a.begin(); it != a.end(); it++)
    d.insert(d.end (), inc(*it));
  cout << "\nOFList<int> d = map(inc, a);\n"; print(d);

  OFList<int> f;
  for (it = a.begin(); it != a.end(); it++)
    if (is_odd (*it))
      f.insert(f.end (), *it);
  cout << "\nOFList<int> f = select(is_odd, a);\n"; print(f);
  OFList<int> ff;
  for (it = f.begin(); it != f.end(); it++)
    if (is_even (*it))
      ff.insert(ff.end (), *it);
  assert(ff.empty());

  int red = 0;
  for (it = a.begin(); it != a.end(); it++)
    red += *it;
  cout << "\nint  red = a.reduce(plus, 0);\n"; cout << red;
  it = a.begin(); ++it; ++it;
  int second = *it;
  cout << "\nint second = a[2];\n"; cout << second;
  OFList<int> g;
  for (it = a.begin(), bit = b.begin(); it != a.end () && bit != b.end (); )
    g.insert (g.end (), *it++ + *bit++);
  cout << "\nOFList<int> g = combine(plus, a, b);\n"; print(g);
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
  cout << "\ng.del(is_odd);\n"; print(g);

  ff.erase (ff.begin (), ff.end());
  for (it = g.begin(); it != g.end(); it++)
    if (is_odd (*it))
      ff.insert (ff.end (), *it);
  assert(ff.empty());

  test_splice ();

  cout << "\ndone\n";
}




