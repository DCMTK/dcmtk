/*
**
** Author: Andrew Hewett	Created: 4.11.95
** Kuratorium OFFIS e.V.
**
** Module: dcerror.cc
**
** Purpose:
** Error handling, codes and strings
** 
**
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1997-07-21 08:17:41 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcerror.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdlib.h>
#include "dcerror.h"


// ********************************************

const char*
dcmErrorConditionToString(E_Condition cond) 
{
    const char* s = NULL;

    switch (cond) {
    case EC_Normal: s = "Normal"; break;
    case EC_InvalidTag:	s = "Invalid Tag"; break;
    case EC_TagNotFound: s = "Tag Not Found"; break;
    case EC_InvalidVR: s = "Invalid VR"; break;
    case EC_InvalidStream: s = "Invalid Stream"; break;
    case EC_EndOfStream: s = "End Of Stream"; break;
    case EC_CorruptedData: s = "Corrupted Data"; break;
    case EC_IllegalCall: s = "Illegal Call, perhaps wrong parameters"; break;
    case EC_SequEnd: s = "Sequence End"; break;
    case EC_DoubledTag: s = "Doubled Tag"; break;
    case EC_StreamNotifyClient: s = "Stream must be processed by client"; break;
    case EC_WrongStreamMode: s = "Mode (R/W, random/sequence) is wrong"; break;
    case EC_MemoryExhausted: s = "Virtual Memory exhausted"; break;
    case EC_ItemEnd: s = "Item End"; break;
    case EC_RepresentationNotFound: s = "Pixel representation not found"; break;
    case EC_CannotChangeRepresentation: s = "Pixel representation connot be changed"; break;
    default: s = "Unknown Error"; break;
    }

    return s;
}


/*
** CVS/RCS Log:
** $Log: dcerror.cc,v $
** Revision 1.5  1997-07-21 08:17:41  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
**
** Revision 1.4  1997/05/22 16:55:05  andreas
** - Added new error code EC_NotImplemented
**
** Revision 1.3  1996/01/29 13:38:26  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.2  1996/01/05 13:27:36  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
** Revision 1.1  1995/11/23 17:02:44  hewett
** Updated for loadable data dictionary.  Some cleanup (more to do).
**
**
*/

