/*
 *
 *  Copyright (C) 1994-2000, OFFIS
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
 *  Update Date:      $Date: 2000-03-08 16:26:35 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcerror.cc,v $
 *  CVS/RCS Revision: $Revision: 1.10 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

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
** Revision 1.10  2000-03-08 16:26:35  meichel
** Updated copyright header.
**
** Revision 1.9  2000/02/23 15:11:52  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.8  2000/02/01 10:12:07  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.7  1999/03/31 09:25:27  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.6  1997/10/01 08:44:12  meichel
** Including <unistd.h> if available.
**
** Revision 1.5  1997/07/21 08:17:41  andreas
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
*/

