/*
 * 
 * Author: Gerd Ehlers      Created:  04-17-94
 *                          Modified: 01-29-95
 * 
 * Module: tagtst.cc
 * 
 * Purpose:
 * testing the class DcmTag
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stream.h>

#include "dctag.h"
#include "dcdebug.h"

extern char *VRTypesName[];

void printTag( DcmTag &tag, char *name )
{
Bdebug((2, "tsttag:printTag(DcmTag&,*name)"));

    char *tmp = "unknown_VR";
    if ( tag.getVR() >= 0 )
        tmp = VRTypesName[ tag.getVR() ];
debug(( 2, "%s. Tag        =(0x%4.4x,0x%4.4x)", name,
          tag.getGTag(), tag.getETag() ));
debug(( 2, "%s.getXTag()   =[%d]", name, tag.getXTag() ));
debug(( 2, "%s.getVR()     =[%d]=\"%s\"", name, tag.getVR(), tmp ));
debug(( 2, "%s.getTagName()=\"%s\"", name, tag.getTagName() ));
debug(( 2, "%s.error()     =[%d]", name, tag.error() ));
Edebug(());

}


main()
{
Bdebug((3, "tsttag:main()"));

    DcmTag t1( 0x0000, 0x0000 );
    DcmTag t2( ET_LengthtoEnd0000 );
    DcmTag t3( "Message Id" );
    DcmTag t4( "Item" );

    printTag( t1, "t1" );
    printTag( t2, "t2" );
    printTag( t3, "t3" );
    printTag( t4, "t4" );
Edebug(());

}

