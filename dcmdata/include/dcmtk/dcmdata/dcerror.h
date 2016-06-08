/*
 *
 *  Copyright (C) 1994-2016, OFFIS e.V.
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
 */


#ifndef DCERROR_H
#define DCERROR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcond.h"       /* for OFCondition */
#include "dcmtk/dcmdata/dcdefine.h"

// include this file in doxygen documentation

/** @file dcerror.h
 *  @brief error handling, codes and strings for the dcmdata module
 */

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
const unsigned short OFM_dcmqrdb  = 12;
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
const unsigned short OFM_dcmstscp = 25;
const unsigned short OFM_dcmqrdbx = 26;
const unsigned short OFM_dcmkosel = 27;
const unsigned short OFM_dcmenc   = 28;
const unsigned short OFM_dcmiod   = 29;
const unsigned short OFM_dcmseg   = 30;
const unsigned short OFM_dcmfg    = 31;
const unsigned short OFM_dcmtract = 32;


// condition constants

/** @name condition constants for module dcmdata.
 *  These constants can be used in addition to the general purpose ones
 *  defined in module ofstd.
 */
//@{
/// Invalid tag
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_InvalidTag;
/// Tag not found
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_TagNotFound;
/// Invalid VR
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_InvalidVR;
/// Invalid stream
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_InvalidStream;
/// End of stream
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_EndOfStream;
/// Corrupted data
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_CorruptedData;
/// Illegal call, perhaps wrong parameters
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_IllegalCall;
/// Sequence end
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_SequEnd;
/// Doubled tag
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_DoubledTag;
/// I/O suspension or premature end of stream
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_StreamNotifyClient;
/// Stream mode (R/W, random/sequence) is wrong
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_WrongStreamMode;
/// Item end
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_ItemEnd;
/// Compressed/uncompressed pixel representation not found
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_RepresentationNotFound;
/// Pixel representation cannot be changed to requested transfer syntax
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_CannotChangeRepresentation;
/// Unsupported compression or encryption
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_UnsupportedEncoding;
/// Parser failure: Putback operation failed
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_PutbackFailed;
/// Too many compression filters
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_DoubleCompressionFilters;
/// Storage media application profile violated
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_ApplicationProfileViolated;
/// Invalid offset
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_InvalidOffset;
/// Too many bytes requested
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_TooManyBytesRequested;
/// Invalid basic offset table
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_InvalidBasicOffsetTable;
/// Element length is larger than (explicit) length of surrounding item
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_ElemLengthLargerThanItem;
/// File meta information header missing
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_FileMetaInfoHeaderMissing;
/// Item or sequence content larger than explicit 32-bit length field permits
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_SeqOrItemContentOverflow;
/// Value Representation (VR) violated
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_ValueRepresentationViolated;
/// Value Multiplicity (VM) violated
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_ValueMultiplicityViolated;
/// Maximum VR length violated
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_MaximumLengthViolated;
/// Element length is larger than 16-bit length field permits
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_ElemLengthExceeds16BitField;
/// Missing Sequence Delimitation Item while reading a sequence
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_SequDelimitationItemMissing;
/// Missing Item Delimitation Item while reading an item
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_ItemDelimitationItemMissing;
/// Sequence delimiter occurs before explicit item length is completely read
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_PrematureSequDelimitationItem;
/// Invalid DICOMDIR, e.g. corrupted data structures
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_InvalidDICOMDIR;
/// Unknown VR, e.g. tag not found in data dictionary
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_UnknownVR;
/// Invalid value, e.g. element value has been provided that does not match the VR
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_InvalidValue;
/// An expected Item was not found
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_ItemNotFound;
/// Given transfer syntax is unknown
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_UnknownTransferSyntax;
/// Cannot check
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_CannotCheck;
/// Missing value, e.g. element must contain a value but is empty
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_MissingValue;
/// Missing attribute
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_MissingAttribute;
/// Internal error
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_InternalError;
/// Invalid character, e.g. non-ASCII character used but no Specific Character Set defined
extern DCMTK_DCMDATA_EXPORT const OFConditionConst EC_InvalidCharacter;
//@}

// status code constants

/// error, cannot select specific character set
extern DCMTK_DCMDATA_EXPORT const unsigned short EC_CODE_CannotSelectCharacterSet;
/// error, cannot convert specific character set
extern DCMTK_DCMDATA_EXPORT const unsigned short EC_CODE_CannotConvertCharacterSet;
/// error, cannot convert to XML
extern DCMTK_DCMDATA_EXPORT const unsigned short EC_CODE_CannotConvertToXML;
/// error, cannot determine start fragment (of compressed pixel data)
extern DCMTK_DCMDATA_EXPORT const unsigned short EC_CODE_CannotDetermineStartFragment;


#endif /* !DCERROR_H */
