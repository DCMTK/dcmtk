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
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dcvrda.h"
#include "dcdebug.h"



// ********************************


DcmDate::DcmDate( DcmTag &tag )
    : DcmByteString( tag )
{
Bdebug((5, "dcvrda:DcmDate::DcmDate(DcmTag&)" ));

    maxLength = 10;
Edebug(());

}


// ********************************


DcmDate::DcmDate( DcmTag &tag,
                  T_VR_UL len,
                  iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvrda:DcmDate::DcmDate(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 10;
Edebug(());

}


// ********************************


DcmDate::DcmDate( const DcmObject &oldObj )
    : DcmByteString( oldObj, EVR_DA )
{
Bdebug((5, "dcvrda:DcmDate::DcmDate(DcmObject&)" ));

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


EVR DcmDate::ident() const
{
    return EVR_DA;
}


// ********************************


