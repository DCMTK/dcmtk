/*
 * 
 * Author: Gerd Ehlers      Created:  03-28-94
 *                          Modified: 02-07-95
 *
 * Module: dcdebug.cc
 * 
 * Purpose:
 * Print debug information
 * 
 * 
 * Last Update:   $Author: andreas $
 * Revision:      $Revision: 1.3 $
 * Status:        $State: Exp $
 *
 */


#ifndef DCDEBUG_H
#define DCDEBUG_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef DEBUG

#include <stdio.h>
extern int DcmDebugLevel;
extern FILE * DcmDebugDevice;

void debug_print(const char* text, ... );

// Set the output device for debugging messages
#define SetDebugDevice(device) DcmDebugDevice = (device);

// Set the debug level
#define SetDebugLevel(level) DcmDebugLevel = (level);

// debug prints a debug message in param if lev <= DcmDebugLevel. param has the
// format of the printf parameters (with round brackets)!
#define debug(lev, param)  { if ((lev) <= DcmDebugLevel) { fprintf(DcmDebugDevice, __FILE__ ", LINE %d:", __LINE__); debug_print param ; } }

// Cdebug does the same as debug but only if a condition cond is TRUE
#define Cdebug(lev, cond, param) { if ((lev) <= DcmDebugLevel && (cond)) { fprintf(DcmDebugDevice, __FILE__ ", LINE %d:", __LINE__); debug_print param; } }

#else  // DEBUG

#define SetDebugDevice(device)
#define SetDebugLevel(param)
#define debug(lev, param)
#define Cdebug(lev, cond, param)

#endif // DEBUG


#endif // DCDEBUG_H

