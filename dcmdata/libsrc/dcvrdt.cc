/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrdt.cc
 * 
 * Purpose:
 * Implementation of class DcmDateTime
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrdt.h"
#include "dcdebug.h"



// ********************************


DcmDateTime::DcmDateTime( const DcmTag &tag,
                          T_VR_UL len,
                          iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvrdt:DcmDateTime::DcmDateTime(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 26;
Edebug(());

}


// ********************************


DcmDateTime::DcmDateTime( const DcmDateTime &newDT )
    : DcmByteString( newDT, EVR_DT )
{
Bdebug((5, "dcvrdt:DcmDateTime::DcmDateTime(DcmDateTime&)" ));

    maxLength = 26;
Edebug(());

}


// ********************************


DcmDateTime::~DcmDateTime()
{
Bdebug((5, "dcvrdt:DcmDateTime::~DcmDateTime()" ));
Edebug(());

}


// ********************************


DcmEVR DcmDateTime::ident() const
{
    return EVR_DT;
}


// ********************************


