/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrlt.cc
 * 
 * Purpose:
 * Implementation of class DcmLongText
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

#include "dcvrlt.h"
#include "dcdebug.h"



// ********************************


DcmLongText::DcmLongText( DcmTag &tag )
    : DcmCharString( tag )
{
Bdebug((5, "dcvrlt:DcmLongText::DcmLongText(DcmTag&)" ));

    maxLength = 10240;
Edebug(());

}


// ********************************


DcmLongText::DcmLongText( DcmTag &tag,
                          T_VR_UL len,
                          iDicomStream *iDStream )
    : DcmCharString( tag, len, iDStream )
{
Bdebug((5, "dcvrlt:DcmLongText::DcmLongText(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 10240;
Edebug(());

}


// ********************************


DcmLongText::DcmLongText( const DcmObject &oldObj )
    : DcmCharString( oldObj, EVR_LT )
{
Bdebug((5, "dcvrlt:DcmLongText::DcmLongText(DcmObject&)" ));

    maxLength = 10240;
Edebug(());

}


// ********************************


DcmLongText::DcmLongText( const DcmLongText &newLT )
    : DcmCharString( newLT, EVR_LT )
{
Bdebug((5, "dcvrlt:DcmLongText::DcmLongText(DcmLongText&)" ));

    maxLength = 10240;
Edebug(());

}


// ********************************


DcmLongText::~DcmLongText()
{
Bdebug((5, "dcvrlt:DcmLongText::~DcmLongText()" ));
Edebug(());

}


// ********************************


EVR DcmLongText::ident() const
{
    return EVR_LT;
}


// ********************************


T_VR_UL DcmLongText::getVM()
{
    return 1L;
}


// ********************************


