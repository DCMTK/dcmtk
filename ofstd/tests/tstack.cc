/*
 *
 *  Copyright (C) 1997-2011, OFFIS e.V.
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
 *  Purpose: test programm for class OFStack
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-05-25 10:05:57 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofstack.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"


OFTEST(ofstd_OFStack)
{
    OFStack<int> st;
    int i;
    st.push(1);
    st.push(2);
    st.push(3);

    OFStack<int> nst(st);

    OFCHECK_EQUAL(st.size(), 3);
    for (i = 3; i >= 1; i--)
    {
        OFCHECK(!st.empty());
        OFCHECK_EQUAL(i, st.top());
        st.pop();
    }

    OFCHECK_EQUAL(nst.size(), 3);
    for (i = 3; i >= 1; i--)
    {
        OFCHECK(!nst.empty());
        OFCHECK_EQUAL(i, nst.top());
        nst.pop();
    }
}


/*
**
** CVS/RCS Log:
** $Log: tstack.cc,v $
** Revision 1.1  2011-05-25 10:05:57  uli
** Imported oftest and converted existing tests to oftest.
**
** Revision 1.11  2010-10-14 13:15:16  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.10  2006/08/14 16:42:48  meichel
** Updated all code in module ofstd to correctly compile if the standard
**   namespace has not included into the global one with a "using" directive.
**
** Revision 1.9  2005/12/08 15:49:11  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.8  2004/01/16 10:37:23  joergr
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.7  2002/04/16 13:37:01  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.6  2001/06/01 15:51:41  meichel
** Updated copyright header
**
** Revision 1.5  2000/03/08 16:36:08  meichel
** Updated copyright header.
**
** Revision 1.4  2000/03/03 14:02:53  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.3  1998/11/27 12:42:11  joergr
** Added copyright message to source files and changed CVS header.
**
**
**
*/
