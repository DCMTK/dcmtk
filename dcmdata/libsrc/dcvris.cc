/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvris.cc
 * 
 * Purpose:
 * Implementation of class DcmIntegerString
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

#include "dcvris.h"
#include "dcdebug.h"



// ********************************


DcmIntegerString::DcmIntegerString( DcmTag &tag )
    : DcmByteString( tag )
{
Bdebug((5, "dcvris:DcmIntegerString::DcmIntegerString(DcmTag&)" ));

    maxLength = 12;
Edebug(());

}


// ********************************


DcmIntegerString::DcmIntegerString( DcmTag &tag,
                                    T_VR_UL len,
                                    iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvris:DcmIntegerString::DcmIntegerString(DcmTag&,len=%ld,*iDS)",
           len ));

    maxLength = 12;
Edebug(());

}


// ********************************


DcmIntegerString::DcmIntegerString( const DcmObject &oldObj )
    : DcmByteString( oldObj, EVR_IS )
{
Bdebug((5, "dcvris:DcmIntegerString::DcmIntegerString(DcmObject&)" ));

    maxLength = 12;
Edebug(());

}


// ********************************


DcmIntegerString::DcmIntegerString( const DcmIntegerString &newIS )
    : DcmByteString( newIS, EVR_IS )
{
Bdebug((5, "dcvris:DcmIntegerString::DcmIntegerString(DcmIntegerString&)" ));

    maxLength = 12;
Edebug(());

}


// ********************************


DcmIntegerString::~DcmIntegerString()
{
Bdebug((5, "dcvris:DcmIntegerString::~DcmIntegerString()" ));
Edebug(());

}


// ********************************


EVR DcmIntegerString::ident() const
{
    return EVR_IS;
}


// ********************************


