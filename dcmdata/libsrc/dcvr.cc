/*
 * 
 * Author: Gerd Ehlers      Created:  04-24-94
 *                          Modified: 02-07-95
 *
 * Module: dcvr.cc
 *
 * Purpose:
 * Implementation of the class DcmVR
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

#include <string.h>

#include "dcvr.h"
#include "dcdebug.h"

extern char *VRTypesName[];
extern short VRTypesName_MaxLen;
extern short DIM_OF_VRTypes;



// ********************************


DcmVR::DcmVR( EVR vr )
{
Bdebug((7, "dcvr:DcmVR::DcmVR(vr=%d)", vr ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( vr >= 0 && vr < (EVR)DIM_OF_VRTypes )
    {
        vrType  = vr;
        errorFlag = EC_Normal;
    }
    else
    {
        vrType  = EVR_UNKNOWN;
        errorFlag = EC_InvalidVR;
    }
Edebug(());

}


// ********************************


DcmVR::DcmVR( char *vrname )
{
Bdebug((7, "dcvr:DcmVR::DcmVR(char*)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    if ( vrname == (char*)NULL )
    {
        vrType = EVR_UNKNOWN;
        errorFlag = EC_InvalidTag;
    }
    else
    {
        int i = 0;
        while ((i < DIM_OF_VRTypes) && (memcmp(VRTypesName[i], vrname, 2) != 0))
            i++;
debug(( 7, "found index i=[%d]", i));

        if ((i < DIM_OF_VRTypes) && (memcmp(VRTypesName[i], vrname, 2) == 0))
        {
            vrType = (EVR)i;
            errorFlag = EC_Normal;
        }
        else
        {
            vrType = EVR_UNKNOWN;
            errorFlag = EC_InvalidTag;
        }
    }
Edebug(());

}


// ********************************


DcmVR::DcmVR( const DcmVR &newvr )
{
Bdebug((7, "dcvr:DcmVR::DcmVR(DcmVR&)" ));
debug(( 8, "Object pointer this=0x%p", this ));

    vrType    = newvr.vrType;
    errorFlag = newvr.errorFlag;
Edebug(());

}


// ********************************


DcmVR::~DcmVR()
{
Bdebug((7, "dcvr:DcmVR::~DcmVR()" ));
debug(( 8, "Object pointer this=0x%p", this ));
Edebug(());

}


// ********************************


DcmVR & DcmVR::operator = ( const EVR &newvr )
{
Bdebug((6, "dcvr:DcmVR::= (newvr=%d)", newvr ));

    if ( newvr >= 0 && newvr < (EVR)DIM_OF_VRTypes )
    {
        vrType  = newvr;
        errorFlag = EC_Normal;
    }
    else
    {
        vrType  = EVR_UNKNOWN;
        errorFlag = EC_InvalidVR;
    }
Edebug(());

    return *this;
}


// ********************************


DcmVR & DcmVR::operator = ( const char *vrname )
{
Bdebug((6, "dcvr:DcmVR::= (char*)" ));

    if ( vrname == (char*)NULL )
    {
        vrType = EVR_UNKNOWN;
        errorFlag = EC_InvalidTag;
    }
    else
    {
        int i = 0;
        while ((i < DIM_OF_VRTypes) && (memcmp(VRTypesName[i], vrname, 2) != 0))
            i++;
debug(( 7, "found index i=[%d]", i));

        if ((i < DIM_OF_VRTypes) && (memcmp(VRTypesName[i], vrname, 2) == 0))
        {
            vrType = (EVR)i;
            errorFlag = EC_Normal;
        }
        else
        {
            vrType = EVR_UNKNOWN;
            errorFlag = EC_InvalidTag;
        }
    }
Edebug(());

    return *this;
}


// ********************************


DcmVR & DcmVR::operator = ( const DcmVR &newvr )
{
Bdebug((6, "dcvr:DcmVR::= (DcmVR&)" ));

    if ( this != &newvr )
    {
        vrType    = newvr.vrType;
        errorFlag = newvr.errorFlag;
    }
    else
    {
debug(( 1, "dcvr:DcmVR::DcmVR(DcmVR&)  Warning: self-assignment" ));

    }
Edebug(());

    return *this;
}


// ********************************


EVR DcmVR::getVR()
{
    errorFlag = (vrType == EVR_UNKNOWN) ? EC_InvalidTag : EC_Normal;
    return vrType;
}


// ********************************


EVR DcmVR::getValidVR()
{
    EVR vr;
    errorFlag = EC_Normal;
    switch ( vrType )
    {
        case EVR_AE :
        case EVR_AS :
        case EVR_AT :
        case EVR_CS :
        case EVR_DA :
        case EVR_DS :
        case EVR_DT :
        case EVR_FL :
        case EVR_FD :
        case EVR_IS :
        case EVR_LO :
        case EVR_LT :
        case EVR_OB :
        case EVR_OW :
        case EVR_PN :
        case EVR_SH :
        case EVR_SL :
            vr = vrType;
            break;
        case EVR_squ:
        case EVR_SQ :
            vr = EVR_SQ;
            break;
        case EVR_SS :
        case EVR_ST :
        case EVR_TM :
        case EVR_UI :
            vr = vrType;
            break;
        case EVR_up :
        case EVR_UL :
            vr = EVR_UL;
            break;
        case EVR_xs :
        case EVR_ux :
        case EVR_US :
            vr = EVR_US;
            break;
        default :
            vr = EVR_OB;    // behandle unbekannte Typen genauso wie OtherByte
            errorFlag = EC_InvalidTag;
            break;
    }
    return vr;
}


// ********************************


char* DcmVR::getVRName()
{
    if ( vrType >= 0 && vrType < (EVR)DIM_OF_VRTypes )
    {
        errorFlag = EC_Normal;
        return VRTypesName[vrType];
    }
    else
    {
        errorFlag = EC_InvalidVR;
        return ERROR_VRName;
    }
}


// ********************************


char* DcmVR::getValidVRName()
{
    return VRTypesName[getValidVR()];
}


// ********************************


