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
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#include "dcvrst.h"
#include "dcdebug.h"



// ********************************


DcmShortText::DcmShortText( DcmTag &tag )
    : DcmCharString( tag )
{
Bdebug((5, "dcvrst:DcmShortText::DcmShortText(DcmTag&)" ));

    maxLength = 1024;
Edebug(());

}


// ********************************


DcmShortText::DcmShortText( DcmTag &tag,
                            T_VR_UL len,
                            iDicomStream *iDStream )
    : DcmCharString( tag, len, iDStream )
{
Bdebug((5, "dcvrst:DcmShortText::DcmShortText(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 1024;
Edebug(());

}


// ********************************


DcmShortText::DcmShortText( const DcmObject &oldObj )
    : DcmCharString( oldObj, EVR_ST )
{
Bdebug((5, "dcvrst:DcmShortText::DcmShortText(DcmObject&)" ));

    maxLength = 1024;
Edebug(());

}


// ********************************


DcmShortText::DcmShortText( const DcmShortText &newST )
    : DcmCharString( newST, EVR_ST )
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


EVR DcmShortText::ident() const
{
    return EVR_ST;
}


// ********************************


T_VR_UL DcmShortText::getVM()
{
    return 1L;
}


// ********************************


