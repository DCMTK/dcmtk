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
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "dcdebug.h"

#ifndef DEBUGLEVEL
#define DEBUGLEVEL 3
#endif

// latest modify for replacement of msg_table allocation with static tables
#define DEBUG_STATIC_MSG

// show debugLevel number at beginning of each line
#define DEBUG_SHOW_LEVEL

// adjust bit_table[], too:
#define max_tab_level 30

// the following define is useless without defining DEBUG_STATIC_MSG:
#define max_msg_length 200

typedef char* pchar;

static int tab_level = 0;
static int   tab_table[ max_tab_level+1 ];

#ifdef DEBUG_STATIC_MSG
static char  msg_table[ max_tab_level+1 ][ max_msg_length+1 ];
#else
static pchar msg_table[ max_tab_level+1 ];
static int   bit_table[] = { 0,0,0,0,0, 0,0,0,0,0,
                             0,0,0,0,0, 0,0,0,0,0,
                             0,0,0,0,0, 0,0,0,0,0,
                             0 };
#endif

int debugLevel = DEBUGLEVEL;



// ********************************


void printtab( int lev )
{
#ifdef DEBUG_SHOW_LEVEL
    if ( debugLevel >= 9 )                 // for debugging the debug-routines
        printf("%d ", lev );
#endif
    for ( int i=0; i < tab_level; i++ )
    {
        printf("  ");
    }
}


// ********************************


void real_SetDebugLevel( int level )
{
    debugLevel = level;
}


// ********************************


void real_Bdebug( int lev, const char* text, ... )
{
#ifdef DEBUG_STATIC_MSG
    strncpy( msg_table[ tab_level ], text, max_msg_length );
    msg_table[ tab_level ][ max_msg_length ] = '\0';
#else
    if ( bit_table[ tab_level ] != 0 )   // tritt ein, falls
        delete msg_table[ tab_level ];   // tab_level == max_tab_level

    bit_table[ tab_level ] = 1;
    msg_table[ tab_level ] = new char[ strlen(text) + 1 ];
    if ( debugLevel >= 9 )                        // for debugging the
    {                                             // debug-routines
        printtab( lev );
        printf("real_Bdebug(): 0x%p allocated\n",
               msg_table[ tab_level ] );
    }
    strcpy( msg_table[ tab_level ], text );
#endif

#ifdef HAVE_VPRINTF
    va_list argptr;
    if ( lev <= debugLevel )
    {
        printtab( lev );
        printf( "BEGIN-" );
        va_start( argptr, text );
        vprintf( text, argptr );
        va_end( argptr );
        printf( "\n" );
    }
#elif HAVE_DOPRNT
#else
	I need vprintf or doprnt
#endif
    tab_table[tab_level] = lev;
    if ( tab_level < max_tab_level )
        tab_level++;
    else
    {
        printtab( lev );
        printf( "ERROR-debug:real_Bdebug()--max TAB-LEVEL reached--\n" );
    }
}


// ********************************


void real_debug( int lev, const char* text, ... )
{
#ifdef HAVE_VPRINTF
    va_list argptr;
    if ( lev <= debugLevel )
    {
        printtab( lev );
        va_start( argptr, text );
        vprintf( text, argptr );
        va_end( argptr );
        printf( "\n" );
    }
#elif HAVE_DOPRNT
#else
	I need vprintf or doprnt
#endif
}


// ********************************


void real_Vdebug( int lev, int printit, const char* text, ... )
{
#ifdef HAVE_VPRINTF
    va_list argptr;
    if ( lev <= debugLevel && printit )
    {
        printtab( lev );
        va_start( argptr, text );
        vprintf( text, argptr );
        va_end( argptr );
        printf( "\n" );
    }
#elif HAVE_DOPRNT
#else
	I need vprintf or doprnt
#endif
}


// ********************************


void real_Edebug( void )
{
    if ( tab_level > 0 )
    {
        tab_level--;
        if ( tab_table[ tab_level ] <= debugLevel )
        {
            printtab( tab_table[ tab_level ] );
            if ( msg_table[ tab_level ] != (char*)NULL )
                printf( "END---%12.12s...\n", msg_table[ tab_level ] );
            else
                printf( "END---\n" );
        }

#ifdef DEBUG_STATIC_MSG
        msg_table[ tab_level ][ 0 ] = '\0';
#else
        if ( bit_table[ tab_level ] != 0 )
        {
            if ( debugLevel >= 9 )         // for debugging the debug-routines
            {
                printtab( tab_table[ tab_level ] );
                printf("real_Edebug(): delete 0x%p\n",
                       msg_table[ tab_level ] );
            }
            delete msg_table[ tab_level ];
            msg_table[ tab_level ] = NULL;
            bit_table[ tab_level ] = 0;
        }
#endif

    }
    else // tab_level <= 0
    {
        printtab( 9 );
        printf( "END--- debug-error: Edebug without leading Bdebug\n" );
    }
}


// ********************************


