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
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrds.h"
#include "dcdebug.h"


// ********************************


DcmDecimalString::DcmDecimalString( const DcmTag &tag,
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


DcmEVR DcmDecimalString::ident() const
{
    return EVR_DS;
}


// ********************************


