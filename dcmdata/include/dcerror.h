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
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1995-11-23 16:37:54 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcerror.h,v $
** CVS/RCS Revision:	$Revision: 1.1 $
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
    EC_EndOfFile = 5,
    EC_CorruptedData = 6,
    EC_IllegalCall = 7,
    EC_SequEnd = 8,
    EC_DoubledTag = 9,
    EC_BufferFull = 10,
    EC_EndOfBuffer = 11
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

const char*
dcmErrorConditionToString(E_Condition cond);


#endif /* !DCERROR_H */

/*
** CVS/RCS Log:
** $Log: dcerror.h,v $
** Revision 1.1  1995-11-23 16:37:54  hewett
** Updated for loadable data dictionary + some cleanup (more to do).
**
**
*/

