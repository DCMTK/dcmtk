/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrda.cc
 * 
 * Purpose:
 * Implementation of class DcmDate
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrda.h"
#include "dcdebug.h"


// ********************************


DcmDate::DcmDate( const DcmTag &tag,
                  T_VR_UL len,
                  iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvrda:DcmDate::DcmDate(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 10;
Edebug(());

}


// ********************************


DcmDate::DcmDate( const DcmDate &newDA )
    : DcmByteString( newDA, EVR_DA )
{
Bdebug((5, "dcvrda:DcmDate::DcmDate(DcmDate&)" ));

    maxLength = 10;
Edebug(());

}


// ********************************


DcmDate::~DcmDate()
{
Bdebug((5, "dcvrda:DcmDate::~DcmDate()" ));
Edebug(());

}


// ********************************


DcmEVR DcmDate::ident() const
{
    return EVR_DA;
}


// ********************************


