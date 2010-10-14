/*
 *
 *  Copyright (C) 2009-2010, OFFIS e.V.
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
 *  Author:  Uli Schlachter
 *
 *  Purpose: test programm for class OFMap
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:15 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#define INCLUDE_CASSERT
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/ofstd/ofconsol.h"


int main()
{
    OFMap<int, int> m;
    OFMap<int, int>::iterator it;

    // Fill a map with some entries
    for (int i = 1; i <= 6; ++i)
        m[i] = i;

#define NEXT(n)                 \
    do                           \
    {                            \
        assert(m[n] == n);       \
        assert(it->first == n);  \
        assert(it->second == n); \
        it++;                    \
    } while (0)

    // and verify they where really added
    assert(m.size() == 6);

    it = m.begin();
    assert(it != m.end());

    NEXT(1);
    NEXT(2);
    NEXT(3);
    NEXT(4);
    NEXT(5);
    NEXT(6);
#undef NEXT

    assert(it == m.end());

    // Now check if removing "4" really removes it
    it = m.find(4);
    assert(it != m.end());
    m.erase(it);
    assert(m.find(4) == m.end());

    // Do the same again but using a different function for removing "5"
    assert(m.find(5) != m.end());
    m.erase(5);
    assert(m.find(5) == m.end());

    // Now remove a range of objects by removing 1 and 2
    assert(m.size() == 4);
    m.erase(m.find(1), m.find(3));
    assert(m.size() == 2);
    assert(m.find(2) == m.end());

    assert(m.size() == 2);
    assert(m[3] == 3);

    COUT << "Everything is ok" << OFendl; // ... or assert() is disabled

    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: tmap.cc,v $
 * Revision 1.2  2010-10-14 13:15:15  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.1  2009-09-29 13:59:34  uli
 * Fix an invalid iterator use in OFMap. A iterator was used after it was passed
 * to erase().
 * Add a test case which verifies some of OFMap's implementation.
 *
 *
 */
