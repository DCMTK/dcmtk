// bsptag.cc    04-17-94  until  11-23-94

#include <stdio.h>
#include "dctag.h"
#include "dcvr.h"
                
void printTag( DcmTag &tag, char *name )
{
    printf( "printTag:%s (%4.4x,%4.4x) vr(% 2.2d)=\"%-10s\" err(%d) \"%s\"\n",
            name,
            tag.getGTag(),
            tag.getETag(),
            tag.getVR(),
            DcmVR( tag.getVR() ).getVRName(),
            tag.error(),
            tag.getTagName() );
}


main()
{
    DcmTag t1( 0x0000, 0x0000 );         // tag doesn't exist
    DcmTag t2( "Message Id" );           // last character is wrong spelled
    DcmTag t3( ET_LengthtoEnd0000 );
    DcmTag t4( "File-set ID" );
    DcmTag t5( t4 );                     // copy of t4

    printTag( t1, "t1 w" );
    printTag( t2, "t2 w" );
    printTag( t3, " t3 " );
    printTag( t4, " t4 " );
    printTag( t5, "=t4 " );
}

