/*
 * 
 * Author: Gerd Ehlers      Created:  05-01-94
 *                          Modified: 02-07-95
 *
 * Module: dcxfer.h
 * 
 * Purpose:
 * handling of transfer syntax
 *
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#ifndef DCXFER_H
#define DCXFER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"


class DcmXfer {
    char               *xferID;
    char               *xferName;
    E_TransferSyntax   xferSyn;
    E_ByteOrder        byteOrder;
    E_VRType           vrType;
    E_JPEGEncapsulated encapsulated;
    T_VR_UL            JPEGProcess8;
    T_VR_UL            JPEGProcess12;

public:
    DcmXfer( E_TransferSyntax xfer );
    DcmXfer( char *xferName_xferID );
    DcmXfer( const DcmXfer &newXfer );
    ~DcmXfer();

    DcmXfer & operator = ( const E_TransferSyntax xfer );
    DcmXfer & operator = ( const DcmXfer &newtag );

    E_TransferSyntax getXfer();
    char             *getXferName();
    char             *getXferID();
    BOOL             isLittleEndian();
    BOOL             isBigEndian();
    BOOL             isImplicitVR();
    BOOL             isExplicitVR();
    BOOL             isEncapsulated();
    BOOL             isNotEncapsulated();
    T_VR_UL          getJPEGProcess8Bit();
    T_VR_UL          getJPEGProcess12Bit();
};


#endif // DCXFER_H

