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
 *  Update Date:      $Date: 2010-10-14 13:14:07 $
 *  CVS/RCS Revision: $Revision: 1.26 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dcerror.h"

const OFConditionConst ECC_InvalidTag(                 OFM_dcmdata,  1, OF_error, "Invalid tag"                                );
const OFConditionConst ECC_TagNotFound(                OFM_dcmdata,  2, OF_error, "Tag not found"                              );
const OFConditionConst ECC_InvalidVR(                  OFM_dcmdata,  3, OF_error, "Invalid VR"                                 );
const OFConditionConst ECC_InvalidStream(              OFM_dcmdata,  4, OF_error, "Invalid stream"                             );
const OFConditionConst ECC_EndOfStream(                OFM_dcmdata,  5, OF_error, "End of stream"                              );
const OFConditionConst ECC_CorruptedData(              OFM_dcmdata,  6, OF_error, "Corrupted data"                             );
const OFConditionConst ECC_IllegalCall(                OFM_dcmdata,  7, OF_error, "Illegal call, perhaps wrong parameters"     );
const OFConditionConst ECC_SequEnd(                    OFM_dcmdata,  8, OF_error, "Sequence end"                               );
const OFConditionConst ECC_DoubledTag(                 OFM_dcmdata,  9, OF_error, "Doubled tag"                                );
const OFConditionConst ECC_StreamNotifyClient(         OFM_dcmdata, 10, OF_error, "I/O suspension or premature end of stream"  );
const OFConditionConst ECC_WrongStreamMode(            OFM_dcmdata, 11, OF_error, "Mode (R/W, random/sequence) is wrong"       );
const OFConditionConst ECC_ItemEnd(                    OFM_dcmdata, 12, OF_error, "Item end"                                   );
const OFConditionConst ECC_RepresentationNotFound(     OFM_dcmdata, 13, OF_error, "Pixel representation not found"             );
const OFConditionConst ECC_CannotChangeRepresentation( OFM_dcmdata, 14, OF_error, "Pixel representation cannot be changed"     );
const OFConditionConst ECC_UnsupportedEncoding(        OFM_dcmdata, 15, OF_error, "Unsupported compression or encryption"      );
// error code 16 is reserved for zlib-related error messages
const OFConditionConst ECC_PutbackFailed(              OFM_dcmdata, 17, OF_error, "Parser failure: Putback operation failed"   );
// error code 18 is reserved for file read error messages
// error code 19 is reserved for file write error messages
const OFConditionConst ECC_DoubleCompressionFilters(   OFM_dcmdata, 20, OF_error, "Too many compression filters"               );
const OFConditionConst ECC_ApplicationProfileViolated( OFM_dcmdata, 21, OF_error, "Storage media application profile violated" );
// error code 22 is reserved for dcmodify error messages
const OFConditionConst ECC_InvalidOffset(              OFM_dcmdata, 23, OF_error, "Invalid offset"                             );
const OFConditionConst ECC_TooManyBytesRequested(      OFM_dcmdata, 24, OF_error, "Too many bytes requested"                   );
// error code 25 is reserved for tag path parsing error messages
const OFConditionConst ECC_InvalidBasicOffsetTable(    OFM_dcmdata, 26, OF_error, "Invalid basic offset table"                 );
const OFConditionConst ECC_ElemLengthLargerThanItem(   OFM_dcmdata, 27, OF_error, "Length of element larger than explicit length of surrounding item" );
const OFConditionConst ECC_FileMetaInfoHeaderMissing(  OFM_dcmdata, 28, OF_error, "File meta information header missing"       );
const OFConditionConst ECC_SeqOrItemContentOverflow(   OFM_dcmdata, 29, OF_error, "Item or sequence content exceeds maximum of 32-bit length field");
const OFConditionConst ECC_ValueRepresentationViolated(OFM_dcmdata, 30, OF_error, "Value Representation violated"              );
const OFConditionConst ECC_ValueMultiplicityViolated(  OFM_dcmdata, 31, OF_error, "Value Multiplicity violated"                );
const OFConditionConst ECC_MaximumLengthViolated(      OFM_dcmdata, 32, OF_error, "Maximum VR length violated"                 );
const OFConditionConst ECC_ElemLengthExceeds16BitField(OFM_dcmdata, 33, OF_error, "Length of element value exceeds maximum of 16-bit length field" );

