/*
 * 
 * Author: Gerd Ehlers	    Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcvrae.cc
 * 
 * Purpose:
 * Implementation of class DcmApplicationEntity
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

#include "dcvrae.h"
#include "dcdebug.h"



// ********************************


DcmApplicationEntity::DcmApplicationEntity( DcmTag &tag )
    : DcmByteString( tag )
{
Bdebug((5, "dcvrae:DcmApplicationEntity::DcmApplicationEntity(DcmTag&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmApplicationEntity::DcmApplicationEntity( DcmTag &tag,
                                            T_VR_UL len,
                                            iDicomStream *iDStream )
    : DcmByteString( tag, len, iDStream )
{
Bdebug((5, "dcvrae:DcmApplicationEntity::DcmApplicationEntity"
           "(DcmTag&,len=%ld,*iDS)", len ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmApplicationEntity::DcmApplicationEntity( const DcmObject &oldObj )
    : DcmByteString( oldObj, EVR_AE )
{
Bdebug((5, "dcvrae:DcmApplicationEntity::DcmApplicationEntity(DcmObject&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmApplicationEntity::DcmApplicationEntity( const DcmApplicationEntity &newAE )
    : DcmByteString( newAE, EVR_AE )
{
Bdebug((5, "dcvrae:DcmApplicationEntity::DcmApplicationEntity"
           "(DcmApplicationEntity&)" ));

    maxLength = 16;
Edebug(());

}


// ********************************


DcmApplicationEntity::~DcmApplicationEntity()
{
Bdebug((5, "dcvrae:DcmApplicationEntity::~DcmApplicationEntity()" ));
Edebug(());

}


// ********************************


EVR DcmApplicationEntity::ident() const
{
    return EVR_AE;
}


// ********************************


