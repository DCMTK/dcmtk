/*
 * 
 * Author: Gerd Ehlers      Created:  04-09-94
 *                          Modified: 01-29-95
 *
 * Module: listtst.cc
 * 
 * Purpose:
 * testing the list-class
 * 
 * 
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stream.h>
#include "dclist.h"
#include "dcvrus.h"
#include "dcdebug.h"
#include "dcdeftag.h"

#define cadd 1000


void printlist( char *comment, DcmList &l )
{
Bdebug((4, "tstlist:printlist( char*, DcmList& )" ));
debug(( 5, "[%s]", comment ));

    DcmObject *obj;
    cout << comment;
    l.seek( ELP_first );
    do {
        obj = l.get();
debug(( 5, "element: [%x]", obj ));

        cout << (int)obj->getLength() - cadd << " ";
    } while ( l.seek( ELP_next ) );
    cout << endl;
Edebug(());

}


void eraselist( DcmList &l )
{
    l.seek( ELP_first );
    while ( l.valid() ) {
       delete l.remove();
    }
}


main()
{
    SetDebugLevel(( 0 ));
    DcmList l;
    DcmTag owntag(DCM_Item);
    int i;
Bdebug((3, "tstlist:main()" ));

    for (i=1; i<=10; i++)
        l.append( new DcmUnsignedShort( owntag, cadd + i*i, NULL ) );
    printlist( "List 1..100:\n", l );

    for (i=1; i<=10; i++)
        l.prepend( new DcmUnsignedShort( owntag, cadd + -i*i, NULL ) );
    printlist( "List -100..100:\n", l );

    l.prepend( new DcmUnsignedShort( owntag, cadd + -111, NULL ) );
    printlist( "Prepend before pos. 0:\n", l );

    l.seek( ELP_first );
    for (i=1; i<=7; i++)
        l.seek( ELP_next );
    l.insert( new DcmUnsignedShort( owntag, cadd + 21, NULL ) );
    l.insert( new DcmUnsignedShort( owntag, cadd + 22, NULL ) );
    l.insert( new DcmUnsignedShort( owntag, cadd + 23, NULL ) );
    printlist( "Insert at pos. 7:\n", l );

    l.seek( ELP_first );
    for (i=1; i<=4; i++)
        l.seek( ELP_next );
    l.insert( new DcmUnsignedShort( owntag, cadd + 51, NULL ), ELP_prev );
    l.insert( new DcmUnsignedShort( owntag, cadd + 52, NULL ), ELP_prev );
    l.insert( new DcmUnsignedShort( owntag, cadd + 53, NULL ), ELP_prev );
    printlist( "Insert before pos. 4:\n", l );

    cout << "Element 25: ";
    cout << l.seek_to(25)->getLength() << endl;
    l.seek_to( 26 );
    l.insert( new DcmUnsignedShort( owntag, cadd + 99, NULL ), ELP_prev );
    printlist( "Direct insert before pos. 26:\n", l );

    eraselist( l );
Edebug(());

}

