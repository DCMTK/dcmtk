/*
 *
 *  Copyright (C) 2009-2011, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-05-25 10:05:57 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/ofstd/ofmap.h"


OFTEST(ofstd_OFMap)
{
    OFMap<int, int> m;
    OFMap<int, int>::iterator it;
    int i;

    // Fill a map with some entries
    for (i = 1; i <= 6; ++i)
        m[i] = i;

    // and verify they where really added
    OFCHECK_EQUAL(m.size(), 6);

    it = m.begin();
    OFCHECK(it != m.end());

    for (i = 1; i <= 6; ++i)
    {
        OFCHECK_EQUAL(m[i], i);
        OFCHECK_EQUAL(it->first, i);
        OFCHECK_EQUAL(it->second, i);
        it++;
    }

    OFCHECK(it == m.end());

    // Now check if removing "4" really removes it
    it = m.find(4);
    OFCHECK(it != m.end());
    m.erase(it);
    OFCHECK(m.find(4) == m.end());

    // Do the same again but using a different function for removing "5"
    OFCHECK(m.find(5) != m.end());
    m.erase(5);
    OFCHECK(m.find(5) == m.end());

    // Now remove a range of objects by removing 1 and 2
    OFCHECK_EQUAL(m.size(), 4);
    m.erase(m.find(1), m.find(3));
    OFCHECK_EQUAL(m.size(), 2);
    OFCHECK(m.find(2) == m.end());

    OFCHECK_EQUAL(m.size(), 2);
    OFCHECK_EQUAL(m[3], 3);
}


/*
 *
 * CVS/RCS Log:
 * $Log: tmap.cc,v $
 * Revision 1.3  2011-05-25 10:05:57  uli
 * Imported oftest and converted existing tests to oftest.
 *
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
