/*
**
** Author: Andrew Hewett	Created: 4.11.95
** Kuratorium OFFIS e.V.
**
** Module: dcerror.h
**
** Purpose:
** Error handling, codes and strings
** 
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:22:55 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcerror.h,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCERROR_H
#define DCERROR_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */


/*
** Enumerated Error Codes/Conditions
*/


typedef enum {
    EC_Normal = 0,
    EC_InvalidTag = 1,
    EC_TagNotFound = 2,
    EC_InvalidVR = 3,
    EC_InvalidStream = 4,
    EC_EndOfStream = 5,
    EC_CorruptedData = 6,
    EC_IllegalCall = 7,
    EC_SequEnd = 8,
    EC_DoubledTag = 9,
	EC_StreamNotifyClient = 10,
	EC_WrongStreamMode = 11,
	EC_MemoryExhausted = 12
} E_Condition;


/*
** Utility functions for Error Codes/Conditions
*/


/*
** dcmErrorConditionToString:
**
** Return a pointer to a statically allocated char array
** describing the error condition.
*/

extern const char*
dcmErrorConditionToString(E_Condition cond);


#endif /* !DCERROR_H */

/*
** CVS/RCS Log:
** $Log: dcerror.h,v $
** Revision 1.2  1996-01-05 13:22:55  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
** Revision 1.1  1995/11/23 16:37:54  hewett
** Updated for loadable data dictionary + some cleanup (more to do).
**
**
*/

