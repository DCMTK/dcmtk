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
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dcvrdt.h"
#include "dcdebug.h"



// ********************************


DcmDateTime::DcmDateTime( DcmTag &tag )
    : DcmByteString( tag )
{
Bdebug((5, "dcvrdt:DcmDateTime::DcmDateTime(DcmTag&)" ));

    maxLength = 26;
Edebug(());

}


// ********************************


DcmDateTime::DcmDateTime( DcmTag &tag,
                          T_VR_UL len,
                          iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvrdt:DcmDateTime::DcmDateTime(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 26;
Edebug(());

}


// ********************************


DcmDateTime::DcmDateTime( const DcmObject &oldObj )
    : DcmByteString( oldObj, EVR_DT )
{
Bdebug((5, "dcvrdt:DcmDateTime::DcmDateTime(DcmObject&)" ));

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


EVR DcmDateTime::ident() const
{
    return EVR_DT;
}


// ********************************


