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
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dcvras.h"
#include "dcdebug.h"



// ********************************


DcmAgeString::DcmAgeString( DcmTag &tag )
    : DcmByteString( tag )
{
Bdebug((5, "dcvras:DcmAgeString::DcmAgeString(DcmTag&)" ));

    maxLength = 4;
Edebug(());

}


// ********************************


DcmAgeString::DcmAgeString( DcmTag &tag,
                            T_VR_UL len,
                            iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvras:DcmAgeString::DcmAgeString(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 4;
Edebug(());

}


// ********************************


DcmAgeString::DcmAgeString( const DcmObject &oldObj )
    : DcmByteString( oldObj, EVR_AS )
{
Bdebug((5, "dcvras:DcmAgeString::DcmAgeString(DcmObject&)" ));

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


EVR DcmAgeString::ident() const
{
    return EVR_AS;
}


// ********************************


