/*
 *
 *  Copyright (C) 1994-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Error handling, codes and strings
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:48:39 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcerror.h,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
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
    EC_MemoryExhausted = 12,
    EC_ItemEnd = 13,
    EC_RepresentationNotFound = 14,
    EC_CannotChangeRepresentation = 15
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
** Revision 1.8  2001-06-01 15:48:39  meichel
** Updated copyright header
**
** Revision 1.7  2000/03/08 16:26:14  meichel
** Updated copyright header.
**
** Revision 1.6  1999/03/31 09:24:38  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.5  1997/07/21 08:15:10  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
**
** Revision 1.4  1997/05/22 16:55:02  andreas
** - Added new error code EC_NotImplemented
**
** Revision 1.3  1996/01/29 13:38:12  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.2  1996/01/05 13:22:55  andreas
** - changed to support new streaming facilities
** - more cleanups
** - merged read / write methods for block and file transfer
**
** Revision 1.1  1995/11/23 16:37:54  hewett
** Updated for loadable data dictionary + some cleanup (more to do).
**
*/

