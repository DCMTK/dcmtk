/*
 *
 *  Copyright (C) 1997-99, OFFIS
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
 *  Author:  Andreas Barth
 *
 *  Purpose: test programm for class OFStack
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 12:42:11 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/tests/tststack.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include <iostream.h>
#include "ofstack.h"

int main()
{
    OFStack<int> st;
    st.push(1);
    st.push(2);
    st.push(3);
    
    OFStack<int> nst(st);
    cout << "Output of number of Elements in st: " << st.size() << endl;
    cout << "Output and deletion of st: ";
    while(!st.empty())
    {
	cout << st.top() << " ";
	st.pop();
    }
    cout << endl;

    cout << "Output of number of Elements in copy from st: " << nst.size() << endl;
    cout << "Output and deletion of copy from st: ";
    while(!nst.empty())
    {
	cout << nst.top() << " ";
	nst.pop();
    }
    cout << endl;
}


/*
**
** CVS/RCS Log:
** $Log: tststack.cc,v $
** Revision 1.3  1998-11-27 12:42:11  joergr
** Added copyright message to source files and changed CVS header.
**
**
**
*/
