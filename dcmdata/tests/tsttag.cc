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
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stream.h>

#include "dctag.h"
#include "dcdebug.h"
#include "dcdeftag.h"


void printTag( DcmTag &tag, char *name )
{
Bdebug((2, "tsttag:printTag(DcmTag&,*name)"));

    const char* tmp = NULL;
    tmp = tag.getVR().getVRName();
debug(( 2, "%s. Tag        =(0x%4.4x,0x%4.4x)", name,
          tag.getGTag(), tag.getETag() ));
debug(( 2, "%s.getXTag()   =[(%x,%x)]", name, tag.getGTag(), tag.getETag() ));
debug(( 2, "%s.getEVR()     =[%d]=\"%s\"", name, tag.getEVR(), tmp ));
debug(( 2, "%s.getTagName()=\"%s\"", name, tag.getTagName() ));
debug(( 2, "%s.error()     =[%d]", name, tag.error() ));
Edebug(());

}


main()
{
Bdebug((3, "tsttag:main()"));

    DcmTag t1( 0x0000, 0x0000 );
    DcmTag t2( DCM_CommandGroupLengthToEnd );
    DcmTag t3( DCM_MessageID );
    DcmTag t4( DCM_Item );

    printTag( t1, "t1" );
    printTag( t2, "t2" );
    printTag( t3, "t3" );
    printTag( t4, "t4" );
Edebug(());

}

