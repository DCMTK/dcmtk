/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrst.cc
 * 
 * Purpose:
 * Implementation of class DcmShortText
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:        $State: Exp $
 *
 */


#include "dcvrst.h"
#include "dcdebug.h"


// ********************************


DcmShortText::DcmShortText( const DcmTag &tag,
                            T_VR_UL len,
                            iDicomStream *iDStream )
    : DcmCharString( tag, len, iDStream )
{
Bdebug((5, "dcvrst:DcmShortText::DcmShortText(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 1024;
Edebug(());

}


// ********************************


DcmShortText::DcmShortText( const DcmShortText& old )
    : DcmCharString( old, EVR_ST )
{
Bdebug((5, "dcvrst:DcmShortText::DcmShortText(DcmShortText&)" ));

    maxLength = 1024;
Edebug(());

}


// ********************************


DcmShortText::~DcmShortText()
{
Bdebug((5, "dcvrst:DcmShortText::~DcmShortText()" ));
Edebug(());

}


// ********************************


DcmEVR DcmShortText::ident() const
{
    return EVR_ST;
}


// ********************************


T_VR_UL DcmShortText::getVM()
{
    return 1L;
}


// ********************************


