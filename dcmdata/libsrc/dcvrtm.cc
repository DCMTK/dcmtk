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
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#include "dcvrtm.h"
#include "dcdebug.h"



// ********************************


DcmTime::DcmTime( DcmTag &tag )
    : DcmByteString( tag )
{
Bdebug((5, "dcvrtm:DcmTime::DcmTime(DcmTag&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmTime::DcmTime( DcmTag &tag,
                  T_VR_UL len,
                  iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvrtm:DcmTime::DcmTime(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmTime::DcmTime( const DcmObject &oldObj )
    : DcmByteString( oldObj, EVR_TM )
{
Bdebug((5, "dcvrtm:DcmTime::DcmTime(DcmObject&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmTime::DcmTime( const DcmTime &newTM )
    : DcmByteString( newTM, EVR_TM )
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


EVR DcmTime::ident() const
{
    return EVR_TM;
}


// ********************************


