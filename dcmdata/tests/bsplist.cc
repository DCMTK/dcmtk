// bsplist.cc    04-09-94  until  11-23-94

#include "dclist.h"
#include "dctk.h"
#include <stream.h>


void printlist( char *str, DcmList &l )
{
    DcmObject *obj;
    Sint32 sint = 0;
    cout << endl << str;
    l.seek( ELP_first );
    do {
        obj = l.get();
	((DcmSignedLong*)obj)->getSint32(sint);
        cout << sint  << " ";
    } while ( l.seek( ELP_next ) );
    cout << endl;
}


void eraselist( DcmList &l )
{
    l.seek( ELP_first );
    while ( l.valid() )
       delete l.remove();
}

DcmObject* createNumber(Sint32 num )
{
    DcmTag tag(DCM_CommandGroupLength);
    DcmSignedLong* sl = new DcmSignedLong( tag );
    sl->putSint32(num);
    return sl;
}

main()
{
    DcmList l;
    int i;

    for (i=4; i<=10; i++)
        l.append( createNumber( i*i ) );
    printlist( "List 16..100:\n", l );

    for (i=4; i<=10; i++)
        l.prepend( createNumber( -i*i ) );
    printlist( "List -100..100:\n", l );

    l.prepend( createNumber( -111 ) );
    printlist( "Prepend before pos. 0:\n", l );

    l.seek( ELP_first );
    for (i=1; i<=7; i++)
        l.seek( ELP_next );
    l.insert( createNumber( 21 ) );
    l.insert( createNumber( 22 ) );
    l.insert( createNumber( 23 ) );
    printlist( "Insert at pos. 7:\n", l );

    l.seek( ELP_first );
    for (i=1; i<=4; i++)
        l.seek( ELP_next );
    l.insert( createNumber( 51 ), ELP_prev );
    l.insert( createNumber( 52 ), ELP_prev );
    l.insert( createNumber( 53 ), ELP_prev );
    printlist( "Insert before pos. 4:\n", l );

    cout << "Element (19) = ";
    Sint32 sint = 0;
    ((DcmSignedLong*)l.seek_to(19))->getSint32(sint);
    cout << sint << endl;

    l.seek_to( 20 );
    l.insert( createNumber( 99 ), ELP_prev );
    printlist( "Direct insert before pos. 20:\n", l );

    eraselist( l );
}

