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
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "dcvrsh.h"
#include "dcdebug.h"


// ********************************


DcmShortString::DcmShortString( const DcmTag &tag,
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


DcmShortString::DcmShortString( const DcmShortString& old )
    : DcmCharString( old, EVR_SH )
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


DcmEVR DcmShortString::ident() const
{
    return EVR_SH;
}


// ********************************


