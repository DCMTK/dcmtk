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
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrcs.h"
#include "dcdebug.h"



// ********************************


DcmCodeString::DcmCodeString( const DcmTag &tag,
                              T_VR_UL len,
                              iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvrcs:DcmCodeString::DcmCodeString(DcmTag&,len=%ld,*iDS)", len ));

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


DcmEVR DcmCodeString::ident() const
{
    return EVR_CS;
}


// ********************************


