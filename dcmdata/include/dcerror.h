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
 *  Update Date:      $Date: 2001-10-12 10:42:28 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcerror.h,v $
 *  CVS/RCS Revision: $Revision: 1.11 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCERROR_H
#define DCERROR_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofcond.h"      /* for OFCondition */

/*
 *  dcmtk module numbers for modules which create their own error codes.
 *  Module numbers > 1023 are reserved for user code.
 */

const unsigned short OFM_dcmdata  =  1;
const unsigned short OFM_ctndisp  =  2;
const unsigned short OFM_dcmimgle =  3;
const unsigned short OFM_dcmjpeg  =  4;
const unsigned short OFM_dcmnet   =  5;
const unsigned short OFM_dcmprint =  6;
const unsigned short OFM_dcmpstat =  7;
const unsigned short OFM_dcmsign  =  8;
const unsigned short OFM_dcmsr    =  9;
const unsigned short OFM_dcmtls   = 10;
const unsigned short OFM_imagectn = 11;
const unsigned short OFM_wlistctn = 12;


// condition constants
extern const OFCondition EC_InvalidTag;
extern const OFCondition EC_TagNotFound;
extern const OFCondition EC_InvalidVR;
extern const OFCondition EC_InvalidStream;
extern const OFCondition EC_EndOfStream;
extern const OFCondition EC_CorruptedData;
extern const OFCondition EC_IllegalCall;
extern const OFCondition EC_SequEnd;
extern const OFCondition EC_DoubledTag;
extern const OFCondition EC_StreamNotifyClient;
extern const OFCondition EC_WrongStreamMode;
extern const OFCondition EC_ItemEnd;
extern const OFCondition EC_RepresentationNotFound;
extern const OFCondition EC_CannotChangeRepresentation;


#ifndef OFCONDITION_STRICT_MODE

// for backward compatibility with existing software
typedef OFCondition E_Condition;

/** Return a pointer to a char array describing the error condition.
 *  For backward compatibility with old software; deprecated.
 */
extern const char *dcmErrorConditionToString(OFCondition cond);

#endif


#endif /* !DCERROR_H */

/*
** CVS/RCS Log:
** $Log: dcerror.h,v $
** Revision 1.11  2001-10-12 10:42:28  meichel
** Introduced conditional define OFCONDITION_STRICT_MODE in which the
**   compatibility options related to the transition to OFCondition are disabled:
**   No OFCondition default constructor, no typedefs for E_Condition, CONDITION,
**   no macros for SUCCESS and condition aliases.
**
** Revision 1.10  2001/09/25 17:22:22  meichel
** Enabled E_Condition and dcmErrorConditionToString() for backward
**   compatibility with existing code.
**
** Revision 1.9  2001/09/25 17:19:26  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.8  2001/06/01 15:48:39  meichel
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

