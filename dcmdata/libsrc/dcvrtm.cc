/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrtm.cc
 * 
 * Purpose:
 * Implementation of class DcmTime
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "dcvrtm.h"
#include "dcdebug.h"


// ********************************


DcmTime::DcmTime( const DcmTag &tag,
                  T_VR_UL len,
                  iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvrtm:DcmTime::DcmTime(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmTime::DcmTime( const DcmTime& old )
    : DcmByteString( old, EVR_TM )
{
Bdebug((5, "dcvrtm:DcmTime::DcmTime(DcmTime&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmTime::~DcmTime()
{
Bdebug((5, "dcvrtm:DcmTime::~DcmTime()" ));
Edebug(());

}


// ********************************


DcmEVR DcmTime::ident() const
{
    return EVR_TM;
}


// ********************************


