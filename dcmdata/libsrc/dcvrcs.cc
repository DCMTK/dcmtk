/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrcs.cc
 * 
 * Purpose:
 * Implementation of class DcmCodeString
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

#include "dcvrcs.h"
#include "dcdebug.h"



// ********************************


DcmCodeString::DcmCodeString( DcmTag &tag )
    : DcmByteString( tag )
{
Bdebug((5, "dcvrcs:DcmCodeString::DcmCodeString(DcmTag&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmCodeString::DcmCodeString( DcmTag &tag,
                              T_VR_UL len,
                              iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvrcs:DcmCodeString::DcmCodeString(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmCodeString::DcmCodeString( const DcmObject &oldObj )
    : DcmByteString( oldObj, EVR_CS )
{
Bdebug((5, "dcvrcs:DcmCodeString::DcmCodeString(DcmObject&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmCodeString::DcmCodeString( const DcmCodeString &newCS )
    : DcmByteString( newCS, EVR_CS )
{
Bdebug((5, "dcvrcs:DcmCodeString::DcmCodeString(DcmCodeString&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmCodeString::~DcmCodeString()
{
Bdebug((5, "dcvrcs:DcmCodeString::~DcmCodeString()" ));
Edebug(());

}


// ********************************


EVR DcmCodeString::ident() const
{
    return EVR_CS;
}


// ********************************


