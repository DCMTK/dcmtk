/*
 * 
 * Author: Gerd Ehlers      Created:  04-24-94
 *                          Modified: 02-07-95
 * 
 * Module: dcvr.h
 *
 * Purpose:
 * Definition of the class DcmVR
 *
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */

#ifndef DCVR_H
#define DCVR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcdicenu.h"


#define ERROR_VRName "INVALID VR"



class DcmVR {
    EVR     vrType;
    E_Condition errorFlag;

public:
    DcmVR( EVR vr );
    DcmVR( char *vrname );
    DcmVR( const DcmVR &newvr );
    ~DcmVR();

    DcmVR & operator = ( const EVR &newvr );
    DcmVR & operator = ( const char *vrname );
    DcmVR & operator = ( const DcmVR &newvr );

    EVR         getVR();
    EVR         getValidVR();
    char*       getVRName();
    char*       getValidVRName();

    E_Condition error() { return errorFlag; }

};


#endif // DCVR_H

