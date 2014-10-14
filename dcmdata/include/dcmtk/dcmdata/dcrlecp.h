/*
 *
 *  Copyright (C) 2002-2014, OFFIS e.V.
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
 */

#ifndef DCRLECP_H
#define DCRLECP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dccodec.h" /* for DcmCodecParameter */

/** codec parameter for RLE codec
 */
class DCMTK_DCMDATA_EXPORT DcmRLECodecParameter: public DcmCodecParameter
{
public:

  /** constructor.
   *  @param pCreateSOPInstanceUID true if a new SOP instance UID should be assigned
   *    upon compression/decompression
   *  @param pFragmentSize maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *    Please note that the DICOM standard does not allow for storing the pixel data with
   *    multiple fragments per frame (when RLE compression is used). So limiting the fragment
   *    size may result in non-standard conformant DICOM images.
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
   *  it must be overwritten in every subclass.
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
