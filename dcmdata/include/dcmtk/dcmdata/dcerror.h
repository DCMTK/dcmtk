/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: Error handling, codes and strings
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:41 $
 *  CVS/RCS Revision: $Revision: 1.36 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCERROR_H
#define DCERROR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcond.h"       /* for OFCondition */

/*
 *  DCMTK module numbers for modules which create their own error codes.
 *  Module numbers > 1023 are reserved for user code.
 */

const unsigned short OFM_dcmdata  =  1;
const unsigned short OFM_ctndisp  =  2;   /* retired */
const unsigned short OFM_dcmimgle =  3;
const unsigned short OFM_dcmimage =  4;
const unsigned short OFM_dcmjpeg  =  5;
const unsigned short OFM_dcmnet   =  6;
const unsigned short OFM_dcmprint =  7;
const unsigned short OFM_dcmpstat =  8;
const unsigned short OFM_dcmsign  =  9;
const unsigned short OFM_dcmsr    = 10;
const unsigned short OFM_dcmtls   = 11;
const unsigned short OFM_imagectn = 12;
const unsigned short OFM_wlistctn = 13;   /* retired */
const unsigned short OFM_dcmwlm   = 14;
const unsigned short OFM_dcmpps   = 15;
const unsigned short OFM_dcmdbsup = 16;
const unsigned short OFM_dcmppswm = 17;
const unsigned short OFM_dcmjp2k  = 18;
const unsigned short OFM_dcmjpls  = 19;
const unsigned short OFM_dcmwave  = 20;
const unsigned short OFM_dcmrt    = 21;
const unsigned short OFM_dcmloco  = 22;
const unsigned short OFM_dcmstcom = 23;
const unsigned short OFM_dcmppscu = 24;


// condition constants

/// invalid tag
extern const OFCondition EC_InvalidTag;
/// tag not found
extern const OFCondition EC_TagNotFound;
/// invalid VR
extern const OFCondition EC_InvalidVR;
/// invalid stream
extern const OFCondition EC_InvalidStream;
/// end of stream
extern const OFCondition EC_EndOfStream;
/// corrupted data
extern const OFCondition EC_CorruptedData;
/// illegal call, perhaps wrong parameters
extern const OFCondition EC_IllegalCall;
/// sequence end
extern const OFCondition EC_SequEnd;
/// doubled tag
extern const OFCondition EC_DoubledTag;
/// I/O suspension or premature end of stream
extern const OFCondition EC_StreamNotifyClient;
/// stream mode (R/W, random/sequence) is wrong
extern const OFCondition EC_WrongStreamMode;
/// item end
extern const OFCondition EC_ItemEnd;
/// compressed/uncompressed pixel representation not found
extern const OFCondition EC_RepresentationNotFound;
/// Pixel representation cannot be changed to requested transfer syntax
extern const OFCondition EC_CannotChangeRepresentation;
/// Unsupported compression or encryption
extern const OFCondition EC_UnsupportedEncoding;
/// Parser failure: Putback operation failed
extern const OFCondition EC_PutbackFailed;
/// Too many compression filters
extern const OFCondition EC_DoubleCompressionFilters;
/// Storage media application profile violated
extern const OFCondition EC_ApplicationProfileViolated;
/// Invalid offset
extern const OFCondition EC_InvalidOffset;
/// Too many bytes requested
extern const OFCondition EC_TooManyBytesRequested;
// Invalid basic offset table
extern const OFCondition EC_InvalidBasicOffsetTable;
/// Element length is larger than (explicit) length of surrounding item
extern const OFCondition EC_ElemLengthLargerThanItem;
/// File meta information header missing
extern const OFCondition EC_FileMetaInfoHeaderMissing;
/// Item or sequence content larger than explicit 32-bit length field permits
extern const OFCondition EC_SeqOrItemContentOverflow;
/// Value Representation (VR) violated
extern const OFCondition EC_ValueRepresentationViolated;
/// Value Multiplicity (VM) violated
extern const OFCondition EC_ValueMultiplicityViolated;
/// Maximum VR length violated
extern const OFCondition EC_MaximumLengthViolated;
/// Element length is larger than 16-bit length field permits
extern const OFCondition EC_ElemLengthExceeds16BitField;

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
** Revision 1.36  2010-10-14 13:15:41  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.35  2010-07-30 16:52:51  onken
** Added error code for new (non-public) module dcmppscu.
**
** Revision 1.34  2010-06-09 15:53:34  joergr
** Added number for new module "dcmstcom", used for module-specific error codes.
**
** Revision 1.33  2010-02-25 13:50:50  joergr
** Fixed issue with element values which exceed the maximum of a 16-bit length
** field.
**
** Revision 1.32  2009-08-03 09:05:29  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.31  2009-07-29 14:16:15  meichel
** Defined new constant OFM_dcmloco
**
** Revision 1.30  2009-03-19 11:46:20  joergr
** Fixed issue with Doxygen documentation.
**
** Revision 1.29  2009-03-05 13:35:47  onken
** Added checks for sequence and item lengths which prevents overflow in length
** field, if total length of contained items (or sequences) exceeds
** 32-bit length field. Also introduced new flag (default: enabled)
** for writing in explicit length mode, which allows for automatically
** switching encoding of only that very sequence/item to undefined
** length coding (thus permitting to actually write the file).
**
** Revision 1.28  2009-02-11 16:36:04  joergr
** Introduced new error code EC_FileMetaInfoHeaderMissing.
**
** Revision 1.27  2009-02-04 17:54:30  joergr
** Fixed various layout and formatting issues.
**
** Revision 1.26  2009-02-04 14:06:03  onken
** Changed parser to make use of the new error ignoring flag when parsing.
** Added check (makes use of new flag) that notes whether an element's value is
** specified larger than the surrounding item (applicable for explicit length
** coding).
**
** Revision 1.25  2009-02-04 10:17:23  joergr
** Introduced new error code EC_InvalidBasicOffsetTable.
**
** Revision 1.24  2008-12-08 14:34:54  joergr
** Added number for new module "dcmrt", used for module-specific error codes.
**
** Revision 1.23  2008-07-17 10:28:36  onken
** Added error constant for upcoming dcmwave module.
**
** Revision 1.22  2007-11-29 14:30:35  meichel
** Updated doxygen API documentation
**
** Revision 1.21  2007/06/13 14:45:47  meichel
** Added module code OFM_dcmjpls and some new error codes.
**
** Revision 1.20  2005/12/08 16:28:12  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.19  2004/08/24 14:53:15  meichel
** Added module number for future dcmjp2k module
**
** Revision 1.18  2002/12/06 12:18:27  joergr
** Added new error status "EC_ApplicationProfileViolated".
**
** Revision 1.17  2002/08/27 16:55:32  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.16  2002/08/12 09:56:59  wilkens
** Added module number (for error codes) for new module dcmppswm.
**
** Revision 1.15  2002/05/28 08:44:21  wilkens
** Added module number for new module dcmdbsup.
**
** Revision 1.14  2002/04/18 14:47:22  wilkens
** Inserted a new dcmtk module number for module mpps.
**
** Revision 1.13  2002/01/08 17:53:30  joergr
** Added new module "dcmwlm".
**
** Revision 1.12  2001/11/19 12:44:50  joergr
** Added dcmimage to list of modules.
**
** Revision 1.11  2001/10/12 10:42:28  meichel
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
