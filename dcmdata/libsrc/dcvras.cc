/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvras.cc
 * 
 * Purpose:
 * Implementation of class DcmAgeString
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvras.h"
#include "dcdebug.h"



// ********************************


DcmAgeString::DcmAgeString( const DcmTag &tag,
                            T_VR_UL len,
                            iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvras:DcmAgeString::DcmAgeString(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 4;
Edebug(());

}


// ********************************


DcmAgeString::DcmAgeString( const DcmAgeString &newAS )
    : DcmByteString( newAS, EVR_AS )
{
Bdebug((5, "dcvras:DcmAgeString::DcmAgeString(DcmAgeString&)" ));

    maxLength = 4;
Edebug(());

}


// ********************************


DcmAgeString::~DcmAgeString()
{
Bdebug((5, "dcvras:DcmAgeString::~DcmAgeString()" ));
Edebug(());

}


// ********************************


DcmEVR DcmAgeString::ident() const
{
    return EVR_AS;
}


// ********************************


