/*
 *
 *  Copyright (C) 2009, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: test programm for class OFVec
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-10-08 13:25:33 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#define INCLUDE_CASSERT
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofconsol.h"


int main()
{
    OFVector<int> m;
    OFVector<int>::iterator it;

    // Just for the fun of it, test if an empty vector is empty
    // (and if one can have vectors in other vectors).
    assert(OFVector<OFVector<char> >().empty());

    // Test whether iterators stay valid if enough space was reserved
    m.reserve(6);
    it = m.begin();

    // Fill with some entries
    for (int i = 1; i <= 6; ++i)
        m.push_back(i);

    assert(it == m.begin());
    assert(it != m.end());

    // verify that the entries where really added
    assert(m.size() == 6);
    for (int i = 1; i <= 6; ++i)
        assert(*(it++) == i);

    assert(it == m.end());

    // check erase()
    for (int i = 1; i <= 3; ++i)
    {
        assert(m.at(0) == i);
        assert(m[0] == i);
        m.erase(m.begin());
    }

    assert(m.size() == 3);

    // test the copy-constructor
    OFVector<int> n(m);
    assert(n.size() == 3);

    // does pop_back() work?
    n.pop_back();
    assert(n.size() == 2);
    assert(m.size() == 3);

    // test operator= and the range-constructor
    n = OFVector<int>(m.begin(), m.end());
    assert(n.size() == 3);
    assert(n[1] == 5);
    n[1] = 42;
    assert(n[1] == 42);
    n.pop_back();

    // does swap do what it is supposed to do?
    it = m.begin();
    m.swap(n);
    assert(it == n.begin());
    assert(n.size() == 3);
    assert(m.size() == 2);
    assert(n[1] == 5);
    assert(m[1] == 42);

    // does clear() really clear the vector?
    n.clear();
    assert(n.empty());

    // this should produce a vector which elements 0 to 49 with value "5",
    // then the values "4" and "42" once and then another 50 times "6".
    n = OFVector<int>(50, 5);
    n.resize(100, 6);
    assert(n.size() == 100);

    n.insert(n.begin() + 50, m.begin(), m.end());
    assert(n.size() == 102);

    it = n.begin();
    for (int i = 0; it != n.end(); ++it, ++i)
    {
        if (i < 50)
            assert(*it == 5);
        else if(i == 50)
            assert(*it == 4);
        else if(i == 51)
            assert(*it == 42);
        else
            assert(*it == 6);
    }

    COUT << "Everything is ok" << OFendl; // ... or assert() is disabled

    return 0;
}


/*
**
** CVS/RCS Log:
** $Log: tvec.cc,v $
** Revision 1.1  2010-10-08 13:25:33  uli
** Implement OFVector.
**
**
**
**
*/
