/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrlo.cc
 * 
 * Purpose:
 * Implementation of class DcmLongString
 * 
 * 
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dcvrlo.h"
#include "dcdebug.h"



// ********************************


DcmLongString::DcmLongString( DcmTag &tag )
    : DcmCharString( tag )
{
Bdebug((5, "dcvrlo:DcmLongString::DcmLongString(DcmTag&)" ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmLongString::DcmLongString( DcmTag &tag,
                              T_VR_UL len,
                              iDicomStream *iDStream )
    : DcmCharString( tag, len, iDStream )
{
Bdebug((5, "dcvrlo:DcmLongString::DcmLongString(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmLongString::DcmLongString( const DcmObject &oldObj )
    : DcmCharString( oldObj, EVR_LO )
{
Bdebug((5, "dcvrlo:DcmLongString::DcmLongString(DcmObject&)" ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmLongString::DcmLongString( const DcmLongString &newLO )
    : DcmCharString( newLO, EVR_LO )
{
Bdebug((5, "dcvrlo:DcmLongString::DcmLongString(DcmLongString&)" ));

    maxLength = 64;
Edebug(());

}


// ********************************


DcmLongString::~DcmLongString()
{
Bdebug((5, "dcvrlo:DcmLongString::~DcmLongString()" ));
Edebug(());

}


// ********************************


EVR DcmLongString::ident() const
{
    return EVR_LO;
}


// ********************************


