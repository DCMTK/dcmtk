/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrsh.cc
 * 
 * Purpose:
 * Implementation of class DcmShortString
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#include "dcvrsh.h"
#include "dcdebug.h"



// ********************************


DcmShortString::DcmShortString( DcmTag &tag )
    : DcmCharString( tag )
{
Bdebug((5, "dcvrsh:DcmShortString::DcmShortString(DcmTag&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmShortString::DcmShortString( DcmTag &tag,
                                T_VR_UL len,
                                iDicomStream *iDStream )
    : DcmCharString( tag, len, iDStream )
{
Bdebug((5, "dcvrsh:DcmShortString::DcmShortString(DcmTag&,len=%ld,*iDS)",
           len ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmShortString::DcmShortString( const DcmObject &oldObj )
    : DcmCharString( oldObj, EVR_SH )
{
Bdebug((5, "dcvrsh:DcmShortString::DcmShortString(DcmObject&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmShortString::DcmShortString( const DcmShortString &newSH )
    : DcmCharString( newSH, EVR_SH )
{
Bdebug((5, "dcvrsh:DcmShortString::DcmShortString(DcmShortString&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmShortString::~DcmShortString()
{
Bdebug((5, "dcvrsh:DcmShortString::~DcmShortString()" ));
Edebug(());

}


// ********************************


EVR DcmShortString::ident() const
{
    return EVR_SH;
}


// ********************************


