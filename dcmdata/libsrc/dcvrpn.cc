/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrpn.cc
 * 
 * Purpose:
 * Implementation of class DcmPersonName
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "dcvrpn.h"
#include "dcdebug.h"


// ********************************


DcmPersonName::DcmPersonName( const DcmTag &tag,
                              T_VR_UL len,
                              iDicomStream *iDStream )
    : DcmCharString( tag, len, iDStream )
{
Bdebug((5, "dcvrpn:DcmPersonName::DcmPersonName(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmPersonName::DcmPersonName( const DcmPersonName& old )
    : DcmCharString( old, EVR_PN )
{
Bdebug((5, "dcvrpn:DcmPersonName::DcmPersonName(DcmPersonName&)" ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmPersonName::~DcmPersonName()
{
Bdebug((5, "dcvrpn:DcmPersonName::~DcmPersonName()" ));
Edebug(());

}


// ********************************


DcmEVR DcmPersonName::ident() const
{
    return EVR_PN;
}


// ********************************


