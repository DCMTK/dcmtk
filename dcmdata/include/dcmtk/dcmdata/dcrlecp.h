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
 *  Author:  Marco Eichelberg
 *
 *  Purpose: codec parameter for RLE
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:42 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCRLECP_H
#define DCRLECP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dccodec.h" /* for DcmCodecParameter */

/** codec parameter for RLE codec
 */
class DcmRLECodecParameter: public DcmCodecParameter
{
public:

  /** constructor.
   *  @param pCreateSOPInstanceUID true if a new SOP instance UID should be assigned
   *    upon compression/decompression
   *  @param pFragmentSize maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @param pCreateOffsetTable create offset table during image compression?
   *  @param pConvertToSC flag indicating whether image should be converted to
   *    Secondary Capture upon compression
   *  @param pReverseDecompressionByteOrder flag indicating whether the byte order should
   *    be reversed upon decompression. Needed to correctly decode some incorrectly encoded
   *    images with more than one byte per sample.
   */
  DcmRLECodecParameter(
    OFBool pCreateSOPInstanceUID = OFFalse,
    Uint32 pFragmentSize = 0,
    OFBool pCreateOffsetTable = OFTrue,
    OFBool pConvertToSC = OFFalse,
    OFBool pReverseDecompressionByteOrder = OFFalse);

  /// copy constructor
  DcmRLECodecParameter(const DcmRLECodecParameter& arg);

  /// destructor
  virtual ~DcmRLECodecParameter();

  /** this methods creates a copy of type DcmCodecParameter *
   *  it must be overweritten in every subclass.
   *  @return copy of this object
   */
  virtual DcmCodecParameter *clone() const;

  /** returns the class name as string.
   *  can be used as poor man's RTTI replacement.
   */
  virtual const char *className() const;

  /** returns maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @return maximum fragment size for compression
   */
  Uint32 getFragmentSize() const
  {
    return fragmentSize;
  }

  /** returns offset table creation flag
   *  @return offset table creation flag
   */
  OFBool getCreateOffsetTable() const
  {
    return createOffsetTable;
  }

  /** returns secondary capture conversion flag
   *  @return secondary capture conversion flag
   */
  OFBool getConvertToSC() const
  {
    return convertToSC;
  }

  /** returns mode for SOP Instance UID creation
   *  @return mode for SOP Instance UID creation
   */
  OFBool getUIDCreation() const
  {
    return createInstanceUID;
  }

  /** returns reverse decompression byte order mode
   *  @return reverse decompression byte order mode
   */
  OFBool getReverseDecompressionByteOrder() const
  {
    return reverseDecompressionByteOrder;
  }


private:

  /// private undefined copy assignment operator
  DcmRLECodecParameter& operator=(const DcmRLECodecParameter&);

  /// maximum fragment size (in kbytes) for compression, 0 for unlimited.
  Uint32 fragmentSize;

  /// create offset table during image compression
  OFBool createOffsetTable;

  /// flag indicating whether image should be converted to Secondary Capture upon compression
  OFBool convertToSC;

  /// create new Instance UID during compression/decompression?
  OFBool createInstanceUID;

  /** enable reverse byte order of RLE segments during decompression, needed to
   *  decompress certain incorrectly encoded RLE images
   */
  OFBool reverseDecompressionByteOrder;
};


#endif

/*
 * CVS/RCS Log
 * $Log: dcrlecp.h,v $
 * Revision 1.6  2010-10-14 13:15:42  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.5  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.4  2008-06-23 12:09:13  joergr
 * Fixed inconsistencies in Doxygen API documentation.
 *
 * Revision 1.3  2005/12/08 16:28:35  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2005/07/26 17:08:33  meichel
 * Added option to RLE decoder that allows to correctly decode images with
 *   incorrect byte order of byte segments (LSB instead of MSB).
 *
 * Revision 1.1  2002/06/06 14:52:35  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
