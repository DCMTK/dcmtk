/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrds.cc
 * 
 * Purpose:
 * Implementation of class DcmDecimalString
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

#include "dcvrds.h"
#include "dcdebug.h"



// ********************************


DcmDecimalString::DcmDecimalString( DcmTag &tag )
    : DcmByteString( tag )
{
Bdebug((5, "dcvrds:DcmDecimalString::DcmDecimalString(DcmTag&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmDecimalString::DcmDecimalString( DcmTag &tag,
                                    T_VR_UL len,
                                    iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvrds:DcmDecimalString::DcmDecimalString(DcmTag&,len=%ld,*iDS)",
           len ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmDecimalString::DcmDecimalString( const DcmObject &oldObj )
    : DcmByteString( oldObj, EVR_DS )
{
Bdebug((5, "dcvrds:DcmDecimalString::DcmDecimalString(DcmObject&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmDecimalString::DcmDecimalString( const DcmDecimalString &newDS )
    : DcmByteString( newDS, EVR_DS )
{
Bdebug((5, "dcvrds:DcmDecimalString::DcmDecimalString(DcmDecimalString&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmDecimalString::~DcmDecimalString()
{
Bdebug((5, "dcvrds:DcmDecimalString::~DcmDecimalString()" ));
Edebug(());

}


// ********************************


EVR DcmDecimalString::ident() const
{
    return EVR_DS;
}


// ********************************


