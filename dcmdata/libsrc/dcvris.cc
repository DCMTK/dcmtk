/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvris.cc
 * 
 * Purpose:
 * Implementation of class DcmIntegerString
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvris.h"
#include "dcdebug.h"


// ********************************


DcmIntegerString::DcmIntegerString( const DcmTag &tag,
                                    T_VR_UL len,
                                    iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvris:DcmIntegerString::DcmIntegerString(DcmTag&,len=%ld,*iDS)",
           len ));

    maxLength = 12;
Edebug(());

}


// ********************************


DcmIntegerString::DcmIntegerString( const DcmIntegerString& old )
    : DcmByteString( old, EVR_IS )
{
Bdebug((5, "dcvris:DcmIntegerString::DcmIntegerString(DcmIntegerString&)" ));

    maxLength = 12;
Edebug(());

}


// ********************************


DcmIntegerString::~DcmIntegerString()
{
Bdebug((5, "dcvris:DcmIntegerString::~DcmIntegerString()" ));
Edebug(());

}


// ********************************


DcmEVR DcmIntegerString::ident() const
{
    return EVR_IS;
}


// ********************************


