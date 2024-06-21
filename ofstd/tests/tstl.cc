/*
 *
 *  Copyright (C) 2017-2024, OFFIS e.V.
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
 *  Author:  Thorben Hasenpusch
 *
 *  Purpose: Minimal tests for the STL classes.
 *
 *  These tests are intended to trigger an error when a standard STL class
 *  (e.g. std::stack, stl::list) is used instead of DCMTK's own implementation,
 *  and the STL implementation does not work. As of 2024, no examples are known,
 *  but we'll keep this in the toolkit for a while.
 *
 *  Note: These tests were originally executed at configure time and directly
 *  used the STL classes in namespace std. The tests have now been converted
 *  to unit tests, and they use DCMTK's abstractions of the STL classes.
 *  This makes sure that the unit tests only trigger an error when a
 *  non-functioning STL class is actually used.
 *
 */

#include "dcmtk/config/osconfig.h"     /* include OS specific configuration first */
#include "dcmtk/ofstd/ofalgo.h"
#include "dcmtk/ofstd/oferror.h"
#include "dcmtk/ofstd/oflimits.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofstack.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/oftuple.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofdiag.h"
#include <climits>

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"

struct X {
    int *n;

    void operator()(int /* x */)
    {
        ++*n;
    }
};

OFTEST(ofstd_std_algorithm)
{
    OFVector<int> vec;
    vec.push_back(1);
    vec.push_back(2);

    int n = 0;
    X x;
    x.n = &n;
    OFForEach(X, X, vec.begin(), vec.end(), x);

    OFCHECK(n == 2);
    OFCHECK(OFFind(X, X, vec.begin(), vec.end(), 44) == vec.end());
    OFCHECK(OFAdjacentFind(X, vec.begin(), vec.end()) == vec.end());
}

OFTEST(ofstd_std_limits)
{
#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_CONST_EXPRESSION_WARNING
    OFCHECK(OFnumeric_limits<signed char>::is_signed);
    OFCHECK(OFnumeric_limits<float>::has_infinity || !OFnumeric_limits<float>::is_iec559);
    OFCHECK(OFnumeric_limits<int>::max() == INT_MAX);
    OFCHECK(OFnumeric_limits<int>::digits == CHAR_BIT * sizeof(int) - 1);
#include DCMTK_DIAGNOSTIC_POP
}

OFTEST(ofstd_std_list)
{
    OFList<int> l;
    OFCHECK(l.empty());

    l.push_back(18);
    l.push_back(22);
    l.push_front(2);
    OFCHECK(*l.begin() == 2);

    int sum = 0;
    for (OFListIterator(int) it = l.begin(); it != l.end(); ++it) {
        sum += *it;
    }
    OFCHECK(sum == 42);

    // test whether we can compare const and mutable iterators
    OFListIterator(int) it = l.begin();
    OFListConstIterator(int) cit = l.begin();
    OFCHECK(it == cit);
}

OFTEST(ofstd_std_map)
{
    OFMap<int, int> m;
    OFCHECK(m.empty());

    m[3] = 555;
    m[66] = 2;
    m[42] = 42;
    OFCHECK(m.size() == 3);
    OFCHECK(m[3] == 555);
    OFCHECK(m[42] == 42);

    m.erase(m.find(3));
    OFCHECK(m.find(3) == m.end());

    m.clear();
    OFCHECK(m.empty());
}

OFTEST(ofstd_std_memory)
{
    int *p = new int(55);
    OFunique_ptr<int> pp(p);

    OFCHECK(*p == *pp);
    OFCHECK(p == pp.get());
    pp.release();

    OFshared_ptr<int> s_p(p);
    OFshared_ptr<int> s_p2(s_p);
    OFshared_ptr<int> s_p3(s_p2);

    OFCHECK(s_p.get() == s_p3.get());
    OFCHECK(*s_p == *s_p3);
}

OFTEST(ofstd_std_stack)
{
    OFStack<int> s;

    OFCHECK(s.empty());

    s.push(1);
    s.push(2);
    s.push(3);
    s.pop();
    s.pop();
    s.push(42);

    OFCHECK(s.top() == 42);
}

OFTEST(ofstd_std_string)
{
    OFString s = "TEST";
    OFCHECK(s[1] == 'E');

    s += "HAHA";

    OFCHECK(s[5] == 'A');

    OFString sub = s.substr(4);
    OFCHECK("HAHA" == sub);

    s.replace(0, 4, "HAHA");
    OFCHECK(s == "HAHAHAHA");
}

OFTEST(ofstd_std_system_error)
{
    // Not much to test, really.
    OFCHECK(strcmp(OFgeneric_category().name(), "generic") == 0);
    OFCHECK(strcmp(OFsystem_category().name(), "system") == 0);
}

OFTEST(ofstd_std_tuple)
{
#if defined(__cplusplus) && (__cplusplus >= 201103L)
    auto tuple = OFmake_tuple(1, "TEST");
#else
    OFtuple<int, const char *> tuple = OFmake_tuple(1, "TEST");
#endif
    OFCHECK(OFget<0>(tuple) == 1);

#if defined(__cplusplus) && (__cplusplus >= 201103L)
    // decltype is C++11
    constexpr bool tuple_size_is_two = OFtuple_size<decltype(tuple)>::value == 2;
    OFCHECK(tuple_size_is_two);
#endif

    int n;
    const char *p;
    OFtie(n, p) = tuple;
    OFCHECK(n == 1);

    // ensure OFtuple can take at least 50 template arguments
    OFtuple<int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int,int> t;
    (void) t;
}

struct Recursive : OFVector<Recursive>
{
    int i;
};

OFTEST(ofstd_std_vector)
{
    OFVector<int> v;

    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    OFCHECK(v.size() == 3);
    OFCHECK(v[2] == 3);

    v.erase(v.begin());
    OFCHECK(v[0] == 2);

    v.pop_back();
    OFCHECK(v.size() == 1);

    v.clear();
    v.push_back(42);
    v.push_back(13);
    v.resize(1);
    OFCHECK(v.size() == 1 && v[0] == 42);
    OFCHECK(v.front() == v[0]);
    OFCHECK(v.back() == v[v.size() - 1]);

    // ensure iterators are NOT invalidated by swap()
    OFVector<int>::iterator it = v.begin();
    OFVector<int> w;
    w.push_back(23);
    v.swap(w);
    OFCHECK(it == w.begin());

    // test if recursive vector structures can be constructed without causing
    // a stack overflow
    OFCHECK(OFVector<Recursive>().size() == 0);

    // test whether we can compare const and mutable iterators
    OFVector<int>::const_iterator cit = w.begin();
    OFCHECK(it == cit);
}

