/*
 *
 * Author: Andreas Barth    Created:  07-17-95
 *                          Modified: 07-17-95
 *
 * Module: dcswap.h
 *
 * Purpose:
 * ByteOrder functions
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCSWAP_H
#define DCSWAP_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcxfer.h"


// ********************************************************
// *** ByteOrder-functions ********************************
// ********************************************************


extern E_ByteOrder ByteOrderFlag; // definiert in dcswap.cc !!!

extern E_ByteOrder FindMachineByteOrder();

extern E_ByteOrder MachineByteOrder();

extern E_ByteOrder ConvXferToByteOrder( E_TransferSyntax ts );
extern char   *SwapN( char *Psrc, int P_width, int times = 1 );
extern T_VR_US SwapShort( T_VR_US &sval );
extern T_VR_UL SwapLong( T_VR_UL &lval );
extern T_VR_FD SwapDouble( T_VR_FD &dval );

#endif // DCSWAP_H
