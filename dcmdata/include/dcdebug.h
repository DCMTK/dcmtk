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
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#ifndef DCDEBUG_H
#define DCDEBUG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef DEBUG

void real_SetDebugLevel( int level );
void real_Bdebug( int lev, const char* text, ... );
void real_debug(  int lev, const char* text, ... );
void real_Vdebug( int lev, int printit, const char* text, ... );
void real_Edebug( void );

#define SetDebugLevel(param) real_SetDebugLevel param
#define Bdebug(param) real_Bdebug param
#define debug(param)  real_debug  param
#define Vdebug(param) real_Vdebug param
#define Edebug(param) real_Edebug param
#define Cdebug(cond,param) if ((cond)) { real_debug param ; }

#else  // DEBUG

#define SetDebugLevel(param)
#define Bdebug(param)
#define debug(param)
#define Vdebug(param)
#define Edebug(param)
#define Cdebug(cond,param)

#endif // DEBUG


#endif // DCDEBUG_H

