/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrlo.cc
 * 
 * Purpose:
 * Implementation of class DcmLongString
 * 
 * 
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrlo.h"
#include "dcdebug.h"


// ********************************


DcmLongString::DcmLongString( const DcmTag &tag,
                              T_VR_UL len,
                              iDicomStream *iDStream )
    : DcmCharString( tag, len, iDStream )
{
Bdebug((5, "dcvrlo:DcmLongString::DcmLongString(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmLongString::DcmLongString( const DcmLongString& old )
    : DcmCharString( old, EVR_LO )
{
Bdebug((5, "dcvrlo:DcmLongString::DcmLongString(DcmLongString&)" ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmLongString::~DcmLongString()
{
Bdebug((5, "dcvrlo:DcmLongString::~DcmLongString()" ));
Edebug(());

}


// ********************************


DcmEVR DcmLongString::ident() const
{
    return EVR_LO;
}


// ********************************