const OFCondition EC_InvalidTag(                 ECC_InvalidTag);
const OFCondition EC_TagNotFound(                ECC_TagNotFound);
const OFCondition EC_InvalidVR(                  ECC_InvalidVR);
const OFCondition EC_InvalidStream(              ECC_InvalidStream);
const OFCondition EC_EndOfStream(                ECC_EndOfStream);
const OFCondition EC_CorruptedData(              ECC_CorruptedData);
const OFCondition EC_IllegalCall(                ECC_IllegalCall);
const OFCondition EC_SequEnd(                    ECC_SequEnd);
const OFCondition EC_DoubledTag(                 ECC_DoubledTag);
const OFCondition EC_StreamNotifyClient(         ECC_StreamNotifyClient);
const OFCondition EC_WrongStreamMode(            ECC_WrongStreamMode);
const OFCondition EC_ItemEnd(                    ECC_ItemEnd);
const OFCondition EC_RepresentationNotFound(     ECC_RepresentationNotFound);
const OFCondition EC_CannotChangeRepresentation( ECC_CannotChangeRepresentation);
const OFCondition EC_UnsupportedEncoding(        ECC_UnsupportedEncoding);
const OFCondition EC_PutbackFailed(              ECC_PutbackFailed);
const OFCondition EC_DoubleCompressionFilters(   ECC_DoubleCompressionFilters);
const OFCondition EC_ApplicationProfileViolated( ECC_ApplicationProfileViolated);
const OFCondition EC_InvalidOffset(              ECC_InvalidOffset);
const OFCondition EC_TooManyBytesRequested(      ECC_TooManyBytesRequested);
const OFCondition EC_InvalidBasicOffsetTable(    ECC_InvalidBasicOffsetTable);
const OFCondition EC_ElemLengthLargerThanItem(   ECC_ElemLengthLargerThanItem);
const OFCondition EC_FileMetaInfoHeaderMissing(  ECC_FileMetaInfoHeaderMissing);
const OFCondition EC_SeqOrItemContentOverflow(   ECC_SeqOrItemContentOverflow);
const OFCondition EC_ValueRepresentationViolated(ECC_ValueRepresentationViolated);
const OFCondition EC_ValueMultiplicityViolated(  ECC_ValueMultiplicityViolated);
const OFCondition EC_MaximumLengthViolated(      ECC_MaximumLengthViolated);
const OFCondition EC_ElemLengthExceeds16BitField(ECC_ElemLengthExceeds16BitField);


const char *dcmErrorConditionToString(OFCondition cond)
{
  return cond.text();
}


/*
** CVS/RCS Log:
** $Log: dcerror.cc,v $
** Revision 1.26  2010-10-14 13:14:07  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.25  2010-02-25 13:51:15  joergr
** Fixed issue with element values which exceed the maximum of a 16-bit length
** field.
**
** Revision 1.24  2009-08-03 09:02:59  joergr
** Added methods that check whether a given string value conforms to the VR and
** VM definitions of the DICOM standards.
**
** Revision 1.23  2009-03-05 13:35:07  onken
** Added checks for sequence and item lengths which prevents overflow in length
** field, if total length of contained items (or sequences) exceeds 32-bit
** length field. Also introduced new flag (default: enabled) for writing
** in explicit length mode, which allows for automatically switching encoding
** of only that very sequence/item to undefined length coding (thus permitting
** to actually write the file).
**
** Revision 1.22  2009-02-11 16:35:27  joergr
** Introduced new error code EC_FileMetaInfoHeaderMissing.
**
** Revision 1.21  2009-02-04 17:59:15  joergr
** Fixed various layout and formatting issues.
**
** Revision 1.20  2009-02-04 14:06:01  onken
** Changed parser to make use of the new error ignoring flag when parsing.
** Added check (makes use of new flag) that notes whether an element's value is
** specified larger than the surrounding item (applicable for explicit length
** coding).
**
** Revision 1.19  2009-02-04 10:16:51  joergr
** Introduced new error code EC_InvalidBasicOffsetTable.
**
** Revision 1.18  2008-12-05 13:51:13  onken
** Introduced new error code number for specific findOrCreatePath() errors.
**
** Revision 1.17  2007-06-13 14:45:47  meichel
** Added module code OFM_dcmjpls and some new error codes.
**
** Revision 1.16  2005/12/08 15:41:09  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.15  2004/11/05 17:20:31  onken
** Added reservation for dcmodify error messages.
**
** Revision 1.14  2002/12/06 12:18:57  joergr
** Added new error status "EC_ApplicationProfileViolated".
**
** Revision 1.13  2002/08/27 16:55:47  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.12  2001/09/25 17:19:50  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.11  2001/06/01 15:49:04  meichel
** Updated copyright header
**
** Revision 1.10  2000/03/08 16:26:35  meichel
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
