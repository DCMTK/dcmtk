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
 * Revision:      $Revision: 1.1 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCSWAP_H
#define DCSWAP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"


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
