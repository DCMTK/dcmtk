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
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrlt.h"
#include "dcdebug.h"


// ********************************


DcmLongText::DcmLongText( const DcmTag &tag,
                          T_VR_UL len,
                          iDicomStream *iDStream )
    : DcmCharString( tag, len, iDStream )
{
Bdebug((5, "dcvrlt:DcmLongText::DcmLongText(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 10240;
Edebug(());

}


// ********************************


DcmLongText::DcmLongText( const DcmLongText& old )
    : DcmCharString( old, EVR_LT )
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


DcmEVR DcmLongText::ident() const
{
    return EVR_LT;
}


// ********************************


T_VR_UL DcmLongText::getVM()
{
    return 1L;
}


// ********************************


